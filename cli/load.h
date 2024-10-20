#include "clicmd.h"

class LoadCommand : public CliCmd
{
public:
    LoadCommand() = default;
    ~LoadCommand() = default;

    std::string command() override;
    std::string help() override;

    CliCmd *create(std::vector<std::string> args) override;

    bool run(beacondbg *emu) override;

protected:
    LoadCommand(std::string fileName);

private:
    std::string fileName_;
};
