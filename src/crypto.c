#include "crypto.h"

struct KeyManager *Key_Manager = NULL;

void djb_hash(uint32_t *buf, const char *hash, size_t hash_len)
{
	if (Key_Manager == NULL)
		fatal("Call to hash function without initializing keys");
	if (hash_len == 0)
		fatal("String to hash was too short");
	// This is a modified djb hash function to get a `unique` number
	// from a string.  This function also factors in the unique keys
	// that are recompiled into the client periodically.

	// important !!  these are signed shifts, so we cannot use an
	// unsigned number for n
	int32_t n = 5381;
	int i;
	for (i = 0; i < 20; i++) {
		n = (n << 5) + n + Key_Manager->hash_key[i] + hash[i % hash_len];
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
		fatal("File keys.bin was not able to be opened");
	Key_Manager = (struct KeyManager*)malloc(sizeof(struct KeyManager));
	if (fread(Key_Manager->hash_key, sizeof(byte), 20, f) != 20)
		fatal("keys.bin read failed");
	Key_Manager->login_key = read_int(f);
	Key_Manager->handshake_number = read_int(f);
	int i;
	int to = fgetc(f);
	Key_Manager->handshake_string = (char*)malloc(sizeof(char) * (to + 1));
	Key_Manager->handshake_string[to] = '\0';
	for (i = 0; i < to; i++) {
		Key_Manager->handshake_string[i] = fgetc(f);
	}
	fclose(f);

	djb_hash(Key_Manager->identification_key, "identification", 14);
	djb_hash(Key_Manager->msg_key, "msg", 3);
}

// Corrected Block TEA algorithm
// An unpatented block cipher that is used to encode the login information
// https://en.wikipedia.org/wiki/XXTEA

#define DELTA 0x9e3779b9
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ (( sum ^ y) + ( Key_Manager->identification_key[(p & 3) ^ e] ^ z)))

void btea(uint32_t *v, size_t n)
{
	uint32_t y, z, sum;
	unsigned int p, rounds, e;
	rounds = 6 + 52/n;
	sum = 0;
	z = v[n-1];
	do {
		sum += DELTA;
		e = (sum >> 2) & 3;
		for (p = 0; p < n - 1; p++) {
			y = v[ p + 1]; 
			z = v[p] += MX;
		}
		y = v[0];
		z = v[n - 1] += MX;
	} while (--rounds);
}

void password_hash(char buf, size_t len, FILE *f)
{
	static const char salt[32] = { -9, 26, -90, -34, -113, 23, 118, -88, 3, -99, 50,
		-72, -95, 86, -78, -87, 62, -99, -59, -35, -50, 86, -45, -73,-92,
		5, 74, 13, 8, -80 };
	UNUSED(salt);
	UNUSED(buf);
	UNUSED(len);
	UNUSED(f);
	// TODO
	// appened the salt to the end of the string
	// send it through a SHA256
	// base 64 encode it as a string
}
