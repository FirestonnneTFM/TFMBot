#include "byte_stream.h"
#include "connection.h"
#include "crypto.h"
#include <string.h>
#include <stdint.h>

struct ByteStream *ByteStream_new()
{
	struct ByteStream *self = (struct ByteStream*)malloc(sizeof(struct ByteStream));
	self->capacity = 10;
	self->count = 0;
	self->position = 0;
	self->array = (byte*)malloc(sizeof(byte) * self->capacity);
	return self;
}

void ByteStream_dispose(struct ByteStream *self)
{
	free(self->array);
	free(self);
}

void ByteStream_write_byte(struct ByteStream *self, byte value)
{
	if (self->position >= self->capacity) {
		self->capacity <<= 1;
		self->array = (byte*)realloc(self->array, self->capacity);
	}
	self->array[self->position] = value;
	self->position++;
	int diff = self->position - self->count;
	if (diff > 0) {
		self->count += diff;
	}
}

void ByteStream_write_bytes(struct ByteStream *self, int len, byte *buf)
{
	int i;
	for (i = 0; i < len; i++) {
		ByteStream_write_byte(self, buf[i]);
	}
}

void ByteStream_write_u16(struct ByteStream *self, uint16_t value)
{
	ByteStream_write_byte(self, value >> 8 & 0xff);
	ByteStream_write_byte(self, value & 0xff);
}

void ByteStream_write_u32(struct ByteStream *self, uint32_t value)
{
	ByteStream_write_byte(self, value >> 24 & 0xff);
	ByteStream_write_byte(self, value >> 16 & 0xff);
	ByteStream_write_byte(self, value >> 8 & 0xff);
	ByteStream_write_byte(self, value & 0xff);
}

byte ByteStream_read_byte(struct ByteStream *self)
{
	if (self->position >= self->count)
		fatal("Read beyond of stream");
	return self->array[self->position++];
}

void ByteStream_read_bytes(struct ByteStream *self, byte *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		buf[i] = ByteStream_read_byte(self);
	}
}

uint16_t ByteStream_read_u16(struct ByteStream *self)
{
	return (ByteStream_read_byte(self) << 8) | ByteStream_read_byte(self);
}

uint32_t ByteStream_read_u32(struct ByteStream *self)
{
	return (ByteStream_read_byte(self) << 24) | (ByteStream_read_byte(self) << 16)
		| (ByteStream_read_byte(self) << 8) | ByteStream_read_byte(self);
}

void ByteStream_write_str(struct ByteStream *self, char *buf)
{
	if (buf) {
		int len = strlen(buf);
		ByteStream_write_u16(self, len);
		ByteStream_write_bytes(self, len, (byte*)buf);
	} else {
		ByteStream_write_u16(self, 0);
	}
}

void ByteStream_write_sock(struct ByteStream *self, sock_t sock, byte k)
{
	byte header[4];
	byte header_size;
	if (self->count < 0x100) {
		header_size = 3;
		header[0] = 1;
		header[1] = (byte)self->count;
	} else {
		header_size = 4;
		header[0] = 2;
		header[1] = (byte)(self->count >> 8);
		header[2] = (byte)(self->count & 0xff);
	}
	header[header_size - 1] = k;
	sock_write(sock, header, header_size);
	sock_write(sock, self->array, self->count);
#ifdef PRINT_ALL_OUT_PACKETS
	printf("[out] ");
	ByteStream_print(self, 0);
#endif
}

char *ByteStream_read_str(struct ByteStream *self)
{
	uint16_t len = ByteStream_read_u16(self);
	char *buf = (char*)malloc(sizeof(char) * (len + 1));
	ByteStream_read_bytes(self, (byte*)buf, len);
	buf[len] = '\0';
	return buf;
}

void ByteStream_read_sock(struct ByteStream *self, sock_t sock)
{
	// Packet encoding: first byte (ll) tells how long the encoded
	// length is (1 for byte, 2 for short), followed by the actual
	// encoded length
	byte ll;
	sock_read_byte(sock, &ll);
	int len;
	byte len_byte[2];
	if (ll == 1) {
		sock_read_byte(sock, len_byte);
		len = len_byte[0];
	} else if (ll == 2) {
		sock_read_byte(sock, len_byte);
		sock_read_byte(sock, len_byte + 1);
		len = (len_byte[0] << 8) | len_byte[1];
	} else {
		fatal("Malformed packet");
		len = 0;
	}
	byte buf[len];
	sock_block_read(sock, buf, len);
	int i;
	for (i = 0; i < len; i++) {
		ByteStream_write_byte(self, buf[i]);
	}
	self->position = 0;
}

void ByteStream_print(struct ByteStream *self, int i)
{
	for (; i < self->count; i++) {
		printf("%02x ", self->array[i]);
	}
	putchar('\n');
}

void ByteStream_print_ascii(struct ByteStream *self, int i)
{
	for (; i < self->count; i++) {
		if (self->array[i] < 0x20)
			putchar('.');
		else
			putchar(self->array[i]);
	}
	putchar('\n');
}

void ByteStream_xor_cipher(struct ByteStream *self, int k)
{
	// do not mess with the CCC prefix !
	// start at 2
	int i;
	for (i = 2; i < self->count; i++) {
		k++;
		self->array[i] ^= Key_Manager->msg_key[k % 20];
	}
}

void ByteStream_block_cipher(struct ByteStream *self)
{
	if (self->count < 2)
		fatal("Block cipher attempted on empty ByteStream");
	while (self->count < 10) {
		ByteStream_write_byte(self, 0);
	}
	// subtract by 2 to ignore the CCC prefix
	int pad_amt = (self->count - 2) % 4;
	int i;
	if (pad_amt) {
		pad_amt = 4 - pad_amt;
		for (i = 0; i < pad_amt; i++) {
			ByteStream_write_byte(self, 0);
		}
	}
	// start at 2 to ignore the CCC prefix
	self->position = 2;
	int num_chunks = (self->count - 2) / 4;
	uint32_t chunks[num_chunks];
	for (i = 0; i < num_chunks; i++) {
		chunks[i] = ByteStream_read_u32(self);
	}
	// the cryto algorithm
	btea(chunks, num_chunks);
	self->count = 2;
	self->position = 2;
	ByteStream_write_u16(self, num_chunks);
	for (i = 0; i < num_chunks; i++) {
		ByteStream_write_u32(self, chunks[i]);
	}
}
