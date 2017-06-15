#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include "common.h"

byte Hash_Key[20];
uint32_t Login_Key;
uint32_t Handshake_Number;
char Handshake_String[20];

uint32_t Identification_Key[20];
uint32_t Msg_Key[20];

void init_keys(void);

#endif
