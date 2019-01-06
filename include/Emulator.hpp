#pragma once
#include "CPU.hpp"
#include "Memory.hpp"

class Emulator {
private:
    CPU cpu;
    uint8_t executeNextOpCode();
public:
    Emulator();
    ~Emulator();
    Memory memory;
    void emulateFrame();
};
