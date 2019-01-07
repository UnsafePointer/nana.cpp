#pragma once
#include "CPUController.hpp"
#include "MemoryController.hpp"
#include "TimerController.hpp"

class Emulator {
private:
    CPUController cpuController;
    TimerController timerController;
    InterruptController interruptController;
    uint8_t executeNextOpCode();
public:
    Emulator();
    ~Emulator();
    MemoryController memoryController;
    void emulateFrame();
};
