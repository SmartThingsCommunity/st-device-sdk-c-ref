idf_component_register(SRCS "smart_switch.c"
                            "device_control.c"
                    EMBED_FILES "device_info.json"
                                "onboarding_config.json"
                    )

set(stdk_config ${CMAKE_CURRENT_SOURCE_DIR}/../stdk_config.cmake)

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
