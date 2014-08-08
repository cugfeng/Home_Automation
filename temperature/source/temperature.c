/*
 * =====================================================================================
 *
 *       Filename:  temperature.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/12/2014 16:09:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "temp_debug.h"
#include "temp_util.h"
#include "ds18b20.h"
#include "ac.h"
#include "json.h"

#define DS18B20_ADDRESS "28-00042d9aa8ff"
#define TEMP_BUFFER_SIZE (6)

static int g_process_alive;

typedef struct {
    char date[12]; /* yyyy-mm-dd */
    char time[8];  /* hh-mm */
    int  temperature;
} TimeTemp;

typedef struct {
    int is_record;
    char record[TEMP_PATH_MAX_LEN];
} TempArgs;

static int fill_date_time(TimeTemp *time_temp)
{
    time_t current_time;
    struct tm *time_formated;

    assert(time_temp != NULL);

    current_time = time(NULL);
    time_formated = localtime(&current_time);
    strftime(time_temp->date, sizeof(time_temp->date), "%F", time_formated);
    strftime(time_temp->time, sizeof(time_temp->time), "%R", time_formated);

    return 0;
}

static int save_buffer_to_file(TimeTemp *buffer, int size, const char *path)
{
    int i;

    assert(buffer != NULL);
    assert(path != NULL);
    if (access(path, F_OK) < 0) {
        TEMP_LOGE("Directory (%s) does not exist!\n", path);
        return -1;
    }

    for (i = 0; i < size; ++i) {
        FILE *fp;
        char filepath[32] = {0};

        strcpy(filepath, path);
        strcat(filepath, "/");
        strcat(filepath, buffer[i].date);
        strcat(filepath, ".txt");
        fp = fopen(filepath, "a+");
        if (fp == NULL) {
            TEMP_LOGE("Failed to open file (%s) for writing!\n", filepath);
            continue;
        }

        fprintf(fp, "%s %d\n", buffer[i].time, buffer[i].temperature);
        fclose(fp);
    }

    return 0;
}

static int save_temp_to_file(int temp)
{
    FILE *fp;
    const char *filepath = TEMP_SETTING_CURRENT;

    fp = fopen(filepath, "w");
    if (fp == NULL) {
        TEMP_LOGE("Failed to open file (%s) for writing!\n", filepath);
        return -1;
    }

    fprintf(fp, "%d\n", temp);
    fclose(fp);

    return 0;
}

void *temp_monitor_task(void *args)
{
    TimeTemp buffer[TEMP_BUFFER_SIZE] = {0};
    int index = 0;

    TempArgs *temp_args = (TempArgs *)args;

    assert(args != NULL);

    while (g_process_alive) {
        int temp = TEMP_ds18b20_read(DS18B20_ADDRESS);
        TEMP_LOGI("Current temperature is %.3f degree\n", temp / 1000.0);
        save_temp_to_file(temp);

        fill_date_time(&(buffer[index]));
        buffer[index].temperature = temp;

        ++index;
        if (index >= TEMP_BUFFER_SIZE) {
            if (temp_args->is_record) {
                save_buffer_to_file(buffer, TEMP_BUFFER_SIZE, temp_args->record);
            }
            memset(buffer, sizeof(buffer), 0);
            index = 0;
        }

        sleep(TEMP_MONITOR_TASK_SLEEP);
    }
    pthread_exit(NULL);

    return NULL;
}

static int get_current_temp(void)
{
    FILE *fp;
    const char *filepath = TEMP_SETTING_CURRENT;
    int ret, current;

    if (access(filepath, F_OK) < 0) {
        TEMP_LOGD("File (%s) does not exist!\n", filepath);
        return -1;
    }

    fp = fopen(filepath, "r");
    if (fp == NULL) {
        TEMP_LOGE("Open file (%s) failed!\n", filepath);
        return -1;
    }

    ret = fscanf(fp, "%d", &current);
    if (ret != 1) {
        TEMP_LOGE("Read current temperature failed!\n");
        current = -1;
    }

    fclose(fp);
    return current;
}

void *temp_setting_task(void *args)
{
    while (g_process_alive) {
        int current, target, tolerance;
        int automode;
        int ret;

        sleep(TEMP_SETTING_TASK_SLEEP);

        ret = json_parse(TEMP_SETTING_JSON);
        if (ret < 0) {
            TEMP_LOGD("Parse %s failed!\n", TEMP_SETTING_JSON);
            continue;
        }

        current   = get_current_temp();
        target    = json_get_target_temp();
        tolerance = json_get_tolerance_temp();
        if (current < 0 || target < 0 || tolerance < 0) {
            TEMP_LOGD("At least one of current (%d), target (%d) and tolerance (%d)"
                    " temperature is invalid!\n", current, target, tolerance);
            continue;
        }
        TEMP_LOGI("Current (%d) target (%d) tolerance (%d)\n",
                current, target, tolerance);

        automode  = json_get_automode();
        if (automode == 1) {
            TEMP_LOGD("Automode is enabled.\n");
        } else if (automode == 0) {
            TEMP_LOGD("Automode is disabled.\n");
            continue;
        } else {
            TEMP_LOGD("Automode setting is invalid.\n");
            continue;
        }

        if (current > (target + tolerance)) {
            TEMP_LOGD("Temperature is too high, turn on ac\n");
            TEMP_ac_turn_on();
        } else if (current < (target - tolerance)) {
            TEMP_LOGD("Temperature is too low, turn off ac\n");
            TEMP_ac_turn_off();
        }
    }
    pthread_exit(NULL);

    return NULL;
}

static void usage(void)
{
    printf("Usage:\n");
    printf("    temperature [options]\n\n");
    printf("Options:\n");
    printf("    -r  --record path   Record temperature into path/yyyy-mm-dd.txt\n\n");

    exit(0);
}

int TEMP_main(int argc, char *argv[])
{
    int ret;
    pthread_t thread_id_monitor, thread_id_setting;

    int ch;
    TempArgs temp_args;
    struct option longopts[] = {
        {"record",  required_argument,  NULL, 'r'},
        {NULL,      0,                  NULL, 0}
    };

    memset(&temp_args, 0, sizeof(temp_args));
    while ((ch = getopt_long(argc, argv, "r:", longopts, NULL)) != -1) {
        switch (ch) {
            case 'r':
                temp_args.is_record = 1;
                strncpy(temp_args.record, optarg, sizeof(temp_args.record));
                break;

            default:
                usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (access(TEMP_SETTING_DIR, F_OK) < 0) {
        mkdir(TEMP_SETTING_DIR, 0755);
        chmod(TEMP_SETTING_DIR, 0777);
    }

    g_process_alive = 1;
    ret = pthread_create(&thread_id_monitor, NULL, temp_monitor_task, &temp_args);
    if (ret < 0) {
        TEMP_LOGE("Create thread monitor failed!\n");
        ret = -1;
        goto join_exit_0;
    }
    ret = pthread_create(&thread_id_setting, NULL, temp_setting_task, NULL);
    if (ret < 0) {
        TEMP_LOGE("Create thread setting failed!\n");
        ret = -1;
        goto join_exit_1;
    }

    while (1) {
        if (access(TEMP_SETTING_EXIT, F_OK) == 0) {
            remove(TEMP_SETTING_EXIT);
            g_process_alive = 0;
            TEMP_LOGD("Process alive is 0, exit...\n");
            break;
        }

        sleep(TEMP_MAIN_TASK_SLEEP);
    }

join_exit_2:
    pthread_join(thread_id_setting, NULL);
join_exit_1:
    pthread_join(thread_id_monitor, NULL);
join_exit_0:
    return ret;
}
