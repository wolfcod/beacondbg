#ifndef __CLICMD_H_
#define __CLICMD_H_

#include <string>

class CliCmd
{
    public:
        virtual std::string help() = 0;    //
        virtual std::string command() = 0; // command

        static bool registerCommand(CliCmd *cmd);
        static std::optional<CliCmd*> getCommand(std::string input);

        virtual bool run() = 0;
};

/** Help class share same object of clicmd.. they are accessing to the same data */
class Help : public CliCmd
{
    public:
        Help() = default;
        ~Help() = default;

        std::string help() override;
        std::string command() override;

        bool run() override;
};

#define CLI_CMD(x)    CliCmd::registerCommand(new x())

#endif
