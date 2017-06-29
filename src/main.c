#include <pthread.h>
#include "bot.h"
#include "key_manager.h"
#include "scheduler.h"
#include "apibot_afk.h"
#include "apibot_follow.h"
#include <string.h>

static void *run_bot(void *ptr)
{
	struct Bot *bot = (struct Bot*)ptr;
	Bot_start(bot);
	Bot_dispose(bot);
	return NULL;
}

static char *asm_name = NULL;

static void print_usage(void)
{
	printf("Usage : %s -a <which bot> [other flags]\n", asm_name);
	puts("FLAGS");
	puts("-a   Api number: the number of the registered bot api to use");
	puts("-n   Bot number: how many bots to connect");
	puts("-u   Username to use: overwrites username function in api");
	puts("-p   Password to use: overwrites password function in api");
	puts("-r   Room to join: overwrites room name function in api");
	exit(0);
}

int main(int argc, char **argv)
{
	asm_name = argv[0];
	if (argc == 1)
		print_usage();
	int api_number = -1;
	int bots_to_create = 1;
	bool arg_a_flag = false;
	bool arg_n_flag = false;
	bool arg_u_flag = false;
	bool arg_p_flag = false;
	bool arg_r_flag = false;
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			int check = 0;
			check += arg_a_flag = argv[i][1] == 'a';
			check += arg_n_flag = argv[i][1] == 'n';
			check += arg_u_flag = argv[i][1] == 'u';
			check += arg_p_flag = argv[i][1] == 'p';
			check += arg_r_flag = argv[i][1] == 'r';
			if (check == 0) {
				printf("Unknown switch `%s`\n", argv[i]);
				print_usage();
			}
		} else {
			if (arg_a_flag) {
				api_number = atoi(argv[i]);
				arg_a_flag = false;
			} else if (arg_n_flag) {
				bots_to_create = atoi(argv[i]);
				arg_n_flag = false;
			} else if (arg_u_flag) {
				override_username = argv[i];
				arg_u_flag = false;
			} else if (arg_p_flag) {
				override_password = argv[i];
				arg_p_flag = false;
			} else if (arg_r_flag) {
				override_roomname = argv[i];
				arg_r_flag = false;
			} else {
				printf("unexpected value `%s`\n", argv[i]);
				print_usage();
			}
		}
	}

	if (api_number < 0) {
		puts("No api number given");
		print_usage();
	}

	Main_Scheduler = Scheduler_new();
	init_keys();
	init_bot_api(2);
	register_apibot_afk();
	register_apibot_follow();

	for (i = 0; i < bots_to_create; i++) {
		struct Bot *bot = Bot_new(api_number);
		pthread_t thread;
		if (pthread_create(&thread, NULL, run_bot, bot))
			fatal("Thread creation failed");
		sleep_ms(3000);
	}
	while (true) {
		Scheduler_tick(Main_Scheduler);
		sleep_ms(10);
	}
	return 0;
}
