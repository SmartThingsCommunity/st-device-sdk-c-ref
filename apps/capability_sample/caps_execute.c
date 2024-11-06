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
#include "caps_execute.h"

static const JSON_H *caps_execute_get_data_value(caps_execute_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const JSON_H *)caps_data->data_value;
}

static void caps_execute_set_data_value(caps_execute_data_t *caps_data, const JSON_H *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->data_value) {
        JSON_DELETE(caps_data->data_value);
    }
    caps_data->data_value = JSON_DUPLICATE(value, true);
}

static void caps_execute_attr_data_send(caps_execute_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no = -1;
    iot_cap_val_t value;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->data_value) {
        printf("value is NULL\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_JSON_OBJECT;
    value.json_object = JSON_PRINT(caps_data->data_value);

    cap_evt = st_cap_create_attr(caps_data->handle,
        (char *)caps_helper_execute.attr_data.name,
        &value,
        NULL,
        NULL);

    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_send_attr(&cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send data value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt);
}


static void caps_execute_cmd_execute_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_execute_data_t *caps_data = usr_data;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    if (caps_data && caps_data->cmd_execute_usr_cb)
        caps_data->cmd_execute_usr_cb(caps_data);
}

static void caps_execute_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_execute_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_execute_attr_data_send(caps_data);
}

caps_execute_data_t *caps_execute_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_execute_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_execute_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_execute_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_execute_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_data_value = caps_execute_get_data_value;
    caps_data->set_data_value = caps_execute_set_data_value;
    caps_data->attr_data_send = caps_execute_attr_data_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_execute.id, caps_execute_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_execute.cmd_execute.name, caps_execute_cmd_execute_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for execute of execute\n");
    }
    } else {
        printf("fail to init execute handle\n");
    }

    return caps_data;
}
