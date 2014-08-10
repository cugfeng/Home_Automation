/*
 * =====================================================================================
 *
 *       Filename:  json.c
 *
 *    Description:  JSON parser
 *
 *        Version:  1.0
 *        Created:  08/07/14 21:15:42
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Shawn Sze, cugfeng@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "jsmn.h"
#include "json.h"
#include "temp_debug.h"

#define MAX_JSON_TOKENS (32)

enum {
    AUTOMODE = 0,
    FROM_HOUR,
    FROM_MINUTE,
    FROM_AMPM,
    TO_HOUR,
    TO_MINUTE,
    TO_AMPM,
    TARGET,
    TOLERANCE,
    ID_MAX
};
static int setting[ID_MAX];
const char *keys[ID_MAX] = {
    "automode",
    "from_hour",
    "from_minute",
    "from_ampm",
    "to_hour",
    "to_minute",
    "to_ampm",
    "target",
    "tolerance"
};
const char *ampm[2] = {
    "AM",
    "PM"
};

int json_read_file(const char *filepath, char **json)
{
    FILE *fp;
    char *buffer;
    int size, count, ret;

    if (access(filepath, F_OK) < 0) {
        TEMP_LOGE("File (%s) does not exist!\n", filepath);
        return -1;
    }

    fp = fopen(filepath, "r");
    if (fp == NULL) {
        TEMP_LOGE("Open file (%s) failed!\n", filepath);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = (char *)malloc(size * sizeof(char));
    if (buffer == NULL) {
        TEMP_LOGF("Out of memory!\n");
        ret = -1;
        goto exit;
    }

    count = 0;
    while (count != size) {
        ret = fread(buffer + count, sizeof(char), size - count, fp);
        if (ret < 0) {
            TEMP_LOGE("Read from file (%s) failed!\n", filepath);
            free(buffer);
            buffer = NULL;
            ret = -1;
            goto exit;
        }

        count += ret;
    }
    *json = buffer;
    ret = size;

exit:
    fclose(fp);
    return ret;
}

int json_parse(const char *filepath)
{
    int ret, size;
    char *json;
    jsmn_parser parser;
    jsmntok_t tokens[MAX_JSON_TOKENS];
    int i, j, k;

    for (i = 0; i < ID_MAX; ++i) {
        setting[i] = -1;
    }

    size = json_read_file(filepath, &json);
    if (size < 0) {
        TEMP_LOGE("Read JSON file failed!\n");
        return -1;
    }

    jsmn_init(&parser);
    ret = jsmn_parse(&parser, json, size, tokens, MAX_JSON_TOKENS);
    if (ret < 0) {
        TEMP_LOGE("Parse JSON file failed!\n");
        ret = -1;
        goto exit;
    }

    assert(tokens[0].type == JSMN_OBJECT);
    for (i = 0; i < (tokens[0].size / 2); ++i) {
        jsmntok_t *key = &tokens[1 + i * 2 + 0];
        jsmntok_t *val = &tokens[1 + i * 2 + 1];

        /* key must be string */
        assert(key->type == JSMN_STRING);
        /* value must be string or integer */
        assert(val->type == JSMN_STRING || val->type == JSMN_PRIMITIVE);

        for (j = 0; j < sizeof(keys)/sizeof(char *); ++j) {
            if (strncmp(json + key->start, keys[j], key->end - key->start) == 0
                    && strlen(keys[j]) == (key->end - key->start)) {
                json[val->end] = '\0';
                if (val->type == JSMN_STRING) {
                    for (k = 0; k < sizeof(ampm)/sizeof(char *); ++k) {
                        if (strncmp(json + val->start, ampm[k], val->end - val->start) == 0
                                && strlen(ampm[k]) == (val->end - val->start)) {
                            setting[j] = k;
                            break;
                        }
                    }
                } else {
                    setting[j] = atoi(json + val->start);
                }
                break;
            }
        }
    }

exit:
    free(json);
    json = NULL;
    return ret;
}

int json_get_automode(void)
{
    return setting[AUTOMODE];
}

int json_get_from_hour(void)
{
    return setting[FROM_HOUR];
}

int json_get_from_minute(void)
{
    return setting[FROM_MINUTE];
}

const char *json_get_from_ampm(void)
{
    return ampm[setting[FROM_AMPM]];
}

int json_get_to_hour(void)
{
    return setting[TO_HOUR];
}

int json_get_to_minute(void)
{
    return setting[TO_MINUTE];
}

const char *json_get_to_ampm(void)
{
    return ampm[setting[TO_AMPM]];
}

int json_get_target_temp(void)
{
    return setting[TARGET];
}

int json_get_tolerance_temp(void)
{
    return setting[TOLERANCE];
}
