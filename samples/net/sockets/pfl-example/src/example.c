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

static char msg[] = "Lorem ipsum dolor sit amet";
static char response[sizeof(msg)] = {};

int main(void)
{
	static struct addrinfo hints;
	struct addrinfo *a;
	int r, s;

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

	r = send(s, msg, sizeof(msg), 0);
	if (r != sizeof(msg)) {
		return r;
	}

	r = recv(s, response, sizeof(response), 0);
	if (r != sizeof(msg)) {
		return r;
	}

	close(s);

	return 0;
}
