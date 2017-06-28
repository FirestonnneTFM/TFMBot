#include "room.h"

void Room_add_player(struct Room *self, struct Player *player)
{
	// check for duplicates in the player list
	struct Player *i;
	for (i = self->players; i != NULL; i = i->next) {
		if (player->id == i->id) {
			if (player != i)
				Player_dispose(player);
			return;
		}
	}
	// Add the player to the linked list
	player->next = self->players;
	self->players = player;
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

struct Player *Room_get_player(struct Room *self, uint32_t player_id)
{
	struct Player *i;
	for (i = self->players; i != NULL; i = i->next) {
		if (i->id == player_id)
			return i;
	}
	return NULL;
}
