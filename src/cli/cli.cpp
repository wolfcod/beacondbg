#include <iostream>
#include <string>
#include <beacondbg.h>

#include "clicmd.h"
#include "quit.h"
#include "breakpoints.h"
#include "load.h"
#include "run.h"

/// <summary>
/// cli -> represents a session..
/// </summary>
/// <param name="emu">a beacondbg session (local or remote)</param>
/// <returns></returns>
int cli(beacondbg *emu)
{
    CliCmd::registerCommand({
        new Help(),
        new Quit(),
        new Breakpoint(),
        new BreakpointList(),
        new LoadCommand(),
        new UnloadCommand(),
        new RunCommand()
    });
    
    while(true)
    {
        emu->prompt();
        std::string input = emu->readln();

        std::optional<CliCmd *> opt = CliCmd::getCommand(input);

        if (!opt) {
            std::string str = "Invalid command " + input;
            emu->println(str);
        } else {
            CliCmd* cmd = *opt;

            if (cmd != nullptr) {
                cmd->onCommand(emu);
                delete cmd;
            }
            else {
                emu->println("Error processing " + input + ".");
            }
        }
    }
    
    return 0;
}