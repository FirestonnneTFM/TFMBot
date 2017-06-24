#include "task.h"

struct Task *Task_new(int delay, bool (*func)(void *), void *arg)
{
	struct Task *self = (struct Task*)malloc(sizeof(struct Task));
	self->delay = delay;
	self->wait_time = delay;
	self->func = func;
	self->arg = arg;
	self->next = NULL;
	return self;
}
