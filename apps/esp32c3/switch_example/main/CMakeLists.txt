idf_component_register(SRCS "main.c"
                            "device_control.c"
                            "iot_cli_cmd.c"
                            "iot_uart_cli.c"
                            "caps_switch.c"
                    EMBED_FILES "device_info.json"
                                "onboarding_config.json"
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


set(CORE_PATH $ENV{STDK_CORE_PATH})
if(NOT CORE_PATH)
    message("WARNING : environment variable 'STDK_CORE_PATH' is not set.")
    set(CORE_PATH "$ENV{IDF_PATH}/../../iot-core")
endif()
add_subdirectory(${CORE_PATH} iotcore)
target_link_libraries(${COMPONENT_LIB} PUBLIC iotcore)
