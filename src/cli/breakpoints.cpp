#ifdef _WIN32
#include <Windows.h>
#else
typedef void* HANDLE;
typedef void* LPVOID;
#endif

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <beacondbg.h>
#include <format>
#include <string_view>
using namespace std::string_literals;
using namespace std::string_view_literals;

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

bool Breakpoint::onCommand(beacondbg *emu)
{
    std::cout << "Not yet implemented." << std::endl;
    return true;
}


CliCmd* Breakpoint::create(std::vector<std::string> args)
{
    return new Breakpoint();
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

template<typename... Args>
std::string Format(const std::string_view message, Args... formatItems)
{
    return std::vformat(message, std::make_format_args(std::forward<Args>(formatItems)...));
}

bool BreakpointList::onCommand(beacondbg *emu)
{
    int i = 0;

    emu->println("Breakpoint list :");

    for (BREAKPOINT* bp : breakpoints)
    {
        i++;
        std::string symbolName = bp->symbolName;

        // std::string_view Text = "[{:2}] at address {:x}"sv;

        std::string fmt = "[" + std::to_string(i) + "] at address " + symbolName; //Format(Text, i, symbolName);
        emu->println(fmt);
    }

    return true;
}

CliCmd* BreakpointList::create(std::vector<std::string> args)
{
    return new BreakpointList();
}