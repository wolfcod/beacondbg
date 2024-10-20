#ifdef _WIN32
#else
typedef void* HANDLE;
typedef void* LPVOID;
#endif

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>

#include "clicmd.h"
#include "breakpoints.h"

/// @brief BREAKPOINT holds basic informations about a breakpoint
struct BREAKPOINT
{
    HANDLE  hProcess;   /* hProcess => Remote process or current process breakpoint */
    LPVOID  lpAddress;
    std::string symbolName; /* breakpoint function name.. can be Library!SymbolName or SymbolName or just it's address */
};


std::string Breakpoint::help()
{
    return std::string("Set a breakpoint");
}

bool Breakpoint::run()
{
    std::cout << "Not yet implemented." << std::endl;
    return true;
}


std::string Breakpoint::command()
{
    return std::string("bp");
}

std::string BreakpointList::help()
{
    return std::string("Return a list of breakpoints.");
}

std::string BreakpointList::command()
{
    return std::string("bl");
}

bool BreakpointList::run()
{
    int i = 0;

    std::cout << "Breakpoint list: " << std::endl;

    for(BREAKPOINT *bp : breakpoints)
    {
        std::cout << "[" << i << "] at address " << bp->symbolName << std::endl;
    }

    return true;
}