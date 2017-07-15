#include "bot_api.h"

static int bot_api_num;
static int bot_api_ptr;
static struct BotApi** bot_api_list;

void init_bot_api(int num)
{
	if (num <= 0)
		fatal("There must be at least one bot api defined");
	bot_api_num = num;
	bot_api_ptr = 0;
	bot_api_list = (struct BotApi**)malloc(sizeof(struct BotApi*) * num);
}

struct BotApi *get_registered_api(int which)
{
	if (which < 0 || which >= bot_api_num)
		fatal("Bot api does not exist");
	return bot_api_list[which];
}

struct BotApi *BotApi_new(char *name)
{
	struct BotApi *self = (struct BotApi*)calloc(1, sizeof(struct BotApi));
	self->name = name;
	return self;
}

void BotApi_dispose(struct BotApi *self)
{
	free(self->name);
	free(self);
}

void BotApi_register(struct BotApi *self)
{
	if (bot_api_ptr == bot_api_num)
		fatal("Attempt to register more bot apis than initialized");
	if (self->name == NULL)
		fatal("Bot api must declare a name");
	bot_api_list[bot_api_ptr] = self;
	printf("Registered module [%d] `%s`\n", bot_api_ptr, self->name);
	bot_api_ptr ++;
}
