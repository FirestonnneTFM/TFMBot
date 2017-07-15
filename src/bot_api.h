#ifndef BOT_API_H
#define BOT_API_H

#include "common.h"
#include "player.h"
#include "control_panel.h"

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
 *
 * NOTE : for `get_username` and `get_login_room`. These methods are
 * meant to communicate strings, but DO NOT RETURN ANY
 * POINTERS. Instead, use the pointer supplied as an argument, which
 * is NULL by default. The function should return true if the pointer
 * needs to be freed; that is, if you called malloc or calloc on
 * it. If you used a string literal then return false so the memory is
 * not attempted to freed.
 */
struct BotApi {
	// the name of the api, not the player username
	char *name;

	/**
	 * Returns the username that the bot should attempt to connect
	 * with. When NULL, "Souris" is used. Can be ignored if the -u
	 * switch is passed.
	 */
	bool (*get_username)(struct Bot *, char **);

	/**
	 * Returns the name of the room that the bot should attempt to
	 * join on login. When NULL, room "village gogogo" will be
	 * used. Can be overridden by the -r switch.
	 */
	bool (*get_login_room)(struct Bot *, char **);

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
	 * panel. A pointer to the control panel is passed in addition to
	 * the bot. The first value represents the command, which is
	 * always a 3 character string, so it is encoded via ascii as a
	 * u32. This way you can use multiple chars with single quotes in
	 * a switch statement. The second argument the length of the body
	 * of the command, followed by the actual string. The body could
	 * be an empty string. Return true if the command was found, or
	 * false if unused.
	*/
	bool(*on_control)(struct Bot *, struct ControlPanel *, uint32_t, char *);
};

void init_bot_api(int);
struct BotApi *get_registered_api(int);
struct BotApi *BotApi_new(char *);
void BotApi_dispose(struct BotApi *);
void BotApi_register(struct BotApi *);

#endif
