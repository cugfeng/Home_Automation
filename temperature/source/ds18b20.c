/*
 * =====================================================================================
 *
 *       Filename:  ds18b20.c
 *
 *    Description:  This file implements temperature sensor ds18b20 related APIs.
 *
 *        Version:  1.0
 *        Created:  07/12/2014 09:13:15
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "temp_debug.h"
#include "temp_util.h"
#include "ds18b20.h"

#define ADDR_MAX_LEN (16)
#define BUFFER_SIZE  (512)

#define CRC_YES (1)
#define CRC_NO  (0)

static int ds18b20_verify_crc(const char *buf)
{
    assert(buf != NULL);

    int len = strlen(buf);
    const char *crc = &(buf[len - 1]);

    if (*crc == '\n') {
        --crc;
    }
    while (isspace(*crc) == 0) {
        --crc;
    }
    ++crc;

    if (strncmp(crc, "YES", 3) == 0) {
        TEMP_LOGI("CRC verify success!\n");
        return CRC_YES;
    } else if (strncmp(crc, "NO", 2) == 0) {
        TEMP_LOGD("CRC verify failed!\n");
        return CRC_NO;
    } else {
        TEMP_LOGE("Unexpected CRC verify result (%s)!\n", crc);
        return CRC_NO;
    }
}

static int ds18b20_decode_temp(const char *buf)
{
    assert(buf != NULL);

    int len = strlen(buf);
    const char *p_temp = &(buf[len - 1]);
    int i_temp = -1;
    int found = 0;

    if (*p_temp == '\n') {
        --p_temp;
    }
    --p_temp;
    while (p_temp >= buf) {
        if (p_temp[0] == 't' && p_temp[1] == '=') {
            p_temp += 2;
            found = 1;
            break;
        }
        --p_temp;
    }

    if (found == 1) {
        i_temp = atoi(p_temp);
        TEMP_LOGI("Temperature is %d\n", i_temp);
    } else {
        TEMP_LOGE("Cannot find temperature in buffer (%s)!\n", buf);
        i_temp = -1;
    }
    
    return i_temp;
}

int TEMP_ds18b20_read(const char *address)
{
    char path[TEMP_PATH_MAX_LEN] = {0};
    char buf[BUFFER_SIZE]   = {0};
    char *p_ret;
    FILE *fp;

    int i_ret;

    assert(strlen(address) <= 16);

    strcpy(path, "/sys/bus/w1/devices/");
    strcat(path, address);
    strcat(path, "/w1_slave");

    if (access(path, F_OK) < 0) {
        TEMP_LOGE("Address (%s) does not exist!\n", address);
        return -1;
    }

    fp = fopen(path, "r");
    if (fp == NULL) {
        TEMP_LOGE("Open file (%s) failed!\n", path);
        return -1;
    }

    memset(buf, sizeof(buf), 0);
    p_ret = fgets(buf, sizeof(buf), fp);
    if (p_ret == NULL) {
        TEMP_LOGE("Read from file (%s) failed!\n", path);
        i_ret = -1;
        goto close_exit;
    }

    if (ds18b20_verify_crc(buf) == CRC_NO) {
        i_ret = -1;
        goto close_exit;
    }

    memset(buf, sizeof(buf), 0);
    p_ret = fgets(buf, sizeof(buf), fp);
    if (p_ret == NULL) {
        TEMP_LOGE("Read from file (%s) failed!\n", path);
        i_ret = -1;
        goto close_exit;
    }

    i_ret = ds18b20_decode_temp(buf);
    if (i_ret < 0) {
        TEMP_LOGE("Decode temperature from buffer (%s) failed!\n", buf);
        i_ret = -1;
        goto close_exit;
    }
    
close_exit:
    fclose(fp);
    return i_ret;
}
