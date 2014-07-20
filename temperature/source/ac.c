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
#include "ac.h"

void TEMP_ac_turn_on(void)
{
    system("irsend SEND_ONCE AC ON_27");
}

void TEMP_ac_turn_off(void)
{
    system("irsend SEND_ONCE AC OFF");
}
