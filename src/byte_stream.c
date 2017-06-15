#include "byte_stream.h"
#include "key_manager.h"
#include "btea.h"
#include <string.h>
#include <stdint.h>
#include <time.h>

ByteStream *ByteStream_new()
{
	ByteStream *self = (ByteStream*)malloc(sizeof(ByteStream));
	self->capacity = 10;
	self->count = 0;
	self->position = 0;
	self->array = (byte*)malloc(sizeof(byte) * self->capacity);
	return self;
}

void ByteStream_write_byte(ByteStream *self, byte value)
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

void ByteStream_write_bytes(ByteStream *self, int len, byte *buf)
{
	int i;
	for (i = 0; i < len; i++) {
		ByteStream_write_byte(self, buf[i]);
	}
}

void ByteStream_write_u16(ByteStream *self, uint16_t value)
{
	ByteStream_write_byte(self, value >> 8 & 0xff);
	ByteStream_write_byte(self, value & 0xff);
}

void ByteStream_write_u32(ByteStream *self, uint32_t value)
{
	ByteStream_write_byte(self, value >> 24 & 0xff);
	ByteStream_write_byte(self, value >> 16 & 0xff);
	ByteStream_write_byte(self, value >> 8 & 0xff);
	ByteStream_write_byte(self, value & 0xff);
}

byte ByteStream_read_byte(ByteStream *self)
{
	if (self->position >= self->count)
		fatal("Read beyond of stream");
	return self->array[self->position++];
}

void ByteStream_read_bytes(ByteStream *self, byte *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		buf[i] = ByteStream_read_byte(self);
	}
}

uint16_t ByteStream_read_u16(ByteStream *self)
{
	return (ByteStream_read_byte(self) << 8) | ByteStream_read_byte(self);
}

uint32_t ByteStream_read_u32(ByteStream *self)
{
	return (ByteStream_read_byte(self) << 24) | (ByteStream_read_byte(self) << 16)
		| (ByteStream_read_byte(self) << 8) | ByteStream_read_byte(self);
}

void ByteStream_write_str(ByteStream *self, char *buf)
{
	if (buf) {
		int len = strlen(buf);
		ByteStream_write_u16(self, len);
		ByteStream_write_bytes(self, len, (byte*)buf);
	} else {
		ByteStream_write_u16(self, 0);
	}
}

void ByteStream_write_sock(ByteStream *self, sock_t sock, byte k)
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
	write(sock, header, header_size);
	write(sock, self->array, self->count);
}

static void read_byte(sock_t sock, byte *buf)
{
	struct timespec sleep_time;
	// this is one ms
	sleep_time.tv_sec = 0;
	sleep_time.tv_nsec = 1000000;
	while (read(sock, buf, 1) != 1) {
		// sleeping prevents the cpu from going crazy while waiting
		// for socket input
		nanosleep(&sleep_time, NULL);
	}
}

char *ByteStream_read_str(ByteStream *self)
{
	uint16_t len = ByteStream_read_u16(self);
	char *buf = (char*)malloc(sizeof(char) * (len + 1));
	ByteStream_read_bytes(self, (byte*)buf, len);
	buf[len] = '\0';
	return buf;
}

void ByteStream_read_sock(ByteStream *self, sock_t sock)
{
	byte ll;
	read_byte(sock, &ll);
	int len;
	byte len_byte[2];
	if (ll == 1) {
		read_byte(sock, len_byte);
		len = len_byte[0];
	} else if (ll == 2) {
		read_byte(sock, len_byte);
		read_byte(sock, len_byte + 1);
		len = (len_byte[0] << 8) | len_byte[1];
	} else {
		fatal("Malformed packet");
		len = 0;
	}
	byte buf[len];
	byte *pbuf = buf;
	int pos = 0;
	while (pos < len) {
		pbuf += pos;
		pos += read(sock, pbuf, len - pos);
	}
	int i;
	for (i = 0; i < len; i++) {
		ByteStream_write_byte(self, buf[i]);
	}
	self->position = 0;
}

void ByteStream_print(ByteStream *self)
{
	int i;
	for (i = 0; i < self->count; i++) {
		printf("%02x ", self->array[i]);
	}
	putchar('\n');
}

void ByteStream_xor_cipher(ByteStream *self, int k)
{
	// do not mess with the CCC prefix !
	// start at 2
	int i;
	for (i = 2; i < self->count; i++, k++) {
		self->array[i] ^= Msg_Key[k % 20];
	}
	
}

void ByteStream_block_cipher(ByteStream *self)
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
	int num_chunks = self->count / 4;
	uint32_t chunks[num_chunks];
	for (i = 0; i < num_chunks; i++) {
		chunks[i] = ByteStream_read_u32(self);
	}
	// the cryto algorithm
	btea(chunks, num_chunks);
	self->count = 0;
	self->position = 2;
	ByteStream_write_u16(self, num_chunks);
	for (i = 0; i < num_chunks; i++) {
		ByteStream_write_u32(self, chunks[i]);
	}
}
