#ifndef CONTROL_PANEL_H
#define CONTROL_PANEL_H

#include "common.h"

struct ControlPanel {
	sock_t sock;
};

struct ControlPanel *ControlPanel_new(sock_t);
bool ControlPanel_listen(struct ControlPanel *);
void ControlPanel_reply(struct ControlPanel *self, char *msg);

#endif
