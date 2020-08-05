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
#include "caps_thermostatFanMode.h"

static int caps_thermostatFanMode_attr_thermostatFanMode_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_THERMOSTATFANMODE_THERMOSTATFANMODE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_thermostatFanMode.attr_thermostatFanMode.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_thermostatFanMode_get_thermostatFanMode_value(caps_thermostatFanMode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->thermostatFanMode_value;
}

static void caps_thermostatFanMode_set_thermostatFanMode_value(caps_thermostatFanMode_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->thermostatFanMode_value) {
        free(caps_data->thermostatFanMode_value);
    }
    caps_data->thermostatFanMode_value = strdup(value);
}

static void caps_thermostatFanMode_attr_thermostatFanMode_send(caps_thermostatFanMode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->thermostatFanMode_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_thermostatFanMode.attr_thermostatFanMode.name,
            caps_data->thermostatFanMode_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send thermostatFanMode value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static const char **caps_thermostatFanMode_get_supportedThermostatFanModes_value(caps_thermostatFanMode_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedThermostatFanModes_value;
}

static void caps_thermostatFanMode_set_supportedThermostatFanModes_value(caps_thermostatFanMode_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedThermostatFanModes_value) {
        for (i = 0; i < caps_data->supportedThermostatFanModes_arraySize; i++) {
            free(caps_data->supportedThermostatFanModes_value[i]);
        }
        free(caps_data->supportedThermostatFanModes_value);
    }
    caps_data->supportedThermostatFanModes_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedThermostatFanModes_value) {
        printf("fail to malloc for supportedThermostatFanModes_value\n");
        caps_data->supportedThermostatFanModes_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedThermostatFanModes_value[i] = strdup(value[i]);
    }

    caps_data->supportedThermostatFanModes_arraySize = arraySize;
}

static void caps_thermostatFanMode_attr_supportedThermostatFanModes_send(caps_thermostatFanMode_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedThermostatFanModes_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_thermostatFanMode.attr_supportedThermostatFanModes.name,
            caps_data->supportedThermostatFanModes_value,
            caps_data->supportedThermostatFanModes_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedThermostatFanModes value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_thermostatFanMode_cmd_fanOn_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_thermostatFanMode_data_t *caps_data = (caps_thermostatFanMode_data_t *)usr_data;
    const char* value = caps_helper_thermostatFanMode.attr_thermostatFanMode.values[CAP_ENUM_THERMOSTATFANMODE_THERMOSTATFANMODE_VALUE_ON];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_thermostatFanMode_set_thermostatFanMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_fanOn_usr_cb)
        caps_data->cmd_fanOn_usr_cb(caps_data);
    caps_thermostatFanMode_attr_thermostatFanMode_send(caps_data);
}

static void caps_thermostatFanMode_cmd_fanCirculate_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_thermostatFanMode_data_t *caps_data = (caps_thermostatFanMode_data_t *)usr_data;
    const char* value = caps_helper_thermostatFanMode.attr_thermostatFanMode.values[CAP_ENUM_THERMOSTATFANMODE_THERMOSTATFANMODE_VALUE_CIRCULATE];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_thermostatFanMode_set_thermostatFanMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_fanCirculate_usr_cb)
        caps_data->cmd_fanCirculate_usr_cb(caps_data);
    caps_thermostatFanMode_attr_thermostatFanMode_send(caps_data);
}

static void caps_thermostatFanMode_cmd_fanAuto_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_thermostatFanMode_data_t *caps_data = (caps_thermostatFanMode_data_t *)usr_data;
    const char* value = caps_helper_thermostatFanMode.attr_thermostatFanMode.values[CAP_ENUM_THERMOSTATFANMODE_THERMOSTATFANMODE_VALUE_AUTO];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_thermostatFanMode_set_thermostatFanMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_fanAuto_usr_cb)
        caps_data->cmd_fanAuto_usr_cb(caps_data);
    caps_thermostatFanMode_attr_thermostatFanMode_send(caps_data);
}

static void caps_thermostatFanMode_cmd_setThermostatFanMode_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_thermostatFanMode_data_t *caps_data = (caps_thermostatFanMode_data_t *)usr_data;
    char *value;
    int index;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    index = caps_thermostatFanMode_attr_thermostatFanMode_str2idx(cmd_data->cmd_data[0].string);
    if (index < 0) {
        printf("%s is not supported value for setThermostatFanMode\n", cmd_data->cmd_data[0].string);
        return;
    }
    value = (char *)caps_helper_thermostatFanMode.attr_thermostatFanMode.values[index];

    caps_thermostatFanMode_set_thermostatFanMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_setThermostatFanMode_usr_cb)
        caps_data->cmd_setThermostatFanMode_usr_cb(caps_data);
    caps_thermostatFanMode_attr_thermostatFanMode_send(caps_data);
}

static void caps_thermostatFanMode_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_thermostatFanMode_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_thermostatFanMode_attr_thermostatFanMode_send(caps_data);
    caps_thermostatFanMode_attr_supportedThermostatFanModes_send(caps_data);
}

caps_thermostatFanMode_data_t *caps_thermostatFanMode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_thermostatFanMode_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_thermostatFanMode_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_thermostatFanMode_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_thermostatFanMode_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_thermostatFanMode_value = caps_thermostatFanMode_get_thermostatFanMode_value;
    caps_data->set_thermostatFanMode_value = caps_thermostatFanMode_set_thermostatFanMode_value;
    caps_data->attr_thermostatFanMode_str2idx = caps_thermostatFanMode_attr_thermostatFanMode_str2idx;
    caps_data->attr_thermostatFanMode_send = caps_thermostatFanMode_attr_thermostatFanMode_send;
    caps_data->get_supportedThermostatFanModes_value = caps_thermostatFanMode_get_supportedThermostatFanModes_value;
    caps_data->set_supportedThermostatFanModes_value = caps_thermostatFanMode_set_supportedThermostatFanModes_value;
    caps_data->attr_supportedThermostatFanModes_send = caps_thermostatFanMode_attr_supportedThermostatFanModes_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_thermostatFanMode.id, caps_thermostatFanMode_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatFanMode.cmd_fanOn.name, caps_thermostatFanMode_cmd_fanOn_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for fanOn of thermostatFanMode\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatFanMode.cmd_fanCirculate.name, caps_thermostatFanMode_cmd_fanCirculate_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for fanCirculate of thermostatFanMode\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatFanMode.cmd_fanAuto.name, caps_thermostatFanMode_cmd_fanAuto_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for fanAuto of thermostatFanMode\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatFanMode.cmd_setThermostatFanMode.name, caps_thermostatFanMode_cmd_setThermostatFanMode_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setThermostatFanMode of thermostatFanMode\n");
    }
    } else {
        printf("fail to init thermostatFanMode handle\n");
    }

    return caps_data;
}
