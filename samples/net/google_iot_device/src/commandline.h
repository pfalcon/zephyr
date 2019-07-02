/* Copyright 2019 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * This module implements a command line argument parser.
 */

#include <iotc_mqtt.h>

/* Flags set by commandline arguments. */
extern iotc_mqtt_qos_t iotc_example_qos;

/* Parameters returned by the parser. These will be in a structure someday. */
extern const char* iotc_project_id;
extern const char* iotc_device_path;
extern const char* iotc_publish_topic;
extern const char* iotc_publish_message;
extern const char* iotc_private_key_filename;

int iotc_parse(int argc, char** argv, char* valid_options,
               const unsigned options_length);
