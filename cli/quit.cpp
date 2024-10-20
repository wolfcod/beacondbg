#include <stdint.h>
#include <vector>
#include <string>
#include <beacondbg.h>

#include "clicmd.h"
#include "quit.h"

Quit::Quit()
{

}

Quit::~Quit()
{

}

std::string Quit::help()
{
    return std::string("Terminate the current application.");

}

std::string Quit::command()
{
    return std::string("quit");
}

CliCmd* Quit::create(std::vector<std::string> args)
{
    return new Quit();
}

bool Quit::run(beacondbg *emu)
{
    std::exit(0);
}