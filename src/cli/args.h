#include "clicmd.h"

class ArgsCommand : public CliCmd
{
public:
    ArgsCommand(beacondbg* emu, std::vector<std::string> args);
    ~ArgsCommand() = default;

    bool onCommand(beacondbg* emu) override;

private:
    std::string fileName_;
};
