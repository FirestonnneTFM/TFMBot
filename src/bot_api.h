#ifndef BOT_API_H
#define BOT_API_H

#include "common.h"
#include "player.h"

struct Bot;

struct BotApi {
	char *name;
	char *(*get_username)(struct Bot *);
	char *(*get_password)(struct Bot *);
	char *(*get_login_room)(struct Bot *);
	void (*on_connect)(struct Bot *);
	void (*on_room_join)(struct Bot *);
	void (*on_new_map)(struct Bot *);
	void (*on_player_join)(struct Bot *, struct Player *);
	void (*on_player_move)(struct Bot *, struct Player *);
	void (*on_player_death)(struct Bot *, struct Player *);
	void (*on_player_chat)(struct Bot *, struct Player *, char *);
	void (*on_dispose)(struct Bot *);
};

void init_bot_api(int);
struct BotApi *get_registered_api(int);
struct BotApi *BotApi_new(char *);
void BotApi_dispose(struct BotApi *);
void BotApi_register(struct BotApi *);

#endif
