#include <stdint.h>
#include <vector>
#include <string>
#include <beacondbg.h>

#include "clicmd.h"
#include "quit.h"

Quit::Quit(beacondbg* emu, std::vector<std::string> args)
{

}

Quit::~Quit()
{

}

bool Quit::onCommand(beacondbg *emu)
{
    std::exit(0);
}