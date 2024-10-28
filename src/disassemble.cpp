#define ZYDIS_STATIC_BUILD

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <inttypes.h>
#include <Zydis/Zydis.h>
#include <stdint.h>

#include "beacondbg.h"
#include "beacon_data.h"

void disassemble(uint64_t addr, beacon_data& beacon)
{
    CHAR data[4096];

    SIZE_T NumberOfBytes;

    ReadProcessMemory(GetCurrentProcess(), (LPCVOID)addr, data, 4096, &NumberOfBytes);

    // Initialize decoder context
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = 0x007FFFFFFF400000;
    ZyanUSize offset = 0;
    const ZyanUSize length = 4096;
    ZydisDecodedInstruction instruction;
    ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];
    while (ZYAN_SUCCESS(ZydisDecoderDecodeFull(&decoder, data + offset, length - offset,
        &instruction, operands)))
    {
        // Print current instruction pointer.
        printf("%016" PRIX64 "  ", runtime_address);

        // Format & print the binary instruction structure to human-readable format
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, operands,
            instruction.operand_count_visible, buffer, sizeof(buffer), runtime_address, ZYAN_NULL);
        puts(buffer);

        offset += instruction.length;
        runtime_address += instruction.length;

        // stupid check..
        if (data[offset] == 0x00 && data[offset + 1] == 0x00) break;
    }

}