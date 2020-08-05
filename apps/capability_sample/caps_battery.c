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
#include "caps_battery.h"

static int caps_battery_get_battery_value(caps_battery_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_battery.attr_battery.min - 1;
    }
    return caps_data->battery_value;
}

static void caps_battery_set_battery_value(caps_battery_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->battery_value = value;
}

static const char *caps_battery_get_battery_unit(caps_battery_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->battery_unit;
}

static void caps_battery_set_battery_unit(caps_battery_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->battery_unit = (char *)unit;
}

static void caps_battery_attr_battery_send(caps_battery_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_battery.attr_battery.name,
            caps_data->battery_value,
            caps_data->battery_unit,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send battery value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_battery_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_battery_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_battery_attr_battery_send(caps_data);
}

caps_battery_data_t *caps_battery_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_battery_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_battery_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_battery_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_battery_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_battery_value = caps_battery_get_battery_value;
    caps_data->set_battery_value = caps_battery_set_battery_value;
    caps_data->get_battery_unit = caps_battery_get_battery_unit;
    caps_data->set_battery_unit = caps_battery_set_battery_unit;
    caps_data->attr_battery_send = caps_battery_attr_battery_send;
    caps_data->battery_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_battery.id, caps_battery_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init battery handle\n");
    }

    return caps_data;
}
