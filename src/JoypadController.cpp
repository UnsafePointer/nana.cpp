#include "JoypadController.hpp"
#include "Utils.hpp"

JoypadController::JoypadController(InterruptController &interruptController, MemoryController &memoryController) : interruptController(&interruptController), memoryController(&memoryController) {

}

JoypadController::~JoypadController() {

}

void JoypadController::handleKeyPress(uint8_t key) {
    bool isKeyPressed = testBit(this->joypadState, key);
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
