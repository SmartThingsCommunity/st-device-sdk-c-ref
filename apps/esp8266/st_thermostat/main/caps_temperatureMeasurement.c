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
#include "caps_temperatureMeasurement.h"

#include "freertos/FreeRTOS.h"

static int caps_temperatureMeasurement_get_temperature_value(caps_temperatureMeasurement_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return caps_data->temperature_value;
}

static void caps_temperatureMeasurement_set_temperature_value(caps_temperatureMeasurement_data_t *caps_data, int value)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->temperature_value = value;
}

static const char *caps_temperatureMeasurement_get_temperature_unit(caps_temperatureMeasurement_data_t *caps_data)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return NULL;
	}
	return caps_data->temperature_unit;
}

static void caps_temperatureMeasurement_set_temperature_unit(caps_temperatureMeasurement_data_t *caps_data, const char *unit)
{
	if (!caps_data) {
		printf("caps_data is NULL\n");
		return;
	}
	caps_data->temperature_unit = (char *)unit;
}

static void caps_temperatureMeasurement_attr_temperature_send(caps_temperatureMeasurement_data_t *caps_data)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int sequence_no;

	cap_evt = st_cap_attr_create_int((char *) caps_helper_temperatureMeasurement.attr_temperature.name, caps_data->temperature_value, caps_data->temperature_unit);

	sequence_no = st_cap_attr_send(caps_data->handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send temperature value\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static void caps_temperatureMeasurement_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	caps_temperatureMeasurement_data_t *caps_data = usr_data;
	if (caps_data && caps_data->init_usr_cb)
		caps_data->init_usr_cb(caps_data);
	caps_temperatureMeasurement_attr_temperature_send(caps_data);
}

caps_temperatureMeasurement_data_t *caps_temperatureMeasurement_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
	caps_temperatureMeasurement_data_t *caps_data = NULL;

	caps_data = malloc(sizeof(caps_temperatureMeasurement_data_t));
	if (!caps_data) {
		printf("fail to malloc for caps_temperatureMeasurement_data\n");
		return NULL;
	}

	memset(caps_data, 0, sizeof(caps_temperatureMeasurement_data_t));

	caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_temperatureMeasurement.id , caps_temperatureMeasurement_init_cb, caps_data);
	caps_data->init_usr_cb = init_usr_cb;
	caps_data->usr_data = usr_data;

	caps_data->get_temperature_value = caps_temperatureMeasurement_get_temperature_value;
	caps_data->set_temperature_value = caps_temperatureMeasurement_set_temperature_value;
	caps_data->get_temperature_unit = caps_temperatureMeasurement_get_temperature_unit;
	caps_data->set_temperature_unit = caps_temperatureMeasurement_set_temperature_unit;
	caps_data->attr_temperature_send = caps_temperatureMeasurement_attr_temperature_send;

	caps_data->temperature_value = caps_helper_temperatureMeasurement.attr_temperature.min;
	caps_data->temperature_unit = (char *)caps_helper_temperatureMeasurement.attr_temperature.units[CAPS_HELPER_TEMPERATURE_MEASUREMENT_UNIT_C];

	return caps_data;
}
