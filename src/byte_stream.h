#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include "common.h"

typedef struct {
	int capacity;
	int count;
	int position;
	byte *array;
} ByteStream;

ByteStream *ByteStream_new(void);
void ByteStream_write_byte(ByteStream *, byte);
void ByteStream_write_bytes(ByteStream *, int, byte *);
void ByteStream_write_u16(ByteStream *, uint16_t);
void ByteStream_write_u32(ByteStream *, uint32_t);
void ByteStream_write_str(ByteStream *, char *);
byte ByteStream_read_byte(ByteStream *);
void ByteStream_read_bytes(ByteStream *, byte *, int);
uint16_t ByteStream_read_u16(ByteStream *);
uint32_t ByteStream_read_u32(ByteStream *);
char *ByteStream_read_str(ByteStream *);
void ByteStream_write_sock(ByteStream *, sock_t, byte);
void ByteStream_read_sock(ByteStream *, sock_t);
void ByteStream_print(ByteStream *, int);
void ByteStream_xor_cipher(ByteStream *, int);
void ByteStream_block_cipher(ByteStream *);

#define ByteStream_dispose(self)				\
	do {										\
		free(self->array);						\
		free(self);								\
		self = NULL;							\
	} while(0)

#endif
