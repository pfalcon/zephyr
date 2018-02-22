/**
 * @file
 * @brief Network stream API definitions
 *
 * An API to abstract different transport protocols for SOCK_STREAMs, etc.
 */

/*
 * Copyright (c) 2018 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __NET_ZSTREAM_H
#define __NET_ZSTREAM_H

struct zstream_api;

struct zstream_obj {
	const struct zstream_api *api;
};

typedef struct zstream_obj *zstream;

struct zstream_api {
	ssize_t (*read)(zstream stream, void *buf, size_t size);
	ssize_t (*write)(zstream stream, const void *buf, size_t size);
	int (*flush)(zstream stream);
	int (*close)(zstream stream);
};

static inline ssize_t zstream_read(zstream stream, void *buf, size_t size)
{
	return stream->api->read(stream, buf, size);
}

static inline ssize_t zstream_write(zstream stream, const void *buf, size_t size)
{
	return stream->api->write(stream, buf, size);
}

ssize_t zstream_writeall(zstream stream, const void *buf, size_t size,
			 size_t *written);

static inline ssize_t zstream_flush(zstream stream)
{
	return stream->api->flush(stream);
}

static inline ssize_t zstream_close(zstream stream)
{
	return stream->api->close(stream);
}

/* Stream object implementation for socket. */
struct zstream_sock {
	const struct zstream_api *api;
	int fd;
};

int zstream_sock_init(struct zstream_sock *p, int fd);

#endif /* __NET_ZSTREAM_H */
