/* ***************************************************************************
 *
 * Copyright 2022 Samsung Electronics All Rights Reserved.
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

#include "caps_switch.h"

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]    asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]    asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]    asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]        asm("_binary_device_info_json_end");

static iot_status_t g_iot_status = IOT_STATUS_IDLE;
static iot_stat_lv_t g_iot_stat_lv;

IOT_CTX* iot_ctx = NULL;

static caps_switch_data_t *cap_switch_data;

static void update_switch_state(int switch_state)
{
    const char* switch_value;

    if (switch_state == SWITCH_ON) {
        switch_value = caps_helper_switch.attr_switch.value_on;
    } else {
        switch_value = caps_helper_switch.attr_switch.value_off;
    }

    //TODO: Set switch attribute value and send
    //
    // Example
    //===============================================================================
    // cap_switch_data->set_switch_value(cap_switch_data, switch_value);
    // cap_switch_data->attr_switch_send(cap_switch_data);
    //===============================================================================
}

static int get_switch_state(void)
{
    const char* switch_value;
    int switch_state = SWITCH_OFF;

    //TODO: Get switch attribute value
    //
    // Example
    //===============================================================================
    // switch_value = cap_switch_data->get_switch_value(cap_switch_data);
    //
    // if (!strcmp(switch_value, caps_helper_switch.attr_switch.value_on)) {
    //     switch_state = SWITCH_ON;
    // } else if (!strcmp(switch_value, caps_helper_switch.attr_switch.value_off)) {
    //     switch_state = SWITCH_OFF;
    // }
    //===============================================================================
    return switch_state;
}

static void cap_switch_cmd_cb(struct caps_switch_data *caps_data)
{
    int switch_state = get_switch_state();
    set_led_mode(switch_state);
}

static void capability_init()
{
    //TODO: initialize switch capability with using capability sample's initializae function
    //
    // Example
    // //===============================================================================
    // cap_switch_data = caps_switch_initialize(iot_ctx, "main", NULL, NULL);
    // if (cap_switch_data) {
    //     const char *switch_init_value = caps_helper_switch.attr_switch.value_on;
    //
    //     cap_switch_data->cmd_on_usr_cb = cap_switch_cmd_cb;
    //     cap_switch_data->cmd_off_usr_cb = cap_switch_cmd_cb;
    //
    //     cap_switch_data->set_switch_value(cap_switch_data, switch_init_value);
    // }
    //===============================================================================
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
            break;
        case IOT_STATUS_IDLE:
        case IOT_STATUS_CONNECTING:
            set_led_mode(get_switch_state());
            break;
        default:
            break;
    }
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

void button_event(IOT_CAP_HANDLE *handle, int type, int count)
{
    if (type == BUTTON_SHORT_PRESS) {
        printf("Button short press, count: %d\n", count);
        switch(count) {
            case 1:
                if (g_iot_status == IOT_STATUS_NEED_INTERACT) {
                    st_conn_ownership_confirm(iot_ctx, true);
                } else {
                    int next_state;

                    if (get_switch_state() == SWITCH_ON) {
                        next_state = SWITCH_OFF;
                    } else {
                        next_state = SWITCH_ON;
                    }

                    set_led_mode(next_state);
                    update_switch_state(next_state);
                }
                break;
            case 5:
                /* clean-up provisioning & registered data with reboot option*/
                st_conn_cleanup(iot_ctx, true);

                break;
            default:
                led_blink(get_switch_state(), 100, count);
                break;
        }
    } else if (type == BUTTON_LONG_PRESS) {
        printf("Button long press, iot_status: %d\n", g_iot_status);
        led_blink(get_switch_state(), 100, 3);
        st_conn_cleanup(iot_ctx, true);
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

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    /**
      Easily integrate your Direct Connected Device using the Direct Connected Devices SDK.
      The SDK manages all MQTT topics and onboarding requirements,
      freeing you to focus on the Capabilities of your device.

      That is, you can simply develop a basic application
      by just calling the APIs provided by st_iot_core layer like below.

      // create a iot context
      1. st_conn_init();

      // create a handle to process capability
      2. st_cap_handle_init(); (called in function 'capability_init')

      // register a callback function to process capability command when it comes from the SmartThings Server.
      3. st_cap_cmd_set_cb(); (called in function 'capability_init')

      // process on-boarding procedure. There is nothing more to do on the app side than call the API.
      4. st_conn_start();
     */

    unsigned char *onboarding_config = (unsigned char *) onboarding_config_start;
    unsigned int onboarding_config_len = onboarding_config_end - onboarding_config_start;
    unsigned char *device_info = (unsigned char *) device_info_start;
    unsigned int device_info_len = device_info_end - device_info_start;

    int err;

    iot_gpio_init();
    xTaskCreate(app_main_task, "app_main_task", 4096, NULL, 10, NULL);

    //create a iot context
    iot_ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
    if (iot_ctx != NULL) {
        iot_err = st_conn_set_noti_cb(iot_ctx, iot_noti_cb, NULL);
        if (iot_err)
            printf("fail to set notification callback function\n");
    } else {
        printf("fail to create the iot_context\n");
    }

    //create a handle to process capability and initialize capability info
    capability_init();

    //connect to server
    err = st_conn_start(iot_ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);
    if (err) {
        printf("fail to start connection. err:%d\n", err);
    }

}
