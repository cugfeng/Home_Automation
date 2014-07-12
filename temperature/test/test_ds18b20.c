/*
 * =====================================================================================
 *
 *       Filename:  test_ds18b20.c
 *
 *    Description:  Test ds18b20 related APIs
 *
 *        Version:  1.0
 *        Created:  07/12/2014 10:47:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>

#include "temp_debug.h"
#include "ds18b20.h"

int main(int argc, char *argv[])
{
    int temp = 0;

    TEMP_set_debug_level(LOGV);
    temp = TEMP_ds18b20_read("28-00042d9aa8ff");

    printf("Current temperature: %.3f degree\n", temp / 1000.0);

    return 0;
}
