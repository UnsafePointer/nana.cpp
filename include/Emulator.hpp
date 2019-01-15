#pragma once
#include "CPUController.hpp"
#include "MemoryController.hpp"
#include "TimerController.hpp"
#include "PPUController.hpp"
#include <map>

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
    std::map<uint8_t, int> instructionCounter;
    void countInstruction(uint8_t opCode);
public:
    Emulator(CPUController &cpuController, TimerController &timerController, InterruptController &interruptController, PPUController &ppuController, Logger &logger, int maxCycles, MemoryController &memoryController);
    ~Emulator();
    MemoryController *memoryController;
    void initialize();
    void emulateFrame();
};
