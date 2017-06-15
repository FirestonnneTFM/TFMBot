#include "bot.h"
#include "key_manager.h"

int main(int argc, char **argv)
{
	init_keys();
	Bot *bot = Bot_new();
	Bot_start(bot);
	Bot_dispose(bot);
	
	return 0;
}
