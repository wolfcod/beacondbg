class Quit
    : public CliCmd
{
public:
    Quit();
    ~Quit();

    std::string help() override;
    std::string command() override;
    bool run() override;
};
