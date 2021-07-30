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
#include <stdlib.h>
#include <string.h>

#include "st_dev.h"
#include "device_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "iot_uart_cli.h"
#include "iot_cli_cmd.h"

#include "caps_switch.h"
#include "caps_button.h"

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]    asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]    asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]    asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]        asm("_binary_device_info_json_end");

static iot_status_t g_iot_status = IOT_STATUS_IDLE;
static iot_stat_lv_t g_iot_stat_lv;

IOT_CTX* ctx = NULL;

//#define SET_PIN_NUMBER_CONFRIM

static int noti_led_mode = LED_ANIMATION_MODE_IDLE;

static caps_switch_data_t *cap_switch_data;
static caps_button_data_t *cap_button_data;


static int get_switch_state(void)
{
    const char* switch_value = cap_switch_data->get_switch_value(cap_switch_data);
    int switch_state = SWITCH_OFF;

    if (!switch_value) {
        return -1;
    }

    if (!strcmp(switch_value, caps_helper_switch.attr_switch.value_on)) {
        switch_state = SWITCH_ON;
    } else if (!strcmp(switch_value, caps_helper_switch.attr_switch.value_off)) {
        switch_state = SWITCH_OFF;
    }
    return switch_state;
}

static void cap_switch_cmd_cb(struct caps_switch_data *caps_data)
{
    int switch_state = get_switch_state();
    change_switch_state(switch_state);
}

static void capability_init()
{
    cap_switch_data = caps_switch_initialize(ctx, "main", NULL, NULL);
    cap_button_data = caps_button_initialize(ctx, "main", NULL, NULL);
    
    if (cap_switch_data) {
        const char *switch_init_value = caps_helper_switch.attr_switch.value_on;
	 const char *button_init_value = caps_helper_button.attr_button.value_pushed;
	 
        cap_switch_data->cmd_on_usr_cb = cap_switch_cmd_cb;
        cap_switch_data->cmd_off_usr_cb = cap_switch_cmd_cb;
	
	
        cap_switch_data->set_switch_value(cap_switch_data, switch_init_value);
        cap_button_data->set_button_value(cap_button_data, button_init_value);
    }
}

static void iot_status_cb(iot_status_t status,
                          iot_stat_lv_t stat_lv, void *usr_data)
{
    g_iot_status = status;
    g_iot_stat_lv = stat_lv;

    printf("status: %d, stat: %d\n", g_iot_status, g_iot_stat_lv);

    switch(status)
    {
        case IOT_STATUS_NEED_INTERACT:
            noti_led_mode = LED_ANIMATION_MODE_FAST;
            break;
        case IOT_STATUS_IDLE:
        case IOT_STATUS_CONNECTING:
            noti_led_mode = LED_ANIMATION_MODE_IDLE;
            change_switch_state(get_switch_state());
            break;
        default:
            break;
    }
}

#if defined(SET_PIN_NUMBER_CONFRIM)
void* pin_num_memcpy(void *dest, const void *src, unsigned int count)
{
    unsigned int i;
    for (i = 0; i < count; i++)
        *((char*)dest + i) = *((char*)src + i);
    return dest;
}
#endif

static void connection_start(void)
{
    iot_pin_t *pin_num = NULL;
    int err;

#if defined(SET_PIN_NUMBER_CONFRIM)
    pin_num = (iot_pin_t *) malloc(sizeof(iot_pin_t));
    if (!pin_num)
        printf("failed to malloc for iot_pin_t\n");

    // to decide the pin confirmation number(ex. "12345678"). It will use for easysetup.
    //    pin confirmation number must be 8 digit number.
    pin_num_memcpy(pin_num, "12345678", sizeof(iot_pin_t));
#endif

    // process on-boarding procedure. There is nothing more to do on the app side than call the API.
    err = st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, pin_num);
    if (err) {
        printf("fail to start connection. err:%d\n", err);
    }
    if (pin_num) {
        free(pin_num);
    }
}

static void connection_start_task(void *arg)
{
    connection_start();
    vTaskDelete(NULL);
}

static void iot_noti_cb(iot_noti_data_t *noti_data, void *noti_usr_data)
{
    printf("Notification message received\n");

    if (noti_data->type == IOT_NOTI_TYPE_DEV_DELETED) {
        printf("[device deleted]\n");
    } else if (noti_data->type == IOT_NOTI_TYPE_RATE_LIMIT) {
        printf("[rate limit] Remaining time:%d, sequence number:%d\n",
               noti_data->raw.rate_limit.remainingTime, noti_data->raw.rate_limit.sequenceNumber);
    }
}

void switch_event()
{
	if (get_switch_state() == SWITCH_ON) {
        change_switch_state(SWITCH_OFF);
        cap_switch_data->set_switch_value(cap_switch_data, caps_helper_switch.attr_switch.value_off);
        cap_switch_data->attr_switch_send(cap_switch_data);
    } else {
        change_switch_state(SWITCH_ON);
        cap_switch_data->set_switch_value(cap_switch_data, caps_helper_switch.attr_switch.value_on);
        cap_switch_data->attr_switch_send(cap_switch_data);
        }
}

void button_event(IOT_CAP_HANDLE *handle, int type, int count)
{
    if (type == BUTTON_SHORT_PRESS) {
        printf("Button short press, count: %d\n", count);
        switch(count) {
            case 1:
            	 cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_pushed);
            	 cap_button_data->attr_button_send(cap_button_data);
                if (g_iot_status == IOT_STATUS_NEED_INTERACT) {
                    st_conn_ownership_confirm(ctx, true);
                    noti_led_mode = LED_ANIMATION_MODE_IDLE;
                    change_switch_state(get_switch_state());
                } else {
                        switch_event();
                }
                break;
            case 2:
                cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_pushed_2x);
                cap_button_data->attr_button_send(cap_button_data);
                break;
            case 3:
                cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_pushed_3x);
                cap_button_data->attr_button_send(cap_button_data);
                switch_event();
                break;
            case 4:
                cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_pushed_4x);
                cap_button_data->attr_button_send(cap_button_data);
                break;
            case 5:
                cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_pushed_5x);
                cap_button_data->attr_button_send(cap_button_data);
                switch_event();
                break;
            case 6:
                cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_pushed_6x);
                cap_button_data->attr_button_send(cap_button_data);
                break;
            case 7:
                /* clean-up provisioning & registered data with reboot option*/
                st_conn_cleanup(ctx, true);

                break;
            default:
                led_blink(get_switch_state(), 100, count);
                break;
        }
    } else if (type == BUTTON_LONG_PRESS) {
        cap_button_data->set_button_value(cap_button_data, caps_helper_button.attr_button.value_down_hold);
        cap_button_data->attr_button_send(cap_button_data);
        printf("Button long press, iot_status: %d\n", g_iot_status);
        led_blink(get_switch_state(), 100, 3);
        st_conn_cleanup(ctx, false);
        xTaskCreate(connection_start_task, "connection_task", 2048, NULL, 10, NULL);
    }
}

static void app_main_task(void *arg)
{
    IOT_CAP_HANDLE *handle = (IOT_CAP_HANDLE *)arg;

    int button_event_type;
    int button_event_count;

    for (;;) {
        if (get_button_event(&button_event_type, &button_event_count)) {
            button_event(handle, button_event_type, button_event_count);
        }
        if (noti_led_mode != LED_ANIMATION_MODE_IDLE) {
            change_led_mode(noti_led_mode);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    /**
      SmartThings Device SDK(STDK) aims to make it easier to develop IoT devices by providing
      additional st_iot_core layer to the existing chip vendor SW Architecture.

      That is, you can simply develop a basic application
      by just calling the APIs provided by st_iot_core layer like below.

      // create a iot context
      1. st_conn_init();

      // create a handle to process capability
      2. st_cap_handle_init(); (called in function 'capability_init')

      // register a callback function to process capability command when it comes from the SmartThings Server.
      3. st_cap_cmd_set_cb(); (called in function 'capability_init')

      // process on-boarding procedure. There is nothing more to do on the app side than call the API.
      4. st_conn_start(); (called in function 'connection_start')
     */

    unsigned char *onboarding_config = (unsigned char *) onboarding_config_start;
    unsigned int onboarding_config_len = onboarding_config_end - onboarding_config_start;
    unsigned char *device_info = (unsigned char *) device_info_start;
    unsigned int device_info_len = device_info_end - device_info_start;

    int iot_err;

    // create a iot context
    ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
    if (ctx != NULL) {
        iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
        if (iot_err)
            printf("fail to set notification callback function\n");
    } else {
        printf("fail to create the iot_context\n");
    }

    // create a handle to process capability and initialize capability info
    capability_init();

    iot_gpio_init();
    register_iot_cli_cmd();
    uart_cli_main();
    xTaskCreate(app_main_task, "app_main_task", 4096, NULL, 10, NULL);

    // connect to server
    connection_start();
}
