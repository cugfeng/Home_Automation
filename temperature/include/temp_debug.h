/*
 * =====================================================================================
 *
 *       Filename:  temp_debug.h
 *
 *    Description:  Control debug information print out.
 *
 *        Version:  1.0
 *        Created:  07/12/2014 10:18:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#define LOG_TAG "TEMP"
#define LOG_BUFFER_SIZE (1024)

#define LOGN (0) /* log level none */
#define LOGE (1) /* log level error */
#define LOGD (2) /* log level debug */
#define LOGV (3) /* log level verbose */

#define TEMP_DEFAULT_DEBUG_LEVEL (LOGE)

#define TEMP_LOGE(format, args...) TEMP_LOG(LOGE, __FILE__, __LINE__, format, ##args)
#define TEMP_LOGD(format, args...) TEMP_LOG(LOGD, __FILE__, __LINE__, format, ##args)

extern int  TEMP_get_debug_level(void);
extern void TEMP_set_debug_level(int level);
extern void TEMP_LOG(int debug_level, const char *file, int line, const char *format, ...);

