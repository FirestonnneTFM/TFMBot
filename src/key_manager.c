#include "key_manager.h"

static void key_cycle(uint32_t *buf, const char *hash, size_t hash_len)
{
	// This is a modified djb hash function to get a `unique` number
	// from a string.  This function also factors in the unique keys
	// that are recompiled into the client periodically.

	// important !!  these are signed shifts, so we cannot use an
	// unsigned number for n
	int32_t n = 5381;
	int i;
	for (i = 0; i < 20; i++) {
		n = (n << 5) + n + Hash_Key[i] + hash[i % hash_len];
	}
	// using the hash (n) as a seed, a 20 * 32 bit key is derived
	for (i = 0; i < 20; i++) {
		n ^= n << 13;
		n ^= n >> 17;
		n ^= n << 5;
		buf[i] = n;
	}
}

// this is from a file
// this is little endian
static uint32_t read_int(FILE *f)
{
	uint32_t res = 0;
	res |= (byte)fgetc(f);
	res |= (byte)fgetc(f) << 8;
	res |= (byte)fgetc(f) << 16;
	res |= (byte)fgetc(f) << 24;
	return res;
}

void init_keys()
{
	FILE *f = fopen("keys.bin", "rb");
	if (f == NULL)
		fatal("File keys.bin was not found");
	fread(Hash_Key, sizeof(byte), 20, f);
	Login_Key = read_int(f);
	Handshake_Number = read_int(f);	
	int i;
	int to = fgetc(f);
	for (i = 0; i < to; i++) {
		Handshake_String[i] = fgetc(f);
	}
	Handshake_String[to] = '\0';
	fclose(f);
	
	key_cycle(Identification_Key, "identification", 14);
	key_cycle(Msg_Key, "msg", 3);
}
