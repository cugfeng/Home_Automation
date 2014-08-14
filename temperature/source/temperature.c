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

static const char *AM = "AM";
static const char *PM = "PM";

static int g_process_alive;
static int g_current_temp = -1;
static pthread_mutex_t g_mutex;
static pthread_cond_t  g_cond;
static pthread_mutex_t g_mutex_time;

typedef struct {
    char date[12]; /* yyyy-mm-dd */
    char time[8];  /* hh-mm */
    int  temperature;
} TimeTemp;

typedef struct {
    int is_record;
    char record[TEMP_PATH_MAX_LEN];
    int is_file;
    char file[TEMP_PATH_MAX_LEN];
} TempArgs;

static int fill_date_time(TimeTemp *time_temp)
{
    time_t current_time;
    struct tm *time_formated;

    assert(time_temp != NULL);

    current_time = time(NULL);
    TEMP_LOGV("Thread monitor lock g_mutex_time\n");
    pthread_mutex_lock(&g_mutex_time);
    time_formated = localtime(&current_time);
    strftime(time_temp->date, sizeof(time_temp->date), "%F", time_formated);
    strftime(time_temp->time, sizeof(time_temp->time), "%R", time_formated);
    pthread_mutex_unlock(&g_mutex_time);
    TEMP_LOGV("Thread monitor unlock g_mutex_time\n");

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
        char filepath[TEMP_PATH_MAX_LEN] = {0};

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
    struct timespec sleep_time = {
        .tv_sec  = 0,
        .tv_nsec = 0
    };

    TempArgs *temp_args = (TempArgs *)args;

    assert(args != NULL);

    while (g_process_alive) {
        int temp = TEMP_ds18b20_read(DS18B20_ADDRESS);
        TEMP_LOGD("Current temperature is %.3f degree\n", temp / 1000.0);
        g_current_temp = temp;
        save_temp_to_file(temp);

        fill_date_time(&(buffer[index]));
        buffer[index].temperature = temp;

        ++index;
        if (index >= TEMP_BUFFER_SIZE) {
            if (temp_args->is_record) {
                save_buffer_to_file(buffer, TEMP_BUFFER_SIZE, temp_args->record);
            }
            memset(buffer, 0, sizeof(buffer));
            index = 0;
        }

        sleep_time.tv_sec = TEMP_MONITOR_TASK_SLEEP + time(NULL);
        pthread_mutex_lock(&g_mutex);
        if (g_process_alive) {
            TEMP_LOGV("Thread monitor timed wait...enter\n");
            pthread_cond_timedwait(&g_cond, &g_mutex, &sleep_time);
            TEMP_LOGV("Thread monitor timed wait...leave\n");
        }
        pthread_mutex_unlock(&g_mutex);
    }
    TEMP_LOGI("Thread monitor has exited!\n");
    pthread_exit(NULL);

    return NULL;
}

static int hour_12_to_24(int hour, const char *ampm)
{
    assert(1 <= hour && hour <= 12);

    if (strcmp(ampm, AM) == 0) {
        if (hour <= 11) {
            return hour; /* 01:00 ~ 11:00 */
        } else {
            return 0; /* 00:00 */
        }
    } else if (strcmp(ampm, PM) == 0) {
        if (hour <= 11) {
            return hour + 12; /* 13:00 ~ 23:00 */
        } else {
            return 12; /* 12:00 */
        }
    } else {
        assert(0);
        return -1;
    }
}

static int hour_24_to_12(int hour)
{
    assert(0 <= hour && hour <= 23);

    if (hour == 0) {
        return 12; /* 12:00 AM */
    } else if (hour <= 11) {
        return hour; /* 01:00 AM ~ 11:00 AM */
    } else if (hour == 12 ) {
        return 12; /* 12:00 PM */
    } else {
        return hour - 12; /* 01:00 PM ~ 11:00 PM */
    }
}

static const char *hour_24_to_ampm(int hour)
{
    assert(0 <= hour && hour <= 23);

    return ((hour <= 11) ? AM : PM);
}

static inline int hour_minute_to_minute(int hour, int minute)
{
    return (hour * 60 + minute);
}

int current_time_in_range(int from_hour, int from_minute, const char *from_ampm,
        int to_hour, int to_minute, const char *to_ampm)
{
    time_t current_time;
    struct tm *time_formated;
    int from_hour_24, to_hour_24;
    int current_total_minute, from_total_minute, to_total_minute;
    int in_range = 0;

    current_time = time(NULL);
    TEMP_LOGV("Thread setting lock g_mutex_time\n");
    pthread_mutex_lock(&g_mutex_time);
    time_formated = localtime(&current_time);
    TEMP_LOGD("Current time is (%02d:%02d %s)\n",
            hour_24_to_12(time_formated->tm_hour),
            time_formated->tm_min,
            hour_24_to_ampm(time_formated->tm_hour));
    current_total_minute = hour_minute_to_minute(
            time_formated->tm_hour, time_formated->tm_min);
    pthread_mutex_unlock(&g_mutex_time);
    TEMP_LOGV("Thread setting lock g_mutex_time\n");

    from_hour_24 = hour_12_to_24(from_hour, from_ampm);
    from_total_minute = hour_minute_to_minute(from_hour_24, from_minute);

    to_hour_24 = hour_12_to_24(to_hour, to_ampm);
    to_total_minute = hour_minute_to_minute(to_hour_24, to_minute);

    if (from_total_minute <= to_total_minute) {
        if (from_total_minute <= current_total_minute
                && current_total_minute <= to_total_minute) {
            in_range = 1;
        }
    } else {
        if ((from_total_minute <= current_total_minute)
                && (current_total_minute <= 24 * 60)) {
            in_range = 1;
        } else if ((0 <= current_total_minute)
                && (current_total_minute <= to_total_minute)) {
            in_range = 1;
        }
    }

    if (in_range) {
        TEMP_LOGD("Current time is in setting time range\n");
        return 1;
    } else {
        TEMP_LOGD("Current time is not in setting time range\n");
        return 0;
    }
}

void *temp_setting_task(void *args)
{
    while (g_process_alive) {
        int current;
        int automode, target, tolerance;
        int from_hour, from_minute;
        int to_hour, to_minute;
        const char *from_ampm;
        const char *to_ampm;
        struct timespec sleep_time = {
            .tv_sec  = 0,
            .tv_nsec = 0
        };
        int ret;

        sleep_time.tv_sec = TEMP_SETTING_TASK_SLEEP + time(NULL);
        pthread_mutex_lock(&g_mutex);
        if (g_process_alive) {
            TEMP_LOGV("Thread setting timed wait...enter\n");
            pthread_cond_timedwait(&g_cond, &g_mutex, &sleep_time);
            TEMP_LOGV("Thread setting timed wait...leave\n");
        }
        pthread_mutex_unlock(&g_mutex);
        if (g_process_alive == 0) {
            break;
        }

        current = g_current_temp;
        if (current < 0) {
            TEMP_LOGE("Current temperature (%d) is invalid!\n", current);
            continue;
        }

        ret = json_parse(TEMP_SETTING_JSON);
        if (ret < 0) {
            TEMP_LOGE("Parse %s failed!\n", TEMP_SETTING_JSON);
            continue;
        }

        automode  = json_get_automode();
        if (automode == 1) {
            TEMP_LOGD("Automode is enabled.\n");
        } else if (automode == 0) {
            TEMP_LOGD("Automode is disabled.\n");
            continue;
        } else {
            TEMP_LOGE("Automode setting is invalid!\n");
            continue;
        }

        from_hour   = json_get_from_hour();
        from_minute = json_get_from_minute();
        from_ampm   = json_get_from_ampm();
        to_hour     = json_get_to_hour();
        to_minute   = json_get_to_minute();
        to_ampm     = json_get_to_ampm();
        TEMP_LOGD("Time range from (%02d:%02d %s) to (%02d:%02d %s)\n",
                from_hour, from_minute, from_ampm,
                to_hour, to_minute, to_ampm);
        if (current_time_in_range(from_hour, from_minute, from_ampm,
                    to_hour, to_minute, to_ampm) == 1) {
            target    = json_get_target_temp();
            tolerance = json_get_tolerance_temp();
            TEMP_LOGD("Temperature target (%d) tolerance (%d)\n", target, tolerance);
            if (current > (target + tolerance)) {
                TEMP_LOGD("Temperature is too high, turn on ac\n");
                TEMP_ac_turn_on();
            } else if (current < (target - tolerance)) {
                TEMP_LOGD("Temperature is too low, turn off ac\n");
                TEMP_ac_turn_off();
            }
        }
    }
    TEMP_LOGI("Thread setting has exited!\n");
    pthread_exit(NULL);

    return NULL;
}

static void usage(void)
{
    printf("Usage:\n");
    printf("    temperature [options]\n\n");
    printf("Options:\n");
    printf("    -r  --record path   Record temperature into path/yyyy-mm-dd.txt\n");
    printf("    -f  --file path     Write log to file instead of stdout\n");
    printf("\n");

    exit(0);
}

int TEMP_main(int argc, char *argv[])
{
    int ret;
    pthread_t thread_id_monitor, thread_id_setting;

    int ch;
    TempArgs temp_args;
    FILE *fp_out;
    struct option longopts[] = {
        {"record",  required_argument,  NULL, 'r'},
        {"file",    required_argument,  NULL, 'f'},
        {NULL,      0,                  NULL, 0}
    };

    memset(&temp_args, 0, sizeof(temp_args));
    while ((ch = getopt_long(argc, argv, "r:f:", longopts, NULL)) != -1) {
        switch (ch) {
            case 'r':
                temp_args.is_record = 1;
                strncpy(temp_args.record, optarg, sizeof(temp_args.record));
                break;

            case 'f':
                temp_args.is_file = 1;
                strncpy(temp_args.file, optarg, sizeof(temp_args.file));
                break;

            default:
                usage();
        }
    }
    argc -= optind;
    argv += optind;

    if (temp_args.is_file) {
        fp_out = fopen(temp_args.file, "w");
        if (fp_out == NULL) {
            printf("Failed to open file (%s) for writting!\n", temp_args.file);
            return -1;
        }
        TEMP_set_fp_out(fp_out);
    }

    if (access(TEMP_SETTING_DIR, F_OK) < 0) {
        mkdir(TEMP_SETTING_DIR, 0755);
        chmod(TEMP_SETTING_DIR, 0777);
    }

    TEMP_update_debug_level();
    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);
    pthread_mutex_init(&g_mutex_time, NULL);

    g_process_alive = 1;
    ret = pthread_create(&thread_id_monitor, NULL, temp_monitor_task, &temp_args);
    if (ret < 0) {
        TEMP_LOGF("Create thread monitor failed!\n");
        ret = -1;
        goto join_exit_0;
    }
    ret = pthread_create(&thread_id_setting, NULL, temp_setting_task, NULL);
    if (ret < 0) {
        TEMP_LOGF("Create thread setting failed!\n");
        ret = -1;
        goto join_exit_1;
    }

    while (1) {
        if (access(TEMP_SETTING_EXIT, F_OK) == 0) {
            remove(TEMP_SETTING_EXIT);
            pthread_mutex_lock(&g_mutex);
            g_process_alive = 0;
            TEMP_LOGI("Process alive has been changed to 0, exiting...\n");
            pthread_cond_broadcast(&g_cond);
            pthread_mutex_unlock(&g_mutex);
            break;
        }

        TEMP_update_debug_level();
        sleep(TEMP_MAIN_TASK_SLEEP);
    }

join_exit_2:
    pthread_join(thread_id_setting, NULL);
join_exit_1:
    pthread_join(thread_id_monitor, NULL);
join_exit_0:
    pthread_mutex_destroy(&g_mutex_time);
    pthread_cond_destroy(&g_cond);
    pthread_mutex_destroy(&g_mutex);
    if (temp_args.is_file) {
        fclose(fp_out);
    }
    return ret;
}
