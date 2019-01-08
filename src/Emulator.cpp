#include "Emulator.hpp"
#include <iostream>

const static uint64_t MaxCyclesPerSecond = 4194304;
const static uint64_t MaxCyclesPerEmulationCycle = MaxCyclesPerSecond / 60;

Emulator::Emulator() {
}

Emulator::~Emulator() {
}

void Emulator::emulateFrame() {
    uint64_t cyclesThisUpdate = 0;
    while (cyclesThisUpdate < MaxCyclesPerEmulationCycle) {
        uint8_t cycles = this->executeNextOpCode();
        cyclesThisUpdate += cycles;
        this->timerController.updateTimers(cycles);
        this->ppuController.updateScreen(cycles);
        this->interruptController.executeInterrupts();
    }
}

uint8_t Emulator::executeNextOpCode() {
    uint8_t opCode = this->memoryController.readMemory8Bit(this->cpuController.programCounter.value());
    uint8_t cycles = 0;
    if (this->cpuController.halted) {
        cycles = 4;
    } else {
        this->cpuController.programCounter.increment();
        cycles = this->cpuController.executeOpCode(opCode);
        std::cout << "Executing OP:" << std::hex << (int)opCode << std::endl;
    }

    if (this->cpuController.pendingDisableInterrupts && opCode != 0xF3) {
        this->cpuController.pendingDisableInterrupts = false;
        this->cpuController.disableInterrupts = true;
    }

    if (this->cpuController.pendingEnableInterrupts && opCode != 0xFB) {
        this->cpuController.pendingEnableInterrupts = false;
        this->cpuController.disableInterrupts = false;
    }

    return cycles;
}
