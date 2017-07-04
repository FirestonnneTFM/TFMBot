#ifndef MAP_H
#define MAP_H

#include "common.h"

struct Map {
	int xml_len;
	char *xml;
};

struct Map *Map_new(void);
void Map_load(struct Map *, byte *, int);

#endif
