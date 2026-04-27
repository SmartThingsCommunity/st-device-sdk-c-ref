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
#include "caps_mode.h"

static const char **caps_mode_get_supportedModes_value(caps_mode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedModes_value;
}

static void caps_mode_set_supportedModes_value(caps_mode_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedModes_value) {
        for (i = 0; i < caps_data->supportedModes_arraySize; i++) {
            free(caps_data->supportedModes_value[i]);
        }
        free(caps_data->supportedModes_value);
    }
    caps_data->supportedModes_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedModes_value) {
        printf("fail to malloc for supportedModes_value\n");
        caps_data->supportedModes_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedModes_value[i] = strdup(value[i]);
    }

    caps_data->supportedModes_arraySize = arraySize;
}

static void caps_mode_attr_supportedModes_send(caps_mode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedModes_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_mode.attr_supportedModes.name,
            caps_data->supportedModes_value,
            caps_data->supportedModes_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedModes value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static const char *caps_mode_get_mode_value(caps_mode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->mode_value;
}

static void caps_mode_set_mode_value(caps_mode_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->mode_value) {
        free(caps_data->mode_value);
    }
    caps_data->mode_value = strdup(value);
}

static void caps_mode_attr_mode_send(caps_mode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->mode_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_mode.attr_mode.name,
            caps_data->mode_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send mode value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_mode_cmd_setMode_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_mode_data_t *caps_data = (caps_mode_data_t *)usr_data;
    char *value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].string;

    caps_mode_set_mode_value(caps_data, value);
    if (caps_data && caps_data->cmd_setMode_usr_cb)
        caps_data->cmd_setMode_usr_cb(caps_data);
    caps_mode_attr_mode_send(caps_data);
}

static void caps_mode_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_mode_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_mode_attr_supportedModes_send(caps_data);
    caps_mode_attr_mode_send(caps_data);
}

caps_mode_data_t *caps_mode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_mode_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_mode_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_mode_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_mode_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_supportedModes_value = caps_mode_get_supportedModes_value;
    caps_data->set_supportedModes_value = caps_mode_set_supportedModes_value;
    caps_data->attr_supportedModes_send = caps_mode_attr_supportedModes_send;
    caps_data->get_mode_value = caps_mode_get_mode_value;
    caps_data->set_mode_value = caps_mode_set_mode_value;
    caps_data->attr_mode_send = caps_mode_attr_mode_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_mode.id, caps_mode_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_mode.cmd_setMode.name, caps_mode_cmd_setMode_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setMode of mode\n");
    }
    } else {
        printf("fail to init mode handle\n");
    }

    return caps_data;
}
