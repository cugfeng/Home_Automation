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

#define DS18B20_ADDRESS "28-00042d9aa8ff"
#define TEMP_BUFFER_SIZE (6)

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
    const char *filepath = "/tmp/current_temperature";

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
    int alive = 1;
    TimeTemp buffer[TEMP_BUFFER_SIZE] = {0};
    int index = 0;

    TempArgs *temp_args = (TempArgs *)args;

    assert(args != NULL);

    while (alive) {
        int temp = TEMP_ds18b20_read(DS18B20_ADDRESS);
        printf("Current temperature is %.3f degree\n", temp / 1000.0);
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

        sleep(60);
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
    pthread_t thread_id;

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

    ret = pthread_create(&thread_id, NULL, temp_monitor_task, &temp_args);
    if (ret < 0) {
        TEMP_LOGE("Create thread failed!\n");
        return -1;
    } else {
        pthread_join(thread_id, NULL);
    }

    return 0;
}

