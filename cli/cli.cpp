#include <iostream>
#include <string>
#include <beacondbg.h>

#include "clicmd.h"
#include "quit.h"
#include "breakpoints.h"
#include "load.h"

int cli(beacondbg *emu)
{
    CliCmd::registerCommand(new Help());
    CliCmd::registerCommand(new Quit());
    CliCmd::registerCommand(new Breakpoint());
    CliCmd::registerCommand(new BreakpointList());
    CliCmd::registerCommand(new LoadCommand());

    while(true)
    {
        std::cout << "beacondbg> ";
        std::string input;
        std::getline(std::cin, input);

        std::optional<CliCmd *> cmd = CliCmd::getCommand(input);

        if (!cmd) {
            std::cout << "Invalid command " << input << std::endl;
        } else {
            std::cout << "Command " << (*cmd)->help() << " ready." << std::endl;

            if (*cmd != nullptr) {
                (*cmd)->run(emu);
                delete (*cmd);
            }
        }
    }
    
    return 0;
}