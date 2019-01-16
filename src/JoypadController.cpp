#include "JoypadController.hpp"
#include "Utils.hpp"

JoypadController::JoypadController(InterruptController &interruptController, MemoryController &memoryController, Logger &logger) : interruptController(&interruptController), memoryController(&memoryController), logger(&logger) {
    this->joypadState = 0xFF;
}

JoypadController::~JoypadController() {

}

void JoypadController::handleKeyPress(uint8_t key) {
    bool isKeyPressed = !testBit(this->joypadState, key);
    this->joypadState = clearBit(this->joypadState, key);

    bool isButton = false;
    if (key > 3) {
        isButton = true;
    }

    uint8_t requested = this->memoryController->readMemoryAvoidingTraps(JoypadRegisterAddress);
    bool requestInterrupt = false;

    if (isButton && !testBit(requested, 5)) {
        requestInterrupt = true;
    } else if (!isButton && !testBit(requested, 4)) {
        requestInterrupt = true;
    }

    if (requestInterrupt && !isKeyPressed) {
        this->interruptController->requestInterrupt(4);
    }
}

void JoypadController::handleKeyRelease(uint8_t key) {
    this->joypadState = setBit(this->joypadState, key);
}

uint8_t JoypadController::getJoypadState() {
    uint8_t value = this->memoryController->readMemoryAvoidingTraps(JoypadRegisterAddress);
    value ^= 0xFF;

    if (!testBit(value, 4)) {
        uint8_t top = this->joypadState >> 4;
        top |= 0xF0;
        value &= top;
    } else if (!testBit(value, 5)) {
        uint8_t bottom = this->joypadState & 0xF;
        bottom |= 0xF0;
        value &= bottom;
    }

    return value;
}

void JoypadController::handleKeyboardEvent(SDL_KeyboardEvent *event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->keysym.sym) {
            case 'a': {
                this->handleKeyPress(4);
                break;
            }
            case 's': {
                this->handleKeyPress(5);
                break;
            }
            case SDLK_UP: {
                this->handleKeyPress(2);
                break;
            }
            case SDLK_DOWN: {
                this->handleKeyPress(3);
                break;
            }
            case SDLK_LEFT: {
                this->handleKeyPress(1);
                break;
            }
            case SDLK_RIGHT: {
                this->handleKeyPress(0);
                break;
            }
            case SDLK_RETURN: {
                this->handleKeyPress(7);
                break;
            }
            case SDLK_SPACE: {
                this->handleKeyPress(6);
                break;
            }
        }
    } else if (event->type == SDL_KEYUP) {
        switch (event->keysym.sym) {
            case 'a': {
                this->handleKeyRelease(4);
                break;
            }
            case 's': {
                this->handleKeyRelease(5);
                break;
            }
            case SDLK_UP: {
                this->handleKeyRelease(2);
                break;
            }
            case SDLK_DOWN: {
                this->handleKeyRelease(3);
                break;
            }
            case SDLK_LEFT: {
                this->handleKeyRelease(1);
                break;
            }
            case SDLK_RIGHT: {
                this->handleKeyRelease(0);
                break;
            }
            case SDLK_RETURN: {
                this->handleKeyRelease(7);
                break;
            }
            case SDLK_SPACE: {
                this->handleKeyRelease(6);
                break;
            }
        }
    }
}
