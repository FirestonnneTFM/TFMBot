#include <pthread.h>
#include "bot.h"
#include "crypto.h"
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
	printf("Bot usage : %s -a <which bot> [other flags]\n", asm_name);
	puts("FLAGS");
	puts("-a   Api number: the number of the registered bot api to use");
	puts("-n   Bot number: how many bots to connect (default: 1)");
	puts("-u   Username to use: overwrites username function in api");
	puts("-p   File path to a hashed password to use: overwrites password function in api");
	puts("     Note: this is a FILE, for security reasons the hashed password is not put as a direct arg");
	puts("     Use the password-hash utility to generate a hashed password");
	puts("-r   Room to join: overwrites room name function in api");
	puts("-x   Extended arg: passes this argument to the bot api");
	puts("     What it means (if anything) depends on which api you use");
	putchar('\n');
	printf("Utililty usage : %s --<util flag>\n", asm_name);
	puts("UTILITY FLAGS");
	puts("--help            Display this help");
	puts("--password-hash   Prints a Transformice-compatible password from");
	puts("                  plaintext provided on stdin");
	puts("--print-keys      Prints the keys found in the keys.bin file (if found)");
	puts("--hash-key        Prints a hashed key from the string on stdin");
	putchar('\n');
	exit(0);
}

static void print_key_byte(byte *arr)
{
	int i;
	for (i = 0; i < 20; i++) {
		printf("%02x ", arr[i]);
	}
	putchar('\n');
}

static void print_usage_prompt(void)
{
	fprintf(stderr, "For usage, use %s --help\n", asm_name);
	exit(-1);
}

static void util_mode(char *arg)
{
	if (strcmp(arg, "help") == 0) {
		print_usage();
	} else if (strcmp(arg, "password-hash") == 0) {
		fatal("Unimplemented");
	} else if (strcmp(arg, "print-keys") == 0) {
		init_keys();
		printf("Hash key : ");
		print_key_byte(Key_Manager->hash_key);
		printf("Login Key : %08x\n", Key_Manager->login_key);
		printf("Handshake Number : %04x\n", Key_Manager->handshake_number);
		printf("Handshake String : %s\n", Key_Manager->handshake_string);
	} else if (strcmp(arg, "hash-key") == 0) {
		init_keys();
		uint32_t buf[20];
		char str[200];
		fgets(str, 190, stdin);
		djb_hash(buf, str, strlen(str));
		int i;
		for (i = 0; i < 20; i++) {
			printf("%08x ", buf[i]);
			if (i % 5 == 4)
				putchar('\n');
		}
		putchar('\n');
		
	} else {
		fprintf(stderr, "Unknown flag --%s\n", arg);
		print_usage_prompt();
	}
}

static void password_from_file(void)
{
	FILE *pw_file = fopen(override_password, "r");
	if (pw_file == NULL)
		fatal("Password file cannot be opened");
	char buf[200];
	char c;
	int i = 0;
	while ((c = fgetc(pw_file)) != EOF && c != '\n') {
		buf[i] = c;
		i++;
	}
	fclose(pw_file);
	override_password = (char*)malloc(sizeof(char) * (i + 1));
	memcpy(override_password, buf, i);
	override_password[i] = '\0';
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
	bool arg_x_flag = false;
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				util_mode(argv[i] + 2);
				return 0;
			}
			int check = 0;
			check += arg_a_flag = argv[i][1] == 'a';
			check += arg_n_flag = argv[i][1] == 'n';
			check += arg_u_flag = argv[i][1] == 'u';
			check += arg_p_flag = argv[i][1] == 'p';
			check += arg_r_flag = argv[i][1] == 'r';
			check += arg_x_flag = argv[i][1] == 'x';
			if (check == 0) {
				fprintf(stderr, "Unknown switch `%s`\n", argv[i]);
				print_usage_prompt();
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
			} else if (arg_x_flag) {
				x_arg = argv[i];
				arg_x_flag = false;
			} else {
				fprintf(stderr, "unexpected value `%s`\n", argv[i]);
				print_usage_prompt();
			}
		}
	}

	if (api_number < 0) {
		fprintf(stderr, "No api number given\n");
		print_usage_prompt();
	}

	if (override_password)
		password_from_file();
	
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
