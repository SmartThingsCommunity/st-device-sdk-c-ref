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

#ifndef _IOT_CAPS_HELPER_FILTER_STATE_
#define _IOT_CAPS_HELPER_FILTER_STATE_

#include "caps/iot_caps_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CAP_ENUM_FILTERSTATE_FILTERLIFEREMAINING_UNIT_PERCENT,
    CAP_ENUM_FILTERSTATE_FILTERLIFEREMAINING_UNIT_MAX
};

enum {
    CAP_ENUM_FILTERSTATE_SUPPORTEDFILTERCOMMANDS_VALUE_RESETFILTER,
    CAP_ENUM_FILTERSTATE_SUPPORTEDFILTERCOMMANDS_VALUE_MAX
};

const static struct iot_caps_filterState {
    const char *id;
    const struct filterState_attr_filterLifeRemaining {
        const char *name;
        const unsigned char property;
        const unsigned char valueType;
        const char *units[CAP_ENUM_FILTERSTATE_FILTERLIFEREMAINING_UNIT_MAX];
        const char *unit_percent;
        const int min;
        const int max;
    } attr_filterLifeRemaining;
    const struct filterState_attr_supportedFilterCommands {
        const char *name;
        const unsigned char property;
        const unsigned char valueType;
        const char *values[CAP_ENUM_FILTERSTATE_SUPPORTEDFILTERCOMMANDS_VALUE_MAX];
        const char *value_resetFilter;
    } attr_supportedFilterCommands;
    const struct filterState_cmd_resetFilter {
        const char *name;
    } cmd_resetFilter;
} caps_helper_filterState = {
    .id = "filterState",
    .attr_filterLifeRemaining =
        {
            .name = "filterLifeRemaining",
            .property = ATTR_SET_VALUE_MIN | ATTR_SET_VALUE_MAX | ATTR_SET_VALUE_REQUIRED,
            .valueType = VALUE_TYPE_INTEGER,
            .units = {"%"},
            .unit_percent = "%",
            .min = 0,
            .max = 100,
        },
    .attr_supportedFilterCommands =
        {
            .name = "supportedFilterCommands",
            .property = ATTR_SET_VALUE_ARRAY,
            .valueType = VALUE_TYPE_STRING,
            .values = {"resetFilter"},
            .value_resetFilter = "resetFilter",
        },
    .cmd_resetFilter =
        {
            .name = "resetFilter",
        },
};

#ifdef __cplusplus
}
#endif

#endif /* _IOT_CAPS_HERLPER_FILTER_STATE_ */
