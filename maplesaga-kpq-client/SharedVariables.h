#pragma once
#include <string>
#include <Windows.h>

struct Point {
	DWORD x;
	DWORD y;
};

struct SharedVariables {
	DWORD processId;
	DWORD TCPPort;
	Point character;
	Point mob;
	Point item;
	DWORD itemCount;
	DWORD charHp;
	DWORD charMp;
	DWORD mapId;
	DWORD currentTickets;
	DWORD ticketsNeeded;
	bool isPartyLeader;
};
