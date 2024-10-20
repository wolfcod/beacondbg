#include <map>
#include <optional>
#include <sstream>
#include <iostream>

#include "clicmd.h"

std::map<std::string, CliCmd*>   commands;

bool CliCmd::registerCommand(CliCmd *cmd)
{
    commands[cmd->command()] = cmd;
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
    
    // Printing the token vector
    for(int i = 0; i < tokens.size(); i++)
        std::cout << tokens[i] << '\n';

    std::map<std::string, CliCmd*>::iterator it = commands.find(tokens[0]);

    if (it == commands.end()) {
        return std::nullopt;
    }

    return it->second;
}

bool Help::run()
{
    std::map<std::string, CliCmd*>::iterator it = commands.begin();

    for(; it != commands.end(); it++)
    {
        std::cout << it->second->command() << " " << it->second->help() << std::endl;
    }
    return true;
}

std::string Help::command()
{
    return std::string("help");
}

std::string Help::help()
{
    return std::string("Print a list of commands available");
}