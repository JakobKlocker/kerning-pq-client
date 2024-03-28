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

	//LPCTSTR pBuf;


	//pBuf = (LPTSTR)MapViewOfFile(this->hMappedFile,   // handle to map object
	//	FILE_MAP_ALL_ACCESS, // read/write permission
	//	0,
	//	0,
	//	sizeof(this->variables));

	//this->variables.processId = 1337;

	//CopyMemory((PVOID)pBuf, &this->variables, (sizeof(this->variables)));


	return 0;
}


//https://learn.microsoft.com/en-us/windows/win32/winsock/complete-client-code
int Client::runServer()
{
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET clientsock = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iResult;
	int port = 1337;
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


	do {
		iResult = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &result);
		std::cout << "trying port: " << port << " iResult:" << iResult << std::endl;
		port++;

		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			return -1;
		}

		// Save correct Port in structure
		
		this->variables.TCPPort = port - 1;

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
	} while (iResult != 0 && port <= 1337 + 2);

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
	return (0);
}

void Client::determineAction(const std::string &receivedStr)
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
			sendPacket(words[1]);
		}
		else {
			std::cerr << "Incomplete sendpacket command.\n";
		}
	}
	else {
		std::cerr << "Unknown action: " << action << std::endl;
	}
}