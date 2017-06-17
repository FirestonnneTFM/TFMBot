#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "connection.h"

void Connection_send(Connection *self, ByteStream *b)
{
	ByteStream_write_sock(b, self->sock, self->k);
	self->k = (self->k + 1) % 100;
}

static uint32_t pack_host(char *host)
{
	uint32_t res = 0;
	int i;
	for (i = 0; i < 4; i++)
		res |=  (unsigned)(host[i] & 0xff) << ((3 - i) * 8);
	return htonl(res);
}

static sock_t open_sock(char *host_name, int port)
{
	sock_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		fatal("Socket can't be opened");
	struct hostent *host = gethostbyname(host_name);
	if (host == NULL)
		fatal("Host could not be resolved");
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = pack_host(host->h_addr_list[0]);
	if (connect(sock, (struct sockaddr*)(&server_addr), sizeof(server_addr)))
		fatal("Could not connect");
	return sock;
}

void Connection_open(Connection *self, char *host, int port)
{
	self->sock = open_sock(host, port);
}