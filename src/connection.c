#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "connection.h"

void Connection_send(struct Connection *self, struct ByteStream *b)
{
	ByteStream_write_sock(b, self->sock, self->k);
	self->k = (self->k + 1) % 100;
}

void Connection_dispose(struct Connection *self)
{
	close(self->sock);
	free(self);
}

static uint32_t pack_host(char *host)
{
	uint32_t res = 0;
	int i;
	for (i = 0; i < 4; i++)
		res |=  (unsigned)(host[i] & 0xff) << ((3 - i) * 8);
	return htonl(res);
}

sock_t open_sock(char *host_name, int port)
{
	sock_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		fatal("Socket can't be created");
	struct hostent *host = gethostbyname(host_name);
	if (host == NULL)
		fatal("Host could not be resolved");
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = pack_host(host->h_addr_list[0]);
	if (connect(sock, (struct sockaddr*)(&server_addr), sizeof(server_addr)))
		return 0;
	return sock;
}

void Connection_open(struct Connection *self, char *host, int port)
{
	if (self->sock)
		close(self->sock);
	self->sock = open_sock(host, port);
	if (self->sock == 0)
		fatal("Could not connect");
}

void sock_write(sock_t sock, void *buf, int len)
{
	if (write(sock, buf, len) != len)
		fatal("Sock write failed");
}

void sock_read_byte(sock_t sock, void *buf)
{
	int n;
	while ((n = read(sock, buf, 1)) != 1) {
		if (n < 0)
			fatal("Sock closed");
		// sleeping prevents the cpu from going crazy while waiting
		// for socket input
		sleep_ms(1);
	}
}

void sock_block_read(sock_t sock, void *buf, int len)
{
	if (len == 0)
		return;
	// pbuf exists to keep a pointer to the start of the fill area
	// pos is a relative count of this
	byte *pbuf = buf;
	int pos = 0;
	// loop until buffer is filled
	while (pos < len) {
		pbuf += pos;
		int n = read(sock, pbuf, len - pos);
		if (n < 0)
			fatal("Read failed");
		pos += n;
	}
}
