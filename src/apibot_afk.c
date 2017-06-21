#include "apibot_afk.h"
#include "bot.h"

static void on_chat(struct Bot *self, struct Player *player, char *message)
{
	UNUSED(self);
	printf("[%s] %s\n", player->name, message);
}

static void on_player_death(struct Bot *self, struct Player *player)
{
	UNUSED(self);
	printf("Player `%s` died", player->name);
}

static void on_player_join(struct Bot *self, struct Player *player)
{
	UNUSED(self);
	printf("Player `%s` joined with id %u\n", player->name, player->id);
}

static void on_room_join(struct Bot *self)
{
	printf("Joined room : %s\n", self->room->name);
}

static void on_new_map(struct Bot *self) {
	 printf("New map: @%u by %s (P%u) : %u players", self->room->map_code,
			self->room->map_author, self->room->map_pcode, self->room->player_count);
	puts("New map");
}

static void on_connect(struct Bot *self)
{
	printf("Connected with name: %s\n", self->player->name);
}

static char *get_login_room(struct Bot *self)
{
	UNUSED(self);
	return "village gogogo";
}

static char *get_username(struct Bot *self)
{
	UNUSED(self);
	return "Sourisss";
}

void register_apibot_afk()
{
	struct BotApi *api = BotApi_new("afk bot");
	api->get_username = get_username;
	api->get_login_room = get_login_room;
	api->on_connect = on_connect;
	api->on_room_join = on_room_join;
	api->on_new_map = on_new_map;
	api->on_player_join = on_player_join;
	api->on_player_death = on_player_death;
	api->on_chat = on_chat;
	BotApi_register(api);
}
