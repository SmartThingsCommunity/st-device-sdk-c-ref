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
#include "driver/gpio.h"

#include "caps_airQualitySensor.h"
#include "caps_alarm.h"
#include "caps_carbonDioxideMeasurement.h"
#include "caps_carbonMonoxideDetector.h"
#include "caps_formaldehydeMeasurement.h"

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]	asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]	asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]	asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]		asm("_binary_device_info_json_end");

static iot_status_t g_iot_status;
static iot_stat_lv_t g_iot_stat_lv;

IOT_CTX *ctx = NULL;

struct caps_airQualitySensor_data *cap_airQualitySensor_data;
struct caps_alarm_data *cap_alarm_data;
struct caps_carbonDioxideMeasurement_data *cap_carbonDioxideMeasurement_data;
struct caps_carbonMonoxideDetector_data *cap_carbonMonoxideDetector_data;
struct caps_formaldehydeMeasurement_data *cap_formaldehydeMeasurement_data;

void change_siren_state(int onoff)
{
	/*
	 * YOUR CODE:
	 */
}

void change_strobe_state(int onoff)
{
	/*
	 * YOUR CODE:
	 */
}

void change_alarm_state(int state)
{
	switch(state)
	{
		case CAPS_HELPER_ALARM_VALUE_BOTH:
			change_siren_state(1);
			change_strobe_state(1);
			break;
		case CAPS_HELPER_ALARM_VALUE_SIREN:
			change_siren_state(1);
			change_strobe_state(0);
			break;
		case CAPS_HELPER_ALARM_VALUE_STROBE:
			change_siren_state(0);
			change_strobe_state(1);
			break;
		case CAPS_HELPER_ALARM_VALUE_OFF:
			change_siren_state(0);
			change_strobe_state(0);
			break;
		default:
			change_siren_state(0);
			change_strobe_state(0);
			printf("Error: unexpected alarm state : %d\n", state);
	}
}
/* dummy value for test */
#define AIR_QUALITY_THRESHOLD 50
#define CARBON_DIOXIDE_THRESHOLD 500
#define CARBON_MONOXIDE_THRESHOLD 100
#define FORMALDEHYDE_THRESHOLD 500

int get_air_quality(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */
	int air_quality_value = 80;

	return air_quality_value;
}

int get_carbon_dioxide(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */
	int carbon_dioxide_value = 300;

	return carbon_dioxide_value;
}

int get_carbon_monoxide(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */
	int carbon_monoxide_value = 70;

	return carbon_monoxide_value;
}

const char* get_carbon_monoxide_state(void)
{
	if (get_carbon_monoxide() > CARBON_MONOXIDE_THRESHOLD) {
		return caps_helper_carbonMonoxideDetector.attr_carbonMonoxide.values[CAPS_HELPER_CARBON_MONOXIDE_DETECTOR_VALUE_DETECTED];
	}
	return caps_helper_carbonMonoxideDetector.attr_carbonMonoxide.values[CAPS_HELPER_CARBON_MONOXIDE_DETECTOR_VALUE_CLEAR];
}

int get_formaldehyde(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */
	int formaldehyde_value = 200;

	return formaldehyde_value;
}

int get_alarm_state(void)
{
	int alarm_state;
	if ( (AIR_QUALITY_THRESHOLD > cap_airQualitySensor_data->get_airQuality_value(cap_airQualitySensor_data))
		|| (CARBON_DIOXIDE_THRESHOLD < cap_carbonDioxideMeasurement_data->get_carbonDioxide_value(cap_carbonDioxideMeasurement_data))
		|| (FORMALDEHYDE_THRESHOLD < cap_formaldehydeMeasurement_data->get_formaldehydeLevel_value(cap_formaldehydeMeasurement_data))
		|| (!strcmp(cap_carbonMonoxideDetector_data->get_carbonMonoxide_value(cap_carbonMonoxideDetector_data), caps_helper_carbonMonoxideDetector.attr_carbonMonoxide.values[CAPS_HELPER_CARBON_MONOXIDE_DETECTOR_VALUE_DETECTED]))) {
		alarm_state = CAPS_HELPER_ALARM_VALUE_BOTH;
	} else {
		alarm_state = CAPS_HELPER_ALARM_VALUE_OFF;
	}

	return alarm_state;
}

void cap_airQualitySensor_init_cb(struct caps_airQualitySensor_data *caps_data)
{
	caps_data->set_airQuality_value(caps_data, get_air_quality());
	caps_data->set_airQuality_unit(caps_data, caps_helper_airQualitySensor.attr_airQuality.units[CAPS_HELPER_AIR_QUALITY_SENSOR_UNIT_CAQI]);
}

void cap_carbonDioxideMeasurement_init_cb(struct caps_carbonDioxideMeasurement_data *caps_data)
{
	caps_data->set_carbonDioxide_value(caps_data, get_carbon_dioxide());
	caps_data->set_carbonDioxide_unit(caps_data, caps_helper_carbonDioxideMeasurement.attr_carbonDioxide.units[CAPS_HELPER_CARBON_DIOXIDE_MEASUREMENT_UNIT_PPM]);
}

void cap_carbonMonoxideDetector_init_cb(struct caps_carbonMonoxideDetector_data *caps_data)
{
	caps_data->set_carbonMonoxide_value(caps_data, get_carbon_monoxide_state());
}

void cap_formaldehydeMeasurement_init_cb(struct caps_formaldehydeMeasurement_data *caps_data)
{
	caps_data->set_formaldehydeLevel_value(caps_data, get_formaldehyde());
	caps_data->set_formaldehydeLevel_unit(caps_data, caps_helper_formaldehydeMeasurement.attr_formaldehydeLevel.units[CAPS_HELPER_FORMALDEHYDE_MEASUREMENT_UNIT_PPM]);
}

void cap_alarm_cmd_cb(struct caps_alarm_data *caps_data)
{
	const char *alarm_value = caps_data->get_alarm_value(caps_data);
	int alarm_state;

	for (alarm_state = 0; alarm_state < CAPS_HELPER_ALARM_VALUE_MAX; alarm_state++) {
		if (!strcmp(alarm_value, caps_helper_alarm.attr_alarm.values[alarm_state])) {
			change_alarm_state(alarm_state);
			return;
		}
	}
	printf("Error: unexpected alarm value %s\n", alarm_value);
}

static void iot_status_cb(iot_status_t status,
			iot_stat_lv_t stat_lv, void *usr_data)
{
	g_iot_status = status;
	g_iot_stat_lv = stat_lv;

	printf("status: %d, stat: %d\n", g_iot_status, g_iot_stat_lv);
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

void update_air_monitor_info(void)
{
	cap_airQualitySensor_data->set_airQuality_value(cap_airQualitySensor_data, get_air_quality());
	cap_carbonDioxideMeasurement_data->set_carbonDioxide_value(cap_carbonDioxideMeasurement_data, get_carbon_dioxide());
	cap_carbonMonoxideDetector_data->set_carbonMonoxide_value(cap_carbonMonoxideDetector_data, get_carbon_monoxide_state());
	cap_formaldehydeMeasurement_data->set_formaldehydeLevel_value(cap_formaldehydeMeasurement_data, get_formaldehyde());
}

void update_alarm_state(void)
{
	int alarm_state = get_alarm_state();
	change_alarm_state(alarm_state);
	cap_alarm_data->set_alarm_value(cap_alarm_data, caps_helper_alarm.attr_alarm.values[alarm_state]);
}

void send_air_monitor_info(void)
{
	cap_alarm_data->attr_alarm_send(cap_alarm_data);
	cap_airQualitySensor_data->attr_airQuality_send(cap_airQualitySensor_data);
	cap_carbonDioxideMeasurement_data->attr_carbonDioxide_send(cap_carbonDioxideMeasurement_data);
	cap_carbonMonoxideDetector_data->attr_carbonMonoxide_send(cap_carbonMonoxideDetector_data);
	cap_formaldehydeMeasurement_data->attr_formaldehydeLevel_send(cap_formaldehydeMeasurement_data);
}

#define MONITOR_DELAY_MS 60000
static void app_task(void *arg)
{
	TimeOut_t monitor_timeout;
	TickType_t monitor_delay_tick = pdMS_TO_TICKS(MONITOR_DELAY_MS);

	vTaskSetTimeOutState(&monitor_timeout);
	for (;;) {
		if (xTaskCheckForTimeOut(&monitor_timeout, &monitor_delay_tick ) != pdFALSE) {
			vTaskSetTimeOutState(&monitor_timeout);
			monitor_delay_tick = pdMS_TO_TICKS(MONITOR_DELAY_MS);

			update_air_monitor_info();
			update_alarm_state();

			send_air_monitor_info();
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

static void device_init(void)
{
	int alarm_init_state = CAPS_HELPER_ALARM_VALUE_OFF;
	cap_alarm_data->set_alarm_value(cap_alarm_data, caps_helper_alarm.attr_alarm.values[alarm_init_state]);

	change_alarm_state(alarm_init_state);
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
	} else {
		printf("fail to create the iot_context\n");
	}

	// 2. create a handle to process capability
	//	implement init_callback function
	cap_alarm_data = caps_alarm_initialize(ctx, "main", NULL, NULL);
	cap_airQualitySensor_data = caps_airQualitySensor_initialize(ctx, "main", cap_airQualitySensor_init_cb, NULL);
	cap_carbonDioxideMeasurement_data = caps_carbonDioxideMeasurement_initialize(ctx, "main", cap_carbonDioxideMeasurement_init_cb, NULL);
	cap_carbonMonoxideDetector_data = caps_carbonMonoxideDetector_initialize(ctx, "main", cap_carbonMonoxideDetector_init_cb, NULL);
	cap_formaldehydeMeasurement_data = caps_formaldehydeMeasurement_initialize(ctx, "main", cap_formaldehydeMeasurement_init_cb, NULL);

	// 3. register a callback function to process capability command when it comes from the SmartThings Server
	//	implement callback function
	cap_alarm_data->cmd_both_usr_cb = cap_alarm_cmd_cb;
	cap_alarm_data->cmd_off_usr_cb = cap_alarm_cmd_cb;
	cap_alarm_data->cmd_siren_usr_cb = cap_alarm_cmd_cb;
	cap_alarm_data->cmd_strobe_usr_cb = cap_alarm_cmd_cb;

	device_init();

	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(app_task, "app_task", 2048, NULL, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);
}
