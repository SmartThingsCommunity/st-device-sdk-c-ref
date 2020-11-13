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

#include "device_control.h"
#include "iot_os_util.h"


int check_timeout(unsigned int *ptime_out, unsigned int *pms_to_wait)
{
	int ret;

	/* Minor optimisation.  The tick count cannot change in this block. */
	const mico_time_t const_tick_count = mico_rtos_get_time();
	const mico_time_t elapsed_time = const_tick_count - *ptime_out;

	if (elapsed_time < *pms_to_wait ) {
		/* Not a genuine timeout. Adjust parameters for time remaining. */
		*pms_to_wait -= elapsed_time;
		*ptime_out = mico_rtos_get_time();
		ret = 0;
	} else {
		*pms_to_wait = 0;
		ret = 1;
	}

	return ret;
}

void change_switch_state(int switch_state)
{
    if (switch_state == SWITCH_OFF) {
        MicoGpioOutputLow(GPIO_OUTPUT_MAINLED);
    } else {
        MicoGpioOutputHigh(GPIO_OUTPUT_MAINLED);
    }
}

int get_button_event(int* button_event_type, int* button_event_count)
{
    static uint32_t button_count = 0;
    static uint32_t button_last_state = BUTTON_GPIO_RELEASED;
    static unsigned int button_timeout;
    static unsigned int long_press_tick = BUTTON_LONG_THRESHOLD_MS;
    static unsigned int button_delay_tick = BUTTON_DELAY_MS;

    uint32_t gpio_level = 0;

    gpio_level = MicoGpioInputGet(GPIO_INPUT_BUTTON);
    if (button_last_state != gpio_level) {
        /* wait debounce time to ignore small ripple of currunt */
        iot_os_delay(BUTTON_DEBOUNCE_TIME_MS);
        gpio_level = MicoGpioInputGet(GPIO_INPUT_BUTTON);
        if (button_last_state != gpio_level) {
            printf("Button event, val: %d, tick: %u\n", gpio_level, (uint32_t)mico_rtos_get_time());
            button_last_state = gpio_level;
            if (gpio_level == BUTTON_GPIO_PRESSED) {
                button_count++;
            }
            button_timeout = mico_rtos_get_time();
            button_delay_tick = BUTTON_DELAY_MS;
            long_press_tick = BUTTON_LONG_THRESHOLD_MS;

        }
    } else if (button_count > 0) {
        if ((gpio_level == BUTTON_GPIO_PRESSED)
                && check_timeout(&button_timeout, &long_press_tick)) {
            *button_event_type = BUTTON_LONG_PRESS;
            *button_event_count = 1;
            button_count = 0;
            return true;
        } else if ((gpio_level == BUTTON_GPIO_RELEASED)
                && check_timeout(&button_timeout, &button_delay_tick)) {
            *button_event_type = BUTTON_SHORT_PRESS;
            *button_event_count = button_count;
            button_count = 0;
            return true;
        }
    }

    return false;
}

void led_blink(int switch_state, int delay, int count)
{
    for (int i = 0; i < count; i++) {
        iot_os_delay(delay);
        change_switch_state(1 - switch_state);
        iot_os_delay(delay);
        change_switch_state(switch_state);
    }
}

void change_led_mode(int noti_led_mode)
{
    static unsigned int led_timeout;
    static unsigned int led_tick = -1;
    static int last_led_mode = -1;
    static int led_state = SWITCH_OFF;

    if (last_led_mode != noti_led_mode) {
        last_led_mode = noti_led_mode;
        led_timeout = mico_rtos_get_time();
        led_tick = 0;
    }

    switch (noti_led_mode)
    {
        case LED_ANIMATION_MODE_IDLE:
            break;
        case LED_ANIMATION_MODE_SLOW:
            if (check_timeout(&led_timeout, &led_tick)) {
                led_state = 1 - led_state;
                change_switch_state(led_state);
                led_timeout = mico_rtos_get_time();
                if (led_state == SWITCH_ON) {
                    led_tick = 200;
                } else {
                    led_tick = 800;
                }
            }
            break;
        case LED_ANIMATION_MODE_FAST:
            if (check_timeout(&led_timeout, &led_tick)) {
                led_state = 1 - led_state;
                change_switch_state(led_state);
                led_timeout = mico_rtos_get_time();
                led_tick = 100;
            }
            break;
        default:
            break;
    }
}

void iot_gpio_init(void)
{
	//COLORLED 0 init
	MicoGpioInitialize(GPIO_OUTPUT_MAINLED, OUTPUT_PUSH_PULL);
	MicoGpioOutputHigh(GPIO_OUTPUT_MAINLED);

	//notify led init
	MicoGpioInitialize(GPIO_OUTPUT_NOTIFICATION_LED, OUTPUT_PUSH_PULL);
	MicoGpioOutputHigh(GPIO_OUTPUT_NOTIFICATION_LED);

	//button init
	MicoGpioInitialize(GPIO_INPUT_BUTTON, INPUT_PULL_UP);
}
