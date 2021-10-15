/* ***************************************************************************
 *
 * Copyright 2019-2021 Samsung Electronics All Rights Reserved.
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
#include "caps_timedSession.h"

static const char *caps_timedSession_get_completionTime_value(caps_timedSession_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->completionTime_value;
}

static void caps_timedSession_set_completionTime_value(caps_timedSession_data_t *caps_data, const char *value)
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

static void caps_timedSession_attr_completionTime_send(caps_timedSession_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->completionTime_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_timedSession.attr_completionTime.name,
            caps_data->completionTime_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send completionTime value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static int caps_timedSession_attr_sessionStatus_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_TIMEDSESSION_SESSIONSTATUS_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_timedSession.attr_sessionStatus.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_timedSession_get_sessionStatus_value(caps_timedSession_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->sessionStatus_value;
}

static void caps_timedSession_set_sessionStatus_value(caps_timedSession_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->sessionStatus_value) {
        free(caps_data->sessionStatus_value);
    }
    caps_data->sessionStatus_value = strdup(value);
}

static void caps_timedSession_attr_sessionStatus_send(caps_timedSession_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->sessionStatus_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_timedSession.attr_sessionStatus.name,
            caps_data->sessionStatus_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send sessionStatus value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_timedSession_cmd_cancel_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_timedSession_data_t *caps_data = (caps_timedSession_data_t *)usr_data;
    const char* value = caps_helper_timedSession.attr_sessionStatus.values[CAP_ENUM_TIMEDSESSION_SESSIONSTATUS_VALUE_CANCELED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_timedSession_set_sessionStatus_value(caps_data, value);
    if (caps_data && caps_data->cmd_cancel_usr_cb)
        caps_data->cmd_cancel_usr_cb(caps_data);
    caps_timedSession_attr_sessionStatus_send(caps_data);
}

static void caps_timedSession_cmd_start_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_timedSession_data_t *caps_data = (caps_timedSession_data_t *)usr_data;
    const char* value = caps_helper_timedSession.attr_sessionStatus.values[CAP_ENUM_TIMEDSESSION_SESSIONSTATUS_VALUE_RUNNING];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_timedSession_set_sessionStatus_value(caps_data, value);
    if (caps_data && caps_data->cmd_start_usr_cb)
        caps_data->cmd_start_usr_cb(caps_data);
    caps_timedSession_attr_sessionStatus_send(caps_data);
}

static void caps_timedSession_cmd_setCompletionTime_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_timedSession_data_t *caps_data = (caps_timedSession_data_t *)usr_data;
    char *value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].string;

    caps_timedSession_set_completionTime_value(caps_data, value);
    if (caps_data && caps_data->cmd_setCompletionTime_usr_cb)
        caps_data->cmd_setCompletionTime_usr_cb(caps_data);
    caps_timedSession_attr_completionTime_send(caps_data);
}

static void caps_timedSession_cmd_pause_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_timedSession_data_t *caps_data = (caps_timedSession_data_t *)usr_data;
    const char* value = caps_helper_timedSession.attr_sessionStatus.values[CAP_ENUM_TIMEDSESSION_SESSIONSTATUS_VALUE_PAUSED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_timedSession_set_sessionStatus_value(caps_data, value);
    if (caps_data && caps_data->cmd_pause_usr_cb)
        caps_data->cmd_pause_usr_cb(caps_data);
    caps_timedSession_attr_sessionStatus_send(caps_data);
}

static void caps_timedSession_cmd_stop_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_timedSession_data_t *caps_data = (caps_timedSession_data_t *)usr_data;
    const char* value = caps_helper_timedSession.attr_sessionStatus.values[CAP_ENUM_TIMEDSESSION_SESSIONSTATUS_VALUE_STOPPED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_timedSession_set_sessionStatus_value(caps_data, value);
    if (caps_data && caps_data->cmd_stop_usr_cb)
        caps_data->cmd_stop_usr_cb(caps_data);
    caps_timedSession_attr_sessionStatus_send(caps_data);
}

static void caps_timedSession_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_timedSession_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_timedSession_attr_completionTime_send(caps_data);
    caps_timedSession_attr_sessionStatus_send(caps_data);
}

caps_timedSession_data_t *caps_timedSession_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_timedSession_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_timedSession_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_timedSession_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_timedSession_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_completionTime_value = caps_timedSession_get_completionTime_value;
    caps_data->set_completionTime_value = caps_timedSession_set_completionTime_value;
    caps_data->attr_completionTime_send = caps_timedSession_attr_completionTime_send;
    caps_data->get_sessionStatus_value = caps_timedSession_get_sessionStatus_value;
    caps_data->set_sessionStatus_value = caps_timedSession_set_sessionStatus_value;
    caps_data->attr_sessionStatus_str2idx = caps_timedSession_attr_sessionStatus_str2idx;
    caps_data->attr_sessionStatus_send = caps_timedSession_attr_sessionStatus_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_timedSession.id, caps_timedSession_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_timedSession.cmd_cancel.name, caps_timedSession_cmd_cancel_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for cancel of timedSession\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_timedSession.cmd_start.name, caps_timedSession_cmd_start_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for start of timedSession\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_timedSession.cmd_setCompletionTime.name, caps_timedSession_cmd_setCompletionTime_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setCompletionTime of timedSession\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_timedSession.cmd_pause.name, caps_timedSession_cmd_pause_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for pause of timedSession\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_timedSession.cmd_stop.name, caps_timedSession_cmd_stop_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for stop of timedSession\n");
    }
    } else {
        printf("fail to init timedSession handle\n");
    }

    return caps_data;
}
