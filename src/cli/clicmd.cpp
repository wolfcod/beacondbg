#include <map>
#include <optional>
#include <sstream>
#include <iostream>
#include <beacondbg.h>
#include <format>

#include "clicmd.h"

std::map<std::string, CliCmd*>   commands;

bool CliCmd::registerCommand(CliCmd *cmd)
{
    commands[cmd->command()] = cmd;
    return true;
}

bool CliCmd::registerCommand(std::initializer_list<CliCmd*> l)
{
    for (CliCmd* c : l)
        registerCommand(c);

    return true;
}

std::optional<CliCmd *> CliCmd::getCommand(std::string input)
{
     // Vector of string to save tokens
    std::vector<std::string> tokens;
    
    // stringstream class check1
    std::stringstream check1(input);
    
    std::string intermediate;
    
    // Tokenizing w.r.t. space ' '
    while(std::getline(check1, intermediate, ' ')) {
        tokens.push_back(intermediate);
    }
    
    std::map<std::string, CliCmd*>::iterator it = commands.find(tokens[0]);

    if (it == commands.end()) {
        return std::nullopt;
    }

    tokens.erase(tokens.begin());

    CliCmd *instance = it->second->create(tokens);
    return instance;
}

bool Help::onCommand(beacondbg *emu)
{
    std::map<std::string, CliCmd*>::iterator it = commands.begin();

    for (; it != commands.end(); it++)
    {
        std::string msg = std::format("{} {}", it->second->command(), it->second->help());
        emu->println(msg);
    }
    return true;
}

CliCmd *Help::create(std::vector<std::string> args)
{
    return new Help();
}

std::string Help::command()
{
    return std::string("help");
}

std::string Help::help()
{
    return std::string("Print a list of commands available");
}