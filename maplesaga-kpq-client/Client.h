#pragma once
#include <Windows.h>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <functional>


#pragma comment(lib,"WS2_32")

class Client;

#include "Hacks.h"
#include "SharedVariables.h"

extern std::atomic<bool> autoAttackOn_callPressButton;

class Client
{
private:
	HANDLE hMappedFile;
	void determineAction(const std::string& receivedStr);
public:
	SharedVariables variables;
	Client();
	int createMappingHandle();
	int runServer();
	void mapVariableStructure();

	//Get all needed data for structure
	void UpdateAllVariablesLoop();


	void getMapId();
	void getNextMobXY();
	void getPlayerXY();
	void getMobCount();
	void getItemXY();
	void getItemCount();

	void enableAutoAttack();
};
