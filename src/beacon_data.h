typedef struct _beacon_data
{
    HMODULE hModule;
    HANDLE hBeacon;
    std::string name;
    std::vector<void*> breakpoints;
    std::vector<unsigned char> args;
    std::map<std::string, ULONG_PTR> symbols;

} beacon_data;