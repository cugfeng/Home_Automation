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
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "temp_debug.h"
#include "ds18b20.h"

#define DS18B20_ADDRESS "28-00042d9aa8ff"
#define TEMP_BUFFER_SIZE (6)

typedef struct {
    char date[12]; /* yyyy-mm-dd */
    char time[8];  /* hh-mm */
    int  temperature;
} TimeTemp;

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

static int save_buffer_to_file(TimeTemp *buffer, int size)
{
    int i;

    assert(buffer != NULL);

    for (i = 0; i < size; ++i) {
        FILE *fp;
        char filepath[32] = {0};

        strcpy(filepath, "record/");
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

void *temp_monitor_task(void *args)
{
    int alive = 1;
    TimeTemp buffer[TEMP_BUFFER_SIZE] = {0};
    int index = 0;

    while (alive) {
        int temp = TEMP_ds18b20_read(DS18B20_ADDRESS);
        printf("Current temperature is %.3f degree\n", temp / 1000.0);

        fill_date_time(&(buffer[index]));
        buffer[index].temperature = temp;

        ++index;
        if (index >= TEMP_BUFFER_SIZE) {
            save_buffer_to_file(buffer, TEMP_BUFFER_SIZE);
            memset(buffer, sizeof(buffer), 0);
            index = 0;
        }

        sleep(60);
    }
    pthread_exit(NULL);

    return NULL;
}

int TEMP_main(int argc, char *argv[])
{
    int ret;
    pthread_t thread_id;

    ret = pthread_create(&thread_id, NULL, temp_monitor_task, NULL);
    if (ret < 0) {
        TEMP_LOGE("Create thread failed!\n");
        return -1;
    }

    pthread_join(thread_id, NULL);

    return 0;
}

