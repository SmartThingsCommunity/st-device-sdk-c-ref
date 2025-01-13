# NVS encryption setup on ESP32

NVS encryption for ESP32 helps to achieve secure storage on the device flash memory. Data stored in NVS partitions can be encrypted using XTS-AES.

The keys required for NVS encryption are stored in yet another partition, which is protected using Flash Encryption. Therefore, enabling Flash Encryption becomes a prerequisite for NVS encryption.

This guide uses `st-device-sdk-c-ref` repo and is setup for ESP32, which clones firmware `esp-idf` v4.3.1

## Export environment variables

Source the esp-idf export script:

```sh
$ cd st-device-sdk-c-ref/bsp/esp32
$ source export.sh
```

## Sample Application

This guide demonstrates reading and writing into custom nvs file before and after encryption. Accordingly, there are two sample applications provided for plaintext and encrypted custom nvs. Both the applications make use of the same example code that is located in `st-device-sdk-c-ref/bsp/esp32/examples/security/flash_encryption`.

The following changes are to be additionally incorporated in `flash_encryption/main/flash_encrypt_main.c` for both the applications, to operate on custom nvs file:

- Define a macro for the name of the Custom NVS partition.

  ```c
  #define CUSTOM_NVS_PART_NAME "custom_nvs"
  ```

- Initialization of custom nvs partition:

  The API works for both the cases, when nvs encryption is enabled and is disabled. When nvs encryption is enabled, nvs keys is first read, using which custom nvs partition is initialized.

  ```c
  esp_err_t custom_nvs_part_initialize(const char *name)
  {
  #if CONFIG_NVS_ENCRYPTION
      esp_err_t ret = ESP_FAIL;

      // 1. find partition with nvs_keys
      const esp_partition_t *key_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                 ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS,
                                                                 "nvs_key");
      if (key_part == NULL) {
          ESP_LOGE(TAG, "CONFIG_NVS_ENCRYPTION is enabled, but no partition with subtype nvs_keys found in the partition table.");
          return ret;
      }

      // 2. read nvs_keys from key partition
      nvs_sec_cfg_t cfg = {};
      ret = nvs_flash_read_security_cfg(key_part, &cfg);
      if (ret != ESP_OK) {
          ESP_LOGE(TAG, "Failed to read nvs keys [rc=0x%x] (%s)", ret, esp_err_to_name(ret));
          return ret;
      }

      // 3. initialize nvs partition
      ret = nvs_flash_secure_init_partition(name, &cfg);
      if (ret != ESP_OK) {
          ESP_LOGE(TAG, "Failed to initialize custom nvs partition [ret=0x%x] (%s)", ret, esp_err_to_name(ret));
          return ret;
      }
      ESP_LOGI(TAG, "NVS partition \"%s\" is encrypted.", name);

      return ret;
  #else
      return nvs_flash_init_partition(name);
  #endif
  }
  ```

- Read/Write in the custom nvs partition:

  The custom nvs partition is opened first. Reading and writing in the encrypted custom nvs partition is performed using regular `nvs_get_*` and `nvs_set_*` APIs.

  ```c
  void example_custom_nvs_part_read_write(void)
  {
      nvs_handle nvs_handle;
      nvs_open_mode nvs_open_mode = NVS_READWRITE;
      uint16_t u16_val = 1000;
      char *str_val;
      size_t str_len;

      esp_err_t err = nvs_open_from_partition(CUSTOM_NVS_PART_NAME, "storage", nvs_open_mode, &nvs_handle);

      // Need to write using nvs_set_ API
      err = nvs_set_u16(nvs_handle, "u16_key", u16_val);
      assert(err == ESP_OK);
      ESP_LOGI(TAG, "Write - custom nvs partition: %s, key: %s, value: %d", CUSTOM_NVS_PART_NAME, "u16_key", u16_val);

      // Need to read using nvs_get_ API
      err = nvs_get_u16(nvs_handle, "u16_key", &u16_val);
      assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
      ESP_LOGI(TAG, "Read  - custom nvs partition: %s, key: %s, value: %d", CUSTOM_NVS_PART_NAME, "u16_key", u16_val);

      // Obtain str length to read str value
      err = nvs_get_str(nvs_handle, "str_key", NULL, &str_len);
      str_val = malloc(str_len);

      err = nvs_get_str(nvs_handle, "str_key", str_val, &str_len);
      assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
      ESP_LOGI(TAG, "Read  - custom nvs partition: %s, key: %s, value: %s, len: %zu", CUSTOM_NVS_PART_NAME, "str_key", str_val, str_len);
  }
  ```

- Main of the application:

  In the `app_main` API, the above defined APIs are invoked for initialization and read/write to custom nvs partition.

  ```c
  void app_main()
  {
      // Existing code
      // ...

      //Custom-nvs partition initialization
      ret = custom_nvs_part_initialize(CUSTOM_NVS_PART_NAME);
      if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
          ESP_ERROR_CHECK(nvs_flash_erase_partition(CUSTOM_NVS_PART_NAME));
          ret = custom_nvs_part_initialize(CUSTOM_NVS_PART_NAME);
      }
      ESP_ERROR_CHECK(ret);

      //Read from custom-nvs partition
      example_custom_nvs_part_read_write();
  }
  ```

## Sample Application 1 : NVS plaintext

### Setup

- Create a partition table csv for custom nvs: `partitions_example.csv`

  ```sh
  ##Sample partition table csv content
  # Name,   Type, SubType, Offset,  Size, Flags
  nvs,        data, nvs,      ,  0x6000,
  # Extra partition to demonstrate reading/writing of encrypted flash
  storage,    data, 0xff,     ,  0x1000,
  factory,    app,  factory,  , 1M,
  # Custom NVS data partition
  custom_nvs, data, nvs,      ,  0x6000,
  ```

- Create custom nvs csv: `sample_nvs_data.csv`

  ```sh
  ## Sample custom nvs csv file
  key,type,encoding,value
  storage,namespace,,
  u8_key,data,u8,255
  i8_key,data,i8,-128
  u16_key,data,u16,65535
  u32_key,data,u32,4294967295
  i32_key,data,i32,-2147483648
  str_key,data,string,"Lorem ipsum dolor sit amet"
  ```

- Create the custom nvs bin file:

  ```sh
  $ python nvs_partition_gen.py generate sample_nvs_data.csv custom_nvs.bin 0x3000
  ```

  The `nvs_partition_gen.py` can be obtained at: `st-device-sdk-c-ref/bsp/esp32/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py`
- Flash Partition table:

  ```sh
  $ idf.py partition_table partition_table-flash
  ```

- Flash custom nvs partition:
  Use the appropriate port & offset for custom nvs partition for flashing.

  ```sh
  $ esptool.py -p /dev/ttyUSB0 --before default_reset --after no_reset write_flash 0x120000 custom_nvs.bin
  ```

> **Note :**
>
> The offset used above to flash custom nvs bin file can be obtained after build with:\
> $ gen_esp32part.py build/partition_table/partition-table.bin

### Build and flash the application

Use the appropriate port for flashing to ESP32

```sh
$ idf.py -p /dev/ttyUSB0 flash monitor
```

### Output

NVS plaintext: When flash encryption is disabled and custom nvs is in plaintext.

```sh
Example to check Flash Encryption status
This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 0, 2MB external flash
FLASH_CRYPT_CNT eFuse value is 0
Flash encryption feature is disabled
Erasing partition "storage" (0x1000 bytes)
Writing data with esp_partition_write:
I (416) example: 0x3fc8fb60   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (416) example: 0x3fc8fb70   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_partition_read:
I (436) example: 0x3fc8fb80   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (446) example: 0x3fc8fb90   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with spi_flash_read:
I (456) example: 0x3fc8fb80   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (466) example: 0x3fc8fb90   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
I (486) example: Write - custom nvs partition: custom_nvs, key: u16_key, value: 1000
I (486) example: Read - custom nvs partition: custom_nvs, key: u16_key, value: 1000
I (486) example: Read - custom nvs partition: custom_nvs, key: str_key, value: "Lorem ipsum dolor sit amet", len: 29
```

## Sample Application 2 : NVS encrypted

### Configuration

```sh
$ cd st-device-sdk-c-ref/bsp/esp32/examples/security/flash_encryption
$ idf.py menuconfig
```

- Enable the flash encryption mode:

  Under `Security features`, check `Enable flash encryption on boot` option.\
  Default usage mode is Development (recommended during test and development phase). However this can be changed under the `Enable usage mode` option.
- Update the partition table offset:

  Enabling flash encryption increases bootloader size. So the offset of partition table must be increased from `0x8000` to `0x9000`.\
  Under `Partition Table`, update this size in `Offset of partition table` option.

### Setup

- Create a partition table csv for custom nvs and nvs key partitions: `partitions_example.csv`

  ```sh
  ##Sample partition table csv content
  # Name,   Type, SubType, Offset,  Size, Flags
  nvs,        data, nvs,      ,  0x6000,
  # Extra partition to demonstrate reading/writing of encrypted flash
  storage,    data, 0xff,     ,  0x1000, encrypted
  factory,    app,  factory,  , 1M,
  # nvs_key partition contains the key that encrypts the NVS partition named nvs. The nvs_key partition needs to be encrypted.
  nvs_key,    data, nvs_keys, , 0x1000, encrypted,
  # Custom NVS data partition
  custom_nvs, data, nvs,      ,  0x6000,
  ```

- Create custom nvs csv: `sample_nvs_data.csv`

  ```sh
  ## Sample custom nvs csv file
  key,type,encoding,value
  storage,namespace,,
  u8_key,data,u8,255
  i8_key,data,i8,-128
  u16_key,data,u16,65535
  u32_key,data,u32,4294967295
  i32_key,data,i32,-2147483648
  str_key,data,string,"Lorem ipsum dolor sit amet"
  ```

- Create nvs keys:

  ```sh
  $ python nvs_partition_gen.py generate-key --keyfile encryption_keys.bin
  ```

  The `nvs_partition_gen.py` can be obtained at : `st-device-sdk-c-ref/bsp/esp32/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py`.\
  The key would be created in `./keys/encryption_keys.bin path`
- Create and encrypt the custom nvs bin file:

  ```sh
  $ python nvs_partition_gen.py encrypt sample_nvs_data.csv encrypted_custom_nvs.bin 0x3000 --inputkey keys/encryption_keys.bin
  ```

- Flash Partition table:

  ```sh
  $ idf.py partition_table partition_table-flash
  ```

- Flash Encrypted custom nvs partition:
  Use the appropriate port & offset for custom nvs partition for flashing.

  ```sh
  $ esptool.py -p /dev/ttyUSB0 --before default_reset --after no_reset write_flash 0x121000 encrypted_custom_nvs.bin
  ```

- Flash NVS key partition:
  Use the appropriate port & offset for nvs key partition for flashing.

  ```sh
  $ esptool.py -p /dev/ttyUSB0 --before default_reset --after no_reset write_flash --encrypt 0x120000 keys/encryption_keys.bin
  ```

> **Note :**
>
> The offsets used above to flash nvs key and custom nvs bin files can be obtained after build with:\
> $ gen_esp32part.py build/partition_table/partition-table.bin

### Build and flash the application

Use the appropriate port for flashing to ESP32.
- First time:

  ```sh
  $ idf.py -p /dev/ttyUSB0 flash monitor
  ```

- Subsequent flash:

  ```sh
  $ idf.py -p /dev/ttyUSB0 encrypted-flash monitor
  ```

### Output

NVS encrypted: When flash encryption is enabled and custom nvs is encrypted using nvs keys.

```sh
Example to check Flash Encryption status
This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 0, 2MB external flash
FLASH_CRYPT_CNT eFuse value is 1
Flash encryption feature is enabled in DEVELOPMENT mode
Erasing partition "storage" (0x1000 bytes)
Writing data with esp_partition_write:
I (525) example: 0x3fc8fd70   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (525) example: 0x3fc8fd80   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with esp_partition_read:
I (535) example: 0x3fc8fd90   00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f  |................|
I (545) example: 0x3fc8fda0   10 11 12 13 14 15 16 17  18 19 1a 1b 1c 1d 1e 1f  |................|
Reading with spi_flash_read:
I (555) example: 0x3fc8fd90   3d ab 17 04 b6 30 c0 c6  c7 af 8a 47 00 a7 19 58  |=....0.....G...X|
I (565) example: 0x3fc8fda0   8a 81 c8 da e9 83 9f 32  bc 20 b9 ad 01 0f d8 ca  |.......2. ......|
I (575) nvs: NVS partition "nvs" is encrypted.
I (585) example: NVS partition "custom_nvs" is encrypted.
I (595) example: Write - custom nvs partition: custom_nvs, key: u16_key, value: 1000
I (595) example: Read  - custom nvs partition: custom_nvs, key: u16_key, value: 1000
I (605) example: Read  - custom nvs partition: custom_nvs, key: str_key, value: "Lorem ipsum dolor sit amet", len: 29
```

## Disabling Flash Encryption

If flash encryption was enabled, flashing of plaintext data will soft-brick the ESP32. The device will reboot continuously, printing the error `flash read err, 1000` or `invalid header: 0xXXXXXX`.

For flash encryption in Development mode, encryption can be disabled by burning the `FLASH_CRYPT_CNT` eFuse. It can be done only **three** times per chip by taking the following steps:
- Uncheck Enable flash encryption on boot option:

  ```sh
  $ idf.py menuconfig
  ```

  Under `Security features`, uncheck `Enable flash encryption on boot` option.\
  **Double-check** that you have disabled this option! If this option is left enabled, the bootloader will immediately re-enable encryption when it boots.
- With flash encryption disabled, build and flash the new bootloader and application by running:

  ```sh
  $ idf.py flash
  ```

- Use `espefuse.py` (in `st-device-sdk-c-ref/bsp/esp32/components/esptool_py/esptool`) to disable the `FLASH_CRYPT_CNT` on chip by running:

  ```sh
  $ espefuse.py burn_efuse FLASH_CRYPT_CNT
  ```

- Reset the ESP32 chip. Flash encryption will be disabled, and the bootloader will boot as usual.

## References

- [ESP NVS Encryption](https://docs.espressif.com/projects/esp-idf/en/v4.3/esp32/api-reference/storage/nvs_flash.html#nvs-encryption)
- [ESP Flash Encryption](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/security/flash-encryption.html)
