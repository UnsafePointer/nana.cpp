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
    Logger *logger;
    uint8_t executeNextOpCode();
    int maxCycles;
    int totalCycles;
public:
    Emulator(CPUController &cpuController, TimerController &timerController, InterruptController &interruptController, PPUController &ppuController, Logger &logger, int maxCycles, MemoryController &memoryController);
    ~Emulator();
    MemoryController *memoryController;
    void initialize();
    void emulateFrame();
};
