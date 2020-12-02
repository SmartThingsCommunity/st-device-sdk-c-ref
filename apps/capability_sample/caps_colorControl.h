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

#include "caps/iot_caps_helper_colorControl.h"
#include "external/JSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_colorControl_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    double hue_value;
    double saturation_value;

    void (*set_color_value)(struct caps_colorControl_data *caps_data, double hue, double saturaiton);
    void (*attr_color_send)(struct caps_colorControl_data *caps_data);
    double (*get_hue_value)(struct caps_colorControl_data *caps_data);
    void (*set_hue_value)(struct caps_colorControl_data *caps_data, double value);
    void (*attr_hue_send)(struct caps_colorControl_data *caps_data);
    double (*get_saturation_value)(struct caps_colorControl_data *caps_data);
    void (*set_saturation_value)(struct caps_colorControl_data *caps_data, double value);
    void (*attr_saturation_send)(struct caps_colorControl_data *caps_data);

    void (*init_usr_cb)(struct caps_colorControl_data *caps_data);

    void (*cmd_setHue_usr_cb)(struct caps_colorControl_data *caps_data);
    void (*cmd_setColor_usr_cb)(struct caps_colorControl_data *caps_data);
    void (*cmd_setSaturation_usr_cb)(struct caps_colorControl_data *caps_data);
} caps_colorControl_data_t;

caps_colorControl_data_t *caps_colorControl_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

