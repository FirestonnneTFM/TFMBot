#ifndef CONNECTION_H
#define CONNECTION_H

#include "common.h"
#include "byte_stream.h"

typedef struct {
	sock_t sock;
	byte k;
} Connection;


void Connection_send(Connection *, ByteStream *);
void Connection_open(Connection *, char *host, int port);

#define Connection_new()((Connection*)calloc(1, sizeof(Connection)))

#define Connection_dispose(self)				\
	do {										\
		close(self->sock);						\
		free(self);								\
		self = NULL;							\
	} while (0)									\


#endif
