#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include "common.h"

struct ByteStream {
	int capacity;
	int count;
	int position;
	byte *array;
};

struct ByteStream *ByteStream_new(void);
void ByteStream_dispose(struct ByteStream *);
void ByteStream_write_byte(struct ByteStream *, byte);
void ByteStream_write_bytes(struct ByteStream *, int, byte *);
void ByteStream_write_u16(struct ByteStream *, uint16_t);
void ByteStream_write_u32(struct ByteStream *, uint32_t);
void ByteStream_write_str(struct ByteStream *, char *);
byte ByteStream_read_byte(struct ByteStream *);
void ByteStream_read_bytes(struct ByteStream *, byte *, int);
uint16_t ByteStream_read_u16(struct ByteStream *);
uint32_t ByteStream_read_u32(struct ByteStream *);
char *ByteStream_read_str(struct ByteStream *);
void ByteStream_write_sock(struct ByteStream *, sock_t, byte);
void ByteStream_read_sock(struct ByteStream *, sock_t);
void ByteStream_print(struct ByteStream *, int);
void ByteStream_xor_cipher(struct ByteStream *, int);
void ByteStream_block_cipher(struct ByteStream *);

#endif
