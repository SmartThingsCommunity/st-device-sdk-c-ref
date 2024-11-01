# SmartThings SDK Reference for Direct Connected Devices for C

[![License](https://img.shields.io/badge/licence-Apache%202.0-brightgreen.svg?style=flat)](LICENSE)

Easily integrate your Direct Connected Device using the Direct Connected Devices SDK. The SDK manages all MQTT topics and onboarding requirements, freeing you to focus on the Capabilities of your device.

> To facilitate development of device applications in an original chipset SDK, the [IoT core device library](https://github.com/SmartThingsCommunity/st-device-sdk-c) and supporting examples are separated into two git repositories:
the [IoT core device library](https://github.com/SmartThingsCommunity/st-device-sdk-c) and the Direct Connected Devices SDK repository. If you want to use the IoT core device library in your original chipset SDK that you previously installed, link the core device library to develop a device application in your existing development environment.

The Direct Connected Devices SDK is distributed in source form and written in C99 for the purpose of portability to most platforms. For a complete overview of the development process and workflow, please refer to the [Direct Connected Devices](https://developer.smartthings.com/docs/devices/direct-connected/get-started) documentation.

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
2. In order to use the pre-supplied SDK build script `build.py`, extract [the toolchain](https://dl.espressif.com/dl/xtensa-lx106-elf-gcc8_4_0-esp-2020r3-linux-amd64.tar.gz) into the `~/esp/xtensa-lx106-elf/` directory as directed in the Expressif documentation linked above.

> The Espressif documentation linked above directs you to add the toochain path to your `PATH` environment variable in your `~/.profile`. This step *not* necessary if you use the pre-supplied build script `build.py` - the path is automatically exported when using the build script.

#### For ESP32:
1. Install [Prerequisites](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/get-started/index.html#step-1-install-prerequisites) for your build system OS.
2. Set up the ESP32 toolchain with `setup.py`

#### For ESP32_v3.3 (Legacy - Ubuntu/Debian Quickstart):
1. Set up the ESP32 Toolchain for Linux according to the [Espressif documentation](https://docs.espressif.com/projects/esp-idf/en/release-v3.3/get-started/index.html#setup-toolchain).
1. In order to use the pre-supplied build script `build.py`, extract [the toolchain](https://dl.espressif.com/dl/xtensa-esp32-elf-linux64-1.22.0-80-g6c4433a-5.2.0.tar.gz) to `~/esp/xtensa-esp32-elf/` as directed in the original Expressif documentation.

> The Espressif documentation linked above directs you to add the toochain path to your `PATH` environment variable in your `~/.profile`. This step *not* necessary if you use the pre-supplied build script `build.py` - the path is automatically exported when using the build script.

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
 ex) python setup.py esp32_v3.3
 ex) python setup.py esp8266
 ex) python setup.py esp32
 ex) python setup.py esp32s2
 ex) python setup.py esp32c3
 ```

2. Check the build configuration of a sample device application (`apps` folder). If you want to use specific build options, you can directly modify the build configuration file (e.g. `sdkconfig`, `sdkconfig.h`) at the root directory of a sample device application. On the Espressif chipset, you can additionally use the `menuconfig` option for configuration.

> If you want to use the default build configuration, you can skip this step.

   - ESP32 Example:
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

#### Flashing

Flashing can be done according to the method outlined by the applicable chipset SDK.

##### ESP8266/ESP32 Example:

You can flash the executables into a target device via `build.py` with additional options.

> You do not need to run `python build.py esp32 st_switch` before running `python build.py esp32 st_switch flash`; this will automatically rebuild anything which needs it.

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

## License

This library is licensed under the [Apache License Ver2.0](LICENSE).
