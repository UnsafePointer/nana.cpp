#include <iostream>
#include <string>
#include "Emulator.hpp"

int main(int argc, char const *argv[])
{
    if (argc <= 1) {
        exit(1);
    }
    std::string gameArg = argv[1];
    Emulator emulator = Emulator();
    emulator.memory.loadCartridge(gameArg);
    return 0;
}
