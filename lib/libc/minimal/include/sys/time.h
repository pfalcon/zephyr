/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#ifndef ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SYS_TIME_H_
#define ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SYS_TIME_H_

struct timeval;
struct timezone;

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif /* ZEPHYR_LIB_LIBC_MINIMAL_INCLUDE_SYS_TIME_H_ */
