#include "pch.h"
#include "Hacks.h"


ULONG clientSocketAddr = 0x9784C4;


typedef void(__thiscall* PacketSend)(PVOID clientSocket, COutPacket* packet);
PVOID* ClientSocket = reinterpret_cast<PVOID*>(clientSocketAddr);
void* sendHook = (void*)0x4751B2;
PacketSend Send = reinterpret_cast<PacketSend>(sendHook);

void callSendPacket(BYTE packet[], int size);


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
int i = 0;
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

void callSendPacket(BYTE packet[], int size)
{
    COutPacket Packet;
    SecureZeroMemory(&Packet, sizeof(COutPacket));
    Packet.Size = size;
    Packet.data = (void*)packet;
    Send(*ClientSocket, &Packet);
}