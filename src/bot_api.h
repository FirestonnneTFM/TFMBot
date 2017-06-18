#ifndef BOT_API_H
#define BOT_API_H

#include "common.h"
#include "player.h"

typedef struct _BotApi {
	const char *name;
	void *data;
	char *(*get_username)(struct _BotApi *);
	char *(*get_password)(struct _BotApi *);
	char *(*get_login_room)(struct _BotApi *);
	void (*on_connect)(struct _BotApi *, Player *);
	void (*on_room_join)(struct _BotApi *, char *);
	void (*on_new_map)(struct _BotApi *, uint32_t, char *, byte, uint16_t);
	void (*on_player_join)(struct _BotApi *, Player *);
	void (*on_player_move)(struct _BotApi *, Player *);
	void (*on_player_death)(struct _BotApi *, Player *);
	void (*on_chat)(struct _BotApi *, Player *, char *);
} BotApi;

void init_bot_api(int);
BotApi *get_registered_api(int);
BotApi *BotApi_new(const char *);
void BotApi_register(BotApi *);

#endif
