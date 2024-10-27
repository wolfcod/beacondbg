#include "clicmd.h"

struct BREAKPOINT;

class Breakpoint : public CliCmd
{
public:
    Breakpoint(beacondbg* emu, std::vector<std::string> args);
    ~Breakpoint() = default;

public:
    bool onCommand(beacondbg *emu) override;
};

class BreakpointList : public CliCmd
{
    friend class Breakpoint;

public:
    BreakpointList(beacondbg *emu, std::vector<std::string> args);
    ~BreakpointList() = default;

    void add(const Breakpoint &breakpoint);
    bool onCommand(beacondbg *emu) override;

private:
    std::vector<BREAKPOINT*> breakpoints;
};
