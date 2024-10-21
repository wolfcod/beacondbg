#include "clicmd.h"

class RunCommand : public CliCmd
{
public:
    RunCommand() = default;
    ~RunCommand() = default;

    std::string command() override;
    std::string help() override;

    CliCmd* create(std::vector<std::string> args) override;

    bool run(beacondbg* emu) override;

protected:
    RunCommand(std::string functionName);

private:
    std::string functionName_;
};
