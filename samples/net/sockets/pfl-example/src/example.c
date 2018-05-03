
#include <net/socket.h>
#include <kernel.h>
#include <net/net_app.h>

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
