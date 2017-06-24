#include "common.h"
#include <string.h>
#include <errno.h>

void fatal(char *msg)
{
	printf("Fatal: %s\n", msg);
	if (errno) {
		printf("errno(%d) = %s\n", errno, strerror(errno));
		exit(errno);
	} else {
		exit(-1);
	}
}
