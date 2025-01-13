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

#include "caps/iot_caps_helper_fanOscillationMode.h"
#include "external/JSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_fanOscillationMode_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    char **supportedFanOscillationModes_value;
    int supportedFanOscillationModes_arraySize;
    char *fanOscillationMode_value;

    const char **(*get_supportedFanOscillationModes_value)(struct caps_fanOscillationMode_data *caps_data);
    void (*set_supportedFanOscillationModes_value)(struct caps_fanOscillationMode_data *caps_data, const char **value, int arraySize);
    void (*attr_supportedFanOscillationModes_send)(struct caps_fanOscillationMode_data *caps_data);
    const char *(*get_fanOscillationMode_value)(struct caps_fanOscillationMode_data *caps_data);
    void (*set_fanOscillationMode_value)(struct caps_fanOscillationMode_data *caps_data, const char *value);
    int (*attr_fanOscillationMode_str2idx)(const char *value);
    void (*attr_fanOscillationMode_send)(struct caps_fanOscillationMode_data *caps_data);

    void (*init_usr_cb)(struct caps_fanOscillationMode_data *caps_data);

    void (*cmd_setFanOscillationMode_usr_cb)(struct caps_fanOscillationMode_data *caps_data);
} caps_fanOscillationMode_data_t;

caps_fanOscillationMode_data_t *caps_fanOscillationMode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

