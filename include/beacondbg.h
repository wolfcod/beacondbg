#ifndef __BEACONDBG_H_
#define __BEACONDBG_H_

#ifdef __cplusplus
#include <vector>

class beacondbg
{
public:
    beacondbg() = default;
    ~beacondbg() = default;

    /** load a beacon in the debugger */
    bool load(const std::vector<unsigned char> &content);

    /** unload the beacon from the memory */
    bool unload();

    /** set a beacon PID used for debugging */
    bool setTargetPid(uint32_t pid);

    bool setBreakPoint(void *address);
    bool clearBreakPoint(void *address);

    bool run(std::string entryPoint, std::vector<unsigned char> args);
private:
    std::vector<unsigned char> beacon;

};
#else
#endif

///
#ifdef __cplusplus
extern "C"
#endif
unsigned char* InternalFunctions[30][2];

#endif