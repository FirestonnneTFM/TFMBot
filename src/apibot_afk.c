#include "apibot_afk.h"
#include "bot.h"

static void on_chat(Player *player, char *message)
{
	printf("[%s] %s\n", player->name, message);
}

static void on_player_death(Player *player)
{
	printf("Player `%s` died", player->name);
}

static void on_player_join(Player *player)
{
	printf("Player `%s` joined with id %u\n", player->name, player->id);
}

static void on_connect(Player *player)
{
	printf("Connected with name: %s\n", player->name);
}

static char *get_username(void)
{
	return "Sourisss";
}

void register_apibot_afk()
{
	BotApi *api = BotApi_new("afk bot");
	api->get_username = get_username;
	api->on_connect = on_connect;
	api->on_player_join = on_player_join;
	api->on_player_death = on_player_death;
	api->on_chat = on_chat;
	BotApi_register(api);
}
