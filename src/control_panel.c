#include "control_panel.h"
#include "connection.h"
#include <string.h>
#include "bot.h"

struct ControlPanel *ControlPanel_new(sock_t sock)
{
	struct ControlPanel *self = (struct ControlPanel*)malloc(sizeof(struct ControlPanel));
	if (sock == 0)
		fatal("Attempted to create ControlPanel on NULL sock");
	self->sock = sock;
	self->bot_index = 0;
	self->chat_mode = 'r';
	self->chat_target = "us";
	return self;
}

#define cmd_yield(s) do { ControlPanel_reply(self, 0, s); return true; } while(0)
#define cmd_get_int(n) do { if (sscanf(msg, "%d", n) != 1) cmd_yield("Integer expected"); } while (0)
#define sel_bot()(bots_running[self->bot_index])

bool ControlPanel_listen(struct ControlPanel *self)
{
	// offset 0 - 2 : cmd
	// offset 3 - 4 : length of message
	// offset 5+    : message
	byte header[5];
	sock_block_read(self->sock, header, 5);
	uint32_t cmd = header[2] | (header[1] << 8) | (header[0] << 16);
	uint16_t len = header[3] | (header[4] << 8);
	char msg[len + 1];
	sock_block_read(self->sock, msg, len);
	msg[len] = '\0';
	if (sel_bot()->api->on_control && sel_bot()->api->on_control(sel_bot(), self, cmd, msg)) {
		return true;
	}
	switch (cmd) {
	case 'say':
		switch (self->chat_mode) {
		case 'r':
			Bot_send_chat(sel_bot(), msg);
			cmd_yield(NULL);
		case 'c':
			Bot_send_cp_chat(sel_bot(), self->chat_target, msg);
			cmd_yield(NULL);
		default:
			cmd_yield("Bad target");
		}
	case 'cmd':
		Bot_send_command(sel_bot(), msg);
		cmd_yield(NULL);
	case 'sel': {
		if (msg[0] == '\0') {
			char buf[32];
			sprintf(buf, "%d bots running", num_bots_running);
			cmd_yield(buf);
		}
		int n;
		cmd_get_int(&n);
		if (n < 0 || n >= num_bots_running)
			cmd_yield("Out of range");
		self->bot_index = n;
		cmd_yield(NULL);
	}
	case 'bot': {
		char buf[128];
		sprintf(buf, "[%u] %s; Room : %s", sel_bot()->player->id,
				sel_bot()->player->name, sel_bot()->room->name);
		cmd_yield(buf);
	}
	case 'cht':{
		if (msg[0] == '\0') {
			Bot_join_cp_chat(sel_bot(), self->chat_target);
		} else {
			Bot_join_cp_chat(sel_bot(), msg);
			if (msg[0] != '#')
				cmd_yield("You might be missing a '#' in the name");
		}
		self->chat_mode = 'c';
		cmd_yield(NULL);
	}
	case 'who':
		cmd_yield(":(");
	case 'cap':
		Bot_do_register(sel_bot(), msg);
		cmd_yield(NULL);
	case 'tar': {
		if (msg[0] == '\0') {
			char buf[32];
			sprintf(buf, "Chat target is %c", self->chat_mode);
			cmd_yield(buf);
		} else {
			self->chat_mode = msg[0];
			cmd_yield(NULL);
		}
	}
	case 'kil':
		puts("Control is killing process");
		ControlPanel_reply(self, 1, "Dead");
		close(self->sock);
		exit(0);
		return false;
	case 'rom':
		Bot_change_room(sel_bot(), msg);
		cmd_yield(NULL);
	default:
		cmd_yield("Command not found");
	}
}

void ControlPanel_reply(struct ControlPanel *self, byte status, char *msg)
{
	byte buf[3];
	buf[0] = status;
	if (msg == NULL) {
		buf[1] = 0;
		buf[2] = 0;
		write(self->sock, buf, 3);
	} else {
		int len = strlen(msg);
		buf[1] = len & 0xff;
		buf[2] = (len >> 8) & 0xff;
		sock_write(self->sock, buf, 3);
		if (len)
			sock_write(self->sock, msg, len);
	}
}
