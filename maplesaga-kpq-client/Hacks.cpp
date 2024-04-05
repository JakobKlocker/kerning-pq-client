#include "pch.h"
#include "Hacks.h"


TeleportFuncPtr TELEPORTFUNC_TELEPORTPLAYER = (TeleportFuncPtr)0x007F5330;
void teleportPlayer(DWORD posX, DWORD posY) {
	DWORD* edi = reinterpret_cast<DWORD*>(0x00978358);

	std::cout << "Inside and I gotta shit" << std::endl;
	DWORD* trigger = reinterpret_cast<DWORD*>(*edi + 0x1E94);
	DWORD* x = reinterpret_cast<DWORD*>(*edi + 0x1E9C);
	DWORD* y = reinterpret_cast<DWORD*>(*edi + 0x1EA0);


	*x = posX;
	*y = posY;
	*trigger = 1;
	//TELEPORTFUNC_TELEPORTPLAYER(*ptr, posX, posY);
}

DWORD JMPTO_AUTOROPEENABLE = 0x804761;
void __declspec(naked) autoRopeEnable_Assembly()
{
	__asm {
		jmp JMPTO_AUTOROPEENABLE
	}
}

DWORD JMPTO1_AUTOROPEDISABLE = 0x804873;
DWORD JMPTO2_AUTOROPEDISABLE = 0x804751;
void __declspec(naked) autoRopeDisable_Assembly()
{
	__asm {
		jnl greater_or_equal
		jmp JMPTO2_AUTOROPEDISABLE
		greater_or_equal :
		jmp JMPTO1_AUTOROPEDISABLE
	}
}


DWORD TMP_HOOKHPMP = 0;
BOOLEAN PAUSEATTACK = 0;
DWORD HPHOOKRET_HOOKMPHP = 0x00740CA4;
DWORD* randomMem = (DWORD*)malloc(3000);
DWORD* ebpSafe = 0;
void __declspec(naked) hookHpMp_Assembly() {

	__asm {
		mov eax, [esp]
		mov client.variables.charHp, eax
		mov eax, [esp + 8]
		mov client.variables.charMp, eax
		mov TMP_HOOKHPMP, 0x740F30
		call[TMP_HOOKHPMP]
		mov ecx, [ebp - 0x10]
		mov TMP_HOOKHPMP, 0x745827
		call[TMP_HOOKHPMP]
		test eax, eax
		pushad
		pushfd

		cmp client.variables.charMp, 300
		ja mpcheck
		mov PAUSEATTACK, 1
		mov esi, 0x978364
		mov esi, [esi]
		mov ecx, esi
		mov ebp, randomMem
		push 0x100
		push 0x1510000
		push 0x11
		push 0x100
		mov TMP_HOOKHPMP, 0x818D3E
		call[TMP_HOOKHPMP]
		mov PAUSEATTACK, 0
		mpcheck:
		cmp client.variables.charHp, 300
		ja done
		mov PAUSEATTACK, 1
		mov esi, 0x978364
		mov esi, [esi]
		mov ecx, esi
		mov ebp, randomMem
		push 0x100
		push 0x01490000
		push 0x11
		push 0x100
		mov TMP_HOOKHPMP, 0x818D3E
		call[TMP_HOOKHPMP]
		mov PAUSEATTACK, 0
		done:
		popfd
		popad

		jmp[HPHOOKRET_HOOKMPHP]
	}
}


char* BUFFER_HOOKSEND;
int PACKETLEN_HOOKSEND;
void* CALLER_HOOKSEND;
DWORD HOOKSENDRET_HOOKSEND = 0x04751B7;
void __declspec(naked) hookSend_Assembly()
{
	__asm {
		mov eax, [esp + 4]
		mov eax, [eax + 4]
		mov[BUFFER_HOOKSEND], eax
		mov eax, [esp + 4]
		mov eax, [eax + 8]
		mov PACKETLEN_HOOKSEND, eax
		mov eax, [esp]
		mov[CALLER_HOOKSEND], eax
		mov eax, 0x896984
		pushad
		pushfd
	}
	__asm {
		popfd
		popad
		jmp HOOKSENDRET_HOOKSEND
	}
}

DWORD RECVHOOK_HOOKRECV = 0x04754C1;
DWORD RECVHOOKRET_HOOKRECV = 0x04754C1 + 5;
DWORD DECODE1_HOOKRECV = 0x00424561;

unsigned char* RECVHOOKPRT_HOOKRECV;
DWORD PLENGHT_HOOKRECV = 0;
DWORD REQUIREDTICKETS = 0;
DWORD STAGECLEARED = 0;
void setCouponsNeeded()
{
	const unsigned char expectedHeader[] = { 0xED, 0x00 };
	const unsigned char stageSuccessPacket[] = { 0x68, 0x00, 0x03, 0x11, 0x00, 0x71, 0x75, 0x65, 0x73, 0x74, 0x2F, 0x70, 0x61, 0x72, 0x74, 0x79, 0x2F, 0x63, 0x6C, 0x65, 0x61, 0x72 };
	const unsigned char mapTransferHeader[] = { 0x1E, 0x00};
	const size_t headerSize = sizeof(expectedHeader);
	std::string str;
	if (std::memcmp(RECVHOOKPRT_HOOKRECV, stageSuccessPacket, sizeof(stageSuccessPacket)) == 0) {
		STAGECLEARED = 1;
	}
	if (std::memcmp(RECVHOOKPRT_HOOKRECV, mapTransferHeader, sizeof(mapTransferHeader)) == 0) {
		STAGECLEARED = 0;
	}
	if (std::memcmp(RECVHOOKPRT_HOOKRECV, expectedHeader, headerSize) == 0) {
		RECVHOOKPRT_HOOKRECV = RECVHOOKPRT_HOOKRECV + 10;
		str.assign((char*)RECVHOOKPRT_HOOKRECV, PLENGHT_HOOKRECV - 10);
		removeSubstring(str, "#b");
		removeSubstring(str, "#k");
		replaceSubstring(str, "\r\n", " ");

		std::cout << str << std::endl;

		int amount = checkCouponAmount(str);
		if (amount)
		{
			std::cout << "Tickets Requiered: " << amount << std::endl;
			REQUIREDTICKETS = amount;
		}
	}
}

int checkCouponAmount(std::string& const str)
{
	if (str.find("STR you receive after advancing as a Warrior") != std::string::npos)
		return 35;
	else if (str.find("DEX you receive after advancing as a Bowman") != std::string::npos)
		return 25;
	else if (str.find("INT you receive after advancing as a Magician") != std::string::npos)
		return 20;
	else if (str.find("DEX you receive after advancing as a Thief") != std::string::npos)
		return 25;
	else if (str.find("minimum level required to leave Maple Island") != std::string::npos)
		return 8;
	else if (str.find("minimum level required to advance to 2nd job") != std::string::npos)
		return 30;
	return 0;


}


void __declspec(naked) hookRecv_Assembly()
{
	__asm {
		push ecx
		push eax
		mov edx, [ecx + 0x14]
		mov eax, [ecx + 0x8]
		movzx ecx, word ptr[ecx + 0x0C]
		sub ecx, edx
		mov[PLENGHT_HOOKRECV], ecx
		add eax, edx
		mov[RECVHOOKPRT_HOOKRECV], eax
		cmp[PLENGHT_HOOKRECV], 271
		jne Process
		mov ecx, dword ptr[RECVHOOKPRT_HOOKRECV]
		mov al, [ecx]
		cmp al, 0xED
		je xorit
		Process :
		pop eax
			pop ecx
			call DECODE1_HOOKRECV
			jmp noxor
			xorit :
		pop eax
			pop ecx
			call DECODE1_HOOKRECV
			xor eax, eax
			noxor :
		pushfd
			pushad
	}
	setCouponsNeeded();
	__asm {
		popad
		popfd
		jmp RECVHOOKRET_HOOKRECV
	}
}

DWORD JMPTO_AUTONPC = 0x641950;
void __declspec(naked) autoNpc_Assembly()
{
	__asm {
		mov eax, 6
		jmp JMPTO_AUTONPC
	}
}


DWORD JMPTO_AUTONPCSECOND = 0x64180F;
void __declspec(naked) autoNpcSecond_Assembly()
{
	__asm {
		mov eax, 0x2001
		xor ebx, ebx
		xor edx, edx
		jmp JMPTO_AUTONPCSECOND
	}
}


DWORD GETCHARACTERDATA = 0x458C4F;
DWORD UNKNOWNCALL = 0x46575E;
DWORD GETDATA = 0x45E2EB;
DWORD CharDataPTR = 0x97E4B0;
unsigned char ZMap[250];
int getItemIdBySlot(int Tab, int Slot) {
	int itemID = 0;
	ZeroMemory(ZMap, 250);
	unsigned char* Zmap = ZMap;
	__asm {
		pushad
		push Slot //Slot
		push Tab //Tab Equip(1), Use(2), Set-Up(3), Etc(4), Cash(5)
		push Zmap
		mov ecx, [CharDataPTR]
		mov ecx, [ecx]
		cmp ecx, 0
		je done

		call GETCHARACTERDATA

		mov esi, [eax + 4]
		cmp esi, 0
		je done
		mov ecx, Zmap
		call UNKNOWNCALL

		lea edi, [esi + 0xC]
		mov ecx, edi
		call GETDATA

		mov[itemID], eax
		done :
		popad
	}
	return itemID;
}

DWORD CALLTOGETDATA = 0x84C94E;
int getItemCountByID(DWORD ItemID)
{
	int itemCount;
	__asm {
		mov ecx, 0x97C418
		push ItemID
		call CALLTOGETDATA
		mov[itemCount], eax
	}
	return itemCount;
}

DWORD JMPTOAIRBYPASS = 0x07AB3A9;
void __declspec(naked) airCheckMagicClaw_Assembly() {
	__asm {
		jmp JMPTOAIRBYPASS
	}
}

void sendPacket(std::string& packetStr)
{
	std::vector<unsigned char> bytes = hexStringToBytes(packetStr);
	unsigned char* packet = bytes.data();
	std::cout << "Sendpacket called with: " << packetStr << std::endl;
	callSendPacket(packet, bytes.size());
}


void detour(void* src, void* dest, int len)
{
	std::cout << "In detour" << std::endl;
	DWORD  curProtection;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

	DWORD how_far = (DWORD)dest - (DWORD)src - 5;
	memset(src, 0x90, len);
	*(char*)src = (char)'\xE9';
	*(DWORD*)((DWORD)src + 1) = how_far;
	VirtualProtect(src, len, curProtection, &curProtection);
	std::cout << "detour done" << std::endl;
}

ULONG CLIENTSOCKETADDR_CALLSENDPACKET = 0x9784C4;
PVOID* CLIENTSOCKET_CALLSENDPACKET = reinterpret_cast<PVOID*>(CLIENTSOCKETADDR_CALLSENDPACKET);
void* SENDHOOK_CALLSENDPACKET = (void*)0x4751B2;
PacketSend SEND_CALLSENDPACKET = reinterpret_cast<PacketSend>(SENDHOOK_CALLSENDPACKET);
void callSendPacket(BYTE packet[], int size)
{
	COutPacket Packet;
	SecureZeroMemory(&Packet, sizeof(COutPacket));
	Packet.Size = size;
	Packet.data = (void*)packet;
	SEND_CALLSENDPACKET(*CLIENTSOCKET_CALLSENDPACKET, &Packet);
}

PressTest PRESSKEY_CALLPRESSBUTTON = (PressTest)0x00818D3E;
std::atomic<bool> autoAttackOn_callPressButton(false);
void callAutoAttack()
{
	DWORD* base = *(DWORD**)0x00978364;
	DWORD* randomMem = (DWORD*)malloc(3000);
	DWORD keyButton = 0x1D0030;

	autoAttackOn_callPressButton = true;

	while (autoAttackOn_callPressButton)
	{
		if (PAUSEATTACK)
			Sleep(150);
		if (!PAUSEATTACK)
			PRESSKEY_CALLPRESSBUTTON(base, randomMem, 0x100, keyButton, 0x100);
		Sleep(50);
	}
	free(randomMem);
	ExitThread(1);
}


std::atomic<bool> autoLootOn_callPressButton(false);
void callAutoLoot()
{
	DWORD* base = *(DWORD**)0x00978364;
	DWORD* randomMem = (DWORD*)malloc(3000);
	DWORD keyButton = 0x2C0000;

	autoLootOn_callPressButton = true;

	while (autoLootOn_callPressButton)
	{
		if (PAUSEATTACK)
			Sleep(150);
		if (!PAUSEATTACK)
			PRESSKEY_CALLPRESSBUTTON(base, randomMem, 0x100, keyButton, 0x100);
		Sleep(50);
	}
	free(randomMem);
	ExitThread(1);
}

void callEnterPortal()
{
	DWORD* base = *(DWORD**)0x00978364;
	DWORD* randomMem = (DWORD*)malloc(3000);
	DWORD keyButton = 0x1480000;
	STAGECLEARED = 0;
	PRESSKEY_CALLPRESSBUTTON(base, randomMem, 0x100, keyButton, 0x100);
}


// below not needed anymore, prepaired + called function directly in c++
//unsigned char** WRAPPER_SENDPACKET;
//void __declspec(naked) sendPacket_Assembly(unsigned char*& packet, int len, BYTE*& CoutPacket) {
//    __asm {
//        mov eax, [esp + 4]
//        mov eax, [eax]
//        mov packet, eax
//
//        mov eax, [esp + 8]
//        mov len, eax
//
//        mov eax, [esp + 0xC]
//        mov CoutPacket, eax
//    }
//    WRAPPER_SENDPACKET = &packet;
//    __asm {
//        mov eax, CoutPacket
//        mov ecx, WRAPPER_SENDPACKET
//        mov[eax + 0x4], ecx
//        mov ecx, len
//        mov[eax + 0x8], ecx
//        push eax
//        mov eax, 0x4751B2
//        mov ecx, 0x9784C4
//        mov ecx, [ecx]
//        call eax
//        ret
//    }
//}