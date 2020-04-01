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
#include "caps_alarm.h"

#include "freertos/FreeRTOS.h"

static const char *caps_alarm_get_alarm_value(caps_alarm_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return caps_data->alarm_value;
}

static void caps_alarm_set_alarm_value(caps_alarm_data_t *caps_data, const char *value)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->alarm_value = (char *)value;
}

static void caps_alarm_attr_alarm_send(caps_alarm_data_t *caps_data)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int sequence_no;

	if (!caps_data || !caps_data->handle) {
		printf("fail to get handle\n");
		return;
	}

	cap_evt = st_cap_attr_create_string((char *)caps_helper_alarm.attr_alarm.name,
		caps_data->alarm_value, NULL);

	sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send alarm value\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static void caps_alarm_cmd_both_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_alarm_data_t *caps_data = (caps_alarm_data_t *)usr_data;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_alarm_set_alarm_value(caps_data, caps_helper_alarm.attr_alarm.values[CAPS_HELPER_ALARM_VALUE_BOTH]);
	if (caps_data && caps_data->cmd_off_usr_cb)
		caps_data->cmd_both_usr_cb(caps_data);
	caps_alarm_attr_alarm_send(caps_data);
}

static void caps_alarm_cmd_off_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_alarm_data_t *caps_data = (caps_alarm_data_t *)usr_data;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_alarm_set_alarm_value(caps_data, caps_helper_alarm.attr_alarm.values[CAPS_HELPER_ALARM_VALUE_OFF]);
	if (caps_data && caps_data->cmd_off_usr_cb)
		caps_data->cmd_off_usr_cb(caps_data);
	caps_alarm_attr_alarm_send(caps_data);
}

static void caps_alarm_cmd_siren_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_alarm_data_t *caps_data = (caps_alarm_data_t *)usr_data;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_alarm_set_alarm_value(caps_data, caps_helper_alarm.attr_alarm.values[CAPS_HELPER_ALARM_VALUE_SIREN]);
	if (caps_data && caps_data->cmd_off_usr_cb)
		caps_data->cmd_siren_usr_cb(caps_data);
	caps_alarm_attr_alarm_send(caps_data);
}

static void caps_alarm_cmd_strobe_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_alarm_data_t *caps_data = (caps_alarm_data_t *)usr_data;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_alarm_set_alarm_value(caps_data, caps_helper_alarm.attr_alarm.values[CAPS_HELPER_ALARM_VALUE_STROBE]);
	if (caps_data && caps_data->cmd_off_usr_cb)
		caps_data->cmd_strobe_usr_cb(caps_data);
	caps_alarm_attr_alarm_send(caps_data);
}
static void caps_alarm_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	caps_alarm_data_t *caps_data = usr_data;
	if (caps_data && caps_data->init_usr_cb)
		caps_data->init_usr_cb(caps_data);
	caps_alarm_attr_alarm_send(caps_data);
}

caps_alarm_data_t *caps_alarm_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
	caps_alarm_data_t *caps_data = NULL;
	int err;

	caps_data = malloc(sizeof(caps_alarm_data_t));
	if (!caps_data) {
		printf("fail to malloc for caps_alarm_data\n");
		return NULL;
	}

	memset(caps_data, 0, sizeof(caps_alarm_data_t));

	caps_data->init_usr_cb = init_usr_cb;
	caps_data->usr_data = usr_data;

	caps_data->get_alarm_value = caps_alarm_get_alarm_value;
	caps_data->set_alarm_value = caps_alarm_set_alarm_value;
	caps_data->attr_alarm_send = caps_alarm_attr_alarm_send;

	caps_data->alarm_value = (char *)caps_helper_alarm.attr_alarm.values[CAPS_HELPER_ALARM_VALUE_OFF];

	if (ctx) {
		caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_alarm.id , caps_alarm_init_cb, caps_data);
	}
	if (caps_data->handle) {
		err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_alarm.cmd_both.name, caps_alarm_cmd_both_cb, caps_data);
		if (err) {
			printf("fail to set cmd_cb for both\n");
		}
		err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_alarm.cmd_siren.name, caps_alarm_cmd_siren_cb, caps_data);
		if (err) {
			printf("fail to set cmd_cb for siren\n");
		}
		err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_alarm.cmd_off.name, caps_alarm_cmd_off_cb, caps_data);
		if (err) {
			printf("fail to set cmd_cb for off\n");
		}
		err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_alarm.cmd_strobe.name, caps_alarm_cmd_strobe_cb, caps_data);
		if (err) {
			printf("fail to set cmd_cb for strobe\n");
		}
	} else {
		printf("fail to init alarm handle\n");
	}
	return caps_data;
}
