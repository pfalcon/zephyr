/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <errno.h>

#ifndef __ZEPHYR__

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#else

#include <net/socket.h>
#include <kernel.h>
#include <net/net_app.h>

#endif

#include <net/zstream.h>
#include <net/zstream_tls.h>

#define PORT 4242

int main(void)
{
	int serv;
	struct sockaddr_in bind_addr;
	static int counter;

	serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind_addr.sin_port = htons(PORT);
	bind(serv, (struct sockaddr *)&bind_addr, sizeof(bind_addr));

	listen(serv, 5);

	printf("Single-threaded TCP echo server waits for a connection on port %d...\n", PORT);

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		char addr_str[32];
		struct zstream_sock stream_sock;
		struct zstream_tls stream_tls;
		zstream stream;
		int client = accept(serv, (struct sockaddr *)&client_addr,
				    &client_addr_len);
		inet_ntop(client_addr.sin_family, &client_addr.sin_addr,
			  addr_str, sizeof(addr_str));
		printf("Connection #%d from %s\n", counter++, addr_str);
		zstream_sock_init(&stream_sock, client);
		stream = (zstream)&stream_sock;

		if (zstream_tls_init(&stream_tls, stream, true) < 0) {
			printf("Error creating TLS connection\n");
			goto error;
		}
		stream = (zstream)&stream_tls;

		while (1) {
			char buf[128], *p;
			int len = zstream_read(stream, buf, sizeof(buf));
			int out_len;

			if (len <= 0) {
				if (len < 0) {
					printf("error: recv: %d\n", errno);
				}
				break;
			}

			p = buf;
			do {
				out_len = zstream_write(stream, p, len);
				if (out_len < 0) {
					printf("error: send: %d\n", errno);
					goto error;
				}
				p += out_len;
				len -= out_len;
			} while (len);

			if (zstream_flush(stream) < 0) {
				printf("error: flush: %d\n", errno);
				goto error;
			}
		}

error:
		zstream_close(stream);
		printf("Connection from %s closed\n", addr_str);
	}
}
