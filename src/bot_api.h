#ifndef BOT_API_H
#define BOT_API_H

#include "common.h"
#include "player.h"

typedef struct {
	const char *name;
	void *data;
	char *(*get_username)(void);
	char *(*get_password)(void);
	void (*on_connect)(Player *);
	void (*on_player_join)(Player *);
	void (*on_player_move)(Player *);
	void (*on_player_death)(Player *);
	void (*on_chat)(Player *, char *);
} BotApi;

void init_bot_api(int);
BotApi *get_registered_api(int);
BotApi *BotApi_new(const char *);
void BotApi_register(BotApi *);

#endif
