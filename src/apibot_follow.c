#include "apibot_follow.h"
#include "bot.h"
#include <string.h>
#include "scheduler.h"

static struct Player *target = NULL;
static struct Player *default_target = NULL;
#define api_data()(*((int*)self->api_data))
#define cmd_yield(s) do { ControlPanel_reply(ctrl, s); return true; } while(0)
	
static bool on_control(struct Bot *self, struct ControlPanel *ctrl, uint32_t cmd, char *msg)
{
	switch (cmd) {
	case 'pon': {
		if (default_target && strlen(msg) < 1)
			target = default_target;
		else
			target = Room_get_player_name_closest(self->room, msg);
		if (target) {
			char buf[32];
			sprintf(buf, "Target set to : %s", target->name);
			cmd_yield(buf);
		} else {
			cmd_yield("Target not found");
		}
	}
	case 'com': {
		if (msg[0])
			default_target = Room_get_player_name_closest(self->room, msg);
		else if (x_arg)
			default_target = Room_get_player_name(self->room, x_arg);
		if (default_target) {
			char buf[32];
			sprintf(buf, "Default set to : %s", default_target->name);
			cmd_yield(buf);
		} else {
			cmd_yield("Player not found");
		}
	}
	default:
		return false;
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

static bool get_login_room(struct Bot *self, char **name)
{
	UNUSED(self);
	*name = "801";
	return false;
}

void register_apibot_follow(void)
{
	struct BotApi *api = BotApi_new("follow bot");
	api->on_connect = on_connect;
	api->get_login_room = get_login_room;
	api->on_player_move = on_player_move;
	api->on_dispose = on_dispose;
	api->on_control = on_control;
	BotApi_register(api);
}




