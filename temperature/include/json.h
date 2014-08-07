/*
 * =====================================================================================
 *
 *       Filename:  json.h
 *
 *    Description:  JSON parser APIs
 *
 *        Version:  1.0
 *        Created:  08/07/14 21:13:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */

extern int json_parse(const char *filepath);
extern int json_get_automode(void);
extern int json_get_target_temp(void);
extern int json_get_tolerance_temp(void);
