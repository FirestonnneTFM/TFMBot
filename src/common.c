#include "common.h"
#include <string.h>
#include <errno.h>

void fatal(char *msg)
{
	fprintf(stderr, "Fatal: %s\n", msg);
	if (errno) {
		fprintf(stderr, "errno(%d); %s\n", errno, strerror(errno));
		exit(errno);
	} else {
		exit(-1);
	}
}
