#ifndef BOT_H
#define BOT_H

#include "common.h"
#include "byte_stream.h"
#include "bot_api.h"
#include "player.h"
#include "connection.h"

typedef struct {
	Connection *main_conn;
	Connection *game_conn;
	BotApi *api;
	Player *player;
} Bot;

Bot *Bot_new(int);
void Bot_start(Bot *);
#define Bot_dispose(self)						\
	do {										\
		Connection_dispose(self->main_conn);	\
		Connection_dispose(self->game_conn);	\
		free(self);								\
		self = NULL;							\
	} while (0);

#endif
