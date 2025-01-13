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
#include "caps_presenceSensor.h"

static int caps_presenceSensor_attr_presence_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_PRESENCESENSOR_PRESENCE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_presenceSensor.attr_presence.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_presenceSensor_get_presence_value(caps_presenceSensor_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->presence_value;
}

static void caps_presenceSensor_set_presence_value(caps_presenceSensor_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->presence_value) {
        free(caps_data->presence_value);
    }
    caps_data->presence_value = strdup(value);
}

static void caps_presenceSensor_attr_presence_send(caps_presenceSensor_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->presence_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_presenceSensor.attr_presence.name,
            caps_data->presence_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send presence value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_presenceSensor_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_presenceSensor_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_presenceSensor_attr_presence_send(caps_data);
}

caps_presenceSensor_data_t *caps_presenceSensor_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_presenceSensor_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_presenceSensor_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_presenceSensor_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_presenceSensor_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_presence_value = caps_presenceSensor_get_presence_value;
    caps_data->set_presence_value = caps_presenceSensor_set_presence_value;
    caps_data->attr_presence_str2idx = caps_presenceSensor_attr_presence_str2idx;
    caps_data->attr_presence_send = caps_presenceSensor_attr_presence_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_presenceSensor.id, caps_presenceSensor_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init presenceSensor handle\n");
    }

    return caps_data;
}
