#include <Windows.h>
#include <vector>
#include <stdint.h>
#include <string>
#include <beacondbg.h>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <iterator>
#include <format>
#include <map>
#include <stack>

#include "../coff/loader.h"

typedef struct _beacon_data
{
    std::string name;
    bof_fd *fd;
    std::vector<void*> breakpoints;
    std::vector<std::string> args;
} beacon_data;

std::map<std::string, beacon_data> beacons;

beacondbg::beacondbg(std::istream& input, std::ostream &output)
    : in(input), out(output), pid(0)
{
  
}

/// <summary>
/// create a beacondbg session, locally or remote
/// </summary>
/// <param name="input"></param>
/// <param name="output"></param>
/// <returns></returns>
beacondbg * beacondbg::create(std::istream& input, std::ostream& output)
{
    beacondbg* instance = new beacondbg(input, output);

    instance->setTargetPid(GetCurrentProcessId());
   
    return instance;
}

bool beacondbg::load(const std::string beaconName, const std::vector<unsigned char> &content)
{
    bof_fd *fd = load_bof((unsigned char *)content.data(), content.size());

    if (fd != NULL)
    {
        beacons[beaconName].fd = fd;
        beacons[beaconName].name = beaconName;
    }

    beaconName_ = beaconName;
    return false;
}

bool beacondbg::loadFromFile(const std::string &fileName)
{
    std::ifstream input(fileName, std::ios::binary);

    if (!input) {
        std::string error = std::format("{} is not a valid path.", fileName);
        this->error(error);
        return false;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    size_t pathSeparator = fileName.find_last_of('\\');
    size_t dotExtension = fileName.find_last_of('.');

    if (pathSeparator == std::string::npos) {
        pathSeparator = 0;
    }
    else {
        pathSeparator++;
    }

    if (dotExtension == std::string::npos) {
        dotExtension = fileName.length();
    }
    else {
        dotExtension++;
    }

    std::string beaconName = fileName.substr(pathSeparator, pathSeparator - dotExtension);

    return this->load(beaconName, buffer);
}

bool beacondbg::unload(const std::string &name)
{
    if (beacons.find(name) != beacons.end()) {
        beacon_data& beacon = beacons[name];

        remove_bof(beacon.fd);  // destroy the beacon data
        beacons.erase(name);
        
        return true;
    }
    return false;
}

bool beacondbg::setTargetPid(uint32_t pid)
{
    return false;
}

bool beacondbg::clearBreakPoint(void *address)
{
    return false;
}

bool beacondbg::setBreakPoint(void *address)
{
    return false;
}

/// <summary>
/// run -> Invoke the beacon entry point 
/// </summary>
/// <param name="entryPoint"></param>
/// <param name="args"></param>
/// <returns></returns>
bool beacondbg::run(std::string entryPoint, std::vector<unsigned char> args)
{
    unsigned char* beaconData = nullptr; size_t beaconSize = 0;

    if (beacons.find(beaconName_) == beacons.end()) {
        return false;
    }

    beacon_data& current_beacon = beacons[beaconName_];

    RunCOFF(current_beacon.fd, (char *) entryPoint.c_str(), args.data(), args.size());
    return true;
}

void beacondbg::print(const std::string fmt)
{
    out << fmt;
}

void beacondbg::println(const std::string fmt)
{
    out << fmt;

    if (!fmt.ends_with("\n"))
        out << std::endl;
}

void beacondbg::clear()
{
    out << "\033[2J";
}

void beacondbg::error(const std::string fmt)
{
    out << "\033[31m" << fmt << "\033[39m" << std::endl;
}

bool beacondbg::prompt()
{
    std::string msg = "]";

    if (this->pid != 0) {
        msg += std::format("\033[32m{}\033[39m", this->pid);
    }

    if (this->beaconName_.length() > 0) {
        msg += std::format("{}", this->beaconName_);
    }

    msg += "]> ";

    out << msg;
    return true;
}
std::string beacondbg::readln()
{
    std::string line;
    std::getline(in, line);
    return line;
}
