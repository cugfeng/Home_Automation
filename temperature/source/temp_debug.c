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

#include "temp_debug.h"

static int temp_debug_level = TEMP_DEFAULT_DEBUG_LEVEL;
static char *str_level[] = {"F", "E", "D", "I", "V"};
static FILE *fp_out = NULL;

int TEMP_get_debug_level(void)
{
    return temp_debug_level;
}

void TEMP_set_debug_level(int level)
{
    temp_debug_level = level;
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
