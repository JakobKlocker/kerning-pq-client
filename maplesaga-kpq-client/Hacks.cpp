#include "pch.h"
#include "Hacks.h"



DWORD TMP_HOOKHPMP = 0;
DWORD HPHOOKRET_HOOKMPHP= 0x00740CA4;
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

        cmp client.variables.charMp, 150
        ja mpcheck
        mov ecx, 0x978358
        mov ecx, [ecx]
        push 0x01510000
        push 0x00000022
        mov TMP_HOOKHPMP, 0x007A4711
        call [TMP_HOOKHPMP]

        mov ecx, 0x978358
        mov ecx, [ecx]
        push 0x81510000
        push 0x00000022
        mov TMP_HOOKHPMP, 0x007A4711
        call[TMP_HOOKHPMP]

        mpcheck:
        cmp client.variables.charHp, 150
        ja done
        mov ecx, 0x978358
        mov ecx, [ecx]
        push 0x01490000
        push 0x00000021
        mov TMP_HOOKHPMP, 0x007A4711
        call[TMP_HOOKHPMP]
        mov ecx, 0x978358
        mov ecx, [ecx]
        push 0x81490000
        push 0x00000021
        mov TMP_HOOKHPMP, 0x007A4711
        call[TMP_HOOKHPMP]

        done:
        popfd
        popad

        jmp[HPHOOKRET_HOOKMPHP]
    }
}


char* BUFFER_HOOKSEND;
int PACKETLEN_HOOKSEND;
void* CALLER_HOOKSEND;
DWORD HOOKSENDRET_HOOKSEND= 0x04751B7;
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
void setCouponsNeeded()
{
    const unsigned char expectedHeader[] = { 0xED, 0x00 };
    const size_t headerSize = sizeof(expectedHeader);
    int result = std::memcmp(RECVHOOKPRT_HOOKRECV, expectedHeader, headerSize);

    std::string str;

    if (result == 0) {
        RECVHOOKPRT_HOOKRECV = RECVHOOKPRT_HOOKRECV + 10;
        str.assign((char *)RECVHOOKPRT_HOOKRECV, PLENGHT_HOOKRECV - 10);
        removeSubstring(str, "#b");
        removeSubstring(str, "#k");
        replaceSubstring(str, "\r\n", " ");

        std::cout << str << std::endl;

        int amount = checkCouponAmount(str);
        if (amount)
        {
            std::cout << "Tickets Requiered: " << amount << std::endl;
            client.variables.ticketsNeeded = amount;
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
        noxor:
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