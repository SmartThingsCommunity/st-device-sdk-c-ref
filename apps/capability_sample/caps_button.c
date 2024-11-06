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
#include "caps_button.h"

static const char **caps_button_get_supportedButtonValues_value(caps_button_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const char **)caps_data->supportedButtonValues_value;
}

static void caps_button_set_supportedButtonValues_value(caps_button_data_t *caps_data, const char **value, int arraySize)
{
    int i;
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->supportedButtonValues_value) {
        for (i = 0; i < caps_data->supportedButtonValues_arraySize; i++) {
            free(caps_data->supportedButtonValues_value[i]);
        }
        free(caps_data->supportedButtonValues_value);
    }
    caps_data->supportedButtonValues_value = malloc(sizeof(char *) * arraySize);
    if (!caps_data->supportedButtonValues_value) {
        printf("fail to malloc for supportedButtonValues_value\n");
        caps_data->supportedButtonValues_arraySize = 0;
        return;
    }
    for (i = 0; i < arraySize; i++) {
        caps_data->supportedButtonValues_value[i] = strdup(value[i]);
    }

    caps_data->supportedButtonValues_arraySize = arraySize;
}

static void caps_button_attr_supportedButtonValues_send(caps_button_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedButtonValues_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_button.attr_supportedButtonValues.name,
            caps_data->supportedButtonValues_value,
            caps_data->supportedButtonValues_arraySize,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedButtonValues value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static int caps_button_attr_button_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_BUTTON_BUTTON_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_button.attr_button.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_button_get_button_value(caps_button_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->button_value;
}

static void caps_button_set_button_value(caps_button_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->button_value) {
        free(caps_data->button_value);
    }
    caps_data->button_value = strdup(value);
}

static void caps_button_attr_button_send(caps_button_data_t *caps_data)
{
    int sequence_no = -1;
    IOT_EVENT *attr = NULL;
    iot_cap_val_t value;
    iot_cap_attr_option_t attr_option = { 0 };

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->button_value) {
        printf("value is NULL\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_STRING;
    value.string = caps_data->button_value;

    attr_option.state_change = true;

    attr = st_cap_create_attr_with_option(caps_data->handle, (char *)caps_helper_button.attr_button.name, &value, NULL, NULL, &attr_option);
    if (attr != NULL){
        sequence_no = st_cap_send_attr(&attr, 1);
        st_cap_free_attr(attr);
    }

    if (sequence_no < 0)
        printf("fail to send button value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}

static int caps_button_get_numberOfButtons_value(caps_button_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_button.attr_numberOfButtons.min - 1;
    }
    return caps_data->numberOfButtons_value;
}

static void caps_button_set_numberOfButtons_value(caps_button_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->numberOfButtons_value = value;
}

static void caps_button_attr_numberOfButtons_send(caps_button_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_button.attr_numberOfButtons.name,
            caps_data->numberOfButtons_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send numberOfButtons value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_button_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_button_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_button_attr_supportedButtonValues_send(caps_data);
    caps_button_attr_button_send(caps_data);
    caps_button_attr_numberOfButtons_send(caps_data);
}

caps_button_data_t *caps_button_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_button_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_button_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_button_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_button_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_supportedButtonValues_value = caps_button_get_supportedButtonValues_value;
    caps_data->set_supportedButtonValues_value = caps_button_set_supportedButtonValues_value;
    caps_data->attr_supportedButtonValues_send = caps_button_attr_supportedButtonValues_send;
    caps_data->get_button_value = caps_button_get_button_value;
    caps_data->set_button_value = caps_button_set_button_value;
    caps_data->attr_button_str2idx = caps_button_attr_button_str2idx;
    caps_data->attr_button_send = caps_button_attr_button_send;
    caps_data->get_numberOfButtons_value = caps_button_get_numberOfButtons_value;
    caps_data->set_numberOfButtons_value = caps_button_set_numberOfButtons_value;
    caps_data->attr_numberOfButtons_send = caps_button_attr_numberOfButtons_send;
    caps_data->numberOfButtons_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_button.id, caps_button_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init button handle\n");
    }

    return caps_data;
}
