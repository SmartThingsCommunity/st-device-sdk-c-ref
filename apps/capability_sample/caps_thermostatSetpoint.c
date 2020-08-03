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
#include "caps_thermostatSetpoint.h"

static double caps_thermostatSetpoint_get_thermostatSetpoint_value(caps_thermostatSetpoint_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_thermostatSetpoint.attr_thermostatSetpoint.min - 1;
    }
    return caps_data->thermostatSetpoint_value;
}

static void caps_thermostatSetpoint_set_thermostatSetpoint_value(caps_thermostatSetpoint_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->thermostatSetpoint_value = value;
}

static const char *caps_thermostatSetpoint_get_thermostatSetpoint_unit(caps_thermostatSetpoint_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->thermostatSetpoint_unit;
}

static void caps_thermostatSetpoint_set_thermostatSetpoint_unit(caps_thermostatSetpoint_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->thermostatSetpoint_unit = (char *)unit;
}

static void caps_thermostatSetpoint_attr_thermostatSetpoint_send(caps_thermostatSetpoint_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    cap_evt = st_cap_attr_create_number((char *) caps_helper_thermostatSetpoint.attr_thermostatSetpoint.name, caps_data->thermostatSetpoint_value, caps_data->thermostatSetpoint_unit);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send thermostatSetpoint value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static void caps_thermostatSetpoint_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_thermostatSetpoint_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_thermostatSetpoint_attr_thermostatSetpoint_send(caps_data);
}

caps_thermostatSetpoint_data_t *caps_thermostatSetpoint_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_thermostatSetpoint_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_thermostatSetpoint_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_thermostatSetpoint_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_thermostatSetpoint_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_thermostatSetpoint_value = caps_thermostatSetpoint_get_thermostatSetpoint_value;
    caps_data->set_thermostatSetpoint_value = caps_thermostatSetpoint_set_thermostatSetpoint_value;
    caps_data->get_thermostatSetpoint_unit = caps_thermostatSetpoint_get_thermostatSetpoint_unit;
    caps_data->set_thermostatSetpoint_unit = caps_thermostatSetpoint_set_thermostatSetpoint_unit;
    caps_data->attr_thermostatSetpoint_send = caps_thermostatSetpoint_attr_thermostatSetpoint_send;
    caps_data->thermostatSetpoint_value = -460;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_thermostatSetpoint.id, caps_thermostatSetpoint_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init thermostatSetpoint handle\n");
    }

    return caps_data;
}
