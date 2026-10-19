/* ***************************************************************************
 *
 * Copyright 2019-2026 Samsung Electronics All Rights Reserved.
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
#include "caps_airPurifierFanMode.h"

static int caps_airPurifierFanMode_attr_airPurifierFanMode_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_AIRPURIFIERFANMODE_AIRPURIFIERFANMODE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_airPurifierFanMode.attr_airPurifierFanMode.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_airPurifierFanMode_get_airPurifierFanMode_value(caps_airPurifierFanMode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->airPurifierFanMode_value;
}

static void caps_airPurifierFanMode_set_airPurifierFanMode_value(caps_airPurifierFanMode_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->airPurifierFanMode_value) {
        free(caps_data->airPurifierFanMode_value);
    }
    caps_data->airPurifierFanMode_value = strdup(value);
}

static void caps_airPurifierFanMode_attr_airPurifierFanMode_send(caps_airPurifierFanMode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->airPurifierFanMode_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_airPurifierFanMode.attr_airPurifierFanMode.name,
            caps_data->airPurifierFanMode_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send airPurifierFanMode value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static const char **caps_airPurifierFanMode_get_supportedAirPurifierFanModes_value(caps_airPurifierFanMode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedAirPurifierFanModes_value;
}

static void caps_airPurifierFanMode_set_supportedAirPurifierFanModes_value(caps_airPurifierFanMode_data_t *caps_data,
        const char **value, int arraySize)
{
    int i;

    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedAirPurifierFanModes_value) {
        for (i = 0; i < caps_data->supportedAirPurifierFanModes_arraySize; i++) {
            free(caps_data->supportedAirPurifierFanModes_value[i]);
        }
        free(caps_data->supportedAirPurifierFanModes_value);
    }

    caps_data->supportedAirPurifierFanModes_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedAirPurifierFanModes_value) {
        printf("fail to malloc for supportedAirPurifierFanModes_value\n");
        caps_data->supportedAirPurifierFanModes_arraySize = 0;
        return;
    }

    for (i = 0; i < arraySize; i++) {
        caps_data->supportedAirPurifierFanModes_value[i] = strdup(value[i]);
    }
    caps_data->supportedAirPurifierFanModes_arraySize = arraySize;
}

static void caps_airPurifierFanMode_attr_supportedAirPurifierFanModes_send(caps_airPurifierFanMode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedAirPurifierFanModes_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_airPurifierFanMode.attr_supportedAirPurifierFanModes.name,
            caps_data->supportedAirPurifierFanModes_value,
            caps_data->supportedAirPurifierFanModes_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedAirPurifierFanModes value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static void caps_airPurifierFanMode_cmd_setAirPurifierFanMode_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_airPurifierFanMode_data_t *caps_data = (caps_airPurifierFanMode_data_t *)usr_data;
    char *value;
    int index;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    index = caps_airPurifierFanMode_attr_airPurifierFanMode_str2idx(cmd_data->cmd_data[0].string);
    if (index < 0) {
        printf("%s is not supported value for setAirPurifierFanMode\n", cmd_data->cmd_data[0].string);
        return;
    }
    value = (char *)caps_helper_airPurifierFanMode.attr_airPurifierFanMode.values[index];

    caps_airPurifierFanMode_set_airPurifierFanMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_setAirPurifierFanMode_usr_cb)
        caps_data->cmd_setAirPurifierFanMode_usr_cb(caps_data);
    caps_airPurifierFanMode_attr_airPurifierFanMode_send(caps_data);
}

static void caps_airPurifierFanMode_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_airPurifierFanMode_data_t *caps_data = usr_data;

    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_airPurifierFanMode_attr_airPurifierFanMode_send(caps_data);
    caps_airPurifierFanMode_attr_supportedAirPurifierFanModes_send(caps_data);
}

caps_airPurifierFanMode_data_t *caps_airPurifierFanMode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb,
        void *usr_data)
{
    caps_airPurifierFanMode_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_airPurifierFanMode_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_airPurifierFanMode_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_airPurifierFanMode_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_airPurifierFanMode_value = caps_airPurifierFanMode_get_airPurifierFanMode_value;
    caps_data->set_airPurifierFanMode_value = caps_airPurifierFanMode_set_airPurifierFanMode_value;
    caps_data->attr_airPurifierFanMode_str2idx = caps_airPurifierFanMode_attr_airPurifierFanMode_str2idx;
    caps_data->attr_airPurifierFanMode_send = caps_airPurifierFanMode_attr_airPurifierFanMode_send;
    caps_data->get_supportedAirPurifierFanModes_value = caps_airPurifierFanMode_get_supportedAirPurifierFanModes_value;
    caps_data->set_supportedAirPurifierFanModes_value = caps_airPurifierFanMode_set_supportedAirPurifierFanModes_value;
    caps_data->attr_supportedAirPurifierFanModes_send = caps_airPurifierFanMode_attr_supportedAirPurifierFanModes_send;

    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_airPurifierFanMode.id,
                caps_airPurifierFanMode_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_airPurifierFanMode.cmd_setAirPurifierFanMode.name,
                caps_airPurifierFanMode_cmd_setAirPurifierFanMode_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setAirPurifierFanMode of airPurifierFanMode\n");
        }
    } else {
        printf("fail to init airPurifierFanMode handle\n");
    }

    return caps_data;
}
