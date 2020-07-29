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

#include "caps_thermostatMode.h"
#include "caps_switchLevel.h"
#include "caps_threeAxis.h"
#include "caps_colorControl.h"
#include "caps_switch.h"
#include "caps_formaldehydeMeasurement.h"

static caps_thermostatMode_data_t *cap_thermostatMode_data;
static caps_switchLevel_data_t *cap_switchLevel_data;
static caps_threeAxis_data_t *cap_threeAxis_data;
static caps_colorControl_data_t *cap_colorControl_data;
static caps_switch_data_t *cap_switch_data;
static caps_formaldehydeMeasurement_data_t *cap_formaldehyde_data;

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]	asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]	asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]	asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]		asm("_binary_device_info_json_end");

static iot_status_t g_iot_status;
static iot_stat_lv_t g_iot_stat_lv;
static bool is_connected = false;

IOT_CTX *ctx = NULL;

void init_capabilities_value()
{
	//thermostatMode
	char *thermostatMode_supportedMode[] = {"auto", "off"};
	cap_thermostatMode_data->set_thermostatMode_value(cap_thermostatMode_data, caps_helper_thermostatMode.attr_thermostatMode.values[CAP_ENUM_THERMOSTATMODE_THERMOSTATMODE_VALUE_AUTO]);
	cap_thermostatMode_data->set_supportedThermostatModes_value(cap_thermostatMode_data,
			(const char **)thermostatMode_supportedMode,
			sizeof(thermostatMode_supportedMode)/sizeof(char *));

	//switchLevel
	cap_switchLevel_data->set_level_value(cap_switchLevel_data, 50);
	cap_switchLevel_data->set_level_unit(cap_switchLevel_data, caps_helper_switchLevel.attr_level.units[CAP_ENUM_SWITCHLEVEL_LEVEL_UNIT_PERCENT]);

	//threeAxis
	cap_threeAxis_data->set_threeAxis_value(cap_threeAxis_data, 0, 0, 0);

	//colorControl
	cap_colorControl_data->set_color_value(cap_colorControl_data, 0, 100);

	//switch
	cap_switch_data->set_switch_value(cap_switch_data, caps_helper_switch.attr_switch.values[CAP_ENUM_SWITCH_SWITCH_VALUE_OFF]);

	//formaldehydeMeasurement
	cap_formaldehyde_data->set_formaldehydeLevel_value(cap_formaldehyde_data, 0);
	cap_formaldehyde_data->set_formaldehydeLevel_unit(cap_formaldehyde_data, caps_helper_formaldehydeMeasurement.attr_formaldehydeLevel.units[CAP_ENUM_FORMALDEHYDEMEASUREMENT_FORMALDEHYDELEVEL_UNIT_MG_PER_M3]);
}

static void cap_thermostatMode_cmd_cb(struct caps_thermostatMode_data *caps_data)
{
	const char *value = caps_data->get_thermostatMode_value(caps_data);
	printf("%s : value : %s", __func__, value);
}
static void cap_switchLevel_cmd_cb(struct caps_switchLevel_data *caps_data)
{
	int value = caps_data->get_level_value(caps_data);
	printf("%s : value : %d", __func__, value);
}
static void cap_colorControl_cmd_cb(struct caps_colorControl_data *caps_data)
{
	double hue = caps_data->get_hue_value(caps_data);
	double saturation = caps_data->get_saturation_value(caps_data);
	printf("%s : %d, %d", __func__, (int)hue, (int)saturation);
}
static void cap_switch_cmd_cb(struct caps_switch_data *caps_data)
{
	const char *value = caps_data->get_switch_value(caps_data);
	printf("%s : value : %s", __func__, value);
}


void send_capabilities_value()
{
	static int send_count = 0;
	send_count++;

	int thermostat_state = send_count % CAP_ENUM_THERMOSTATMODE_THERMOSTATMODE_VALUE_MAX;
	cap_thermostatMode_data->set_thermostatMode_value(cap_thermostatMode_data, caps_helper_thermostatMode.attr_thermostatMode.values[thermostat_state]);
	cap_thermostatMode_data->attr_thermostatMode_send(cap_thermostatMode_data);

	int level_value = (send_count * 25) % 100;
	cap_switchLevel_data->set_level_value(cap_switchLevel_data, level_value);
	cap_switchLevel_data->attr_level_send(cap_switchLevel_data);

	int x = (send_count * 2000) % 10000;
	int y = (send_count * 2000) % 10000;
	int z = (send_count * 2000) % 10000;
	cap_threeAxis_data->set_threeAxis_value(cap_threeAxis_data, x, y, z);
	cap_threeAxis_data->attr_threeAxis_send(cap_threeAxis_data);

	double hue = (send_count * 25) % 100; 
	double saturation = (send_count * 25) % 100;
	cap_colorControl_data->set_color_value(cap_colorControl_data, hue, saturation);
	cap_colorControl_data->attr_color_send(cap_colorControl_data);

	int switch_state = send_count % CAP_ENUM_SWITCH_SWITCH_VALUE_MAX;
	cap_switch_data->set_switch_value(cap_switch_data, caps_helper_switch.attr_switch.values[switch_state]);
	cap_switch_data->attr_switch_send(cap_switch_data);

	int formaldehyde_value = (send_count * 100000 % 1000000);
	cap_formaldehyde_data->set_formaldehydeLevel_value(cap_formaldehyde_data, formaldehyde_value);
	cap_formaldehyde_data->attr_formaldehydeLevel_send(cap_formaldehyde_data);
}

static void iot_status_cb(iot_status_t status,
		iot_stat_lv_t stat_lv, void *usr_data)
{
	g_iot_status = status;
	g_iot_stat_lv = stat_lv;

	printf("status: %d, stat: %d\n", g_iot_status, g_iot_stat_lv);

	if ((status == IOT_STATUS_CONNECTING)&&(stat_lv == IOT_STAT_LV_DONE)) {
		printf("connect done!\n");
		is_connected = true;
	} else {
		is_connected = false;
	}
	switch(status)
	{
		case IOT_STATUS_CONNECTING:
		case IOT_STATUS_NEED_INTERACT:
		case IOT_STATUS_IDLE:
		default:
			break;
	}
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

static void app_main_task(void *arg)
{
	int timedelay_msec = 30000;
	for (;;) {
		if (is_connected) {
			send_capabilities_value();
		}
		vTaskDelay(timedelay_msec / portTICK_PERIOD_MS);
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
	  //create a handle to process capability
	  //register a callback function to process capability command when it comes from the SmartThings Server.
	  //needed when it is necessary to keep monitoring the device status
	  //process on-boarding procedure. There is nothing more to do on the app side than call the API.
	 */

	unsigned char *onboarding_config = (unsigned char *) onboarding_config_start;
	unsigned int onboarding_config_len = onboarding_config_end - onboarding_config_start;
	unsigned char *device_info = (unsigned char *) device_info_start;
	unsigned int device_info_len = device_info_end - device_info_start;
	IOT_CAP_HANDLE *handle = NULL;
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
	//	if needed, implement init_callback function (cap_switch_init_cb)
	cap_thermostatMode_data = caps_thermostatMode_initialize(ctx, "main", NULL, NULL);
	cap_switchLevel_data = caps_switchLevel_initialize(ctx, "main", NULL, NULL);
	cap_threeAxis_data = caps_threeAxis_initialize(ctx, "main", NULL, NULL);
	cap_colorControl_data = caps_colorControl_initialize(ctx, "main", NULL, NULL);
	cap_switch_data = caps_switch_initialize(ctx, "main", NULL, NULL);
	cap_formaldehyde_data = caps_formaldehydeMeasurement_initialize(ctx, "main", NULL, NULL);
	// 3. register a callback function to process capability command when it comes from the SmartThings Server
	//	if needed, implement callback function (cap_switch_cmd_cb)
	cap_thermostatMode_data->cmd_setThermostatMode_usr_cb = cap_thermostatMode_cmd_cb;
	cap_switchLevel_data->cmd_setLevel_usr_cb = cap_switchLevel_cmd_cb;
	cap_colorControl_data->cmd_setColor_usr_cb = cap_colorControl_cmd_cb;
	cap_switch_data->cmd_on_usr_cb = cap_switch_cmd_cb;
	cap_switch_data->cmd_off_usr_cb = cap_switch_cmd_cb;

	init_capabilities_value();

	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(app_main_task, "app_main_task", 2048, (void *)handle, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

}
