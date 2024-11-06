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
#include "caps_refresh.h"

static void caps_refresh_cmd_refresh_cb(IOT_CAP_HANDLE *handle, iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
    caps_refresh_data_t *caps_data = (caps_refresh_data_t *)usr_data;

    printf("called [%s] func with num_args:%u\n", __func__, cmd_data->num_args);

    if (caps_data && caps_data->cmd_refresh_usr_cb)
        caps_data->cmd_refresh_usr_cb(caps_data);
}

static void caps_refresh_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    caps_refresh_data_t *caps_data = usr_data;
    if (caps_data && caps_data->init_usr_cb)
        caps_data->init_usr_cb(caps_data);
}

caps_refresh_data_t *caps_refresh_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data)
{
    caps_refresh_data_t *caps_data = NULL;
    int err;

    caps_data = malloc(sizeof(caps_refresh_data_t));
    if (!caps_data) {
        printf("fail to malloc for caps_refresh_data\n");
        return NULL;
    }

    memset(caps_data, 0, sizeof(caps_refresh_data_t));

    caps_data->init_usr_cb = init_usr_cb;
    caps_data->usr_data = usr_data;

    if (ctx) {
        caps_data->handle = st_cap_handle_init(ctx, component, caps_helper_refresh.id, caps_refresh_init_cb, caps_data);
    }
    if (caps_data->handle) {
        err = st_cap_cmd_set_cb(caps_data->handle, caps_helper_refresh.cmd_refresh.name, caps_refresh_cmd_refresh_cb, caps_data);
        if (err) {
            printf("fail to set cmd_cb for refresh of refresh\n");
    }
    } else {
        printf("fail to init refresh handle\n");
    }

    return caps_data;
}
