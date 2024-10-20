#include <stdint.h>
#include <beacon.h>
#include <stdio.h>
#include <memory>

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

void BeaconDataParse(datap * parser, char * buffer, int size)
{
    memset(parser, 0, sizeof(datap));
    parser->buffer = buffer;
    parser->length = 0;
    parser->original = buffer;
    parser->size = size;
}

/** what is it? */
char *  BeaconDataPtr(datap * parser, int size)
{
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

int     BeaconDataLength(datap * parser)
{
    int v = 0;
    if (parser != NULL)
        v = parser->length;

    return v;
}

char *  BeaconDataExtract(datap * parser, int * size)
{
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
    return FALSE;
}

void    BeaconFormatAlloc(formatp * format, int maxsz)
{
    memset(format, 0, sizeof(formatp));
    format->buffer = (char *)calloc(maxsz, 1);
    format->original = format->buffer;
    format->size = maxsz;
}

void    BeaconFormatReset(formatp * format)
{
    if (format->original != NULL)
    {
        memset(format->original, 0, format->size);
        format->buffer = format->original;
        format->length = 0;
    }
}

void    BeaconFormatAppend(formatp * format, const char * text, int len)
{
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

void    BeaconFormatPrintf(formatp * format, const char * fmt, ...)
{
    // not implemented
}

char *  BeaconFormatToString(formatp * format, int * size)
{
    // not implemented
    return NULL;
}
void    BeaconFormatFree(formatp * format)
{
    if (format != NULL)
    {
        free(format->original);
        memset(format, 0, sizeof(formatp));
    }
}
void    BeaconFormatInt(formatp * format, int value)
{
    int v = bswap32(value);

    if ((format->size - format->length) >= value)
    {
        memcpy(format->buffer, &v, 4);
        format->buffer += 4;
        format->length -= 4;
    }
}

void   BeaconOutput(int type, const char * data, int len)
{

}
void   BeaconPrintf(int type, const char * fmt, ...)
{

}


/* Token Functions */
BOOL   BeaconUseToken(HANDLE token)
{
return FALSE;
}
void   BeaconRevertToken()
{

}
BOOL   BeaconIsAdmin()
{
return FALSE;
}

/* Spawn+Inject Functions */
void   BeaconGetSpawnTo(BOOL x86, char * buffer, int length)
{

}
void   BeaconInjectProcess(HANDLE hProc, int pid, char * payload, int p_len, int p_offset, char * arg, int a_len)
{

}
void   BeaconInjectTemporaryProcess(PROCESS_INFORMATION * pInfo, char * payload, int p_len, int p_offset, char * arg, int a_len)
{

}
BOOL   BeaconSpawnTemporaryProcess(BOOL x86, BOOL ignoreToken, STARTUPINFO * si, PROCESS_INFORMATION * pInfo)
{
return FALSE;
}
void   BeaconCleanupProcess(PROCESS_INFORMATION * pInfo)
{

}

/* Utility Functions */
BOOL   toWideChar(char * src, wchar_t * dst, int max)
{
return FALSE;
}

BOOL BeaconAddValue(const char * key, void * ptr)
{
    return FALSE;
}
void * BeaconGetValue(const char * key)
{
    return NULL;
}
BOOL BeaconRemoveValue(const char * key)
{
    return FALSE;
}


PDATA_STORE_OBJECT BeaconDataStoreGetItem(size_t index)
{
return NULL;
}
void BeaconDataStoreProtectItem(size_t index)
{

}
void BeaconDataStoreUnprotectItem(size_t index)
{

}
size_t BeaconDataStoreMaxEntries()
{
    return 0;
}

/* Beacon User Data functions */
char * BeaconGetCustomUserData()
{
    return NULL;
}


BOOL BeaconGetSyscallInformation(PBEACON_SYSCALLS info, BOOL resolveIfNotInitialized)
{
    return FALSE;
}

/* Beacon System call functions which will use the current system call method */
LPVOID BeaconVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    return NULL;
}
LPVOID BeaconVirtualAllocEx(HANDLE processHandle, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
    return NULL;
}
BOOL BeaconVirtualProtect(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    return FALSE;
}
BOOL BeaconVirtualProtectEx(HANDLE processHandle, LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
    return FALSE;
}
BOOL BeaconVirtualFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType)
{
    return FALSE;
}
BOOL BeaconGetThreadContext(HANDLE threadHandle, PCONTEXT threadContext)
{
    return GetThreadContext(threadHandle, threadContext);
}

BOOL BeaconSetThreadContext(HANDLE threadHandle, PCONTEXT threadContext)
{
    return SetThreadContext(threadHandle, threadContext);
}
DWORD BeaconResumeThread(HANDLE threadHandle)
{
    return ResumeThread(threadHandle);
}
HANDLE BeaconOpenProcess(DWORD desiredAccess, BOOL inheritHandle, DWORD processId)
{
    return NULL;
}
HANDLE BeaconOpenThread(DWORD desiredAccess, BOOL inheritHandle, DWORD threadId)
{
    return NULL;
}
BOOL BeaconCloseHandle(HANDLE object)
{
    return FALSE;
}
BOOL BeaconUnmapViewOfFile(LPCVOID baseAddress)
{
    return FALSE;
}
SIZE_T BeaconVirtualQuery(LPCVOID address, PMEMORY_BASIC_INFORMATION buffer, SIZE_T length)
{
    return 0;
}
BOOL BeaconDuplicateHandle(HANDLE hSourceProcessHandle, HANDLE hSourceHandle, HANDLE hTargetProcessHandle, LPHANDLE lpTargetHandle, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwOptions)
{
    return FALSE;
}
BOOL BeaconReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesRead)
{
    return FALSE;
}
BOOL BeaconWriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten)
{
    return FALSE;
}
/* Beacon Gate APIs */
VOID BeaconDisableBeaconGate()
{

}
VOID BeaconEnableBeaconGate()
{

}