#include <vector>
#include <stdint.h>
#include <beacondbg.h>

bool beacondbg::load(const std::vector<unsigned char> &content)
{
    /** parse the beacon coff file and proceed to load it */
    return false;
}

bool beacondbg::unload()
{
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