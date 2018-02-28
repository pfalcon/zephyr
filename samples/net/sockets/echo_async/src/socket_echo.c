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
#include <fcntl.h>
#include <poll.h>

#else

#include <sys/fcntl.h>
#include <net/socket.h>
#include <kernel.h>
#include <net/net_app.h>

#endif

#include <net/tls_conf.h>
#include <net/zstream.h>
#include <net/zstream_tls.h>

#include "../../../echo_server/src/test_certs.h"

/* For Zephyr, keep max number of fd's in sync with max poll() capacity */
#ifdef CONFIG_NET_SOCKETS_POLL_MAX
#define NUM_FDS CONFIG_NET_SOCKETS_POLL_MAX
#else
#define NUM_FDS 5
#endif

#define NUM_LISTEN_FDS 2

#define PORT 4242

/* Number of simultaneous client connections will be NUM_FDS be minus 2 */
struct pollfd pollfds[NUM_FDS];
struct zstream_sock streams_sock[NUM_FDS];
struct zstream_tls streams_tls[NUM_FDS];
int pollnum;
static mbedtls_ssl_config *tls_conf;
static struct ztls_cert_key_pair cert_key;

static void nonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

static void block(int fd)
{
	int fl = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, fl & ~O_NONBLOCK);
}

int register_sock(int fd)
{
	int i;
	if (pollnum < NUM_FDS) {
		i = pollnum++;
	} else {
		for (i = 0; i < NUM_FDS; i++) {
			if (pollfds[i].fd < 0) {
				goto found;
			}
		}

		return -1;
	}

found:
	/* Don't create streams for listening sockets */
	if (i >= NUM_LISTEN_FDS) {
		zstream stream;
		zstream_sock_init(&streams_sock[i], fd);
		stream = (zstream)&streams_sock[i];

		if (zstream_tls_init2(&streams_tls[i], stream, tls_conf, NULL) < 0) {
			printf("Error creating TLS connection\n");
			return -1;
		}
	}

	pollfds[i].fd = fd;
	pollfds[i].events = POLLIN;

	return 0;
}

void unregister_sock(int idx)
{
	zstream_close((zstream)&streams_tls[idx]);
	pollfds[idx].fd = -1;
}

int main(void)
{
	int res;
	static int counter;
	int serv4, serv6;
	struct sockaddr_in bind_addr4 = {
		.sin_family = AF_INET,
		.sin_port = htons(PORT),
		.sin_addr = {
			.s_addr = htonl(INADDR_ANY),
		},
	};
	struct sockaddr_in6 bind_addr6 = {
		.sin6_family = AF_INET6,
		.sin6_port = htons(PORT),
		.sin6_addr = IN6ADDR_ANY_INIT,
	};

	if (ztls_get_tls_server_conf(&tls_conf) < 0) {
		printf("Unable to initialize TLS\n");
		return 1;
	}

	res = ztls_parse_cert_key_pair(&cert_key,
				       rsa_example_cert_der,
				       rsa_example_cert_der_len,
				       rsa_example_keypair_der,
				       rsa_example_keypair_der_len);
	if (res < 0) {
		printf("Unable to parse cert/privkey\n");
		return 1;
	}

	res = ztls_conf_add_own_cert_key_pair(tls_conf, &cert_key);
	if (res < 0) {
		printf("Unable to set cert/privkey\n");
		return 1;
	}

	serv4 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	res = bind(serv4, (struct sockaddr *)&bind_addr4, sizeof(bind_addr4));
	if (res == -1) {
		printf("Cannot bind IPv4, errno: %d\n", errno);
	}

	serv6 = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	#ifdef IPV6_V6ONLY
	/* For Linux, we need to make socket IPv6-only to bind it to the
	 * same port as IPv4 socket above.
	 */
	int TRUE = 1;
	setsockopt(serv6, IPPROTO_IPV6, IPV6_V6ONLY, &TRUE, sizeof(TRUE));
	#endif
	res = bind(serv6, (struct sockaddr *)&bind_addr6, sizeof(bind_addr6));
	if (res == -1) {
		printf("Cannot bind IPv6, errno: %d\n", errno);
	}

	nonblock(serv4);
	nonblock(serv6);
	listen(serv4, 5);
	listen(serv6, 5);

	register_sock(serv4);
	register_sock(serv6);

	printf("Asynchronous TCP echo server waits for connections on port %d...\n", PORT);

	while (1) {
		struct sockaddr_storage client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		char addr_str[32];

		res = poll(pollfds, pollnum, -1);
		if (res == -1) {
			printf("poll error: %d\n", errno);
			continue;
		}

		for (int i = 0; i < pollnum; i++) {
			if (!(pollfds[i].revents & POLLIN)) {
				continue;
			}
			int fd = pollfds[i].fd;
			if (i < NUM_LISTEN_FDS) {
				/* If server socket */
				int client = accept(fd, (struct sockaddr *)&client_addr,
						    &client_addr_len);
				void *addr = &((struct sockaddr_in *)&client_addr)->sin_addr;

				inet_ntop(client_addr.ss_family, addr,
					  addr_str, sizeof(addr_str));
				printf("Connection #%d from %s fd=%d\n", counter++,
				       addr_str, client);
				if (register_sock(client) < 0) {
					close(client);
				} else {
					nonblock(client);
				}
			} else {
				char buf[128];
				int len = zstream_read((zstream)&streams_tls[i], buf, sizeof(buf));
				if (len <= 0) {
					if (len < 0) {
						if (errno == EAGAIN) {
							/* Underlying socket could be readable, but stream still have EAGAIN */
							continue;
						}
						printf("error: recv: %d\n", errno);
					}
error:
					unregister_sock(i);
					printf("Connection fd=%d closed\n", fd);
				} else {
					int out_len;
					const char *p;
					/* We implement semi-async server,
					 * where reads are async, but writes
					 * *can* be sync (blocking). Note that
					 * in majority of cases they expected
					 * to not block, but to be robust, we
					 * handle all possibilities.
					 */
					block(fd);
					for (p = buf; len; len -= out_len) {
						out_len = zstream_write((zstream)&streams_tls[i], p, len);
						if (out_len < 0) {
							printf("error: "
							       "send: %d\n",
							       errno);
							goto error;
						}
						p += out_len;
					}
					nonblock(fd);
				}
			}
		}
	}
}
