OTA demo device application
===========================

This document describes how the **Firmware Update** capability could be used at SmartThings Device SDK enabled device to help building device manufacturer own OTA (over-the-air) firmware update service.

This OTA demo device application is part of [OTA demo](../../../doc/ota_demo.md) system which includes all necessary component for demonstration.

Because this application is based on _switch_example_ . you could find more detailed information which isn't described here from _st_switch_'s [README.md](../switch_example/README.md).
***

Change configuration
-------------------
These compile options will enable HTTPS and MBEDTLS feature to connect OTA server.   
`stdkconfig`
```c
// Enable option for HTTPS
# ESP HTTP client
CONFIG_ESP_HTTP_CLIENT_ENABLE_HTTPS=y

// Enable option for MBEDTLS
# Network
CONFIG_STDK_IOT_CORE_NET_MBEDTLS=y
```

***

Import certificate
-------------------
This application requires _root.pem_, _public_key.pem_ which is used for https connection and firmware signature validation.   
You can find how to create these file from [here](../../../doc/ota_demo.md#preparing-certificate)

```sh
$ cp root.pem [st-device-sdk-c-ref path]/apps/esp32/ota_demo/main
$ cp public_key.pem [st-device-sdk-c-ref path]/apps/esp32/ota_demo/main
```

***

Register capability callback
-------------------
This code init handle and add command callback for **Firmware Update** capability.   
`main.c`
```c
// create ota data for capability
cap_ota_data = caps_ota_initialize(ctx, "main", NULL, NULL);

// set command callback
cap_ota_data->cmd_update_firmware_usr_cb = cap_update_cmd_cb;
```
***

Send currentVersion
-------------------
This code will update `currentVersion` attribute by sending event.   
`main.c`
```c
char *firmware_version = get_current_firmware_version();

cap_ota_data->set_currentVersion(cap_ota_data, firmware_version);
```
***

Lookup available new firmware
-----------------------------
This code will lookup available new firmware by reading OTA server's `versioninfo.json` and send `availableVersion` event.   
`main.c`
```c

void ota_polling_task(void *arg)
{
    while (1) {
        ...
        ota_check_for_update((void *)arg);
        ...
    }
}

void ota_check_for_update(void *user_data)
{
    ...
    ota_err_t ret = _read_version_info_from_server(&read_data, &read_data_len);
    if (ret == OTA_OK) {
        ...
        ret = _get_available_version(read_data, read_data_len, current_version, &available_version);
        ...
        if (available_version) {
            caps_data->set_availableVersion(caps_data, available_version);
            caps_data->attr_availableVersion_send(caps_data);
        ...
        }
    }
}
```
***

Run firmware update
-------------------
This code will run firmware update by receiving `updateFirmware` command.   
`main.c`
```c
static void ota_update_task(void * pvParameter)
{
    printf("\n Starting OTA...\n");

    ```
    ota_err_t ret = ota_update_device();
    ```

    printf("Prepare to restart system!");
    ota_restart_device();
}

static void cap_update_cmd_cb(struct caps_ota_data *caps_data)
{
	ota_nvs_flash_init();
	xTaskCreate(&ota_update_task, "ota_update_task", 8096, NULL, 5, &ota_task_handle);
}
```

