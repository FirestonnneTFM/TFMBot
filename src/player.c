#include "player.h"

struct Player *Player_new()
{
	struct Player *self = (struct Player*)calloc(1, sizeof(struct Player));
	self->round_num = 1;
	return self;
}

void Player_from_old_protocol(struct Player *self, struct ByteStream *b)
{
	if (ByteStream_read_byte(b) != 0x01)
		return;
	int start = b->position;
	int i;
	int len = 0;
	while (ByteStream_read_byte(b) != '#')
		len ++;
	free(self->name);
	self->name = (char*)malloc(sizeof(char) * (len + 1));
	self->name[len] = '\0';
	for (i = 0; i < len; i++) {
		self->name[i] = b->array[start + i];
	}
	self->id = 0;
	byte digit;
	while ((digit = ByteStream_read_byte(b)) != '#') {
		self->id *= 10;
		self->id += digit;
	}
}

