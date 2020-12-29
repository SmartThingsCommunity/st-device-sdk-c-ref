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

#include "caps/iot_caps_helper_samsungTV.h"
#include "external/JSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_samsungTV_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    int volume_value;
    JSON_H *messageButton_value;
    char *switch_value;
    char *mute_value;
    char *pictureMode_value;
    char *soundMode_value;

    int (*get_volume_value)(struct caps_samsungTV_data *caps_data);
    void (*set_volume_value)(struct caps_samsungTV_data *caps_data, int value);
    void (*attr_volume_send)(struct caps_samsungTV_data *caps_data);
    const JSON_H *(*get_messageButton_value)(struct caps_samsungTV_data *caps_data);
    void (*set_messageButton_value)(struct caps_samsungTV_data *caps_data, const JSON_H *value);
    void (*attr_messageButton_send)(struct caps_samsungTV_data *caps_data);
    const char *(*get_switch_value)(struct caps_samsungTV_data *caps_data);
    void (*set_switch_value)(struct caps_samsungTV_data *caps_data, const char *value);
    int (*attr_switch_str2idx)(const char *value);
    void (*attr_switch_send)(struct caps_samsungTV_data *caps_data);
    const char *(*get_mute_value)(struct caps_samsungTV_data *caps_data);
    void (*set_mute_value)(struct caps_samsungTV_data *caps_data, const char *value);
    int (*attr_mute_str2idx)(const char *value);
    void (*attr_mute_send)(struct caps_samsungTV_data *caps_data);
    const char *(*get_pictureMode_value)(struct caps_samsungTV_data *caps_data);
    void (*set_pictureMode_value)(struct caps_samsungTV_data *caps_data, const char *value);
    int (*attr_pictureMode_str2idx)(const char *value);
    void (*attr_pictureMode_send)(struct caps_samsungTV_data *caps_data);
    const char *(*get_soundMode_value)(struct caps_samsungTV_data *caps_data);
    void (*set_soundMode_value)(struct caps_samsungTV_data *caps_data, const char *value);
    int (*attr_soundMode_str2idx)(const char *value);
    void (*attr_soundMode_send)(struct caps_samsungTV_data *caps_data);

    void (*init_usr_cb)(struct caps_samsungTV_data *caps_data);

    void (*cmd_on_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_off_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_mute_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_unmute_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_setPictureMode_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_setSoundMode_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_volumeDown_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_showMessage_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_volumeUp_usr_cb)(struct caps_samsungTV_data *caps_data);
    void (*cmd_setVolume_usr_cb)(struct caps_samsungTV_data *caps_data);
} caps_samsungTV_data_t;

caps_samsungTV_data_t *caps_samsungTV_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

