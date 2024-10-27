#ifndef __CLICMD_H_
#define __CLICMD_H_

#include <string>
#include <optional>

class beacondbg;

class CliCmd;

typedef CliCmd* (*create_callback)(beacondbg* emu, std::vector<std::string> args);

struct CliData
{
    std::string command;
    std::string help;
    create_callback create;
};

class CliCmd
{
public:
    static bool registerCommand(std::initializer_list<CliData*> l);
    static bool registerCommand(CliData* cmd);
    static std::optional<create_callback> getCommand(std::string input, std::vector<std::string> &args);

public:
    virtual bool onCommand(beacondbg *emu) = 0;

    virtual ~CliCmd() = default;
};

/** Help class share same object of clicmd.. they are accessing to the same data */
class Help : public CliCmd
{
    public:
        Help(beacondbg *emu, std::vector<std::string> args);
        ~Help() = default;

        bool onCommand(beacondbg *) override;
};

#endif
