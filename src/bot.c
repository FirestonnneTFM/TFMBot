#include "bot.h"
#include "key_manager.h"
#include "byte_stream.h"

#define HOST "164.132.202.12"
//#define HOST "127.0.0.1"
#define PORT 5555

Bot *Bot_new(void)
{
	Bot *self = (Bot*)malloc(sizeof(Bot));
	self->sock = 0;
	self->key_offset = 0;
	return self;
}

void Bot_send(Bot *self, ByteStream *b, byte *k)
{
	ByteStream_write_sock(b, self->sock, *k);
	*k = (*k + 1) % 100;
}

static inline void Bot_handle_packet(Bot *self, uint16_t ccc, ByteStream *b)
{
	switch (ccc) {
	case 0x1A03: {
		uint32_t players_online = ByteStream_read_u32(b);
		self->key_offset = ByteStream_read_byte(b);
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
		Bot_send(self, b, &self->key_offset);
		ByteStream_dispose(b);
		
		b = ByteStream_new();
		ByteStream_write_u16(b, 0x1C11);
		ByteStream_write_str(b, "en");
		ByteStream_write_str(b, "Windows 8");
		ByteStream_write_str(b, "WIN 25,0,0,127");
		ByteStream_write_byte(b, 0);
		Bot_send(self, b, &self->key_offset);
		ByteStream_dispose(b);

		b = ByteStream_new();
		ByteStream_write_u16(b, 0x1C11);
		ByteStream_write_str(b, "en");
		ByteStream_write_str(b, "Windows 8");
		ByteStream_write_str(b, "WIN 25,0,0,127");
		ByteStream_write_byte(b, 0);
		Bot_send(self, b, &self->key_offset);
		ByteStream_dispose(b);

		b = ByteStream_new();
		ByteStream_write_u16(b, 0x1A08);
		ByteStream_write_str(b, "Souris");
		ByteStream_write_str(b, NULL);
		ByteStream_write_str(b, "app:/TransformiceAIR.swf/[[DYNAMIC]]/2/[[DYNAMIC]]/4");
		ByteStream_write_str(b, "village gogogo");
		ByteStream_write_u32(b, Login_Key ^ login_xor);
		ByteStream_block_cipher(b);
		ByteStream_write_byte(b, 0);
		Bot_send(self, b, &self->key_offset);
		ByteStream_dispose(b);
		break;
	}
	case 0x1404:
	case 0x1009:
		// who cares
		break;
	default:
		printf("%04x ", ccc);
		ByteStream_print(b);
	}
}

// these are pretty ugly, and are probably don't need to be accurate,
// but who cares

#define FONT_HASH "509651e4ebc76077067c4054a8c1888d9164be8fa5ff82292fd70fae2273f183"

#define FLASH_SERVER_STRING "A=t&SA=t&SV=t&EV=t&MP3=t&AE=t&VE=t&ACC=t&PR=t&SP=f&SB=f&DEB=f&V=WIN 25,0,0,127&M=Adobe Windows&R=1366x768&COL=color&AR=1.0&OS=Windows 8&ARCH=x86&L=en&IME=t&PR32=t&PR64=t&LS=en-US&PT=Desktop&AVD=f&LFD=f&WD=f&TLS=t&ML=5.1&DP=72"

void Bot_start(Bot *self)
{
	self->sock = open_sock(HOST, PORT);
	// handshake packet
	ByteStream *hp = ByteStream_new();
	ByteStream_write_u16(hp, 0x1C01);
	ByteStream_write_u16(hp, Handshake_Number);
	ByteStream_write_str(hp, Handshake_String);
	ByteStream_write_str(hp, "Desktop");
	ByteStream_write_str(hp, "-");
	ByteStream_write_u32(hp, 0x00001FBD);
	ByteStream_write_str(hp, NULL);
	ByteStream_write_str(hp, FONT_HASH);
	ByteStream_write_str(hp, FLASH_SERVER_STRING);
	ByteStream_write_u32(hp, 0);
	ByteStream_write_u32(hp, 0x00006257);
	ByteStream_write_str(hp, NULL);
	Bot_send(self, hp, &self->key_offset);
	ByteStream_dispose(hp);
	while (true) {
		hp = ByteStream_new();
		ByteStream_read_sock(hp, self->sock);
		Bot_handle_packet(self, ByteStream_read_u16(hp), hp);
		ByteStream_dispose(hp);
	}
}

