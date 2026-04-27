/* ***************************************************************************
 *
 * Copyright 2019-2026 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#include "iot_caps_helper_filterState.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_filterState_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    int filterLifeRemaining_value;
    char *filterLifeRemaining_unit;
    const char **supportedFilterCommands_value;
    int supportedFilterCommands_array_size;

    int (*get_filterLifeRemaining_value)(struct caps_filterState_data *caps_data);
    void (*set_filterLifeRemaining_value)(struct caps_filterState_data *caps_data, int value);
    const char *(*get_filterLifeRemaining_unit)(struct caps_filterState_data *caps_data);
    void (*set_filterLifeRemaining_unit)(struct caps_filterState_data *caps_data, const char *unit);
    void (*attr_filterLifeRemaining_send)(struct caps_filterState_data *caps_data);
    const char **(*get_supportedFilterCommands_value)(struct caps_filterState_data *caps_data);
    void (*set_supportedFilterCommands_value)(struct caps_filterState_data *caps_data, const char **value, int array_size);
    void (*attr_supportedFilterCommands_send)(struct caps_filterState_data *caps_data);

    void (*init_usr_cb)(struct caps_filterState_data *caps_data);
    void (*cmd_resetFilter_usr_cb)(struct caps_filterState_data *caps_data);
} caps_filterState_data_t;

caps_filterState_data_t *caps_filterState_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif
