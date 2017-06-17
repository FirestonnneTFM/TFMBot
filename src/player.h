#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

typedef struct {
	uint32_t id;
	char *name;
	uint16_t title;
	uint16_t x;
	uint16_t y;
	uint16_t acc_x;
	uint16_t acc_y;
	byte key_right;
	byte key_left;
	byte key_jumping;
	byte run_animation;
	uint32_t round_num;
} Player;

Player *Player_new(void);

#endif
