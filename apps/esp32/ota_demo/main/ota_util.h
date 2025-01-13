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

typedef enum ota_err_type {
    OTA_OK                 = 0,
    OTA_FAIL               = -1,
    OTA_ERR_INVALID_ARG  = -2,
    OTA_ERR_NO_MEM        = -3,
    OTA_ERR_INVALID_SIZE = -4
} ota_err_t;

void ota_nvs_flash_init();
void ota_check_for_update(void *user_data);
void ota_restart_device();
ota_err_t ota_update_device();
int ota_get_polling_period_day();
ota_err_t ota_api_get_firmware_version_load(unsigned char *device_info, unsigned int device_info_len, char **firmware_version);
