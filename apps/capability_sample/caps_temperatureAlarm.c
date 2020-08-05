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
#include "caps_temperatureAlarm.h"

static int caps_temperatureAlarm_attr_temperatureAlarm_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_TEMPERATUREALARM_TEMPERATUREALARM_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_temperatureAlarm.attr_temperatureAlarm.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_temperatureAlarm_get_temperatureAlarm_value(caps_temperatureAlarm_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->temperatureAlarm_value;
}

static void caps_temperatureAlarm_set_temperatureAlarm_value(caps_temperatureAlarm_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->temperatureAlarm_value) {
        free(caps_data->temperatureAlarm_value);
    }
    caps_data->temperatureAlarm_value = strdup(value);
}

static void caps_temperatureAlarm_attr_temperatureAlarm_send(caps_temperatureAlarm_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->temperatureAlarm_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_temperatureAlarm.attr_temperatureAlarm.name,
            caps_data->temperatureAlarm_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send temperatureAlarm value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_temperatureAlarm_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_temperatureAlarm_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_temperatureAlarm_attr_temperatureAlarm_send(caps_data);
}

caps_temperatureAlarm_data_t *caps_temperatureAlarm_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_temperatureAlarm_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_temperatureAlarm_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_temperatureAlarm_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_temperatureAlarm_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_temperatureAlarm_value = caps_temperatureAlarm_get_temperatureAlarm_value;
    caps_data->set_temperatureAlarm_value = caps_temperatureAlarm_set_temperatureAlarm_value;
    caps_data->attr_temperatureAlarm_str2idx = caps_temperatureAlarm_attr_temperatureAlarm_str2idx;
    caps_data->attr_temperatureAlarm_send = caps_temperatureAlarm_attr_temperatureAlarm_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_temperatureAlarm.id, caps_temperatureAlarm_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init temperatureAlarm handle\n");
    }

    return caps_data;
}
