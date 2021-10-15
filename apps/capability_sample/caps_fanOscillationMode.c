/* ***************************************************************************
 *
 * Copyright 2019-2021 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "st_dev.h"
#include "caps_fanOscillationMode.h"

static const char **caps_fanOscillationMode_get_supportedFanOscillationModes_value(caps_fanOscillationMode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedFanOscillationModes_value;
}

static void caps_fanOscillationMode_set_supportedFanOscillationModes_value(caps_fanOscillationMode_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedFanOscillationModes_value) {
        for (i = 0; i < caps_data->supportedFanOscillationModes_arraySize; i++) {
            free(caps_data->supportedFanOscillationModes_value[i]);
        }
        free(caps_data->supportedFanOscillationModes_value);
    }
    caps_data->supportedFanOscillationModes_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedFanOscillationModes_value) {
        printf("fail to malloc for supportedFanOscillationModes_value\n");
        caps_data->supportedFanOscillationModes_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedFanOscillationModes_value[i] = strdup(value[i]);
    }

    caps_data->supportedFanOscillationModes_arraySize = arraySize;
}

static void caps_fanOscillationMode_attr_supportedFanOscillationModes_send(caps_fanOscillationMode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedFanOscillationModes_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_fanOscillationMode.attr_supportedFanOscillationModes.name,
            caps_data->supportedFanOscillationModes_value,
            caps_data->supportedFanOscillationModes_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedFanOscillationModes value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static int caps_fanOscillationMode_attr_fanOscillationMode_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_FANOSCILLATIONMODE_FANOSCILLATIONMODE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_fanOscillationMode.attr_fanOscillationMode.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_fanOscillationMode_get_fanOscillationMode_value(caps_fanOscillationMode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->fanOscillationMode_value;
}

static void caps_fanOscillationMode_set_fanOscillationMode_value(caps_fanOscillationMode_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->fanOscillationMode_value) {
        free(caps_data->fanOscillationMode_value);
    }
    caps_data->fanOscillationMode_value = strdup(value);
}

static void caps_fanOscillationMode_attr_fanOscillationMode_send(caps_fanOscillationMode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->fanOscillationMode_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_fanOscillationMode.attr_fanOscillationMode.name,
            caps_data->fanOscillationMode_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send fanOscillationMode value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_fanOscillationMode_cmd_setFanOscillationMode_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_fanOscillationMode_data_t *caps_data = (caps_fanOscillationMode_data_t *)usr_data;
    char *value;
    int index;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    index = caps_fanOscillationMode_attr_fanOscillationMode_str2idx(cmd_data->cmd_data[0].string);
    if (index < 0) {
        printf("%s is not supported value for setFanOscillationMode\n", cmd_data->cmd_data[0].string);
        return;
    }
    value = (char *)caps_helper_fanOscillationMode.attr_fanOscillationMode.values[index];

    caps_fanOscillationMode_set_fanOscillationMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_setFanOscillationMode_usr_cb)
        caps_data->cmd_setFanOscillationMode_usr_cb(caps_data);
    caps_fanOscillationMode_attr_fanOscillationMode_send(caps_data);
}

static void caps_fanOscillationMode_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_fanOscillationMode_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_fanOscillationMode_attr_supportedFanOscillationModes_send(caps_data);
    caps_fanOscillationMode_attr_fanOscillationMode_send(caps_data);
}

caps_fanOscillationMode_data_t *caps_fanOscillationMode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_fanOscillationMode_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_fanOscillationMode_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_fanOscillationMode_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_fanOscillationMode_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_supportedFanOscillationModes_value = caps_fanOscillationMode_get_supportedFanOscillationModes_value;
    caps_data->set_supportedFanOscillationModes_value = caps_fanOscillationMode_set_supportedFanOscillationModes_value;
    caps_data->attr_supportedFanOscillationModes_send = caps_fanOscillationMode_attr_supportedFanOscillationModes_send;
    caps_data->get_fanOscillationMode_value = caps_fanOscillationMode_get_fanOscillationMode_value;
    caps_data->set_fanOscillationMode_value = caps_fanOscillationMode_set_fanOscillationMode_value;
    caps_data->attr_fanOscillationMode_str2idx = caps_fanOscillationMode_attr_fanOscillationMode_str2idx;
    caps_data->attr_fanOscillationMode_send = caps_fanOscillationMode_attr_fanOscillationMode_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_fanOscillationMode.id, caps_fanOscillationMode_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_fanOscillationMode.cmd_setFanOscillationMode.name, caps_fanOscillationMode_cmd_setFanOscillationMode_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setFanOscillationMode of fanOscillationMode\n");
    }
    } else {
        printf("fail to init fanOscillationMode handle\n");
    }

    return caps_data;
}
