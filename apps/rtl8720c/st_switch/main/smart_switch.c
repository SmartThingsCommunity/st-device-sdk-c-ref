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

#include "platform_stdlib.h"
#include "st_dev.h"
#include "PinNames.h"
#include <gpio_api.h>

IOT_CTX *ctx = NULL;
IOT_CAP_HANDLE * handle;

enum smartswitch_switch_onoff_state {
	SMARTSWITCH_SWITCH_OFF = 0,
	SMARTSWITCH_SWITCH_ON = 1,
};

#define LED_PORT PA_4
gpio_t gpio_ctrl;
int led_onoff = 0;
IOT_CAP_HANDLE *handle = NULL;
// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]	asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]	asm("_binary_onboarding_config_json_end");
extern unsigned int _binary_onboarding_config_json_size;
// device_info_start is null-terminated string
extern const uint8_t device_info_start[]	asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]		asm("_binary_device_info_json_end");
extern unsigned int _binary_device_info_json_size;


int led_status()
{
	return led_onoff;
}

static void led_state_switch(enum smartswitch_switch_onoff_state state)
{
	gpio_write(&gpio_ctrl, state);
	led_onoff = state;
}

void led_switch(int on)
{
	enum smartswitch_switch_onoff_state state = on ? SMARTSWITCH_SWITCH_ON: SMARTSWITCH_SWITCH_OFF;
	const char * switch_str = on ? "on" : "off";
	IOT_EVENT *switch_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	/* Setup switch state */
	switch_evt = st_cap_attr_create_string("switch", switch_str, NULL);

	/* Send switch off event */
	sequence_no = st_cap_attr_send(handle, 1, &switch_evt);
	st_cap_attr_free(switch_evt);
	if (sequence_no < 0) {
		printf("fail to send off_data\r\n");
	} else {
		printf( "Sequence number return : %d\r\n", sequence_no);
		led_state_switch(state);
	}
}

void led_init()
{
	gpio_init(&gpio_ctrl, LED_PORT);
	gpio_mode(&gpio_ctrl, PullNone);
	gpio_dir(&gpio_ctrl, PIN_OUTPUT);
	gpio_write(&gpio_ctrl, 1);
	led_state_switch(SMARTSWITCH_SWITCH_ON);
}

static void iot_status_cb(iot_status_t status,
		iot_stat_lv_t stat_lv, void *usr_data)
{
	switch(status)
	{
		case IOT_STATUS_NEED_INTERACT:
			printf("come here \r\n");
			break;
		case IOT_STATUS_IDLE:
			printf("come here \r\n");
			break;
		case IOT_STATUS_CONNECTING:
			printf("come here \r\n");
			break;
		default:
			printf("come here \r\n");
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
		printf("fail to send init_data\r\n");

	printf("Sequence number return : %d\r\n", sequence_no);
	st_cap_attr_free(init_evt);
}

void cap_switch_cmd_off_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	led_switch(0);
}


void cap_switch_cmd_on_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	led_switch(1);
}

void cap_switch_noti_cb(iot_noti_data_t *noti_data, void *noti_usr_data)
{
	printf("Notification message received\r\n");

	if (noti_data->type == IOT_NOTI_TYPE_DEV_DELETED) {
		printf("[device deleted]\r\n");
	} else if (noti_data->type == IOT_NOTI_TYPE_RATE_LIMIT) {
		printf("[rate limit] Remaining time:%d, sequence number:%d\r\n",
			noti_data->raw.rate_limit.remainingTime, noti_data->raw.rate_limit.sequenceNumber);
	}
}

void app_main(void)
{
	/**
	  SmartThings Device Kit(STDK) aims to make it easier to develop IoT devices by providing
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
		iot_err = st_conn_set_noti_cb(ctx, cap_switch_noti_cb, NULL);
		if (iot_err)
			printf("fail to set notification callback function\r\n");

	// 2. create a handle to process capability
	//	implement init_callback function (cap_switch_init_cb)
		handle = st_cap_handle_init(ctx, "main", "switch", cap_switch_init_cb, NULL);

	// 3. register a callback function to process capability command when it comes from the SmartThings Server
	//	implement callback function (cap_switch_cmd_off_cb)
		iot_err = st_cap_cmd_set_cb(handle, "off", cap_switch_cmd_off_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\r\n");

	//	implement callback function (cap_switch_cmd_on_cb)
		iot_err = st_cap_cmd_set_cb(handle, "on", cap_switch_cmd_on_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for on\r\n");

	} else {
		printf("fail to create the iot_context\r\n");
	}

	led_init();

	// 4. needed when it is necessary to keep monitoring the device status
	//xTaskCreate(smartswitch_task, "smartswitch_task", 2048, (void *)handle, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

}
