/* ***************************************************************************
 *
 * Copyright 2019-2020 Samsung Electronics All Rights Reserved.
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

#include "st_dev.h"
#include "caps_windowShade.h"

#include "freertos/FreeRTOS.h"

static int caps_windowShade_attr_windowShade_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_WINDOWSHADE_WINDOWSHADE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_windowShade.attr_windowShade.values[index])) {
            return index;
        }
    }   
    return -1; 
}

static const char *caps_windowShade_get_windowShade_value(caps_windowShade_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->windowShade_value;
}

static void caps_windowShade_set_windowShade_value(caps_windowShade_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->windowShade_value) {
        free(caps_data->windowShade_value);
    }
    caps_data->windowShade_value = strdup(value);
}

static void caps_windowShade_attr_windowShade_send(caps_windowShade_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->windowShade_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string((char *)caps_helper_windowShade.attr_windowShade.name,
        caps_data->windowShade_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send windowShade value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static const char **caps_windowShade_get_supportedWindowShadeCommands_value(caps_windowShade_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedWindowShadeCommands_value;
}

static void caps_windowShade_set_supportedWindowShadeCommands_value(caps_windowShade_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedWindowShadeCommands_value) {
        for (i = 0; i < caps_data->supportedWindowShadeCommands_arraySize; i++) {
            free(caps_data->supportedWindowShadeCommands_value[i]);
        }
        free(caps_data->supportedWindowShadeCommands_value);
    }
    caps_data->supportedWindowShadeCommands_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedWindowShadeCommands_value) {
        printf("fail to malloc for supportedWindowShadeCommands_value\n");
        caps_data->supportedWindowShadeCommands_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedWindowShadeCommands_value[i] = strdup(value[i]);
    }

    caps_data->supportedWindowShadeCommands_arraySize = arraySize;
}

static void caps_windowShade_attr_supportedWindowShadeCommands_send(caps_windowShade_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedWindowShadeCommands_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string_array((char *)caps_helper_windowShade.attr_supportedWindowShadeCommands.name,
        caps_data->supportedWindowShadeCommands_arraySize, caps_data->supportedWindowShadeCommands_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send supportedWindowShadeCommands value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static void caps_windowShade_cmd_close_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_windowShade_data_t *caps_data = (caps_windowShade_data_t *)usr_data;
    const char* value = caps_helper_windowShade.attr_windowShade.values[CAP_ENUM_WINDOWSHADE_WINDOWSHADE_VALUE_CLOSED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_windowShade_set_windowShade_value(caps_data, value);
    if (caps_data && caps_data->cmd_close_usr_cb)
        caps_data->cmd_close_usr_cb(caps_data);
    caps_windowShade_attr_windowShade_send(caps_data);
}

static void caps_windowShade_cmd_pause_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_windowShade_data_t *caps_data = (caps_windowShade_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_pause_usr_cb)
        caps_data->cmd_pause_usr_cb(caps_data);
}

static void caps_windowShade_cmd_open_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_windowShade_data_t *caps_data = (caps_windowShade_data_t *)usr_data;
    const char* value = caps_helper_windowShade.attr_windowShade.values[CAP_ENUM_WINDOWSHADE_WINDOWSHADE_VALUE_OPEN];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_windowShade_set_windowShade_value(caps_data, value);
    if (caps_data && caps_data->cmd_open_usr_cb)
        caps_data->cmd_open_usr_cb(caps_data);
    caps_windowShade_attr_windowShade_send(caps_data);
}

static void caps_windowShade_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_windowShade_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_windowShade_attr_windowShade_send(caps_data);
    caps_windowShade_attr_supportedWindowShadeCommands_send(caps_data);
}

caps_windowShade_data_t *caps_windowShade_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_windowShade_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_windowShade_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_windowShade_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_windowShade_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_windowShade_value = caps_windowShade_get_windowShade_value;
    caps_data->set_windowShade_value = caps_windowShade_set_windowShade_value;
    caps_data->attr_windowShade_str2idx = caps_windowShade_attr_windowShade_str2idx;
    caps_data->attr_windowShade_send = caps_windowShade_attr_windowShade_send;
    caps_data->get_supportedWindowShadeCommands_value = caps_windowShade_get_supportedWindowShadeCommands_value;
    caps_data->set_supportedWindowShadeCommands_value = caps_windowShade_set_supportedWindowShadeCommands_value;
    caps_data->attr_supportedWindowShadeCommands_send = caps_windowShade_attr_supportedWindowShadeCommands_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_windowShade.id, caps_windowShade_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_windowShade.cmd_close.name, caps_windowShade_cmd_close_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for close of windowShade\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_windowShade.cmd_pause.name, caps_windowShade_cmd_pause_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for pause of windowShade\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_windowShade.cmd_open.name, caps_windowShade_cmd_open_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for open of windowShade\n");
    }
    } else {
        printf("fail to init windowShade handle\n");
    }

    return caps_data;
}
