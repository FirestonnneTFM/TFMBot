#ifndef ROOM_H
#define ROOM_H

#include "common.h"

struct Room {
	char *name;
	byte map_pcode;
	char *map_author;
	uint16_t player_count;
	uint32_t map_code;
};

#define Room_new()((struct Room*)calloc(1, sizeof(struct Room)))

#endif
