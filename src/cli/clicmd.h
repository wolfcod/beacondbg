#ifndef __CLICMD_H_
#define __CLICMD_H_

#include <string>
#include <optional>

class beacondbg;

class CliCmd
{
public:
    static bool registerCommand(std::initializer_list<CliCmd*> l);
    static bool registerCommand(CliCmd* cmd);
    static std::optional<CliCmd*> getCommand(std::string input);

public:
    virtual std::string help() = 0;    //
    virtual std::string command() = 0; // command

    // This has to be used only in the final instance
    virtual CliCmd* create(std::vector<std::string> args) = 0;

    virtual bool onCommand(beacondbg *emu) = 0;

    virtual ~CliCmd() = default;
};

/** Help class share same object of clicmd.. they are accessing to the same data */
class Help : public CliCmd
{
    public:
        Help() = default;
        ~Help() = default;

        std::string help() override;
        std::string command() override;

        bool onCommand(beacondbg *) override;
        
        CliCmd *create(std::vector<std::string> args) override;

};

#endif
