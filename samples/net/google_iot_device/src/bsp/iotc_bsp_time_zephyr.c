/* Copyright 2019 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <iotc_bsp_time.h>

#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <net/sntp.h>
#include <stdio.h> //remove

void iotc_bsp_time_init() {
  struct sntp_time ts;
  int res = sntp_simple("time.nist.gov", 3000, &ts);

  if (res < 0) {
    printf("Cannot acquire current time\n");
    exit(1);
  }

  struct timespec tspec;
  tspec.tv_sec = ts.seconds;
  tspec.tv_nsec = ((u64_t)ts.fraction * (1000 * 1000 * 1000)) >> 32;
  res = clock_settime(CLOCK_REALTIME, &tspec);
  printf("clock_settime: %d, errno: %d\n", res, errno);
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
