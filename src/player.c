#include "player.h"
#include <string.h>

Player *Player_new()
{
	Player *self = (Player*)calloc(1, sizeof(Player));
	return self;
}

