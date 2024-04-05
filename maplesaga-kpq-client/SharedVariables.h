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
	DWORD charHp;
	DWORD charMp;
	Point mob;
	DWORD mobCount;
	Point item;
	DWORD itemCount;
	DWORD mapId;
	DWORD currentTickets;
	DWORD ticketsNeeded;
	DWORD itemID;
	DWORD itemCountByID;
	bool isPartyLeader;
	DWORD stageCleared;
};
