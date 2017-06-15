#include "btea.h"
#include "key_manager.h"

// Corrected Block TEA algorithm
// An unpatented block cipher that is used to encode the login information
// https://en.wikipedia.org/wiki/XXTEA

#define DELTA 0x9e3779b9
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ (( sum ^ y) + ( Identification_Key[(p & 3) ^ e] ^ z)))

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
