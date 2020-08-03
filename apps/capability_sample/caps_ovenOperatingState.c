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
#include "caps_ovenOperatingState.h"

static int caps_ovenOperatingState_attr_ovenJobState_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_OVENOPERATINGSTATE_OVENJOBSTATE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_ovenOperatingState.attr_ovenJobState.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_ovenOperatingState_get_ovenJobState_value(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->ovenJobState_value;
}

static void caps_ovenOperatingState_set_ovenJobState_value(caps_ovenOperatingState_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->ovenJobState_value) {
        free(caps_data->ovenJobState_value);
    }
    caps_data->ovenJobState_value = strdup(value);
}

static void caps_ovenOperatingState_attr_ovenJobState_send(caps_ovenOperatingState_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->ovenJobState_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string((char *)caps_helper_ovenOperatingState.attr_ovenJobState.name,
        caps_data->ovenJobState_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send ovenJobState value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static const char *caps_ovenOperatingState_get_completionTime_value(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->completionTime_value;
}

static void caps_ovenOperatingState_set_completionTime_value(caps_ovenOperatingState_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->completionTime_value) {
        free(caps_data->completionTime_value);
    }
    caps_data->completionTime_value = strdup(value);
}

static void caps_ovenOperatingState_attr_completionTime_send(caps_ovenOperatingState_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->completionTime_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string((char *)caps_helper_ovenOperatingState.attr_completionTime.name,
        caps_data->completionTime_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send completionTime value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static const char **caps_ovenOperatingState_get_supportedMachineStates_value(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedMachineStates_value;
}

static void caps_ovenOperatingState_set_supportedMachineStates_value(caps_ovenOperatingState_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedMachineStates_value) {
        for (i = 0; i < caps_data->supportedMachineStates_arraySize; i++) {
            free(caps_data->supportedMachineStates_value[i]);
        }
        free(caps_data->supportedMachineStates_value);
    }
    caps_data->supportedMachineStates_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedMachineStates_value) {
        printf("fail to malloc for supportedMachineStates_value\n");
        caps_data->supportedMachineStates_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedMachineStates_value[i] = strdup(value[i]);
    }

    caps_data->supportedMachineStates_arraySize = arraySize;
}

static void caps_ovenOperatingState_attr_supportedMachineStates_send(caps_ovenOperatingState_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedMachineStates_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string_array((char *)caps_helper_ovenOperatingState.attr_supportedMachineStates.name,
        caps_data->supportedMachineStates_arraySize, caps_data->supportedMachineStates_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send supportedMachineStates value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static int caps_ovenOperatingState_get_progress_value(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_ovenOperatingState.attr_progress.min - 1;
    }
    return caps_data->progress_value;
}

static void caps_ovenOperatingState_set_progress_value(caps_ovenOperatingState_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->progress_value = value;
}

static const char *caps_ovenOperatingState_get_progress_unit(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->progress_unit;
}

static void caps_ovenOperatingState_set_progress_unit(caps_ovenOperatingState_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->progress_unit = (char *)unit;
}

static void caps_ovenOperatingState_attr_progress_send(caps_ovenOperatingState_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    cap_evt = st_cap_attr_create_int((char *) caps_helper_ovenOperatingState.attr_progress.name, caps_data->progress_value, caps_data->progress_unit);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send progress value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static int caps_ovenOperatingState_get_operationTime_value(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_ovenOperatingState.attr_operationTime.min - 1;
    }
    return caps_data->operationTime_value;
}

static void caps_ovenOperatingState_set_operationTime_value(caps_ovenOperatingState_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->operationTime_value = value;
}

static void caps_ovenOperatingState_attr_operationTime_send(caps_ovenOperatingState_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    cap_evt = st_cap_attr_create_int((char *) caps_helper_ovenOperatingState.attr_operationTime.name, caps_data->operationTime_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send operationTime value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static int caps_ovenOperatingState_attr_machineState_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_OVENOPERATINGSTATE_MACHINESTATE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_ovenOperatingState.attr_machineState.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_ovenOperatingState_get_machineState_value(caps_ovenOperatingState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->machineState_value;
}

static void caps_ovenOperatingState_set_machineState_value(caps_ovenOperatingState_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->machineState_value) {
        free(caps_data->machineState_value);
    }
    caps_data->machineState_value = strdup(value);
}

static void caps_ovenOperatingState_attr_machineState_send(caps_ovenOperatingState_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->machineState_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string((char *)caps_helper_ovenOperatingState.attr_machineState.name,
        caps_data->machineState_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send machineState value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static void caps_ovenOperatingState_cmd_start_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_ovenOperatingState_data_t *caps_data = usr_data;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    if (caps_data && caps_data->cmd_start_usr_cb)
        caps_data->cmd_start_usr_cb(caps_data);
}

static void caps_ovenOperatingState_cmd_stop_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_ovenOperatingState_data_t *caps_data = (caps_ovenOperatingState_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_stop_usr_cb)
        caps_data->cmd_stop_usr_cb(caps_data);
}

static void caps_ovenOperatingState_cmd_setMachineState_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_ovenOperatingState_data_t *caps_data = usr_data;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    if (caps_data && caps_data->cmd_setMachineState_usr_cb)
        caps_data->cmd_setMachineState_usr_cb(caps_data);
}

static void caps_ovenOperatingState_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_ovenOperatingState_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_ovenOperatingState_attr_ovenJobState_send(caps_data);
    caps_ovenOperatingState_attr_completionTime_send(caps_data);
    caps_ovenOperatingState_attr_supportedMachineStates_send(caps_data);
    caps_ovenOperatingState_attr_progress_send(caps_data);
    caps_ovenOperatingState_attr_operationTime_send(caps_data);
    caps_ovenOperatingState_attr_machineState_send(caps_data);
}

caps_ovenOperatingState_data_t *caps_ovenOperatingState_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_ovenOperatingState_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_ovenOperatingState_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_ovenOperatingState_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_ovenOperatingState_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_ovenJobState_value = caps_ovenOperatingState_get_ovenJobState_value;
    caps_data->set_ovenJobState_value = caps_ovenOperatingState_set_ovenJobState_value;
    caps_data->attr_ovenJobState_str2idx = caps_ovenOperatingState_attr_ovenJobState_str2idx;
    caps_data->attr_ovenJobState_send = caps_ovenOperatingState_attr_ovenJobState_send;
    caps_data->get_completionTime_value = caps_ovenOperatingState_get_completionTime_value;
    caps_data->set_completionTime_value = caps_ovenOperatingState_set_completionTime_value;
    caps_data->attr_completionTime_send = caps_ovenOperatingState_attr_completionTime_send;
    caps_data->get_supportedMachineStates_value = caps_ovenOperatingState_get_supportedMachineStates_value;
    caps_data->set_supportedMachineStates_value = caps_ovenOperatingState_set_supportedMachineStates_value;
    caps_data->attr_supportedMachineStates_send = caps_ovenOperatingState_attr_supportedMachineStates_send;
    caps_data->get_progress_value = caps_ovenOperatingState_get_progress_value;
    caps_data->set_progress_value = caps_ovenOperatingState_set_progress_value;
    caps_data->get_progress_unit = caps_ovenOperatingState_get_progress_unit;
    caps_data->set_progress_unit = caps_ovenOperatingState_set_progress_unit;
    caps_data->attr_progress_send = caps_ovenOperatingState_attr_progress_send;
    caps_data->get_operationTime_value = caps_ovenOperatingState_get_operationTime_value;
    caps_data->set_operationTime_value = caps_ovenOperatingState_set_operationTime_value;
    caps_data->attr_operationTime_send = caps_ovenOperatingState_attr_operationTime_send;
    caps_data->get_machineState_value = caps_ovenOperatingState_get_machineState_value;
    caps_data->set_machineState_value = caps_ovenOperatingState_set_machineState_value;
    caps_data->attr_machineState_str2idx = caps_ovenOperatingState_attr_machineState_str2idx;
    caps_data->attr_machineState_send = caps_ovenOperatingState_attr_machineState_send;
    caps_data->progress_value = 0;
    caps_data->operationTime_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_ovenOperatingState.id, caps_ovenOperatingState_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_ovenOperatingState.cmd_start.name, caps_ovenOperatingState_cmd_start_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for start of ovenOperatingState\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_ovenOperatingState.cmd_stop.name, caps_ovenOperatingState_cmd_stop_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for stop of ovenOperatingState\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_ovenOperatingState.cmd_setMachineState.name, caps_ovenOperatingState_cmd_setMachineState_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setMachineState of ovenOperatingState\n");
    }
    } else {
        printf("fail to init ovenOperatingState handle\n");
    }

    return caps_data;
}
