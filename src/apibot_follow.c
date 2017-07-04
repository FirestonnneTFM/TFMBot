#include "apibot_follow.h"
#include "bot.h"
#include <string.h>
#include "scheduler.h"

#define COMMANDER_USERNAME()(x_arg)

static struct Player *target = NULL;
static struct Player *commander = NULL;
#define api_data()(*((int*)self->api_data))

static void on_player_join(struct Bot *self, struct Player *player)
{
	UNUSED(self);
	if (strcmp(player->name, COMMANDER_USERNAME()) == 0) {
		commander = player;
		printf("Commander set to : %s\n", COMMANDER_USERNAME());
	}
}

static inline bool cmd_check(char *raw, char a, char b)
{
	return raw[1] == a && raw[2] == b && raw[3] == ' ';
}

static void on_player_chat(struct Bot *self, struct Player *player, char *msg)
{
	if (! commander)
		return;
	if (player->id != commander->id || msg[0] != '~' || strlen(msg) < 3)
		return;
	char *ptr = msg + 4;
	if (cmd_check(msg, 'o', 'n')) {
		if (strlen(ptr) < 3)
			target = Room_get_player_name(self->room, COMMANDER_USERNAME());
		else
			target = Room_get_player_name(self->room, ptr);
		if (target)
			printf("Target set to : %s\n", ptr);
	} else if (cmd_check(msg, 't', 'o')) {
		Bot_change_room(self, ptr);
	}
}

struct tuple {
	struct Bot *bot;
	struct Player *player;
};

static bool send_target_coords(void *ptr)
{
	struct tuple *t = (struct tuple*)ptr;
	Bot_send_player_coords(t->bot, t->player);
	free(t->player);
	free(t);
	return false;
}

static void on_player_move(struct Bot *self, struct Player *player)
{
	if (target && player->id == target->id) {
		struct tuple *t = (struct tuple*)malloc(sizeof(struct tuple));
		t->bot = self;
		t->player = (struct Player*)malloc(sizeof(struct Player));
		memcpy(t->player, player, sizeof(struct Player));
		Scheduler_add(Main_Scheduler, Task_new(api_data() * 100,
			send_target_coords, t));
	}
}

static bool send_coords(void *ptr)
{
	if (target)
		return false;
	struct Bot *self = (struct Bot*)ptr;
	Bot_send_player_coords(self, self->player);
	return true;
}

static void on_connect(struct Bot *self)
{
	if (x_arg == NULL)
		fatal("Follow bot needs an x-arg for the commander");
	self->player->x = 0x1921;
	self->player->y = 0x0A0C;
	self->api_data = (int*)malloc(sizeof(int));
	api_data() = num_bots_running;
	printf("[%d] Connected with name: %s\n", num_bots_running, self->player->name);
	Scheduler_add(Main_Scheduler, Task_new(3000, send_coords, self));
}

static void on_dispose(struct Bot *self)
{
	free(self->api_data);
}

static char *get_login_room(struct Bot *self)
{
	UNUSED(self);
	return "801";
}

static char *get_username(struct Bot *self)
{
	UNUSED(self);
	return "Souris";
}

void register_apibot_follow(void)
{
	struct BotApi *api = BotApi_new("follow bot");
	api->get_username = get_username;
	api->on_connect = on_connect;
	api->get_login_room = get_login_room;
	api->on_player_join = on_player_join;
	api->on_player_move = on_player_move;
	api->on_dispose = on_dispose;
	api->on_player_chat = on_player_chat;
	BotApi_register(api);
}
