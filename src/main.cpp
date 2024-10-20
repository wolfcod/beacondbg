#include <tclap/CmdLine.h>
#include <config.h>
#include <cli.h>

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmdLine("beacondbg", ' ', BEACONDBG_VERSION, true);

    TCLAP::ValueArg<std::string> input("i", "input", "Beacon input file", false, "beacon.obj", "string", cmdLine);
    TCLAP::MultiArg<std::string> bArgs("a", "args", "Beacon arguments", false, "value", cmdLine);

    cmdLine.parse(argc, argv);

    cli();
    return 0;
}