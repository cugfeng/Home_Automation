/*
 * =====================================================================================
 *
 *       Filename:  temp_debug.c
 *
 *    Description:  Control debug information print out.
 *
 *        Version:  1.0
 *        Created:  07/12/2014 10:45:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>

#include "temp_util.h"
#include "temp_debug.h"

static int temp_debug_level = TEMP_DEFAULT_DEBUG_LEVEL;
static char *str_level[] = {"N", "F", "E", "D", "I", "V"};
static FILE *fp_out = NULL;

int TEMP_get_debug_level(void)
{
    return temp_debug_level;
}

void TEMP_set_debug_level(int level)
{
    if (temp_debug_level != level) {
        TEMP_LOGI("Set debug level as (LOG%s)\n", str_level[level]);
        temp_debug_level = level;
    }
}

int TEMP_update_debug_level(void)
{
    FILE *fp;
    const char *filepath = TEMP_SETTING_LEVEL;
    int ret, level;

    if (access(filepath, F_OK) < 0) {
        TEMP_LOGV("File (%s) does not exist!\n", filepath);
        return 0;
    }

    fp = fopen(filepath, "r");
    if (fp == NULL) {
        TEMP_LOGE("Open file (%s) failed!\n", filepath);
        return -1;
    }

    ret = fscanf(fp, "%d", &level);
    if (ret == 1) {
        if (LOGN <= level && level <= LOGV) {
            TEMP_set_debug_level(level);
            ret = 0;
        } else {
            TEMP_LOGE("Invalid debug level (%d)!\n", level);
            remove(filepath);
            ret = -1;
        }
    } else {
        TEMP_LOGE("Read debug level failed!\n");
        ret = -1;
    }

    fclose(fp);
    return ret;
}

void TEMP_set_fp_out(FILE *fp)
{
    assert(fp != NULL);

    fp_out = fp;
}

void TEMP_LOG(int debug_level, const char *file, int line, const char *format, ...)
{
    if (fp_out == NULL) {
        fp_out = stdout;
    }

    if (debug_level <= temp_debug_level) {
        char buf[LOG_BUFFER_SIZE] = {0};

        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

        fprintf(fp_out, "[%s][%s][%s][%d]: %s",
                str_level[debug_level], LOG_TAG, file, line, buf);
    }
}
