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
#include "esp_log.h"

#include "iot_uart_cli.h"
//#include "iot_cli_cmd.h"

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


enum button_state {
    BUTTON_PUSHED = 1,
    BUTTON_DOWN = 0,
    BUTTON_DOUBLE = 2,
    BUTTON_HELD = 3
};

static int noti_led_mode = LED_ANIMATION_MODE_IDLE;

static caps_button_data_t *cap_button_data;

static int button_state = -1;

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
            printf("CONNECTED\n");
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
//////////////////// Button Control ////////////////////
int update_button_state()
{
    int button_event_type;
    int button_event_count;

    get_button_event(&button_event_type, &button_event_count);

    int32_t update_state;
    
    switch(button_event_type){
        case BUTTON_SHORT_PRESS :
            if (button_event_count > 1){
                update_state = BUTTON_DOUBLE;
            }
            else{
                update_state = BUTTON_PUSHED;
            }
            break;
        case BUTTON_LONG_PRESS :
            update_state = BUTTON_HELD;
            break;
        default :
            update_state = BUTTON_DOWN;
    }

    if (button_state != update_state){
        button_state = update_state;
        return 1;
    }

    return 0;
}

char *cap_enum_button_state()
{
    if (button_state == 1){
        return "pushed";
    }
    else if (button_state == 3){
        return "held";
    }
    else if (button_state == 2){
        return "double";
    }
    else{
        return "down";
    }
}

static void capability_init()
{
    cap_button_data = caps_button_initialize(ctx, "main", NULL, NULL); //Undefined reference to "caps_button_initialize" error occured.
    if (cap_button_data) {
        const char *button_init_value = caps_helper_button.attr_button.value_pushed;
        cap_button_data->set_button_value(cap_button_data, button_init_value);
    }
}

static void button_main_task(void *arg)
{
    
    //infinite loop
    for (;;){
        if (g_iot_status == IOT_STATUS_CONNECTING){
            if (update_button_state()){
 		        cap_button_data->button_value = cap_enum_button_state();
                cap_button_data->attr_button_send(cap_button_data);
            }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        }   
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
    IOT_CAP_HANDLE *handle = NULL;
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
    //register_iot_cli_cmd();
    uart_cli_main();

    xTaskCreate(button_main_task, "button_main_task", 4096,(void *)handle, 10, NULL);

    // connect to server
    connection_start();
}
