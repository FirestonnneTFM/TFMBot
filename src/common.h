#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

//#define PRINT_ALL_OUT_PACKETS
//#define PRINT_ALL_IN_PACKETS
//#define PRINT_ALL_UNHANDLED_PACKETS

#define UNUSED(x)((void)(x))
#define sleep_ms(x)(usleep(1000 * x))

typedef uint8_t byte;
typedef int sock_t;

void print_errno(bool);
void fatal(const char *);
void warning(const char *);

#endif
