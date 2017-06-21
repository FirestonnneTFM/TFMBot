#ifndef BOT_H
#define BOT_H

#include "common.h"
#include "connection.h"
#include "byte_stream.h"
#include "bot_api.h"
#include "player.h"
#include "room.h"

struct Bot {
	struct Connection *main_conn;
	struct Connection *game_conn;
	struct BotApi *api;
	struct Player *player;
	struct Room *room;
};

struct Bot *Bot_new(int);
void Bot_start(struct Bot *);
#define Bot_dispose(self)						\
	do {										\
		Connection_dispose(self->main_conn);	\
		Connection_dispose(self->game_conn);	\
		free(self);								\
		self = NULL;							\
	} while (0);

#endif
