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
#include <stdio.h>
#include <stdlib.h>

#include "st_dev.h"
#include "caps_doorControl.h"

static int caps_doorControl_attr_door_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_DOORCONTROL_DOOR_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_doorControl.attr_door.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_doorControl_get_door_value(caps_doorControl_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->door_value;
}

static void caps_doorControl_set_door_value(caps_doorControl_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->door_value) {
        free(caps_data->door_value);
    }
    caps_data->door_value = strdup(value);
}

static void caps_doorControl_attr_door_send(caps_doorControl_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->door_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_doorControl.attr_door.name,
            caps_data->door_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send door value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_doorControl_cmd_close_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_doorControl_data_t *caps_data = (caps_doorControl_data_t *)usr_data;
    const char* value = caps_helper_doorControl.attr_door.values[CAP_ENUM_DOORCONTROL_DOOR_VALUE_CLOSED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_doorControl_set_door_value(caps_data, value);
    if (caps_data && caps_data->cmd_close_usr_cb)
        caps_data->cmd_close_usr_cb(caps_data);
    caps_doorControl_attr_door_send(caps_data);
}

static void caps_doorControl_cmd_open_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_doorControl_data_t *caps_data = (caps_doorControl_data_t *)usr_data;
    const char* value = caps_helper_doorControl.attr_door.values[CAP_ENUM_DOORCONTROL_DOOR_VALUE_OPEN];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_doorControl_set_door_value(caps_data, value);
    if (caps_data && caps_data->cmd_open_usr_cb)
        caps_data->cmd_open_usr_cb(caps_data);
    caps_doorControl_attr_door_send(caps_data);
}

static void caps_doorControl_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_doorControl_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_doorControl_attr_door_send(caps_data);
}

caps_doorControl_data_t *caps_doorControl_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_doorControl_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_doorControl_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_doorControl_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_doorControl_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_door_value = caps_doorControl_get_door_value;
    caps_data->set_door_value = caps_doorControl_set_door_value;
    caps_data->attr_door_str2idx = caps_doorControl_attr_door_str2idx;
    caps_data->attr_door_send = caps_doorControl_attr_door_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_doorControl.id, caps_doorControl_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_doorControl.cmd_close.name, caps_doorControl_cmd_close_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for close of doorControl\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_doorControl.cmd_open.name, caps_doorControl_cmd_open_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for open of doorControl\n");
    }
    } else {
        printf("fail to init doorControl handle\n");
    }

    return caps_data;
}
