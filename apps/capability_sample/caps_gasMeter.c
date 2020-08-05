/* ***************************************************************************
 *
 * Copyright 2019-2020 Samsung Electronics All Rights Reserved.
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "st_dev.h"
#include "caps_gasMeter.h"

static const char *caps_gasMeter_get_gasMeterTime_value(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->gasMeterTime_value;
}

static void caps_gasMeter_set_gasMeterTime_value(caps_gasMeter_data_t *caps_data, const char *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->gasMeterTime_value) {
        free(caps_data->gasMeterTime_value);
    }
    caps_data->gasMeterTime_value = strdup(value);
}

static void caps_gasMeter_attr_gasMeterTime_send(caps_gasMeter_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->gasMeterTime_value) {
        printf("value is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRING(caps_data->handle,
            (char *)caps_helper_gasMeter.attr_gasMeterTime.name,
            caps_data->gasMeterTime_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send gasMeterTime value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static double caps_gasMeter_get_gasMeter_value(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_gasMeter.attr_gasMeter.min - 1;
    }
    return caps_data->gasMeter_value;
}

static void caps_gasMeter_set_gasMeter_value(caps_gasMeter_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->gasMeter_value = value;
}

static const char *caps_gasMeter_get_gasMeter_unit(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->gasMeter_unit;
}

static void caps_gasMeter_set_gasMeter_unit(caps_gasMeter_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->gasMeter_unit = (char *)unit;
}

static void caps_gasMeter_attr_gasMeter_send(caps_gasMeter_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_gasMeter.attr_gasMeter.name,
            caps_data->gasMeter_value,
            caps_data->gasMeter_unit,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send gasMeter value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static double caps_gasMeter_get_gasMeterCalorific_value(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_gasMeter.attr_gasMeterCalorific.min - 1;
    }
    return caps_data->gasMeterCalorific_value;
}

static void caps_gasMeter_set_gasMeterCalorific_value(caps_gasMeter_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->gasMeterCalorific_value = value;
}

static void caps_gasMeter_attr_gasMeterCalorific_send(caps_gasMeter_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_gasMeter.attr_gasMeterCalorific.name,
            caps_data->gasMeterCalorific_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send gasMeterCalorific value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static double caps_gasMeter_get_gasMeterVolume_value(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_gasMeter.attr_gasMeterVolume.min - 1;
    }
    return caps_data->gasMeterVolume_value;
}

static void caps_gasMeter_set_gasMeterVolume_value(caps_gasMeter_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->gasMeterVolume_value = value;
}

static const char *caps_gasMeter_get_gasMeterVolume_unit(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->gasMeterVolume_unit;
}

static void caps_gasMeter_set_gasMeterVolume_unit(caps_gasMeter_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->gasMeterVolume_unit = (char *)unit;
}

static void caps_gasMeter_attr_gasMeterVolume_send(caps_gasMeter_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_gasMeter.attr_gasMeterVolume.name,
            caps_data->gasMeterVolume_value,
            caps_data->gasMeterVolume_unit,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send gasMeterVolume value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static const JSON_H *caps_gasMeter_get_gasMeterPrecision_value(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return (const JSON_H *)caps_data->gasMeterPrecision_value;
}

static void caps_gasMeter_set_gasMeterPrecision_value(caps_gasMeter_data_t *caps_data, const JSON_H *value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    if (caps_data->gasMeterPrecision_value) {
        JSON_DELETE(caps_data->gasMeterPrecision_value);
    }
    caps_data->gasMeterPrecision_value = JSON_DUPLICATE(value, true);
}

static void caps_gasMeter_attr_gasMeterPrecision_send(caps_gasMeter_data_t *caps_data)
{
    IOT_EVENT *cap_evt;
    uint8_t evt_num = 1;
    int sequence_no = -1;
    iot_cap_val_t value;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->gasMeterPrecision_value) {
        printf("value is NULL\n");
        return;
    }

    value.type = IOT_CAP_VAL_TYPE_JSON_OBJECT;
    value.json_object = JSON_PRINT(caps_data->gasMeterPrecision_value);

    cap_evt = st_cap_create_attr(caps_data->handle,
        (char *)caps_helper_gasMeter.attr_gasMeterPrecision.name,
        &value,
        NULL,
        NULL);

    if (!cap_evt) {
        printf("fail to create cap_evt\n");
        return;
    }

    sequence_no = st_cap_send_attr(&cap_evt, evt_num);
    if (sequence_no < 0)
        printf("fail to send gasMeterPrecision value\n");

    printf("Sequence number return : %d\n", sequence_no);
    st_cap_free_attr(cap_evt);
}


static double caps_gasMeter_get_gasMeterConversion_value(caps_gasMeter_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_gasMeter.attr_gasMeterConversion.min - 1;
    }
    return caps_data->gasMeterConversion_value;
}

static void caps_gasMeter_set_gasMeterConversion_value(caps_gasMeter_data_t *caps_data, double value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->gasMeterConversion_value = value;
}

static void caps_gasMeter_attr_gasMeterConversion_send(caps_gasMeter_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_gasMeter.attr_gasMeterConversion.name,
            caps_data->gasMeterConversion_value,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send gasMeterConversion value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}


static void caps_gasMeter_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_gasMeter_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_gasMeter_attr_gasMeterTime_send(caps_data);
    caps_gasMeter_attr_gasMeter_send(caps_data);
    caps_gasMeter_attr_gasMeterCalorific_send(caps_data);
    caps_gasMeter_attr_gasMeterVolume_send(caps_data);
    caps_gasMeter_attr_gasMeterPrecision_send(caps_data);
    caps_gasMeter_attr_gasMeterConversion_send(caps_data);
}

caps_gasMeter_data_t *caps_gasMeter_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_gasMeter_data_t *caps_data = NULL;

    caps_data = malloc(sizeof(caps_gasMeter_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_gasMeter_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_gasMeter_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_gasMeterTime_value = caps_gasMeter_get_gasMeterTime_value;
    caps_data->set_gasMeterTime_value = caps_gasMeter_set_gasMeterTime_value;
    caps_data->attr_gasMeterTime_send = caps_gasMeter_attr_gasMeterTime_send;
    caps_data->get_gasMeter_value = caps_gasMeter_get_gasMeter_value;
    caps_data->set_gasMeter_value = caps_gasMeter_set_gasMeter_value;
    caps_data->get_gasMeter_unit = caps_gasMeter_get_gasMeter_unit;
    caps_data->set_gasMeter_unit = caps_gasMeter_set_gasMeter_unit;
    caps_data->attr_gasMeter_send = caps_gasMeter_attr_gasMeter_send;
    caps_data->get_gasMeterCalorific_value = caps_gasMeter_get_gasMeterCalorific_value;
    caps_data->set_gasMeterCalorific_value = caps_gasMeter_set_gasMeterCalorific_value;
    caps_data->attr_gasMeterCalorific_send = caps_gasMeter_attr_gasMeterCalorific_send;
    caps_data->get_gasMeterVolume_value = caps_gasMeter_get_gasMeterVolume_value;
    caps_data->set_gasMeterVolume_value = caps_gasMeter_set_gasMeterVolume_value;
    caps_data->get_gasMeterVolume_unit = caps_gasMeter_get_gasMeterVolume_unit;
    caps_data->set_gasMeterVolume_unit = caps_gasMeter_set_gasMeterVolume_unit;
    caps_data->attr_gasMeterVolume_send = caps_gasMeter_attr_gasMeterVolume_send;
    caps_data->get_gasMeterPrecision_value = caps_gasMeter_get_gasMeterPrecision_value;
    caps_data->set_gasMeterPrecision_value = caps_gasMeter_set_gasMeterPrecision_value;
    caps_data->attr_gasMeterPrecision_send = caps_gasMeter_attr_gasMeterPrecision_send;
    caps_data->get_gasMeterConversion_value = caps_gasMeter_get_gasMeterConversion_value;
    caps_data->set_gasMeterConversion_value = caps_gasMeter_set_gasMeterConversion_value;
    caps_data->attr_gasMeterConversion_send = caps_gasMeter_attr_gasMeterConversion_send;
    caps_data->gasMeter_value = 0;
    caps_data->gasMeterCalorific_value = 0;
    caps_data->gasMeterVolume_value = 0;
    caps_data->gasMeterConversion_value = 0;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_gasMeter.id, caps_gasMeter_init_cb, caps_data);
    }
    if (!caps_data->handle) {
        printf("fail to init gasMeter handle\n");
    }

    return caps_data;
}
