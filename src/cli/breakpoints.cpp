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
#include "clicmd.h"
#include "breakpoints.h"

/// @brief BREAKPOINT holds basic informations about a breakpoint
struct BREAKPOINT
{
    HANDLE  hProcess;   /* hProcess => Remote process or current process breakpoint */
    LPVOID  lpAddress;
    std::string symbolName; /* breakpoint function name.. can be Library!SymbolName or SymbolName or just it's address */
};

Breakpoint::Breakpoint(beacondbg* emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        emu->setError(BeaconError::InvalidArguments);
        return;
    }

    if (emu->getStatus() != BeaconStatus::loaded) {
        emu->setError(BeaconError::InvalidStatus);
        return;
    }

    std::string address = args[0];

    if (args[0].starts_with("0x")) {
        long addr = std::stoul(address, nullptr, 16);
        if (addr == 0) {
            emu->setError(BeaconError::InvalidAddress);
        }
    }
}

bool Breakpoint::onCommand(beacondbg *emu)
{
    emu->error("bp not yet implemented");
    return true;
}

template<typename... Args>
std::string Format(const std::string_view message, Args... formatItems)
{
    return std::vformat(message, std::make_format_args(std::forward<Args>(formatItems)...));
}

BreakpointList::BreakpointList(beacondbg* emu, std::vector<std::string> args)
{
    if (args.size() == 0) {
        //emu->setError(BeaconErrors::InvalidArguments);
        return;
    }

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
