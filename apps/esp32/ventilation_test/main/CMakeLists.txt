idf_component_register(SRCS "main.c"
                            "device_control.c"
                            "ventilation_sensor_stub.c"
                            "iot_cli_cmd.c"
                            "iot_uart_cli.c"
                            "caps_switch.c"
                            "caps_mode.c"
                            "caps_fanSpeed.c"
                            "caps_filterState.c"
                    EMBED_FILES "device_info.json"
                                "onboarding_config.json"
                    )

set(STDK_IOT_CORE_USE_DEFINED_CONFIG "y")

add_subdirectory($ENV{STDK_CORE_PATH} iotcore)
target_link_libraries(${COMPONENT_LIB} PUBLIC iotcore)
