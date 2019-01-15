#include "Emulator.hpp"
#include "Utils.hpp"
#include <iostream>

const static uint64_t MaxCyclesPerSecond = 4194304;
const static uint64_t MaxCyclesPerEmulationCycle = MaxCyclesPerSecond / 60;

Emulator::Emulator(CPUController &cpuController, TimerController &timerController, InterruptController &interruptController, PPUController &ppuController, Logger &logger, int maxCycles, MemoryController &memoryController) : cpuController(&cpuController), timerController(&timerController), interruptController(&interruptController), ppuController(&ppuController), logger(&logger), maxCycles(maxCycles), memoryController(&memoryController) {
    this->totalCycles = 0;
}

Emulator::~Emulator() {

}

void Emulator::initialize() {
}

void Emulator::countInstruction(uint8_t opCode) {
    int count = 0;
    if (this->instructionCounter.find(opCode) != this->instructionCounter.end()) {
        count = this->instructionCounter[opCode];
    }
    count++;
    this->instructionCounter[opCode] = count;
}

void Emulator::emulateFrame() {
    uint64_t cyclesThisUpdate = 0;
    while (cyclesThisUpdate < MaxCyclesPerEmulationCycle) {
        uint8_t cycles = this->executeNextOpCode();
        cyclesThisUpdate += cycles;
        this->timerController->updateTimers(cycles);
        this->ppuController->updateScreen(cycles);
        this->interruptController->executeInterrupts();
        if (this->maxCycles != 0) {
            this->totalCycles += cycles;
            std::ostringstream message;
            message << "Total number of cycles: " << this->totalCycles;
            this->logger->logMessage(message.str());
            if (this->totalCycles >= this->maxCycles) {
                exit(0);
            }
        }
    }
}

uint8_t Emulator::executeNextOpCode() {
    uint8_t opCode = this->memoryController->readMemory8Bit(this->cpuController->programCounter.value());
    this->countInstruction(opCode);
    uint8_t cycles = 0;
    if (this->cpuController->halted) {
        cycles = 4;
    } else {
        this->cpuController->programCounter.increment();
        cycles = this->cpuController->executeOpCode(opCode);
        std::ostringstream message;
        message << "OP: " << formatHexUInt8(opCode) << ", Cycles: " << formatDecUInt8(cycles) << ", Program Counter: " << formatHexUInt16((int)this->cpuController->programCounter.value() - 1) << ", Flags: " << formatBinaryUint8(this->cpuController->flags());
        this->logger->logMessage(message.str());
        this->cpuController->logRegisters();
    }

    if (this->cpuController->pendingDisableInterrupts && opCode != 0xF3) {
        this->cpuController->pendingDisableInterrupts = false;
        this->cpuController->disableInterrupts = true;
    }

    if (this->cpuController->pendingEnableInterrupts && opCode != 0xFB) {
        this->cpuController->pendingEnableInterrupts = false;
        this->cpuController->disableInterrupts = false;
    }

    return cycles;
}
