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
#include "caps_thermostatHeatingSetpoint.h"

#include "freertos/FreeRTOS.h"

static int caps_thermostatHeatingSetpoint_get_heatingSetpoint_value(caps_thermostatHeatingSetpoint_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.min - 1;
	}
	return caps_data->heatingSetpoint_value;
}

static void caps_thermostatHeatingSetpoint_set_heatingSetpoint_value(caps_thermostatHeatingSetpoint_data_t *caps_data, int value)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->heatingSetpoint_value = value;
}

static const char *caps_thermostatHeatingSetpoint_get_heatingSetpoint_unit(caps_thermostatHeatingSetpoint_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return caps_data->heatingSetpoint_unit;
}

static void caps_thermostatHeatingSetpoint_set_heatingSetpoint_unit(caps_thermostatHeatingSetpoint_data_t *caps_data, const char *unit)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->heatingSetpoint_unit = (char *)unit;
}

static void caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send(caps_thermostatHeatingSetpoint_data_t *caps_data)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int sequence_no;

	if (!caps_data || !caps_data->handle) {
		printf("fail to get handle\n");
		return;
	}

	cap_evt = st_cap_attr_create_int((char *) caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.name, caps_data->heatingSetpoint_value, caps_data->heatingSetpoint_unit);
	if (!cap_evt) {
		printf("fail to create cap_evt\n");
		return;
	}

	sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send heatingSetpoint value\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static void caps_thermostatHeatingSetpoint_cmd_set_heatingSetpoint_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	caps_thermostatHeatingSetpoint_data_t *caps_data = usr_data;
	int value;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	value = cmd_data->cmd_data[0].number;

	caps_thermostatHeatingSetpoint_set_heatingSetpoint_value(caps_data, value);
	if (caps_data && caps_data->cmd_setHeatingSetpoint_usr_cb)
		caps_data->cmd_setHeatingSetpoint_usr_cb(caps_data);
	caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send(caps_data);
}

static void caps_thermostatHeatingSetpoint_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	caps_thermostatHeatingSetpoint_data_t *caps_data = usr_data;
	if (caps_data && caps_data->init_usr_cb)
		caps_data->init_usr_cb(caps_data);
	caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send(caps_data);
}

caps_thermostatHeatingSetpoint_data_t *caps_thermostatHeatingSetpoint_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
	caps_thermostatHeatingSetpoint_data_t *caps_data = NULL;
	int err;

	caps_data = malloc(sizeof(caps_thermostatHeatingSetpoint_data_t));
	if (!caps_data) {
		printf("fail to malloc for caps_thermostatHeatingSetpoint_data\n");
		return NULL;
	}

	memset(caps_data, 0, sizeof(caps_thermostatHeatingSetpoint_data_t));

	caps_data->init_usr_cb = init_usr_cb;
	caps_data->usr_data = usr_data;

	caps_data->get_heatingSetpoint_value = caps_thermostatHeatingSetpoint_get_heatingSetpoint_value;
	caps_data->set_heatingSetpoint_value = caps_thermostatHeatingSetpoint_set_heatingSetpoint_value;
	caps_data->get_heatingSetpoint_unit = caps_thermostatHeatingSetpoint_get_heatingSetpoint_unit;
	caps_data->set_heatingSetpoint_unit = caps_thermostatHeatingSetpoint_set_heatingSetpoint_unit;
	caps_data->attr_heatingSetpoint_send = caps_thermostatHeatingSetpoint_attr_heatingSetpoint_send;

	caps_data->heatingSetpoint_value = caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.min;
	caps_data->heatingSetpoint_unit = (char *)caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.units[CAPS_HELPER_THERMOSTAT_HEATING_SETPOINT_UNIT_C];

	if (ctx) {
		caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_thermostatHeatingSetpoint.id , caps_thermostatHeatingSetpoint_init_cb, caps_data);
	}
	if (caps_data->handle) {
		err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_thermostatHeatingSetpoint.cmd_setHeatingSetpoint.name, caps_thermostatHeatingSetpoint_cmd_set_heatingSetpoint_cb, caps_data);
		if (err) {
			printf("fail to set cmd_cb for set_heatingSetpoint\n");
		}
	} else {
		printf("fail to init thermostatHeatingSetpoint handle\n");
	}

	return caps_data;
}
