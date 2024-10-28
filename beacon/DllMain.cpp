#include <Windows.h>
#include <stdint.h>

#include "loader.h"
#include "tbd.h"

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


static DWORD WINAPI ThreadBeacon(LPVOID lpParameter)
{
    LPTBD tbd = (LPTBD)lpParameter;

    TlsSetValue(0, (LPVOID)tbd);

    int r = RunCOFF((bof_fd*)tbd->hBeacon, (char*)tbd->lpProcName, (unsigned char*)tbd->lpData, (int)tbd->Size);

    CloseHandle(tbd->hThread);
    return 0;
}

BOOL WINAPI RunBeacon(HANDLE hBeacon, LPCSTR lpProcName, LPVOID lpData, SIZE_T Size)
{
    if (hBeacon != NULL) {
        LPTBD tbd = (LPTBD)calloc(1, sizeof(TBD));
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


BOOL WINAPI FindNextSymbol(HANDLE hBeacon, LPCSTR* lpSymbolName, LPVOID* Address)
{
    bof_fd* beacon = (bof_fd*)hBeacon;

    uint32_t tempcounter;
    struct coff_sym* coff_sym_ptr = (struct coff_sym*)(beacon->coff_data + beacon->coff_header_ptr->PointerToSymbolTable);

    for (tempcounter = beacon->symbol_position; tempcounter < beacon->coff_header_ptr->NumberOfSymbols; tempcounter++) {
        if (coff_sym_ptr[tempcounter].StorageClass != 2) {
            beacon->symbol_position++;
            continue;
        }

        if (coff_sym_ptr[tempcounter].first.value[0] == 0) {
            uint32_t e_offset = coff_sym_ptr[tempcounter].first.value[1];
            uint8_t* str_table = (uint8_t *) & coff_sym_ptr[beacon->coff_header_ptr->NumberOfSymbols];
            uint32_t str_size = *((uint32_t*)str_table);
            *lpSymbolName = (LPCSTR)(str_table + e_offset);

        }
        else {
            *lpSymbolName = (coff_sym_ptr[tempcounter].first.Name);
        }

        if (coff_sym_ptr[tempcounter].SectionNumber == 0xffff) {
            *Address = NULL;
        }
        else {
            *Address = (LPVOID)((ULONG_PTR)beacon->sectionMapping[coff_sym_ptr[tempcounter].SectionNumber - 1] + coff_sym_ptr[tempcounter].Value);
        }
        beacon->symbol_position++;
        return TRUE;
    }

    return FALSE;
}

BOOL WINAPI FindFirstSymbol(HANDLE hBeacon, LPCSTR* lpSymbolName, LPVOID* Address)
{
    if (hBeacon == NULL) {
        *lpSymbolName = NULL;
        *Address = NULL;
        return FALSE;
    }

    bof_fd* beacon = (bof_fd *)hBeacon;
    beacon->symbol_position = 0;
    return FindNextSymbol(hBeacon, lpSymbolName, Address);
}

BOOL WINAPI ClearBreakPoint()
{
    return FALSE;
}

BOOL WINAPI SetBreakPoint()
{
    return FALSE;
}