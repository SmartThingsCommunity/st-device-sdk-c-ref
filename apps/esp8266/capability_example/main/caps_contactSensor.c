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

#include "st_dev.h"
#include "caps_contactSensor.h"

#include "freertos/FreeRTOS.h"

static int caps_contactSensor_attr_contact_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_CONTACTSENSOR_CONTACT_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_contactSensor.attr_contact.values[index])) {
            return index;
        }
    }   
    return -1; 
}

static const char *caps_contactSensor_get_contact_value(caps_contactSensor_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->contact_value;
}

static void caps_contactSensor_set_contact_value(caps_contactSensor_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->contact_value) {
        free(caps_data->contact_value);
    }
    caps_data->contact_value = strdup(value);
}

static void caps_contactSensor_attr_contact_send(caps_contactSensor_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->contact_value) {
        printf("value is NULL\n");
        return;
    }

    cap_evt = st_cap_attr_create_string((char *)caps_helper_contactSensor.attr_contact.name,
        caps_data->contact_value, NULL);
    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
    if (sequence_no < 0)
        printf("fail to send contact value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_attr_free(cap_evt);
}


static void caps_contactSensor_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_contactSensor_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_contactSensor_attr_contact_send(caps_data);
}

caps_contactSensor_data_t *caps_contactSensor_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_contactSensor_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_contactSensor_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_contactSensor_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_contactSensor_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_contact_value = caps_contactSensor_get_contact_value;
    caps_data->set_contact_value = caps_contactSensor_set_contact_value;
    caps_data->attr_contact_str2idx = caps_contactSensor_attr_contact_str2idx;
    caps_data->attr_contact_send = caps_contactSensor_attr_contact_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_contactSensor.id, caps_contactSensor_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init contactSensor handle\n");
    }

    return caps_data;
}
