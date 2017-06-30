#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"
#include "byte_stream.h"

struct Player {
	uint32_t id;
	char *name;
	uint16_t title;
	uint16_t x;
	uint16_t y;
	uint16_t acc_x;
	uint16_t acc_y;
	byte key_right;
	byte key_left;
	byte jumping;
	byte animation_frame;
	uint32_t round_num;
	struct Player *next;
};

struct Player *Player_new(void);
bool Player_from_old_protocol(struct Player *, struct ByteStream *);
void Player_copy(struct Player *, struct Player *);
void Player_dispose(struct Player *);

#endif
