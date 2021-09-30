# SmartThings SDK Reference for Direct Connected Devices for C

[![License](https://img.shields.io/badge/licence-Apache%202.0-brightgreen.svg?style=flat)](LICENSE)

Easily integrate your Direct Connected Device using the Direct Connected Devices SDK. The SDK manages all MQTT topics and onboarding requirements, freeing you to focus on the Capabilities of your device.

> To facilitate development of device applications in an original chipset SDK, the [IoT core device library](https://github.com/SmartThingsCommunity/st-device-sdk-c) and supporting examples are separated into two git repositories:
the [IoT core device library](https://github.com/SmartThingsCommunity/st-device-sdk-c) and the Direct Connected Devices SDK repository. If you want to use the IoT core device library in your original chipset SDK that you previously installed, link the core device library to develop a device application in your existing development environment.

The Direct Connected Devices SDK is distributed in source form and written in C99 for the purpose of portability to most platforms. For a complete overview of the development process and workflow, please refer to the [Direct Connected Devices](https://developer-preview.smartthings.com/docs/devices/direct-connected/get-started) documentation.

## Directory Structure and Overview

- `apps`: Contains sample device applications for various chipsets.
- `bsp`: Contains the original chipset vendors' SDKs.  If you use a chipset that has already been ported, this vendor's SDK can easily be downloaded as a submodule in this directory through the predefined script `setup.py`.
- `doc`: Documentation.
- `iot-core`: Contains the [IoT core device library](https://github.com/SmartThingsCommunity/st-device-sdk-c). It can also be downloaded as a submodule in this directory through the predefined script `setup.py`.
- `output`: Where build outputs will be placed.
- `patches` : Patches to be applied to the original chipset vendors' SDKs for resolving various problems.
- `tools`: Various scripts to be applied for each chipset.

## Build a Sample Device Application

The Direct Connected Devices SDK builds on the environments of each chipset vendor's SDK, providing an all-in-one development package.

### Prerequisites

Install the toolchain defined in the chipset SDK you use according to the directions below. If no development environment is described for your chipset, it is assumed to only be applicable for 64-bit Ubuntu-based environments.

#### For ESP8266 (Ubuntu/Debian Quickstart):
1. Set up the ESP8266 Toolchain for Linux according to the [Espressif documentation](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/linux-setup.html).
2. In order to use the pre-supplied SDK build script `build.py`, extract [the toolchain](https://dl.espressif.com/dl/xtensa-lx106-elf-linux64-1.22.0-100-ge567ec7-5.2.0.tar.gz) into the `~/esp/xtensa-lx106-elf/` directory as directed in the Expressif documentation linked above.

> The Espressif documentation linked above directs you to add the toochain path to your `PATH` environment variable in your `~/.profile`. This step *not* necessary if you use the pre-supplied build script `build.py` - the path is automatically exported when using the build script.

#### For ESP32:
1. Install [Prerequisites](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html#step-1-install-prerequisites) for your build system OS.
2. Set up the ESP32 toolchain with `setup.py`

#### For ESP32_v3.3 (Legacy - Ubuntu/Debian Quickstart):
1. Set up the ESP32 Toolchain for Linux according to the [Espressif documentation](https://docs.espressif.com/projects/esp-idf/en/release-v3.3/get-started/index.html#setup-toolchain).
1. In order to use the pre-supplied build script `build.py`, extract [the toolchain](https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz) to `~/esp/xtensa-esp32-elf/` as directed in the original Expressif documentation.

> The Espressif documentation linked above directs you to add the toochain path to your `PATH` environment variable in your `~/.profile`. This step *not* necessary if you use the pre-supplied build script `build.py` - the path is automatically exported when using the build script.

#### For RTL8195:

For Windows systems:

1. Install the [mbed Microcontroller](https://os.mbed.com/handbook/Windows-serial-configuration) to use the USB serial port on Windows.
1. Upgrade [DAP firmware](https://www.amebaiot.com/en/change-dap-firmware/). The recommended DAP firmware is `DAP_FW_Ameba_V12_1_3-2M.bin`.

> The above steps are applicable for a Windows PC for downloading a binary. You must build the source code on Cygwin or linux for this chipset.

#### For EMW3166:

1. Set up the [MiCoder Toolchain for Linux](http://firmware.mxchip.com/MiCoder_v1.1.Linux.tar.gz) according to the MXCHIP documentation.
1. Install [MiCO Cube](http://developer.mxchip.com/developer/md/bWljby1oYW5kYm9vay8yLk1pQ09fdG9vbHMvc2VjX2xpbmtfcGFnZS9NaUNPX0N1YmVfRW5nbGlzaC5tZA). In order to use the pre-supplied build script `build.py`, set `mico config --global MICODER /path_of_the_toolchain`

### Build

1. Download the Direct Connected Device SDK source code as shown in the example below and run `setup.py`.

> `setup.py` will automatically download the ported original chipset vendor's SDKs as submodules in the `bsp` directory through the `setup.py` script. If a chipset vendor's SDK does not exist in git format, you can manually copy it to the `bsp` directory. At this time, the IoT Core Device Library will also be downloaded to the `iot-core` directory.

 ```sh
 $ cd ~
 $ git clone https://github.com/SmartThingsCommunity/st-device-sdk-c-ref.git
 $ cd st-device-sdk-c-ref
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
 ex) python setup.py esp32c3
 ```

2. Check the build configuration of a sample device application (`apps` folder). If you want to use specific build options, you can directly modify the build configuration file (e.g. `sdkconfig`, `sdkconfig.h`) at the root directory of a sample device application. On the Espressif chipset, you can additionally use the `menuconfig` option for configuration.

> If you want to use the default build configuration, you can skip this step.

   - ESP8266 Example:
     > Note that the `menuconfig` option is only supported on the Espressif chipset.
     ```sh
     # python build.py {app_path} {option}
     $ cd ~/st-device-sdk-c-ref
     $ python build.py apps/esp32/switch_example menuconfig
     ```

3. Run `build.py` in the SDK's root directory. This builds the sample executables and places them in `output/{chip_name}/`.

   ```sh
   $ python build.py apps/esp32/switch_example          # python build.py {app_path}

   ```

### Flash and Monitor

The serial port of your device must to be matched to the computer environment for serial port flashing and monitoring. This data is described in the build configuration file (`sdkconfig`)


|Chipset|Buad Rate for Flashing|Baud Rate for Monitoring|Data Bit|Parity|Stop Bits|
|-------|----------------------|------------------------|--------|------|---|
|ESP8266/ESP32|115200|74880|8|None|1|
|RTL8195|115200|115200|8|None|1|
|EMW3166|9600 (user UART)|115200 (USB UART)|8|None|1|

#### Flashing

Flashing can be done according to the method outlined by the applicable chipset SDK.

##### ESP8266/ESP32 Example:

You can flash the executables into a target device via `build.py` with additional options.

> You do not need to run `python build.py esp8266 st_switch` before running `python build.py esp8266 st_switch flash`; this will automatically rebuild anything which needs it.

Available Options:

- **clean**: clean previous build outputs
  ```sh
  # python build.py {app_path} {options}
  $ python build.py apps/esp32/switch_example clean
  ```
- **flash**: download executable binaries to the device
- **monitor**: monitor the serial output of device. this option can be used with flash option.
  ```sh
  # python build.py {app_path} {options}
  $ python build.py apps/esp32/switch_example flash monitor
  ```

##### RTL8195 Example:

> In order to flash the RTL chipset binary, the steps below must be performed on a Windows PC, even if you build the source code on a Linux environment.

1. Connect Ameba RTL8195 to a Windows PC; a removable disk `MBED` will appear.
1. Copy `ram_all.bin` from `output/rtl8195/iotcore_xxx/ram_all.bin` to MBED.
1. After successfully copying, reset Ameba RTL8195 target and monitor.

##### EMW3166 Example

> In order to flash the MXCHIP emw3166 chipset binary, the steps below must be performed on a Windows PC with a terminal that supports Ymodem transmission.

1. Connect user UART (PB7-tx, PB6-rx) of emw3166 to a Windows PC with a USB-TTL converter. Then connect USB UART power.
1. Create a serial session with flashing config (Baud rate 9600) to connect the user UART port, then hold 'BOOT' and press 'RESET' to enable flashing.
1. Transfer the binary file located in `output/emw3166/iotcore_xxx/xxx@MK3166.bin` using Ymodem on your terminal.
1. After transfer is successful, press 'RESET' on MiCOKit-3166 target and monitor with USB UART.

## License

This library is licensed under the [Apache License Ver2.0](LICENSE).
