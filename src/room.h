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
bool Room_add_player(struct Room *, struct Player *);
void Room_dispose_player(struct Room *, uint32_t);
struct Player *Room_get_player_id(struct Room *, uint32_t);
struct Player *Room_get_player_name(struct Room *, char *);
struct Player *Room_get_player_name_closest(struct Room *, char *);

#endif
