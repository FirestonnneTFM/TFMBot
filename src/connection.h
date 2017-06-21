#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"
#include "byte_stream.h"

struct Connection {
	sock_t sock;
	byte k;
};


void Connection_send(struct Connection *, struct ByteStream *);
void Connection_open(struct Connection *, char *host, int port);

#define Connection_new()((struct Connection*)calloc(1, sizeof(struct Connection)))

#define Connection_dispose(self)				\
	do {										\
		close(self->sock);						\
		free(self);								\
		self = NULL;							\
	} while (0)									\


#endif
