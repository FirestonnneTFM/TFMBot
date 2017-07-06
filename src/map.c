#include "map.h"
#include <zlib.h>
#include <string.h>

struct Map *Map_new()
{
	return (struct Map*)calloc(1, sizeof(struct Map));
}

#define XML_BUF_SIZE 4096

void Map_load(struct Map *self, byte *buf, int len)
{
	char *xml = (char*)malloc(sizeof(char) * XML_BUF_SIZE);
	
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = (uInt)len;
	stream.next_in = (Bytef*)buf;
	stream.avail_out = XML_BUF_SIZE;
	stream.next_out = (Bytef*)xml;

	inflateInit(&stream);
	inflate(&stream, Z_NO_FLUSH);
	inflateEnd(&stream);

	if (stream.total_out == XML_BUF_SIZE)
		puts("Warning : map xml was too large for buffer");

	free(self->xml);
	self->xml_len = stream.total_out;
	self->xml = (char*)malloc(sizeof(char) * (self->xml_len + 1));
	self->xml[self->xml_len] = '\0';
	memcpy(self->xml, xml, self->xml_len);
	free(xml);
}
