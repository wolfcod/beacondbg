#include <tclap/CmdLine.h>
#include <config.h>

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmdLine("beacondbg", ' ', BEACONDBG_VERSION, true);

    cmdLine.parse(argc, argv);

    return 0;
}