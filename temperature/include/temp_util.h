/*
 * =====================================================================================
 *
 *       Filename:  temp_util.h
 *
 *    Description:  Utilites for temperature module
 *
 *        Version:  1.0
 *        Created:  07/20/2014 07:04:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */

#define TEMP_PATH_MAX_LEN (128)

#define TEMP_SETTING_DIR     "/tmp/temperature"
#define TEMP_SETTING_CURRENT "/tmp/temperature/current"
#define TEMP_SETTING_JSON    "/tmp/temperature/setting.json"
#define TEMP_SETTING_LEVEL   "/tmp/temperature/debug_level"
#define TEMP_SETTING_EXIT    "/tmp/temperature/exit"

#define TEMP_MONITOR_TASK_SLEEP (30)    /* second */
#define TEMP_SETTING_TASK_SLEEP (60)    /* second */
#define TEMP_MAIN_TASK_SLEEP    (03)    /* second */
#define TEMP_SETTING_COUNT_MAX  (10)
