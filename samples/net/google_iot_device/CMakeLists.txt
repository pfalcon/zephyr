cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(google_iot_device)

set(IOTC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../modules/lib/google/iot-device-sdk-embedded-c)

target_include_directories(app PRIVATE ${IOTC_PATH}/include)

zephyr_include_directories(${APPLICATION_SOURCE_DIR}/src/tls_config)

target_sources(app PRIVATE
	src/main.c
	src/commandline.c
	src/example_utils.c)

