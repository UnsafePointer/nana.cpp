#pragma once
#include "CPUController.hpp"
#include "MemoryController.hpp"
#include "TimerController.hpp"
#include "PPUController.hpp"

class Emulator {
private:
    CPUController *cpuController;
    TimerController *timerController;
    InterruptController *interruptController;
    PPUController *ppuController;
    uint8_t executeNextOpCode();
public:
    Emulator(CPUController &cpuController, TimerController &timerController, InterruptController &interruptController, PPUController &ppuController, MemoryController &memoryController);
    ~Emulator();
    MemoryController *memoryController;
    void initialize();
    void emulateFrame();
};
