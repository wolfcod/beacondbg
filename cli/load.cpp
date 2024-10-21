#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <beacondbg.h>

#include "load.h"

std::string LoadCommand::command()
{
    return std::string("load");
}
    
std::string LoadCommand::help()
{
    return std::string("load a beacon in memory");
}

CliCmd *LoadCommand::create(std::vector<std::string> args)
{
    if (args.size() == 0)
    {
        std::cout << "Error. Argument required. load beacon" << std::endl;
        return nullptr;
    }

    return new LoadCommand(args[0]);
}

LoadCommand::LoadCommand(std::string fileName) : fileName_(fileName) {}

bool LoadCommand::run(beacondbg *emu)
{
    std::ifstream input(fileName_, std::ios::binary);

    if (!input) {
        std::cout << "Cannot read the file " << fileName_ << std::endl;
        return false;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    emu->load(buffer);
    return true;
}
