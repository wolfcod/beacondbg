#include "clicmd.h"

class LoadCommand : public CliCmd
{
public:
    LoadCommand(beacondbg *emu, std::vector<std::string> args);
    ~LoadCommand() = default;

    bool onCommand(beacondbg *emu) override;

private:
    std::string fileName_;
};

class UnloadCommand : public CliCmd
{
public:
    UnloadCommand(beacondbg *emu, std::vector<std::string> args);
    ~UnloadCommand() = default;

    bool onCommand(beacondbg* emu) override;
private:
    std::string  beaconName_;
};