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

#include "caps/iot_caps_helper_ultravioletIndex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_ultravioletIndex_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    double ultravioletIndex_value;

    double (*get_ultravioletIndex_value)(struct caps_ultravioletIndex_data *caps_data);
    void (*set_ultravioletIndex_value)(struct caps_ultravioletIndex_data *caps_data, double value);
    void (*attr_ultravioletIndex_send)(struct caps_ultravioletIndex_data *caps_data);

    void (*init_usr_cb)(struct caps_ultravioletIndex_data *caps_data);
} caps_ultravioletIndex_data_t;

caps_ultravioletIndex_data_t *caps_ultravioletIndex_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

