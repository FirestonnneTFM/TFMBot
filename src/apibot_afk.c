#include "apibot_afk.h"
#include "bot.h"

static void on_chat(BotApi *self, Player *player, char *message)
{
	UNUSED(self);
	printf("[%s] %s\n", player->name, message);
}

static void on_player_death(BotApi *self, Player *player)
{
	UNUSED(self);
	printf("Player `%s` died", player->name);
}

static void on_player_join(BotApi *self, Player *player)
{
	UNUSED(self);
	printf("Player `%s` joined with id %u\n", player->name, player->id);
}

static void on_room_join(BotApi *self, char *roomname)
{
	UNUSED(self);
	printf("Joined room : %s\n", roomname);
}

static void on_new_map(BotApi *self, uint32_t code, char *author,
					   byte pcode, uint16_t num_players) {
	UNUSED(self);
	printf("New map: @%u by %s (P%u) : %u players",
		   code, author, pcode, num_players);
}

static void on_connect(BotApi *self, Player *player)
{
	UNUSED(self);
	printf("Connected with name: %s\n", player->name);
}

static char *get_login_room(BotApi *self)
{
	UNUSED(self);
	return "village gogogo";
}

static char *get_username(BotApi *self)
{
	UNUSED(self);
	return "Sourisss";
}

void register_apibot_afk()
{
	BotApi *api = BotApi_new("afk bot");
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
