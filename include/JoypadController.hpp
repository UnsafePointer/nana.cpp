#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
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
    void handleKeyPress(uint8_t key);
    void handleKeyRelease(uint8_t key);
public:
    JoypadController(InterruptController &interruptController, MemoryController &memoryController, Logger &logger);
    ~JoypadController();
    uint8_t getJoypadState();
    void handleKeyboardEvent(SDL_KeyboardEvent *event);
};
