#include "clicmd.h"

class DisasmCommand : public CliCmd
{
public:
    DisasmCommand(beacondbg* emu, std::vector<std::string> args);
    ~DisasmCommand() = default;

    bool onCommand(beacondbg* emu) override;

private:
    std::string functionName_;
};
