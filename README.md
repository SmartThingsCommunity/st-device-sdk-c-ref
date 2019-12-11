# SmartThings Device SDK Reference

[![License](https://img.shields.io/badge/licence-Apache%202.0-brightgreen.svg?style=flat)](LICENSE)

The SmartThings Device SDK Reference is the git repository of examples using the core device library that allow device applications to securely connect to the SmartThings Cloud. To facilitate the development of device application in an original chipset SDK, the core device library and the examples were separated into two git repositories. That is, if you want to use the core device library in your original chipset SDK that installed before, you may simply link it to develop a device application in your existing development environment.

It is distributed in source form and written in C99 for the purpose of portability to most platforms. If you want to know the overall workflow of using this SDK, please refer to the [Getting Started](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref/blob/master/doc/getting_started.md).

## Directory layout

The reference examples git is delivered via the following directory structure :

- `apps` : sample device applications for each chipset
- `bsp` : original chipset vendor's SDKs as submodules
- `doc` : documents
- `iot-core` : core device library as submodule
- `output` : build outputs will be placed
- `patches` : patches to be applied in the original chipset vendor's SDK for resolving some problems
- `tools` : scripts to be applied for each chipset

## Building a sample device application

Basically, this release builds on the environments of chipset SDKs.

### Prerequisites

- Install the toolchain defined in the chipset SDK used.
  - Example for ESP32 (Ubuntu/Debian quickstart)
    - Get the ESP toolchain for Linux, available on the Expressif website:
      - [64-bit Linux](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started-get-prerequisites)
      Steps (Ubuntu, Debian)
      1. sudo apt-get install git wget flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache libncurses-dev
      2. mkdir ~/esp; cd esp; git clone --recursive https://github.com/espressif/esp-idf.git; cd esp-idf
      3. pip install --user -r ./requirements.txt
      4. git checkout --track remotes/origin/release/v3.3
      5. ./install.sh esp32
      6. . ./export.sh
      7. Proceed to [build step](https://github.com/SmartThingsCommunity/st-device-sdk-c-ref/blob/master/README.md#Build).

  - Example for ESP8266
    - Setup [Toolchain for Linux](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/linux-setup.html) according to the available Expressif website.
      In order to use a pre-supplied build script, please extract the toolchain into `~/esp/xtensa-lx106-elf/` directory like the original Expressif guide.

  - Example for RTL8195
    - Install [mbed Microcontroller](https://os.mbed.com/handbook/Windows-serial-configuration) to use the USB serial port on Windows.
    - Upgrade [DAP F/W](https://www.amebaiot.com/en/change-dap-firmware/)
      Recommended DAP F/W is the `DAP_FW_Ameba_V12_1_3-2M.bin`

### Build

1. Clone the source code and submodules. Basically, the SmartThings Device SDK Reference will use ported original chipset vendor's SDKs as submodules in the `bsp` directory. If a chipset vendor's SDK does not exist as git format, you can manually copy it under the `bsp` directory.

   - Download the source code via `git clone`.
   - And then, use the `setup.sh` to clone submodules automatically in the SDK's root directory.
     ```sh
     $ cd ~
     $ git clone https://github.com/SmartThingsCommunity/st-device-sdk-c-ref.git
     $ cd st-device-sdk-c-ref
     $ ./setup.sh esp8266                       # ./setup.sh {chip_name}
     ```

2. Check the build configuration of a sample device application. If you want to use specific build options, you can directly modify the build configuration file(e.g. sdkconfig, sdkconfig.h) at the root directory of a sample device application. On the Espressif chipset, you can additionally use the `menuconfig` option to configure them.

   - If you use a default build configuration file, you can skip this step.
   - Example for ESP8266
     > Note :
     > The `menuconfig` option is just supported on the Espressif chipset.
     ```sh
     # ./build.sh {chip_name} {app_name} {option}
     $ cd ~/st-device-sdk-c-ref
     $ ./build.sh esp8266 st_switch menuconfig
     ```

3. Run build.sh in the SDK's root directory. This builds the sample executables and places them in `output/{chip_name}/`.

   - build
     ```sh
     $ ./build.sh esp8266 st_switch          # ./build.sh {chip_name} {app_name}
     ```

### flash & monitor

Serial port needs to be matched to the computer environment for serial port flashing and monitoring.

- Example for ESP8266
  - Baud rate 115200 for flashing or 74880 for monitoring, Date bit 8, Parity None, Stop bits 1
  - These data are described in the build configuration file(e.g.  sdkconfig)
- Example for RTL8195
  - Baud rate 115200, Date bit 8, Parity None, Stop bits 1

Flashing can be done according to the method supported by chipset SDK.

- Example for ESP8266

  You can flash the executables into a target device via build.sh with additional option. Actually, you don't need to run `./build.sh esp8266 st_switch` before running `./build.sh esp8266 st_switch flash`, this will automatically rebuild anything which needs it.

  - options
    - clean : clean previous build outputs
      ```sh
      # ./build.sh {chip_name} {app_name} {options}
      $ ./build.sh esp8266 st_switch clean
      ```
    - flash : download executable binaries to the device
    - monitor : monitor the serial output of device. this option can be used with flash option.
      ```sh
      # ./build.sh {chip_name} {app_name} {options}
      $ ./build.sh esp8266 st_switch flash monitor
      ```

- Example for RTL8195

  In order to flash, you have to proceed at Windows PC, even if you build the source code for the RTL chipset on Linux.
  - Connect Ameba RTL8195 to Windows PC, you can find removable disk named MBED.
  - Just copy `ram_all.bin` in `output/rtl8195/iotcore_xxx/ram_all.bin` to MBED.
  - After copy successfully, please reset Ameba RTL8195 target and monitor

## License

This library is licensed under the [Apache License Ver2.0](LICENSE).
