#include "control_panel.h"
#include "connection.h"
#include <unistd.h>
#include <string.h>

struct ControlPanel *ControlPanel_new(sock_t sock)
{
	struct ControlPanel *self = (struct ControlPanel*)malloc(sizeof(struct ControlPanel));
	if (sock == 0)
		fatal("Attempted to create ControlPanel on NULL sock");
	self->sock = sock;
	return self;
}

bool ControlPanel_listen(struct ControlPanel *self)
{
	byte l[2];
	sock_read_byte(self->sock, l);
	sock_read_byte(self->sock, l + 1);
	uint16_t len = l[0] | (l[1] << 8);
	byte buf[len + 1];
	sock_block_read(self->sock, buf, len);
	buf[len] = 0;
	puts((char*)buf);
	ControlPanel_reply(self, NULL);
	return true;
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
		write(self->sock, buf, 2);
		if (len)
			write(self->sock, msg, len);
	}
}
