#ifndef BOT_H
#define BOT_H

#include "common.h"
#include "byte_stream.h"

typedef struct {
	sock_t sock;
	byte key_offset;
} Bot;

Bot *Bot_new(void);
void Bot_start(Bot *);
void Bot_send(Bot *, ByteStream *, byte *);
#define Bot_dispose(self)						\
	do {										\
	close(self->sock);							\
	free(self);									\
	self = NULL;								\
	} while (0);

#endif
