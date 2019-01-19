#include "JoypadController.hpp"
#include "Utils.hpp"
#ifdef __SWITCH__
#include <switch.h>
#endif

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

#ifdef __SWITCH__
void JoypadController::handleJoystickEvent(SDL_Event *event) {
    if (event->type == SDL_JOYBUTTONDOWN) {
        if (event->jbutton.which == 0) {
            if (event->jbutton.button == 0) {
                this->handleKeyPress(4);
            } else if (event->jbutton.button == 1) {
                this->handleKeyPress(5);
            } else if (event->jbutton.button == 13) {
                this->handleKeyPress(2);
            } else if (event->jbutton.button == 15) {
                this->handleKeyPress(3);
            } else if (event->jbutton.button == 12) {
                this->handleKeyPress(1);
            } else if (event->jbutton.button == 14) {
                this->handleKeyPress(0);
            } else if (event->jbutton.button == 10) {
                this->handleKeyPress(7);
            } else if (event->jbutton.button == 11) {
                this->handleKeyPress(6);
            }
        }
    } else if (event->type == SDL_JOYBUTTONUP) {
        if (event->jbutton.which == 0) {
            if (event->jbutton.button == 0) {
                this->handleKeyRelease(4);
            } else if (event->jbutton.button == 1) {
                this->handleKeyRelease(5);
            } else if (event->jbutton.button == 13) {
                this->handleKeyRelease(2);
            } else if (event->jbutton.button == 15) {
                this->handleKeyRelease(3);
            } else if (event->jbutton.button == 12) {
                this->handleKeyRelease(1);
            } else if (event->jbutton.button == 14) {
                this->handleKeyRelease(0);
            } else if (event->jbutton.button == 10) {
                this->handleKeyRelease(7);
            } else if (event->jbutton.button == 11) {
                this->handleKeyRelease(6);
            }
        }
    }
}
#else
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
#endif
