#include <string>
#include <vector>
#include <iostream>
#include <beacondbg.h>

#include "load.h"

std::string LoadCommand::command()
{
    return std::string("load");
}
    
std::string LoadCommand::help()
{
    return std::string("load coff-path");
}

CliCmd *LoadCommand::create(std::vector<std::string> args)
{
    if (args.size() == 0)
    {
        return nullptr;
    }

    return new LoadCommand(args[0]);
}

LoadCommand::LoadCommand(std::string fileName) : fileName_(fileName) {}

bool LoadCommand::onCommand(beacondbg *emu)
{
    return emu->loadFromFile(fileName_);
}

std::string UnloadCommand::command()
{
    return std::string("unload");
}

std::string UnloadCommand::help()
{
    return std::string("unload beacon-name");
}

CliCmd* UnloadCommand::create(std::vector<std::string> args)
{
    if (args.size() == 0)
    {
        return nullptr;
    }

    return new UnloadCommand(args[0]);
}

UnloadCommand::UnloadCommand(std::string beaconName) : beaconName_(beaconName) {}

bool UnloadCommand::onCommand(beacondbg* emu)
{
    return emu->unload(this->beaconName_);
}

