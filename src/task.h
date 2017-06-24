#ifndef TASK_H
#define TASK_H

#include "common.h"

struct Task {
	int delay;
	int wait_time;
	bool (*func)(void *);
	void *arg;
	struct Task *next;
};

struct Task *Task_new(int, bool (*func)(void *), void *);

#endif
