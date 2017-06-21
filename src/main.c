#include "bot.h"
#include "key_manager.h"
#include "apibot_afk.h"

int main(int argc, char **argv)
{
	UNUSED(argc);
	UNUSED(argv);
	
	init_keys();
	init_bot_api(1);
	register_apibot_afk();

	struct Bot *bot = Bot_new(0);
	Bot_start(bot);
	Bot_dispose(bot);
	
	return 0;
}
