#pragma once
#include "CPU.hpp"
#include "Memory.hpp"

class Emulator {
public:
    CPU cpu;
    Memory memory;
public:
    Emulator();
    ~Emulator();
};
