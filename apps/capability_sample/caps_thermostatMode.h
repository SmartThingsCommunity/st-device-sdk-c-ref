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

#include "caps/iot_caps_helper_thermostatMode.h"
#include "external/JSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_thermostatMode_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    char *thermostatMode_value;
    char **supportedThermostatModes_value;
    int supportedThermostatModes_arraySize;

    const char *(*get_thermostatMode_value)(struct caps_thermostatMode_data *caps_data);
    void (*set_thermostatMode_value)(struct caps_thermostatMode_data *caps_data, const char *value);
    int (*attr_thermostatMode_str2idx)(const char *value);
    void (*attr_thermostatMode_send)(struct caps_thermostatMode_data *caps_data);
    const char **(*get_supportedThermostatModes_value)(struct caps_thermostatMode_data *caps_data);
    void (*set_supportedThermostatModes_value)(struct caps_thermostatMode_data *caps_data, const char **value, int arraySize);
    void (*attr_supportedThermostatModes_send)(struct caps_thermostatMode_data *caps_data);

    void (*init_usr_cb)(struct caps_thermostatMode_data *caps_data);

    void (*cmd_heat_usr_cb)(struct caps_thermostatMode_data *caps_data);
    void (*cmd_emergencyHeat_usr_cb)(struct caps_thermostatMode_data *caps_data);
    void (*cmd_auto_usr_cb)(struct caps_thermostatMode_data *caps_data);
    void (*cmd_cool_usr_cb)(struct caps_thermostatMode_data *caps_data);
    void (*cmd_off_usr_cb)(struct caps_thermostatMode_data *caps_data);
    void (*cmd_setThermostatMode_usr_cb)(struct caps_thermostatMode_data *caps_data);
} caps_thermostatMode_data_t;

caps_thermostatMode_data_t *caps_thermostatMode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

