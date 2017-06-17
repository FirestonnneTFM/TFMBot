#include <pthread.h>
#include <sys/ioctl.h>
#include "bot.h"
#include "key_manager.h"

#define HOST "164.132.202.12"
//#define HOST "127.0.0.1"
#define PORT 5555

Bot *Bot_new(int which_api)
{
	Bot *self = (Bot*)malloc(sizeof(Bot));
	self->main_conn = Connection_new();
	self->game_conn = Connection_new();
	self->api = get_registered_api(which_api);
	self->player = Player_new();
	return self;
}

static inline void Bot_handle_packet(Bot *self, Connection *conn, uint16_t ccc, ByteStream *b)
{
	switch (ccc) {
	case 0x1A03: {
		uint32_t players_online = ByteStream_read_u32(b);
		conn->k = ByteStream_read_byte(b);
		char *community = ByteStream_read_str(b);
		char *country = ByteStream_read_str(b);
		uint32_t login_xor = ByteStream_read_u32(b);
		printf("Connected %s.%s :: %u online players\n",
			   country, community, players_online);
		free(country);
		free(community);
		
		b = ByteStream_new();
		ByteStream_write_u16(b, 0x0802);
		// 0x0E is for E2 community
		ByteStream_write_u16(b, 0x0E00);
		Connection_send(conn, b);
		ByteStream_dispose(b);
		
		b = ByteStream_new();
		ByteStream_write_u16(b, 0x1C11);
		ByteStream_write_str(b, "en");
		ByteStream_write_str(b, "Windows 8");
		ByteStream_write_str(b, "WIN 25,0,0,127");
		ByteStream_write_byte(b, 0);
		Connection_send(conn, b);
		ByteStream_dispose(b);

		char *username;
		if (self->api->get_username == NULL)
			username = "Souris";
		else
			username = self->api->get_username();
		char *password;
		if (self->api->get_password == NULL)
			password = NULL;
		else
			password = self->api->get_password();
		
		b = ByteStream_new();
		ByteStream_write_u16(b, 0x1A08);
		ByteStream_write_str(b, username);
		ByteStream_write_str(b, password);
		ByteStream_write_str(b, "app:/TransformiceAIR.swf/[[DYNAMIC]]/2/[[DYNAMIC]]/4");
		ByteStream_write_str(b, "village gogogo");
		ByteStream_write_u32(b, Login_Key ^ login_xor);
		ByteStream_block_cipher(b);
		ByteStream_write_byte(b, 0);
		Connection_send(conn, b);
		ByteStream_dispose(b);
		break;
	}
	case 0x1A02:
		// some zero value
		ByteStream_read_u32(b);
		self->player->name = ByteStream_read_str(b);
		break;
	case 0x2C01: {
		// information to connect to game sock
		self->player->id = ByteStream_read_u32(b);
		char *ip = ByteStream_read_str(b);
		Connection_open(self->game_conn, ip, 5555);
		free(ip);
		if (self->api->on_connect) {
			self->api->on_connect(self->player);
		}

		b = ByteStream_new();
		ByteStream_write_u32(b, self->player->id);
		Connection_send(self->game_conn, b);
		ByteStream_dispose(b);
		break;
	}
	case 0x1C32:
		// this package appears to contain a lot of garbage
		// and it wants you to send back a bunch of garbage
		// we are going to ignore it now
		break;
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
		// who cares
		break;
	default:
		printf("%04x ", ccc);
		ByteStream_print(b);
	}
}

static inline void Bot_check_conn(Bot *self, Connection *conn)
{
	if (conn->sock == 0)
		return;
	int count;
	ioctl(conn->sock, FIONREAD, &count);
	if (count > 0) {
		ByteStream *b = ByteStream_new();
		ByteStream_read_sock(b, conn->sock);
		Bot_handle_packet(self, conn, ByteStream_read_u16(b), b);
		ByteStream_dispose(b);
	}
}

// these are pretty ugly, and are probably don't need to be accurate,
// but who cares

#define FONT_HASH "509651e4ebc76077067c4054a8c1888d9164be8fa5ff82292fd70fae2273f183"

#define FLASH_SERVER_STRING "A=t&SA=t&SV=t&EV=t&MP3=t&AE=t&VE=t&ACC=t&PR=t&SP=f&SB=f&DEB=f&V=WIN 25,0,0,127&M=Adobe Windows&R=1366x768&COL=color&AR=1.0&OS=Windows 8&ARCH=x86&L=en&IME=t&PR32=t&PR64=t&LS=en-US&PT=Desktop&AVD=f&LFD=f&WD=f&TLS=t&ML=5.1&DP=72"

void Bot_start(Bot *self)
{
	Connection_open(self->main_conn, HOST, PORT);
	// handshake packet
	ByteStream *b = ByteStream_new();
	ByteStream_write_u16(b, 0x1C01);
	ByteStream_write_u16(b, Handshake_Number);
	ByteStream_write_str(b, Handshake_String);
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
	while (true) {
		Bot_check_conn(self, self->main_conn);
		Bot_check_conn(self, self->game_conn);
	}
}

