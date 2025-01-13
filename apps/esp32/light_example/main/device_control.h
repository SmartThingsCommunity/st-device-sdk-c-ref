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


//#define CONFIG_TARGET_WEMOS_D1_R32
#ifdef CONFIG_TARGET_WEMOS_D1_R32

#define GPIO_INPUT_BUTTON 18

#define GPIO_OUTPUT_COLORLED_R 16
#define GPIO_OUTPUT_COLORLED_G 26
#define GPIO_OUTPUT_COLORLED_B 17
#define GPIO_OUTPUT_COLORLED_0 25

#elif CONFIG_IDF_TARGET_ESP32C3

#define GPIO_INPUT_BUTTON 9

#define GPIO_OUTPUT_COLORLED_R 3
#define GPIO_OUTPUT_COLORLED_G 2
#define GPIO_OUTPUT_COLORLED_B 1
#define GPIO_OUTPUT_COLORLED_0 0

#elif CONFIG_IDF_TARGET_ESP32S2

#define GPIO_INPUT_BUTTON 0

#define GPIO_OUTPUT_COLORLED_R 13
#define GPIO_OUTPUT_COLORLED_G 12
#define GPIO_OUTPUT_COLORLED_B 11
#define GPIO_OUTPUT_COLORLED_0 10

#else // ESP32_DEVKITC_V4

#define GPIO_INPUT_BUTTON 0

#define GPIO_OUTPUT_COLORLED_R 12
#define GPIO_OUTPUT_COLORLED_G 14
#define GPIO_OUTPUT_COLORLED_B 27
#define GPIO_OUTPUT_COLORLED_0 26

#endif

enum switch_onoff_state {
    SWITCH_OFF = 0,
    SWITCH_ON = 1,
};

enum color_led_gpio_state {
    COLOR_LED_OFF = 0,
    COLOR_LED_ON = 1,
};

enum led_animation_mode_list {
    LED_ANIMATION_MODE_IDLE = 0,
    LED_ANIMATION_MODE_FAST,
    LED_ANIMATION_MODE_SLOW,
};

enum button_gpio_state {
    BUTTON_GPIO_RELEASED = 1,
    BUTTON_GPIO_PRESSED = 0,
};

#define BUTTON_DEBOUNCE_TIME_MS 20
#define BUTTON_LONG_THRESHOLD_MS 5000
#define BUTTON_DELAY_MS 300

enum button_event_type {
    BUTTON_LONG_PRESS = 0,
    BUTTON_SHORT_PRESS = 1,
};

void change_switch_state(int switch_state);
void update_color_info(int color_temp);
void change_switch_level(int level);
void button_isr_handler(void *arg);
int get_button_event(int* button_event_type, int* button_event_count);
void led_blink(int switch_state, int delay, int count);
void change_led_mode(int noti_led_mode);
void iot_gpio_init(void);
