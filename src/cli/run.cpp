#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <beacondbg.h>

#include "run.h"

RunCommand::RunCommand(beacondbg* emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        emu->setError(BeaconError::InvalidArguments);
        return;
    }

    functionName_ = args.at(0);
}

bool RunCommand::onCommand(beacondbg* emu)
{
    std::vector<unsigned char> args;

    if (emu->run(functionName_, args) == false) {
        emu->setError(BeaconError::EntryPointNotFound);
        return false;

    }
    std::cout << std::endl;
    return true;
}
