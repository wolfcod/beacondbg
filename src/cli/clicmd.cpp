#include <map>
#include <optional>
#include <sstream>
#include <iostream>
#include <beacondbg.h>
#include <format>

#include "clicmd.h"

static std::map<std::string, CliData*> commands;

bool CliCmd::registerCommand(CliData *cmd)
{
    commands[cmd->command] = cmd;
    return true;
}

bool CliCmd::registerCommand(std::initializer_list<CliData*> l)
{
    for (CliData* c : l)
        registerCommand(c);

    return true;
}

std::optional<create_callback> CliCmd::getCommand(std::string input, std::vector<std::string> &tokens)
{   
    if (input.size() == 0)
        return std::nullopt;

    // stringstream class check1
    std::stringstream check1(input);
    
    std::string intermediate;
    
    // Tokenizing w.r.t. space ' '
    while(std::getline(check1, intermediate, ' ')) {
        tokens.push_back(intermediate);
    }
    
    std::map<std::string, CliData*>::iterator it = commands.find(tokens[0]);

    if (it == commands.end()) {
        return std::nullopt;
    }

    tokens.erase(tokens.begin());

    return it->second->create;
}

Help::Help(beacondbg* emu, std::vector<std::string> args)
{

}

bool Help::onCommand(beacondbg *emu)
{
    std::map<std::string, CliData*>::iterator it = commands.begin();

    for (; it != commands.end(); it++)
    {
        std::string msg = std::format("\033[32m{}\t\033[39m{}", it->second->command, it->second->help);
        emu->println(msg);
    }
    return true;
}
