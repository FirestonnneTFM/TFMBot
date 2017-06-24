#include <pthread.h>
#include "bot.h"
#include "key_manager.h"
#include "scheduler.h"
#include "apibot_afk.h"
#include "apibot_follow.h"

static volatile int bots_running = 0;

static void *run_bot(void *ptr)
{
	bots_running ++;
	struct Bot *bot = (struct Bot*)ptr;
	Bot_start(bot);
	Bot_dispose(bot);
	bots_running --;
	return NULL;
}

int main(int argc, char **argv)
{
	if (argc < 3)
		fatal("Missing api number and/or number of bots");

	// TODO: better arg processing
	int api_number = atoi(argv[1]);
	int num_bot = atoi(argv[2]);

	Main_Scheduler = Scheduler_new();
	init_keys();
	init_bot_api(2);
	register_apibot_afk();
	register_apibot_follow();

	int i;
	for (i = 0; i < num_bot; i++) {
		struct Bot *bot = Bot_new(api_number);
		pthread_t thread;
		if (pthread_create(&thread, NULL, run_bot, bot))
			fatal("Thread creation failed");
		sleep_ms(1500);
	}
	while (true) {
		Scheduler_tick(Main_Scheduler);
		sleep_ms(10);
	}
	return 0;
}
