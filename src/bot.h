#ifndef BOT_H
#define BOT_H

#include "common.h"
#include "connection.h"
#include "byte_stream.h"
#include "bot_api.h"
#include "player.h"
#include "room.h"
#include "map.h"

struct Bot {
	bool running;
	void *api_data;
	struct Connection *main_conn;
	struct Connection *bulle_conn;
	uint32_t cp_index;
	struct BotApi *api;
	struct Player *player;
	struct Room *room;
	struct Map *map;
};

extern volatile int num_bots_running;
extern struct Bot **bots_running;
extern char *x_arg;
extern byte login_mode;
extern char *override_username;
extern char *override_password;
extern char *override_roomname;

struct Bot *Bot_new(int);
void Bot_dispose(struct Bot *);
void Bot_start(struct Bot *);
void Bot_send_player_coords(struct Bot *, struct Player *);
void Bot_change_room(struct Bot *, char *);
void Bot_send_chat(struct Bot *, char *);
void Bot_send_command(struct Bot *, char *);
void Bot_send_emote(struct Bot *, byte);
void Bot_join_cp_chat(struct Bot *, char *);
void Bot_send_cp_chat(struct Bot *, char *, char *);
void Bot_do_register(struct Bot *self, char *);

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

#define CCC_OLD_PROTOCOL 0x0101
#define CCC_CP_PROTOCOL 0x3c03
#define CCC_HEARTBEAT 0x1A1A
#define CCC_HANDSHAKE 0x1C01
#define CCC_HANDSHAKE_OK 0x1A03
#define CCC_CAPTCHA 0x1A14
#define CCC_REGISTER 0x1A07
#define CCC_LOGIN 0x1A08
#define CCC_LOGIN_OK 0x1A02
#define CCC_SWITCH_BULLE 0x2C01
#define CCC_SET_COMMUNITY 0x0802
#define CCC_OS_INFO 0x1C11
#define CCC_SET_KEY_OFFSET 0x2C16
#define CCC_WHICH_GAME 0x0701
#define CCC_ROOM_JOIN 0x0515
#define CCC_NEW_MAP 0x0502
#define CCC_PLAYER_POSITION 0x0404
#define CCC_PLAYER_DUCK 0x0409
#define CCC_PLAYER_CHAT 0x0606
#define CCC_PLAYER_CHAT_COMMAND 0x061a
#define CCC_PLAYER_EMOTE 0x0801
#define CCC_SERVER_TEXT 0x1c05
#define CCC_TIME_VALUE 0x0614

#define OLD_CCC_LIST_OF_PLAYERS 0x0809
#define OLD_CCC_NEW_PLAYER 0x0808
#define OLD_CCC_PLAYER_DIE 0x0805
#define OLD_CCC_PLAYER_LEAVE 0x0807
#define OLD_CCC_NEW_SHAMAN 0x0815
#define OLD_CCC_BANNED 0x1a12

#define CP_CCC_CONNECT 0x0003
#define CP_CCC_JOIN_CHAT 0x0036
#define CP_CCC_JOIN_CHAT_OK 0x003E
#define CP_CCC_CHAT_SEND 0x0030
#define CP_CCC_CHAT_SEND_STATUS 0x0031
#define CP_CCC_CHAT_RECV 0x0040


#endif
