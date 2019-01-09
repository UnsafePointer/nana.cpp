#pragma once
#include <cstdint>
#include "InterruptController.hpp"
#include "MemoryController.hpp"

const static uint16_t JoypadRegisterAddress = 0xFF00;

class JoypadController {
private:
    InterruptController interruptController;
    MemoryController memoryController;
    uint8_t joypadState;
public:
    JoypadController();
    ~JoypadController();
    void handleKeyPress(uint8_t key);
    void handleKeyRelease(uint8_t key);
    uint8_t getJoypadState();
};
