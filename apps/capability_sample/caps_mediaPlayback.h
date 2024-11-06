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

#include "caps/iot_caps_helper_mediaPlayback.h"
#include "external/JSON.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct caps_mediaPlayback_data {
    IOT_CAP_HANDLE* handle;
    void *usr_data;
    void *cmd_data;

    char **supportedPlaybackCommands_value;
    int supportedPlaybackCommands_arraySize;
    char *playbackStatus_value;

    const char **(*get_supportedPlaybackCommands_value)(struct caps_mediaPlayback_data *caps_data);
    void (*set_supportedPlaybackCommands_value)(struct caps_mediaPlayback_data *caps_data, const char **value, int arraySize);
    void (*attr_supportedPlaybackCommands_send)(struct caps_mediaPlayback_data *caps_data);
    const char *(*get_playbackStatus_value)(struct caps_mediaPlayback_data *caps_data);
    void (*set_playbackStatus_value)(struct caps_mediaPlayback_data *caps_data, const char *value);
    int (*attr_playbackStatus_str2idx)(const char *value);
    void (*attr_playbackStatus_send)(struct caps_mediaPlayback_data *caps_data);

    void (*init_usr_cb)(struct caps_mediaPlayback_data *caps_data);

    void (*cmd_setPlaybackStatus_usr_cb)(struct caps_mediaPlayback_data *caps_data);
    void (*cmd_play_usr_cb)(struct caps_mediaPlayback_data *caps_data);
    void (*cmd_pause_usr_cb)(struct caps_mediaPlayback_data *caps_data);
    void (*cmd_rewind_usr_cb)(struct caps_mediaPlayback_data *caps_data);
    void (*cmd_fastForward_usr_cb)(struct caps_mediaPlayback_data *caps_data);
    void (*cmd_stop_usr_cb)(struct caps_mediaPlayback_data *caps_data);
} caps_mediaPlayback_data_t;

caps_mediaPlayback_data_t *caps_mediaPlayback_initialize(IOT_CTX *ctx, const char *component, void *init_usr_cb, void *usr_data);
#ifdef __cplusplus
}
#endif

