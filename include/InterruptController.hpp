#pragma once
#include "MemoryController.hpp"
#include "CPUController.hpp"
#include <cstdint>

const static uint16_t InterruptRequestRegisterAddress = 0xFF0F;
const static uint16_t InterruptEnabledRegisterAddress = 0xFFFF;

class InterruptController {
private:
    MemoryController *memoryController;
    CPUController *cpuController;
    void executeInterrupt(uint8_t interruptId);
public:
    InterruptController(MemoryController &memoryController, CPUController &cpuController);
    ~InterruptController();
    void requestInterrupt(uint8_t interruptId);
    void clearRequestInterrupt(uint8_t interruptId);
    void executeInterrupts();
};
