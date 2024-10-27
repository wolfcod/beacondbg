#include <string>
#include <vector>
#include <iostream>
#include <beacondbg.h>

#include "load.h"

LoadCommand::LoadCommand(beacondbg *emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        emu->setError(BeaconError::InvalidArguments);
        return;
    }

    fileName_ = args.at(0);
}

bool LoadCommand::onCommand(beacondbg *emu)
{
    return emu->loadFromFile(fileName_);
}

UnloadCommand::UnloadCommand(beacondbg* emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        emu->setError(BeaconError::InvalidArguments);
        return;
    }

    beaconName_ = args.at(0);
}

bool UnloadCommand::onCommand(beacondbg* emu)
{
    return emu->unload(this->beaconName_);
}

