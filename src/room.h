#ifndef ROOM_H
#define ROOM_H

#include "common.h"
#include "player.h"

struct Room {
	char *name;
	byte map_pcode;
	char *map_author;
	uint16_t player_count;
	uint32_t map_code;
	struct Player *players;
};

#define Room_new()((struct Room*)calloc(1, sizeof(struct Room)))
void Room_add_player(struct Room *, struct Player *);
void Room_dispose_player(struct Room *, uint32_t);

#endif
