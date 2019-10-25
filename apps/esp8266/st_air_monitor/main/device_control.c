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


#include "device_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"


static xQueueHandle button_event_queue = NULL;

void change_siren_state(int onoff)
{
	if (onoff) {
		gpio_set_level(GPIO_OUTPUT_SIREN, SIREN_GPIO_ON);
	} else {
		gpio_set_level(GPIO_OUTPUT_SIREN, SIREN_GPIO_OFF);
	}
}

void change_strobe_state(int onoff)
{
	if (onoff) {
		gpio_set_level(GPIO_OUTPUT_STROBE, STROBE_GPIO_ON);
	} else {
		gpio_set_level(GPIO_OUTPUT_STROBE, STROBE_GPIO_OFF);
	}
}

int get_button_event(int* button_event_type, int* button_event_count)
{
	static uint32_t button_count = 0;
	static uint32_t button_last_state = BUTTON_GPIO_RELEASED;
	static TimeOut_t button_timeout;
	static TickType_t long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
	static TickType_t button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);

	uint32_t gpio_level = 0;

	gpio_level = gpio_get_level(GPIO_INPUT_BUTTON);
	if (button_last_state != gpio_level) {
		/* wait debounce time to ignore small ripple of currunt */
		vTaskDelay( pdMS_TO_TICKS(BUTTON_DEBOUNCE_TIME_MS) );
		gpio_level = gpio_get_level(GPIO_INPUT_BUTTON);
		if (button_last_state != gpio_level) {
			printf("Button event, val: %d, tick: %u\n", gpio_level, (uint32_t)xTaskGetTickCount());
			button_last_state = gpio_level;
			if (gpio_level == BUTTON_GPIO_PRESSED) {
				button_count++;
			}
			vTaskSetTimeOutState(&button_timeout);
			button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);
			long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
		}
	} else if (button_count > 0) {
		if ((gpio_level == BUTTON_GPIO_PRESSED)
				&& (xTaskCheckForTimeOut(&button_timeout, &long_press_tick ) != pdFALSE)) {
			*button_event_type = BUTTON_LONG_PRESS;
			*button_event_count = 1;
			button_count = 0;
			return true;
		} else if ((gpio_level == BUTTON_GPIO_RELEASED)
				&& (xTaskCheckForTimeOut(&button_timeout, &button_delay_tick ) != pdFALSE)) {
			*button_event_type = BUTTON_SHORT_PRESS;
			*button_event_count = button_count;
			button_count = 0;
			return true;
		}
	}

	return false;
}

void led_blink(int gpio, int delay, int count)
{
	uint32_t gpio_level;

	gpio_level =  gpio_get_level(gpio);
	for (int i = 0; i < count; i++) {
		gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, 1 - gpio_level);
		vTaskDelay(delay / portTICK_PERIOD_MS);
		gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, gpio_level);
		vTaskDelay(delay / portTICK_PERIOD_MS);
	}
}

void change_led_state(int noti_led_mode)
{
	static TimeOut_t led_timeout;
	static TickType_t led_tick = -1;
	static int last_led_mode = -1;

	uint32_t gpio_level = 0;

	if (last_led_mode != noti_led_mode) {
		last_led_mode = noti_led_mode;
		vTaskSetTimeOutState(&led_timeout);
		led_tick = 0;
	}

	switch (noti_led_mode)
	{
		case LED_ANIMATION_MODE_IDLE:
			break;
		case LED_ANIMATION_MODE_SLOW:
			gpio_level =  gpio_get_level(GPIO_OUTPUT_NOTIFICATION_LED);
			if ((gpio_level == NOTIFICATION_LED_GPIO_ON) &&
					(xTaskCheckForTimeOut(&led_timeout, &led_tick ) != pdFALSE)) {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
				vTaskSetTimeOutState(&led_timeout);
				led_tick = pdMS_TO_TICKS(1000);
			}
			if ((gpio_level == NOTIFICATION_LED_GPIO_OFF) &&
					(xTaskCheckForTimeOut(&led_timeout, &led_tick ) != pdFALSE)) {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
				vTaskSetTimeOutState(&led_timeout);
				led_tick = pdMS_TO_TICKS(200);
			}
			break;
		case LED_ANIMATION_MODE_FAST:
			gpio_level =  gpio_get_level(GPIO_OUTPUT_NOTIFICATION_LED);
			if ((gpio_level == NOTIFICATION_LED_GPIO_ON) &&
					(xTaskCheckForTimeOut(&led_timeout, &led_tick ) != pdFALSE)) {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_OFF);
				vTaskSetTimeOutState(&led_timeout);
				led_tick = pdMS_TO_TICKS(100);
			}
			if ((gpio_level == NOTIFICATION_LED_GPIO_OFF) &&
					(xTaskCheckForTimeOut(&led_timeout, &led_tick ) != pdFALSE)) {
				gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
				vTaskSetTimeOutState(&led_timeout);
				led_tick = pdMS_TO_TICKS(100);
			}
			break;
		default:
			break;
	}
}

void gpio_init(void)
{
	gpio_config_t io_conf;

	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_STROBE;
	io_conf.pull_down_en = 1;
	io_conf.pull_up_en = 0;
	gpio_config(&io_conf);
	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_STROBE_0;
	gpio_config(&io_conf);

	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_SIREN;
	gpio_config(&io_conf);

	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_NOUSE;
	gpio_config(&io_conf);

	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_NOTIFICATION_LED;
	gpio_config(&io_conf);


	io_conf.intr_type = GPIO_INTR_ANYEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = 1 << GPIO_INPUT_BUTTON;
	io_conf.pull_down_en = (BUTTON_GPIO_RELEASED == 0);
	io_conf.pull_up_en = (BUTTON_GPIO_RELEASED == 1);
	gpio_config(&io_conf);

	gpio_set_intr_type(GPIO_INPUT_BUTTON, GPIO_INTR_ANYEDGE);

	button_event_queue = xQueueCreate(10, sizeof(uint32_t));

	gpio_install_isr_service(0);

	gpio_set_level(GPIO_OUTPUT_STROBE, STROBE_GPIO_OFF);
	gpio_set_level(GPIO_OUTPUT_STROBE_0, 0);
	gpio_set_level(GPIO_OUTPUT_SIREN, SIREN_GPIO_OFF);
	gpio_set_level(GPIO_OUTPUT_NOTIFICATION_LED, NOTIFICATION_LED_GPIO_ON);
}


