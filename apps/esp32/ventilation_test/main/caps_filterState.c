/* ***************************************************************************
 *
 * Copyright 2019-2026 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "st_dev.h"
#include "caps_filterState.h"

static int caps_filterState_get_filterLifeRemaining_value(caps_filterState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return caps_helper_filterState.attr_filterLifeRemaining.min - 1;
    }
    return caps_data->filterLifeRemaining_value;
}

static void caps_filterState_set_filterLifeRemaining_value(caps_filterState_data_t *caps_data, int value)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->filterLifeRemaining_value = value;
}

static const char *caps_filterState_get_filterLifeRemaining_unit(caps_filterState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->filterLifeRemaining_unit;
}

static void caps_filterState_set_filterLifeRemaining_unit(caps_filterState_data_t *caps_data, const char *unit)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->filterLifeRemaining_unit = (char *)unit;
}

static void caps_filterState_attr_filterLifeRemaining_send(caps_filterState_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }

    ST_CAP_SEND_ATTR_NUMBER(caps_data->handle,
            (char *)caps_helper_filterState.attr_filterLifeRemaining.name,
            caps_data->filterLifeRemaining_value,
            caps_data->filterLifeRemaining_unit,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send filterLifeRemaining value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static const char **caps_filterState_get_supportedFilterCommands_value(caps_filterState_data_t *caps_data)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return NULL;
    }
    return caps_data->supportedFilterCommands_value;
}

static void caps_filterState_set_supportedFilterCommands_value(caps_filterState_data_t *caps_data, const char **value, int array_size)
{
    if (!caps_data) {
        printf("caps_data is NULL\n");
        return;
    }
    caps_data->supportedFilterCommands_value = value;
    caps_data->supportedFilterCommands_array_size = array_size;
}

static void caps_filterState_attr_supportedFilterCommands_send(caps_filterState_data_t *caps_data)
{
    int sequence_no = -1;

    if (!caps_data || !caps_data->handle) {
        printf("fail to get handle\n");
        return;
    }
    if (!caps_data->supportedFilterCommands_value || caps_data->supportedFilterCommands_array_size <= 0) {
        printf("supportedFilterCommands is NULL\n");
        return;
    }

    ST_CAP_SEND_ATTR_STRINGS_ARRAY(caps_data->handle,
            (char *)caps_helper_filterState.attr_supportedFilterCommands.name,
            (char **)caps_data->supportedFilterCommands_value,
            caps_data->supportedFilterCommands_array_size,
            NULL,
            NULL,
            sequence_no);

    if (sequence_no < 0)
        printf("fail to send supportedFilterCommands value\n");
    else
        printf("Sequence number return : %d\n", sequence_no);
}

static void caps_filterState_cmd_resetFilter_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_filterState_data_t *caps_data = (caps_filterState_data_t *)usr_data;

    (void)handle;
    caps_data->cmd_data = cmd_data;

    if (caps_data->cmd_resetFilter_usr_cb) {
        caps_data->cmd_resetFilter_usr_cb(caps_data);
    }
}

static void caps_filterState_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_filterState_data_t *caps_data = usr_data;

    (void)handle;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
    caps_filterState_attr_filterLifeRemaining_send(caps_data);
    caps_filterState_attr_supportedFilterCommands_send(caps_data);
}

caps_filterState_data_t *caps_filterState_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_filterState_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_filterState_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_filterState_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_filterState_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    caps_data->get_filterLifeRemaining_value = caps_filterState_get_filterLifeRemaining_value;
    caps_data->set_filterLifeRemaining_value = caps_filterState_set_filterLifeRemaining_value;
    caps_data->get_filterLifeRemaining_unit = caps_filterState_get_filterLifeRemaining_unit;
    caps_data->set_filterLifeRemaining_unit = caps_filterState_set_filterLifeRemaining_unit;
    caps_data->attr_filterLifeRemaining_send = caps_filterState_attr_filterLifeRemaining_send;
    caps_data->get_supportedFilterCommands_value = caps_filterState_get_supportedFilterCommands_value;
    caps_data->set_supportedFilterCommands_value = caps_filterState_set_supportedFilterCommands_value;
    caps_data->attr_supportedFilterCommands_send = caps_filterState_attr_supportedFilterCommands_send;
    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_filterState.id, caps_filterState_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_filterState.cmd_resetFilter.name, caps_filterState_cmd_resetFilter_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for resetFilter of filterState\n");
        }
    } else {
        printf("fail to init filterState handle\n");
    }

    return caps_data;
}
