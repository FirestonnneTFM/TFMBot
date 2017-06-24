#ifndef BOT_H
#define BOT_H

#include "common.h"
#include "connection.h"
#include "byte_stream.h"
#include "bot_api.h"
#include "player.h"
#include "room.h"

struct Bot {
	bool running;
	void *api_data;
	struct Connection *main_conn;
	struct Connection *game_conn;
	struct BotApi *api;
	struct Player *player;
	struct Room *room;
};

struct Bot *Bot_new(int);
void Bot_dispose(struct Bot *);
void Bot_start(struct Bot *);
void Bot_send_player_coords(struct Bot *, struct Player *);

#endif
