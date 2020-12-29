idf_component_register(SRCS "main.c"
                            "device_control.c"
                            "iot_cli_cmd.c"
                            "iot_uart_cli.c"
                            "ota_util.c"
                            "caps_switch.c"
                            "caps_switchLevel.c"
                            "caps_firmwareUpdate.c"
                    INCLUDE_DIRS "$ENV{STDK_CORE_PATH}/src/include/bsp"
                                 "$ENV{STDK_CORE_PATH}/src/include/os"
                                 "$ENV{STDK_CORE_PATH}/src/include/mqtt"
                                 "$ENV{STDK_CORE_PATH}/src/include/external"
                                 "$ENV{STDK_CORE_PATH}/src/port/net/mbedtls"
                    EMBED_FILES "device_info.json"
                                "onboarding_config.json"
                                "public_key.pem"
                                "root.pem"
                    )

set(STDK_IOT_CORE_USE_DEFINED_CONFIG "y")

set(STDK_LINK_LIBRARY
        __idf_libsodium
        __idf_json
   )

set(STDK_INCLUDE_PATH
        "$ENV{IDF_PATH}/components/freertos/include/freertos"
        "$ENV{IDF_PATH}/components/nvs_flash/include"
        "$ENV{IDF_PATH}/components/spi_flash/include"
        "$ENV{IDF_PATH}/components/bootloader_support/include"
    )

add_subdirectory($ENV{STDK_CORE_PATH} iotcore)
target_link_libraries(${COMPONENT_LIB} PUBLIC iotcore)
