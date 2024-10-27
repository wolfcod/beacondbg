#include <Windows.h>
#include <stdint.h>

#include "loader.h"

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
    }

    return TRUE;
}

struct ThreadBeaconData
{
    HANDLE hThread;
    DWORD dwThreadId;

    HANDLE hBeacon;
    LPCSTR lpProcName;
    LPVOID lpData;
    SIZE_T Size;
};

static DWORD WINAPI ThreadBeacon(LPVOID lpParameter)
{
    ThreadBeaconData* tbd = (ThreadBeaconData*)lpParameter;

    int r = RunCOFF((bof_fd*)tbd->hBeacon, (char*)tbd->lpProcName, (unsigned char*)tbd->lpData, (int)tbd->Size);

    CloseHandle(tbd->hThread);
    return 0;
}

BOOL WINAPI RunBeacon(HANDLE hBeacon, LPCSTR lpProcName, LPVOID lpData, SIZE_T Size)
{
    if (hBeacon != NULL) {
        ThreadBeaconData* tbd = (ThreadBeaconData*)calloc(1, sizeof(ThreadBeaconData));
        if (tbd != NULL) {
            tbd->hBeacon = hBeacon;
            tbd->lpProcName = _strdup(lpProcName);
            tbd->lpData = (lpData != NULL) ? malloc(Size) : NULL;
            tbd->Size = Size;
            if (tbd->lpData != NULL) {
                memcpy(tbd->lpData, lpData, Size);
            }
            DWORD dwThreadId = 0;
            HANDLE hBeaconThread = CreateThread(NULL, 0, ThreadBeacon, (LPVOID)tbd, 0, &dwThreadId);
            bof_fd* beacon = (bof_fd*)hBeacon;
            tbd->hThread = hBeaconThread;
            tbd->dwThreadId = dwThreadId;
        }
        
        return tbd != NULL;
    }

    return FALSE;
}

HANDLE WINAPI LoadBeacon(LPVOID lpAddress, SIZE_T Size)
{
    bof_fd *beacon = load_bof((unsigned char*)lpAddress, (uint32_t)Size);

    return (HANDLE)beacon;;
}

BOOL WINAPI CloseBeacon(HANDLE hBeacon)
{
    if (hBeacon != NULL)
    {
        remove_bof((bof_fd*)hBeacon);
        return TRUE;
    }

    return FALSE;
}