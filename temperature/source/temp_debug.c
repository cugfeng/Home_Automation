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
 *         Author:  Shawn Sze
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdarg.h>

#include "temp_debug.h"

static int temp_debug_level = TEMP_DEFAULT_DEBUG_LEVEL;
static char *str_level[] = {"N", "E", "D", "V"};

int TEMP_get_debug_level(void)
{
    return temp_debug_level;
}

void TEMP_set_debug_level(int level)
{
    temp_debug_level = level;
}

void TEMP_LOG(int debug_level, const char *file, int line, const char *format, ...)
{
    if (debug_level <= temp_debug_level) {
        char buf[LOG_BUFFER_SIZE] = {0};

        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

        printf("[%s][%s][%s][%d]: %s", str_level[debug_level], LOG_TAG, file, line, buf);
    }
}
