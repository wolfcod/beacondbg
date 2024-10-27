#ifndef __BEACONDBG_H_
#define __BEACONDBG_H_

#ifdef __cplusplus
#include <vector>

enum class BeaconError
{
    InvalidArguments,
    FileNotFound,
    FileNotValid,
    EntryPointNotFound,
    InvalidStatus,  // this command cannot be executed in the current status
    none    // latest arguments.. everything is fine
};

enum class BeaconStatus
{
    ready,  /* the beacon is ready to accept any commands ... */
    loaded, /* a beacon is loaded */
    running,    /* a beacon is running.. so only breakpoints or terminate commands are accepted */
};

class beacondbg
{
public:
    static beacondbg* create(std::istream &cin, std::ostream &cout);

public:
    ~beacondbg() = default;

    inline void setError(BeaconError value) { this->errorStatus = value; }
    inline BeaconError getError() { return this->errorStatus; }
    inline void setStatus(BeaconStatus value) { this->beaconStatus = value; }
    inline BeaconStatus getStatus() { return this->beaconStatus; }

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
    BeaconError errorStatus;
    BeaconStatus beaconStatus;
};
#else
#endif

///
#ifdef __cplusplus
extern "C"
#endif
unsigned char* InternalFunctions[30][2];

#endif