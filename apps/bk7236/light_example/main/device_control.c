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

#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "device_control.h"
#include "driver/gpio_types.h"
#include "bk_gpio.h"
#include <driver/gpio.h>
#include "gpio_driver.h"

static int rgb_color_red = 255;
static int rgb_color_green = 0;
static int rgb_color_blue = 0;

static void update_rgb_from_color_temp(int color_temp, int *red, int *green, int *blue)
{
    int ct_table[10][3] = {
            {160, 0, 0}, //0
            {220, 20, 0}, //1000
            {255, 50, 0}, //2000
            {255, 160, 0}, //3000
            {255, 230, 130}, //4000
            {255, 255, 255}, //5000
            {120, 150, 255}, //6000
            {60, 80, 240}, //7000
            {30, 70, 200}, //8000
            {10, 50, 130}, //9000
    };

    if (color_temp < 0) {
        *red = ct_table[0][0];
        *green = ct_table[0][1];
        *blue = ct_table[0][2];
        return;
    }
    if (color_temp >= 9000) {
        *red = ct_table[9][0];
        *green = ct_table[9][1];
        *blue = ct_table[9][2];
        return;
    }

    int idx = color_temp / 1000;
    int remain = color_temp % 1000;
    *red = ct_table[idx][0] + (ct_table[idx+1][0]-ct_table[idx][0])*remain/1000;
    *green = ct_table[idx][1] + (ct_table[idx+1][1]-ct_table[idx][1])*remain/1000;
    *blue = ct_table[idx][2] + (ct_table[idx+1][2]-ct_table[idx][2])*remain/1000;
}

void change_switch_state(int switch_state)
{
    if (switch_state == SWITCH_OFF) {
        bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_R);
        bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_G);
        bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_B);
    } else {
        if (rgb_color_red > 127)
            bk_gpio_set_output_high(GPIO_OUTPUT_MAINLED_R);
        else
            bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_R);
        if (rgb_color_green > 127)
            bk_gpio_set_output_high(GPIO_OUTPUT_MAINLED_G);
        else
            bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_G);
        if (rgb_color_blue > 127)
            bk_gpio_set_output_high(GPIO_OUTPUT_MAINLED_B);
        else
            bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_B);
    }
}

void update_color_info(int color_temp)
{
    update_rgb_from_color_temp(color_temp,
                               &rgb_color_red, &rgb_color_green, &rgb_color_blue);
}

void change_switch_level(int level)
{
    printf("switch level is changed to %d", level);
    return;
}

int get_button_event(int* button_event_type, int* button_event_count)
{
    static uint32_t button_count = 0;
    static uint8_t button_last_state = BUTTON_GPIO_RELEASED;
    static TimeOut_t button_timeout;
    static TickType_t long_press_tick = pdMS_TO_TICKS(BUTTON_LONG_THRESHOLD_MS);
    static TickType_t button_delay_tick = pdMS_TO_TICKS(BUTTON_DELAY_MS);

    uint8_t gpio_level = 0;

    gpio_level = bk_gpio_get_input(GPIO_INPUT_BUTTON);
    
    if (button_last_state != gpio_level) {
        /* wait debounce time to ignore small ripple of currunt */
        vTaskDelay( pdMS_TO_TICKS(BUTTON_DEBOUNCE_TIME_MS) );

        gpio_level = bk_gpio_get_input(GPIO_INPUT_BUTTON);

        if (button_last_state != gpio_level) {
            printf("Button event, val: %ld, tick: %lu\n", gpio_level, (uint32_t)xTaskGetTickCount());
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
    gpio_config_t io_conf = {0};
    bk_gpio_driver_init();

    io_conf.func_mode = GPIO_SECOND_FUNC_DISABLE;
    io_conf.io_mode = GPIO_OUTPUT_ENABLE;
    io_conf.pull_mode = GPIO_PULL_UP_EN;

    gpio_dev_unmap(GPIO_OUTPUT_MAINLED_G);
    gpio_dev_unmap(GPIO_OUTPUT_MAINLED_R);
    gpio_dev_unmap(GPIO_OUTPUT_MAINLED_B);

    bk_gpio_set_config(GPIO_OUTPUT_MAINLED_G, &io_conf);
    bk_gpio_set_config(GPIO_OUTPUT_MAINLED_R, &io_conf);
    bk_gpio_set_config(GPIO_OUTPUT_MAINLED_B, &io_conf);

    bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_G);
    bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_R);
    bk_gpio_set_output_low(GPIO_OUTPUT_MAINLED_B);

    io_conf.func_mode = GPIO_SECOND_FUNC_DISABLE;
    io_conf.io_mode = GPIO_INPUT_ENABLE;
    io_conf.pull_mode = GPIO_PULL_UP_EN;

    gpio_dev_unmap(GPIO_INPUT_BUTTON);

    bk_gpio_set_config(GPIO_INPUT_BUTTON, &io_conf);
}

