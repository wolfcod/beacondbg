#include <stdint.h>
#include <vector>
#include <string>
#include <beacondbg.h>

#include "clicmd.h"
#include "sym.h"

SymCommand::SymCommand(beacondbg* emu, std::vector<std::string> args)
{

}

SymCommand::~SymCommand()
{

}

bool SymCommand::onCommand(beacondbg* emu)
{
    if (emu->getStatus() == BeaconStatus::ready) {
        emu->error("no beacon loaded");
        return false;
    }

    emu->enumerateSymbols();
    return true;
}