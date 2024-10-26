#ifndef __BEACONDBG_H_
#define __BEACONDBG_H_

#ifdef __cplusplus
#include <vector>

class beacondbg
{
public:
    static beacondbg* create(std::istream &cin, std::ostream &cout);

public:
    ~beacondbg() = default;

    /** load a beacon in the debugger */
    bool load(const std::string beaconName, const std::vector<unsigned char> &content);
    bool loadFromFile(const std::string &fileName);

    /** unload the beacon from the memory */
    bool unload(const std::string &name);

    /** set a beacon PID used for debugging */
    bool setTargetPid(uint32_t pid);

    bool setBreakPoint(void *address);
    bool clearBreakPoint(void *address);

    bool run(std::string entryPoint, std::vector<unsigned char> args);

    void print(const std::string fmt);
    void println(const std::string fmt);
    void clear();
    void error(const std::string fmt);
    std::string readln();

    bool prompt();
private:
    std::string beaconName_;
    int pid;
    std::istream &in;
    std::ostream &out;

private:
    beacondbg(std::istream& input, std::ostream& output);
};
#else
#endif

///
#ifdef __cplusplus
extern "C"
#endif
unsigned char* InternalFunctions[30][2];

#endif