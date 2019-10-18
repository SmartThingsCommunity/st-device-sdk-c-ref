# SmartThings Device SDK Reference

[![License](https://img.shields.io/badge/licence-Apache%202.0-brightgreen.svg?style=flat)](LICENSE)

The SmartThings Device SDK Reference is the git repository of examples using the core device library that allow device applications to securely connect to the SmartThings Cloud Server. To facilitate the development of device application in an original chipset SDK,  the device core library and the example was separated into two git repositories. That is, if you want to use the core device library in your original chipset SDK that installed before, you may simply link this core device library to develop a device application in your existing development environment.

It is distributed in source form and written in C99 for the purpose of portability to most platforms. If you want to know the overall workflow, please refer to the [Getting Started](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref/doc/getting_started.md).

## Directory layout

The reference SDK is delivered via the following directory structure :

- apps : sample device applications for each chipset
- bsp : original chipset vendor's SDKs as submodules
- iot-core : core device library as submodule
- patches : patches to be applied in the original chipset vendor's SDK for resolving some problems
- tools : scripts to be applied for each chipset

## Building a sample device application

Basically, this release builds on the environments of chipset SDKs .

### Prerequisites

- Install the toolchain defined in the chipset sdk used.

### Build

1. Clone the source code and submodules. This IoT device SDK uses original chipset vendor's SDKs as submodules in the `bsp` directory.

   - Download the source code via `git clone`.

   - And then, use `setup.sh` to clone submodules automatically in the SDK's root directory.

     ```sh
     cd ~
     git clone https://github.com/SmartThingsCommunity/st-device-sdk-c-ref.git
     cd st-device-sdk-c-ref
     ./setup.sh esp8266                       # ./setup.sh {chip_name}
     ```

2. Check the build configuration of a sample device application. If you want to use specific build options, you can modify the build configuration file at the root directory of a sample device application.

   - The build configuration example of switch device for ESP8266.

     ```sh
     cd ~/st-device-sdk-c-ref/apps/esp8266/st_switch
     vi sdkconfig
     ```

   - If you use a default build options, you can skip this step.

3. Run build.sh in the SDK's root directory. This builds the sample executables and places them in `output/{chip_name}/`.

   - build

     ```sh
     ./build.sh esp8266 st_switch          # ./build.sh {chip_name} {app_name}
     ```

### flash & monitor

Serial port needs to be matched to the computer environment for serial port flashing and monitoring. You can flash the executables into a target device via build.sh with additional option. Actually, you don't need to run `./build.sh esp8266 st_switch` before running `./build.sh esp8266 st_switch flash`, this will automatically rebuild anything which needs it.

- options

  - clean : clean previous build outputs

    ```sh
    # ./build.sh {chip_name} {app_name} {options}
    ./build.sh esp8266 st_switch clean
    ```

  - flash : download executable binaries to the device

  - monitor : monitor the serial output of device. this option can be used with flash option.

    ```sh
    # ./build.sh {chip_name} {app_name} {options}
    ./build.sh esp8266 st_switch flash monitor
    ```

## License

This library is licensed under the [Apache License Ver2.0](LICENSE).
