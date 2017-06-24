#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"
#include "byte_stream.h"

struct Connection {
	sock_t sock;
	byte k;
};

#define Connection_new()((struct Connection*)calloc(1, sizeof(struct Connection)))
void Connection_dispose(struct Connection *);
void Connection_send(struct Connection *, struct ByteStream *);
void Connection_open(struct Connection *, char *host, int port);

#endif
