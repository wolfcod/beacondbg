#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <fstream>
#include <beacondbg.h>

#include "run.h"

std::string RunCommand::command()
{
    return std::string("run");
}

std::string RunCommand::help()
{
    return std::string("run the entrypoint of beacon");
}

CliCmd* RunCommand::create(std::vector<std::string> args)
{
    if (args.size() == 0)
    {
        std::cout << "Error. Argument required. function name" << std::endl;
        return nullptr;
    }

    return new RunCommand(args[0]);
}

RunCommand::RunCommand(std::string functionName) : functionName_(functionName) {}

bool RunCommand::onCommand(beacondbg* emu)
{
    std::vector<unsigned char> args;

    emu->run(functionName_, args);
    std::cout << std::endl;
    return true;
}
