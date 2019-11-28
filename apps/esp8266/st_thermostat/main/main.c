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

//for implementing main features of IoT device
#include <stdbool.h>
#include <string.h>

#include "st_dev.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define POINT_UNIT "C"
#define TEMPERATURE_PERIOD_MS 2000
#define TEMPERATURE_SIMUL 1

typedef enum thermostat_mode {
	MODE_AUTO,
	MODE_COOL,
	MODE_ECO,
	MODE_RUSH_HOUR,
	MODE_EMERGENCY_HEAT,
	MODE_HEAT,
	MODE_OFF,
	MODE_MAX_NUM,
} thermostat_mode_t;

const char *thermostat_mode_str[MODE_MAX_NUM] = {
	[MODE_AUTO] = "auto",
	[MODE_COOL] = "cool",
	[MODE_ECO] = "eco",
	[MODE_RUSH_HOUR] = "rush hour",
	[MODE_EMERGENCY_HEAT] = "emergency heat",
	[MODE_HEAT] = "heat",
	[MODE_OFF] = "off",
};

typedef enum thermostat_fan_mode {
	FAN_MODE_AUTO,
	FAN_MODE_CIRCULATE,
	FAN_MODE_FOLLOWSCHEDULE,
	FAN_MODE_ON,
	FAN_MODE_MAX_NUM,
} thermostat_fan_mode_t;

const char *thermostat_fan_mode_str[MODE_MAX_NUM] = {
	[FAN_MODE_AUTO] = "auto",
	[FAN_MODE_CIRCULATE] = "circulate",
	[FAN_MODE_FOLLOWSCHEDULE] = "followschedule",
	[FAN_MODE_ON] = "on",
};

typedef enum thermostat_op_sate {
	OP_STATE_COOLING,
	OP_STATE_FAN_ONLY,
	OP_STATE_HEATING,
	OP_STATE_IDLE,
	OP_STATE_PENDING_COOL,
	OP_STATE_PENDING_HEAT,
	OP_STATE_VENT_ECONOMIZER,
	OP_STATE_MAX_NUM,
} thermostat_op_state_t;

const char *thermostat_op_state_str[MODE_MAX_NUM] = {
	[OP_STATE_COOLING] = "cooling",
	[OP_STATE_FAN_ONLY] = "fan only",
	[OP_STATE_HEATING] = "heating",
	[OP_STATE_IDLE] = "idle",
	[OP_STATE_PENDING_COOL] = "pending cool",
	[OP_STATE_PENDING_HEAT] = "pending heat",
	[OP_STATE_VENT_ECONOMIZER] = "vent economizer",
	NULL
};

extern const uint8_t onboarding_cfg_start[] asm("_binary_onboarding_config_json_start");
extern const uint8_t onboarding_cfg_end[] asm("_binary_onboarding_config_json_end");

extern const uint8_t device_info_start[] asm("_binary_device_info_json_start");
extern const uint8_t device_info_end[] asm("_binary_device_info_json_end");

static iot_status_t g_iot_status;
static iot_stat_lv_t g_iot_stat_lv;
static IOT_CTX *ctx = NULL;
static IOT_CAP_HANDLE *gh_temperature = NULL;

static double temperature;
static double cooling_point;
static double heating_point;
static thermostat_mode_t thermo_mode;
static thermostat_fan_mode_t thermo_fan_mode;
static thermostat_op_state_t thermo_op_state;

static char strbuf[16] = {0};

static double thermostat_get_temperature(void);
static void temperature_send_cap_measurement(IOT_CAP_HANDLE *handle, double point);

static char *util_double2str(double val)
{
	memset(strbuf, 0, sizeof(strbuf));

	/* support up to one decimal point (e.g., 36.5) */
	snprintf(strbuf, sizeof(strbuf), "%d.%d", (int)val, ((int)(val * 10) % 10));

	return strbuf;
}

static double temperature_simul(void)
{
	static double delta = 2;

	if (temperature < heating_point) {
		delta = 2;
		printf("[Turn on Heater]\n");
	} else if (temperature > cooling_point) {
		printf("[Turn on Cooler]\n");
		delta = -2;
	}

	temperature += delta;

	return temperature;
}

static void temperature_update(void)
{
	double temp;

#if TEMPERATURE_SIMUL
	temp = temperature_simul();
#else
	temp = thermostat_get_temperature();
	temperature = temp;
#endif
	if ((g_iot_status == IOT_STATUS_CONNECTING) &&
	    (g_iot_stat_lv == IOT_STAT_LV_DONE)) {
		temperature_send_cap_measurement(gh_temperature, temp);
	}
}

static void temperature_show(void)
{
	char buf[64];
	char *str;
	int c = 0;

	str = util_double2str(temperature);
	c = sprintf(buf + c, "temperature: %s ", str);

	str = util_double2str(cooling_point);
	c += sprintf(buf + c, "cooling: %s ", str);

	str = util_double2str(heating_point);
	c += sprintf(buf + c, "heating: %s ", str);

	printf("%s\n", buf);
}

static int thermostat_mode_str2idx(char *mode)
{
	int i;

	for (i = 0; i < MODE_MAX_NUM; i++) {
		if (!strcmp(mode, thermostat_mode_str[i]))
			return i;
	}

	printf("%s: '%s' is not supported mode\n", __func__, mode);

	return -1;
}

static int thermostat_fan_mode_str2idx(char *mode)
{
	int i;

	for (i = 0; i < FAN_MODE_MAX_NUM; i++) {
		if (!strcmp(mode, thermostat_fan_mode_str[i]))
			return i;
	}

	printf("%s: '%s' is not supported mode\n", __func__, mode);

	return -1;
}

static void thermostat_set_mode(thermostat_mode_t mode)
{
	/*
	 * YOUR CODE:
	 * implement a ability to control the mode of thermostat
	 */

	printf("%s: changed mode = %s", __func__,
				thermostat_mode_str[mode]);
}

static void thermostat_set_fan_mode(thermostat_fan_mode_t mode)
{
	/*
	 * YOUR CODE:
	 * implement a ability to control the fan mode of thermostat
	 */

	printf("%s: changed fan mode = %s", __func__,
				thermostat_fan_mode_str[mode]);
}

static double thermostat_get_temperature(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the temperature
	 */

	return 18.5;
}

static thermostat_mode_t thermostat_get_mode(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the mode of thermostat
	 */

	return MODE_AUTO;
}

static thermostat_fan_mode_t thermostat_get_fan_mode(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the fan mode of thermostat
	 */

	return FAN_MODE_AUTO;
}

static double thermostat_get_cooling_point(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the temperature for cooling
	 */

	return 29.5;
}

static double thermostat_get_heating_point(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the temperature for heating
	 */

	return 13.5;
}

static thermostat_op_state_t thermostat_get_op_state(void)
{
	/*
	 * YOUR CODE:
	 * implement a ability to obtain the operating state of thermostat
	 */

	return OP_STATE_IDLE;
}

static void temperature_send_cap_measurement(IOT_CAP_HANDLE *handle, double point)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;
	char *str;

	str = util_double2str(point);

	if ((point < -460) || (point > 10000)) {
		printf("%s: '%s' is out of range\n", __func__, str);
		return;
	}

	//printf("%s: point = %s\n", __func__, str);

	evt = st_cap_attr_create_number("temperature", point, POINT_UNIT);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_mode(IOT_CAP_HANDLE *handle, thermostat_mode_t mode)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;

	if (mode > MODE_MAX_NUM) {
		printf("%s: '%d' is not supported mode\n", __func__, mode);
		return;
	}

	printf("%s: %s\n", __func__, (char *)thermostat_mode_str[mode]);

	evt = st_cap_attr_create_string("thermostatMode",
				(char *)thermostat_mode_str[mode], NULL);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_supported_mode(IOT_CAP_HANDLE *handle)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;

	printf("%s\n", __func__);

	evt = st_cap_attr_create_string_array("supportedThermostatModes",
				MODE_MAX_NUM, (char **)thermostat_mode_str, NULL);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_fan_mode(IOT_CAP_HANDLE *handle, thermostat_fan_mode_t mode)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;

	if (mode > FAN_MODE_MAX_NUM) {
		printf("%s: '%d' is not supported mode\n", __func__, mode);
		return;
	}

	printf("%s: %s\n", __func__, (char *)thermostat_fan_mode_str[mode]);

	evt = st_cap_attr_create_string("thermostatFanMode",
				(char *)thermostat_fan_mode_str[mode], NULL);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_supported_fan_mode(IOT_CAP_HANDLE *handle)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;

	printf("%s\n", __func__);

	evt = st_cap_attr_create_string_array("supportedThermostatFanModes",
				FAN_MODE_MAX_NUM, (char **)thermostat_fan_mode_str, NULL);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_cooling(IOT_CAP_HANDLE *handle, double point)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;
	char *str;

	str = util_double2str(point);

	if ((point < -460) || (point > 10000)) {
		printf("%s: '%s' is out of range\n", __func__, str);
		return;
	}

	printf("%s: point = %s\n", __func__, str);

	evt = st_cap_attr_create_number("coolingSetpoint", point, POINT_UNIT);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_heating(IOT_CAP_HANDLE *handle, double point)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;
	char *str;

	str = util_double2str(point);

	if ((point < -460) || (point > 10000)) {
		printf("%s: '%s' is out of range\n", __func__, str);
		return;
	}

	printf("%s: point = %s\n", __func__, str);

	evt = st_cap_attr_create_number("heatingSetpoint", point, POINT_UNIT);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void thermostat_send_cap_op_state(IOT_CAP_HANDLE *handle, thermostat_op_state_t state)
{
	IOT_EVENT *evt;
	uint8_t evt_num = 1;
	int32_t seq_no;

	if (state > OP_STATE_MAX_NUM) {
		printf("%s: '%d' is not supported state\n", __func__, state);
		return;
	}

	printf("%s: state = %s\n", __func__, (char *)thermostat_op_state_str[state]);

	evt = st_cap_attr_create_string("thermostatOperatingState",
				(char *)thermostat_op_state_str[state], NULL);

	seq_no = st_cap_attr_send(handle, evt_num, &evt);
	if (seq_no < 0) {
		printf("%s: st_cap_attr_send is failed\n", __func__);
	}

	st_cap_attr_free(evt);
}

static void temperature_init_cb_measurement(IOT_CAP_HANDLE *handle, void *usr_data)
{
	temperature = thermostat_get_temperature();

	temperature_send_cap_measurement(handle, temperature);
}

static void thermostat_init_cb_mode(IOT_CAP_HANDLE *handle, void *usr_data)
{
	thermostat_send_cap_supported_mode(handle);

	thermo_mode = thermostat_get_mode();

	thermostat_send_cap_mode(handle, thermo_mode);
}

static void thermostat_init_cb_fan_mode(IOT_CAP_HANDLE *handle, void *usr_data)
{
	thermostat_send_cap_supported_fan_mode(handle);

	thermo_fan_mode = thermostat_get_fan_mode();

	thermostat_send_cap_fan_mode(handle, thermo_fan_mode);
}

static void thermostat_init_cb_cooling(IOT_CAP_HANDLE *handle, void *usr_data)
{
	cooling_point = thermostat_get_cooling_point();

	thermostat_send_cap_cooling(handle, cooling_point);
}

static void thermostat_init_cb_heating(IOT_CAP_HANDLE *handle, void *usr_data)
{
	heating_point = thermostat_get_heating_point();

	thermostat_send_cap_heating(handle, heating_point);
}

static void thermostat_init_cb_op_state(IOT_CAP_HANDLE *handle, void *usr_data)
{
	thermo_op_state = thermostat_get_op_state();

	thermostat_send_cap_op_state(handle, thermo_op_state);
}

static void thermostat_cmd_cb_mode_auto(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_mode_t mode = MODE_AUTO;

	thermostat_set_mode(mode);

	thermostat_send_cap_mode(handle, mode);
}

static void thermostat_cmd_cb_mode_cool(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_mode_t mode = MODE_COOL;

	thermostat_set_mode(mode);

	thermostat_send_cap_mode(handle, mode);
}

static void thermostat_cmd_cb_mode_emergencyheat(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_mode_t mode = MODE_EMERGENCY_HEAT;

	thermostat_set_mode(mode);

	thermostat_send_cap_mode(handle, mode);
}

static void thermostat_cmd_cb_mode_heat(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_mode_t mode = MODE_HEAT;

	thermostat_set_mode(mode);

	thermostat_send_cap_mode(handle, mode);
}

static void thermostat_cmd_cb_mode_off(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_mode_t mode = MODE_OFF;

	thermostat_set_mode(mode);

	thermostat_send_cap_mode(handle, mode);
}

static void thermostat_cmd_cb_mode_set(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	char *mode_str;
	int mode_num;

	if (cmd_data->num_args != 1) {
		printf("%s: invalid args (%d)\n", __func__,
				cmd_data->num_args);
	}

	mode_str = cmd_data->cmd_data[0].string;
	mode_num = thermostat_mode_str2idx(mode_str);
	if (mode_num < 0) {
		printf("%s: '%s' is not supported mode\n",
				__func__, mode_str);
	} else {
		thermo_mode = mode_num;
		thermostat_set_mode(thermo_mode);
	}

	thermostat_send_cap_mode(handle, thermo_mode);
}

static void thermostat_cmd_cb_fan_mode_auto(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_fan_mode_t mode = FAN_MODE_AUTO;

	thermostat_set_fan_mode(mode);

	thermostat_send_cap_fan_mode(handle, mode);
}

static void thermostat_cmd_cb_fan_mode_circulate(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_fan_mode_t mode = FAN_MODE_CIRCULATE;

	thermostat_set_fan_mode(mode);

	thermostat_send_cap_fan_mode(handle, mode);
}

static void thermostat_cmd_cb_fan_mode_on(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	thermostat_fan_mode_t mode = FAN_MODE_ON;

	thermostat_set_fan_mode(mode);

	thermostat_send_cap_fan_mode(handle, mode);
}

static void thermostat_cmd_cb_fan_mode_set(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	char *mode_str;
	int mode_num;

	if (cmd_data->num_args != 1) {
		printf("%s: invalid args (%d)\n", __func__,
				cmd_data->num_args);
	}

	mode_str = cmd_data->cmd_data[0].string;
	mode_num = thermostat_fan_mode_str2idx(mode_str);
	if (mode_num < 0) {
		printf("%s: not supported mode\n", __func__);
	} else {
		thermo_fan_mode = mode_num;
		thermostat_set_mode(thermo_fan_mode);
	}

	thermostat_send_cap_fan_mode(handle, thermo_fan_mode);
}

static void thermostat_cmd_cb_cooling(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	double point;
	char *str;

	if (cmd_data->num_args != 1) {
		printf("%s: invalid args (%d)\n", __func__,
				cmd_data->num_args);
	}

	point = cmd_data->cmd_data[0].number;

	if (point > heating_point) {
		cooling_point = point;
		str = util_double2str(cooling_point);
		printf("%s: changed to %s\n", __func__, str);
	} else {
		str = util_double2str(point);
		printf("%s: '%s' is invalid\n", __func__, str);
	}

	thermostat_send_cap_cooling(handle, cooling_point);
}

static void thermostat_cmd_cb_heating(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	double point;
	char *str;

	if (cmd_data->num_args != 1) {
		printf("%s: invalid args (%d)\n", __func__,
				cmd_data->num_args);
	}

	point = cmd_data->cmd_data[0].number;

	if (point < cooling_point) {
		heating_point = point;
		str = util_double2str(heating_point);
		printf("%s: changed to %s\n", __func__, str);
	} else {
		str = util_double2str(point);
		printf("%s: '%s' is invalid\n", __func__, str);
	}

	thermostat_send_cap_heating(handle, heating_point);
}

static void temperature_init_cap_measurement(IOT_CTX *ctx)
{
	IOT_CAP_HANDLE *handle;

	handle = st_cap_handle_init(ctx, "main", "temperatureMeasurement",
				temperature_init_cb_measurement, NULL);
	if (handle == NULL) {
		printf("%s: st_cap_handle_init is failed\n", __func__);
	}

	gh_temperature = handle;
}

static void thermostat_init_cap_mode(IOT_CTX *ctx)
{
	IOT_CAP_HANDLE *handle;
	int st_err;

	handle = st_cap_handle_init(ctx, "main", "thermostatMode",
				thermostat_init_cb_mode, NULL);
	if (handle == NULL) {
		printf("%s: st_cap_handle_init is failed\n", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "auto",
				thermostat_cmd_cb_mode_auto, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "cool",
				thermostat_cmd_cb_mode_cool, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "emergencyHeat",
				thermostat_cmd_cb_mode_emergencyheat, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "heat",
				thermostat_cmd_cb_mode_heat, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "off",
				thermostat_cmd_cb_mode_off, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "setThermostatMode",
				thermostat_cmd_cb_mode_set, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}
}

static void thermostat_init_cap_fan_mode(IOT_CTX *ctx)
{
	IOT_CAP_HANDLE *handle;
	int st_err;

	handle = st_cap_handle_init(ctx, "main", "thermostatFanMode",
				thermostat_init_cb_fan_mode, NULL);
	if (handle == NULL) {
		printf("%s: st_cap_handle_init is failed\n", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "fanAuto",
				thermostat_cmd_cb_fan_mode_auto, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "fanCirculate",
				thermostat_cmd_cb_fan_mode_circulate, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "fanOn",
				thermostat_cmd_cb_fan_mode_on, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "setThermostatFanMode",
				thermostat_cmd_cb_fan_mode_set, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed\n", __func__);
	}
}

static void thermostat_init_cap_cooling(IOT_CTX *ctx)
{
	IOT_CAP_HANDLE *handle;
	int st_err;

	handle = st_cap_handle_init(ctx, "main", "thermostatCoolingSetpoint",
				thermostat_init_cb_cooling, NULL);
	if (handle == NULL) {
		printf("%s: st_cap_handle_init is failed\n", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "setCoolingSetpoint",
				thermostat_cmd_cb_cooling, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed\n", __func__);
	}
}

static void thermostat_init_cap_heating(IOT_CTX *ctx)
{
	IOT_CAP_HANDLE *handle;
	int st_err;

	handle = st_cap_handle_init(ctx, "main", "thermostatHeatingSetpoint",
				thermostat_init_cb_heating, NULL);
	if (handle == NULL) {
		printf("%s: st_cap_handle_init is failed\n", __func__);
	}

	st_err = st_cap_cmd_set_cb(handle, "setHeatingSetpoint",
				thermostat_cmd_cb_heating, NULL);
	if (st_err) {
		printf("%s: st_cap_cmd_set_cb is failed\n", __func__);
	}
}

static void thermostat_init_cap_op_state(IOT_CTX *ctx)
{
	IOT_CAP_HANDLE *handle;

	handle = st_cap_handle_init(ctx, "main", "thermostatOperatingState",
				thermostat_init_cb_op_state, NULL);
	if (handle == NULL) {
		printf("%s: st_cap_handle_init is failed\n", __func__);
	}
}

static void iot_noti_cb(iot_noti_data_t *noti_data, void *noti_usr_data)
{
	printf("Notification message received\n");

	if (noti_data->type == IOT_NOTI_TYPE_DEV_DELETED) {
		printf("[device deleted]\n");
	} else if (noti_data->type == IOT_NOTI_TYPE_RATE_LIMIT) {
		printf("[rate limit] Remaining time:%d, sequence number:%d\n",
		       noti_data->raw.rate_limit.remainingTime,
		       noti_data->raw.rate_limit.sequenceNumber);
	}
}

static void iot_status_cb(iot_status_t status,
			iot_stat_lv_t stat_lv, void *usr_data)
{
	const char *status_str[] = {
		[IOT_STATUS_IDLE] = "idle",
		[IOT_STATUS_PROVISIONING] = "provisioning",
		[IOT_STATUS_NEED_INTERACT] = "need interact",
		[IOT_STATUS_CONNECTING] = "connecting",
	};

	const char *stat_lv_str[] = {
		[IOT_STAT_LV_STAY] = "stay",
		[IOT_STAT_LV_START] = "start",
		[IOT_STAT_LV_DONE] = "done",
		[IOT_STAT_LV_FAIL] = "fail",
	};

	g_iot_status = status;
	g_iot_stat_lv = stat_lv;

	printf("status: %s, stat: %s\n",
			(char *)status_str[g_iot_status],
			(char *)stat_lv_str[g_iot_stat_lv]);
}

static unsigned int task_get_current_ms(void)
{
	return (xTaskGetTickCountFromISR() * portTICK_PERIOD_MS);
}

static void thermostat_task(void *arg)
{
	TimeOut_t temperature_timeout;
	TickType_t temperature_period_tick = pdMS_TO_TICKS(TEMPERATURE_PERIOD_MS);
	
	vTaskDelay(20000 / portTICK_PERIOD_MS);

	vTaskSetTimeOutState(&temperature_timeout);

	while (1) {
		if (xTaskCheckForTimeOut(&temperature_timeout, &temperature_period_tick ) != pdFALSE) {
			vTaskSetTimeOutState(&temperature_timeout);
			temperature_period_tick = pdMS_TO_TICKS(TEMPERATURE_PERIOD_MS);

			temperature_update();
			temperature_show();
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	unsigned char *onboarding_cfg = (unsigned char *)onboarding_cfg_start;
	unsigned int onboarding_cfg_len = onboarding_cfg_end - onboarding_cfg_start;
	unsigned char *device_info = (unsigned char *)device_info_start;
	unsigned int device_info_len = device_info_end - device_info_start;
	int st_err;

	ctx = st_conn_init(onboarding_cfg, onboarding_cfg_len, device_info, device_info_len);
	if (ctx != NULL) {
		st_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
		if (st_err) {
			printf("st_conn_set_noti_cb is failed\n");
		}

		temperature_init_cap_measurement(ctx);

		thermostat_init_cap_mode(ctx);

		thermostat_init_cap_fan_mode(ctx);

		thermostat_init_cap_cooling(ctx);

		thermostat_init_cap_heating(ctx);

		thermostat_init_cap_op_state(ctx);
	} else {
		printf("st_conn_init is failed\n");
	}

	xTaskCreate(thermostat_task, "thermostat_task", 2048, NULL, 10, NULL);

	st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);
}
