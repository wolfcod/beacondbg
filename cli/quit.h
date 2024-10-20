class Quit
    : public CliCmd
{
public:
    Quit();
    ~Quit();

    std::string help() override;
    std::string command() override;

    CliCmd* create(std::vector<std::string> args) override;
    bool run(beacondbg *emu) override;
};
