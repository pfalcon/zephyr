# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(google_iot_device)

set(IOTC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../modules/lib/google/iot-device-sdk-embedded-c)

zephyr_include_directories(${APPLICATION_SOURCE_DIR}/src/tls_config)

zephyr_include_directories(app PRIVATE ${IOTC_PATH}/include)
zephyr_include_directories(app PRIVATE ${IOTC_PATH}/include/bsp)

zephyr_include_directories(app PRIVATE ${IOTC_PATH}/src/bsp)
zephyr_include_directories(app PRIVATE ${IOTC_PATH}/src/libiotc)
zephyr_include_directories(app PRIVATE ${IOTC_PATH}/src/libiotc/memory)
zephyr_include_directories(app PRIVATE ${IOTC_PATH}/src/libiotc/datastructures)

#  ERROR: TODO(atigyi): cert verify MUST be turned back on
add_compile_definitions(IOTC_DISABLE_CERTVERIFY)

target_sources(app PRIVATE
	src/main.c
	src/commandline.c
	src/example_utils.c
	src/bsp/iotc_bsp_tls_mbedtls.c
	src/bsp/iotc_bsp_crypto_mbedtls.c
	# TODO(atigyi): implement the BSP IO FS
	#  src/bsp/iotc_bsp_io_fs_zephyr.c
	src/bsp/iotc_bsp_io_net_zephyr.c
	src/bsp/iotc_bsp_mem_zephyr.c
	src/bsp/iotc_bsp_rng_zephyr.c
	src/bsp/iotc_bsp_time_zephyr.c
	)


