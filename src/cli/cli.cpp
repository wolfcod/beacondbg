#include <iostream>
#include <string>
#include <beacondbg.h>

#include "clicmd.h"
#include "quit.h"
#include "breakpoints.h"
#include "load.h"
#include "run.h"


/** this function return true if the dbg has errors... */
static bool handleError(beacondbg* emu)
{
    if (emu->getError() != BeaconError::none) {
        switch (emu->getError()) {
        case BeaconError::InvalidArguments:
            emu->error("Invalid syntax.");
            break;
        case BeaconError::FileNotFound:
            emu->error("File not found");
            break;
        case BeaconError::EntryPointNotFound:
            emu->error("invalid entry point");
            break;
        default:
            emu->error("Invalid error status.");
            break;
        }

        return true;
    }

    return false;
}
/// <summary>
/// cli -> represents a session..
/// </summary>
/// <param name="emu">a beacondbg session (local or remote)</param>
/// <returns></returns>
int cli(beacondbg *emu)
{
    CliData supportedCommands[7] = {
        { "help", "list of commands supported",[](beacondbg* emu, std::vector<std::string> args) -> CliCmd* {return new Help(emu, args); }},
        { "quit", "terminate session", [](beacondbg* emu, std::vector<std::string> args) ->CliCmd* { return new Quit(emu, args);  }},
        { "bp", "set a breakpoint", [](beacondbg* emu, std::vector<std::string> args) ->CliCmd* { return new Breakpoint(emu, args);  }},
        { "bl", "list all breakspoints", [](beacondbg* emu, std::vector<std::string> args) ->CliCmd* { return new BreakpointList(emu, args);  }},
        { "load", "load a beacon in memory", [](beacondbg *emu, std::vector<std::string> args)->CliCmd* { return new LoadCommand(emu, args);  }},
        { "unload", "unload a beacon from memory", [](beacondbg* emu, std::vector<std::string> args)->CliCmd* { return new UnloadCommand(emu, args); }},
        { "run", "run a beacon (start a session)", [](beacondbg* emu, std::vector<std::string> args)->CliCmd* { return new RunCommand(emu, args); }}
    };


    for (CliData& c : supportedCommands)
        CliCmd::registerCommand(&c);

    emu->setStatus(BeaconStatus::ready);

    while(true)
    {
        emu->setError(BeaconError::none);

        emu->prompt();
        std::string input = emu->readln();

        std::vector<std::string> args;

        std::optional<create_callback> opt = CliCmd::getCommand(input, args);

        if (!opt) {
            std::string str = "Invalid command " + input;
            emu->println(str);
        } else {
            CliCmd* cmd = (*opt)(emu, args);
           
            // process errors
            if (handleError(emu) == false) {
                cmd->onCommand(emu);
                handleError(emu);
            }
            delete cmd;
        }
    }
    
    return 0;
}