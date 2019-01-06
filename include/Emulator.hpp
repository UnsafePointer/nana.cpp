#pragma once
#include "CPUController.hpp"
#include "MemoryController.hpp"

class Emulator {
private:
    CPUController cpuController;
    uint8_t executeNextOpCode();
public:
    Emulator();
    ~Emulator();
    MemoryController memoryController;
    void emulateFrame();
};
