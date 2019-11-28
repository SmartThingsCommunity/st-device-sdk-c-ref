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

#include "st_dev.h"
#include "device_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]	asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]	asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]	asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]		asm("_binary_device_info_json_end");

static iot_status_t g_iot_status;

static int noti_led_mode = LED_ANIMATION_MODE_IDLE;

IOT_CTX *ctx = NULL;

IOT_CAP_HANDLE* alarm_handle = NULL;
IOT_CAP_HANDLE* air_quality_handle = NULL;
IOT_CAP_HANDLE* carbon_dioxide_handle = NULL;
IOT_CAP_HANDLE* carbon_monoxide_handle = NULL;
IOT_CAP_HANDLE* formaldehyde_handle = NULL;

enum {
	ALARM_STATE_OFF = 0,
	ALARM_STATE_BOTH,
	ALARM_STATE_SIREN,
	ALARM_STATE_STROBE,
};

int g_alarm_state = ALARM_STATE_OFF;

/* dummy value for test */
#define AIR_QUALITY_THRESHOLD 50
#define CARBON_DIOXIDE_THRESHOLD 500
#define CARBON_MONOXIDE_THRESHOLD 1
#define FORMALDEHYDE_THRESHOLD 500
int g_air_quality_value = 25;
int g_carbon_dioxide_value= 250;
int g_carbon_monoxide_value = 0;
int g_formaldehyde_value = 250;

int get_air_quality(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */

	return g_air_quality_value;
}

int get_carbon_dioxide(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */

	return g_carbon_dioxide_value;
}

int get_carbon_monoxide(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */

	return g_carbon_monoxide_value;
}

int get_formaldehyde(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the value from your sensor
	 */

	return g_formaldehyde_value;
}

int get_alarm_state(void)
{
	return g_alarm_state;
}

void change_alarm_state(int state)
{
	switch(state)
	{
		case ALARM_STATE_BOTH:
			change_siren_state(1);
			change_strobe_state(1);
			break;
		case ALARM_STATE_SIREN:
			change_siren_state(1);
			change_strobe_state(0);
			break;
		case ALARM_STATE_STROBE:
			change_siren_state(0);
			change_strobe_state(1);
			break;
		case ALARM_STATE_OFF:
			change_siren_state(0);
			change_strobe_state(0);
			break;
		default:
			change_siren_state(0);
			change_strobe_state(0);
			printf("error: unexpected alarm state : %d\n", state);
	}
	g_alarm_state = state;
}

void send_alarm_capability(int state)
{
	IOT_CAP_HANDLE *handle = alarm_handle;
	IOT_EVENT *cap_evt;
	int32_t sequence_no;
	switch(state)
	{
		case ALARM_STATE_BOTH:
			cap_evt = st_cap_attr_create_string("alarm", "both", NULL);
			break;
		case ALARM_STATE_SIREN:
			cap_evt = st_cap_attr_create_string("alarm", "siren", NULL);
			break;
		case ALARM_STATE_STROBE:
			cap_evt = st_cap_attr_create_string("alarm", "strobe", NULL);
			break;
		case ALARM_STATE_OFF:
			cap_evt = st_cap_attr_create_string("alarm", "off", NULL);
			break;
		default:
			printf("error: unexpected alarm state : %d\n", state);
			cap_evt = st_cap_attr_create_string("alarm", "off", NULL);
	}

	sequence_no = st_cap_attr_send(handle, 1, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

void send_air_quality_capability(int value)
{
	IOT_CAP_HANDLE *handle = air_quality_handle;
	IOT_EVENT *cap_evt;
	int32_t sequence_no;

	cap_evt = st_cap_attr_create_int("airQuality", value, "CAQI");
	sequence_no = st_cap_attr_send(handle, 1, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

void send_carbon_dioxide_capability(int value)
{
	IOT_CAP_HANDLE *handle = carbon_dioxide_handle;
	IOT_EVENT *cap_evt;
	int32_t sequence_no;

	cap_evt = st_cap_attr_create_int("carbonDioxide", value, "ppm");
	sequence_no = st_cap_attr_send(handle, 1, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

void send_carbon_monoxide_capability(int value)
{
	IOT_CAP_HANDLE *handle = carbon_monoxide_handle;
	IOT_EVENT *cap_evt;
	int32_t sequence_no;

	if (value)
		cap_evt = st_cap_attr_create_string("carbonMonoxide", "detected", NULL);
	else
		cap_evt = st_cap_attr_create_string("carbonMonoxide", "clear", NULL);

	sequence_no = st_cap_attr_send(handle, 1, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

void send_formaldehyde_capability(int value)
{
	IOT_CAP_HANDLE *handle = formaldehyde_handle;
	IOT_EVENT *cap_evt;
	int32_t sequence_no;

	cap_evt = st_cap_attr_create_int("formaldehydeLevel", value, "ppm");

	sequence_no = st_cap_attr_send(handle, 1, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}


static void button_event(uint32_t type, uint32_t count)
{
	if (type == BUTTON_SHORT_PRESS) {
		printf("Button short press, count: %d\n", count);
		switch(count) {
			case 1:
				change_alarm_state(ALARM_STATE_OFF);
				send_alarm_capability(ALARM_STATE_OFF);
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
			gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
			break;
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

#define MONITOR_DELAY_MS 60000
static void app_task(void *arg)
{
	int button_event_type;
	int button_event_count;

	int air_quality_value;
	int carbon_dioxide_value;
	int carbon_monoxide_value;
	int formaldehyde_value;
	int alarm_state;

	TimeOut_t monitor_timeout;
	TickType_t monitor_delay_tick = pdMS_TO_TICKS(MONITOR_DELAY_MS);

	vTaskSetTimeOutState(&monitor_timeout);
	for (;;) {
		if (get_button_event(&button_event_type, &button_event_count)) {
			button_event(button_event_type, button_event_count);
		}

		if (xTaskCheckForTimeOut(&monitor_timeout, &monitor_delay_tick ) != pdFALSE) {
			vTaskSetTimeOutState(&monitor_timeout);
			monitor_delay_tick = pdMS_TO_TICKS(MONITOR_DELAY_MS);

			air_quality_value = get_air_quality();
			carbon_dioxide_value= get_carbon_dioxide();
			carbon_monoxide_value = get_carbon_monoxide();
			formaldehyde_value = get_formaldehyde();

			if ( (air_quality_value >= AIR_QUALITY_THRESHOLD) ||
					(carbon_dioxide_value >= CARBON_DIOXIDE_THRESHOLD) ||
					(carbon_monoxide_value >= CARBON_MONOXIDE_THRESHOLD) ||
					(formaldehyde_value >= FORMALDEHYDE_THRESHOLD) ) {
			 	alarm_state = ALARM_STATE_BOTH;
			} else {
				alarm_state = ALARM_STATE_OFF;
			}

			change_alarm_state(alarm_state);
			send_alarm_capability(alarm_state);
			send_air_quality_capability(air_quality_value);
			send_carbon_dioxide_capability(carbon_dioxide_value);
			send_carbon_monoxide_capability(carbon_monoxide_value);
			send_formaldehyde_capability(formaldehyde_value);
		}

		if (noti_led_mode != LED_ANIMATION_MODE_IDLE) {
			change_led_state(noti_led_mode);
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

/* capability command callback functions */
void cap_alarm_cmd_both_cb(IOT_CAP_HANDLE *handle,
		            iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	change_alarm_state(ALARM_STATE_BOTH);
	send_alarm_capability(ALARM_STATE_BOTH);
}

void cap_alarm_cmd_siren_cb(IOT_CAP_HANDLE *handle,
		            iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	change_alarm_state(ALARM_STATE_SIREN);
	send_alarm_capability(ALARM_STATE_SIREN);
}

void cap_alarm_cmd_strobe_cb(IOT_CAP_HANDLE *handle,
		            iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	change_alarm_state(ALARM_STATE_STROBE);
	send_alarm_capability(ALARM_STATE_STROBE);
}

void cap_alarm_cmd_off_cb(IOT_CAP_HANDLE *handle,
		            iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	change_alarm_state(ALARM_STATE_OFF);
	send_alarm_capability(ALARM_STATE_OFF);
}

/* capability init callback functions */
void cap_alarm_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_alarm_capability(get_alarm_state());
}

void cap_air_quality_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_air_quality_capability(get_air_quality());
}

void cap_carbon_dioxide_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_carbon_dioxide_capability(get_carbon_dioxide());
}

void cap_carbon_monoxide_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_carbon_monoxide_capability(get_carbon_monoxide());
}

void cap_formaldehyde_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_formaldehyde_capability(get_formaldehyde());
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

	ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
	if (ctx != NULL) {
		iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
		if (iot_err)
			printf("fail to set notification callback function\n");

		alarm_handle = st_cap_handle_init(ctx, "main", "alarm", cap_alarm_init_cb, NULL);
		iot_err = st_cap_cmd_set_cb(alarm_handle, "both", cap_alarm_cmd_both_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\n");
		iot_err = st_cap_cmd_set_cb(alarm_handle, "off", cap_alarm_cmd_off_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\n");
		iot_err = st_cap_cmd_set_cb(alarm_handle, "siren", cap_alarm_cmd_siren_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\n");
		iot_err = st_cap_cmd_set_cb(alarm_handle, "strobe", cap_alarm_cmd_strobe_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\n");

		air_quality_handle = st_cap_handle_init(ctx, "main", "airQualitySensor",
				cap_air_quality_init_cb, NULL);
		carbon_dioxide_handle = st_cap_handle_init(ctx, "main", "carbonDioxideMeasurement",
				cap_carbon_dioxide_init_cb, NULL);
		carbon_monoxide_handle = st_cap_handle_init(ctx, "main", "carbonMonoxideDetector",
				cap_carbon_monoxide_init_cb, NULL);
		formaldehyde_handle = st_cap_handle_init(ctx, "main", "formaldehydeMeasurement",
				cap_formaldehyde_init_cb, NULL);

	} else {
		printf("fail to create the iot_context\n");
	}

	gpio_init();

	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(app_task, "app_task", 2048, NULL, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

}
