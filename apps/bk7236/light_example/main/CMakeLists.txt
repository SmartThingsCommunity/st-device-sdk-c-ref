armino_component_register(SRCS "main.c"
                            "device_control.c"
                            "caps_activityLightingMode.c"
                            "caps_colorTemperature.c"
                            "caps_dustSensor.c"
                            "caps_switch.c"
                            "caps_switchLevel.c"
                    EMBED_FILES "device_info.json"
                                "onboarding_config.json"
                    )

set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../")
set(STDK_IOT_CORE_USE_DEFINED_CONFIG "y")
set(STDK_CORE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../iot-core")
add_subdirectory(${STDK_CORE_PATH} iotcore)
#add_subdirectory($ENV{STDK_CORE_PATH} iotcore)
target_link_libraries(${COMPONENT_LIB} PUBLIC iotcore)
