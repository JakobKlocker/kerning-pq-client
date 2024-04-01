#include "pch.h"
#include "Client.h"



Client::Client()
{
	DWORD pId = GetCurrentProcessId();
	if (!pId)
	{
		//Write in log file XX failed if DEBUG is on
		std::exit(1);
	}
	this->variables.processId = pId;
	this->createMappingHandle();
}


// See https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
int Client::createMappingHandle()
{
	std::wstring mappingName = L"client";
	wchar_t clientNumber = L'1';

	HANDLE hExistingCheck;

	do {
		hExistingCheck = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,
			FALSE,
			(mappingName + clientNumber).c_str());
		clientNumber++;
	} while (hExistingCheck != NULL);
	clientNumber--;

	this->hMappedFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(this->variables),
		(mappingName + clientNumber).c_str());


	if (this->hMappedFile == NULL)
	{
		//Write in log file XX failed if DEBUG is on
		//GetLastError();
		return 1;
	}

	return 0;
}


//https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code

int PORT = 1337;
int Client::runServer()
{
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET clientsock = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iResult;
	char recvbuf[500];

	std::cout << "running server..." << std::endl;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return -1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	restartSocket:
	do {
		iResult = getaddrinfo(NULL, std::to_string(PORT).c_str(), &hints, &result);
		std::cout << "trying port: " << PORT << " iResult:" << iResult << std::endl;
		PORT++;

		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return -1;
		}

		// Create a SOCKET for the server to listen for client connections.
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return -1;
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
		}
	} while (iResult != 0 && PORT <= 1337 + 2);
	PORT--;

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return -1;
	}

	// Accept a client socket
	clientsock = accept(ListenSocket, NULL, NULL);
	if (clientsock == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Client connected to Server\n";

	// No longer need server socket
	closesocket(ListenSocket);
	ZeroMemory(recvbuf, 500);
	do {
		iResult = recv(clientsock, recvbuf, 500, 0);
		if (iResult != 0 && iResult < 4096 && iResult > 0)
		{
			recvbuf[iResult] = '\0';
			std::cout << "Received: " << recvbuf << std::endl;
			determineAction(recvbuf);
		}
		Sleep(5);
	} while (iResult != 0 && iResult != 0xffffffff); // If Connection lost iResult = 0xffffffff
	std::cout << "Connection to Client lost.\n";
	goto restartSocket;
	return (0);
}

int ItemID = 0;
int ItemCountID = 0;
void Client::determineAction(const std::string& receivedStr)
{
	std::istringstream iss(receivedStr);
	std::vector<std::string> words {
		std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>{}
	};

	if (words.empty()) {
		std::cerr << "Received empty string.\n";
		return;
	}

	// Determine the action based on the first word
	const std::string& action = words[0];
	if (action == "sendpacket") {
		if (words.size() >= 2) {
			std::string restOfTheString = "";
			for (size_t i = 1; i < words.size(); ++i) {
				restOfTheString += words[i];
			}
			sendPacket(restOfTheString);
		}
		else {
			std::cerr << "Incomplete sendpacket command.\n";
		}
	}
	else if (action == "teleport") {
		if (words.size() == 3) { // Check if there are enough arguments
			int x, y;
			std::istringstream(words[1]) >> x && std::istringstream(words[2]) >> y;
			teleportPlayer(x, y);
		}
		else {
			std::cerr << "Incomplete teleport command.\n";
		}
	}
	else if (action == "rope") {
		std::cout << "Size: " << words.size() << std::endl;
		if (words.size() == 2) { // Check if there are enough arguments
			if (words[1] == "True")
				detour((char*)0x80474C, autoRopeEnable_Assembly, 6);
			else
				detour((char*)0x80474C, autoRopeDisable_Assembly, 6);
		}
		else {
			std::cerr << "Incomplete rope command.\n";
		}
	}
	else if (action == "attack")
	{
		if (words.size() == 2) { // Check if there are enough arguments
			if (words[1] == "True")
				CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)callAutoAttack, nullptr, 0, nullptr);
			else
			{
				autoAttackOn_callPressButton = false;
				std::cout << "Auto Attack Disabled" << std::endl;
			}
		}
	}
	else if (action == "loot")
	{
		if (words.size() == 2) { // Check if there are enough arguments
			if (words[1] == "True")
			{
				std::cout << "True Loot..." << std::endl;
				CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)callAutoLoot, nullptr, 0, nullptr);
			}
			else
			{
				autoLootOn_callPressButton = false;
				std::cout << "Auto Loot Disabled" << std::endl;
			}
		}
	}
	else if (action == "getitemid") {
		if (words.size() == 3) { // Check if there are enough arguments
			int tab, slot;
			std::istringstream(words[1]) >> tab && std::istringstream(words[2]) >> slot;
			ItemID = getItemIdBySlot(tab, slot);
		}
	}
	else if (action == "getcount") {
		if (words.size() == 2) { // Check if there are enough arguments
			int itemID;
			std::istringstream(words[1]) >> itemID;
			ItemCountID = getItemCountByID(itemID);
		}
	}
	else if (action == "portal")
	{
		callEnterPortal();
	}
	else {
		std::cerr << "Unknown action: " << action << std::endl;
	}
}


void Client::mapVariableStructure()
{
	LPCTSTR pBuf;

	pBuf = (LPCTSTR)MapViewOfFile(this->hMappedFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(this->variables));

	if (pBuf == NULL)
	{
		std::cout << "Unable to map file" << std::endl;
		return;
	}

	CopyMemory((PVOID)pBuf, &this->variables, sizeof(this->variables));

	UnmapViewOfFile(pBuf);
}

void Client::getMapId()
{
	DWORD* mapId = *(DWORD**)0x979268;
	mapId = reinterpret_cast<DWORD*>(reinterpret_cast<char*>(mapId) + 0x62C);
	this->variables.mapId = *mapId;
	this->variables.TCPPort = PORT;
	this->variables.itemID = ItemID;
	this->variables.itemCountByID = ItemCountID;
	this->variables.ticketsNeeded = REQUIREDTICKETS;
}

void Client::getNextMobXY()
{
	DWORD mobX = 0;
	DWORD mobY = 0;
	DWORD* mob = *(DWORD**)0x97F57C;

	mob = *reinterpret_cast<DWORD**>(reinterpret_cast<char*>(mob) + 0x28);
	if (!mob)
		return;

	mob = *reinterpret_cast<DWORD**>(reinterpret_cast<char*>(mob) + 0x4);
	if (!mob)
		return;

	mobX = *reinterpret_cast<DWORD*>(reinterpret_cast<char*>(mob) + 0x480);
	mobY = *reinterpret_cast<DWORD*>(reinterpret_cast<char*>(mob) + 0x484);

	this->variables.mob.x = mobX;
	this->variables.mob.y = mobY;

	//std::cout << "MobX: " << mobX << "MobY: " << mobY << std::endl;
}

void Client::getPlayerXY()
{
	DWORD* playerBase = *(DWORD**)0x979268;
	if (!playerBase)
		return;

	DWORD playerX = 0;
	DWORD playerY = 0;

	playerX = *reinterpret_cast<DWORD*>(reinterpret_cast<char*>(playerBase) + 0x59C);
	playerY = *reinterpret_cast<DWORD*>(reinterpret_cast<char*>(playerBase) + 0x5A0);

	this->variables.character.x = playerX;
	this->variables.character.y = playerY;


	//std::cout << "Char X: " << playerX << std::endl;
}

void Client::getMobCount()
{
	DWORD* mob = *(DWORD**)0x97F57C;
	this->variables.mobCount = *reinterpret_cast<DWORD*>(reinterpret_cast<char*>(mob) + 0x24);

	//std::cout << "Mob Count: " << this->variables.mobCount << std::endl;
}


typedef int(__thiscall* Decrypt)(int* addr);
void* Decrypt_Addr = (void*)0x45E2EB;
Decrypt CallDecrypt = reinterpret_cast<Decrypt>(Decrypt_Addr);
void Client::getItemXY()
{
	DWORD* item = *(DWORD**)0x9791D0;
	DWORD* itemXPtr;
	DWORD* itemYPtr;
	DWORD itemX;
	DWORD itemY;

	if (!item)
		return;

	item = *reinterpret_cast<DWORD**>(reinterpret_cast<char*>(item) + 0x2C);
	if (!item)
		return;

	item = *reinterpret_cast<DWORD**>(reinterpret_cast<char*>(item) + 0x4);
	if (!item)
		return;

	// call decrypt function with item + 68 for Y and item + 74 for X
	itemXPtr = reinterpret_cast<DWORD*>(reinterpret_cast<char*>(item) + 0x74);
	if (!itemXPtr)
		return;

	itemYPtr = reinterpret_cast<DWORD*>(reinterpret_cast<char*>(item) + 0x68);
	if (!itemYPtr)
		return;

	itemX = CallDecrypt((int*)itemXPtr);
	itemY = CallDecrypt((int*)itemYPtr);

	this->variables.item.x = itemX;
	this->variables.item.y = itemY;

	//std::cout << "itemX: "  << itemX << "   ItemY: " << itemY << std::endl;
}

void Client::getItemCount()
{
	DWORD* item = *(DWORD**)0x9791D0;
	this->variables.itemCount = *reinterpret_cast<DWORD*>(reinterpret_cast<char*>(item) + 0x28);

	//std::cout << "ItemCount: " << this->variables.itemCount << std::endl;
}

void Client::UpdateAllVariablesLoop()
{
	while (1)
	{
		getMapId();
		getNextMobXY();
		getItemXY();
		getItemCount();
		getPlayerXY();
		getMobCount();
		this->mapVariableStructure();
		Sleep(50);
	}
}