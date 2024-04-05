#pragma once
#ifndef Hacks_H
#define Hacks_H
#include <iomanip>

#include "generalFunctions.h"
#include "Client.h"

extern Client client;
void hookHpMp_Assembly();
void hookSend_Assembly();

void hookRecv_Assembly();
void autoNpc_Assembly();
void autoNpcSecond_Assembly();
void sendPacket_Assembly(unsigned char* &packet, int len, BYTE*& CoutPacket);
void autoRopeDisable_Assembly();
void autoRopeEnable_Assembly();
void airCheckMagicClaw_Assembly();

void sendPacket(std::string& packetStr);
void detour(void* src, void* dest, int len);
void callSendPacket(BYTE packet[], int size);
int checkCouponAmount(std::string& const str);
void teleportPlayer(DWORD posX, DWORD posY);
void callAutoAttack();
void callAutoLoot();
void callEnterPortal();
int getItemCountByID(DWORD ItemID);
int getItemIdBySlot(int Tab, int Slot);
struct COutPacket {
	int zero;
	void* data;
	int Size;
	int	pad;
	int pad_1;
};

typedef void(__thiscall* PacketSend)(PVOID clientSocket, COutPacket* packet);
typedef int(__stdcall* TeleportFuncPtr)(int, DWORD, DWORD);

typedef void(__stdcall* PressKey)(int, int);

extern DWORD REQUIREDTICKETS;
extern DWORD STAGECLEARED;
#endif