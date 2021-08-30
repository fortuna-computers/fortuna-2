#include "manager.hh"

int main(int argc, char* argv[])
{
    Manager manager;
    manager.run(argc == 2 ? argv[1] : std::optional<std::string> {});
}
