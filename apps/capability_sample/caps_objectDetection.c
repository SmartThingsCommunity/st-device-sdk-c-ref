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
#include "caps_objectDetection.h"

static const JSON_H *caps_objectDetection_get_detected_value(caps_objectDetection_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const JSON_H *)caps_data->detected_value;
}

static void caps_objectDetection_set_detected_value(caps_objectDetection_data_t *caps_data, const JSON_H *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->detected_value) {
        JSON_DELETE(caps_data->detected_value);
    }
    caps_data->detected_value = JSON_DUPLICATE(value, true);
}

static void caps_objectDetection_attr_detected_send(caps_objectDetection_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no = -1;
    iot_cap_val_t value;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->detected_value) {
        printf("value is NULL\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_JSON_OBJECT;
    value.json_object = JSON_PRINT(caps_data->detected_value);

    cap_evt = st_cap_create_attr(caps_data->handle,
        (char *)caps_helper_objectDetection.attr_detected.name,
        &value,
        NULL,
        NULL);

    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_send_attr(&cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send detected value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt);
}


static const char **caps_objectDetection_get_supportedValues_value(caps_objectDetection_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedValues_value;
}

static void caps_objectDetection_set_supportedValues_value(caps_objectDetection_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedValues_value) {
        for (i = 0; i < caps_data->supportedValues_arraySize; i++) {
            free(caps_data->supportedValues_value[i]);
        }
        free(caps_data->supportedValues_value);
    }
    caps_data->supportedValues_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedValues_value) {
        printf("fail to malloc for supportedValues_value\n");
        caps_data->supportedValues_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedValues_value[i] = strdup(value[i]);
    }

    caps_data->supportedValues_arraySize = arraySize;
}

static void caps_objectDetection_attr_supportedValues_send(caps_objectDetection_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedValues_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_objectDetection.attr_supportedValues.name,
            caps_data->supportedValues_value,
            caps_data->supportedValues_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedValues value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_objectDetection_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_objectDetection_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_objectDetection_attr_detected_send(caps_data);
    caps_objectDetection_attr_supportedValues_send(caps_data);
}

caps_objectDetection_data_t *caps_objectDetection_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_objectDetection_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_objectDetection_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_objectDetection_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_objectDetection_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_detected_value = caps_objectDetection_get_detected_value;
    caps_data->set_detected_value = caps_objectDetection_set_detected_value;
    caps_data->attr_detected_send = caps_objectDetection_attr_detected_send;
    caps_data->get_supportedValues_value = caps_objectDetection_get_supportedValues_value;
    caps_data->set_supportedValues_value = caps_objectDetection_set_supportedValues_value;
    caps_data->attr_supportedValues_send = caps_objectDetection_attr_supportedValues_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_objectDetection.id, caps_objectDetection_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init objectDetection handle\n");
    }

    return caps_data;
}
