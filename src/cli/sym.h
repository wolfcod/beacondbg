#include "clicmd.h"

class SymCommand
    : public CliCmd
{
public:
    SymCommand(beacondbg* emu, std::vector<std::string> args);
    ~SymCommand();

    bool onCommand(beacondbg* emu) override;
};
