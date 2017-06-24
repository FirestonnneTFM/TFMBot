#include "scheduler.h"

struct Scheduler *Main_Scheduler = NULL;

struct Scheduler *Scheduler_new()
{
	struct Scheduler *self = (struct Scheduler*)malloc(sizeof(struct Scheduler));
	self->tasks = NULL;
	self->num_tasks = 0;
	self->last_time = (struct timespec*)malloc(sizeof(struct timespec));
	clock_gettime(CLOCK_REALTIME, self->last_time);
	return self;
}

void Scheduler_add(struct Scheduler *self, struct Task *task)
{
	task->next = self->tasks;
	self->tasks = task;
	self->num_tasks++;
}

void Scheduler_tick(struct Scheduler *self)
{
	struct timespec current_time;
	clock_gettime(CLOCK_REALTIME, &current_time);
	// difference in the two time readings in ms
	int diff = ((current_time.tv_sec - self->last_time->tv_sec) * 1000)
		+ ((current_time.tv_nsec - self->last_time->tv_nsec) / 1000000);
	struct Task **i;
	for (i = &(self->tasks); *i != NULL; i = &((*i)->next)) {
		struct Task *task = *i;
		task->wait_time -= diff;
		if (task->wait_time <= 0) {
			if (task->func(task->arg)) {
				task->wait_time = task->delay;
			} else {
				// remove the task from the list
				*i = task->next;
				free(task);
				self->num_tasks--;
				// need this null check as it's possible task->next is
				// null, and then the iterative step of the for loop
				// would fail
				if (*i == NULL)
					break;
			}
		}
	}
	
	self->last_time->tv_sec = current_time.tv_sec;
	self->last_time->tv_nsec = current_time.tv_nsec;
}
