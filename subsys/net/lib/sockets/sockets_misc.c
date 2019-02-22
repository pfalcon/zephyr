/*
 * Copyright (c) 2019 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <net/socket.h>

int zsock_gethostname(char *buf, size_t len)
{
	const char *p = net_hostname_get();

	strncpy(buf, p, len);

	return 0;
}

#ifdef CONFIG_POSIX_HEADERS
FUNC_ALIAS(zsock_gethostname, gethostname, int);
#endif
