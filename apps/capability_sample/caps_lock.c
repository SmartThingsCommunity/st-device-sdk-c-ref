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
#include "caps_lock.h"

static int caps_lock_attr_lock_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_LOCK_LOCK_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_lock.attr_lock.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_lock_get_lock_value(caps_lock_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->lock_value;
}

static void caps_lock_set_lock_value(caps_lock_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->lock_value) {
        free(caps_data->lock_value);
    }
    caps_data->lock_value = strdup(value);
}

static void caps_lock_attr_lock_send(caps_lock_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->lock_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_lock.attr_lock.name,
            caps_data->lock_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send lock value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_lock_cmd_lock_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_lock_data_t *caps_data = (caps_lock_data_t *)usr_data;
    const char* value = caps_helper_lock.attr_lock.values[CAP_ENUM_LOCK_LOCK_VALUE_LOCKED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_lock_set_lock_value(caps_data, value);
    if (caps_data && caps_data->cmd_lock_usr_cb)
        caps_data->cmd_lock_usr_cb(caps_data);
    caps_lock_attr_lock_send(caps_data);
}

static void caps_lock_cmd_unlock_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_lock_data_t *caps_data = (caps_lock_data_t *)usr_data;
    const char* value = caps_helper_lock.attr_lock.values[CAP_ENUM_LOCK_LOCK_VALUE_UNLOCKED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_lock_set_lock_value(caps_data, value);
    if (caps_data && caps_data->cmd_unlock_usr_cb)
        caps_data->cmd_unlock_usr_cb(caps_data);
    caps_lock_attr_lock_send(caps_data);
}

static void caps_lock_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_lock_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_lock_attr_lock_send(caps_data);
}

caps_lock_data_t *caps_lock_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_lock_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_lock_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_lock_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_lock_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_lock_value = caps_lock_get_lock_value;
    caps_data->set_lock_value = caps_lock_set_lock_value;
    caps_data->attr_lock_str2idx = caps_lock_attr_lock_str2idx;
    caps_data->attr_lock_send = caps_lock_attr_lock_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_lock.id, caps_lock_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_lock.cmd_lock.name, caps_lock_cmd_lock_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for lock of lock\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_lock.cmd_unlock.name, caps_lock_cmd_unlock_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for unlock of lock\n");
    }
    } else {
        printf("fail to init lock handle\n");
    }

    return caps_data;
}
