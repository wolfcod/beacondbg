#pragma once

#ifndef __TBD_H_
#define __TBD_H_

typedef struct ThreadBeaconData
{
    HANDLE hThread;
    DWORD dwThreadId;

    HANDLE hBeacon;
    LPCSTR lpProcName;
    LPVOID lpData;
    SIZE_T Size;
} TBD, *LPTBD;

#endif
