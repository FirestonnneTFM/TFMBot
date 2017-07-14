#ifndef BOT_API_H
#define BOT_API_H

#include "common.h"
#include "player.h"

struct Bot;

/**
 * The `struct BotApi` is essentially made to encapsulate a bunch of
 * function pointers to various events that may occur during. It is the
 * backbone of any custom-defined bot.
 *
 * The first argument to any of these functions is a reference to the
 * struct Bot that is calling the api.
 *
 * All of the function pointers can also be left as NULL, which can
 * either make the bot not listen for the event, or cause a default
 * action to occur.
 */
struct BotApi {
	// the name of the api, not the player username
	char *name;

	/**
	 * Returns the username that the bot should attempt to connect
	 * with. When NULL, "Souris" is used. Can be ignored if the -u
	 * switch is passed.
	 */
	char *(*get_username)(struct Bot *);

	/**
	 * Returns the hashed password that the bot will use to login. Use
	 * the --hash-password utility to create a hashed password from
	 * plaintext. Neither plantext nor hashed passwords should be
	 * present in shared source code, as both will compromise the
	 * account. When NULL, an empty password will be used, and the bot
	 * will connect as a guest account. Can be ignored if the -p
	 * switched is passed, which generally speaking is probably the
	 * best practice security-wise.
	 */
	char *(*get_password)(struct Bot *);

	/**
	 * Returns the name of the room that the bot should attempt to
	 * join on login. When NULL, room "village gogogo" will be
	 * used. Can be overridden by the -r switch.
	 */
	char *(*get_login_room)(struct Bot *);

	/**
	 * Event triggered immediately after obtaining a username from the
	 * server.
	 */
	void (*on_connect)(struct Bot *);

	/**
	 * Event triggerd immediately after joining a room.
	 */
	void (*on_room_join)(struct Bot *);

	/**
	 * Event triggered when a new map is loaded
	 */
	void (*on_new_map)(struct Bot *);

	/**
	 * Event triggered when a new player joins the room. New player is
	 * passed as the argument.
	 */
	void (*on_player_join)(struct Bot *, struct Player *);

	/**
	 * Event triggered when a player leaves the room. Said player is
	 * passed as the argument, but note that immediately after that
	 * pointer is freed, so if struct is needed, you should copy it to
	 * new memory
	 */

	void (*on_player_leave)(struct Bot *, struct Player *);

	/**
	 * Event triggered when a player's coordinates and movement
	 * information is sent from the server. Said player is passed as
	 * the argument.
	 */
	void (*on_player_move)(struct Bot *, struct Player *);

	/**
	 * Event triggered when a player crouches. Said player is passed
	 */
	void (*on_player_duck)(struct Bot *, struct Player *);

	/**
	 * Event triggered when a player dies; player passed as the
	 * argument.
	 */
	void (*on_player_death)(struct Bot *, struct Player *);

	/**
	 * Event triggered when a player talks in the general room
	 * chat. The player and the message are passes as arguments.
	 */
	void (*on_player_chat)(struct Bot *, struct Player *, char *);

	/**
	 * Event triggered when a player sends an emote, like dancing,
	 * crying, or sleeping. The player is passed, along with the id of
	 * the emote. Several #define's are made in bot.h for some emotes
	 */
	void (*on_player_emote)(struct Bot *, struct Player *, byte);

	/**
	 * Event triggered when the struct Bot is being freed, along with
	 * this api. This should call free() on memory that was allocated
	 * in any of the other events.
	 */
	void (*on_dispose)(struct Bot *);

	/**
	 * Event triggered when a command is sent from the control
	 * panel. The first string passed is the command, which is always
	 * 3 characters, and represents the command. The second argument
	 * the length of the body of the command, followed by the actual
	 * string. The body could be an empty string. Return true if the
	 * command was found, or false if unused.
	*/
	bool(*on_control)(struct Bot *, char *, char *); };

void init_bot_api(int);
struct BotApi *get_registered_api(int);
struct BotApi *BotApi_new(char *);
void BotApi_dispose(struct BotApi *);
void BotApi_register(struct BotApi *);

#endif
