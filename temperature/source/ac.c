/*
 * =====================================================================================
 *
 *       Filename:  ac.c
 *
 *    Description:  Air Conditioner control APIs
 *
 *        Version:  1.0
 *        Created:  07/20/2014 09:42:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "temp_debug.h"
#include "ac.h"

int TEMP_ac_turn_on(void)
{
    int ret;

    ret = system("irsend SEND_ONCE AC ON_27");
    if (ret < 0) {
        TEMP_LOGE("Turn on AC failed!\n");
    }

    return ret;
}

int TEMP_ac_turn_off(void)
{
    int ret;

    ret = system("irsend SEND_ONCE AC OFF");
    if (ret < 0) {
        TEMP_LOGE("Turn off AC failed!\n");
    }

    return ret;
}
