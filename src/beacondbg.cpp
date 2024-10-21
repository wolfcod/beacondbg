#include <vector>
#include <stdint.h>
#include <string>
#include <beacondbg.h>
#include "../coff/loader.h"

bool beacondbg::load(const std::vector<unsigned char> &content)
{
    /** parse the beacon coff file and proceed to load it */
    beacon.clear();
    beacon = content;
    return false;
}

bool beacondbg::unload()
{
    beacon.clear();
    return false;
}

bool beacondbg::setTargetPid(uint32_t pid)
{
    return false;
}

bool beacondbg::clearBreakPoint(void *address)
{
    return false;
}

bool beacondbg::setBreakPoint(void *address)
{
    return false;
}

bool beacondbg::run(std::string entryPoint, std::vector<unsigned char> args)
{
    RunCOFF((char *) entryPoint.c_str(), beacon.data(), beacon.size(), args.data(), args.size());
    return true;
}