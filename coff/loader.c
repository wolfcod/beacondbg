#define _CRT_SECURE_NO_WARNINGS

#if defined(_WIN32)
#include <windows.h>
#include <beacondbg.h>
#include <beacon.h>
#else
#define BOOL int
#endif

#define size_t int

/*
 * COFF Loader Project
 * -------------------
 * This is a re-implementation of a COFF loader, with a BOF compatibility layer
 * it's meant to provide functional example of loading a COFF file in memory
 * and maybe be useful.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"

 /* Enable or disable debug output if testing or adding new relocation types */
#ifdef DEBUG
#define DEBUG_PRINT(x, ...) printf(x, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(x, ...)
#endif

/* Defining symbols for the OS version, will try to define anything that is
 * different between the arch versions by specifying them here. */
#if defined(__x86_64__) || defined(_WIN64)
#define PREPENDSYMBOLVALUE "__imp_"
#else
#define PREPENDSYMBOLVALUE "__imp__"
#endif

#define COFFLOADER_RETURN_VAL_IF(expr, val, fmt, ...) if ((expr)) { DEBUG_PRINT(fmt, __VA_ARGS__); return val; }

unsigned char* unhexlify(unsigned char* value, int *outlen) {
    unsigned char* retval = NULL;
    char byteval[3] = { 0 };
    unsigned int counter = 0;
    int counter2 = 0;
    char character = 0;
    if (value == NULL) {
        return NULL;
    }
    DEBUG_PRINT("Unhexlify Strlen: %lu\n", (long unsigned int)strlen((char*)value));
    if (strlen((char*)value) % 2 != 0) {
        DEBUG_PRINT("Either value is NULL, or the hexlified string isn't valid\n");
        goto errcase;
    }

    retval = calloc(strlen((char*)value) + 1, 1);
    if (retval == NULL) {
        goto errcase;
    }

    counter2 = 0;
    for (counter = 0; counter < strlen((char*)value); counter += 2) {
        memcpy(byteval, value + counter, 2);
        character = (char)strtol(byteval, NULL, 16);
        memcpy(retval + counter2, &character, 1);
        counter2++;
    }
    *outlen = counter2;

errcase:
    return retval;
}



/* Helper to just get the contents of a file, used for testing. Real
 * implementations of this in an agent would use the tasking from the
 * C2 server for this */
unsigned char* getContents(char* filepath, uint32_t* outsize) {
    FILE *fin = NULL;
    uint32_t fsize = 0;
    size_t readsize = 0;
    unsigned char* buffer = NULL;
    unsigned char* tempbuffer = NULL;

    fin = fopen(filepath, "rb");
    if (fin == NULL) {
        return NULL;
    }
    fseek(fin, 0, SEEK_END);
    fsize = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    tempbuffer = calloc(fsize, 1);
    if (tempbuffer == NULL) {
        fclose(fin);
        return NULL;
    }
    memset(tempbuffer, 0, fsize);
    readsize = fread(tempbuffer, 1, fsize, fin);

    fclose(fin);
    buffer = calloc(readsize, 1);
    if (buffer == NULL) {
        free(tempbuffer);
        return NULL;
    }
    memset(buffer, 0, readsize);
    memcpy(buffer, tempbuffer, readsize - 1);
    free(tempbuffer);
    *outsize = fsize;
    return buffer;
}

static BOOL starts_with(const char* string, const char* substring) {
    return strncmp(string, substring, strlen(substring)) == 0;
}

/* Helper function to process a symbol string, determine what function and
 * library its from, and return the right function pointer. Will need to
 * implement in the loading of the beacon internal functions, or any other
 * internal functions you want to have available. */
void* process_symbol(char* symbolstring) {
    void* functionaddress = NULL;
    char localcopy[1024] = { 0 };
    char* locallib = NULL;
    char* localfunc = NULL;
#if defined(_WIN32)    
    int tempcounter = 0;
    HMODULE llHandle = NULL;
#endif

    strncpy(localcopy, symbolstring, sizeof(localcopy) - 1);
    if (starts_with(symbolstring, PREPENDSYMBOLVALUE"Beacon") || starts_with(symbolstring, PREPENDSYMBOLVALUE"toWideChar") ||
        starts_with(symbolstring, PREPENDSYMBOLVALUE"GetProcAddress") || starts_with(symbolstring, PREPENDSYMBOLVALUE"LoadLibraryA") ||
        starts_with(symbolstring, PREPENDSYMBOLVALUE"GetModuleHandleA") || starts_with(symbolstring, PREPENDSYMBOLVALUE"FreeLibrary") ||
        starts_with(symbolstring, "__C_specific_handler")) {
        if(strcmp(symbolstring, "__C_specific_handler") == 0)
        {
            localfunc = symbolstring;
            return InternalFunctions[29][1];
        }
        else
        {
            localfunc = symbolstring + strlen(PREPENDSYMBOLVALUE);
        }
        DEBUG_PRINT("\t\tInternalFunction: %s\n", localfunc);
        /* TODO: Get internal symbol here and set to functionaddress, then
         * return the pointer to the internal function*/
#if defined(_WIN32)
        for (tempcounter = 0; tempcounter < 30; tempcounter++) {
            if (InternalFunctions[tempcounter][0] != NULL) {
                if (starts_with(localfunc, (char*)(InternalFunctions[tempcounter][0]))) {
                    functionaddress = (void*)InternalFunctions[tempcounter][1];
                    return functionaddress;
                }
            }
        }
#endif
    }
    else if (strncmp(symbolstring, PREPENDSYMBOLVALUE, strlen(PREPENDSYMBOLVALUE)) == 0) {
        DEBUG_PRINT("\t\tYep its an external symbol\n");
        locallib = localcopy + strlen(PREPENDSYMBOLVALUE);

        locallib = strtok(locallib, "$");
        localfunc = strtok(NULL, "$");
        DEBUG_PRINT("\t\tLibrary: %s\n", locallib);
        localfunc = strtok(localfunc, "@");
        DEBUG_PRINT("\t\tFunction: %s\n", localfunc);
        /* Resolve the symbols here, and set the functionpointervalue */
#if defined(_WIN32)
        llHandle = LoadLibraryA(locallib);
        DEBUG_PRINT("\t\tHandle: 0x%lx\n", llHandle);
        functionaddress = GetProcAddress(llHandle, localfunc);
        DEBUG_PRINT("\t\tProcAddress: 0x%p\n", functionaddress);
#endif
    }
    return functionaddress;
}

/* Just a generic runner for testing, this is pretty much just a reference
 * implementation, return values will need to be checked, more relocation
 * types need to be handled, and needs to have different arguments for use
 * in any agent. */
int RunCOFF(bof_fd *bof, char* functionname, unsigned char* argumentdata, int argumentSize) {

    char* entryfuncname = functionname;
    
    uint32_t tempcounter;
    struct coff_sym* coff_sym_ptr = (struct coff_sym*)(bof->coff_data + bof->coff_header_ptr->PointerToSymbolTable);

    void(__cdecl * foo)(char*, unsigned long);

    DEBUG_PRINT("Symbols:\n");
    for (tempcounter = 0; tempcounter < bof->coff_header_ptr->NumberOfSymbols; tempcounter++) {
        DEBUG_PRINT("\t%s: Section: %d, Value: 0x%X\n", coff_sym_ptr[tempcounter].first.Name, coff_sym_ptr[tempcounter].SectionNumber, coff_sym_ptr[tempcounter].Value);
        if (strcmp(coff_sym_ptr[tempcounter].first.Name, entryfuncname) == 0) {
            DEBUG_PRINT("\t\tFound entry!\n");
#ifdef _WIN32
            /* So for some reason VS 2017 doesn't like this, but char* casting works, so just going to do that */
#ifdef _MSC_VER
            foo = (void(__cdecl*)(char*, unsigned long))(bof->sectionMapping[coff_sym_ptr[tempcounter].SectionNumber - 1] + coff_sym_ptr[tempcounter].Value);
#else
            foo = (void(*)(char *, unsigned long))(sectionMapping[coff_sym_ptr[tempcounter].SectionNumber - 1] + coff_sym_ptr[tempcounter].Value);
#endif
            //sectionMapping[coff_sym_ptr[tempcounter].SectionNumber-1][coff_sym_ptr[tempcounter].Value+7] = '\xcc';
            DEBUG_PRINT("Trying to run: %p\n", foo);
            foo((char*)argumentdata, argumentSize);
#endif
        }
    }
    DEBUG_PRINT("Back\n");

    return 0;
}

/* Just a generic runner for testing, this is pretty much just a reference
 * implementation, return values will need to be checked, more relocation
 * types need to be handled, and needs to have different arguments for use
 * in any agent. */
static int init_bof(bof_fd *bof)
{
    coff_sect_t* coff_sect_ptr = NULL;
    coff_reloc_t* coff_reloc_ptr = NULL;
    int retcode = 0;
    int counter = 0;
    int reloccount = 0;
    unsigned int tempcounter = 0;
    uint32_t symptr = 0;

    COFFLOADER_RETURN_VAL_IF(bof->coff_size == 0, 1, "COFF file size is 0\n");
    COFFLOADER_RETURN_VAL_IF(bof->coff_size < sizeof(struct coff_file_header), 1,
        "COFF file size too small for a COFF file header\n");

    bof->coff_header_ptr = (struct coff_file_header*)bof->coff_data;

    COFFLOADER_RETURN_VAL_IF(bof->coff_header_ptr->PointerToSymbolTable < sizeof(struct coff_file_header),
        1, "COFF symbol table offset is inside the file header\n");
    COFFLOADER_RETURN_VAL_IF(bof->coff_size < bof->coff_header_ptr->PointerToSymbolTable, 1,
        "COFF symbol table offset exceeds file size\n");

    // Byte index of the strtab/end of symtab
    size_t coff_strtab_index =
        bof->coff_header_ptr->PointerToSymbolTable + bof->coff_header_ptr->NumberOfSymbols * sizeof(struct coff_sym);

    COFFLOADER_RETURN_VAL_IF(bof->coff_size < coff_strtab_index, 1, "COFF symbol table exceeds COFF file size\n");
    COFFLOADER_RETURN_VAL_IF(bof->coff_size < coff_strtab_index + sizeof(uint32_t), 1,
        "COFF string table offset exceeds COFF file size\n");

    uint32_t coff_strtab_size = *(uint32_t*)(bof->coff_data + coff_strtab_index);

    COFFLOADER_RETURN_VAL_IF(bof->coff_size < coff_strtab_index + coff_strtab_size, 1,
        "COFF string table exceeds COFF file size\n");
    COFFLOADER_RETURN_VAL_IF(bof->coff_size != coff_strtab_index + coff_strtab_size, 1,
        "COFF file contains extraneous data\n");

    struct coff_sym* coff_sym_ptr = (struct coff_sym*)(bof->coff_data + bof->coff_header_ptr->PointerToSymbolTable);

    void* funcptrlocation = NULL;
    size_t offsetvalue = 0;
    uint64_t longoffsetvalue = 0;

    HMODULE kern = GetModuleHandleA("kernel32.dll");
    InternalFunctions[29][1] = (unsigned char*)GetProcAddress(kern, "__C_specific_handler");
    DEBUG_PRINT("found address of %x\n", InternalFunctions[29][1]);
#ifdef _WIN32
    /* NOTE: I just picked a size, look to see what is max/normal. */
#ifdef DEBUG
    int* sectionSize = NULL;
#endif
    void(*foo)(char* in, unsigned long datalen);
    int relocationCount = 0;
#endif

    DEBUG_PRINT("Machine 0x%X\n", bof->coff_header_ptr->Machine);
    DEBUG_PRINT("Number of sections: %d\n", bof->coff_header_ptr->NumberOfSections);
    DEBUG_PRINT("TimeDateStamp : %X\n", bof->coff_header_ptr->TimeDateStamp);
    DEBUG_PRINT("PointerToSymbolTable : 0x%X\n", bof->coff_header_ptr->PointerToSymbolTable);
    DEBUG_PRINT("NumberOfSymbols: %u\n", bof->coff_header_ptr->NumberOfSymbols);
    DEBUG_PRINT("OptionalHeaderSize: %d\n", bof->coff_header_ptr->SizeOfOptionalHeader);
    DEBUG_PRINT("Characteristics: %d\n", bof->coff_header_ptr->Characteristics);
    DEBUG_PRINT("\n");
    /* Actually allocate an array to keep track of the sections */
    bof->sectionMapping = (char**)calloc(sizeof(char*) * (bof->coff_header_ptr->NumberOfSections + 1), 1);
#ifdef DEBUG
    sectionSize = (int*)calloc(sizeof(int) * (bof->coff_header_ptr->NumberOfSections + 1), 1);
#endif
    if (bof->sectionMapping == NULL) {
        DEBUG_PRINT("Failed to allocate sectionMapping\n");
        goto cleanup;
    }

    /* Handle the allocation and copying of the sections we're going to use
     * for right now I'm just VirtualAlloc'ing memory, this can be changed to
     * other methods, but leaving that up to the person implementing it. */
    for (counter = 0; counter < bof->coff_header_ptr->NumberOfSections; counter++) {
        coff_sect_ptr = (coff_sect_t*)(bof->coff_data + sizeof(coff_file_header_t) + (sizeof(coff_sect_t) * counter));
        DEBUG_PRINT("Name: %s\n", coff_sect_ptr->Name);
        DEBUG_PRINT("VirtualSize: 0x%X\n", coff_sect_ptr->VirtualSize);
        DEBUG_PRINT("VirtualAddress: 0x%X\n", coff_sect_ptr->VirtualAddress);
        DEBUG_PRINT("SizeOfRawData: 0x%X\n", coff_sect_ptr->SizeOfRawData);
        DEBUG_PRINT("PointerToRelocations: 0x%X\n", coff_sect_ptr->PointerToRelocations);
        DEBUG_PRINT("PointerToRawData: 0x%X\n", coff_sect_ptr->PointerToRawData);
        DEBUG_PRINT("NumberOfRelocations: %d\n", coff_sect_ptr->NumberOfRelocations);
        relocationCount += coff_sect_ptr->NumberOfRelocations;
        /* NOTE: When changing the memory loading information of the loader,
         * you'll want to use this field and the defines from the Section
         * Flags table of Microsofts page, some defined in COFFLoader.h */
        DEBUG_PRINT("Characteristics: %x\n", coff_sect_ptr->Characteristics);
#ifdef _WIN32
        DEBUG_PRINT("Allocating 0x%x bytes\n", coff_sect_ptr->VirtualSize);
        /* NOTE: Might want to allocate as PAGE_READWRITE and VirtualProtect
         * before execution to either PAGE_READWRITE or PAGE_EXECUTE_READ
         * depending on the Section Characteristics. Parse them all again
         * before running and set the memory permissions. */
        bof->sectionMapping[counter] = VirtualAlloc(NULL, coff_sect_ptr->SizeOfRawData, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
#ifdef DEBUG
        sectionSize[counter] = coff_sect_ptr->SizeOfRawData;
#endif
        if (bof->sectionMapping[counter] == NULL) {
            DEBUG_PRINT("Failed to allocate memory\n");
        }
        DEBUG_PRINT("Allocated section %d at %p\n", counter, sectionMapping[counter]);
        if (coff_sect_ptr->PointerToRawData != 0) {
            memcpy(bof->sectionMapping[counter], bof->coff_data + coff_sect_ptr->PointerToRawData, coff_sect_ptr->SizeOfRawData);
        }
        else {
            memset(bof->sectionMapping[counter], 0, coff_sect_ptr->SizeOfRawData);
        }
#endif
    }
    DEBUG_PRINT("Total Relocations: %d\n", relocationCount);
    /* Allocate and setup the GOT for functions, same here as above. */
    /* Actually allocate enough for worst case every relocation, may not be needed, but hey better safe than sorry */
#ifdef _WIN32
#ifdef _WIN64
    bof->functionMapping = VirtualAlloc(NULL, relocationCount * 8, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
#else
    functionMapping = VirtualAlloc(NULL, relocationCount * 8, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_EXECUTE_READWRITE);
#endif
    if (bof->functionMapping == NULL) {
        DEBUG_PRINT("Failed to allocate functionMapping\n");
        goto cleanup;
    }
#endif

    /* Start parsing the relocations, and *hopefully* handle them correctly. */
    for (counter = 0; counter < bof->coff_header_ptr->NumberOfSections; counter++) {
        DEBUG_PRINT("Doing Relocations of section: %d\n", counter);
        coff_sect_ptr = (coff_sect_t*)(bof->coff_data + sizeof(coff_file_header_t) + (sizeof(coff_sect_t) * counter));
        coff_reloc_ptr = (coff_reloc_t*)(bof->coff_data + coff_sect_ptr->PointerToRelocations);
        for (reloccount = 0; reloccount < coff_sect_ptr->NumberOfRelocations; reloccount++) {
            DEBUG_PRINT("\tVirtualAddress: 0x%X\n", coff_reloc_ptr->VirtualAddress);
            DEBUG_PRINT("\tSymbolTableIndex: 0x%X\n", coff_reloc_ptr->SymbolTableIndex);
            DEBUG_PRINT("\tType: 0x%X\n", coff_reloc_ptr->Type);
            if (coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].first.Name[0] != 0) {
                symptr = coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].first.value[1];
                DEBUG_PRINT("\tSymPtr: 0x%X\n", symptr);
                DEBUG_PRINT("\tSymName: %s\n", coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].first.Name);
                DEBUG_PRINT("\tSectionNumber: 0x%X\n", coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber);

                /* This is the code for relative offsets in other sections of the COFF file. */
#ifdef _WIN32
#ifdef _WIN64
            /* Type == 1 relocation is the 64-bit VA of the relocation target */
                if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_ADDR64) {
                    memcpy(&longoffsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(uint64_t));
                    DEBUG_PRINT("\tReadin longOffsetValue : 0x%llX\n", longoffsetvalue);
                    longoffsetvalue = (uint64_t)(bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + (uint64_t)longoffsetvalue);
                    longoffsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tModified longOffsetValue : 0x%llX Base Address: %p\n", longoffsetvalue, sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1]);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &longoffsetvalue, sizeof(uint64_t));
                }
                /* This is Type == 3 relocation code */
                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_ADDR32NB) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\tReadin OffsetValue : 0x%0X\n", offsetvalue);
                    DEBUG_PRINT("\t\tReferenced Section: 0x%X\n", sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue);
                    DEBUG_PRINT("\t\tEnd of Relocation Bytes: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4);
                    if (((char*)(bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue) - (char*)(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue = ((char*)(bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue) - (char*)(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tSetting 0x%p to OffsetValue: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                /* This is Type == 4 relocation code, needed to make global variables to work correctly */
                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32_1) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    offsetvalue += 1;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }

                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32_2) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    offsetvalue += 2;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }

                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32_3) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    offsetvalue += 3;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }

                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32_4) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    offsetvalue += 4;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32_5) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    offsetvalue += 5;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }

                else {
                    DEBUG_PRINT("No code for relocation type: %d\n", coff_reloc_ptr->Type);
                }
#else
             /* This is Type == IMAGE_REL_I386_DIR32 relocation code */
                if (coff_reloc_ptr->Type == IMAGE_REL_I386_DIR32) {
                    memcpy(&offsetvalue, sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\tReadin OffsetValue : 0x%0X\n", offsetvalue);
                    offsetvalue = (uint32_t)(sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1]) + offsetvalue;
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tSetting 0x%p to: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_I386_REL32) {
                    memcpy(&offsetvalue, sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\tReadin OffsetValue : 0x%0X\n", offsetvalue);
                    offsetvalue += (sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));

                }
#endif //WIN64 statement close
#endif //WIN32 statement close
            }
            else {
                symptr = coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].first.value[1];
                DEBUG_PRINT("\tSymPtr: 0x%X\n", symptr);
                DEBUG_PRINT("\tSymVal: %s\n", ((char*)(coff_sym_ptr + coff_header_ptr->NumberOfSymbols)) + symptr);
                DEBUG_PRINT("\tSectionNumber: 0x%X\n", coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber);

                /* This is the code to handle functions themselves, so using a makeshift Global Offset Table for it */
#ifdef _WIN32
                funcptrlocation = process_symbol(((char*)(coff_sym_ptr + bof->coff_header_ptr->NumberOfSymbols)) + symptr);
                if (funcptrlocation == NULL && coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber == 0) {
                    DEBUG_PRINT("Failed to resolve symbol\n");
                    retcode = 1;
                    goto cleanup;
                }
#ifdef _WIN64
                if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_ADDR64) {
                    memcpy(&longoffsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(uint64_t));
                    DEBUG_PRINT("\tReadin longOffsetValue : 0x%llX\n", longoffsetvalue);
                    longoffsetvalue = (uint64_t)(bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + (uint64_t)longoffsetvalue);
                    longoffsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tModified longOffsetValue : 0x%llX Base Address: %p\n", longoffsetvalue, bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1]);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &longoffsetvalue, sizeof(uint64_t));
                }

                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32 && funcptrlocation != NULL) {
                    /* This is Type == 4 relocation code */
                    DEBUG_PRINT("Doing function relocation\n");
                    if (((bof->functionMapping + (bof->functionMappingCount * 8)) - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    memcpy(bof->functionMapping + (bof->functionMappingCount * 8), &funcptrlocation, sizeof(uint64_t));
                    offsetvalue = (int32_t)((bof->functionMapping + (bof->functionMappingCount * 8)) - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                    bof->functionMappingCount++;
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_REL32) {
                    /* This shouldn't be needed here, but incase there's a defined symbol
                     * that somehow doesn't have a function, try to resolve it here.*/
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    if ((bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    DEBUG_PRINT("\t\tReferenced Section: 0x%X\n", bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue);
                    DEBUG_PRINT("\t\tReadin offset value: 0x%X\n", offsetvalue);
                    DEBUG_PRINT("\t\tVirtualAddressOffset: 0x%X\n", (sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += (bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\t\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_AMD64_ADDR32NB) {
                    memcpy(&offsetvalue, bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\tReadin OffsetValue : 0x%0X\n", offsetvalue);
                    DEBUG_PRINT("\t\tReferenced Section: 0x%X\n", sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue);
                    DEBUG_PRINT("\t\tEnd of Relocation Bytes: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4);
                    if (((char*)(bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue) - (char*)(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4)) > 0xffffffff) {
                        DEBUG_PRINT("Relocations > 4 gigs away, exiting\n");
                        retcode = 1;
                        goto cleanup;
                    }
                    offsetvalue = ((char*)(bof->sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] + offsetvalue) - (char*)(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tSetting 0x%p to OffsetValue: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(bof->sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                else {
                    DEBUG_PRINT("No code for relocation type: %d\n", coff_reloc_ptr->Type);
                }
#else
                if (coff_reloc_ptr->Type == IMAGE_REL_I386_DIR32 && funcptrlocation != NULL) {
                    /* This is Type == IMAGE_REL_I386_DIR32 relocation code */
                    memcpy(functionMapping + (functionMappingCount * 4), &funcptrlocation, sizeof(uint32_t));
                    offsetvalue = (int32_t)(functionMapping + (functionMappingCount * 4));
                    DEBUG_PRINT("\tSetting 0x%p to virtual address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                    functionMappingCount++;
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_I386_DIR32) {
                    memcpy(&offsetvalue, sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\tReadin OffsetValue : 0x%0X\n", offsetvalue);
                    offsetvalue = (uint32_t)(sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1]) + offsetvalue;
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tSetting 0x%p to virtual address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));
                }
                else if (coff_reloc_ptr->Type == IMAGE_REL_I386_REL32) {
                    memcpy(&offsetvalue, sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, sizeof(int32_t));
                    DEBUG_PRINT("\tReadin OffsetValue : 0x%0X\n", offsetvalue);
                    offsetvalue += (sectionMapping[coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber - 1] - (sectionMapping[counter] + coff_reloc_ptr->VirtualAddress + 4));
                    offsetvalue += coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value;
                    DEBUG_PRINT("\tSetting 0x%p to relative address: 0x%X\n", sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, offsetvalue);
                    memcpy(sectionMapping[counter] + coff_reloc_ptr->VirtualAddress, &offsetvalue, sizeof(uint32_t));

                }
                else {
                    DEBUG_PRINT("No code for relocation type: %d\n", coff_reloc_ptr->Type);
                }
#endif
#endif
            }
            DEBUG_PRINT("\tValueNumber: 0x%X\n", coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].Value);
            DEBUG_PRINT("\tSectionNumber: 0x%X\n", coff_sym_ptr[coff_reloc_ptr->SymbolTableIndex].SectionNumber);
            coff_reloc_ptr = (coff_reloc_t*)(((char*)coff_reloc_ptr) + sizeof(coff_reloc_t));
            DEBUG_PRINT("\n");
        }
        DEBUG_PRINT("\n");
    }

    /* Some debugging code to see what the sections look like in memory */
#if DEBUG
#ifdef _WIN32
    for (tempcounter = 0; tempcounter < coff_header_ptr->NumberOfSections; tempcounter++) {
        DEBUG_PRINT("Section: %u\n", tempcounter);
        if (sectionMapping[tempcounter] != NULL) {
            DEBUG_PRINT("\t");
            for (counter = 0; counter < sectionSize[tempcounter]; counter++) {
                DEBUG_PRINT("%02X ", (uint8_t)(sectionMapping[tempcounter][counter]));
            }
            DEBUG_PRINT("\n");
        }
    }
#endif
#endif

    goto leave;

    /* Cleanup the allocated memory */
#ifdef _WIN32
    cleanup :
    if (bof->sectionMapping) {
        for (tempcounter = 0; tempcounter < bof->coff_header_ptr->NumberOfSections; tempcounter++) {
            if (bof->sectionMapping[tempcounter]) {
                VirtualFree(bof->sectionMapping[tempcounter], 0, MEM_RELEASE);
            }
        }
        free(bof->sectionMapping);
        bof->sectionMapping = NULL;
    }
#ifdef DEBUG
    if (sectionSize) {
        free(sectionSize);
        sectionSize = NULL;
    }
#endif
    if (bof->functionMapping) {
        VirtualFree(bof->functionMapping, 0, MEM_RELEASE);
    }
#endif

leave:
    DEBUG_PRINT("Returning\n");
    return retcode;
}

#ifdef COFF_STANDALONE
int main(int argc, char* argv[]) {
    char* coff_data = NULL;
    unsigned char* arguments = NULL;
    int argumentSize = 0;
#ifdef _WIN32
    char* outdata = NULL;
    int outdataSize = 0;
#endif
    uint32_t filesize = 0;
    int checkcode = 0;
    if (argc < 3) {
        printf("ERROR: %s go /path/to/object/file.o (arguments)\n", argv[0]);
        return 1;
    }

    coff_data = (char*)getContents(argv[2], &filesize);
    if (coff_data == NULL) {
        return 1;
    }
    printf("Got contents of COFF file\n");
    arguments = unhexlify((unsigned char*)argv[3], &argumentSize);
    printf("Running/Parsing the COFF file\n");
    checkcode = RunCOFF(argv[1], (unsigned char*)coff_data, filesize, arguments, argumentSize);
    if (checkcode == 0) {
#ifdef _WIN32
        printf("Ran/parsed the coff\n");
        outdata = BeaconGetOutputData(&outdataSize);
        if (outdata != NULL) {

            printf("Outdata Below:\n\n%s\n", outdata);
        }
#endif
    }
    else {
        printf("Failed to run/parse the COFF file\n");
    }
    if (coff_data) {
        free(coff_data);
    }
    return 0;
}

#endif

bof_fd* load_bof(unsigned char* coff_data, uint32_t filesize)
{
    if (coff_data == NULL || filesize == NULL)
    {
        DEBUG_PRINT("coff_data or filesize cannot be 0");
        return NULL;
    }

    bof_fd* fd = (bof_fd*)calloc(1, sizeof(bof_fd));
    if (fd == NULL)
        return NULL;

    fd->coff_data = (unsigned char*)calloc(1, filesize);
    if (fd->coff_data == NULL)
    {
        DEBUG_PRINT("Error allocating %d bytes", filesize);
        free(fd);
        return NULL;
    }

    memcpy(fd->coff_data, coff_data, filesize);
    fd->coff_size = filesize;

    init_bof(fd);

    return fd;
}

int remove_bof(bof_fd* bof)
{
    if (bof == NULL)
        return -1;

    
    /* Cleanup the allocated memory */
    uint16_t tempcounter = 0;

#ifdef _WIN32
    cleanup :
    if (bof->sectionMapping != NULL) {
        for (tempcounter = 0; tempcounter < bof->coff_header_ptr->NumberOfSections; tempcounter++) {
            if (bof->sectionMapping[tempcounter] != NULL) {
                VirtualFree(bof->sectionMapping[tempcounter], 0, MEM_RELEASE);
            }
        }
        free(bof->sectionMapping);
        bof->sectionMapping = NULL;
    }
#ifdef DEBUG
    if (sectionSize) {
        free(sectionSize);
        sectionSize = NULL;
    }
#endif
    if (bof->functionMapping) {
        VirtualFree(bof->functionMapping, 0, MEM_RELEASE);
    }
    bof->functionMapping = NULL;
    bof->functionMappingCount = 0;
#endif
    return 0;
}