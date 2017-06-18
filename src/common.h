#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define UNUSED(...)((void)(__VA_ARGS__))
#define one_ms_wait()(usleep(1000000))

typedef uint8_t byte;
typedef int sock_t;

void fatal(char *);

#endif
