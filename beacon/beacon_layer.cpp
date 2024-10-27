/*
 * Cobalt Strike 4.X BOF compatibility layer
 * -----------------------------------------
 * The whole point of these files are to allow beacon object files built for CS
 * to run fine inside of other tools without recompiling.
 *
 * Built off of the beacon.h file provided to build for CS.
 */

#include <Windows.h>
#include <intrin.h>
#include <stdint.h>
#include <beacon.h>
#include <stdio.h>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "loader.h"

#ifdef _WIN32
#define DEFAULTPROCESSNAME "rundll32.exe"
#ifdef _WIN64
#define X86PATH "SysWOW64"
#define X64PATH "System32"
#else
#define X86PATH "System32"
#define X64PATH "sysnative"
#endif
#endif

 /* Data Parsing */
EXTERN_C
struct BEACON_Functions InternalFunctions[30] = {
    {"BeaconDataParse", (unsigned char*)BeaconDataParse, 0},
    {"BeaconDataInt", (unsigned char*)BeaconDataInt, 0},
    {"BeaconDataShort", (unsigned char*)BeaconDataShort, 0},
    {"BeaconDataLength", (unsigned char*)BeaconDataLength, 0},
    {"BeaconDataExtract", (unsigned char*)BeaconDataExtract, 0},
    {"BeaconFormatAlloc", (unsigned char*)BeaconFormatAlloc, 0},
    {"BeaconFormatReset", (unsigned char*)BeaconFormatReset, 0},
    {"BeaconFormatFree", (unsigned char*)BeaconFormatFree, 0},
    {"BeaconFormatAppend", (unsigned char*)BeaconFormatAppend, 0},
    {"BeaconFormatPrintf", (unsigned char*)BeaconFormatPrintf, 0},
    {"BeaconFormatToString", (unsigned char*)BeaconFormatToString, 0},
    {"BeaconFormatInt", (unsigned char*)BeaconFormatInt, 0},
    {"BeaconPrintf", (unsigned char*)BeaconPrintf, 0},
    {"BeaconOutput", (unsigned char*)BeaconOutput, 0},
    {"BeaconUseToken", (unsigned char*)BeaconUseToken, 0},
    {"BeaconRevertToken", (unsigned char*)BeaconRevertToken, 0},
    {"BeaconIsAdmin", (unsigned char*)BeaconIsAdmin, 0},
    {"BeaconGetSpawnTo", (unsigned char*)BeaconGetSpawnTo, 0},
    {"BeaconSpawnTemporaryProcess", (unsigned char*)BeaconSpawnTemporaryProcess, 0},
    {"BeaconInjectProcess", (unsigned char*)BeaconInjectProcess, 0},
    {"BeaconInjectTemporaryProcess", (unsigned char*)BeaconInjectTemporaryProcess, 0},
    {"BeaconCleanupProcess", (unsigned char*)BeaconCleanupProcess, 0},
    {"toWideChar", (unsigned char*)toWideChar, 0},
    {"LoadLibraryA", (unsigned char*)BeaconLoadLibraryA, 0},
    {"GetProcAddress", (unsigned char*)BeaconGetProcAddress, 0},
    {"GetModuleHandleA", (unsigned char*)BeaconGetModuleHandleA, 0},
    {"FreeLibrary", (unsigned char*)BeaconFreeLibrary, 0},
    {"__C_specific_handler", NULL, 0}

};

static int bswap32(int value)
{
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)
    int tmp = (value & 0xff) << 24 | (((value >> 8) & 0xff) << 16) | (((value >> 16) & 0xff) << 8) | (value >> 24) & 0xff;
    value = tmp;
#endif
    return value;
}

static int bswap16(short value)
{
#if defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)
    short tmp = (((value >> 16) & 0xff) << 8) | (value >> 24) & 0xff;
    value = tmp;
#endif
    return value;
}

#ifdef _DEBUG
#define PRINT_STACK_TRACE(FunctionName) { \
    void *caller = _ReturnAddress();    \
    printf("[%p] "#FunctionName"\n", caller);   \
}
#else
#define PRINT_STACK_TRACE(FunctionName) do {} while(0)
#endif

void BeaconDataParse(datap * parser, char * buffer, int size)
{
    PRINT_STACK_TRACE(BeaconDataParse);
    memset(parser, 0, sizeof(datap));
    parser->buffer = buffer;
    parser->length = 0;
    parser->original = buffer;
    parser->size = size;
}

/** what is it? */
char *  BeaconDataPtr(datap * parser, int size)
{
    PRINT_STACK_TRACE(BeaconDataPtr);
    char * tmp = NULL;
    if (parser != NULL && parser->length >= size)
    {
        tmp = parser->buffer;
        parser->length -= size;
        parser->buffer += size;
    }

    return tmp;
}

int BeaconDataInt(datap * parser)
{
    PRINT_STACK_TRACE(BeaconDataInt);
    int v = 0;

    if (parser != NULL && parser->length >= 4)
    {
        parser->length -= 4;
        memcpy(&v, parser->buffer, 4);

        parser->buffer += 4;
        v = bswap32(v);
    }
    return v;
}
short   BeaconDataShort(datap * parser)
{
    PRINT_STACK_TRACE(BeaconDataShort);
    short v = 0;

    if (parser != NULL && parser->length >= 2)
    {
        parser->length -= 2;
        memcpy(&v, parser->buffer, 2);

        parser->buffer += 2;
        v = bswap16(v);
    }
    return v;
}

int BeaconDataLength(datap * parser)
{
    PRINT_STACK_TRACE(BeaconDataLength);
    int v = 0;
    if (parser != NULL)
        v = parser->length;

    return v;
}

char* BeaconDataExtract(datap * parser, int * size)
{
    PRINT_STACK_TRACE(BeaconDataExtract);
    *size = 0;

    if (parser != NULL)
    {
        *size = BeaconDataInt(parser);
        char *ptr = parser->buffer;
        parser->length += *size;
        parser->buffer += *size;
        return ptr;
    }

    return NULL;
}

/** @brief TO be implemented */
BOOL BeaconInformation(PBEACON_INFO info)
{
    PRINT_STACK_TRACE(BeaconInformation);
    return FALSE;
}

void BeaconFormatAlloc(formatp * format, int maxsz)
{
    PRINT_STACK_TRACE(BeaconFormatAlloc);
    memset(format, 0, sizeof(formatp));
    format->buffer = (char *)calloc(maxsz, 1);
    format->original = format->buffer;
    format->size = maxsz;
}

void BeaconFormatReset(formatp * format)
{
    PRINT_STACK_TRACE(BeaconFormatReset);
    if (format->original != NULL)
    {
        memset(format->original, 0, format->size);
        format->buffer = format->original;
        format->length = 0;
    }
}

void BeaconFormatAppend(formatp * format, const char * text, int len)
{
    PRINT_STACK_TRACE(BeaconFormatAppend);
    if ((format->size - format->length) >= (len + 4))
    {
        int v = bswap32(len);   // store length in big endian
        memcpy(format->buffer, &v, 4);
        format->buffer += 4;
        format->length -= 4;
        memcpy(format->buffer, text, len);
        format->length -= len;
    }
}

void BeaconFormatPrintf(formatp * format, const char * fmt, ...)
{
    PRINT_STACK_TRACE(BeaconFormatPrintf);
    if (format == NULL || fmt == NULL)
        return;

    va_list args;

    va_start(args, fmt);

    char* tmp = NULL;
    int length = vsnprintf(NULL, 0, fmt, args);
    va_end(args, fmt);

    tmp = (char*)malloc(length + 1);

    if (tmp == NULL)
        return;

    va_start(args, fmt);
    vsnprintf(tmp, length, fmt, args);
    va_end(args, fmt);

    size_t bytes_left = format->size - format->length;

    if (bytes_left < length)
        length = bytes_left;    // 

    memcpy(format->buffer, tmp, length);
    format->buffer += length;
    format->length -= length;
}

char *  BeaconFormatToString(formatp * format, int * size)
{
    PRINT_STACK_TRACE(BeaconFormatString);
    *size = format->length;
    return format->original;
}

void BeaconFormatFree(formatp * format)
{
    PRINT_STACK_TRACE(BeaconFormatFree);
    if (format != NULL)
    {
        free(format->original);
        memset(format, 0, sizeof(formatp));
    }
}

void BeaconFormatInt(formatp * format, int value)
{
    PRINT_STACK_TRACE(BeaconFormatInt);
    int v = bswap32(value);

    if ((format->size - format->length) >= value)
    {
        memcpy(format->buffer, &v, 4);
        format->buffer += 4;
        format->length -= 4;
    }
}

void BeaconOutput(int type, const char * data, int len)
{
    PRINT_STACK_TRACE(BeaconOutput);
    if (type == CALLBACK_OUTPUT) {
        printf("[CALLBACK_OUTPUT] %s\n", data);
    }
    else {
        printf("[CALLBACK_%d] not implemented. %p %d\n", type, data, len);
    }
}

/** print the beacon output in the console.. should be in beacondbg session... */
void BeaconPrintf(int type, const char * fmt, ...)
{
    PRINT_STACK_TRACE(BeaconPrintf);
    va_list args;

    va_start(args, fmt);

    char* tmp = NULL;
    int length = vsnprintf(NULL, 0, fmt, args);
    va_end(args, fmt);

    tmp = (char *)malloc(length + 1);

    if (tmp == NULL)
        return;

    va_start(args, fmt);
    vsnprintf(tmp, length, fmt, args);
    va_end(args, fmt);

    printf(tmp);

    free(tmp);
}

/* Token Functions */
BOOL BeaconUseToken(HANDLE token)
{
    PRINT_STACK_TRACE(BeaconUseToken);
    return SetThreadToken(NULL, token);
}

void BeaconRevertToken()
{
    PRINT_STACK_TRACE(BeaconRevertToken);
    if (!RevertToSelf()) {
        printf("[BeaconRevertToken] RevertToSelf failed.\n");
    }
}

/** TODO */
BOOL   BeaconIsAdmin()
{
    PRINT_STACK_TRACE(BeaconIsAdmin);
    return FALSE;
}

/** TODO 
    Spawn+Inject Functions */
void   BeaconGetSpawnTo(BOOL x86, char * buffer, int length)
{
    PRINT_STACK_TRACE(BeaconGetSpawnTo);
    return;
}

void   BeaconInjectProcess(HANDLE hProc, int pid, char * payload, int p_len, int p_offset, char * arg, int a_len)
{
    PRINT_STACK_TRACE(BeaconInjectProcess);
}
void   BeaconInjectTemporaryProcess(PROCESS_INFORMATION * pInfo, char * payload, int p_len, int p_offset, char * arg, int a_len)
{
    PRINT_STACK_TRACE(BeaconInjectTemporaryProcess);
}
BOOL   BeaconSpawnTemporaryProcess(BOOL x86, BOOL ignoreToken, STARTUPINFO * si, PROCESS_INFORMATION * pInfo)
{
    PRINT_STACK_TRACE(BeaconSpawnTemporaryProcess);
    return FALSE;
}

void BeaconCleanupProcess(PROCESS_INFORMATION * pInfo)
{
    PRINT_STACK_TRACE(BeaconCleanupProcess);
    if (pInfo != NULL) {
        CloseHandle(pInfo->hProcess); pInfo->hProcess = NULL;
        CloseHandle(pInfo->hThread); pInfo->hThread = NULL;
    }
}

/* Utility Functions */
BOOL toWideChar(char* src, wchar_t* dst, int max)
{
    PRINT_STACK_TRACE(toWideChar);
    if (max < sizeof(wchar_t)) {
        return FALSE;
    }

    return MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, src, -1, dst, max / sizeof(wchar_t));
}


/** Values stored by a beacon in a cache.. they should be protected? */
static std::map<std::string, void*> values;

BOOL BeaconAddValue(const char * key, void * ptr)
{
    PRINT_STACK_TRACE(BeaconAddValue);
    values[std::string(key)] = ptr;
    return TRUE;
}

void * BeaconGetValue(const char * key)
{
    PRINT_STACK_TRACE(BeaconGetValue);
    std::map<std::string, void*>::iterator it;

    it = values.find(std::string(key));

    if (it == values.end())
        return NULL;
    
    return it->second;
}

BOOL BeaconRemoveValue(const char * key)
{
    PRINT_STACK_TRACE(BeaconRemoveValue);
    std::map<std::string, void*>::iterator it;
    it = values.find(std::string(key));

    if (it == values.end())
        return FALSE;

    values.erase(it);
    return TRUE;
}

std::vector<PDATA_STORE_OBJECT> dataStoreObject;

PDATA_STORE_OBJECT BeaconDataStoreGetItem(size_t index)
{
    PRINT_STACK_TRACE(BeaconDataStoreGetItem);
    if (dataStoreObject.size() < index)
        return NULL;

    return dataStoreObject.at(index);
}

void BeaconDataStoreProtectItem(size_t index)
{
    PRINT_STACK_TRACE(BeaconDataStoreProtectItem);
}

void BeaconDataStoreUnprotectItem(size_t index)
{
    PRINT_STACK_TRACE(BeaconDataStoreUnprotectItem);
}

/// <summary>
/// BeaconDataStoreMaxEntries .. using a vector we can store an unlimited number of objects
/// </summary>
/// <returns></returns>
size_t BeaconDataStoreMaxEntries()
{
    PRINT_STACK_TRACE(BeaconDataStoreMaxEntries);
    return 32;
}

/* Beacon User Data functions */
char * BeaconGetCustomUserData()
{
    PRINT_STACK_TRACE(BeaconGetCustomUserData);
    return NULL;
}


BOOL BeaconGetSyscallInformation(PBEACON_SYSCALLS info, BOOL resolveIfNotInitialized)
{
    PRINT_STACK_TRACE(BeaconGetSyscallInformation);
    return FALSE;
}

/* Beacon System call functions which will use the current system call method */
LPVOID BeaconVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    PRINT_STACK_TRACE(BeaconVirtualAlloc);
    return VirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}

LPVOID BeaconVirtualAllocEx(HANDLE processHandle, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    PRINT_STACK_TRACE(BeaconVirtualAllocEx);
    return VirtualAllocEx(processHandle, lpAddress, dwSize, flAllocationType, flProtect);
}

BOOL BeaconVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    PRINT_STACK_TRACE(BeaconVirtualProtect);
    return VirtualProtect(lpAddress, dwSize, flNewProtect, lpflOldProtect);
}

BOOL BeaconVirtualProtectEx(HANDLE processHandle, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    PRINT_STACK_TRACE(BeaconVirtualProtectEx);
    return VirtualProtectEx(processHandle, lpAddress, dwSize, flNewProtect, lpflOldProtect);
}

BOOL BeaconVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    PRINT_STACK_TRACE(BeaconVirtualFree);
    return VirtualFree(lpAddress, dwSize, dwFreeType);
}

BOOL BeaconGetThreadContext(HANDLE threadHandle, PCONTEXT threadContext)
{
    PRINT_STACK_TRACE(BeaconGetThreadContext);
    return GetThreadContext(threadHandle, threadContext);
}

BOOL BeaconSetThreadContext(HANDLE threadHandle, PCONTEXT threadContext)
{
    PRINT_STACK_TRACE(BeaconSetThreadContext);
    return SetThreadContext(threadHandle, threadContext);
}

DWORD BeaconResumeThread(HANDLE threadHandle)
{
    PRINT_STACK_TRACE(BeaconResumeThread);
    return ResumeThread(threadHandle);
}

HANDLE BeaconOpenProcess(DWORD desiredAccess, BOOL inheritHandle, DWORD processId)
{
    PRINT_STACK_TRACE(BeaconOpenProcess);
    return OpenProcess(desiredAccess, inheritHandle, processId);
}

HANDLE BeaconOpenThread(DWORD desiredAccess, BOOL inheritHandle, DWORD threadId)
{
    PRINT_STACK_TRACE(BeaconOpenThread);
    return OpenThread(desiredAccess, inheritHandle, threadId);
}

BOOL BeaconCloseHandle(HANDLE object)
{
    PRINT_STACK_TRACE(BeaconCloseHandle);
    return CloseHandle(object);
}

BOOL BeaconUnmapViewOfFile(LPCVOID baseAddress)
{
    PRINT_STACK_TRACE(BeaconUnmapViewOfFile);
    return UnmapViewOfFile(baseAddress);
}

SIZE_T BeaconVirtualQuery(LPCVOID address, PMEMORY_BASIC_INFORMATION buffer, SIZE_T length)
{
    PRINT_STACK_TRACE(BeaconVirtualQuery);
    return VirtualQuery(address, buffer, length);
}

BOOL BeaconDuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, LPHANDLE lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{
    PRINT_STACK_TRACE(BeaconDuplicateHandle);
    return DuplicateHandle(hSourceProcessHandle, hSourceHandle, hTargetProcessHandle, lpTargetHandle, dwDesiredAccess, bInheritHandle, dwOptions);
}

BOOL BeaconReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesRead)
{
    PRINT_STACK_TRACE(BeaconReadProcessMemory);
    return ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

BOOL BeaconWriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten)
{
    PRINT_STACK_TRACE(BeaconWriteProcessMemory);
    return WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}

/* Beacon Gate APIs */
VOID BeaconDisableBeaconGate()
{
    PRINT_STACK_TRACE(BeaconDisableBeaconGate);
}

VOID BeaconEnableBeaconGate()
{
    PRINT_STACK_TRACE(BeaconEnableBeaconGate);
}

HMODULE WINAPI BeaconLoadLibraryA(LPCSTR lpLibFileName)
{
    PRINT_STACK_TRACE(LoadLibraryA);
    return LoadLibraryA(lpLibFileName);
}

FARPROC WINAPI BeaconGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
    PRINT_STACK_TRACE(GetProcAddress);
    return GetProcAddress(hModule, lpProcName);
}

HMODULE WINAPI BeaconGetModuleHandleA(LPCSTR lpModuleName)
{
    PRINT_STACK_TRACE(GetModuleHandleA);
    return GetModuleHandleA(lpModuleName);
}

BOOL WINAPI BeaconFreeLibrary(HMODULE hLibModule)
{
    PRINT_STACK_TRACE(FreeLibrary);
    return FreeLibrary(hLibModule);
}
