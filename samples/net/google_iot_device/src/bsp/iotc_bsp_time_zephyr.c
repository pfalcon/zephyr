/* Copyright 2019 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iotc_bsp_time.h>

#include <stddef.h>
#include <sys/time.h>

void iotc_bsp_time_init() { /* empty */
}

iotc_time_t iotc_bsp_time_getcurrenttime_seconds() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  return (iotc_time_t)((current_time.tv_sec) +
                       (current_time.tv_usec + 500000) /
                           1000000); /* round the microseconds to seconds */
}

iotc_time_t iotc_bsp_time_getcurrenttime_milliseconds() {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);
  return (iotc_time_t)((current_time.tv_sec * 1000) +
                       (current_time.tv_usec + 500) /
                           1000); /* round the microseconds to milliseconds */
}
