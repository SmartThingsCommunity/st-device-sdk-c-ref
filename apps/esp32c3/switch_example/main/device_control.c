/* ***************************************************************************
 *
 * Copyright 2021 Samsung Electronics All Rights Reserved.
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

#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"

#ifdef CONFIG_LED_RMT
static led_strip_t *strip;
static int red_value;
static int green_value;
static int blue_value;
static int onoff_state;

static const char *TAG = "led_strip";

static void led_strip_init() {
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(CONFIG_RMT_TX_GPIO, CONFIG_RMT_TX_CHANNEL);
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(1, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    ESP_ERROR_CHECK(strip->clear(strip, 100));
}

static void led_strip_update() {
    if (onoff_state == SWITCH_ON) {
        ESP_ERROR_CHECK(strip->set_pixel(strip, 0, red_value, green_value, blue_value));
    } else {
        ESP_ERROR_CHECK(strip->set_pixel(strip, 0, 0, 0, 0));
    }
    ESP_ERROR_CHECK(strip->refresh(strip, 100));
}

static void led_strip_onoff(int onoff) {
    onoff_state = onoff;
    led_strip_update();
}

static void led_strip_set_rgb(int red, int green, int blue) {
    red_value = red;
    green_value = green;
    blue_value = blue;
}
#endif //CONFIG_LED_RMT

void change_switch_state(int switch_state)
{
#ifdef CONFIG_LED_GPIO 
    if (switch_state == SWITCH_OFF) {
        gpio_set_level(GPIO_OUTPUT_MAINLED, MAINLED_GPIO_OFF);
    } else {
        gpio_set_level(GPIO_OUTPUT_MAINLED, MAINLED_GPIO_ON);
    }
#else
    led_strip_onoff(switch_state);
#endif
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

void led_blink(int switch_state, int delay, int count)
{
    for (int i = 0; i < count; i++) {
        vTaskDelay(delay / portTICK_PERIOD_MS);
        change_switch_state(1 - switch_state);
        vTaskDelay(delay / portTICK_PERIOD_MS);
        change_switch_state(switch_state);
    }
}

void change_led_mode(int noti_led_mode)
{
    static TimeOut_t led_timeout;
    static TickType_t led_tick = -1;
    static int last_led_mode = -1;
    static int led_state = SWITCH_OFF;

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
            if (xTaskCheckForTimeOut(&led_timeout, &led_tick ) != pdFALSE) {
                led_state = 1 - led_state;
                change_switch_state(led_state);
                vTaskSetTimeOutState(&led_timeout);
                if (led_state == SWITCH_ON) {
                    led_tick = pdMS_TO_TICKS(200);
                } else {
                    led_tick = pdMS_TO_TICKS(800);
                }
            }
            break;
        case LED_ANIMATION_MODE_FAST:
            if (xTaskCheckForTimeOut(&led_timeout, &led_tick ) != pdFALSE) {
                led_state = 1 - led_state;
                change_switch_state(led_state);
                vTaskSetTimeOutState(&led_timeout);
                led_tick = pdMS_TO_TICKS(100);
            }
            break;
        default:
            break;
    }
}

void iot_gpio_init(void)
{
	gpio_config_t io_conf;

	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
	io_conf.pull_down_en = 1;
	io_conf.pull_up_en = 0;

#ifdef CONFIG_LED_GPIO
	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_MAINLED;
	gpio_config(&io_conf);
	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_MAINLED_0;
	gpio_config(&io_conf);

	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_NOUSE1;
	gpio_config(&io_conf);
	io_conf.pin_bit_mask = 1 << GPIO_OUTPUT_NOUSE2;
	gpio_config(&io_conf);
#endif


	io_conf.intr_type = GPIO_INTR_ANYEDGE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = 1 << GPIO_INPUT_BUTTON;
	io_conf.pull_down_en = (BUTTON_GPIO_RELEASED == 0);
	io_conf.pull_up_en = (BUTTON_GPIO_RELEASED == 1);
	gpio_config(&io_conf);

	gpio_set_intr_type(GPIO_INPUT_BUTTON, GPIO_INTR_ANYEDGE);

	gpio_install_isr_service(0);

#ifdef CONFIG_LED_GPIO
	gpio_set_level(GPIO_OUTPUT_MAINLED, MAINLED_GPIO_ON);
	gpio_set_level(GPIO_OUTPUT_MAINLED_0, 0);
#else
    led_strip_init();
    led_strip_set_rgb(DEFAULT_RED_VALUE, DEFAULT_GREEN_VALUE, DEFAULT_BLUE_VALUE);
    led_strip_onoff(SWITCH_ON);
#endif
}


