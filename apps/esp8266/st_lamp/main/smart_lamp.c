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

enum smartlamp_switch_onoff_state {
	SMARTLAMP_SWITCH_OFF = 0,
	SMARTLAMP_SWITCH_ON = 1,
};

static int smartlamp_switch_state = SMARTLAMP_SWITCH_ON;
static double smartlamp_color_hue = 0;
static double smartlamp_color_saturation = 100;
static int smartlamp_color_level = 50;
static int smartlamp_color_red = 255;
static int smartlamp_color_green = 255;
static int smartlamp_color_blue = 255;
static int smartlamp_brightness_level = 100;

static iot_status_t g_iot_status;

static int noti_led_mode = LED_ANIMATION_MODE_IDLE;

IOT_CTX* ctx = NULL;


/* Send integer type capability to SmartThings Sever */
static void send_capability_integer(IOT_CAP_HANDLE *handle, char* attribute_name, int value)
{
	IOT_EVENT *cap_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	cap_evt = st_cap_attr_create_int(attribute_name, value, NULL);

	sequence_no = st_cap_attr_send(handle, evt_num, &cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt);
}

static void send_color_capability(IOT_CAP_HANDLE *handle, double hue, double saturation)
{
	IOT_EVENT *cap_evt[2];
	uint8_t evt_num = 2;
	int32_t sequence_no;

	cap_evt[0] = st_cap_attr_create_number("hue", hue, NULL);
	cap_evt[1] = st_cap_attr_create_number("saturation", saturation, NULL);

	sequence_no = st_cap_attr_send(handle, evt_num, cap_evt);
	if (sequence_no < 0)
		printf("fail to send data\n");

	printf("Sequence number return : %d\n", sequence_no);
	st_cap_attr_free(cap_evt[0]);
	st_cap_attr_free(cap_evt[1]);

}


static void color_led_onoff(int32_t state)
{
	if (state == 0) {
		gpio_set_level(GPIO_OUTPUT_COLORLED_R, 0);
		gpio_set_level(GPIO_OUTPUT_COLORLED_G, 0);
		gpio_set_level(GPIO_OUTPUT_COLORLED_B, 0);
	} else {
		gpio_set_level(GPIO_OUTPUT_COLORLED_R, (smartlamp_color_red > 127));
		gpio_set_level(GPIO_OUTPUT_COLORLED_G, (smartlamp_color_green > 127));
		gpio_set_level(GPIO_OUTPUT_COLORLED_B, (smartlamp_color_blue > 127));
	}
}

static void change_switch_state(int32_t state)
{
	/* change state */
	smartlamp_switch_state = state;
	color_led_onoff(smartlamp_switch_state);
	if(state == SMARTLAMP_SWITCH_ON) {
		gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
		color_led_onoff(1);
	} else {
		gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
		color_led_onoff(0);
	}
}

static void send_switch_cap_evt(IOT_CAP_HANDLE *handle, int32_t state)
{
	IOT_EVENT *switch_evt;
	uint8_t evt_num = 1;
	int32_t sequence_no;

	/* Setup switch onoff state */
	if(state == SMARTLAMP_SWITCH_ON) {
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
					if (smartlamp_switch_state == SMARTLAMP_SWITCH_ON) {
						change_switch_state(SMARTLAMP_SWITCH_OFF);
						send_switch_cap_evt(handle, SMARTLAMP_SWITCH_OFF);
					} else {
						change_switch_state(SMARTLAMP_SWITCH_ON);
						send_switch_cap_evt(handle, SMARTLAMP_SWITCH_ON);
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
			if (smartlamp_switch_state == SMARTLAMP_SWITCH_ON) {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
			} else {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
			}
			break;
		default:
			break;
	}
}


#include "driver/hw_timer.h"
#define PWM_LEVEL 10
uint32_t ulHighFrequencyTimerTicks;
void hw_timer_callback(void *arg)
{
	ulHighFrequencyTimerTicks++;

	color_led_onoff((ulHighFrequencyTimerTicks% (PWM_LEVEL + 1))
			< (smartlamp_switch_state * PWM_LEVEL * smartlamp_brightness_level/100));
}

void cap_color_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_color_capability(handle, smartlamp_color_hue, smartlamp_color_saturation);
}

void cap_color_cmd_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	int tmp_state;
	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);

	smartlamp_color_saturation = cmd_data->cmd_data[0].number;
	smartlamp_color_hue = cmd_data->cmd_data[1].number;

	update_rgb_from_hsl(smartlamp_color_hue, smartlamp_color_saturation, smartlamp_color_level,
					&smartlamp_color_red, &smartlamp_color_green, &smartlamp_color_blue);

	printf("HSL (%lf, %lf, %d), RGB (%d, %d, %d)\n",
			smartlamp_color_hue, smartlamp_color_saturation, smartlamp_color_level,
			smartlamp_color_red, smartlamp_color_green, smartlamp_color_blue);

	tmp_state = smartlamp_switch_state;
	smartlamp_switch_state = SMARTLAMP_SWITCH_ON;


	send_color_capability(handle, smartlamp_color_hue, smartlamp_color_saturation);

	vTaskDelay(LED_BLINK_TIME / portTICK_PERIOD_MS);
	smartlamp_switch_state = tmp_state;
}

void cap_level_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
	send_capability_integer(handle, "level", smartlamp_brightness_level);
}

void level_cmd_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	int tmp_state;

	printf("called [%s] func with : num_args:%u\n",
		__func__, cmd_data->num_args);
	smartlamp_brightness_level = cmd_data->cmd_data[0].integer;

	tmp_state = smartlamp_switch_state;
	smartlamp_switch_state = SMARTLAMP_SWITCH_ON;

	send_capability_integer(handle, "level", smartlamp_brightness_level);

	vTaskDelay(LED_BLINK_TIME / portTICK_PERIOD_MS);
	smartlamp_switch_state = tmp_state;
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

	change_switch_state(SMARTLAMP_SWITCH_OFF);

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

	change_switch_state(SMARTLAMP_SWITCH_ON);

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

static void smartlamp_task(void *arg)
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

//	IOT_CTX* ctx = NULL;
	unsigned char *onboarding_config = (unsigned char *) onboarding_config_start;
	unsigned int onboarding_config_len = onboarding_config_end - onboarding_config_start - 1;
	unsigned char *device_info = (unsigned char *) device_info_start;
	unsigned int device_info_len = device_info_end - device_info_start - 1;
	IOT_CAP_HANDLE* switch_handle = NULL;
	IOT_CAP_HANDLE* color_handle = NULL;
	IOT_CAP_HANDLE* level_handle = NULL;
	int iot_err;

	hw_timer_init(hw_timer_callback, NULL);
	hw_timer_alarm_us(1000, true);

	// 1. create a iot context
	ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
	if (ctx != NULL) {
		iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
		if (iot_err)
			printf("fail to set notification callback function\n");

	// 2. create a handle to process capability
	//	implement init_callback function
		switch_handle = st_cap_handle_init(ctx, "main", "switch", cap_switch_init_cb, NULL);
		color_handle = st_cap_handle_init(ctx, "main", "colorControl", cap_color_init_cb, NULL);
		level_handle = st_cap_handle_init(ctx, "main", "switchLevel", cap_level_init_cb, NULL);

	// 3. register a callback function to process capability command when it comes from the SmartThings Server
	//	implement callback function
		iot_err = st_cap_cmd_set_cb(switch_handle, "off", cap_switch_cmd_off_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for off\n");
		iot_err = st_cap_cmd_set_cb(switch_handle, "on", cap_switch_cmd_on_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for on\n");
		iot_err = st_cap_cmd_set_cb(color_handle, "setColor", cap_color_cmd_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for setcolor\n");
		iot_err = st_cap_cmd_set_cb(level_handle, "setLevel", level_cmd_cb, NULL);
		if (iot_err)
			printf("fail to set cmd_cb for setcolor\n");

	} else {
		printf("fail to create the iot_context\n");
	}

	gpio_init();

	update_rgb_from_hsl(smartlamp_color_hue, smartlamp_color_saturation, smartlamp_color_level,
			&smartlamp_color_red, &smartlamp_color_green, &smartlamp_color_blue);

	printf("HSL (%lf, %lf, %d), RGB (%d, %d, %d)\n",
			smartlamp_color_hue, smartlamp_color_saturation, smartlamp_color_level,
			smartlamp_color_red, smartlamp_color_green, smartlamp_color_blue);
	color_led_onoff(smartlamp_switch_state);

	// 4. needed when it is necessary to keep monitoring the device status
	xTaskCreate(smartlamp_task, "smartlamp_task", 2048, (void *)switch_handle, 10, NULL);

	// 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

}
