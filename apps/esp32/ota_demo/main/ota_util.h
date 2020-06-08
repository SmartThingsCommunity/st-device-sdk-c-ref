/* ***************************************************************************
 *
 * Copyright (c) 2020 Samsung Electronics All Rights Reserved.
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

#include <esp_https_ota.h>
#include <esp_ota_ops.h>
#include <esp_log.h>

#define OTA_FIRMWARE_VERSION "ota_demo_ver1"

int ota_get_polling_period_day();
void ota_nvs_flash_init();
esp_err_t ota_api_get_available_version(char *update_info, unsigned int update_info_len, char **new_version);
esp_err_t ota_https_update_device();
esp_err_t ota_https_read_version_info(char **version_info, unsigned int *version_info_len);
