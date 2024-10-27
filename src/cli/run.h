#include "clicmd.h"

class RunCommand : public CliCmd
{
public:
    RunCommand(beacondbg* emu, std::vector<std::string> args);
    ~RunCommand() = default;

    bool onCommand(beacondbg* emu) override;

private:
    std::string functionName_;
};
