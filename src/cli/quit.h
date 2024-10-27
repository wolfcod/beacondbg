#include "clicmd.h"

class Quit
    : public CliCmd
{
public:
    Quit(beacondbg* emu, std::vector<std::string> args);
    ~Quit();

    bool onCommand(beacondbg *emu) override;
};
