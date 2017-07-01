#ifndef CRYPTO_H
#define CRYPTO_H

#include "common.h"

struct KeyManager {
	byte hash_key[20];
	uint32_t login_key;
	uint32_t handshake_number;
	char *handshake_string;
	uint32_t identification_key[20];
	uint32_t msg_key[20];
};

extern struct KeyManager *Key_Manager;
void btea(uint32_t *v, size_t n);
void password_hash(char buf, size_t, FILE *);
void djb_hash(uint32_t *, const char *, size_t);
void init_keys(void);

#endif
