# Getting Started for Direct-connected device

SmartThings Direct-connected devices are Wi-Fi enabled devices that use the SmartThings Platform as their primary cloud infrastructure. And these devices will use the MQTT protocol for communication.

This article demonstrates how to integrate a direct-connected device with SmartThings.

## Workflow

When developing a new direct-connected device application with SmartThings SDK, you will progress through the following steps. You can also refer to the [Samsung Developer - Code Lab](https://developer.samsung.com/codelab/smartthings/smartthings-device-sdk/overview.html). It will be more practical for you to understand the _SmartThings SDK for Direct Connected Devices_.

- [Setup Environment](#setup-environment)
- [Register a Device](#register-a-device)
- [Develop a Device Application](#develop-a-device-application)


  ![Work Flow](./res/workflow.jpg)

&nbsp;

## Setup Environment

The first thing you need to do is set up your programming environment.

### Get source code of _SmartThings SDK for Direct Connected Devices_

There are two git repositories for working with the _SmartThings SDK for Direct Connected Devices_. The first git repository is for the Internet of Things (IoT) core device library itself, while the second git repository is for sample device applications that use this IoT core device library on the chipset vendor's own SDK.

- [IoT Core Device Library](https://github.com/SmartThingsCommunity/st-device-sdk-c)
- [Reference](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref)

#### Download Reference

You can just choose to download the [Reference repository](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref) from GitHub, if you use a chipset that has already been ported. In this case, the `IoT Core Device Library` and `a chipset SDK` can be easily downloaded as submodules in this Reference repository through the predefined `setup.py` script. If you are new for using the _SmartThings SDK for Direct Connected Devices_, we recommend you to use the [Reference repository](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref) for easier understanding.

From the terminal, navigate to the directory you want the _SmartThings SDK for Direct Connected Devices_ to locate (e.g. `~/`) and clone it using the following git command:

```sh
$ cd ~
$ git clone https://github.com/SmartThingsCommunity/st-device-sdk-c-ref.git
```

> **Note** :
>
> For the rest of this document, we will assume the above path (`~/st-device-sdk-c-ref`) is the default reference source code.

You can use a script to automatically download the `IoT Core Device Library` and `a chipset vendor's SDK` (e.g. `esp32`) as follows:

```sh
$ cd ~/st-device-sdk-c-ref
$ python setup.py
Usage: python setup.py [BSP_NAME]
--------------------------------------------------
  ex) python setup.py rtl8721c
  ex) python setup.py rtl8720c
  ex) python setup.py rtl8195
  ex) python setup.py esp32_v3.3
  ex) python setup.py emw3080
  ex) python setup.py emw3166
  ex) python setup.py esp8266
  ex) python setup.py esp32
  ex) python setup.py esp32s2


$ python setup.py esp32
```

### Create device identity

In order for your device to connect to the SmartThings cloud, authentication data files are required.

Run the following `stdk-keygen.py` script in a terminal on your local machine to create a Device Identity.

```sh
$ cd ~/st-device-sdk-c-ref/iot-core/tools/keygen/
$ python3 stdk-keygen.py --firmware switch_example_001
Use following serial number and public key
for the identity of your device in Developer Workspace.

Serial Number:
STDK**E90W***uCX

Public Key:
nFN5x***uQusQ****ZHoBSFaAoP9***kNdLnjDJRew=

$ tree
.
├── output_STDK**E90W***uCX
│   ├── device_info.json
│   ├── device.pubkey.b64
│   └── device.seckey.b64
├── README.md
└── stdk-keygen.py

1 directory, 5 files

$ cat output_STDK**E90W***uCX/device_info.json
{
  "deviceInfo": {
    "firmwareVersion": "switch_example_001",
    "privateKey": "dH**JKMRD5X****BaV+fGoXa3qZfNW3V****jHXOmd0=",
    "publicKey": "nFN5x***uQusQ****ZHoBSFaAoP9***kNdLnjDJRew=",
    "serialNumber": "STDK**E90W***uCX"
  }
}
```

Copy the Serial Number and Public Key after running the command. You will need to upload these values to the SmartThings Platform via [Developer Workspace - Projects](https://smartthings.developer.samsung.com/workspace/projects) during the [next phase](#register-test-devices).

If you create a device identity with a command with an option like above,  You can get the ready-to-use `device_info.json` file directly.

### Setup toolchain

You must setup a toolchain according to each chipset you selected.

**Example for ESP8266** :

- Setup [ESP8266 Toolchain](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html#setup-toolchain).

- Setup ESP8266 environment for the _SmartThings SDK for Direct Connected Devices_ by running `setup.py`
  ```sh
  $ cd ~/st-device-sdk-c-ref
  $ python setup.py esp8266
  ```


**Example for ESP32** :

- Setup [ESP32 Development Environment](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/get-started/index.html#step-4-set-up-the-environment-variables).

- Setup ESP32 environment for the _SmartThings SDK for Direct Connected Devices_ by running `setup.py`
  ```sh
  $ cd ~/st-device-sdk-c-ref
  $ python setup.py esp32
  ```

**Example for ESP32_v3.3 (legacy)** :

- Setup [ESP32 Toolchain](https://docs.espressif.com/projects/esp-idf/en/release-v3.3/get-started/index.html#setup-toolchain).

- Setup ESP32 v3.3 environment for the _SmartThings SDK for Direct Connected Devices_ by running `setup.py`
  ```sh
  $ cd ~/st-device-sdk-c-ref
  $ python setup.py esp32_v3.3
  ```


**Example for RTL8195**

- Install [mbed Microcontroller](https://os.mbed.com/handbook/Windows-serial-configuration) to use the USB serial port on Windows.
- Upgrade [DAP F/W](https://www.amebaiot.com/en/change-dap-firmware/)
  Recommended DAP F/W is the `DAP_FW_Ameba_V12_1_3-2M.bin`

> **Note :**
>
> You have to proceed the above steps at Windows PC for downloading a binary. But you have to build the source code on the cygwin or linux for this chipset.



## Register a Device

Now that the programming environment is setup, you need to create project and register device information to the SmartThings Platform.

First, please sign in to the [Developer Workspace](https://smartthings.developer.samsung.com/workspace/) with a Samsung Account.

![sign in](./res/sign_in.png)

### Create a new project

To create project for direct connected device. Please select select _New Project_ &rightarrow; _Device Integration_ &rightarrow; _Direct-connected_  

![create project](./res/create-project.gif)


&nbsp;

### Add a device profile

A device profile contains the components, capabilities, and metadata (ID, name, etc.). This information defines the actions and attributes that an IoT device can perform.

Click the _DEFINE DEVICE PROFILE_ &rightarrow; _ADD A DEVICE PROFILE_ and then fill out required information (e.g. Basic infor, Component & Capability, UI display)

![create device profile](./res/create-device-profile.gif)

&nbsp;

The "[Health Check](https://smartthings.developer.samsung.com/docs/devices/health.html)" capability is automatically added for all direct connected devices. Please leave it and add your capabilities  

&nbsp;

### Add a device onboarding

The device onboarding guides device owners when their device is first registering and connecting to SmartThings.  You can customize the screens presented by adding a device onboarding.  The ownership validation type is also defined at this stage.

![create device onboarding](./res/create-device-onboarding.gif)

&nbsp;  


### Add a product info

The product info defines how this device is shown at SmartThings mobile app catalog. 
You can define device’s category and its regional availability.

![create product info](./res/add-product-info.gif)  

&nbsp;  
  

### Deploy your device to test

You can start testing by deploying your device to test from _Test_ &rightarrow; _Test Devices_ or _Overview_ page.  You will be able to see your device in the SmartThings mobile app when in Developer Mode only when after it has been deployed for testing.
- [SmartThings Device Publishing Process](https://smartthings.developer.samsung.com/docs/devices/publishing/publishing-basics.html)

![deplay to test](./res/deploy_to_test_switch.png)

&nbsp;  

### Register test devices

You can add identity of device for authenticating your device to SmartThings cloud. This requires device information like serial number and device public key (ED25519).
Because maximum number of test device is limited per user, Once you reaches maximum number of test device, you should remove one of existing one.

![register test device](./res/register-test-device.gif)

This example shows how to create ED25519 key pair with SDK tools. You can get device_info.json file as a result from tools/keygen/linux/output_{ serialNumber}

Key generator utility (keygen.py) is located at `st-iot-device-sdk-c-reference/iot-core/tools/keygen/` or `st-device-sdk-c/tools/keygen/`
Serial number for testing device would be randomly generated by this tool which has STDK + 12-digit alphanumberic format.

```sh
$ cd ~/st-device-sdk-c-ref/iot-core/tools/keygen/
$ python3 stdk-keygen.py --firmware switch_example_001
Use following serial number and public key
for the identity of your device in Developer Workspace.

Serial Number:
STDK**E90W***uCX

Public Key:
nFN5x***uQusQ****ZHoBSFaAoP9***kNdLnjDJRew=
```

Copy `STDK**E90W***uCX` from keygen output and paste it into “Device serial number” field of “Register A Test Device” page.
Copy public key string from keygen output (`nFN5x***uQusQ****ZHoBSFaAoP9***kNdLnjDJRew=` in this example) and paste it into “Device Public Key” field.

![adding test device](./res/adding_test_device2.png)
&nbsp;  

### Generate Device QR code
Using device QR code could be helpful while device onboarding. QR code should have format like below. (Please refer here for more details)

**{Your mnId}**: 4-digit alphanumeric mnId of your account  
**{Device onboardingId}**: 3-digit number Onboarding ID, you can find it from your developer workspace project “Device Onboarding” > “Other Info” page  
**{Device serialNumber}**: device serial number which is registered at your developer workspace project

```
https://qr.samsungiots.com/?m={Your mnId}&s={Device onboardingId}&r={Device serialNumber}
```

You can simply generate QR code with using below python3 script
```python
import qrcode

mnid = 'FFFF' # "FFFF" is an example. you should replace it with yours
onboardingId = '111' # "111" is an example. you should replace it with yours
serialNumber = 'STDKtest0001' # "STDKtest0001" is an example. you should replace it with yours
qrUrl = 'https://qr.samsungiots.com/?m=' + mnid + '&s=' + onboardingId + '&r=' + serialNumber
qrcode.QRCode(box_size=10, border=4)
img = qrcode.make(qrUrl)
img.save(serialNumber + '.png')
```

### Download onboarding_config.json

This is the information that the IoT device must have in order to connect to the SmartThings Platform.

If you use a sample device application from this git repository, please download it and then, just overwrite the existing `onboarding_config.json` file is in the `main` directory of sample device application with the new one you downloaded. In fact, overwriting is just one of several possible ways to inject it to the device. If the json information is guaranteed to be a parameter in the `st_conn_init()` function, you can refer to it differently according to your own development way.

<img src="res/downloading_onboarding_config_file_switch.png" style="zoom:70%;"/>
&nbsp;  
&nbsp;  

## Develop a device application

You have set up the environment and registered a device, so now it is time to develop a new device application in the SmartThings Device SDK Reference repository.

### Create a new device project

An easy way to get started is to branch out one of the example projects provided in the git repository. We will use the "switch_example" application from the SmartThings Device SDK Reference directory here.

```sh
# Full path of the ESP32
~/st-device-sdk-c-ref/apps/esp32/switch_example/
```

### Update device information

Your IoT device needs two pieces of information when connecting to the SmartThings Platform. These json formatted information should be passed to SmartThings Device SDK as a parameter of `st_conn_init()` api.

- device_info.json
- onboarding_config.json

1. device_info.json

   `device_info.json` file could contain per device specific information including firmware version, device private key, device public key and device serial number.

   We have two type of the format of `device_info.json`.

   - **All inclusive**

     `device_info.json` file could include all the information as json format. Our SmartThings Device SDK sample application (e.g. `light_example`, `switch_example`) uses this method.  
     This is the easiest and simplest way to contain security information for devices. But having per device specific information as json format may not be suitable for manufacturing. So this is recommended for the test devices for individual developer.
     ```json
     {
       "deviceInfo": {
         "firmwareVersion": "firmwareVersion_here",
         "privateKey": "privateKey_here",
         "publicKey": "publicKey_here",
         "serialNumber": "serialNumber_here"
       }
     }
     ```
     This file can be generated by [stdk-keygen.py](https://github.com/SmartThingsCommunity/st-device-sdk-c/tree/master/tools/keygen#individual) like below example.
     ```sh
     python3 stdk-keygen.py --firmware switch_example_ver001 --mnid fJXX
     ```

   - **Firmware version only**

     We can leave only the firmware version at `device_info.json` like below.
     ```json
     {
       "deviceInfo": {
         "firmwareVersion": "firmwareVersion_here"
       }
     }
     ```
     In this case other sercurity information like device private key, device public key and device serial number should be moved to dedicated secure partition - called as SmartThings Non-Volatile (STNV) Partition - with `CONFIG_STDK_IOT_CORE_SUPPORT_STNV_PARTITION` option enabled.  
     This is subitable for manufacturing because it could have dedicated secure partition for immutable information. SmartThings Device SDK expect security information from STNV parition with below path name.
     The mapping between `iot_nvd_t` and real storage path name can be found at `iot_bsp_nv_data_XXX.c` for [each bsp's porting layer](https://github.com/SmartThingsCommunity/st-device-sdk-c/tree/master/src/port/bsp).  
     [stdk-keygen.py](https://github.com/SmartThingsCommunity/st-device-sdk-c/tree/master/tools/keygen#commercial) also supports batch creation by reading csv formatted series of device serial number.

     | Data Path     | Description                   | Detail                              | Examples                 |
     | :------------ | :---------------------------- | :---------------------------------- | :----------------------- |
     | PublicKey     | Client (= Device) Public key  | base64 encoded ED25519 Public Key   | device.pubkey.b64        |
     | PrivateKey    | Client (= Device) Private key | base64 encoded ED25519 Privated Key | device.seckey.b64        |
     | SerialNum     | Device Serial Number          | alphanumberic string (8~30)         | SN12345678F              |

     > **Note :**
     >
     > If `CONFIG_STDK_IOT_CORE_SUPPORT_STNV_PARTITION` option has enabled, SmartThings Device SDK ignores `privateKey`, `publicKey` and `serialNumber` field from `device_info.json` file

2. onboarding_config.json

   The `onboarding_config.json` contains project common information. this can be downloaded from Developer Workspace project page as a file. Our sample appliation (e.g. `light_example`, `switch_example`) embedding it as an object blob. But developer could decide how to embed it as a part of device firmware.
   ```json
   {
     "onboardingConfig": {
       "deviceOnboardingId": "NAME",
       "mnId": "MNID",
       "setupId": "999",
       "vid": "VID",
       "deviceTypeId": "TYPE",
       "ownershipValidationTypes": [
         "JUSTWORKS",
         "BUTTON",
         "PIN",
         "QR"
       ],
       "identityType": "ED25519",
       "deviceIntegrationProfileKey": {
         "id": "DIP_UUID",
         "majorVersion": 0,
         "minorVersion": 1
       }
     }
   }
   ```
  
### Develop device application

A device application is developed using the APIs provided by the IoT Core Device Library. We recommend reuse of the example device applications, like `switch_example`, `light_example`. This allows for rapid development as you begin to develop your new device.  
Major APIs are like below:

- connection management
  - st_conn_init()
  - st_conn_set_noti_cb()
  - st_conn_start()
  - st_conn_cleanup()
  - st_conn_ownership_confirm()

- capability management
  - st_cap_handle_init()
  - st_cap_cmd_set_cb()
  - st_cap_create_attr()
    - ST_CAP_CREATE_ATTR_NUMBER
    - ST_CAP_SEND_ATTR_NUMBER
    - ST_CAP_CREATE_ATTR_STRING
    - ST_CAP_SEND_ATTR_STRING
    - ST_CAP_CREATE_ATTR_STRINGS_ARRAY
    - ST_CAP_SEND_ATTR_STRINGS_ARRAY
  - st_cap_free_attr()
  - st_cap_send_attr()

***Main function example for ESP32*** :

```sh
void app_main(void)
{
    /**
      SmartThings Device SDK aims to make it easier to develop IoT devices by providing
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
    ctx = st_conn_init(onboarding_config, onboarding_config_len, device_info, device_info_len);
    if (ctx != NULL) {
        iot_err = st_conn_set_noti_cb(ctx, iot_noti_cb, NULL);
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

```

### Build & Flash the App

Go to the root directory of the SmartThings Device SDK Reference git repo. and execute the build script(build.py) with the below parameter.

```sh
# Example for ESP32
$ cd ~/st-device-sdk-c-ref/
$ python build.py apps/esp32/switch_example             # python build.py {app_directory}
```

After compiling, following results can be seen. In fact, as you know, we have to flash these results according to the binary download method that exists per chipset.

```sh
# Example for ESP32
$ cd ~/st-device-sdk-c-ref/
$ tree output/ -L 3
output/
`-- esp32
    ├── iotcore_switch_example_20211223_e242418_20aab5c
    │   ├── bootloader.bin
    │   ├── debug
    │   ├── ota_data_initial.bin
    │   ├── partition-table.bin
    │   └── switch_example.bin

```

In case of Espressif chipset (e.g. ESP8266, ESP32), you can now run the following command to flash the entire binaries (e.g. app, bootloader, and init data bin) to the chipset.

```sh
# Example for ESP32
# python build.py {app_directory} {option}
$ cd ~/st-device-sdk-c-ref/
$ python build.py apps/esp32/switch_example flash
```

The serial port needs to be matched to the computer environment for serial port flashing. For example, the settings for serial port flashing can be configured with `menuconfig` option in Espressif. If the serial port setting does not match your environment, please execute the following:

> Note :
> The `menuconfig` option is only supported on the Espressif chipset. If you use a different chipset, please set it according to the selected original chipset guide.
```sh
# Example for ESP32
# python build.py {app_directory} {option}
$ cd ~/st-device-sdk-c-ref
$ python build.py apps/esp32/switch_example menuconfig
```

Plus, You don't need to run `python build.py appes/esp32/switch_example` before running `python build.py appes/esp32/switch_example flash`, this will automatically rebuild everything that needs to be built before flashing.

For more details about flashing and monitoring, please refer to the [README](../README.md#flash--monitor) file.

### Test

The SmartThings App should be used to control an IoT device that is running with the SmartThings Platform. Go to application market place (e.g. Play Store, App Store, Galaxy Store) and download the SmartThings app on your mobile device.

1. **Enable developer mode**  
You need to enable the ***Developer Mode*** in the SmartThings app before start testing.  
Please refer [here](https://smartthings.developer.samsung.com/docs/testing/developer-mode.html) for more details.  
![developer mode](./res/developer_mode.png)

2. **Add device**  
Go to ***Add device*** and click ***My Testing Devices*** or ***Scan QR code***. You can now see and add your self-published devices.  
![add device 1](./res/add_device1.png)
![add device 2](./res/add_device2.png)

3. **Control device**  
You can control test device from dash board action and device plug user interface  
![device control 1](./res/device_control1.png)
![device control 2](./res/device_control2.png)
