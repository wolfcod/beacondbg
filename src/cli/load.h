#include "clicmd.h"

class LoadCommand : public CliCmd
{
public:
    LoadCommand() = default;
    ~LoadCommand() = default;

    std::string command() override;
    std::string help() override;

    CliCmd *create(std::vector<std::string> args) override;

    bool onCommand(beacondbg *emu) override;

protected:
    LoadCommand(std::string fileName);

private:
    std::string fileName_;
};

class UnloadCommand : public CliCmd
{
public:
    UnloadCommand() = default;
    ~UnloadCommand() = default;

    std::string command() override;
    std::string help() override;

    CliCmd* create(std::vector<std::string> args) override;

    bool onCommand(beacondbg* emu) override;

protected:
    UnloadCommand(std::string args);

private:
    std::string  beaconName_;
};