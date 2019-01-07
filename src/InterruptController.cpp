#include "InterruptController.hpp"
#include "Utils.hpp"

InterruptController::InterruptController() {

}

InterruptController::~InterruptController() {

}


void InterruptController::requestInterrupt(uint8_t interruptId) {
    uint8_t value = this->memoryController.readMemory8Bit(InterruptRequestRegisterAddress);
    value = setBit(value, interruptId);
    this->memoryController.writeMemory(InterruptRequestRegisterAddress, value);
}

void InterruptController::clearRequestInterrupt(uint8_t interruptId) {
    uint8_t value = this->memoryController.readMemory8Bit(InterruptRequestRegisterAddress);
    value = clearBit(value, interruptId);
    this->memoryController.writeMemory(InterruptRequestRegisterAddress, value);
}

void InterruptController::executeInterrupts() {
    if (this->cpuController.disableInterrupts) {
        return;
    }
    uint8_t interruptRequest = this->memoryController.readMemory8Bit(InterruptRequestRegisterAddress);
    if (interruptRequest > 0) {
        for (uint8_t bitPosition = 0; bitPosition < 8; bitPosition++) {
            if (testBit(interruptRequest, bitPosition)) {
                uint8_t interruptEnabled = this->memoryController.readMemory8Bit(InterruptEnabledRegisterAddress);
                if (testBit(interruptEnabled, bitPosition)) {
                    this->executeInterrupt(bitPosition);
                }
            }
        }
    }
}

void InterruptController::executeInterrupt(uint8_t interruptId) {
    this->cpuController.pushIntoStack(this->cpuController.programCounter.value());
    this->cpuController.halted = false;

    switch (interruptId) {
        case 0: {
            this->cpuController.programCounter.setValue(0x40);
            break;
        }
        case 1: {
            this->cpuController.programCounter.setValue(0x48);
            break;
        }
        case 2: {
            this->cpuController.programCounter.setValue(0x50);
            break;
        }
        case 3: {
            this->cpuController.programCounter.setValue(0x60);
            break;
        }
    }

    this->cpuController.disableInterrupts = true;
    this->clearRequestInterrupt(interruptId);
}
