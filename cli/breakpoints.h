
struct BREAKPOINT;

class Breakpoint : public CliCmd
{
public:
    Breakpoint() = default;
    ~Breakpoint() = default;

    std::string help() override;
    std::string command() override;   

public:
    CliCmd* create(std::vector<std::string> args) override;
    bool run(beacondbg *emu) override;
};

class BreakpointList : public CliCmd
{
    friend class Breakpoint;

public:
    BreakpointList() = default;
    ~BreakpointList() = default;

    std::string help() override;
    std::string command() override;
    
    CliCmd* create(std::vector<std::string> args) override;
    void add(const Breakpoint &breakpoint);
    bool run(beacondbg *emu) override;

private:
    std::vector<BREAKPOINT*> breakpoints;
};
