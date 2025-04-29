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

#include "caps/iot_caps_helper_antiSnoringPillow.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ST_CAP_SEND_ATTR_BOOLEAN(cap_handle, attribute, value_number, unit, data, output_seq_num)\
{\
	IOT_EVENT *attr = NULL;\
	iot_cap_val_t value;\
\
	value.type = IOT_CAP_VAL_TYPE_BOOLEAN;\
	value.boolean = value_number;\
	attr = st_cap_create_attr(cap_handle, attribute, &value, unit, data);\
	if (attr != NULL){\
		output_seq_num = st_cap_send_attr(&attr, 1);\
		st_cap_free_attr(attr);\
	}\
}

typedef struct caps_antiSnoringPillow_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    char *state_value;
    int snoringTime_value;
    int snoringTimeDelta_value;
    bool supportSnoringTimeGraph_value;
    int pillowOperationTime_value;
    int pillowOperationTimeDelta_value;
    bool supportPillowOperationTimeGraph_value;

    const char *(*get_state_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_state_value)(struct caps_antiSnoringPillow_data *caps_data, const char *value);
    int (*attr_state_str2idx)(const char *value);
    void (*attr_state_send)(struct caps_antiSnoringPillow_data *caps_data);

    int (*get_snoringTime_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_snoringTime_value)(struct caps_antiSnoringPillow_data *caps_data, int value);
    void (*attr_snoringTime_send)(struct caps_antiSnoringPillow_data *caps_data);

    int (*get_snoringTimeDelta_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_snoringTimeDelta_value)(struct caps_antiSnoringPillow_data *caps_data, int value);
    void (*attr_snoringTimeDelta_send)(struct caps_antiSnoringPillow_data *caps_data);

    bool (*get_supportSnoringTimeGraph_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_supportSnoringTimeGraph_value)(struct caps_antiSnoringPillow_data *caps_data, bool value);
    void (*attr_supportSnoringTimeGraph_send)(struct caps_antiSnoringPillow_data *caps_data);

    int (*get_pillowOperationTime_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_pillowOperationTime_value)(struct caps_antiSnoringPillow_data *caps_data, int value);
    void (*attr_pillowOperationTime_send)(struct caps_antiSnoringPillow_data *caps_data);

    int (*get_pillowOperationTimeDelta_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_pillowOperationTimeDelta_value)(struct caps_antiSnoringPillow_data *caps_data, int value);
    void (*attr_pillowOperationTimeDelta_send)(struct caps_antiSnoringPillow_data *caps_data);

    bool (*get_supportPillowOperationTimeGraph_value)(struct caps_antiSnoringPillow_data *caps_data);
    void (*set_supportPillowOperationTimeGraph_value)(struct caps_antiSnoringPillow_data *caps_data, bool value);
    void (*attr_supportPillowOperationTimeGraph_send)(struct caps_antiSnoringPillow_data *caps_data);

    void (*init_usr_cb)(struct caps_antiSnoringPillow_data *caps_data);

    void (*cmd_on_usr_cb)(struct caps_antiSnoringPillow_data *caps_data);
    void (*cmd_off_usr_cb)(struct caps_antiSnoringPillow_data *caps_data);
} caps_antiSnoringPillow_data_t;

caps_antiSnoringPillow_data_t *caps_antiSnoringPillow_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

