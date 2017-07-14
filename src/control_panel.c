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
	return self;
}

#define cmd_chk(a, b, c)(header[0] == a && header[1] == b && header[2] == c)
#define cmd_yield(s) do { ControlPanel_reply(self, s); return true; } while(0);
#define sel_bot()(bots_running[self->bot_index])

bool ControlPanel_listen(struct ControlPanel *self)
{
	// offset 0 - 2 : cmd
	// offset 3 - 4 : length of message
	// offset 5+    : message
	byte header[5];
	sock_block_read(self->sock, header, 5);
	uint16_t len = header[3] | (header[4] << 8);
	char msg[len + 1];
	sock_block_read(self->sock, msg, len);
	msg[len] = '\0';
	// this is probably the ugliest control flow
	if (cmd_chk('s', 'a', 'y')) {
		Bot_send_chat(sel_bot(), msg);
		cmd_yield(NULL);
	} else if (cmd_chk('c', 'm', 'd')) {
		Bot_send_command(sel_bot(), msg);
		cmd_yield(NULL);
	} else if (cmd_chk('s', 'e', 'l')) {
		int n;
		if (sscanf(msg, "%d", &n) != 1)
			cmd_yield("Integer expected");
		if (n < 0 || n >= num_bots_running)
			cmd_yield("Out of range");
		self->bot_index = n;
		cmd_yield(NULL);
	} else if (cmd_chk('l', 's', 't')) {
		char buf[64];
		sprintf(buf, "%d bots running", num_bots_running);
		cmd_yield(buf);
	} else if (cmd_chk('b', 'o', 't')) {
		char buf[128];
		sprintf(buf, "Player #%u %s; Room : %s", sel_bot()->player->id,
				sel_bot()->player->name, sel_bot()->room->name);
		cmd_yield(buf);
	} else if (cmd_chk('w', 'h', 'o')) {
		cmd_yield(":(((((");
	} else if (sel_bot()->api->on_control) {
		char cmd[4];
		int i;
		for (i = 0; i < 3; i++)
			cmd[i] = header[i];
		cmd[i] = '\0';
		if (! sel_bot()->api->on_control(sel_bot(), cmd, msg))
			cmd_yield("Command not found");
	} else {
		cmd_yield("Command not found");
	}
	cmd_yield("Error, command never yields");
}

void ControlPanel_reply(struct ControlPanel *self, char *msg)
{
	byte buf[2];
	if (msg == NULL) {
		buf[0] = 0;
		buf[1] = 0;
		write(self->sock, buf, 2);
	} else {
		int len = strlen(msg);
		buf[0] = len & 0xff;
		buf[1] = (len >> 8) & 0xff;
		sock_write(self->sock, buf, 2);
		if (len)
			sock_write(self->sock, msg, len);
	}
}
