#include "bot_api.h"
#include <string.h>

static int Bot_Api_Num;
static int Bot_Api_Ptr;
static BotApi** Bot_Api_List;

void init_bot_api(int num)
{
	if (num <= 0)
		fatal("There must be at least one bot api defined");
	Bot_Api_Num = num;
	Bot_Api_Ptr = 0;
	Bot_Api_List = (BotApi**)malloc(sizeof(BotApi*) * num);
}

BotApi *get_registered_api(int which)
{
	if (which < 0 || which >= Bot_Api_Num)
		fatal("Bot api does not exist");
	return Bot_Api_List[which];
}

BotApi *BotApi_new(const char *name)
{
	BotApi *self = (BotApi*)malloc(sizeof(BotApi));
	memset(self, 0, sizeof(BotApi));
	self->name = name;
	return self;
}

void BotApi_register(BotApi *self)
{
	if (Bot_Api_Ptr == Bot_Api_Num)
		fatal("Attempt to register more bot apis than initialized");
	Bot_Api_List[Bot_Api_Ptr] = self;
	Bot_Api_Ptr ++;
	printf("Registered module `%s`\n", self->name);
}
