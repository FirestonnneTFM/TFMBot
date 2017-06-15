#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

typedef uint8_t byte;
typedef int sock_t;

sock_t open_sock(char *, int);
void fatal(char *);

#endif
