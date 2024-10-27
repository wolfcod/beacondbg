#include <tclap/CmdLine.h>
#include <config.h>
#include <beacondbg.h>
#include <cli.h>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmdLine("beacondbg", ' ', BEACONDBG_VERSION, true);

    TCLAP::ValueArg<std::string> bof("B", "bof", "BOF input file", false, "beacon.obj", "file", cmdLine);
    TCLAP::ValueArg<std::string> args("a", "args", "argument file", false, "beacon.dat", "file", cmdLine);

    cmdLine.parse(argc, argv);

    beacondbg* emu = beacondbg::create(std::cin, std::cout);

    if (bof.isSet()) {
        if (emu->loadFromFile(bof.getValue()) == false) {
            return -1;
        }
    }

    if (args.isSet()) {
        if (emu->loadBofPack(args.getValue()) == false) {
            return -1;
        }
    }
    std::thread t1(cli, emu);

    t1.join();

    return 0;
}