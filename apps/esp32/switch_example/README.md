# SmartThings Device SDK Switch Example

## Introduction

SmartThings direct-connected device is Wi-Fi enabled device that uses the SmartThings cloud as its primary cloud infrastructure. And this device will use the MQTT protocol for communication.

## Getting started

For information on detailed workflow, please refer to the [Getting Started](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref/blob/master/doc/getting_started.md)

## Components and Capabilities

SmartThings Device is defined using components and capabilities. Capabilities define the features of the device, and capabilities are grouped into components.
Components and Capabilities are contained in device profile. You can create a device profile in Developer Workspace and associate it with an integration.

This example assumes the following component and capabilities are used. :  

'main' component  
- 'healthCheck' capability  
- 'switch' capability  

('healthCheck' capability is automatically added by Developer Workspace. It doesn't need handler at device side)

## SmartThings Device SDK config
If you want to use specific SmartThings Device SDK build options, you can directly modify the build configuration file. For this example, SmartThings Device SDK config is saved in 'sdkconfig' file. If you want to change this, please execute the following :
```sh
# ./build.sh {app_path} {option}
$ cd ~/st-device-sdk-c-ref/
$ ./build.sh app/esp32/switch_example menuconfig
```

