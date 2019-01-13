#include "TimerController.hpp"
#include "Utils.hpp"

TimerController::TimerController(MemoryController &memoryController, InterruptController &interruptController, Logger &logger) : memoryController(&memoryController), interruptController(&interruptController), logger(&logger)  {
    this->dividerRegisterCyclesCounter = 0;
    this->timerCyclesCounter = 0;
}

TimerController::~TimerController() {

}

bool TimerController::isTimerEnabled() {
    uint8_t value = this->memoryController->readMemory8Bit(TimerControllerAddress);
    return testBit(value, 2);
}

void TimerController::updateTimers(uint8_t cycles) {
    this->updateDividerRegister(cycles);

    if (!this->isTimerEnabled()) {
        return;
    }

    this->timerCyclesCounter -= cycles;
    if (this->timerCyclesCounter > 0) {
        return;
    }

    this->setTimerCycleCounter();

    uint8_t timerCounter = this->memoryController->readMemory8Bit(TimerControllerAddress);
    if (timerCounter == 255) {
        uint8_t timerModulator = this->memoryController->readMemory8Bit(timerModulatorAddress);
        this->memoryController->writeMemory(TimerCounterAddress, timerModulator);
        this->interruptController->requestInterrupt(2);
    } else {
        this->memoryController->writeMemory(TimerCounterAddress, timerCounter+1);
    }
}

uint8_t TimerController::clockFrequency() {
    uint8_t value = this->memoryController->readMemory8Bit(TimerControllerAddress);
    return value & 0x3;
}

void TimerController::setTimerCycleCounter() {
    uint8_t frequency = this->clockFrequency();
    switch (frequency) {
        case 0: {
            this->timerCyclesCounter = 1024;
            break;
        }
        case 1: {
            this->timerCyclesCounter = 16;
            break;
        }
        case 2: {
            this->timerCyclesCounter = 64;
            break;
        }
        case 3: {
            this->timerCyclesCounter = 256;
            break;
        }
    }
}

void TimerController::updateDividerRegister(uint8_t cycles) {
    this->dividerRegisterCyclesCounter += cycles;
    if (this->dividerRegisterCyclesCounter >= 256) {
        this->dividerRegisterCyclesCounter = 0;
        uint8_t value = this->memoryController->readMemory8Bit(DividerRegisterAddress);
        this->memoryController->writeMemoryAvoidingTraps(DividerRegisterAddress, value+1);
    }
}
