#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <time.h>
#include "common.h"
#include "task.h"

struct Scheduler {
	struct Task *tasks;
	int num_tasks;
	struct timespec *last_time;
};

struct Scheduler *Scheduler_new(void);
void Scheduler_add(struct Scheduler *, struct Task *);
void Scheduler_tick(struct Scheduler *);

extern struct Scheduler *Main_Scheduler;

#endif
