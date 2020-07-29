# SmartThings Device SDK(STDK) Switch Example

## Introduction

SmartThings direct-connected device is Wi-Fi enabled device that uses the SmartThings cloud as its primary cloud infrastructure. And this device will use the MQTT protocol for communication.

## Getting started

For information on detailed workflow, please refer to the [Getting Started](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref/blob/master/doc/getting_started.md)

## STDK config
If you want to use specific STDK build options, you can directly modify the build configuration file. For esp32 example, STDK config is saved in 'sdkconfig' file. If you want to change this, please execute the following :
```sh
# ./build.sh {app_path} {option}
$ cd ~/st-device-sdk-c-ref/
$ ./build.sh app/esp32/switch_example menuconfig
```

