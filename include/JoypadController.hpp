#pragma once
#include <cstdint>
#include "InterruptController.hpp"
#include "MemoryController.hpp"
#include "Logger.hpp"

const static uint16_t JoypadRegisterAddress = 0xFF00;

class JoypadController {
private:
    InterruptController *interruptController;
    MemoryController *memoryController;
    Logger *logger;
    uint8_t joypadState;
public:
    JoypadController(InterruptController &interruptController, MemoryController &memoryController, Logger &logger);
    ~JoypadController();
    void handleKeyPress(uint8_t key);
    void handleKeyRelease(uint8_t key);
    uint8_t getJoypadState();
};
