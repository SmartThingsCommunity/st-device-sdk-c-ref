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
#include "caps_antiSnoringPillow.h"

static int caps_antiSnoringPillow_attr_state_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_ANTISNORINGPILLOW_STATE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_antiSnoringPillow.attr_state.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_antiSnoringPillow_get_state_value(caps_antiSnoringPillow_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->state_value;
}

static void caps_antiSnoringPillow_set_state_value(caps_antiSnoringPillow_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->state_value) {
        free(caps_data->state_value);
    }
    caps_data->state_value = strdup(value);
}

static void caps_antiSnoringPillow_attr_state_send(caps_antiSnoringPillow_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->state_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_state.name,
            caps_data->state_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static int caps_antiSnoringPillow_get_snoringTime_value(struct caps_antiSnoringPillow_data *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return -1;
    }
    return caps_data->snoringTime_value;
}

static void caps_antiSnoringPillow_set_snoringTime_value(struct caps_antiSnoringPillow_data *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->snoringTime_value = value;
}

static void caps_antiSnoringPillow_attr_snoringTime_send(struct caps_antiSnoringPillow_data *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_snoringTime.name,
            caps_data->snoringTime_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static int caps_antiSnoringPillow_get_snoringTimeDelta_value(struct caps_antiSnoringPillow_data *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return -1;
    }
    return caps_data->snoringTimeDelta_value;
}

static void caps_antiSnoringPillow_set_snoringTimeDelta_value(struct caps_antiSnoringPillow_data *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->snoringTimeDelta_value = value;
}

static void caps_antiSnoringPillow_attr_snoringTimeDelta_send(struct caps_antiSnoringPillow_data *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_snoringTimeDelta.name,
            caps_data->snoringTimeDelta_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static bool caps_antiSnoringPillow_get_supportSnoringTimeGraph_value(struct caps_antiSnoringPillow_data *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return false;
    }
    return caps_data->supportSnoringTimeGraph_value;
}

static void caps_antiSnoringPillow_set_supportSnoringTimeGraph_value(struct caps_antiSnoringPillow_data *caps_data, bool value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->supportSnoringTimeGraph_value = value;
}

static void caps_antiSnoringPillow_attr_supportSnoringTimeGraph_send(struct caps_antiSnoringPillow_data *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_BOOLEAN(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_supportSnoringTimeGraph.name,
            caps_data->supportSnoringTimeGraph_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static int caps_antiSnoringPillow_get_pillowOperationTime_value(struct caps_antiSnoringPillow_data *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return -1;
    }
    return caps_data->pillowOperationTime_value;
}

static void caps_antiSnoringPillow_set_pillowOperationTime_value(struct caps_antiSnoringPillow_data *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->pillowOperationTime_value = value;
}

static void caps_antiSnoringPillow_attr_pillowOperationTime_send(struct caps_antiSnoringPillow_data *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_pillowOperationTime.name,
            caps_data->pillowOperationTime_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static int caps_antiSnoringPillow_get_pillowOperationTimeDelta_value(struct caps_antiSnoringPillow_data *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return -1;
    }
    return caps_data->pillowOperationTimeDelta_value;
}

static void caps_antiSnoringPillow_set_pillowOperationTimeDelta_value(struct caps_antiSnoringPillow_data *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->pillowOperationTimeDelta_value = value;
}

static void caps_antiSnoringPillow_attr_pillowOperationTimeDelta_send(struct caps_antiSnoringPillow_data *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_pillowOperationTimeDelta.name,
            caps_data->pillowOperationTimeDelta_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static bool caps_antiSnoringPillow_get_supportPillowOperationTimeGraph_value(struct caps_antiSnoringPillow_data *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return false;
    }
    return caps_data->supportPillowOperationTimeGraph_value;
}

static void caps_antiSnoringPillow_set_supportPillowOperationTimeGraph_value(struct caps_antiSnoringPillow_data *caps_data, bool value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->supportPillowOperationTimeGraph_value = value;
}

static void caps_antiSnoringPillow_attr_supportPillowOperationTimeGraph_send(struct caps_antiSnoringPillow_data *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_BOOLEAN(caps_data->handle,
            (char *)caps_helper_antiSnoringPillow.attr_supportPillowOperationTimeGraph.name,
            caps_data->supportPillowOperationTimeGraph_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send state value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static void caps_antiSnoringPillow_cmd_on_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_antiSnoringPillow_data_t *caps_data = (caps_antiSnoringPillow_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_on_usr_cb)
        caps_data->cmd_on_usr_cb(caps_data);
}

static void caps_antiSnoringPillow_cmd_off_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_antiSnoringPillow_data_t *caps_data = (caps_antiSnoringPillow_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_off_usr_cb)
        caps_data->cmd_off_usr_cb(caps_data);
}

static void caps_antiSnoringPillow_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_antiSnoringPillow_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_antiSnoringPillow_attr_state_send(caps_data);
}

caps_antiSnoringPillow_data_t *caps_antiSnoringPillow_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_antiSnoringPillow_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_antiSnoringPillow_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_antiSnoringPillow_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_antiSnoringPillow_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_state_value = caps_antiSnoringPillow_get_state_value;
    caps_data->set_state_value = caps_antiSnoringPillow_set_state_value;
    caps_data->attr_state_str2idx = caps_antiSnoringPillow_attr_state_str2idx;
    caps_data->attr_state_send = caps_antiSnoringPillow_attr_state_send;

    caps_data->get_snoringTime_value = caps_antiSnoringPillow_get_snoringTime_value;
    caps_data->set_snoringTime_value = caps_antiSnoringPillow_set_snoringTime_value;
    caps_data->attr_snoringTime_send = caps_antiSnoringPillow_attr_snoringTime_send;

    caps_data->get_snoringTimeDelta_value = caps_antiSnoringPillow_get_snoringTimeDelta_value;
    caps_data->set_snoringTimeDelta_value = caps_antiSnoringPillow_set_snoringTimeDelta_value;
    caps_data->attr_snoringTimeDelta_send = caps_antiSnoringPillow_attr_snoringTimeDelta_send;

    caps_data->get_supportSnoringTimeGraph_value = caps_antiSnoringPillow_get_supportSnoringTimeGraph_value;
    caps_data->set_supportSnoringTimeGraph_value = caps_antiSnoringPillow_set_supportSnoringTimeGraph_value;
    caps_data->attr_supportSnoringTimeGraph_send = caps_antiSnoringPillow_attr_supportSnoringTimeGraph_send;

    caps_data->get_pillowOperationTime_value = caps_antiSnoringPillow_get_pillowOperationTime_value;
    caps_data->set_pillowOperationTime_value = caps_antiSnoringPillow_set_pillowOperationTime_value;
    caps_data->attr_pillowOperationTime_send = caps_antiSnoringPillow_attr_pillowOperationTime_send;

    caps_data->get_pillowOperationTimeDelta_value = caps_antiSnoringPillow_get_pillowOperationTimeDelta_value;
    caps_data->set_pillowOperationTimeDelta_value = caps_antiSnoringPillow_set_pillowOperationTimeDelta_value;
    caps_data->attr_pillowOperationTimeDelta_send = caps_antiSnoringPillow_attr_pillowOperationTimeDelta_send;

    caps_data->get_supportPillowOperationTimeGraph_value = caps_antiSnoringPillow_get_supportPillowOperationTimeGraph_value;
    caps_data->set_supportPillowOperationTimeGraph_value = caps_antiSnoringPillow_set_supportPillowOperationTimeGraph_value;
    caps_data->attr_supportPillowOperationTimeGraph_send = caps_antiSnoringPillow_attr_supportPillowOperationTimeGraph_send;

    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_antiSnoringPillow.id, caps_antiSnoringPillow_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_antiSnoringPillow.cmd_on.name, caps_antiSnoringPillow_cmd_on_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for on of antiSnoringPillow\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_antiSnoringPillow.cmd_off.name, caps_antiSnoringPillow_cmd_off_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for off of antiSnoringPillow\n");
    }
    } else {
        printf("fail to init antiSnoringPillow handle\n");
    }

    return caps_data;
}
