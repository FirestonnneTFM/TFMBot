#ifndef BOT_H
#define BOT_H

#include "common.h"
#include "connection.h"
#include "byte_stream.h"
#include "bot_api.h"
#include "player.h"
#include "room.h"
#include "map.h"

#define EMOTE_DANCE 0
#define EMOTE_LAUGH 1
#define EMOTE_CRY 2
#define EMOTE_KISS 3
#define EMOTE_RAGE 4
#define EMOTE_CLAP 5
#define EMOTE_SLEEP 6
#define EMOTE_FACEPALM 7
#define EMOTE_SIT 8
#define EMOTE_CONFETTI 9
#define EMOTE_FLAG 10
#define EMOTE_HIGH_FIVE 13
#define EMOTE_RAINBOW 18
#define EMOTE_LOVE 21
#define EMOTE_RPS_REQUEST 25

struct Bot {
	bool running;
	void *api_data;
	struct Connection *main_conn;
	struct Connection *game_conn;
	struct BotApi *api;
	struct Player *player;
	struct Room *room;
	struct Map *map;
};

extern volatile int num_bots_running;
extern char *x_arg;
extern char *override_username;
extern char *override_password;
extern char *override_roomname;

struct Bot *Bot_new(int);
void Bot_dispose(struct Bot *);
void Bot_start(struct Bot *);
void Bot_send_player_coords(struct Bot *, struct Player *);
void Bot_change_room(struct Bot *, char *);
void Bot_send_chat(struct Bot *, char *);
void Bot_send_emote(struct Bot *, byte);

#endif
