#pragma once
#include <iomanip>

#include "generalFunctions.h"
#include "Client.h"

extern Client client;

void hookHpMp_Assembly();
void hookSend_Assembly();
void sendPacket_Assembly(unsigned char* &packet, int len, BYTE*& CoutPacket);

void sendPacket(std::string& packetStr);
void detour(void* src, void* dest, int len);

struct COutPacket {
	int zero;
	void* data;
	int Size;
	int	pad;
	int pad_1;
};


