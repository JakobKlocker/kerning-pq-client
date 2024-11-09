// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"
#include "dllmain.h"

HWND hCurWnd = NULL;
HWND hCurWndCorrect = NULL;

void GetAllWindowsFromProcessID(DWORD dwProcessID)
{
    // find all hWnds (vhWnds) associated with a process id (dwProcessID)
    
    do
    {
        hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
        DWORD dwProcID = 0;
        GetWindowThreadProcessId(hCurWnd, &dwProcID);
        if (dwProcID == dwProcessID)
        {
            hCurWndCorrect = hCurWnd;  // add the found hCurWnd to the vector
            wprintf(L"Found hWnd %d\n", hCurWnd);
        }
    } while (hCurWnd != NULL);
}

Client client;
void passStuff(char* buffer, int len)
{
    std::cout << "Bytes in memory:" << std::endl;
    for (int i = 0; i < len; i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::endl;
}

int mainThread() 
{
    //Create Console
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w+", stdout);
    //freopen_s(&f, "CONIN$", "r", stdin);

    //std::cout << "Test" << std::endl;

    GetAllWindowsFromProcessID(GetCurrentProcessId());

    detour((char*)0x740C95, hookHpMp_Assembly, 5);
    //detour((char*)0x4751B2, hookSend_Assembly, 5);
    detour((char*)0x04754C1, hookRecv_Assembly, 5);
    detour((char*)0x641940, autoNpc_Assembly, 5);
    detour((char*)0x6417FF, autoNpcSecond_Assembly, 5);
    detour((char*)0x7AB37E, airCheckMagicClaw_Assembly, 6);


    std::thread mainThread(&Client::UpdateAllVariablesLoop, client);
    std::thread secondThread(&Client::runServer, client);

    while (1)
    {
        Sleep(1500);
    }



    return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)mainThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

