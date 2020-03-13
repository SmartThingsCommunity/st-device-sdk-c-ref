/* ***************************************************************************
 *
 * Copyright 2019 Samsung Electronics All Rights Reserved.
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

//for implementing main features of IoT device
#include <stdbool.h>
#include <string.h>

#include "st_dev.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "caps_temperatureMeasurement.h"
#include "caps_thermostatCoolingSetpoint.h"
#include "caps_thermostatHeatingSetpoint.h"
#include "caps_thermostatFanMode.h"
#include "caps_thermostatMode.h"
#include "caps_thermostatOperatingState.h"

#define TEMPERATURE_PERIOD_MS 10000

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]	asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]	asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]	asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]		asm("_binary_device_info_json_end");

static iot_status_t g_iot_status;
static iot_stat_lv_t g_iot_stat_lv;

static IOT_CTX *ctx = NULL;

struct caps_temperatureMeasurement_data *cap_temperature_handle;
struct caps_thermostatCoolingSetpoint_data *cap_coolingsetpoint_handle;
struct caps_thermostatHeatingSetpoint_data *cap_heatingsetpoint_handle;
struct caps_thermostatFanMode_data *cap_fanmode_handle;
struct caps_thermostatMode_data *cap_mode_handle;
struct caps_thermostatOperatingState_data *cap_opstate_handle;

const char *supported_fan_mode[] = { "auto", "on" };
const char *supported_mode[] = { "auto", "cool", "eco" };

static int thermostat_fan_mode_str2idx(const char *mode)
{
	int i;

	for (i = 0; i < CAPS_HELPER_THERMOSTAT_FAN_MODE_VALUE_MAX; i++) {
		if (!strcmp(mode, caps_helper_thermostatFanMode.attr_thermostatFanMode.values[i]))
			return i;
	}

	printf("%s: '%s' is not supported mode\n", __func__, mode);

	return -1;
}

static int thermostat_mode_str2idx(const char *mode)
{
	int i;

	for (i = 0; i < CAPS_HELPER_THERMOSTAT_MODE_VALUE_MAX; i++) {
		if (!strcmp(mode, caps_helper_thermostatMode.attr_thermostatMode.values[i]))
			return i;
	}

	printf("%s: '%s' is not supported mode\n", __func__, mode);

	return -1;
}

static void thermostat_set_mode(int mode)
{
	/*
	 * YOUR CODE:
	 * implement a ability to control the mode of thermostat
	 */
	printf("%s: changed mode = %s", __func__,
				caps_helper_thermostatMode.attr_thermostatMode.values[mode]);
}

static void thermostat_set_fan_mode(int mode)
{
	/*
	 * YOUR CODE:
	 * implement a ability to control the fan mode of thermostat
	 */
	printf("%s: changed fan mode = %s", __func__,
				caps_helper_thermostatFanMode.attr_thermostatFanMode.values[mode]);
}

static void thermostat_set_cooling_point(double point)
{
	/*
	 * YOUR CODE:
	 * implement a ability to set cooling point
	 */
	printf("%s: changed cooling point = %lf", __func__, point);
}

static void thermostat_set_heating_point(double point)
{
	/*
	 * YOUR CODE:
	 * implement a ability to set heating point
	 */
	printf("%s: changed heating point = %lf", __func__, point);
}

static double thermostat_get_temperature(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the temperature
	 */
	return 18.5;
}

static void cap_temperatureMeasurement_init_cb(struct caps_temperatureMeasurement_data *caps_data)
{
	const int initial_temperature_value = thermostat_get_temperature();
	const char *temperature_unit =
		caps_helper_temperatureMeasurement.attr_temperature.units[CAPS_HELPER_TEMPERATURE_MEASUREMENT_UNIT_C];

	caps_data->set_temperature_value(caps_data, initial_temperature_value);
	caps_data->set_temperature_unit(caps_data, temperature_unit);
}

static void cap_thermostatCoolingSetpoint_init_cb(struct caps_thermostatCoolingSetpoint_data *caps_data)
{
	const int initial_cooling_point_value = 30.5;
	const char *cooling_point_unit =
		caps_helper_thermostatCoolingSetpoint.attr_coolingSetpoint.units[CAPS_HELPER_THERMOSTAT_COOLING_SETPOINT_UNIT_C];

	caps_data->set_coolingSetpoint_value(caps_data, initial_cooling_point_value);
	caps_data->set_coolingSetpoint_unit(caps_data, cooling_point_unit);
}

static void cap_thermostatHeatingSetpoint_init_cb(struct caps_thermostatHeatingSetpoint_data *caps_data)
{
	const int initial_heating_point_value = 13.5;
	const char *heating_point_unit =
		caps_helper_thermostatHeatingSetpoint.attr_heatingSetpoint.units[CAPS_HELPER_THERMOSTAT_HEATING_SETPOINT_UNIT_C];

	caps_data->set_heatingSetpoint_value(caps_data, initial_heating_point_value);
	caps_data->set_heatingSetpoint_unit(caps_data, heating_point_unit);
}

static void cap_thermostatFanMode_init_cb(struct caps_thermostatFanMode_data *caps_data)
{
	const char *initial_fan_mode =
		caps_helper_thermostatFanMode.attr_thermostatFanMode.values[CAPS_HELPER_THERMOSTAT_FAN_MODE_VALUE_AUTO];

	caps_data->set_thermostatFanMode_value(caps_data, initial_fan_mode);
	caps_data->set_supportedThermostatFanModes_value(caps_data, supported_fan_mode, sizeof(supported_fan_mode)/sizeof(char *));
}

static void cap_thermostatMode_init_cb(struct caps_thermostatMode_data *caps_data)
{
	const char* initial_mode =
		caps_helper_thermostatMode.attr_thermostatMode.values[CAPS_HELPER_THERMOSTAT_MODE_VALUE_AUTO];

	caps_data->set_thermostatMode_value(caps_data, initial_mode);
	caps_data->set_supportedThermostatModes_value(caps_data, supported_mode, sizeof(supported_mode)/sizeof(char *));

}

static void cap_thermostatOperatingState_init_cb(struct caps_thermostatOperatingState_data *caps_data)
{
	const char* initial_op_state =
		caps_helper_thermostatOperatingState.attr_thermostatOperatingState.values[CAPS_HELPER_THERMOSTAT_OPERATING_STATE_VALUE_IDLE];
	caps_data->set_thermostatOperatingState_value(caps_data, initial_op_state);
}

static void cap_thermostatCoolingSetpoint_cmd_cb(struct caps_thermostatCoolingSetpoint_data *caps_data)
{
	int point = caps_data->get_coolingSetpoint_value(caps_data);
	thermostat_set_cooling_point(point);
}

static void cap_thermostatHeatingSetpoint_cmd_cb(struct caps_thermostatHeatingSetpoint_data *caps_data)
{
	int point = caps_data->get_heatingSetpoint_value(caps_data);
	thermostat_set_heating_point(point);
}

static void cap_thermostatFanMode_cmd_cb(struct caps_thermostatFanMode_data *caps_data)
{
	const char *fan_mode = caps_data->get_thermostatFanMode_value(caps_data);
	thermostat_set_fan_mode( thermostat_fan_mode_str2idx(fan_mode)) ;
}

static void cap_thermostatMode_cmd_cb(struct caps_thermostatMode_data *caps_data)
{
	const char *mode = caps_data->get_thermostatMode_value(caps_data);
	thermostat_set_mode( thermostat_mode_str2idx(mode) );
}


void iot_noti_cb(iot_noti_data_t *noti_data, void *noti_usr_data)
{
	printf("Notification message received\n");

	if (noti_data->type == IOT_NOTI_TYPE_DEV_DELETED) {
		printf("[device deleted]\n");
	} else if (noti_data->type == IOT_NOTI_TYPE_RATE_LIMIT) {
		printf("[rate limit] Remaining time:%d, sequence number:%d\n",
			noti_data->raw.rate_limit.remainingTime, noti_data->raw.rate_limit.sequenceNumber);
	}
}

static void iot_status_cb(iot_status_t status,
			iot_stat_lv_t stat_lv, void *usr_data)
{
	g_iot_status = status;
	g_iot_stat_lv = stat_lv;

	printf("status: %d, stat: %d\n", g_iot_status, g_iot_stat_lv);
}

static void thermostat_task(void *arg)
{
	TimeOut_t temperature_timeout;
	TickType_t temperature_period_tick = pdMS_TO_TICKS(TEMPERATURE_PERIOD_MS);
	int temperature;
	vTaskDelay(TEMPERATURE_PERIOD_MS / portTICK_PERIOD_MS);

	vTaskSetTimeOutState(&temperature_timeout);

	while (1) {
		if (xTaskCheckForTimeOut(&temperature_timeout, &temperature_period_tick ) != pdFALSE) {
			vTaskSetTimeOutState(&temperature_timeout);
			temperature_period_tick = pdMS_TO_TICKS(TEMPERATURE_PERIOD_MS);

			temperature = thermostat_get_temperature();
			cap_temperature_handle->set_temperature_value(cap_temperature_handle, temperature);
			cap_temperature_handle->attr_temperature_send(cap_temperature_handle);
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	/**
	  SmartThings Device SDK(STDK) aims to make it easier to develop IoT devices by providing
	  additional st_iot_core layer to the existing chip vendor SW Architecture.

	  That is, you can simply develop a basic application by just calling the APIs provided by st_iot_core layer
	  like below. st_iot_core currently offers 14 API.

	  //create a iot context
	  1. st_conn_init();

	  //create a handle to process capability
	  2. st_cap_handle_init();

	  //register a callback function to process capability command when it comes from the SmartThings Server.
	  3. st_cap_cmd_set_cb();

	  //needed when it is necessary to keep monitoring the device status
	  4. user_defined_task()

	  //process on-boarding procedure. There is nothing more to do on the app side than call the API.
	  5. st_conn_start();
	 */

	unsigned char *onboarding_config = (unsigned char *) onboarding_config_start;
	unsigned int onboarding_config_len = onboarding_config_end - onboarding_config_start;
	unsigned char *device_info = (unsigned char *) device_info_start;
	unsigned int device_info_len = device_info_end - device_info_start;

	int iot_err;

	// 1. create a iot context
	ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
	if (ctx != NULL) {
		iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
		if (iot_err)
			printf("fail to set notification callback function\n");

	// 2. create a handle to process capability
	//	implement init_callback function
		cap_temperature_handle = caps_temperatureMeasurement_initialize(ctx, "main", cap_temperatureMeasurement_init_cb, NULL);
		cap_coolingsetpoint_handle = caps_thermostatCoolingSetpoint_initialize(ctx, "main", cap_thermostatCoolingSetpoint_init_cb, NULL);
		cap_heatingsetpoint_handle = caps_thermostatHeatingSetpoint_initialize(ctx, "main", cap_thermostatHeatingSetpoint_init_cb, NULL);
		cap_fanmode_handle = caps_thermostatFanMode_initialize(ctx, "main", cap_thermostatFanMode_init_cb, NULL);
		cap_mode_handle = caps_thermostatMode_initialize(ctx, "main", cap_thermostatMode_init_cb, NULL);
		cap_opstate_handle = caps_thermostatOperatingState_initialize(ctx, "main", cap_thermostatOperatingState_init_cb, NULL);

	// 3. register a callback function to process capability command when it comes from the SmartThings Server
	//	implement callback function
		cap_coolingsetpoint_handle->cmd_setCoolingSetpoint_usr_cb = cap_thermostatCoolingSetpoint_cmd_cb;
		cap_heatingsetpoint_handle->cmd_setHeatingSetpoint_usr_cb = cap_thermostatHeatingSetpoint_cmd_cb;
		cap_fanmode_handle->cmd_setThermostatFanMode_usr_cb = cap_thermostatFanMode_cmd_cb;
		cap_mode_handle->cmd_setThermostatMode_usr_cb = cap_thermostatMode_cmd_cb;
	} else {
		printf("fail to create the iot_context\n");
	}

	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(thermostat_task, "thermostat_task", 2048, NULL, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);
}
