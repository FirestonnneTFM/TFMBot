#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define UNUSED(x)((void)(x))
#define sleep_ms(x)(usleep(1000 * x))

typedef uint8_t byte;
typedef int sock_t;

void fatal(char *);

#endif
