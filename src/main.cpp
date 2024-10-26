#include <tclap/CmdLine.h>
#include <config.h>
#include <beacondbg.h>
#include <cli.h>
#include <iostream>

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmdLine("beacondbg", ' ', BEACONDBG_VERSION, true);

    TCLAP::ValueArg<std::string> input("i", "input", "Beacon input file", false, "beacon.obj", "string", cmdLine);
    TCLAP::MultiArg<std::string> bArgs("a", "args", "Beacon arguments", false, "value", cmdLine);

    cmdLine.parse(argc, argv);

    beacondbg* emu = beacondbg::create(std::cin, std::cout);

    if (input.isSet()) {
        if (emu->loadFromFile(input.getValue()) == false) {
            return -1;
        }
    }

    cli(emu);
    return 0;
}