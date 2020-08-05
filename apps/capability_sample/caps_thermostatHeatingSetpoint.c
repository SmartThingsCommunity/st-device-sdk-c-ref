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
#include "caps_thermostatHeatingSetpoint.h"

static double caps_thermostatHeatingSetpoint_get_heatingSetpoint_value(caps_thermostatHeatingSetpoint_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.min - 1;
    }
    return caps_data->heatingSetpoint_value;
}

static void caps_thermostatHeatingSetpoint_set_heatingSetpoint_value(caps_thermostatHeatingSetpoint_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->heatingSetpoint_value = value;
}

static const char *caps_thermostatHeatingSetpoint_get_heatingSetpoint_unit(caps_thermostatHeatingSetpoint_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->heatingSetpoint_unit;
}

static void caps_thermostatHeatingSetpoint_set_heatingSetpoint_unit(caps_thermostatHeatingSetpoint_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->heatingSetpoint_unit = (char *)unit;
}

static void caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send(caps_thermostatHeatingSetpoint_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.name,
            caps_data->heatingSetpoint_value,
            caps_data->heatingSetpoint_unit,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send heatingSetpoint value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_thermostatHeatingSetpoint_cmd_setHeatingSetpoint_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_thermostatHeatingSetpoint_data_t *caps_data = (caps_thermostatHeatingSetpoint_data_t *)usr_data;
    double value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].number;

    caps_thermostatHeatingSetpoint_set_heatingSetpoint_value(caps_data, value);
    if (caps_data && caps_data->cmd_setHeatingSetpoint_usr_cb)
        caps_data->cmd_setHeatingSetpoint_usr_cb(caps_data);
    caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send(caps_data);
}

static void caps_thermostatHeatingSetpoint_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_thermostatHeatingSetpoint_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send(caps_data);
}

caps_thermostatHeatingSetpoint_data_t *caps_thermostatHeatingSetpoint_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_thermostatHeatingSetpoint_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_thermostatHeatingSetpoint_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_thermostatHeatingSetpoint_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_thermostatHeatingSetpoint_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_heatingSetpoint_value = caps_thermostatHeatingSetpoint_get_heatingSetpoint_value;
    caps_data->set_heatingSetpoint_value = caps_thermostatHeatingSetpoint_set_heatingSetpoint_value;
    caps_data->get_heatingSetpoint_unit = caps_thermostatHeatingSetpoint_get_heatingSetpoint_unit;
    caps_data->set_heatingSetpoint_unit = caps_thermostatHeatingSetpoint_set_heatingSetpoint_unit;
    caps_data->attr_heatingSetpoint_send = caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send;
    caps_data->heatingSetpoint_value = -460;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_thermostatHeatingSetpoint.id, caps_thermostatHeatingSetpoint_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatHeatingSetpoint.cmd_setHeatingSetpoint.name, caps_thermostatHeatingSetpoint_cmd_setHeatingSetpoint_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setHeatingSetpoint of thermostatHeatingSetpoint\n");
    }
    } else {
        printf("fail to init thermostatHeatingSetpoint handle\n");
    }

    return caps_data;
}
