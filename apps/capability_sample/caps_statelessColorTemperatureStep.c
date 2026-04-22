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
#include "caps_statelessColorTemperatureStep.h"

static int caps_statelessColorTemperatureStep_get_stepSize_value(caps_statelessColorTemperatureStep_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_statelessColorTemperatureStep.cmd_stepColorTemperatureByPercent.min - 1;
    }
    return caps_data->stepSize_value;
}

static void caps_statelessColorTemperatureStep_set_stepSize_value(caps_statelessColorTemperatureStep_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->stepSize_value = value;
}

static void caps_statelessColorTemperatureStep_cmd_stepColorTemperatureByPercent_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_statelessColorTemperatureStep_data_t *caps_data = (caps_statelessColorTemperatureStep_data_t *)usr_data;
    int value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].integer;
    if (value < caps_helper_statelessColorTemperatureStep.cmd_stepColorTemperatureByPercent.min ||
            value > caps_helper_statelessColorTemperatureStep.cmd_stepColorTemperatureByPercent.max) {
        printf("%d is out of range for stepColorTemperatureByPercent\n", value);
        return;
    }

    caps_data->cmd_data = cmd_data;
    caps_statelessColorTemperatureStep_set_stepSize_value(caps_data, value);
    if (caps_data && caps_data->cmd_stepColorTemperatureByPercent_usr_cb)
        caps_data->cmd_stepColorTemperatureByPercent_usr_cb(caps_data);
}

static void caps_statelessColorTemperatureStep_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_statelessColorTemperatureStep_data_t *caps_data = usr_data;

    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
}

caps_statelessColorTemperatureStep_data_t *caps_statelessColorTemperatureStep_initialize(IOT_CTX *ctx, const char *component,
        void *init_usr_cb, void *usr_data)
{
    caps_statelessColorTemperatureStep_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_statelessColorTemperatureStep_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_statelessColorTemperatureStep_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_statelessColorTemperatureStep_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;
    caps_data->get_stepSize_value = caps_statelessColorTemperatureStep_get_stepSize_value;
    caps_data->set_stepSize_value = caps_statelessColorTemperatureStep_set_stepSize_value;

    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_statelessColorTemperatureStep.id,
                caps_statelessColorTemperatureStep_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_statelessColorTemperatureStep.cmd_stepColorTemperatureByPercent.name,
                caps_statelessColorTemperatureStep_cmd_stepColorTemperatureByPercent_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for stepColorTemperatureByPercent of statelessColorTemperatureStep\n");
        }
    } else {
        printf("fail to init statelessColorTemperatureStep handle\n");
    }

    return caps_data;
}
