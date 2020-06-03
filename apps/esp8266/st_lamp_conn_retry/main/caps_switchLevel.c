/* ***************************************************************************
 *
 * Copyright 2020 Samsung Electronics All Rights Reserved.
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

#include <string.h>

#include "st_dev.h"
#include "caps_switchLevel.h"

#include "freertos/FreeRTOS.h"

static int caps_switchLevel_get_level_value(caps_switchLevel_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return caps_helper_switchLevel.attr_level.min - 1;
	}
	return caps_data->level_value;
}

static void caps_switchLevel_set_level_value(caps_switchLevel_data_t *caps_data, int value)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->level_value = value;
}

static const char *caps_switchLevel_get_level_unit(caps_switchLevel_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return caps_data->level_unit;
}

static void caps_switchLevel_set_level_unit(caps_switchLevel_data_t *caps_data, const char *unit)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->level_unit = (char *)unit;
}

static void caps_switchLevel_attr_level_send(caps_switchLevel_data_t *caps_data)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	if (!caps_data || !caps_data->handle) {
		printf("fail to get handle\n");
		return;
	}

	cap_evt = st_cap_attr_create_int((char *)caps_helper_switchLevel.attr_level.name, caps_data->level_value, caps_data->level_unit);
	if (!cap_evt) {
		printf("fail to create cap_evt\n");
		return;
	}

	sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send level value\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static void caps_switchLevel_cmd_set_level_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_switchLevel_data_t *caps_data = usr_data;
	int level;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	level = cmd_data->cmd_data[0].number;

	caps_switchLevel_set_level_value(caps_data, level);
	if (caps_data && caps_data->cmd_set_level_usr_cb)
		caps_data->cmd_set_level_usr_cb(caps_data);
	caps_switchLevel_attr_level_send(caps_data);
}

static void caps_switchLevel_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	caps_switchLevel_data_t *caps_data = usr_data;
	if (caps_data && caps_data->init_usr_cb)
		caps_data->init_usr_cb(caps_data);
	caps_switchLevel_attr_level_send(caps_data);
}

caps_switchLevel_data_t *caps_switchLevel_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
	caps_switchLevel_data_t *caps_data = NULL;
	int err;

	caps_data = malloc(sizeof(caps_switchLevel_data_t));
	if (!caps_data) {
		printf("fail to malloc for caps_switchLevel_data\n");
		return NULL;
	}

	memset(caps_data, 0, sizeof(caps_switchLevel_data_t));

	caps_data->init_usr_cb = init_usr_cb;
	caps_data->usr_data = usr_data;

	caps_data->get_level_value = caps_switchLevel_get_level_value;
	caps_data->set_level_value = caps_switchLevel_set_level_value;
	caps_data->get_level_unit = caps_switchLevel_get_level_unit;
	caps_data->set_level_unit = caps_switchLevel_set_level_unit;
	caps_data->attr_level_send = caps_switchLevel_attr_level_send;

	caps_data->level_value = caps_helper_switchLevel.attr_level.min;
	caps_data->level_unit = (char *)caps_helper_switchLevel.attr_level.units[CAPS_HELPER_SWITCH_LEVEL_UNIT_PERCENT];

	if (ctx) {
		caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_switchLevel.id, caps_switchLevel_init_cb, caps_data);
	}
	if (caps_data->handle) {
		err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_switchLevel.cmd_setLevel.name, caps_switchLevel_cmd_set_level_cb, caps_data);
		if (err) {
			printf("fail to set cmd_cb for setLevel\n");
		}
	} else {
		printf("fail to init switchLevel handle\n");
	}

	return caps_data;
}
