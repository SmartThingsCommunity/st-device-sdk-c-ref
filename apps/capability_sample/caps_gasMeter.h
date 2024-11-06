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

#include "caps/iot_caps_helper_gasMeter.h"
#include "external/JSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_gasMeter_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    char *gasMeterTime_value;
    double gasMeter_value;
    char *gasMeter_unit;
    double gasMeterCalorific_value;
    double gasMeterVolume_value;
    char *gasMeterVolume_unit;
    JSON_H *gasMeterPrecision_value;
    double gasMeterConversion_value;

    const char *(*get_gasMeterTime_value)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeterTime_value)(struct caps_gasMeter_data *caps_data, const char *value);
    void (*attr_gasMeterTime_send)(struct caps_gasMeter_data *caps_data);
    double (*get_gasMeter_value)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeter_value)(struct caps_gasMeter_data *caps_data, double value);
    const char *(*get_gasMeter_unit)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeter_unit)(struct caps_gasMeter_data *caps_data, const char *unit);
    void (*attr_gasMeter_send)(struct caps_gasMeter_data *caps_data);
    double (*get_gasMeterCalorific_value)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeterCalorific_value)(struct caps_gasMeter_data *caps_data, double value);
    void (*attr_gasMeterCalorific_send)(struct caps_gasMeter_data *caps_data);
    double (*get_gasMeterVolume_value)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeterVolume_value)(struct caps_gasMeter_data *caps_data, double value);
    const char *(*get_gasMeterVolume_unit)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeterVolume_unit)(struct caps_gasMeter_data *caps_data, const char *unit);
    void (*attr_gasMeterVolume_send)(struct caps_gasMeter_data *caps_data);
    const JSON_H *(*get_gasMeterPrecision_value)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeterPrecision_value)(struct caps_gasMeter_data *caps_data, const JSON_H *value);
    void (*attr_gasMeterPrecision_send)(struct caps_gasMeter_data *caps_data);
    double (*get_gasMeterConversion_value)(struct caps_gasMeter_data *caps_data);
    void (*set_gasMeterConversion_value)(struct caps_gasMeter_data *caps_data, double value);
    void (*attr_gasMeterConversion_send)(struct caps_gasMeter_data *caps_data);

    void (*init_usr_cb)(struct caps_gasMeter_data *caps_data);
} caps_gasMeter_data_t;

caps_gasMeter_data_t *caps_gasMeter_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

