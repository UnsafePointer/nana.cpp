#pragma once
#include <cstdint>
#include "MemoryController.hpp"
#include "InterruptController.hpp"

const static uint16_t DividerRegisterAddress = 0xFF04;
const static uint16_t TimerCounterAddress = 0xFF05;
const static uint16_t timerModulatorAddress = 0xFF06;
const static uint16_t TimerControllerAddress = 0xFF07;

class TimerController {
private:
    MemoryController *memoryController;
    InterruptController *interruptController;
    int16_t timerCyclesCounter;
    uint16_t dividerRegisterCyclesCounter;
public:
    TimerController(MemoryController &memoryController, InterruptController &interruptController);
    ~TimerController();
    bool isTimerEnabled();
    void updateTimers(uint8_t cycles);
    uint8_t clockFrequency();
    void setTimerCycleCounter();
    void updateDividerRegister(uint8_t cycles);
};
