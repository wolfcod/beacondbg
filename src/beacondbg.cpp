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

#include "beacondbg.h"
#include "beacon_data.h"


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

typedef BOOL (WINAPI *RunBeacon_ptr)(HANDLE hBeacon, LPCSTR lpProcName, LPVOID lpData, SIZE_T Size);
typedef HANDLE (WINAPI* LoadBeacon_ptr)(LPVOID lpData, SIZE_T Size);
typedef BOOL(WINAPI* CloseBeacon_ptr)(HANDLE hBeacon);

typedef BOOL(WINAPI* FindFirstSymbol_ptr)(HANDLE hBeacon, LPVOID* lpSymbolName, LPVOID* Address);

bool beacondbg::load(const std::string beaconName, const std::vector<unsigned char> &content)
{
    if (beacons.find(beaconName) == beacons.end()) {
        HMODULE hModule = LoadLibraryA("beacon.dll");
        if (hModule == NULL) {
            this->setError(BeaconError::CriticalError);
            return false;
        }

        LoadBeacon_ptr LoadBeacon = (LoadBeacon_ptr)GetProcAddress(hModule, "LoadBeacon");
        
        HANDLE hBeacon = LoadBeacon((LPVOID)content.data(), content.size());

        if (hBeacon != NULL) {
            beacons[beaconName].hModule = hModule;
            beacons[beaconName].name = beaconName;
            beacons[beaconName].hBeacon = hBeacon;
            beaconName_ = beaconName;
            this->setStatus(BeaconStatus::loaded);
        }
        else {
            this->setError(BeaconError::FileNotValid);
        }
    }

    return false;
}

bool beacondbg::loadArgs(const std::vector<unsigned char>& content)
{
    if (beacons.empty()) {
        this->setError(BeaconError::InvalidArguments);
        return false;
    }

    beacon_data &data = beacons.at(0);
    data.args = content;
    return true;
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
 
    std::string beaconName = fileName.substr(pathSeparator, dotExtension - pathSeparator);

    return this->load(beaconName, buffer);
}

bool beacondbg::loadBofPack(const std::string& fileName)
{
    std::ifstream input(fileName, std::ios::binary);

    if (!input) {
        std::string error = std::format("{} is not a valid path.", fileName);
        this->error(error);
        return false;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    return this->loadArgs(buffer);
}

bool beacondbg::unload(const std::string &name)
{
    if (beacons.find(name) != beacons.end()) {
        beacon_data& beacon = beacons[name];
        CloseBeacon_ptr CloseBeacon = (CloseBeacon_ptr)GetProcAddress(beacon.hModule, "CloseBeacon");

        CloseBeacon(beacon.hBeacon);
        beacon.hBeacon = nullptr;
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

    RunBeacon_ptr RunBeacon = (RunBeacon_ptr)GetProcAddress(current_beacon.hModule, "RunBeacon");

    if (args.size() == 0) {
        args = current_beacon.args;
    }

    RunBeacon(current_beacon.hBeacon, (char*)entryPoint.c_str(), args.data(), args.size());
    return true;
}

extern void disassemble(uint64_t addr, beacon_data& beacon);

bool beacondbg::disassemble(std::string symbol, std::vector<unsigned char> args)
{
    unsigned char* beaconData = nullptr; size_t beaconSize = 0;

    if (beacons.find(beaconName_) == beacons.end()) {
        this->error(std::format("{} undefined.", beaconName_));
        return false;
    }

    beacon_data& current_beacon = beacons[beaconName_];

    if (current_beacon.symbols.find(symbol) == current_beacon.symbols.end()) {
        this->error(std::format("symbol {} not found.", symbol));
        return false;
    }

    ::disassemble((uint64_t)current_beacon.symbols[symbol], current_beacon);
    return true;
}

bool beacondbg::enumerateSymbols()
{
    if (beacons.find(beaconName_) == beacons.end()) {
        return false;
    }

    beacon_data& current_beacon = beacons[beaconName_];

    FindFirstSymbol_ptr FindFirstSymbol = (FindFirstSymbol_ptr)GetProcAddress(current_beacon.hModule, "FindFirstSymbol");
    FindFirstSymbol_ptr FindNextSymbol = (FindFirstSymbol_ptr)GetProcAddress(current_beacon.hModule, "FindNextSymbol");

    LPVOID SymbolName; LPVOID SymbolAddress;

    if (FindFirstSymbol(current_beacon.hBeacon, &SymbolName, &SymbolAddress)) do {
        if (SymbolAddress == NULL)
            continue;

        std::string line = std::format("\033[32m{}\t\033[31m0x{:x}", (const char*)SymbolName, (ULONG_PTR)SymbolAddress);
        this->println(line);

        current_beacon.symbols.insert({ std::string((const char*)SymbolName), (ULONG_PTR)SymbolAddress });
    } while (FindNextSymbol(current_beacon.hBeacon, &SymbolName, &SymbolAddress));

    this->print("\033[39m>");
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
    std::string msg;

    if (this->pid != 0) {
        msg += std::format("\033[32m{}\033[39m", this->pid);
    }

    if (this->beaconName_.length() > 0) {
        msg += std::format("\033[32m{}", this->beaconName_);
    }

    msg += "\033[39m>";

    out << msg;
    return true;
}
std::string beacondbg::readln()
{
    std::string line;
    std::getline(in, line);
    return line;
}
