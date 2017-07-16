#include <pthread.h>
#include <sys/ioctl.h>
#include "bot.h"
#include "crypto.h"
#include "scheduler.h"

#define HOST "164.132.202.12"
#define PORT 5555
//#define PRINT_ALL_PACKETS

volatile int num_bots_running = 0;
struct Bot **bots_running = NULL;
char *x_arg = NULL;
byte login_mode = 0;
char *override_username = NULL;
char *override_password = NULL;
char *override_roomname = NULL;

static bool heartbeat_task(void *ptr)
{
	struct Bot *self = (struct Bot*)ptr;
	// we must send this heartbeat every so often
	// ten seconds is fine
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_HEARTBEAT);
	// send it to both connections
	Connection_send(self->main_conn, b);
	Connection_send(self->bulle_conn, b);
	ByteStream_dispose(b);
	return self->running;
}

struct Bot *Bot_new(int which_api)
{
	struct Bot *self = (struct Bot*)malloc(sizeof(struct Bot));
	self->running = true;
	self->api_data = NULL;
	self->main_conn = Connection_new();
	self->bulle_conn = Connection_new();
	self->cp_index = 0;
	self->api = get_registered_api(which_api);
	self->room = Room_new();
	self->player = Player_new();
	self->map = Map_new();
	return self;
}

void Bot_dispose(struct Bot *self)
{
	Connection_dispose(self->main_conn);
	Connection_dispose(self->bulle_conn);
	if (self->api->on_dispose)
		self->api->on_dispose(self);
	free(self->api);
	free(self->room);
	free(self->player);
	free(self->map);
	free(self);
	num_bots_running --;
}

void Bot_send_player_coords(struct Bot *self, struct Player *player)
{
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_PLAYER_POSITION);
	ByteStream_write_u32(b, player->round_num);
	ByteStream_write_byte(b, player->key_right);
	ByteStream_write_byte(b, player->key_left);
	ByteStream_write_u16(b, player->x);
	ByteStream_write_u16(b, player->y);
	ByteStream_write_u16(b, player->acc_x);
	ByteStream_write_u16(b, player->acc_y);
	ByteStream_write_byte(b, player->jumping);
	ByteStream_write_byte(b, player->animation_frame);
	ByteStream_write_byte(b, 0);
	Connection_send(self->bulle_conn, b);
	ByteStream_dispose(b);
}

void Bot_change_room(struct Bot *self, char *room_name)
{
	// this doesn't work yet since connecting to a new room requires a
	// new game sock to be opened, a bit of a pain
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, 0x0526);
	ByteStream_write_byte(b, 0xff);
	ByteStream_write_str(b, room_name);
	ByteStream_write_byte(b, 0);
	Connection_send(self->bulle_conn, b);
	ByteStream_dispose(b);
}

void Bot_send_chat(struct Bot *self, char *msg)
{
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_PLAYER_CHAT);
	ByteStream_write_str(b, msg);
	ByteStream_xor_cipher(b, self->bulle_conn->k);
	Connection_send(self->bulle_conn, b);
	ByteStream_dispose(b);
}

void Bot_send_command(struct Bot *self, char *cmd)
{
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_PLAYER_CHAT_COMMAND);
	ByteStream_write_str(b, cmd);
	ByteStream_xor_cipher(b, self->main_conn->k);
	Connection_send(self->main_conn, b);
	ByteStream_dispose(b);
}

void Bot_send_emote(struct Bot *self, byte emote)
{
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_PLAYER_EMOTE);
	ByteStream_write_byte(b, emote);
	ByteStream_write_u32(b, 0xFFFFFFFF);
	Connection_send(self->bulle_conn, b);
	ByteStream_dispose(b);
}

void Bot_join_cp_chat(struct Bot *self, char *chat)
{
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_CP_PROTOCOL);
	ByteStream_write_u16(b, CP_CCC_JOIN_CHAT);
	ByteStream_write_u32(b, ++(self->cp_index));
	ByteStream_write_str(b, chat);
	ByteStream_write_byte(b, 0x01);
	ByteStream_xor_cipher(b, self->main_conn->k);
	Connection_send(self->main_conn, b);
	ByteStream_dispose(b);
}

void Bot_send_cp_chat(struct Bot *self, char *chat, char *msg)
{
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_CP_PROTOCOL);
	ByteStream_write_u16(b, CP_CCC_CHAT_SEND);
	ByteStream_write_u32(b, ++(self->cp_index));
	ByteStream_write_str(b, chat);
	ByteStream_write_str(b, msg);
	ByteStream_xor_cipher(b, self->main_conn->k);
	Connection_send(self->main_conn, b);
	ByteStream_dispose(b);
}

void Bot_do_register(struct Bot *self, char *captcha)
{
	char *username = NULL;
	bool free_username = false;
	if (override_username)
		username = override_username;
	else if (self->api->get_username)
		free_username = self->api->get_username(self, &username);
	char *password = override_password;
	if (password == NULL)
		fatal("Registration requires a password");

	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_REGISTER);
	ByteStream_write_str(b, username);
	ByteStream_write_str(b, password);
	ByteStream_write_str(b, captcha);
	ByteStream_write_u16(b, 0);
	ByteStream_write_str(b, "app:/TransformiceAIR.swf/[[DYNAMIC]]/2/[[DYNAMIC]]/4");
	ByteStream_xor_cipher(b, self->main_conn->k);
	Connection_send(self->main_conn, b);
	ByteStream_dispose(b);
	
	if (free_username)
		free(username);
}

static inline void Bot_handle_packet(struct Bot *self, struct Connection *conn, uint16_t ccc, struct ByteStream *b)
{
	switch (ccc) {
	case CCC_HANDSHAKE_OK: {
		uint32_t players_online = ByteStream_read_u32(b);
		conn->k = ByteStream_read_byte(b);
		char *community = ByteStream_read_str(b);
		char *country = ByteStream_read_str(b);
		uint32_t login_xor = ByteStream_read_u32(b);
		printf("Connected %s.%s :: %u online players\n",
			   country, community, players_online);
		free(country);
		free(community);
		
		Scheduler_add(Main_Scheduler, Task_new(10000, heartbeat_task, self));

		b = ByteStream_new();
		ByteStream_write_u16(b, CCC_SET_COMMUNITY);
		// 0x0E is for E2 community
		ByteStream_write_u16(b, 0x0E00);
		Connection_send(conn, b);
		ByteStream_dispose(b);

		b = ByteStream_new();
		ByteStream_write_u16(b, CCC_OS_INFO);
		ByteStream_write_str(b, "en");
		ByteStream_write_str(b, "Windows 8");
		ByteStream_write_str(b, "WIN 25,0,0,127");
		ByteStream_write_byte(b, 0);
		Connection_send(conn, b);
		ByteStream_dispose(b);

		char *username = NULL;
		bool free_username = false;
		if (override_username)
			username = override_username;
		else if (self->api->get_username)
			free_username = self->api->get_username(self, &username);
		
		char *password = override_password ? override_password : NULL;

		char *login_room = NULL;
		bool free_login_room = false;
		if (override_roomname)
			login_room = override_roomname;
		else if (self->api->get_login_room)
			free_login_room = self->api->get_login_room(self, &login_room);
		else
			login_room = "village gogogo";
		
		switch (login_mode) {
		case 1:
			b = ByteStream_new();
			ByteStream_write_u16(b, CCC_CAPTCHA);
			Connection_send(conn, b);
			ByteStream_dispose(b);
			return;
		case 2:
			return;
		}
		
		b = ByteStream_new();
		ByteStream_write_u16(b, CCC_LOGIN);
		ByteStream_write_str(b, username);
		ByteStream_write_str(b, password);
		ByteStream_write_str(b, "app:/TransformiceAIR.swf/[[DYNAMIC]]/2/[[DYNAMIC]]/4");
		ByteStream_write_str(b, login_room);
		ByteStream_write_u32(b, Key_Manager->login_key ^ login_xor);
		ByteStream_block_cipher(b);
		ByteStream_write_byte(b, 0);
		Connection_send(conn, b);
		ByteStream_dispose(b);
		if (free_username)
			free(username);
		if (free_login_room)
			free(login_room);
		break;
	}
	case CCC_LOGIN_OK:
		// I think this might be your forum ID, zero for guests
		ByteStream_read_u32(b);
		self->player->name = ByteStream_read_str(b);
		if (self->api->on_login) {
			self->api->on_login(self);
		}
		break;
	case CCC_SWITCH_BULLE: {
		// information to connect to game sock
		self->player->id = ByteStream_read_u32(b);
		char *ip = ByteStream_read_str(b);
		Connection_open(self->bulle_conn, ip, 5555);
		free(ip);
		b = ByteStream_new();
		ByteStream_write_u16(b, CCC_SWITCH_BULLE);
		ByteStream_write_u32(b, self->player->id);
		Connection_send(self->bulle_conn, b);
		ByteStream_dispose(b);
		break;
	}
	case CCC_SET_KEY_OFFSET:
		conn->k = ByteStream_read_byte(b);
		break;
	case 0x1C32:
		// this packet appears to contain a lot of garbage
		// and it wants you to send back a bunch of garbage
		// we are going to ignore it now
		break;
	case CCC_WHICH_GAME:
		// this one byte packet tells you what game you are joining
		// 00 = transformice
		break;
	case CCC_CAPTCHA: {
		uint16_t width = ByteStream_read_u16(b);
		uint16_t height = ByteStream_read_u16(b);
		if (width * height != ByteStream_read_u16(b))
			warning("Bitmap width / height did not match size");
		puts("CAPTCHA");
		int x, y;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				byte v = ByteStream_read_u32(b) >> 24;
				if (v < 50)
					putchar(' ');
				else if (v < 100)
					putchar('.');
				else if (v < 200)
					putchar('*');
				else
					putchar('8');
			}
			putchar('\n');
		}
		break;
	}
	case CCC_ROOM_JOIN:
		// packet for room
		if (self->api->on_room_join == NULL)
			break;
		ByteStream_read_byte(b);
		free(self->room->name);
		self->room->name = ByteStream_read_str(b);
		self->api->on_room_join(self);
		break;
	case CCC_NEW_MAP: {
		// packet for map
		if (self->api->on_new_map == NULL)
			break;
		self->room->map_code = ByteStream_read_u32(b);
		self->room->player_count = ByteStream_read_u16(b);
		// this byte is always 0x01 (maybe it signals if the map is
		// compressed?)
		ByteStream_read_byte(b);
		uint32_t compressed_map_len = ByteStream_read_u32(b);
		Map_load(self->map, b->array + b->position, compressed_map_len);
		b->position += compressed_map_len;
		free(self->room->map_author);
		self->room->map_author = ByteStream_read_str(b);
		self->room->map_pcode = ByteStream_read_byte(b);
		self->api->on_new_map(self);
		break;
	}
	case CCC_OLD_PROTOCOL: {
		// old protocol packet (why these still exist nobody knows)
		// this u16 is the length of the old protocol packet
		if (ByteStream_read_u16(b) < 2)
			break;
		uint16_t old_ccc = ByteStream_read_u16(b);
		switch (old_ccc) {
		case OLD_CCC_LIST_OF_PLAYERS: {
			// list of players in the room
			bool flag = true;
			while (flag) {
				struct Player *player = Player_new();
				if (! Player_from_old_protocol(player, b)) {
					free(player);
					break;
				}
				Room_add_player(self->room, player);
				flag = false;
				while (b->position < b->count) {
					if (ByteStream_read_byte(b) == 0x01) {
						b->position--;
						flag = true;
						break;
					}
				}
			}
			break;
		}
		case OLD_CCC_NEW_PLAYER: {
			// new player joins rooms
			struct Player *player = Player_new();
			Player_from_old_protocol(player, b);
			if (Room_add_player(self->room, player) && self->api->on_player_join)
				self->api->on_player_join(self, player);
			break;
		}
		case OLD_CCC_PLAYER_DIE: {
			// gets sent when somebody dies
			if (self->api->on_player_death == NULL)
				break;
			uint32_t id = 0;
			if (ByteStream_read_byte(b) != 0x01)
				break;
			byte digit;
			while ((digit = ByteStream_read_byte(b)) != 0x01) {
				id *= 10;
				id += digit - '0';
			}
			self->api->on_player_death(self, Room_get_player_id(self->room, id));
			break;
		}
		case OLD_CCC_PLAYER_LEAVE: {
			// gets sent when somebody leaves the room
			uint32_t id = 0;
			if (ByteStream_read_byte(b) != 0x01)
				break;
			byte digit;
			while ((digit = ByteStream_read_byte(b)) != 0x01) {
				id *= 10;
				id += digit - '0';
			}
			Room_dispose_player(self->room, id);
			break;
		}
		case OLD_CCC_NEW_SHAMAN: {
			// tells you who the shaman is
			// 01 32 39 37 39 39 37 37 01 
			break;
		}
		case OLD_CCC_BANNED: {
			// ban message :(
			if (ByteStream_read_byte(b) != 0x01)
				break;
			float time = 0.0f;
			byte digit;
			while ((digit = ByteStream_read_byte(b)) != 0x01) {
				time *= 10;
				time += digit - '0';
			}
			time /= 3600000.0;
			int len = b->count - b->position;
			char *msg = (char*)malloc(sizeof(char) * (len + 1));
			msg[len] = '\0';
			int i;
			for (i = 0; i < len; i++)
				msg[i] = b->array[b->position + i];
			printf("Banned %f hours : %s\n", time, msg);
			break;
		}
		default:
			printf("OLD PROTOCOL  %04x : ", old_ccc);
			ByteStream_print(b, 6);
		}
		break;
	}
	case CCC_CP_PROTOCOL: {
		uint32_t cp_ccc = ByteStream_read_u16(b);
		switch (cp_ccc) {
		case CP_CCC_JOIN_CHAT_OK: {
			if (self->api->on_cp_chat_join == NULL)
				break;
			char *chat_name = ByteStream_read_str(b);
			self->api->on_cp_chat_join(self, chat_name);
			free(chat_name);
			break;
		}
		case CP_CCC_CHAT_RECV: {
			if (self->api->on_cp_chat_recv == NULL)
				break;
			char *username = ByteStream_read_str(b);
			// might be community?
			ByteStream_read_u32(b);
			char *chat = ByteStream_read_str(b);
			char *msg = ByteStream_read_str(b);
			self->api->on_cp_chat_recv(self, chat, username, msg);
			free(username);
			free(chat);
			free(msg);
			break;
		}
		case CP_CCC_CONNECT:
			puts("Connected to community platform");
			break;
		case 0x0037:
			// recv after joining a chat
			break;
		case CP_CCC_CHAT_SEND_STATUS: {
			// recv when you send a message
			// some counter
			ByteStream_read_u32(b);
			byte status = ByteStream_read_byte(b);
			switch (status) {
			case 0x00:
				// success
				break;
			case 0x17:
				puts("Message failed : spam protection");
				break;
			default:
				printf("Message failed (0x%x)\n", status);
			}
			break;
		}
		default:
			printf("CP PROTOCOL  %04x : ", cp_ccc);
			ByteStream_print(b, 4);
		}
		break;
	}
	case CCC_PLAYER_POSITION: {
		// player movement / location
		if (self->api->on_player_move == NULL)
			break;
		struct Player *player = Room_get_player_id(self->room, ByteStream_read_u32(b));
		if (player == NULL)
			break;
		player->round_num = ByteStream_read_u32(b);
		player->key_right = ByteStream_read_byte(b);
		player->key_left = ByteStream_read_byte(b);
		player->x = ByteStream_read_u16(b);
		player->y = ByteStream_read_u16(b);
		player->acc_x = ByteStream_read_u16(b);
		player->acc_y = ByteStream_read_u16(b);
		player->jumping = ByteStream_read_byte(b);
		player->animation_frame = ByteStream_read_byte(b);
		self->api->on_player_move(self, player);
		break;
	}
	case CCC_PLAYER_DUCK: {
		// duck
		if (self->api->on_player_duck == NULL)
			break;
		struct Player *player = Room_get_player_id(self->room, ByteStream_read_u32(b));
		if (player == NULL)
			break;
		player->ducking = ByteStream_read_byte(b);
		self->api->on_player_duck(self, player);
		break;
	}
	case CCC_PLAYER_CHAT: {
		// chat message
		if (self->api->on_player_chat == NULL)
			break;
		struct Player *player = Room_get_player_id(self->room, ByteStream_read_u32(b));
		if (player == NULL)
			break;
		// this string is the mouse name, but we disregard since we
		// have the id
		b->position += ByteStream_read_u16(b);
		// byte identifies community
		ByteStream_read_byte(b);
		char *message = ByteStream_read_str(b);
		self->api->on_player_chat(self, player, message);
		break;
	}
	case CCC_PLAYER_EMOTE: {
		// emote
		if (self->api->on_player_emote == NULL)
			break;
		struct Player *player = Room_get_player_id(self->room, ByteStream_read_u32(b));
		if (player == NULL)
			break;
		byte emote_id = ByteStream_read_byte(b);
		self->api->on_player_emote(self, player, emote_id);
		break;
	}
	case CCC_SERVER_TEXT: {
		// text sent from server, for example output of /mod
		uint32_t len = ByteStream_read_u32(b);
		if (*(b->array + b->position + len) == '\0')
			puts((char*)b->array + b->position);
		break;
	}
	case CCC_TIME_VALUE: {
		// result of /time
		ByteStream_print_ascii(b, 5);
		break;
	}
	case 0x1404:
	case 0x1009:
	case 0x6406:
	case 0x3C04:
	case 0x0816:
	case 0x0808:
	case 0x141b:
	case 0x1f01:
	case 0x1a21:
	case 0x1c02:
	case 0x071E:
	case 0x1c06:
		// who cares
		break;
#ifdef PRINT_ALL_PACKETS
	default:
		printf("%04x ", ccc);
		ByteStream_print(b, 2);
#endif
	}
}

static bool Bot_check_conn(struct Bot *self, struct Connection *conn)
{
	if (conn->sock == 0)
		return false;
	int count;
	if (ioctl(conn->sock, FIONREAD, &count) == -1)
		fatal("ioctl failed");
	bool ret = count > 0;
	if (ret) {
		struct ByteStream *b = ByteStream_new();
		ByteStream_read_sock(b, conn->sock);
		Bot_handle_packet(self, conn, ByteStream_read_u16(b), b);
		ByteStream_dispose(b);
	}
	return ret;
}

// these are pretty ugly, and are probably don't need to be accurate,
// but who cares

#define FONT_HASH "509651e4ebc76077067c4054a8c1888d9164be8fa5ff82292fd70fae2273f183"

#define FLASH_SERVER_STRING "A=t&SA=t&SV=t&EV=t&MP3=t&AE=t&VE=t&ACC=t&PR=t&SP=f&SB=f&DEB=f&V=WIN 25,0,0,127&M=Adobe Windows&R=1366x768&COL=color&AR=1.0&OS=Windows 8&ARCH=x86&L=en&IME=t&PR32=t&PR64=t&LS=en-US&PT=Desktop&AVD=f&LFD=f&WD=f&TLS=t&ML=5.1&DP=72"

void Bot_start(struct Bot *self)
{
	Connection_open(self->main_conn, HOST, PORT);
	num_bots_running ++;
	// handshake packet
	struct ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, CCC_HANDSHAKE);
	ByteStream_write_u16(b, Key_Manager->handshake_number);
	ByteStream_write_str(b, Key_Manager->handshake_string);
	ByteStream_write_str(b, "Desktop");
	ByteStream_write_str(b, "-");
	ByteStream_write_u32(b, 0x00001FBD);
	ByteStream_write_str(b, NULL);
	ByteStream_write_str(b, FONT_HASH);
	ByteStream_write_str(b, FLASH_SERVER_STRING);
	ByteStream_write_u32(b, 0);
	// this number is supposed to the time in ms that the game took to
	// load, so this doesn't have to be a magic number
	ByteStream_write_u32(b, 0x00006257);
	ByteStream_write_str(b, NULL);
	Connection_send(self->main_conn, b);
	ByteStream_dispose(b);

	while (self->running) {
		Bot_check_conn(self, self->main_conn);
		Bot_check_conn(self, self->bulle_conn);
		sleep_ms(1);
	}
}
