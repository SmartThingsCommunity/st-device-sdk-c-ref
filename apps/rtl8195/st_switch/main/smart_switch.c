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
#include <stdbool.h>
#include "st_dev.h"
#include "PinNames.h"
#include <gpio_api.h>
#include "gpio_irq_api.h"
#include "gpio_irq_ex_api.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

IOT_CTX *ctx = NULL;
IOT_CAP_HANDLE *handle = NULL;

enum smartswitch_switch_onoff_state {
	SMARTSWITCH_SWITCH_OFF = 0,
	SMARTSWITCH_SWITCH_ON = 1,
};

enum button_gpio_state {
    BUTTON_GPIO_RELEASED = 0,
    BUTTON_GPIO_PRESSED = 1,
};

static int smartswitch_switch_state = SMARTSWITCH_SWITCH_ON;
#define BUTTON_DEBOUNCE_TIME_MS 20

#define GPIO_OUTPUT_COLORLED_0 PC_5
#define GPIO_OUTPUT_NOTIFICATION_LED PC_4
#define GPIO_INPUT_BUTTON PC_1
#define  BUTTON_SHORT_PRESS	1

gpio_t gpio_ctrl_noti;
gpio_t gpio_ctrl_zero;
gpio_t gpio_ctrl_button;

static xQueueHandle button_event_queue = NULL;

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
	return smartswitch_switch_state;
}

static void led_state_switch(enum smartswitch_switch_onoff_state state)
{
	gpio_write(&gpio_ctrl_noti, state);
	smartswitch_switch_state = state;
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

static void button_event(IOT_CAP_HANDLE *handle, uint32_t type, uint32_t count)
{
	if (type == BUTTON_SHORT_PRESS) {
		printf("Button short press, count: %d\r\n", count);
		switch(count) {
			case 1:
				/* change switch state and LED state */
				if (smartswitch_switch_state == SMARTSWITCH_SWITCH_ON) {
					led_state_switch(SMARTSWITCH_SWITCH_OFF);
					led_switch(0);
				} else {
					led_state_switch(SMARTSWITCH_SWITCH_ON);
					led_switch(1);
				}
				break;
			default:
				break;
		}
	}
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

void button_isr_handler(void *arg)
{
	static uint32_t last_time_ms = 0;
	uint32_t now_ms = xTaskGetTickCountFromISR() * portTICK_PERIOD_MS;

	/* check debounce time to ignore small ripple of currunt */
	if (now_ms - last_time_ms > BUTTON_DEBOUNCE_TIME_MS) {
		last_time_ms = now_ms;
		xQueueSendFromISR(button_event_queue, &now_ms, NULL);
	}
}

bool get_button_event(int* button_event_type, int* button_event_count)
{
	static uint32_t press_count = 0;
	uint32_t button_time_ms = 0;
	uint32_t now_ms = 0;
	uint32_t gpio_value = 0;

	now_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
	if (xQueueReceive(button_event_queue, &button_time_ms, 0)) {
		vTaskDelay(BUTTON_DEBOUNCE_TIME_MS / portTICK_PERIOD_MS);
		gpio_value = gpio_read(&gpio_ctrl_button);
		if (gpio_value == BUTTON_GPIO_PRESSED){
			*button_event_type = BUTTON_SHORT_PRESS;
			*button_event_count = 1;
			return true;
		}
	}
	return false;
}

void led_button_init(void)
{
	//0 init
	gpio_init(&gpio_ctrl_zero, GPIO_OUTPUT_COLORLED_0);
	gpio_mode(&gpio_ctrl_zero, PullDown);
	gpio_dir(&gpio_ctrl_zero, PIN_OUTPUT);
	gpio_write(&gpio_ctrl_zero, 0);

	//notify led init
	gpio_init(&gpio_ctrl_noti, GPIO_OUTPUT_NOTIFICATION_LED);
	gpio_mode(&gpio_ctrl_noti, PullNone);
	gpio_dir(&gpio_ctrl_noti, PIN_OUTPUT);
	gpio_write(&gpio_ctrl_noti, 1);
	led_state_switch(SMARTSWITCH_SWITCH_ON);

	//button init
	button_event_queue = xQueueCreate(10, sizeof(uint32_t));
	gpio_irq_init(&gpio_ctrl_button, GPIO_INPUT_BUTTON, button_isr_handler, (uint32_t)(&gpio_ctrl_button));
	gpio_irq_set(&gpio_ctrl_button, IRQ_LOW, 1);   // LOW Trigger
	gpio_irq_enable(&gpio_ctrl_button);

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

void iot_noti_cb(iot_noti_data_t *noti_data, void *noti_usr_data)
{
	printf("Notification message received\r\n");

	if (noti_data->type == IOT_NOTI_TYPE_DEV_DELETED) {
		printf("[device deleted]\r\n");
	} else if (noti_data->type == IOT_NOTI_TYPE_RATE_LIMIT) {
		printf("[rate limit] Remaining time:%d, sequence number:%d\r\n",
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

		vTaskDelay(10 / portTICK_PERIOD_MS);
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

	printf("APP_MAIN_START >>PT<<\r\n");

	// 1. create a iot context
	ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
	if (ctx != NULL) {
		iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
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

	printf("IOT_INIT_DONE >>PT<<\r\n");

	led_button_init();
	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(smartswitch_task, "smartswitch_task", 2048, (void *)handle, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

	printf("IOT_CONNECT_DONE >>PT<<\r\n");

}
