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
#include "caps_colorControl.h"

static void caps_colorControl_set_color_value(caps_colorControl_data_t *caps_data, double hue, double saturation)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }

    caps_data->hue_value = hue;
    caps_data->saturation_value = saturation;
}

static void caps_colorControl_attr_color_send(caps_colorControl_data_t *caps_data)
{
    IOT_EVENT *cap_evt[2];
    uint8_t evt_num = 2;
    int32_t sequence_no;
    iot_cap_val_t value[2];

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }


    value[0].type = IOT_CAP_VAL_TYPE_NUMBER;
    value[0].number = caps_data->hue_value;

    cap_evt[0] = st_cap_create_attr(caps_data->handle,
            (char *) caps_helper_colorControl.attr_hue.name,
            &value[0],
            NULL,
            NULL);

    value[1].type = IOT_CAP_VAL_TYPE_NUMBER;
    value[1].number = caps_data->saturation_value;

    cap_evt[1] = st_cap_create_attr(caps_data->handle,
            (char *) caps_helper_colorControl.attr_saturation.name,
            &value[1],
            NULL,
            NULL);

    if (!cap_evt[0] || !cap_evt[1]) {
        printf("fail to create cap_evt\n");
        free(cap_evt[0]);
        free(cap_evt[1]);
        return;
    }

    sequence_no = st_cap_send_attr(cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send color data\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt[0]);
    st_cap_free_attr(cap_evt[1]);
}


static double caps_colorControl_get_hue_value(caps_colorControl_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_colorControl.attr_hue.min - 1;
    }
    return caps_data->hue_value;
}

static void caps_colorControl_set_hue_value(caps_colorControl_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->hue_value = value;
}

static void caps_colorControl_attr_hue_send(caps_colorControl_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_colorControl.attr_hue.name,
            caps_data->hue_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send hue value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static double caps_colorControl_get_saturation_value(caps_colorControl_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_colorControl.attr_saturation.min - 1;
    }
    return caps_data->saturation_value;
}

static void caps_colorControl_set_saturation_value(caps_colorControl_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->saturation_value = value;
}

static void caps_colorControl_attr_saturation_send(caps_colorControl_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_colorControl.attr_saturation.name,
            caps_data->saturation_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send saturation value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_colorControl_cmd_setHue_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_colorControl_data_t *caps_data = (caps_colorControl_data_t *)usr_data;
    double value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].number;

    caps_colorControl_set_hue_value(caps_data, value);
    if (caps_data && caps_data->cmd_setHue_usr_cb)
        caps_data->cmd_setHue_usr_cb(caps_data);
    caps_colorControl_attr_hue_send(caps_data);
}

static void caps_colorControl_cmd_setColor_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_colorControl_data_t *caps_data = usr_data;
    JSON_H* json_object;
    JSON_H* item;
    double hue = caps_helper_colorControl.attr_hue.min - 1;
    double saturation = caps_helper_colorControl.attr_saturation.min - 1;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    json_object = JSON_PARSE(cmd_data->cmd_data[0].json_object);
    if (!json_object) {
        printf("fail to parse json object\n");
        return;
    }
    item = JSON_GET_OBJECT_ITEM(json_object, "hue");
    if (JSON_IS_NUMBER(item)) {
        hue = item->valuedouble;
    }
    item = JSON_GET_OBJECT_ITEM(json_object, "saturation");
    if (JSON_IS_NUMBER(item)) {
        saturation = item->valuedouble;
    }
    JSON_DELETE(json_object);

    caps_colorControl_set_color_value(caps_data, hue, saturation);
    if (caps_data && caps_data->cmd_setColor_usr_cb)
        caps_data->cmd_setColor_usr_cb(caps_data);
    caps_colorControl_attr_color_send(caps_data);
}

static void caps_colorControl_cmd_setSaturation_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_colorControl_data_t *caps_data = (caps_colorControl_data_t *)usr_data;
    double value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].number;

    caps_colorControl_set_saturation_value(caps_data, value);
    if (caps_data && caps_data->cmd_setSaturation_usr_cb)
        caps_data->cmd_setSaturation_usr_cb(caps_data);
    caps_colorControl_attr_saturation_send(caps_data);
}

static void caps_colorControl_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_colorControl_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_colorControl_attr_color_send(caps_data);
    caps_colorControl_attr_hue_send(caps_data);
    caps_colorControl_attr_saturation_send(caps_data);
}

caps_colorControl_data_t *caps_colorControl_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_colorControl_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_colorControl_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_colorControl_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_colorControl_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->set_color_value = caps_colorControl_set_color_value;
    caps_data->attr_color_send = caps_colorControl_attr_color_send;
    caps_data->get_hue_value = caps_colorControl_get_hue_value;
    caps_data->set_hue_value = caps_colorControl_set_hue_value;
    caps_data->attr_hue_send = caps_colorControl_attr_hue_send;
    caps_data->get_saturation_value = caps_colorControl_get_saturation_value;
    caps_data->set_saturation_value = caps_colorControl_set_saturation_value;
    caps_data->attr_saturation_send = caps_colorControl_attr_saturation_send;
    caps_data->hue_value = 0;
    caps_data->saturation_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_colorControl.id, caps_colorControl_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_colorControl.cmd_setHue.name, caps_colorControl_cmd_setHue_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setHue of colorControl\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_colorControl.cmd_setColor.name, caps_colorControl_cmd_setColor_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setColor of colorControl\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_colorControl.cmd_setSaturation.name, caps_colorControl_cmd_setSaturation_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setSaturation of colorControl\n");
    }
    } else {
        printf("fail to init colorControl handle\n");
    }

    return caps_data;
}
