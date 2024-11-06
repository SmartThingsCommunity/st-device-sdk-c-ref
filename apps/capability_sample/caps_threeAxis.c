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
#include "caps_threeAxis.h"

static const JSON_H *caps_threeAxis_get_threeAxis_value(caps_threeAxis_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }  
    return caps_data->threeAxis_value;
}

static void caps_threeAxis_set_threeAxis_value(caps_threeAxis_data_t *caps_data, int x, int y, int z)
{
    JSON_H *array = NULL;
    JSON_H *array_obj = NULL;

    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }

    array = JSON_CREATE_ARRAY();
    if (!array) {
        printf("fail to create json array\n");
        return;
    }
    array_obj = JSON_CREATE_NUMBER((double) x);
    JSON_ADD_ITEM_TO_ARRAY(array, array_obj);
    array_obj = JSON_CREATE_NUMBER((double) y);
    JSON_ADD_ITEM_TO_ARRAY(array, array_obj);
    array_obj = JSON_CREATE_NUMBER((double) z);
    JSON_ADD_ITEM_TO_ARRAY(array, array_obj);

    JSON_DELETE(caps_data->threeAxis_value);
    caps_data->threeAxis_value = array;
}

static void caps_threeAxis_attr_threeAxis_send(caps_threeAxis_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no = -1;
    iot_cap_val_t value;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    if (!caps_data->threeAxis_value) {
        printf("value is NULL\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_JSON_OBJECT;
    value.json_object = JSON_PRINT(caps_data->threeAxis_value);

    cap_evt = st_cap_create_attr(caps_data->handle,
            (char *)caps_helper_threeAxis.attr_threeAxis.name,
            &value,
            NULL,
            NULL);

    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_send_attr(&cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send threeAxis value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt);
}



static void caps_threeAxis_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_threeAxis_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_threeAxis_attr_threeAxis_send(caps_data);
}

caps_threeAxis_data_t *caps_threeAxis_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_threeAxis_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_threeAxis_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_threeAxis_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_threeAxis_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_threeAxis_value = caps_threeAxis_get_threeAxis_value;
    caps_data->set_threeAxis_value = caps_threeAxis_set_threeAxis_value;
    caps_data->attr_threeAxis_send = caps_threeAxis_attr_threeAxis_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_threeAxis.id, caps_threeAxis_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init threeAxis handle\n");
    }

    return caps_data;
}
