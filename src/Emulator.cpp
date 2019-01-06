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
    }
}

uint8_t Emulator::executeNextOpCode() {
    uint8_t opCode = this->memoryController.readMemory8Bit(this->cpu.programCounter.value());
    uint8_t cycles = 0;
    if (this->cpu.halted) {
        cycles = 4;
    } else {
        this->cpu.programCounter.increment();
        cycles = this->cpu.executeOpCode(opCode);
        std::cout << "Executing OP:" << std::hex << (int)opCode << std::endl;
    }

    if (this->cpu.pendingDisableInterrupts && opCode != 0xF3) {
        this->cpu.pendingDisableInterrupts = false;
        this->cpu.disableInterrupts = true;
    }

    if (this->cpu.pendingEnableInterrupts && opCode != 0xFB) {
        this->cpu.pendingEnableInterrupts = false;
        this->cpu.disableInterrupts = false;
    }

    return cycles;
}
