#include "apibot_follow.h"
#include "bot.h"
#include <string.h>
#include "scheduler.h"

//#define FOLLOW_USERNAME "Liar_2349892"
#define FOLLOW_USERNAME "*Ring"

static struct Player *target = NULL;
#define api_data()(*((int*)self->api_data))

static void on_player_join(struct Bot *self, struct Player *player)
{
	UNUSED(self);
	if (strcmp(player->name, FOLLOW_USERNAME) == 0) {
		target = player;
		printf("Target set to : %s\n", FOLLOW_USERNAME);
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
		t->player = Player_new();
		Player_copy(t->player, player);
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
	BotApi_register(api);
}
