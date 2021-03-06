#include "apibot_afk.h"
#include "bot.h"
#include "scheduler.h"
#include "control_panel.h"

#define api_data()(*((bool*)self->api_data))

static int num_of_usernames = 0;
static char** list_of_usernames = NULL;

static void on_dispose(struct Bot *self)
{
	free(self->api_data);
	free(list_of_usernames);
}

static void on_player_chat(struct Bot *self, struct Player *player, char *message)
{
	UNUSED(self);
	printf("[%s] %s\n", player->name, message);
}

static void on_player_join(struct Bot *self, struct Player *player)
{
	UNUSED(self);
	printf("Player `%s` joined with id %u\n", player->name, player->id);
}

static void on_room_join(struct Bot *self)
{
	Bot_send_command(self, "time");
	printf("Joined room : %s\n", self->room->name);
}

static void on_new_map(struct Bot *self) {
	 printf("New map: @%u by %s (P%u) : %u players\n", self->room->map_code,
			self->room->map_author, self->room->map_pcode, self->room->player_count);
}

static bool send_coords(void *ptr)
{
	struct Bot *self = (struct Bot*)ptr;
	if (api_data())
		self->player->x ++;
	else
		self->player->x --;
	api_data() = ! api_data();
	self->player->jumping = api_data();
	Bot_send_player_coords(self, self->player);
	return true;
}

static void on_login(struct Bot *self)
{
	printf("Connected with name: %s\n", self->player->name);
	self->player->x = 0x1921;
	self->player->y = 0x0A0C;
	Scheduler_add(Main_Scheduler, Task_new(3000, send_coords, self));
	self->api_data = calloc(1, sizeof(bool));
}

static bool get_login_room(struct Bot *self, char **roomname)
{
	UNUSED(self);
	*roomname = (char*)malloc(sizeof(char) * 16);
	sprintf(*roomname, "village ");
	int i;
	for (i = 8; i < 15; i++) {
		roomname[0][i] = (rand() % 26) + 'a';
	}
	roomname[0][i] = '\0';
	return true;
}

static bool get_username(struct Bot *self, char **username)
{
	UNUSED(self);
	int index = num_bots_running - 1;
	if (index >= num_of_usernames)
		return false;
	*username = list_of_usernames[index];
	return true;
}

static void on_cp_chat_join(struct Bot *self, char *chat_name)
{
	UNUSED(self);
	printf("Joined chat #%s\n", chat_name);
}

static void on_cp_chat_recv(struct Bot *self, char *chat, char *username, char *msg) {
	UNUSED(self);
	printf("[#%s] [%s] %s\n", chat, username, msg);
}

void register_apibot_afk()
{
	if (x_arg) {
		// read potential usernames from file
		FILE *f = fopen(x_arg, "r");
		if (f == NULL)
			fatal("Username file: Could not open");
		if (fscanf(f, "%d", &num_of_usernames) != 1)
			fatal("Username file: need number of usernames");
		list_of_usernames = (char**)malloc(sizeof(char*) * num_of_usernames);
		int i;
		// hope for no buffer overflow
		for (i = 0; i < num_of_usernames; i++) {
			list_of_usernames[i] = (char*)calloc(1, 16);
			if (fscanf(f, "%s", list_of_usernames[i]) != 1)
				fatal("Username file: not enough usernames provided");
		}
		fclose(f);
	}
	struct BotApi *api = BotApi_new("afk bot");
	api->get_username = get_username;
	api->get_login_room = get_login_room;
	api->on_login = on_login;
	api->on_room_join = on_room_join;
	api->on_new_map = on_new_map;
	api->on_player_join = on_player_join;
	api->on_player_chat = on_player_chat;
	api->on_dispose = on_dispose;
	api->on_cp_chat_join = on_cp_chat_join;
	api->on_cp_chat_recv = on_cp_chat_recv;
	BotApi_register(api);
}
