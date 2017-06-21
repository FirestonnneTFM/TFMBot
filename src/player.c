#include "player.h"
#include <string.h>

struct Player *Player_new()
{
	return (struct Player*)calloc(1, sizeof(struct Player));
}

