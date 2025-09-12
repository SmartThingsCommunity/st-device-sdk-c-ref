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
#include "caps_soundDetection.h"

static int caps_soundDetection_attr_soundDetected_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SOUNDDETECTION_SOUNDDETECTED_VALUE_MAX; index++) {
	if (!strcmp(value, caps_helper_soundDetection.attr_soundDetected.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_soundDetection_get_soundDetected_value(caps_soundDetection_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->soundDetected_value;
}

static void caps_soundDetection_set_soundDetected_value(caps_soundDetection_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->soundDetected_value) {
        free(caps_data->soundDetected_value);
    }
    caps_data->soundDetected_value = strdup(value);
}

static void caps_soundDetection_attr_soundDetected_send(caps_soundDetection_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->soundDetected_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_soundDetection.attr_soundDetected.name,
            caps_data->soundDetected_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send soundDetected value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}

static int caps_soundDetection_attr_soundDetectionState_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SOUNDDETECTION_SOUNDDETECTIONSTATE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_soundDetection.attr_soundDetectionState.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_soundDetection_get_soundDetectionState_value(caps_soundDetection_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->soundDetectionState_value;
}

static void caps_soundDetection_set_soundDetectionState_value(caps_soundDetection_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->soundDetectionState_value) {
        free(caps_data->soundDetectionState_value);
    }
    caps_data->soundDetectionState_value = strdup(value);
}

static void caps_soundDetection_attr_soundDetectionState_send(caps_soundDetection_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->soundDetectionState_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_soundDetection.attr_soundDetectionState.name,
            caps_data->soundDetectionState_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send soundDetectionState value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}

static const char **caps_soundDetection_get_supportedSoundTypes_value(caps_soundDetection_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedSoundTypes_value;
}

static void caps_soundDetection_set_supportedSoundTypes_value(caps_soundDetection_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedSoundTypes_value) {
        for (i = 0; i < caps_data->supportedSoundTypes_arraySize; i++) {
            free(caps_data->supportedSoundTypes_value[i]);
        }
        free(caps_data->supportedSoundTypes_value);
    }
    caps_data->supportedSoundTypes_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedSoundTypes_value) {
        printf("fail to malloc for supportedSoundTypes_value\n");
        caps_data->supportedSoundTypes_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedSoundTypes_value[i] = strdup(value[i]);
    }

    caps_data->supportedSoundTypes_arraySize = arraySize;
}

static void caps_soundDetection_attr_supportedSoundTypes_send(caps_soundDetection_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedSoundTypes_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_soundDetection.attr_supportedSoundTypes.name,
            caps_data->supportedSoundTypes_value,
            caps_data->supportedSoundTypes_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedSoundTypes value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static void caps_soundDetection_cmd_enableSoundDetection_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_soundDetection_data_t *caps_data = usr_data;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    if (caps_data && caps_data->cmd_enableSoundDetection_usr_cb)
        caps_data->cmd_enableSoundDetection_usr_cb(caps_data);
}

static void caps_soundDetection_cmd_disableSoundDetection_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_soundDetection_data_t *caps_data = (caps_soundDetection_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_disableSoundDetection_usr_cb)
        caps_data->cmd_disableSoundDetection_usr_cb(caps_data);
}

static void caps_soundDetection_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_soundDetection_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_soundDetection_attr_soundDetected_send(caps_data);
    caps_soundDetection_attr_soundDetectionState_send(caps_data);
    caps_soundDetection_attr_supportedSoundTypes_send(caps_data);
}

caps_soundDetection_data_t *caps_soundDetection_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_soundDetection_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_soundDetection_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_soundDetection_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_soundDetection_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_soundDetected_value = caps_soundDetection_get_soundDetected_value;
    caps_data->set_soundDetected_value = caps_soundDetection_set_soundDetected_value;
    caps_data->attr_soundDetected_str2idx = caps_soundDetection_attr_soundDetected_str2idx;
    caps_data->attr_soundDetected_send = caps_soundDetection_attr_soundDetected_send;
    caps_data->get_soundDetectionState_value = caps_soundDetection_get_soundDetectionState_value;
    caps_data->set_soundDetectionState_value = caps_soundDetection_set_soundDetectionState_value;
    caps_data->attr_soundDetectionState_str2idx = caps_soundDetection_attr_soundDetectionState_str2idx;
    caps_data->attr_soundDetectionState_send = caps_soundDetection_attr_soundDetectionState_send;
    caps_data->get_supportedSoundTypes_value = caps_soundDetection_get_supportedSoundTypes_value;
    caps_data->set_supportedSoundTypes_value = caps_soundDetection_set_supportedSoundTypes_value;
    caps_data->attr_supportedSoundTypes_send = caps_soundDetection_attr_supportedSoundTypes_send;

    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_soundDetection.id, caps_soundDetection_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_soundDetection.cmd_enableSoundDetection.name, caps_soundDetection_cmd_enableSoundDetection_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for enableSoundDetection of soundDetection\n");
        }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_soundDetection.cmd_disableSoundDetection.name, caps_soundDetection_cmd_disableSoundDetection_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for disableSoundDetection of soundDetection\n");
        }
    } else {
        printf("fail to init soundDetection handle\n");
    }

    return caps_data;
}

