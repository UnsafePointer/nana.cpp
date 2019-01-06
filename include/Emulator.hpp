#pragma once
#include "CPU.hpp"
#include "MemoryController.hpp"

class Emulator {
private:
    CPU cpu;
    uint8_t executeNextOpCode();
public:
    Emulator();
    ~Emulator();
    MemoryController memoryController;
    void emulateFrame();
};
