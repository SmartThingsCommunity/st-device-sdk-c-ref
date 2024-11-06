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
#include "caps_ovenSetpoint.h"

static int caps_ovenSetpoint_get_ovenSetpoint_value(caps_ovenSetpoint_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_ovenSetpoint.attr_ovenSetpoint.min - 1;
    }
    return caps_data->ovenSetpoint_value;
}

static void caps_ovenSetpoint_set_ovenSetpoint_value(caps_ovenSetpoint_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->ovenSetpoint_value = value;
}

static void caps_ovenSetpoint_attr_ovenSetpoint_send(caps_ovenSetpoint_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_ovenSetpoint.attr_ovenSetpoint.name,
            caps_data->ovenSetpoint_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send ovenSetpoint value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_ovenSetpoint_cmd_setOvenSetpoint_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_ovenSetpoint_data_t *caps_data = (caps_ovenSetpoint_data_t *)usr_data;
    int value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].integer;

    caps_ovenSetpoint_set_ovenSetpoint_value(caps_data, value);
    if (caps_data && caps_data->cmd_setOvenSetpoint_usr_cb)
        caps_data->cmd_setOvenSetpoint_usr_cb(caps_data);
    caps_ovenSetpoint_attr_ovenSetpoint_send(caps_data);
}

static void caps_ovenSetpoint_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_ovenSetpoint_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_ovenSetpoint_attr_ovenSetpoint_send(caps_data);
}

caps_ovenSetpoint_data_t *caps_ovenSetpoint_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_ovenSetpoint_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_ovenSetpoint_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_ovenSetpoint_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_ovenSetpoint_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_ovenSetpoint_value = caps_ovenSetpoint_get_ovenSetpoint_value;
    caps_data->set_ovenSetpoint_value = caps_ovenSetpoint_set_ovenSetpoint_value;
    caps_data->attr_ovenSetpoint_send = caps_ovenSetpoint_attr_ovenSetpoint_send;
    caps_data->ovenSetpoint_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_ovenSetpoint.id, caps_ovenSetpoint_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_ovenSetpoint.cmd_setOvenSetpoint.name, caps_ovenSetpoint_cmd_setOvenSetpoint_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setOvenSetpoint of ovenSetpoint\n");
    }
    } else {
        printf("fail to init ovenSetpoint handle\n");
    }

    return caps_data;
}
