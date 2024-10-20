
struct BREAKPOINT;

class Breakpoint : public CliCmd
{
    public:
        Breakpoint() = default;
        ~Breakpoint() = default;
    
        std::string help() override;
        std::string command() override;        
        bool run() override;
};

class BreakpointList : public CliCmd
{
    friend class Breakpoint;

    public:
        BreakpointList() = default;
        ~BreakpointList() = default;

        std::string help() override;
        std::string command() override;
        bool run() override;

        void add(const Breakpoint &breakpoint);

    private:
    std::vector<BREAKPOINT*> breakpoints;
};
