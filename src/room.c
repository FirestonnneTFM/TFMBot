#include "room.h"
#include <string.h>

bool Room_add_player(struct Room *self, struct Player *player)
{
	// check for duplicates in the player list
	struct Player *i;
	for (i = self->players; i != NULL; i = i->next) {
		if (player->id == i->id) {
			if (player != i)
				Player_dispose(player);
			return false;
		}
	}
	// Add the player to the linked list
	player->next = self->players;
	self->players = player;
	return true;
}

void Room_dispose_player(struct Room *self, uint32_t player_id)
{
	struct Player **i;
	for (i = &(self->players); *i != NULL; i = &((*i)->next)) {
		struct Player *player = *i;
		if (player->id == player_id) {
			*i = player->next;
			Player_dispose(player);
			return;
		}
	}
}

struct Player *Room_get_player_id(struct Room *self, uint32_t player_id)
{
	struct Player *i;
	for (i = self->players; i != NULL; i = i->next) {
		if (i->id == player_id)
			return i;
	}
	return NULL;
}

struct Player *Room_get_player_name(struct Room *self, char *player_name)
{
	struct Player *i;
	for (i = self->players; i != NULL; i = i->next) {
		if (strcmp(i->name, player_name) == 0)
			return i;
	}
	return NULL;
}

struct Player *Room_get_player_name_closest(struct Room *self, char *partial)
{
	struct Player *i;
	struct Player *ret = NULL;
	int highest = 0;
	for (i = self->players; i != NULL; i = i->next) {
		int j = 0;
		while (partial[j] && i->name[j] && partial[j] == i->name[j])
			j++;
		if (j > highest) {
			highest = j;
			ret = i;
		}
	}
	return ret;
}
