#ifndef BOT_API_H
#define BOT_API_H

#include "common.h"
#include "player.h"

struct Bot;

struct BotApi {
	const char *name;
	void *data;
	char *(*get_username)(struct Bot *);
	char *(*get_password)(struct Bot *);
	char *(*get_login_room)(struct Bot *);
	void (*on_connect)(struct Bot *);
	void (*on_room_join)(struct Bot *);
	void (*on_new_map)(struct Bot *);
	void (*on_player_join)(struct Bot *, struct Player *);
	void (*on_player_move)(struct Bot *, struct Player *);
	void (*on_player_death)(struct Bot *, struct Player *);
	void (*on_chat)(struct Bot *, struct Player *, char *);
};

void init_bot_api(int);
struct BotApi *get_registered_api(int);
struct BotApi *BotApi_new(const char *);
void BotApi_register(struct BotApi *);

#endif
