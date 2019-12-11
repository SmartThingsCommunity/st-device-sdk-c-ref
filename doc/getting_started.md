# Getting Started for Direct-connected device

SmartThings Direct-connected devices are Wi-Fi enabled devices that use the SmartThings cloud as their primary cloud infrastructure. Your device will use the MQTT protocol for communication.

This article demonstrates how to integrate a direct-connected device with SmartThings.

## Workflow

When developing a new direct-connected device application with SmartThings Device SDK (STDK for short), you will progress through the following steps.

- [Setup Environment](#Setup Environment)

- [Register a Device](#Register a Device)

- [Develop a Device Application](#Develop a device application)

  <img src="res/workflow.jpg" style="zoom:120%;" align="left"/>

## Setup Environment

The first thing you need to do is set up your programming environment.

### Get STDK source code

There are two git repositories for working with the SmartThings Device SDK. The first git repository has the Internet of Things (IoT) core device library while the second git repository has sample device applications for reference.

- [IoT Core Device Library](https://github.com/SmartThingsCommunity/st-device-sdk-c)
- [Reference](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref)

#### Download [Reference](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref)

You can choose to only download the [Reference](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref) repository from git, if you use a chipset that has already been ported. In this case, the `IoT Core Device Library` can be easily downloaded as a submodule in the `Reference` by the predefined `setup.sh` script. If you are the first to use this STDK, we strongly recommend  that you choose the [Reference](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref) repository for easier understanding.

From the terminal, navigate to the directory you want the STDK to live and clone it using the following git command:

```sh
$ cd ~
$ git clone https://github.com/SmartThingsCommunity/st-device-sdk-c-ref.git
```

> **Note** : For the rest of this document, we will assume the above path (~/st-device-sdk-c-ref) is the default reference source code.

You can use a script to setup a chipset SDK as follows:

```sh
$ cd ~/st-device-sdk-c-ref
$ ./setup.sh
    Usage: ./setup.sh CHIP_NAME
- - - - - - - - - - - - - - - - - - -
    ex) ./setup.sh esp8266
    ex) ./setup.sh esp32
    ex) ./setup.sh rtl8195
    ex) ./setup.sh rtl8720c

$ ./setup.sh esp8266                       # ./setup.sh {chip_name}
```

### Create device identity

In order for your IoT device to connect to the SmartThings Cloud, there are required authentication data files. To create these authentication data, you must know your MNID in the personal information window of  [Developer Workspace](https://smartthings.developer.samsung.com/workspace/). Please sign in with Samsung Account and check your MNID first.

Open a terminal window and run the following `stdk-keygen` command to create a Device Identity.

```sh
$ cd ~/st-device-sdk-c-ref/iot-core/tools/keygen/linux
$ ./stdk-keygen -h
usage: stdk-keygen -m MNID [-f firmware_version]

$ ./stdk-keygen -m **** -f V201910       # replace '****' with your MNID
Go Device Identity of Developer Workspace.

Serial Number:
┌──────────┬──────────┐
│ STDK**** │ ce**2**3 │
└──────────┴──────────┘

Public Key:
1D********a21F********8WwP********yU/n8vFvM=

$ tree
.
├── ed25519.pubkey
├── ed25519.seckey
└── output_STDK****ce**2**3
    └── device_info.json
```

Copy the Serial Number and Public Key after you run the command on your computer. You will need to upload that value to the SmartThings Cloud via [Developer Workspace](https://smartthings.developer.samsung.com/workspace/projects) during the next phase.

If you create a device identity with a command with an option like above,  You can get the ready-to-use device_info.json file directly.

### Setup toolchain

You must setup a toolchain according to each chipset you selected.

***Example for ESP8266*** :

*For additional information see the following git repositories.*

- *[ESP8266 RTOS Example](https://github.com/espressif/ESP8266_RTOS_SDK/tree/release/v3.2)*
- *[ESP8266 Docs](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/index.html)*

*ESP8266 supports multiple host environments including Windows, Linux, and macOS. Based on experience, compile times are significantly faster on Linux but it is up to you which environment you prefer to use.*

1. *Install Prerequisites*

   *Get the following packages :*

   ```sh
   $ sudo apt-get install gcc git wget make libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-cryptography python-future python-pyparsing python-pyelftools
   ```

   > ***Note :***
   >
   > *Some older Linux distributions may be missing some of the Python packages listed above.*
   >
   > *Also note pyserial version 2.x is not supported by ESP-IDF.*

2. *Setup Toolchain*

   *Get the ESP8266 toolchain for Linux, available on the Expressif website:*

   - *[64-bit Linux](https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz)*

   - *[32-bit Linux](https://dl.espressif.com/dl/xtensa-lx106-elf-linux32-1.22.0-92-g8facf4c-5.2.0.tar.gz)*

   *Once the file is downloaded, extract it in the ~/esp directory.*

   ```sh
   $ mkdir -p ~/esp
   $ cd ~/esp
   $ tar -xzf ~/Downloads/xtensa-lx106-elf-linux64-1.22.0-92-g8facf4c-5.2.0.tar.gz
   ```

   *The above command places the files in the ~/esp/xtensa-lx106-elf/ directory.*

   *According to the original ESP8266 guideline, you will need to add it to your PATH environment variable in ~/.profile file. But If you installed the toolchain along the step above, it is not necessary. Because that path is exported in the build script.*

## Register a Device

Now that the environment is setup, you need to register a device.

First, please sign in to the [Developer Workspace](https://smartthings.developer.samsung.com/workspace/).

### Create a new project

Select Direct-connected device.

<img src="res/direct_connected.jpg" style="zoom:40%;" align="left"/>

Enter a name for the project, and then press "CREATE PROJECT" button to create a direct-connected device project on the SmartThings Cloud.

<img src="res/create_project.jpg" style="zoom:35%;" align="left"/>



### Create a device profile

A device profile contains the components, capabilities, and metadata (ID, name, etc.) that defined in a SmartThings IoT device.

Click the "GO TO DEVICE PROFILE" and then, enter the all remaining information required.

<img src="res/create_device_profile.jpg" style="zoom:50%;" align="left"/>



### Create a device onboarding profile

Click the "GO TO DEVICE ONBOARDING" and then, enter the all remaining information required.

<img src="res/create_onboarding_profile.jpg" style="zoom:50%;" align="left"/>



### Deploy to test

You can publish your device to the SmartThings platform for testing. And then you will be able to access your device through the SmartThings app. Actually this step is for self-testing. If you officially want to publish your device with enrolled organizations ID(e.g. company MNID), please refer to the process below.

- [Official Publishing Process](https://smartthings.developer.samsung.com/docs/devices/publishing/publishing-basics.html)

<img src="res/deploy_to_test.jpg" style="zoom:50%;" align="left"/>



### Upload device identity

This step uploads device identity data generated in the first phase.

Enter the serial number.

<img src="res/device_identity_sn.jpg" style="zoom:50%;" align="left"/>



Paste the public key that copied from the first phase in the Public Key value box that appears

And then, click ADD button.

<img src="res/input_public_key.jpg" style="zoom:50%;" align="left"/>

### Download onboarding_config.json

This is the information that the IoT device must have in order to connect to the SmartThings Cloud. 

If you use a pre-supplied sample device application, please download it and then, just overwrite the existing `onboarding_config.json` file is in the `main` directory of sample device application with the new one you downloaded.

<img src="res/downloading_onboarding_config_file.jpg" style="zoom:50%;" align="left"/>



## Develop a device application

You have the environment set up and you have registered a device, so now it is time to create a new device project.

### Create a new device project

An easy way to get started is to branch off of one of the example projects provided in the git repository. We will use the "st_switch" application from the STDK directory here.

```sh
# Full path of the ESP8266
~/st-device-sdk-c-ref/apps/esp8266/st_switch/
```

### Update device information

Your IoT device needs two pieces of information before connecting to the SmartThings Cloud :

- Device Identity
- onboarding_config.json

1. Device Identity

   Device identity provides data that needs to be sent for authentication with the server. There are two different packets of information, one for the individual developer and one for the manufacturer.

   - Individual developer

     All of the data below should be included in the `device_info.json` file in the `main` directory of the device application.

     **[For ED25519]**

     If you create a device identity with a command with an option like `./stdk-keygen -m **** -f V201910` like the first phase,  you can get the ready-to-use `device_info.json` file directly. In this case, please make sure you overwrite the existing `device_info.json` file with the new one you created.

     > ***Note :***
     >
     > If you are using the `device_info.json` file, please disable the build configuration associated with SmartThings Non-Volatile memory partition as follows. That is, don't set the build configuration below to `y`. :
     >
     > `CONFIG_STDK_IOT_CORE_SUPPORT_STNV_PARTITION=`

     ```sh
     $ cd ~/st-device-sdk-c-ref/iot-core/tools/keygen/linux
     $ tree
     .
     ├── ed25519.pubkey
     ├── ed25519.seckey
     └── output_STDK****ce**2**3
         └── device_info.json

     $ cat output_STDK****ce**2**3/device_info.json
     {
       "deviceInfo": {
         "firmwareVersion": "V201910",
         "privateKey": "3d********AOyY********ezJQ********TMKLGxzbQ=",
         "publicKey": "1D********a21F********8WwP********yU/n8vFvM=",
         "serialNumber": "STDK****ce**2**3"
       }
     }

     # overwrite device_info.json file
     $ cp ./output_STDK****ce**2**3/device_info.json ~/st-device-sdk-c-ref/apps/esp8266/st_switch/main/
     ```

   -  Manufacturer

     For the manufacturer, we cannot place the device identity data in the source code because it is impossible to build and flash every time for each device. To solve this problem, the production level application should store device identity data for each device in a secure location during the manufacturing process. For example, device identity data will be flashed into the SmartThings Non-Volatile memory location.

     > ***Note :***
   >
     > If you want to flash the device identity data in a specific partition, you should set the build configuration below to `y`. : 
     >
     > `CONFIG_STDK_IOT_CORE_SUPPORT_STNV_PARTITION=y`

     **[For ED25519]**

   | Flashed items | Type | Description                   | Examples          |
   | :------------ | :--- | :---------------------------- | :---------------- |
   | PKType        | data | PubKey Algorithm type         | ED25519           |
   | CACert        | file | Server CA Certificate         | root.crt.pem      |
   | PublicKey     | file | Client (= Device) Public key  | device.pubkey.b64 |
   | PrivateKey    | file | Client (= Device) Private key | device.seckey.b64 |
   | SerialNum     | data | Device Serial Number          | cc50e309dcd0      |

2. onboarding_config.json

   Place the `onboarding_config.json` file created during the device registration phase in the `main` directory of device application. Please make sure you overwrite the existing `onboarding_config.json` file with the new one you created like `device_info.json` file above.

   ***Example***

   ```sh
   # Location for ESP8266
   ~/st-device-sdk-c-ref/apps/esp8266/st_switch/main/onboarding_config.json
   ```

   ```sh
   # Example of onboarding_config.json
   {
     "onboardingConfig": {
       "deviceOnboardingId": "T_DOI_SWITCH_001",
       "mnId": "****",
       "setupId": "001",
       "vid": "T_VID_SWITCH_001",
       "deviceTypeId": "Switch",
       "ownershipValidationTypes": [
         "BUTTON"
       ],
       "identityType": "ED25519"
     }
   }
   ```

   - deviceOnboardingId : It is a prefix to be used for the SSID of Soft-AP during Easy-setup process. This value comes from ‘Device Onboarding ID’ when doing `Create a device information` on the DevWS.
     - Example of SSID : STDK[x28d], T_DOI_SWITCH_001[5678]
   - mnId : Manufacturer ID. A unique four-letter ID assigned to SmartThings developers (individual MNID) or enrolled organizations (company MNID) that can be viewed at My Page > MNID.
   - setupId : a unique three-digit number. This value comes from `Device Onboarding` when doing `Create a device onboarding profile` on the DevWS.
   - vid :   An alphanumeric identifier for your device. This value comes from ‘VID(Vendor ID)’ when doing `Create a device profile` on the DevWS.
   - deviceTypeId : This determines the device's icon and default UI layout in the SmartThings app. This is the value you selected from the value given as a list when doing `Create a device profile`.
   - ownershipValidationTypes : This is the type of ownership confirmation used during onboarding process. There are four supported types
     - JUSTWORKS
     - BUTTON
     - PIN
     - QR

   - identityType : A unique certificate or public key pair type used to authenticate a device on SmartThings Cloud. You can choose between
     - ED25519
     - *X.509 (will be supported)*

### Develop device application

A device application is developed using the APIs provided by the IoT Core Device Library. We recommend reuse of the pre-supplied sample device applications, like `st_switch`. This allows for rapid development as you begin to develop your new device. Please refer to the [API references](https://github.com/SmartThingsCommunity/st-device-sdk-c/blob/master/doc/STDK_APIs.pdf) related to the IoT core device library as shown:

- connection management
  - st_conn_init()
  - st_conn_set_noti_cb()
  - st_conn_start()
  - st_conn_cleanup()
  - st_conn_ownership_confirm()

- capability management
  - st_cap_handle_init()
  - st_cap_cmd_set_cb()
  - st_cap_attr_create_int()
  - st_cap_attr_create_number()
  - st_cap_attr_create_string()
  - st_cap_attr_create_string_array()
  - st_cap_attr_free()
  - st_cap_attr_send()

***Main function example for ESP8266*** :

```sh
/**
SmartThings Device SDK(STDK) aims to make it easier to develop IoT devices by providing
additional IoT Core Library to the existing chipset vendor's SW Architecture.

That is, you can simply develop a basic device application by just calling the APIs provided by IoT Core Device Library like below. Currently, IoT Core Device Library offers 13 APIs.

//create an iot context
1. st_conn_init();

//create a handle to process capability
2. st_cap_handle_init();

//register a callback function to process capability command when it comes from the SmartThings Cloud.
3. st_cap_cmd_set_cb();

//needed when it is necessary to keep monitoring the device status
4. user_defined_task()

//process on-boarding procedure. There is nothing more to do on the app side than call the API.
5. st_conn_start();
*/

void app_main(void)
{

    unsigned char *onboarding_profile = (unsigned char *) onboarding_profile_start;
    unsigned int onboarding_profile_len = onboarding_profile_end - onboarding_profile_start - 1;
    unsigned char *device_info = (unsigned char *) device_info_start;
    unsigned int device_info_len = device_info_end - device_info_start - 1;
    IOT_CAP_HANDLE *handle = NULL;
    int iot_err;

    // 1. create an iot context
    ctx = st_conn_init(onboarding_profile, onboarding_profile_len, device_info, device_info_len);
    if (ctx != NULL) {
      iot_err = st_conn_set_noti_cb(ctx, cap_switch_noti_cb, NULL);
      if (iot_err)
        printf("fail to set notification callback function\n");

    // 2. create a handle to process capability
    // implement init_callback function (cap_switch_init_cb)
    handle = st_cap_handle_init(ctx, "main", "switch", cap_switch_init_cb, NULL);

    // 3. register a callback function to process capability command when it comes from the SmartThings Cloud
    // implement callback function (cap_switch_cmd_off_cb)
    iot_err = st_cap_cmd_set_cb(handle, "off", cap_switch_cmd_off_cb, NULL);
    if (iot_err)
      printf("fail to set cmd_cb for off\n");

    // implement callback function (cap_switch_cmd_on_cb)
    iot_err = st_cap_cmd_set_cb(handle, "on", cap_switch_cmd_on_cb, NULL);
    if (iot_err)
      printf("fail to set cmd_cb for on\n");

    } else {
      printf("fail to create the iot_context\n");
    }

    // 4. needed when it is necessary to keep monitoring the device status
    xTaskCreate(smartswitch_task, "smartswitch_task", 2048, (void *)handle, 10, NULL);

    // 5. process on-boarding procedure. There is nothing more to do on the app side than call the API.
    st_conn_start(ctx, (st_status_cb)&iot_status_cb, IOT_STATUS_ALL, NULL, NULL);

}
```

### Build & Flash the App

Go to the root directory of the STDK and execute the build script(build.sh) with the below parameter.

```sh
# Example for ESP8266
$ cd ~/st-device-sdk-c-ref/
$ ./build.sh esp8266 st_switch             # ./build.sh {chip_name} {app_directory}
```

After compiling, following results can be seen. In fact, as you know, we have to flash these results according to the binary download method that exists per chipset.

```sh
# Example for ESP8266
$ cd ~/st-device-sdk-c-ref/
$ tree output/ -L 3
output/
`-- esp8266
    `-- iotcore_st_switch_20191125_ab144f7_d63f3d2    #iotcore_'app_name'_'date'_'commit'
        |-- address_info.txt
        |-- bootloader.bin
        |-- debug
        |-- partition.bin
        `-- st_switch_demo.bin
```

In case of ESP8266, you can now execute the following command to flash the entire binaries(e.g. app, bootloader, and init data bin) to a new chipset.

```sh
# Example for ESP8266
$ cd ~/st-device-sdk-c-ref/
$ ./build.sh esp8266 st_switch flash
```

The serial port needs to be matched to the computer environment for serial port flashing. For example, in ESP8266 the settings for serial port flashing can be configured with `make menuconfig`. If the serial port setting does not match your environment, please execute the following:

```sh
# Example for ESP8266
$ cd ~/st-device-sdk-c-ref/apps/esp8266/st_switch
$ make menuconfig
```

You don't need to run ‘./build.sh esp8266 st_switch’ before running “./build.sh esp8266 st_switch flash”, this will automatically rebuild anything which needs it.

### Test

The SmartThings App should be used to control an IoT device that is running with the SmartThings Cloud. Go to Google Play or the iOS App Store and download the SmartThings app on your phone.

| Step | Description                                                  |
| :--: | ------------------------------------------------------------ |
|  1   | **Enable developer mode**<br />You must enable the `Developer Mode` in the SmartThings app before testing<br />Please refer to the link below<br />https://smartthings.developer.samsung.com/docs/testing/developer-mode.html |
|  2   | **Reset the device**<br />Just push the reset button of device or run the command below in the console window.<br />  $ cd ~/st-device-sdk-c-ref<br />  $ ./build.sh esp8266 st_switch monitor         # This is only for ESP8266 |
|  3   | **Add device(Onboarding Process)**<br />There are two ways to add a device in SmartThings application.<br />1. Select the Device Onboarding Name via "My Testing Devices" menu.(Left Fig.) <br />2. Use the Detection popup window(Right Fig.)<br /><img src="res/my_testing_device.jpg" style="zoom:75%;" align="left"/><img src="res/easy_setup.jpg" style="zoom:75%;" align="right"/> |
|  4   | **Control & Monitor a device**<br />Now that your device is on the SmartThings App. If there is no problem during above onboarding process, it means your device is well registered on the SmartThings Cloud. <br />Control & Monitor your device via the App and make sure the App is working the way you think it should.<br /><img src="res/added_device.jpg" style="zoom:75%;" align="left"/> |
