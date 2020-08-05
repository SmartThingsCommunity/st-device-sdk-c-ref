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
#include "caps_samsungTV.h"

static int caps_samsungTV_get_volume_value(caps_samsungTV_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_samsungTV.attr_volume.min - 1;
    }
    return caps_data->volume_value;
}

static void caps_samsungTV_set_volume_value(caps_samsungTV_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->volume_value = value;
}

static void caps_samsungTV_attr_volume_send(caps_samsungTV_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_samsungTV.attr_volume.name,
            caps_data->volume_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send volume value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static const JSON_H *caps_samsungTV_get_messageButton_value(caps_samsungTV_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const JSON_H *)caps_data->messageButton_value;
}

static void caps_samsungTV_set_messageButton_value(caps_samsungTV_data_t *caps_data, const JSON_H *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->messageButton_value) {
        JSON_DELETE(caps_data->messageButton_value);
    }
    caps_data->messageButton_value = JSON_DUPLICATE(value, true);
}

static void caps_samsungTV_attr_messageButton_send(caps_samsungTV_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no = -1;
    iot_cap_val_t value;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->messageButton_value) {
        printf("value is NULL\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_JSON_OBJECT;
    value.json_object = JSON_PRINT(caps_data->messageButton_value);

    cap_evt = st_cap_create_attr(caps_data->handle,
        (char *)caps_helper_samsungTV.attr_messageButton.name,
        &value,
        NULL,
        NULL);

    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_send_attr(&cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send messageButton value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt);
}


static int caps_samsungTV_attr_switch_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SAMSUNGTV_SWITCH_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_samsungTV.attr_switch.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_samsungTV_get_switch_value(caps_samsungTV_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->switch_value;
}

static void caps_samsungTV_set_switch_value(caps_samsungTV_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->switch_value) {
        free(caps_data->switch_value);
    }
    caps_data->switch_value = strdup(value);
}

static void caps_samsungTV_attr_switch_send(caps_samsungTV_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->switch_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_samsungTV.attr_switch.name,
            caps_data->switch_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send switch value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static int caps_samsungTV_attr_mute_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SAMSUNGTV_MUTE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_samsungTV.attr_mute.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_samsungTV_get_mute_value(caps_samsungTV_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->mute_value;
}

static void caps_samsungTV_set_mute_value(caps_samsungTV_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->mute_value) {
        free(caps_data->mute_value);
    }
    caps_data->mute_value = strdup(value);
}

static void caps_samsungTV_attr_mute_send(caps_samsungTV_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->mute_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_samsungTV.attr_mute.name,
            caps_data->mute_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send mute value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static int caps_samsungTV_attr_pictureMode_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SAMSUNGTV_PICTUREMODE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_samsungTV.attr_pictureMode.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_samsungTV_get_pictureMode_value(caps_samsungTV_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->pictureMode_value;
}

static void caps_samsungTV_set_pictureMode_value(caps_samsungTV_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->pictureMode_value) {
        free(caps_data->pictureMode_value);
    }
    caps_data->pictureMode_value = strdup(value);
}

static void caps_samsungTV_attr_pictureMode_send(caps_samsungTV_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->pictureMode_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_samsungTV.attr_pictureMode.name,
            caps_data->pictureMode_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send pictureMode value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static int caps_samsungTV_attr_soundMode_str2idx(const char *value)
{
    int index;

    for (index = 0; index < CAP_ENUM_SAMSUNGTV_SOUNDMODE_VALUE_MAX; index++) {
        if (!strcmp(value, caps_helper_samsungTV.attr_soundMode.values[index])) {
            return index;
        }
    }
    return -1;
}

static const char *caps_samsungTV_get_soundMode_value(caps_samsungTV_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->soundMode_value;
}

static void caps_samsungTV_set_soundMode_value(caps_samsungTV_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->soundMode_value) {
        free(caps_data->soundMode_value);
    }
    caps_data->soundMode_value = strdup(value);
}

static void caps_samsungTV_attr_soundMode_send(caps_samsungTV_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->soundMode_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_samsungTV.attr_soundMode.name,
            caps_data->soundMode_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send soundMode value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);

}


static void caps_samsungTV_cmd_on_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    const char* value = caps_helper_samsungTV.attr_switch.values[CAP_ENUM_SAMSUNGTV_SWITCH_VALUE_ON];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_samsungTV_set_switch_value(caps_data, value);
    if (caps_data && caps_data->cmd_on_usr_cb)
        caps_data->cmd_on_usr_cb(caps_data);
    caps_samsungTV_attr_switch_send(caps_data);
}

static void caps_samsungTV_cmd_off_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    const char* value = caps_helper_samsungTV.attr_switch.values[CAP_ENUM_SAMSUNGTV_SWITCH_VALUE_OFF];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_samsungTV_set_switch_value(caps_data, value);
    if (caps_data && caps_data->cmd_off_usr_cb)
        caps_data->cmd_off_usr_cb(caps_data);
    caps_samsungTV_attr_switch_send(caps_data);
}

static void caps_samsungTV_cmd_mute_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    const char* value = caps_helper_samsungTV.attr_mute.values[CAP_ENUM_SAMSUNGTV_MUTE_VALUE_MUTED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_samsungTV_set_mute_value(caps_data, value);
    if (caps_data && caps_data->cmd_mute_usr_cb)
        caps_data->cmd_mute_usr_cb(caps_data);
    caps_samsungTV_attr_mute_send(caps_data);
}

static void caps_samsungTV_cmd_unmute_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    const char* value = caps_helper_samsungTV.attr_mute.values[CAP_ENUM_SAMSUNGTV_MUTE_VALUE_UNMUTED];

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    caps_samsungTV_set_mute_value(caps_data, value);
    if (caps_data && caps_data->cmd_unmute_usr_cb)
        caps_data->cmd_unmute_usr_cb(caps_data);
    caps_samsungTV_attr_mute_send(caps_data);
}

static void caps_samsungTV_cmd_setPictureMode_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    char *value;
    int index;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    index = caps_samsungTV_attr_pictureMode_str2idx(cmd_data->cmd_data[0].string);
    if (index < 0) {
        printf("%s is not supported value for setPictureMode\n", cmd_data->cmd_data[0].string);
        return;
    }
    value = (char *)caps_helper_samsungTV.attr_pictureMode.values[index];

    caps_samsungTV_set_pictureMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_setPictureMode_usr_cb)
        caps_data->cmd_setPictureMode_usr_cb(caps_data);
    caps_samsungTV_attr_pictureMode_send(caps_data);
}

static void caps_samsungTV_cmd_setSoundMode_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    char *value;
    int index;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    index = caps_samsungTV_attr_soundMode_str2idx(cmd_data->cmd_data[0].string);
    if (index < 0) {
        printf("%s is not supported value for setSoundMode\n", cmd_data->cmd_data[0].string);
        return;
    }
    value = (char *)caps_helper_samsungTV.attr_soundMode.values[index];

    caps_samsungTV_set_soundMode_value(caps_data, value);
    if (caps_data && caps_data->cmd_setSoundMode_usr_cb)
        caps_data->cmd_setSoundMode_usr_cb(caps_data);
    caps_samsungTV_attr_soundMode_send(caps_data);
}

static void caps_samsungTV_cmd_volumeDown_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_volumeDown_usr_cb)
        caps_data->cmd_volumeDown_usr_cb(caps_data);
}

static void caps_samsungTV_cmd_showMessage_cb(IOT_CAP_HANDLE *handle,
        iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = usr_data;

    printf("called [%s] func with : num_args:%u\n", __func__, cmd_data->num_args);
    caps_data->cmd_data = cmd_data;

    if (caps_data && caps_data->cmd_showMessage_usr_cb)
        caps_data->cmd_showMessage_usr_cb(caps_data);
}

static void caps_samsungTV_cmd_volumeUp_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_volumeUp_usr_cb)
        caps_data->cmd_volumeUp_usr_cb(caps_data);
}

static void caps_samsungTV_cmd_setVolume_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = (caps_samsungTV_data_t *)usr_data;
    int value;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    value = cmd_data->cmd_data[0].integer;

    caps_samsungTV_set_volume_value(caps_data, value);
    if (caps_data && caps_data->cmd_setVolume_usr_cb)
        caps_data->cmd_setVolume_usr_cb(caps_data);
    caps_samsungTV_attr_volume_send(caps_data);
}

static void caps_samsungTV_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_samsungTV_attr_volume_send(caps_data);
    caps_samsungTV_attr_messageButton_send(caps_data);
    caps_samsungTV_attr_switch_send(caps_data);
    caps_samsungTV_attr_mute_send(caps_data);
    caps_samsungTV_attr_pictureMode_send(caps_data);
    caps_samsungTV_attr_soundMode_send(caps_data);
}

caps_samsungTV_data_t *caps_samsungTV_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_samsungTV_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_samsungTV_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_samsungTV_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_samsungTV_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_volume_value = caps_samsungTV_get_volume_value;
    caps_data->set_volume_value = caps_samsungTV_set_volume_value;
    caps_data->attr_volume_send = caps_samsungTV_attr_volume_send;
    caps_data->get_messageButton_value = caps_samsungTV_get_messageButton_value;
    caps_data->set_messageButton_value = caps_samsungTV_set_messageButton_value;
    caps_data->attr_messageButton_send = caps_samsungTV_attr_messageButton_send;
    caps_data->get_switch_value = caps_samsungTV_get_switch_value;
    caps_data->set_switch_value = caps_samsungTV_set_switch_value;
    caps_data->attr_switch_str2idx = caps_samsungTV_attr_switch_str2idx;
    caps_data->attr_switch_send = caps_samsungTV_attr_switch_send;
    caps_data->get_mute_value = caps_samsungTV_get_mute_value;
    caps_data->set_mute_value = caps_samsungTV_set_mute_value;
    caps_data->attr_mute_str2idx = caps_samsungTV_attr_mute_str2idx;
    caps_data->attr_mute_send = caps_samsungTV_attr_mute_send;
    caps_data->get_pictureMode_value = caps_samsungTV_get_pictureMode_value;
    caps_data->set_pictureMode_value = caps_samsungTV_set_pictureMode_value;
    caps_data->attr_pictureMode_str2idx = caps_samsungTV_attr_pictureMode_str2idx;
    caps_data->attr_pictureMode_send = caps_samsungTV_attr_pictureMode_send;
    caps_data->get_soundMode_value = caps_samsungTV_get_soundMode_value;
    caps_data->set_soundMode_value = caps_samsungTV_set_soundMode_value;
    caps_data->attr_soundMode_str2idx = caps_samsungTV_attr_soundMode_str2idx;
    caps_data->attr_soundMode_send = caps_samsungTV_attr_soundMode_send;
    caps_data->volume_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_samsungTV.id, caps_samsungTV_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_on.name, caps_samsungTV_cmd_on_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for on of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_off.name, caps_samsungTV_cmd_off_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for off of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_mute.name, caps_samsungTV_cmd_mute_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for mute of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_unmute.name, caps_samsungTV_cmd_unmute_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for unmute of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_setPictureMode.name, caps_samsungTV_cmd_setPictureMode_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setPictureMode of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_setSoundMode.name, caps_samsungTV_cmd_setSoundMode_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setSoundMode of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_volumeDown.name, caps_samsungTV_cmd_volumeDown_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for volumeDown of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_showMessage.name, caps_samsungTV_cmd_showMessage_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for showMessage of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_volumeUp.name, caps_samsungTV_cmd_volumeUp_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for volumeUp of samsungTV\n");
    }
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_samsungTV.cmd_setVolume.name, caps_samsungTV_cmd_setVolume_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for setVolume of samsungTV\n");
    }
    } else {
        printf("fail to init samsungTV handle\n");
    }

    return caps_data;
}
