#include "common.h"
#include <string.h>
#include <errno.h>

void print_errno(bool clear)
{
	fprintf(stderr, "errno(%d); %s\n", errno, strerror(errno));
	if (clear)
		errno = 0;
}

void fatal(const char *msg)
{
	fprintf(stderr, "Fatal: %s\n", msg);
	if (errno) {
		print_errno(false);
		exit(errno);
	} else {
		exit(-1);
	}
}

void warning(const char *msg)
{
	fprintf(stderr, "Warning: %s\n", msg);
}
