/*
 * Copyright (c) 2018 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/types.h>

#ifdef __ZEPHYR__
#include <net/socket.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#endif

#include <net/zstream.h>

static ssize_t zstream_sock_read(zstream stream, void *buf, size_t size)
{
	struct zstream_sock *self = (struct zstream_sock *)stream;
	return recv(self->fd, buf, size, 0);
}

static ssize_t zstream_sock_write(zstream stream, const void *buf, size_t size)
{
	struct zstream_sock *self = (struct zstream_sock *)stream;
	return send(self->fd, buf, size, 0);
}

static int zstream_sock_flush(zstream stream)
{
	return 0;
}

static int zstream_sock_close(zstream stream)
{
	struct zstream_sock *self = (struct zstream_sock *)stream;
	return close(self->fd);
}

static const struct zstream_api zstream_sock_api = {
	.read = zstream_sock_read,
	.write = zstream_sock_write,
	.flush = zstream_sock_flush,
	.close = zstream_sock_close,
};

int zstream_sock_init(struct zstream_sock *p, int fd)
{
	p->api = &zstream_sock_api;
	p->fd = fd;
	return 0;
}

struct zstream_sock *zstream_sock_alloc()
{
	// TODO
	return NULL;
}
