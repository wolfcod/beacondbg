#include <string>
#include <vector>
#include <iostream>
#include <beacondbg.h>

#include "args.h"

ArgsCommand::ArgsCommand(beacondbg* emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        emu->setError(BeaconError::InvalidArguments);
        return;
    }

    fileName_ = args.at(0);
}

bool ArgsCommand::onCommand(beacondbg* emu)
{
    return emu->loadBofPack(fileName_);
}
