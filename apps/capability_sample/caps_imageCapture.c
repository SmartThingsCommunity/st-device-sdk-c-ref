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
#include "caps_imageCapture.h"

static bool caps_imageCapture_get_encrypted_value(caps_imageCapture_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return false;
    }
    return caps_data->encrypted_value;
}

static void caps_imageCapture_set_encrypted_value(caps_imageCapture_data_t *caps_data, bool value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->encrypted_value = value;
}

static void caps_imageCapture_attr_encrypted_send(caps_imageCapture_data_t *caps_data)
{
    IOT_EVENT *cap_evt = NULL;
    uint8_t evt_num = 1;
    iot_cap_val_t value;
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_BOOLEAN;
    value.boolean = caps_data->encrypted_value;

    cap_evt = st_cap_create_attr(caps_data->handle,
            (char *)caps_helper_imageCapture.attr_encrypted.name,
            &value,
            NULL,
            NULL);

    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_send_attr(&cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send encrypted value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt);
}


static const char *caps_imageCapture_get_image_value(caps_imageCapture_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->image_value;
}

static void caps_imageCapture_set_image_value(caps_imageCapture_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->image_value) {
        free(caps_data->image_value);
    }
    caps_data->image_value = strdup(value);
}

static void caps_imageCapture_attr_image_send(caps_imageCapture_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->image_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_imageCapture.attr_image.name,
            caps_data->image_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send image value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static const char *caps_imageCapture_get_captureTime_value(caps_imageCapture_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->captureTime_value;
}

static void caps_imageCapture_set_captureTime_value(caps_imageCapture_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->captureTime_value) {
        free(caps_data->captureTime_value);
    }
    caps_data->captureTime_value = strdup(value);
}

static void caps_imageCapture_attr_captureTime_send(caps_imageCapture_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->captureTime_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_imageCapture.attr_captureTime.name,
            caps_data->captureTime_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send captureTime value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_imageCapture_cmd_take_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_imageCapture_data_t *caps_data = usr_data;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    if (caps_data && caps_data->cmd_take_usr_cb)
        caps_data->cmd_take_usr_cb(caps_data);
}

static void caps_imageCapture_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_imageCapture_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_imageCapture_attr_encrypted_send(caps_data);
    caps_imageCapture_attr_image_send(caps_data);
    caps_imageCapture_attr_captureTime_send(caps_data);
}

caps_imageCapture_data_t *caps_imageCapture_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_imageCapture_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_imageCapture_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_imageCapture_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_imageCapture_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_encrypted_value = caps_imageCapture_get_encrypted_value;
    caps_data->set_encrypted_value = caps_imageCapture_set_encrypted_value;
    caps_data->attr_encrypted_send = caps_imageCapture_attr_encrypted_send;
    caps_data->get_image_value = caps_imageCapture_get_image_value;
    caps_data->set_image_value = caps_imageCapture_set_image_value;
    caps_data->attr_image_send = caps_imageCapture_attr_image_send;
    caps_data->get_captureTime_value = caps_imageCapture_get_captureTime_value;
    caps_data->set_captureTime_value = caps_imageCapture_set_captureTime_value;
    caps_data->attr_captureTime_send = caps_imageCapture_attr_captureTime_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_imageCapture.id, caps_imageCapture_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_imageCapture.cmd_take.name, caps_imageCapture_cmd_take_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for take of imageCapture\n");
    }
    } else {
        printf("fail to init imageCapture handle\n");
    }

    return caps_data;
}
