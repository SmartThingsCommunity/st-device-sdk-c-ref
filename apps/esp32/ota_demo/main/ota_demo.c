/* ***************************************************************************
 *
 * Copyright (c) 2020 Samsung Electronics All Rights Reserved.
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


#include "st_dev.h"
#include "device_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "string.h"
#include "ota_util.h"

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]	asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]	asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]	asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]		asm("_binary_device_info_json_end");

enum smartswitch_switch_onoff_state {
	SMARTSWITCH_SWITCH_OFF = 0,
	SMARTSWITCH_SWITCH_ON = 1,
};

static int32_t smartswitch_switch_state = SMARTSWITCH_SWITCH_ON;

static iot_status_t g_iot_status;

static int noti_led_mode = LED_ANIMATION_MODE_IDLE;

/* TODO: Need 'get_ctx' function (get ctx from cap_handle) */
IOT_CTX *ctx = NULL;

TaskHandle_t ota_task_handle = NULL;

IOT_CAP_HANDLE *ota_cap_handle = NULL;

static void change_switch_state(int32_t state)
{
	/* change state */
	smartswitch_switch_state = state;
	if(state == SMARTSWITCH_SWITCH_ON) {
		gpio_set_level(GPIO_OUTPUT_MAINLED, MAINLED_GPIO_ON);
		gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
	} else {
		gpio_set_level(GPIO_OUTPUT_MAINLED, MAINLED_GPIO_OFF);
		gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
	}
}

static void send_switch_cap_evt(IOT_CAP_HANDLE *handle, int32_t state)
{
	IOT_EVENT *switch_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	/* Setup switch onoff state */
	if(state == SMARTSWITCH_SWITCH_ON) {
		switch_evt = st_cap_attr_create_string("switch", "on", NULL);
	} else {
		switch_evt = st_cap_attr_create_string("switch", "off", NULL);
	}

	/* Send switch onoff event */
	sequence_no = st_cap_attr_send(handle, evt_num, &switch_evt);
	if (sequence_no < 0)
		printf("fail to send switch onoff data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(switch_evt);
}

void cap_available_version_set(char *available_version)
{
	IOT_EVENT *init_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	if (!available_version) {
		printf("invalid parameter");
		return;
	}

	/* Setup switch on state */
	init_evt = st_cap_attr_create_string("availableVersion", available_version, NULL);

	/* Send switch on event */
	sequence_no = st_cap_attr_send(ota_cap_handle, evt_num, &init_evt);
	if (sequence_no < 0)
		printf("fail to send init_data\n");

	st_cap_attr_free(init_evt);
}

void cap_current_version_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	IOT_EVENT *init_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	/* Setup switch on state */
	init_evt = st_cap_attr_create_string("currentVersion", OTA_FIRMWARE_VERSION, NULL);

	/* Send switch on event */
	sequence_no = st_cap_attr_send(handle, evt_num, &init_evt);
	if (sequence_no < 0)
		printf("fail to send init_data\n");

	st_cap_attr_free(init_evt);
}

static void _task_fatal_error()
{
	ota_task_handle = NULL;
	printf("Exiting task due to fatal error...");
	(void)vTaskDelete(NULL);

	while (1) {
		;
	}
}
static void ota_task_func(void * pvParameter)
{
	printf("\n Starting OTA...\n");

	esp_err_t ret = ota_https_update_device();
	if (ret != ESP_OK) {
		printf("Firmware Upgrades Failed (%d) \n", ret);
		_task_fatal_error();
	}

	printf("Prepare to restart system!");
	esp_restart();
}

void update_firmware_cmd_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	ota_nvs_flash_init();

	xTaskCreate(&ota_task_func, "ota_task_func", 8096, NULL, 5, &ota_task_handle);
}

static void ota_polling_task_func(void *arg)
{
	while (1) {

		vTaskDelay(30000 / portTICK_PERIOD_MS);

		printf("\n Starting check new version...\n");

		if (ota_task_handle != NULL) {
			printf("Device is updating.. \n");
			continue;
		}

		if (g_iot_status != IOT_STATUS_CONNECTING) {
			printf("Device is not connected to cloud.. \n");
			continue;
		}

		char *read_data = NULL;
		unsigned int read_data_len = 0;

		esp_err_t ret = ota_https_read_version_info(&read_data, &read_data_len);
		if (ret == ESP_OK) {
			char *available_version = NULL;

			esp_err_t err = ota_api_get_available_version(read_data, read_data_len, &available_version);
			if (err != ESP_OK) {
				printf("ota_api_get_available_version is failed : %d\n", err);
				continue;
			}

			cap_available_version_set(available_version);

			if (available_version)
				free(available_version);
		}

		/* Set polling period */
		unsigned int polling_day = ota_get_polling_period_day();
		unsigned int task_delay_sec = polling_day * 24 * 3600;
		vTaskDelay(task_delay_sec * 1000 / portTICK_PERIOD_MS);
	}
}

static void button_event(IOT_CAP_HANDLE *handle, uint32_t type, uint32_t count)
{
	if (type == BUTTON_SHORT_PRESS) {
		printf("Button short press, count: %d\n", count);
		switch(count) {
			case 1:
				if (g_iot_status == IOT_STATUS_NEED_INTERACT) {
					st_conn_ownership_confirm(ctx, true);
					gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
					noti_led_mode = LED_ANIMATION_MODE_IDLE;
				} else {
					/* change switch state and LED state */
					if (smartswitch_switch_state == SMARTSWITCH_SWITCH_ON) {
						change_switch_state(SMARTSWITCH_SWITCH_OFF);
						send_switch_cap_evt(handle, SMARTSWITCH_SWITCH_OFF);
					} else {
						change_switch_state(SMARTSWITCH_SWITCH_ON);
						send_switch_cap_evt(handle, SMARTSWITCH_SWITCH_ON);
					}
				}
				break;

			default:
				led_blink(GPIO_OUTPUT_NOTIFICATION_LED, 100, count);
				break;
		}
	} else if (type == BUTTON_LONG_PRESS) {
		printf("Button long press, count: %d\n", count);
		led_blink(GPIO_OUTPUT_NOTIFICATION_LED, 100, 3);
		/* clean-up provisioning & registered data with reboot option*/
		st_conn_cleanup(ctx, true);
	}
}

static void iot_status_cb(iot_status_t status,
		iot_stat_lv_t stat_lv, void *usr_data)
{
	g_iot_status = status;
	printf("iot_status: %d, lv: %d\n", status, stat_lv);

	switch(status)
	{
		case IOT_STATUS_NEED_INTERACT:
			noti_led_mode = LED_ANIMATION_MODE_FAST;
			break;
		case IOT_STATUS_IDLE:
		case IOT_STATUS_CONNECTING:
			noti_led_mode = LED_ANIMATION_MODE_IDLE;
			if (smartswitch_switch_state == SMARTSWITCH_SWITCH_ON) {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
			} else {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
			}
			break;
		default:
			break;
	}
}

void cap_switch_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	IOT_EVENT *init_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	/* Setup switch on state */
	init_evt = st_cap_attr_create_string("switch", "on", NULL);

	/* Send switch on event */
	sequence_no = st_cap_attr_send(handle, evt_num, &init_evt);
	if (sequence_no < 0)
		printf("fail to send init_data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(init_evt);
}

void cap_switch_cmd_off_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	IOT_EVENT *off_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	change_switch_state(SMARTSWITCH_SWITCH_OFF);

	/* Setup switch off state */
	off_evt = st_cap_attr_create_string("switch", "off", NULL);

	/* Send switch off event */
	sequence_no = st_cap_attr_send(handle, evt_num, &off_evt);
	if (sequence_no < 0)
		printf("fail to send off_data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(off_evt);
}


void cap_switch_cmd_on_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	IOT_EVENT *on_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	change_switch_state(SMARTSWITCH_SWITCH_ON);

	/* Setup switch on state */
	on_evt = st_cap_attr_create_string("switch", "on", NULL);

	/* Send switch on event */
	sequence_no = st_cap_attr_send(handle, evt_num, &on_evt);
	if (sequence_no < 0)
		printf("fail to send on_data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(on_evt);
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

static void smartswitch_task(void *arg)
{
	IOT_CAP_HANDLE *handle = (IOT_CAP_HANDLE *)arg;

	int button_event_type;
	int button_event_count;

	for (;;) {
		if (get_button_event(&button_event_type, &button_event_count)) {
			button_event(handle, button_event_type, button_event_count);
		}

		if (noti_led_mode != LED_ANIMATION_MODE_IDLE) {
			change_led_state(noti_led_mode);
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
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
	IOT_CAP_HANDLE *handle = NULL;
	int iot_err;

	// 1. create a iot context
	ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
	if (ctx != NULL) {
		iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
		if (iot_err)
			printf("fail to set notification callback function\n");

	// 2. create a handle to process capability
	//	implement init_callback function (cap_switch_init_cb)
		handle = st_cap_handle_init(ctx, "main", "switch", cap_switch_init_cb, NULL);

		ota_cap_handle = st_cap_handle_init(ctx, "main", "firmwareUpdate", cap_current_version_init_cb, NULL);

	// 3. register a callback function to process capability command when it comes from the SmartThings Server
	//	implement callback function (cap_switch_cmd_off_cb)
		iot_err = st_cap_cmd_set_cb(handle, "off", cap_switch_cmd_off_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\n");

	//	implement callback function (cap_switch_cmd_on_cb)
		iot_err = st_cap_cmd_set_cb(handle, "on", cap_switch_cmd_on_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for on\n");

		iot_err = st_cap_cmd_set_cb(ota_cap_handle, "updateFirmware", update_firmware_cmd_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for updateFirmware");

	} else {
		printf("fail to create the iot_context\n");
	}

	gpio_init();

	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(smartswitch_task, "smartswitch_task", 2048, (void *)handle, 10, NULL);

	// 5. needed to check whether firmware update is available
	xTaskCreate(ota_polling_task_func, "ota_polling_task_func", 8096, (void *)handle, 5, NULL);

	// 6. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

}
