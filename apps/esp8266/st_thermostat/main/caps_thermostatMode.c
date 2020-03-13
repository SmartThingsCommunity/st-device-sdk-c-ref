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
#include "caps_thermostatMode.h"

#include "freertos/FreeRTOS.h"

static const char *caps_thermostatMode_get_thermostatMode_value(caps_thermostatMode_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return caps_data->thermostatMode_value;
}

static void caps_thermostatMode_set_thermostatMode_value(caps_thermostatMode_data_t *caps_data, const char *value)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->thermostatMode_value = (char *)value;
}

static void caps_thermostatMode_attr_thermostatMode_send(caps_thermostatMode_data_t *caps_data)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int sequence_no;

	cap_evt = st_cap_attr_create_string(caps_helper_thermostatMode.attr_thermostatMode.name,
		caps_data->thermostatMode_value, NULL);

	sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send thermostatMode value\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static const char **caps_thermostatMode_get_supportedThermostatModes_value(caps_thermostatMode_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return (const char **)caps_data->supportedThermostatModes_value;
}

static void caps_thermostatMode_set_supportedThermostatModes_value(caps_thermostatMode_data_t *caps_data, const char **value, int array_size)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->supportedThermostatModes_value = (char **)value;
	caps_data->supportedThermostatModes_array_size = array_size;
}

static void caps_thermostatMode_attr_supportedThermostatModes_send(caps_thermostatMode_data_t *caps_data)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int sequence_no;

	cap_evt = st_cap_attr_create_string_array((char *)caps_helper_thermostatMode.attr_supportedThermostatModes.name,
		caps_data->supportedThermostatModes_array_size, caps_data->supportedThermostatModes_value, NULL);

	sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send supportedThermostatModes value\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static void caps_thermostatMode_cmd_auto_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = (caps_thermostatMode_data_t *)usr_data;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_thermostatMode_set_thermostatMode_value(caps_data, CAPS_HELPER_THERMOSTAT_MODE_VALUE_AUTO);
	if (caps_data && caps_data->cmd_auto_usr_cb)
		caps_data->cmd_auto_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
}

static void caps_thermostatMode_cmd_cool_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = (caps_thermostatMode_data_t *)usr_data;
	const char* value = caps_helper_thermostatMode.attr_thermostatMode.values[CAPS_HELPER_THERMOSTAT_MODE_VALUE_COOL];

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_thermostatMode_set_thermostatMode_value(caps_data, value);
	if (caps_data && caps_data->cmd_cool_usr_cb)
		caps_data->cmd_cool_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
}

static void caps_thermostatMode_cmd_emergencyHeat_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = (caps_thermostatMode_data_t *)usr_data;
	const char* value = caps_helper_thermostatMode.attr_thermostatMode.values[CAPS_HELPER_THERMOSTAT_MODE_VALUE_EMERGENCY_HEAT];

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_thermostatMode_set_thermostatMode_value(caps_data, value);
	if (caps_data && caps_data->cmd_emergencyHeat_usr_cb)
		caps_data->cmd_emergencyHeat_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
}

static void caps_thermostatMode_cmd_heat_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = (caps_thermostatMode_data_t *)usr_data;
	const char* value = caps_helper_thermostatMode.attr_thermostatMode.values[CAPS_HELPER_THERMOSTAT_MODE_VALUE_HEAT];

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_thermostatMode_set_thermostatMode_value(caps_data, value);
	if (caps_data && caps_data->cmd_heat_usr_cb)
		caps_data->cmd_heat_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
}

static void caps_thermostatMode_cmd_off_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = (caps_thermostatMode_data_t *)usr_data;
	const char* value = caps_helper_thermostatMode.attr_thermostatMode.values[CAPS_HELPER_THERMOSTAT_MODE_VALUE_OFF];

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	caps_thermostatMode_set_thermostatMode_value(caps_data, value);
	if (caps_data && caps_data->cmd_off_usr_cb)
		caps_data->cmd_off_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
}

static int caps_thermostatMode_attr_thermostatMode_str2idx(const char* value)
{
	int index;

	for (index = 0; index < CAPS_HELPER_THERMOSTAT_MODE_VALUE_MAX; index++) {
		if (!strcmp(value, caps_helper_thermostatMode.attr_thermostatMode.values[index])) {
			return index;
		}
	}
	return -1;
}

static void caps_thermostatMode_cmd_setThermostatMode_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = (caps_thermostatMode_data_t *)usr_data;
	int index;
	const char *value;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	index = caps_thermostatMode_attr_thermostatMode_str2idx(cmd_data->cmd_data[0].string);
	if (index < 0) {
		printf("%s is not supported value for setThermostatMode\n", cmd_data->cmd_data[0].string);
		return;
	}

	value = caps_helper_thermostatMode.attr_thermostatMode.values[index];
	caps_thermostatMode_set_thermostatMode_value(caps_data, value);
	if (caps_data && caps_data->cmd_setThermostatMode_usr_cb)
		caps_data->cmd_setThermostatMode_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
}

static void caps_thermostatMode_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = usr_data;
	if (caps_data && caps_data->init_usr_cb)
		caps_data->init_usr_cb(caps_data);
	caps_thermostatMode_attr_thermostatMode_send(caps_data);
	caps_thermostatMode_attr_supportedThermostatModes_send(caps_data);
}

caps_thermostatMode_data_t *caps_thermostatMode_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
	caps_thermostatMode_data_t *caps_data = NULL;
	int err;

	caps_data = malloc(sizeof(caps_thermostatMode_data_t));
	if (!caps_data) {
		printf("fail to malloc for caps_thermostatMode_data\n");
		return NULL;
	}

	memset(caps_data, 0, sizeof(caps_thermostatMode_data_t));

	caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_thermostatMode.id , caps_thermostatMode_init_cb, caps_data);
	caps_data->init_usr_cb = init_usr_cb;
	caps_data->usr_data = usr_data;

	caps_data->get_thermostatMode_value = caps_thermostatMode_get_thermostatMode_value;
	caps_data->set_thermostatMode_value = caps_thermostatMode_set_thermostatMode_value;
	caps_data->attr_thermostatMode_send = caps_thermostatMode_attr_thermostatMode_send;

	caps_data->get_supportedThermostatModes_value = caps_thermostatMode_get_supportedThermostatModes_value;
	caps_data->set_supportedThermostatModes_value = caps_thermostatMode_set_supportedThermostatModes_value;
	caps_data->attr_supportedThermostatModes_send = caps_thermostatMode_attr_supportedThermostatModes_send;

	caps_data->thermostatMode_value = (char *)caps_helper_thermostatMode.attr_thermostatMode.values[CAPS_HELPER_THERMOSTAT_MODE_VALUE_AUTO];
	caps_data->supportedThermostatModes_value = (char **)caps_helper_thermostatMode.attr_supportedThermostatModes.values;
	caps_data->supportedThermostatModes_array_size = sizeof(caps_helper_thermostatMode.attr_supportedThermostatModes.values)/sizeof(char *);

	err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatMode.cmd_auto.name, caps_thermostatMode_cmd_auto_cb, caps_data);
	if (err) {
		printf("fail to set cmd_cb for auto\n");
		return NULL;
	}
	err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatMode.cmd_cool.name, caps_thermostatMode_cmd_cool_cb, caps_data);
	if (err) {
		printf("fail to set cmd_cb for cool\n");
		return NULL;
	}
	err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatMode.cmd_emergencyHeat.name, caps_thermostatMode_cmd_emergencyHeat_cb, caps_data);
	if (err) {
		printf("fail to set cmd_cb for emergencyHeat\n");
		return NULL;
	}
	err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatMode.cmd_heat.name, caps_thermostatMode_cmd_heat_cb, caps_data);
	if (err) {
		printf("fail to set cmd_cb for heat\n");
		return NULL;
	}
	err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatMode.cmd_off.name, caps_thermostatMode_cmd_off_cb, caps_data);
	if (err) {
		printf("fail to set cmd_cb for off\n");
		return NULL;
	}
	err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatMode.cmd_setThermostatMode.name, caps_thermostatMode_cmd_setThermostatMode_cb, caps_data);
	if (err) {
		printf("fail to set cmd_cb for setThermostatMode\n");
		return NULL;
	}

	return caps_data;
}
