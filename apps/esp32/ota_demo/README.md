OTA demo device application
===========================

This document describes how the **Firmware Update** capability could be used at SmartThings Device SDK enabled device to help building device manufacturer own OTA (over-the-air) firmware update service.

This OTA demo device application is part of [OTA demo](../../../doc/ota_demo.md) system which includes all necessary component for demonstration.

Because this application is based on _st_switch_ example. you could find more detailed information which isn't described here from _st_switch_'s [README.md](../st_switch/README.md).
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
`ota_demo.c`
```c
// create handle for capability
ota_cap_handle = st_cap_handle_init(ctx, "main", "firmwareUpdate", cap_current_version_init_cb, NULL);

// set command callback
iot_err = st_cap_cmd_set_cb(ota_cap_handle, "updateFirmware", update_firmware_cmd_cb, NULL);
if (iot_err)
    printf("fail to set cmd_cb for updateFirmware");
```
***

Send currentVersion
-------------------
This code will update `currentVersion` attribute by sending event.   
`ota_demo.c`
```c
void cap_current_version_init_cb(IOT_CAP_HANDLE *handle, void *usr_data)
{
    ...
	init_evt = st_cap_attr_create_string("currentVersion", OTA_FIRMWARE_VERSION, NULL);

	sequence_no = st_cap_attr_send(handle, evt_num, &init_evt);
    ...
}
```
***

Lookup available new firmware
-----------------------------
This code will lookup available new firmware by reading OTA server's `versioninfo.json` and send `availableVersion` event.   
`ota_demo.c`
```c
static void ota_polling_task_func(void *arg)
{
	while (1) {
        ...
		esp_err_t ret = ota_https_read_version_info(&read_data, &read_data_len);
		if (ret == ESP_OK) {
			...
			esp_err_t err = ota_api_get_available_version(read_data, read_data_len, &available_version);
			...
			cap_available_version_set(available_version);
            	...
		}
        ...
	}
}
```
***

Run firmware update
-------------------
This code will run firmware update by receiving `updateFirmware` command.   
`ota_demo.c`
```c
static void ota_task_func(void * pvParameter)
{
	printf("Starting OTA...\n");

	esp_err_t ret = ota_https_update_device();
	if (ret != ESP_OK) {
		printf("Firmware Upgrades Failed (%d) \n", ret);
		_task_fatal_error();
	}

	printf("Restart system!\n");
	esp_restart();
}

void update_firmware_cmd_cb(IOT_CAP_HANDLE *handle,
			iot_cap_cmd_data_t *cmd_data, void *usr_data)
{
	ota_nvs_flash_init();
	xTaskCreate(&ota_task_func, "ota_task_func", 8096, NULL, 5, &ota_task_handle);
}
```

