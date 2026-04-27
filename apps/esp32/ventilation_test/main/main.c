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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "st_dev.h"
#include "device_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "iot_uart_cli.h"
#include "iot_cli_cmd.h"

#include "caps/iot_caps_helper_airQualityHealthConcern.h"
#include "caps/iot_caps_helper_carbonDioxideMeasurement.h"
#include "caps/iot_caps_helper_fineDustSensor.h"
#include "caps_switch.h"
#include "caps_mode.h"
#include "caps_fanSpeed.h"
#include "caps_filterState.h"
#include "ventilation_sensor_source.h"

// onboarding_config_start is null-terminated string
extern const uint8_t onboarding_config_start[]    asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_config_end[]    asm("_binary_onboarding_config_json_end");

// device_info_start is null-terminated string
extern const uint8_t device_info_start[]    asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[]        asm("_binary_device_info_json_end");

static st_device_status g_device_status = ST_DEVICE_STATUS_INIT;

IOT_CTX* iot_ctx = NULL;

static int noti_led_mode = LED_ANIMATION_MODE_IDLE;

static caps_switch_data_t *cap_switch_data;
static caps_mode_data_t *cap_mode_data;
static caps_fanSpeed_data_t *cap_fan_speed_data;
static caps_filterState_data_t *cap_filter_state_data;
static IOT_CAP_HANDLE *cap_air_quality_health_concern_handle;
static IOT_CAP_HANDLE *cap_carbon_dioxide_handle;
static IOT_CAP_HANDLE *cap_fine_dust_handle;

static const char *supported_modes[] = {
    "auto",
    "clean_ventilation",
    "clean_circulation",
    "away",
    "sleep",
};

typedef struct ventilation_state {
    bool power_on;
    int fan_speed;
    int mode_index;
    ventilation_sensor_values_t sensors;
} ventilation_state_t;

static ventilation_state_t g_state = {
    .power_on = true,
    .fan_speed = 2,
    .mode_index = 0,
};

#define SENSOR_UPDATE_INTERVAL_MS   (10 * 1000)

static const char *supported_filter_commands[] = {
    caps_helper_filterState.attr_supportedFilterCommands.value_resetFilter,
};

static const char *supported_air_quality_values[] = {
    "unknown",
    caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_good,
    caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_moderate,
    caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_slightlyUnhealthy,
    caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_unhealthy,
    caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_veryUnhealthy,
    caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_hazardous,
};

static void send_primary_status(void);
static void send_sensor_status(void);
static void send_supported_air_quality_values_once(void);
static void send_supported_filter_commands_once(void);

typedef struct ventilation_sensor_report_state {
    bool initialized;
    bool supported_air_quality_values_sent;
    bool supported_filter_commands_sent;
    int last_reported_carbon_dioxide_ppm;
    int last_reported_fine_dust_pm25;
    int last_reported_filter_life_remaining;
    const char *last_reported_air_quality_health_concern;
    TickType_t last_report_tick;
} ventilation_sensor_report_state_t;

static ventilation_sensor_report_state_t g_sensor_report_state;

#define SENSOR_MAX_REPORT_INTERVAL_MS   (30 * 60 * 1000)
#define CO2_REPORT_THRESHOLD_PPM        10
#define FINE_DUST_REPORT_THRESHOLD_UG   5

static int get_switch_state(void)
{
    return g_state.power_on ? SWITCH_ON : SWITCH_OFF;
}

static const char *get_mode_value_by_index(int mode_index)
{
    if (mode_index < 0 || mode_index >= (int)(sizeof(supported_modes) / sizeof(supported_modes[0]))) {
        return supported_modes[0];
    }

    return supported_modes[mode_index];
}

static int get_mode_index_by_value(const char *mode_value)
{
    int index;

    if (!mode_value) {
        return 0;
    }

    for (index = 0; index < (int)(sizeof(supported_modes) / sizeof(supported_modes[0])); index++) {
        if (!strcmp(mode_value, supported_modes[index])) {
            return index;
        }
    }

    return 0;
}

static const char *get_air_quality_health_concern_value(int air_quality_score)
{
    if (air_quality_score <= 50) {
        return caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_good;
    }
    if (air_quality_score <= 100) {
        return caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_moderate;
    }
    if (air_quality_score <= 150) {
        return caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_slightlyUnhealthy;
    }
    if (air_quality_score <= 200) {
        return caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_unhealthy;
    }
    if (air_quality_score <= 300) {
        return caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_veryUnhealthy;
    }

    return caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.value_hazardous;
}

static void update_led_state(void)
{
    change_switch_state(get_switch_state());
}

static void send_primary_status_and_led(void)
{
    send_primary_status();
    update_led_state();
}

static void send_primary_status(void)
{
    if (cap_switch_data) {
        cap_switch_data->set_switch_value(cap_switch_data,
                g_state.power_on ? caps_helper_switch.attr_switch.value_on
                                 : caps_helper_switch.attr_switch.value_off);
        cap_switch_data->attr_switch_send(cap_switch_data);
    }

    if (cap_mode_data) {
        cap_mode_data->set_mode_value(cap_mode_data, get_mode_value_by_index(g_state.mode_index));
        cap_mode_data->attr_mode_send(cap_mode_data);
    }

    if (cap_fan_speed_data) {
        cap_fan_speed_data->set_fanSpeed_value(cap_fan_speed_data, g_state.fan_speed);
        cap_fan_speed_data->attr_fanSpeed_send(cap_fan_speed_data);
    }
}

static void send_sensor_status(void)
{
    IOT_EVENT *events[4];
    bool sent_air_quality_health_concern = false;
    bool sent_carbon_dioxide = false;
    bool sent_fine_dust = false;
    bool sent_filter_life = false;
    TickType_t now = xTaskGetTickCount();
    bool force_interval_sync;
    const char *air_quality_health_concern;
    int carbon_dioxide_delta = 0;
    int fine_dust_delta = 0;
    int filter_life_delta = 0;
    int event_count = 0;
    int sequence_no = -1;
    int index;

    memset(events, 0, sizeof(events));

    air_quality_health_concern = get_air_quality_health_concern_value(g_state.sensors.air_quality_score);
    force_interval_sync = !g_sensor_report_state.initialized ||
            ((now - g_sensor_report_state.last_report_tick) >= pdMS_TO_TICKS(SENSOR_MAX_REPORT_INTERVAL_MS));

    if (g_sensor_report_state.initialized) {
        carbon_dioxide_delta = g_state.sensors.carbon_dioxide_ppm -
                g_sensor_report_state.last_reported_carbon_dioxide_ppm;
        fine_dust_delta = g_state.sensors.fine_dust_pm25 -
                g_sensor_report_state.last_reported_fine_dust_pm25;
        filter_life_delta = g_state.sensors.filter_life_remaining -
                g_sensor_report_state.last_reported_filter_life_remaining;
    }

    if (cap_carbon_dioxide_handle &&
            (force_interval_sync ||
             !g_sensor_report_state.initialized ||
             abs(g_state.sensors.carbon_dioxide_ppm - g_sensor_report_state.last_reported_carbon_dioxide_ppm) >= CO2_REPORT_THRESHOLD_PPM)) {
        ST_CAP_CREATE_ATTR_NUMBER(cap_carbon_dioxide_handle,
                (char *)caps_helper_carbonDioxideMeasurement.attr_carbonDioxide.name,
                g_state.sensors.carbon_dioxide_ppm,
                caps_helper_carbonDioxideMeasurement.attr_carbonDioxide.unit_ppm,
                NULL,
                events[event_count]);
        if (events[event_count] != NULL) {
            event_count++;
            sent_carbon_dioxide = true;
        }
    } else if (cap_carbon_dioxide_handle && g_sensor_report_state.initialized) {
        printf("DEBUG: carbonDioxide ignored. current=%d last=%d delta=%d threshold=%d\n",
                g_state.sensors.carbon_dioxide_ppm,
                g_sensor_report_state.last_reported_carbon_dioxide_ppm,
                carbon_dioxide_delta,
                CO2_REPORT_THRESHOLD_PPM);
    }

    if (cap_fine_dust_handle &&
            (force_interval_sync ||
             !g_sensor_report_state.initialized ||
             abs(g_state.sensors.fine_dust_pm25 - g_sensor_report_state.last_reported_fine_dust_pm25) >= FINE_DUST_REPORT_THRESHOLD_UG)) {
        ST_CAP_CREATE_ATTR_NUMBER(cap_fine_dust_handle,
                (char *)caps_helper_fineDustSensor.attr_fineDustLevel.name,
                g_state.sensors.fine_dust_pm25,
                caps_helper_fineDustSensor.attr_fineDustLevel.unit_ug_per_m3,
                NULL,
                events[event_count]);
        if (events[event_count] != NULL) {
            event_count++;
            sent_fine_dust = true;
        }
    } else if (cap_fine_dust_handle && g_sensor_report_state.initialized) {
        printf("DEBUG: fineDust ignored. current=%d last=%d delta=%d threshold=%d\n",
                g_state.sensors.fine_dust_pm25,
                g_sensor_report_state.last_reported_fine_dust_pm25,
                fine_dust_delta,
                FINE_DUST_REPORT_THRESHOLD_UG);
    }

    if (cap_air_quality_health_concern_handle &&
            (force_interval_sync ||
             !g_sensor_report_state.initialized ||
             sent_carbon_dioxide ||
             sent_fine_dust ||
             g_sensor_report_state.last_reported_air_quality_health_concern == NULL ||
             strcmp(air_quality_health_concern, g_sensor_report_state.last_reported_air_quality_health_concern))) {
        ST_CAP_CREATE_ATTR_STRING(cap_air_quality_health_concern_handle,
                (char *)caps_helper_airQualityHealthConcern.attr_airQualityHealthConcern.name,
                (char *)air_quality_health_concern,
                NULL,
                NULL,
                events[event_count]);
        if (events[event_count] != NULL) {
            event_count++;
            sent_air_quality_health_concern = true;
        }
    }

    if (cap_filter_state_data && cap_filter_state_data->handle &&
            (force_interval_sync ||
             !g_sensor_report_state.initialized ||
             g_state.sensors.filter_life_remaining != g_sensor_report_state.last_reported_filter_life_remaining)) {
        ST_CAP_CREATE_ATTR_NUMBER(cap_filter_state_data->handle,
                (char *)caps_helper_filterState.attr_filterLifeRemaining.name,
                g_state.sensors.filter_life_remaining,
                caps_helper_filterState.attr_filterLifeRemaining.unit_percent,
                NULL,
                events[event_count]);
        if (events[event_count] != NULL) {
            event_count++;
            sent_filter_life = true;
        }
    } else if (cap_filter_state_data && cap_filter_state_data->handle && g_sensor_report_state.initialized) {
        printf("DEBUG: filterLifeRemaining ignored. current=%d last=%d delta=%d\n",
                g_state.sensors.filter_life_remaining,
                g_sensor_report_state.last_reported_filter_life_remaining,
                filter_life_delta);
    }

    if (event_count > 0) {
        sequence_no = st_cap_send_attr(events, event_count);
        for (index = 0; index < event_count; index++) {
            st_cap_free_attr(events[index]);
        }

        if (sequence_no >= 0) {
            if (sent_carbon_dioxide) {
                g_sensor_report_state.last_reported_carbon_dioxide_ppm = g_state.sensors.carbon_dioxide_ppm;
            }
            if (sent_fine_dust) {
                g_sensor_report_state.last_reported_fine_dust_pm25 = g_state.sensors.fine_dust_pm25;
            }
            if (sent_filter_life) {
                g_sensor_report_state.last_reported_filter_life_remaining = g_state.sensors.filter_life_remaining;
            }
            if (sent_air_quality_health_concern) {
                g_sensor_report_state.last_reported_air_quality_health_concern = air_quality_health_concern;
            }

            g_sensor_report_state.initialized = true;
            g_sensor_report_state.last_report_tick = now;
        }
    } else {
        printf("DEBUG: sensor tick ignored. no capability update sent.\n");
    }
}

static void send_supported_filter_commands_once(void)
{
    IOT_EVENT *event = NULL;
    int sequence_no = -1;

    if (!cap_filter_state_data || !cap_filter_state_data->handle || g_sensor_report_state.supported_filter_commands_sent) {
        return;
    }

    ST_CAP_CREATE_ATTR_STRINGS_ARRAY(cap_filter_state_data->handle,
            (char *)caps_helper_filterState.attr_supportedFilterCommands.name,
            (char **)supported_filter_commands,
            sizeof(supported_filter_commands) / sizeof(supported_filter_commands[0]),
            NULL,
            NULL,
            event);
    if (event == NULL) {
        return;
    }

    sequence_no = st_cap_send_attr(&event, 1);
    st_cap_free_attr(event);

    if (sequence_no >= 0) {
        g_sensor_report_state.supported_filter_commands_sent = true;
    }
}

static void send_supported_air_quality_values_once(void)
{
    IOT_EVENT *event = NULL;
    int sequence_no = -1;

    if (!cap_air_quality_health_concern_handle || g_sensor_report_state.supported_air_quality_values_sent) {
        return;
    }

    ST_CAP_CREATE_ATTR_STRINGS_ARRAY(cap_air_quality_health_concern_handle,
            (char *)"supportedAirQualityValues",
            (char **)supported_air_quality_values,
            sizeof(supported_air_quality_values) / sizeof(supported_air_quality_values[0]),
            NULL,
            NULL,
            event);
    if (event == NULL) {
        return;
    }

    sequence_no = st_cap_send_attr(&event, 1);
    st_cap_free_attr(event);

    if (sequence_no >= 0) {
        g_sensor_report_state.supported_air_quality_values_sent = true;
    }
}

static void send_all_status(void)
{
    send_primary_status();
    send_sensor_status();
    update_led_state();
}

static void update_sensor_values(void)
{
    ventilation_sensor_source_update(g_state.power_on, &g_state.sensors);
}

static void cap_switch_cmd_cb(struct caps_switch_data *caps_data)
{
    const char *switch_value = caps_data->get_switch_value(caps_data);

    g_state.power_on = switch_value && !strcmp(switch_value, caps_helper_switch.attr_switch.value_on);
    send_primary_status_and_led();
}

static void cap_mode_cmd_cb(struct caps_mode_data *caps_data)
{
    g_state.mode_index = get_mode_index_by_value(caps_data->get_mode_value(caps_data));

    if (g_state.mode_index == 3) {
        g_state.fan_speed = 1;
    } else if (g_state.mode_index == 4) {
        g_state.fan_speed = 1;
    }

    send_primary_status_and_led();
}

static void cap_fan_speed_cmd_cb(struct caps_fanSpeed_data *caps_data)
{
    int fan_speed = caps_data->get_fanSpeed_value(caps_data);

    if (fan_speed < 1) {
        fan_speed = 1;
    } else if (fan_speed > 3) {
        fan_speed = 3;
    }

    g_state.fan_speed = fan_speed;
    send_primary_status_and_led();
}

static void cap_filter_state_reset_cmd_cb(struct caps_filterState_data *caps_data)
{
    (void)caps_data;
    ventilation_sensor_source_reset_filter_life(&g_state.sensors);
    send_sensor_status();
}

static void capability_init()
{
    cap_switch_data = caps_switch_initialize(iot_ctx, "main", NULL, NULL);
    if (cap_switch_data) {
        cap_switch_data->cmd_on_usr_cb = cap_switch_cmd_cb;
        cap_switch_data->cmd_off_usr_cb = cap_switch_cmd_cb;
    }

    cap_mode_data = caps_mode_initialize(iot_ctx, "main", NULL, NULL);
    if (cap_mode_data) {
        cap_mode_data->cmd_setMode_usr_cb = cap_mode_cmd_cb;
        cap_mode_data->set_supportedModes_value(
                cap_mode_data,
                supported_modes,
                sizeof(supported_modes) / sizeof(supported_modes[0]));
    }

    cap_fan_speed_data = caps_fanSpeed_initialize(iot_ctx, "main", NULL, NULL);
    if (cap_fan_speed_data) {
        cap_fan_speed_data->cmd_setFanSpeed_usr_cb = cap_fan_speed_cmd_cb;
    }

    cap_air_quality_health_concern_handle = st_cap_handle_init(
            iot_ctx, "main", caps_helper_airQualityHealthConcern.id, NULL, NULL);
    cap_carbon_dioxide_handle = st_cap_handle_init(
            iot_ctx, "main", caps_helper_carbonDioxideMeasurement.id, NULL, NULL);
    cap_fine_dust_handle = st_cap_handle_init(
            iot_ctx, "main", caps_helper_fineDustSensor.id, NULL, NULL);

    cap_filter_state_data = caps_filterState_initialize(iot_ctx, "main", NULL, NULL);
    if (cap_filter_state_data) {
        cap_filter_state_data->cmd_resetFilter_usr_cb = cap_filter_state_reset_cmd_cb;
    }

    ventilation_sensor_source_init(&g_state.sensors);
    send_all_status();
    send_supported_air_quality_values_once();
    send_supported_filter_commands_once();
}

static void iot_status_cb(st_device_status device_status, void *usr_data)
{
    printf("Device status %d\n", device_status);
    g_device_status = device_status;
    switch (device_status) {
        case ST_DEVICE_STATUS_INIT:
            break;
        case ST_DEVICE_STATUS_ONBOARDING_READY:
            noti_led_mode = LED_ANIMATION_MODE_IDLE;
            update_led_state();
            break;
        case ST_DEVICE_STATUS_ONBOARDING_START:
            break;
        case ST_DEVICE_STATUS_ONBOARDING_NEED_CONFIRM:
            noti_led_mode = LED_ANIMATION_MODE_FAST;
            break;
        case ST_DEVICE_STATUS_ONBOARDING_ONBOARDED:
            break;
        case ST_DEVICE_STATUS_CLOUD_DISCONNECTED:
        case ST_DEVICE_STATUS_CLOUD_CONNECTED:
            noti_led_mode = LED_ANIMATION_MODE_IDLE;
            send_all_status();
            break;
    }
}

static void connection_start(void)
{
    int err;

    // process on-boarding procedure. There is nothing more to do on the app side than call the API.
    err = st_conn_start(iot_ctx, (st_status_cb)&iot_status_cb, NULL, NULL);
    if (err) {
        printf("fail to start connection. err:%d\n", err);
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

void button_event(IOT_CAP_HANDLE *handle, int type, int count)
{
    (void)handle;

    if (type == BUTTON_SHORT_PRESS) {
        printf("Button short press, count: %d\n", count);
        switch(count) {
            case 1:
                if (g_device_status == ST_DEVICE_STATUS_ONBOARDING_NEED_CONFIRM) {
                    st_conn_ownership_confirm(iot_ctx, true);
                    noti_led_mode = LED_ANIMATION_MODE_IDLE;
                    change_switch_state(get_switch_state());
                } else {
                    g_state.power_on = !g_state.power_on;
                    if (cap_switch_data) {
                        cap_switch_data->set_switch_value(
                                cap_switch_data,
                                g_state.power_on ? caps_helper_switch.attr_switch.value_on
                                                 : caps_helper_switch.attr_switch.value_off);
                        cap_switch_data->attr_switch_send(cap_switch_data);
                    }
                    change_switch_state(get_switch_state());
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
        printf("Button long press\n");
        led_blink(get_switch_state(), 100, 3);
        st_conn_cleanup(iot_ctx, false);
        xTaskCreate(connection_start_task, "connection_task", 1024*3, NULL, 10, NULL);
    }
}

static void app_main_task(void *arg)
{
    IOT_CAP_HANDLE *handle = (IOT_CAP_HANDLE *)arg;
    int button_event_type;
    int button_event_count;
    TickType_t last_sensor_update = xTaskGetTickCount();

    for (;;) {
        if (get_button_event(&button_event_type, &button_event_count)) {
            button_event(handle, button_event_type, button_event_count);
        }
        if (noti_led_mode != LED_ANIMATION_MODE_IDLE) {
            change_led_mode(noti_led_mode);
        }

        if ((xTaskGetTickCount() - last_sensor_update) >= pdMS_TO_TICKS(SENSOR_UPDATE_INTERVAL_MS)) {
            update_sensor_values();
            send_sensor_status();
            last_sensor_update = xTaskGetTickCount();
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
    iot_ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
    if (iot_ctx != NULL) {
        iot_err = st_conn_set_noti_cb(iot_ctx, iot_noti_cb, NULL);
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
