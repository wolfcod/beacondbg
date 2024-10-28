#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <beacondbg.h>

#include "disasm.h"

DisasmCommand::DisasmCommand(beacondbg* emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        emu->setError(BeaconError::InvalidArguments);
        return;
    }

    functionName_ = args.at(0);
}

bool DisasmCommand::onCommand(beacondbg* emu)
{
    std::vector<unsigned char> args;

    if (emu->disassemble(functionName_, args) == false) {
        emu->setError(BeaconError::EntryPointNotFound);
        return false;

    }
    emu->println("");
    return true;
}
