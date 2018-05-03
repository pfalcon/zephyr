#include <stdio.h>

#ifndef __ZEPHYR__

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#define CONFIG_NET_APP_PEER_IPV4_ADDR "0.0.0.0"

#else

#include <net/socket.h>
#include <kernel.h>
#include <net/net_app.h>

#endif

#include <net/tls_conf.h>
#include <net/zstream.h>
#include <net/zstream_tls.h>

static char msg[] = "Lorem ipsum dolor sit amet";
static char response[sizeof(msg)] = {};

int main(void)
{
	static struct addrinfo hints;
	struct addrinfo *a;
	int r, s;

	struct zstream_sock stream_sock;
	struct zstream_tls stream_tls;
	struct zstream *stream;
	mbedtls_ssl_config *tls_conf;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	r = getaddrinfo(CONFIG_NET_APP_PEER_IPV4_ADDR, "4242", &hints, &a);
	if (r != 0) {
		return r;
	}

	s = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
	if (s < 0) {
		return s;
	}

	r = connect(s, a->ai_addr, a->ai_addrlen);
	if (r < 0) {
		return r;
	}

	zstream_sock_init(&stream_sock, s);
	stream = (struct zstream *)&stream_sock;

	if (ztls_get_tls_client_conf(&tls_conf) < 0) {
		printf("Unable to initialize TLS config\n");
		return 1;
	}
	mbedtls_ssl_conf_authmode(tls_conf, MBEDTLS_SSL_VERIFY_NONE);

	r = zstream_tls_init(&stream_tls, stream, tls_conf, CONFIG_NET_APP_PEER_IPV4_ADDR);
	if (r < 0) {
		printf("Unable to initialize TLS\n");
		return 1;
	}
	stream = (struct zstream *)&stream_tls;

	r = zstream_write(stream, msg, sizeof(msg));
	if (r != sizeof(msg)) {
		return r;
	}

	r = zstream_read(stream, response, sizeof(response));
	if (r != sizeof(msg)) {
		return r;
	}

	close(s);

	printf("success\n");

	return 0;
}
