#include <tclap/CmdLine.h>
#include <iostream>
#include <string>

int cli()
{
    while(true)
    {
        std::cout << "beacondbg> ";
        std::string input;
        std::getline(std::cin, input);

        if (input.compare("quit") == 0)
            break;
    }
    
    return 0;
}