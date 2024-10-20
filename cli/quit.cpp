#include <stdint.h>
#include <vector>
#include <string>

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

bool Quit::run()
{
    std::exit(0);
}