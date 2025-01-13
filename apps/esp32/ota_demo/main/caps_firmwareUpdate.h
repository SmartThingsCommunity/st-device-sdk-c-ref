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

#include "caps/iot_caps_helper_firmwareUpdate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_firmwareUpdate_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    char *lastUpdateStatus_value;
    char *state_value;
    char *currentVersion_value;
    char *lastUpdateTime_value;
    char *availableVersion_value;
    char *lastUpdateStatusReason_value;

    const char *(*get_lastUpdateStatus_value)(struct caps_firmwareUpdate_data *caps_data);
    void (*set_lastUpdateStatus_value)(struct caps_firmwareUpdate_data *caps_data, const char *value);
    int (*attr_lastUpdateStatus_str2idx)(const char *value);
    void (*attr_lastUpdateStatus_send)(struct caps_firmwareUpdate_data *caps_data);
    const char *(*get_state_value)(struct caps_firmwareUpdate_data *caps_data);
    void (*set_state_value)(struct caps_firmwareUpdate_data *caps_data, const char *value);
    int (*attr_state_str2idx)(const char *value);
    void (*attr_state_send)(struct caps_firmwareUpdate_data *caps_data);
    const char *(*get_currentVersion_value)(struct caps_firmwareUpdate_data *caps_data);
    void (*set_currentVersion_value)(struct caps_firmwareUpdate_data *caps_data, const char *value);
    void (*attr_currentVersion_send)(struct caps_firmwareUpdate_data *caps_data);
    const char *(*get_lastUpdateTime_value)(struct caps_firmwareUpdate_data *caps_data);
    void (*set_lastUpdateTime_value)(struct caps_firmwareUpdate_data *caps_data, const char *value);
    void (*attr_lastUpdateTime_send)(struct caps_firmwareUpdate_data *caps_data);
    const char *(*get_availableVersion_value)(struct caps_firmwareUpdate_data *caps_data);
    void (*set_availableVersion_value)(struct caps_firmwareUpdate_data *caps_data, const char *value);
    void (*attr_availableVersion_send)(struct caps_firmwareUpdate_data *caps_data);
    const char *(*get_lastUpdateStatusReason_value)(struct caps_firmwareUpdate_data *caps_data);
    void (*set_lastUpdateStatusReason_value)(struct caps_firmwareUpdate_data *caps_data, const char *value);
    void (*attr_lastUpdateStatusReason_send)(struct caps_firmwareUpdate_data *caps_data);

    void (*init_usr_cb)(struct caps_firmwareUpdate_data *caps_data);

    void (*cmd_checkForFirmwareUpdate_usr_cb)(struct caps_firmwareUpdate_data *caps_data);
    void (*cmd_updateFirmware_usr_cb)(struct caps_firmwareUpdate_data *caps_data);
} caps_firmwareUpdate_data_t;

caps_firmwareUpdate_data_t *caps_firmwareUpdate_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

