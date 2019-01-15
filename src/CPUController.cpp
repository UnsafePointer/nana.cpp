#include "CPUController.hpp"
#include "Utils.hpp"

CPUController::CPUController(MemoryController &memoryController, Logger &logger) : memoryController(&memoryController), logger(&logger) {
	this->programCounter = Register16Bit();
	this->programCounter.setValue(0x100);
	this->af = Register16Bit();
	this->af.setValue(0x01B0);
	this->bc = Register16Bit();
	this->bc.setValue(0x0013);
	this->de = Register16Bit();
	this->de.setValue(0x00D8);
	this->hl = Register16Bit();
	this->hl.setValue(0x014D);
	this->stackPointer = Register16Bit();
	this->stackPointer.setValue(0xFFFE);
	this->halted = false;
	this->disableInterrupts = true;
	this->pendingDisableInterrupts = false;
	this->pendingEnableInterrupts = false;
}

CPUController::~CPUController() {

}

uint8_t CPUController::executeOpCode(uint8_t opCode) {
    switch (opCode) {
	// 8-Bit Loads
	// LD nn,n
	case 0x06: {
		return this->cpu8BitRegisterMemoryLoad(this->bc.high);
	}
	case 0x0E: {
		return this->cpu8BitRegisterMemoryLoad(this->bc.low);
	}
	case 0x16: {
		return this->cpu8BitRegisterMemoryLoad(this->de.high);
	}
	case 0x1E: {
		return this->cpu8BitRegisterMemoryLoad(this->de.low);
	}
	case 0x26: {
		return this->cpu8BitRegisterMemoryLoad(this->de.high);
	}
	case 0x2E: {
		return this->cpu8BitRegisterMemoryLoad(this->de.low);
	}
	// LD r1,r2
	case 0x7F: {
		return this->cpu8BitRegisterLoad(this->af.high, this->af.high);
	}
	case 0x78: {
		return this->cpu8BitRegisterLoad(this->af.high, this->bc.high);
	}
	case 0x79: {
		return this->cpu8BitRegisterLoad(this->af.high, this->bc.low);
	}
	case 0x7A: {
		return this->cpu8BitRegisterLoad(this->af.high, this->de.high);
	}
	case 0x7B: {
		return this->cpu8BitRegisterLoad(this->af.high, this->de.low);
	}
	case 0x7C: {
		return this->cpu8BitRegisterLoad(this->af.high, this->hl.high);
	}
	case 0x7D: {
		return this->cpu8BitRegisterLoad(this->af.high, this->hl.low);
	}
	case 0x7E: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->af.high, this->hl.value());
	}
	case 0x40: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->bc.high);
	}
	case 0x41: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->bc.low);
	}
	case 0x42: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->de.high);
	}
	case 0x43: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->de.low);
	}
	case 0x44: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->hl.high);
	}
	case 0x45: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->hl.low);
	}
	case 0x46: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->bc.high, this->hl.value());
	}
	case 0x48: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->bc.high);
	}
	case 0x49: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->bc.low);
	}
	case 0x4A: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->de.high);
	}
	case 0x4B: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->de.low);
	}
	case 0x4C: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->hl.high);
	}
	case 0x4D: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->hl.low);
	}
	case 0x4E: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->bc.low, this->hl.value());
	}
	case 0x50: {
		return this->cpu8BitRegisterLoad(this->de.high, this->bc.high);
	}
	case 0x51: {
		return this->cpu8BitRegisterLoad(this->de.high, this->bc.low);
	}
	case 0x52: {
		return this->cpu8BitRegisterLoad(this->de.high, this->de.high);
	}
	case 0x53: {
		return this->cpu8BitRegisterLoad(this->de.high, this->de.low);
	}
	case 0x54: {
		return this->cpu8BitRegisterLoad(this->de.high, this->hl.high);
	}
	case 0x55: {
		return this->cpu8BitRegisterLoad(this->de.high, this->hl.low);
	}
	case 0x56: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->de.high, this->hl.value());
	}
	case 0x58: {
		return this->cpu8BitRegisterLoad(this->de.low, this->bc.high);
	}
	case 0x59: {
		return this->cpu8BitRegisterLoad(this->de.low, this->bc.low);
	}
	case 0x5A: {
		return this->cpu8BitRegisterLoad(this->de.low, this->de.high);
	}
	case 0x5B: {
		return this->cpu8BitRegisterLoad(this->de.low, this->de.low);
	}
	case 0x5C: {
		return this->cpu8BitRegisterLoad(this->de.low, this->hl.high);
	}
	case 0x5D: {
		return this->cpu8BitRegisterLoad(this->de.low, this->hl.low);
	}
	case 0x5E: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->de.low, this->hl.value());
	}
	case 0x60: {
		return this->cpu8BitRegisterLoad(this->de.high, this->bc.high);
	}
	case 0x61: {
		return this->cpu8BitRegisterLoad(this->de.high, this->bc.low);
	}
	case 0x62: {
		return this->cpu8BitRegisterLoad(this->de.high, this->de.high);
	}
	case 0x63: {
		return this->cpu8BitRegisterLoad(this->de.high, this->de.low);
	}
	case 0x64: {
		return this->cpu8BitRegisterLoad(this->de.high, this->hl.high);
	}
	case 0x65: {
		return this->cpu8BitRegisterLoad(this->de.high, this->hl.low);
	}
	case 0x66: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->de.high, this->hl.value());
	}
	case 0x68: {
		return this->cpu8BitRegisterLoad(this->de.low, this->bc.high);
	}
	case 0x69: {
		return this->cpu8BitRegisterLoad(this->de.low, this->bc.low);
	}
	case 0x6A: {
		return this->cpu8BitRegisterLoad(this->de.low, this->de.high);
	}
	case 0x6B: {
		return this->cpu8BitRegisterLoad(this->de.low, this->de.low);
	}
	case 0x6C: {
		return this->cpu8BitRegisterLoad(this->de.low, this->hl.high);
	}
	case 0x6D: {
		return this->cpu8BitRegisterLoad(this->de.low, this->hl.low);
	}
	case 0x6E: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->de.low, this->hl.value());
	}
	case 0x70: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->bc.high);
	}
	case 0x71: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->bc.low);
	}
	case 0x72: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->de.high);
	}
	case 0x73: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->de.low);
	}
	case 0x74: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->hl.high);
	}
	case 0x75: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->hl.low);
	}
	case 0x36: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		this->memoryController->writeMemory(this->hl.value(), value);
		return 12;
	}
	// LD A,n
	case 0x0A: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->af.high, this->bc.value());
	}
	case 0x1A: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->af.high, this->de.value());
	}
	case 0xFA: {
		uint16_t address = this->memoryController->readMemory16Bit(this->programCounter.value());
		this->programCounter.increment();
		this->programCounter.increment();
		uint8_t value = this->memoryController->readMemory8Bit(address);
		this->af.high.bits = value;
		return 16;
	}
	case 0x3E:{
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		this->af.high.bits = value;
		return 8;
	}
	// LD n,A
	case 0x47: {
		return this->cpu8BitRegisterLoad(this->bc.high, this->af.high);
	}
	case 0x4F: {
		return this->cpu8BitRegisterLoad(this->bc.low, this->af.high);
	}
	case 0x57: {
		return this->cpu8BitRegisterLoad(this->de.high, this->af.high);
	}
	case 0x5F: {
		return this->cpu8BitRegisterLoad(this->de.low, this->af.high);
	}
	case 0x67: {
		return this->cpu8BitRegisterLoad(this->de.high, this->af.high);
	}
	case 0x6F: {
		return this->cpu8BitRegisterLoad(this->de.low, this->af.high);
	}
	case 0x02: {
		return this->cpu8BitRegisterMemoryWrite(this->bc.value(), this->af.high);
	}
	case 0x12: {
		return this->cpu8BitRegisterMemoryWrite(this->de.value(), this->af.high);
	}
	case 0x77: {
		return this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->af.high);
	}
	case 0xEA: {
		uint16_t address = this->memoryController->readMemory16Bit(this->programCounter.value());
		this->programCounter.increment();
		this->programCounter.increment();
		this->memoryController->writeMemory(address, this->af.high.bits);
		return 16;
	}
	// LD A,(C)
	case 0xF2: {
		return this->cpu8BitRegisterMemoryAddressLoad(this->af.high, (0xFF00 + this->bc.low.bits));
	}
	// LD (C),A
	case 0xE2: {
		return this->cpu8BitRegisterMemoryWrite((0xFF00 + this->bc.low.bits), this->af.high);
	}
	// LD A,(HLD)
	// LD A,(HL-)
	// LDD A,(HL)
	case 0x3A: {
		uint8_t cycles = this->cpu8BitRegisterMemoryAddressLoad(this->af.high, this->hl.value());
		this->hl.decrement();
		return cycles;
	}
	// LD (HLD),A
	// LD (HL-),A
	// LDD (HL),A
	case 0x32: {
		uint8_t cycles = this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->af.high);
		this->hl.decrement();
		return cycles;
	}
	// LD A,(HLI)
	// LD A,(HL+)
	// LDI A,(HL)
	case 0x2A: {
		uint8_t cycles = this->cpu8BitRegisterMemoryAddressLoad(this->af.high, this->hl.value());
		this->hl.increment();
		return cycles;
	}
	// LD (HLI),A
	// LD (HL+),A
	// LDI (HL),A
	case 0x22: {
		uint8_t cycles = this->cpu8BitRegisterMemoryWrite(this->hl.value(), this->af.high);
		this->hl.increment();
		return cycles;
	}
	// LDH (n),A
	case 0xE0: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint16_t address = 0xFF00 + value;
		this->memoryController->writeMemory(address, this->af.high.bits);
		return 12;
	}
	// LDH A,(n)
	case 0xF0: {
		uint8_t addressValue = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint16_t address = 0xFF00 + addressValue;
		uint8_t value = this->memoryController->readMemory8Bit(address);
		this->af.high.bits = value;
		return 12;
	}
	// 16-Bit Loads
	// LD n,nn
	case 0x01: {
		return this->cpu16BitRegisterMemoryLoad(this->bc);
	}
	case 0x11: {
		return this->cpu16BitRegisterMemoryLoad(this->de);
	}
	case 0x21:{
		return this->cpu16BitRegisterMemoryLoad(this->hl);
	}
	case 0x31: {
		return this->cpu16BitRegisterMemoryLoad(this->stackPointer);
	}
	// LD SP,HL
	case 0xF9: {
		this->stackPointer.setValue(this->hl.value());
		return 8;
	}
	// LD HL,SP+n
	// LDHL SP,n
	case 0xF8: {
		int8_t signedValue = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		this->clearFlagZ();
		this->clearFlagN();

		uint32_t value = this->stackPointer.value() + signedValue;

		if (value > 0xFFFF) {
			this->setFlagC();
		} else {
			this->clearFlagC();
		}

		if (((this->stackPointer.value()&0xF) + (value&0xF)) > 0xF) {
			this->setFlagH();
		} else {
			this->clearFlagH();
		}

		this->hl.setValue(0x00FFFF & value);

		return 12;
	}
	// LD (nn),SP
	case 0x08: {
		uint16_t value = this->memoryController->readMemory16Bit(this->programCounter.value());
		this->programCounter.increment();
		this->programCounter.increment();
		this->memoryController->writeMemory(value, this->stackPointer.low.bits);
		value++;
		this->memoryController->writeMemory(value, this->stackPointer.high.bits);
		return 20;
	}
	// PUSH nn
	case 0xF5: {
		this->pushIntoStack(this->af.value());
		return 16;
	}
	case 0xC5: {
		this->pushIntoStack(this->bc.value());
		return 16;
	}
	case 0xD5:{
		this->pushIntoStack(this->de.value());
		return 16;
	}
	case 0xE5:{
		this->pushIntoStack(this->hl.value());
		return 16;
	}
	// POP nn
	case 0xF1: {
		this->af.setValue(this->popFromStack());
		return 12;
	}
	case 0xC1: {
		this->bc.setValue(this->popFromStack());
		return 12;
	}
	case 0xD1: {
		this->de.setValue(this->popFromStack());
		return 12;
	}
	case 0xE1: {
		this->hl.setValue(this->popFromStack());
		return 12;
	}
	// 8-Bit ALU
	// ADD A,n
	case 0x87: {
		return this->cpu8BitRegisterAdd(this->af.high, this->af.high.bits, false);
	}
	case 0x80: {
		return this->cpu8BitRegisterAdd(this->af.high, this->bc.high.bits, false);
	}
	case 0x81: {
		return this->cpu8BitRegisterAdd(this->af.high, this->bc.low.bits, false);
	}
	case 0x82: {
		return this->cpu8BitRegisterAdd(this->af.high, this->de.high.bits, false);
	}
	case 0x83: {
		return this->cpu8BitRegisterAdd(this->af.high, this->de.low.bits, false);
	}
	case 0x84: {
		return this->cpu8BitRegisterAdd(this->af.high, this->hl.high.bits, false);
	}
	case 0x85: {
		return this->cpu8BitRegisterAdd(this->af.high, this->hl.low.bits, false);
	}
	case 0x86: {
		uint8_t cycles = this->cpu8BitRegisterAdd(this->af.high, this->memoryController->readMemory8Bit(this->hl.value()), false);
		cycles += 4; // 8
		return cycles;
	}
	case 0xC6: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitRegisterAdd(this->af.high, value, false);
		cycles += 4; // 8
		return cycles;
	}
	// ADC A,n
	case 0x8F: {
		return this->cpu8BitRegisterAdd(this->af.high, this->af.high.bits, true);
	}
	case 0x88: {
		return this->cpu8BitRegisterAdd(this->af.high, this->bc.high.bits, true);
	}
	case 0x89: {
		return this->cpu8BitRegisterAdd(this->af.high, this->bc.low.bits, true);
	}
	case 0x8A: {
		return this->cpu8BitRegisterAdd(this->af.high, this->de.high.bits, true);
	}
	case 0x8B: {
		return this->cpu8BitRegisterAdd(this->af.high, this->de.low.bits, true);
	}
	case 0x8C: {
		return this->cpu8BitRegisterAdd(this->af.high, this->hl.high.bits, true);
	}
	case 0x8D: {
		return this->cpu8BitRegisterAdd(this->af.high, this->hl.low.bits, true);
	}
	case 0x8E: {
		uint8_t cycles = this->cpu8BitRegisterAdd(this->af.high, this->memoryController->readMemory8Bit(this->hl.value()), true);
		cycles += 4; // 8
		return cycles;
	}
	case 0xCE: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitRegisterAdd(this->af.high, value, true);
		cycles += 4; // 8
		return cycles;
	}
	// SUB A,n
	case 0x97: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->af.high.bits, false);
	}
	case 0x90: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->bc.high.bits, false);
	}
	case 0x91: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->bc.low.bits, false);
	}
	case 0x92: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->de.high.bits, false);
	}
	case 0x93: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->de.low.bits, false);
	}
	case 0x94: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->hl.high.bits, false);
	}
	case 0x95: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->hl.low.bits, false);
	}
	case 0x96: {
		uint8_t cycles = this->cpu8BitRegisterSubtract(this->af.high, this->memoryController->readMemory8Bit(this->hl.value()), false);
		cycles += 4; // 8
		return cycles;
	}
	case 0xD6: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitRegisterSubtract(this->af.high, value, false);
		cycles += 4; // 8
		return cycles;
	}
	// SBC A,n
	case 0x9F: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->af.high.bits, true);
	}
	case 0x98: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->bc.high.bits, true);
	}
	case 0x99: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->bc.low.bits, true);
	}
	case 0x9A: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->de.high.bits, true);
	}
	case 0x9B: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->de.low.bits, true);
	}
	case 0x9C: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->hl.high.bits, true);
	}
	case 0x9D: {
		return this->cpu8BitRegisterSubtract(this->af.high, this->hl.low.bits, true);
	}
	case 0x9E: {
		uint8_t cycles = this->cpu8BitRegisterSubtract(this->af.high, this->memoryController->readMemory8Bit(this->hl.value()), true);
		cycles += 4; // 8
		return cycles;
	}
	case 0xDE: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitRegisterSubtract(this->af.high, value, true);
		cycles += 4; // 8
		return cycles;
	}
	// AND n
	case 0xA7: {
		return this->cpu8BitAnd(this->af.high.bits);
	}
	case 0xA0: {
		return this->cpu8BitAnd(this->bc.high.bits);
	}
	case 0xA1: {
		return this->cpu8BitAnd(this->bc.low.bits);
	}
	case 0xA2: {
		return this->cpu8BitAnd(this->de.high.bits);
	}
	case 0xA3: {
		return this->cpu8BitAnd(this->de.low.bits);
	}
	case 0xA4: {
		return this->cpu8BitAnd(this->hl.high.bits);
	}
	case 0xA5: {
		return this->cpu8BitAnd(this->hl.low.bits);
	}
	case 0xA6: {
		uint8_t value = this->memoryController->readMemory8Bit(this->hl.value());
		uint8_t cycles = this->cpu8BitAnd(value);
		cycles += 4;
		return cycles;
	}
	case 0xE6: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitAnd(value);
		cycles += 4;
		return cycles;
	}
	// OR n
	case 0xB7: {
		return this->cpu8BitOr(this->af.high.bits);
	}
	case 0xB0: {
		return this->cpu8BitOr(this->bc.high.bits);
	}
	case 0xB1: {
		return this->cpu8BitOr(this->bc.low.bits);
	}
	case 0xB2: {
		return this->cpu8BitOr(this->de.high.bits);
	}
	case 0xB3: {
		return this->cpu8BitOr(this->de.low.bits);
	}
	case 0xB4: {
		return this->cpu8BitOr(this->hl.high.bits);
	}
	case 0xB5: {
		return this->cpu8BitOr(this->hl.low.bits);
	}
	case 0xB6:{
		uint8_t value = this->memoryController->readMemory8Bit(this->hl.value());
		uint8_t cycles = this->cpu8BitOr(value);
		cycles += 4;
		return cycles;
	}
	case 0xF6: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitOr(value);
		cycles += 4;
		return cycles;
	}
	// XOR n
	case 0xAF: {
		return this->cpu8BitXor(this->af.high.bits);
	}
	case 0xA8: {
		return this->cpu8BitXor(this->bc.high.bits);
	}
	case 0xA9: {
		return this->cpu8BitXor(this->bc.low.bits);
	}
	case 0xAA: {
		return this->cpu8BitXor(this->de.high.bits);
	}
	case 0xAB: {
		return this->cpu8BitXor(this->de.low.bits);
	}
	case 0xAC: {
		return this->cpu8BitXor(this->hl.high.bits);
	}
	case 0xAD: {
		return this->cpu8BitXor(this->hl.low.bits);
	}
	case 0xAE: {
		uint8_t value = this->memoryController->readMemory8Bit(this->hl.value());
		uint8_t cycles = this->cpu8BitXor(value);
		cycles += 4;
		return cycles;
	}
	case 0xEE: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitXor(value);
		cycles += 4;
		return cycles;
	}
	// CP n
	case 0xBF: {
		return this->cpu8BitCompare(this->af.high.bits);
	}
	case 0xB8: {
		return this->cpu8BitCompare(this->bc.high.bits);
	}
	case 0xB9: {
		return this->cpu8BitCompare(this->bc.low.bits);
	}
	case 0xBA: {
		return this->cpu8BitCompare(this->de.high.bits);
	}
	case 0xBB: {
		return this->cpu8BitCompare(this->de.low.bits);
	}
	case 0xBC: {
		return this->cpu8BitCompare(this->hl.high.bits);
	}
	case 0xBD: {
		return this->cpu8BitCompare(this->hl.low.bits);
	}
	case 0xBE: {
		uint8_t value = this->memoryController->readMemory8Bit(this->hl.value());
		uint8_t cycles = this->cpu8BitCompare(value);
		cycles += 4;
		return cycles;
	}
	case 0xFE: {
		uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		uint8_t cycles = this->cpu8BitCompare(value);
		cycles += 4;
		return cycles;
	}
	// INC n
	case 0x3C: {
		return this->cpu8BitRegisterIncrement(this->af.high);
	}
	case 0x04: {
		return this->cpu8BitRegisterIncrement(this->bc.high);
	}
	case 0x0C: {
		return this->cpu8BitRegisterIncrement(this->bc.low);
	}
	case 0x14: {
		return this->cpu8BitRegisterIncrement(this->de.high);
	}
	case 0x1C: {
		return this->cpu8BitRegisterIncrement(this->de.low);
	}
	case 0x24: {
		return this->cpu8BitRegisterIncrement(this->hl.high);
	}
	case 0x2C: {
		return this->cpu8BitRegisterIncrement(this->hl.low);
	}
	case 0x34: {
		return this->cpu8BitIncrementMemoryAddress(this->hl.value());
	}
	// DEC n
	case 0x3D: {
		return this->cpu8BitRegisterDecrement(this->af.high);
	}
	case 0x05: {
		return this->cpu8BitRegisterDecrement(this->bc.high);
	}
	case 0x0D: {
		return this->cpu8BitRegisterDecrement(this->bc.low);
	}
	case 0x15: {
		return this->cpu8BitRegisterDecrement(this->de.high);
	}
	case 0x1D: {
		return this->cpu8BitRegisterDecrement(this->de.low);
	}
	case 0x25: {
		return this->cpu8BitRegisterDecrement(this->hl.high);
	}
	case 0x2D: {
		return this->cpu8BitRegisterDecrement(this->hl.low);
	}
	case 0x35: {
		return this->cpu8BitDecrementMemoryAddress(this->hl.value());
	}
	// 16-Bit ALU
	// ADD HL,n
	case 0x09: {
		return this->cpu16BitRegisterAdd(this->hl, this->bc);
	}
	case 0x19: {
		return this->cpu16BitRegisterAdd(this->hl, this->de);
	}
	case 0x29: {
		return this->cpu16BitRegisterAdd(this->hl, this->hl);
	}
	case 0x39: {
		return this->cpu16BitRegisterAdd(this->hl, this->stackPointer);
	}
	// ADD SP,n
	case 0xE8: {
		int8_t signedValue = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		this->clearFlagZ();
		this->clearFlagN();

		uint32_t value = this->stackPointer.value() + signedValue;

		if (value > 0xFFFF) {
			this->setFlagC();
		} else {
			this->clearFlagC();
		}

		if (((this->stackPointer.value()&0xF)) + (value&0xF) > 0xF) {
			this->setFlagH();
		} else {
			this->clearFlagH();
		}

		this->stackPointer.setValue(0x00FFFF & value);
		return 16;
	}
	// INC nn
	case 0x03: {
		return this->cpu16BitRegisterIncrement(this->bc);
	}
	case 0x13: {
		return this->cpu16BitRegisterIncrement(this->de);
	}
	case 0x23: {
		return this->cpu16BitRegisterIncrement(this->hl);
	}
	case 0x33: {
		return this->cpu16BitRegisterIncrement(this->stackPointer);
	}
	// DEC nn
	case 0x0B: {
		return this->cpu16BitRegisterDecrement(this->bc);
	}
	case 0x1B: {
		return this->cpu16BitRegisterDecrement(this->de);
	}
	case 0x2B: {
		return this->cpu16BitRegisterDecrement(this->hl);
	}
	case 0x3B: {
		return this->cpu16BitRegisterDecrement(this->stackPointer);
	}
	// Extended Instruction Set
	case 0xCB: {
		uint8_t code = this->memoryController->readMemory8Bit(this->programCounter.value());
		this->programCounter.increment();
		return this->executeExtendedOpCode(code);
	}
	// Miscellaneous
	// DDA
	case 0x27: {
		return this->cpuDDA();
	}
	// CPL
	case 0x2F: {
		uint8_t value = this->af.high.bits;
		this->af.high.bits = value ^ 0xFF;
		this->setFlagN();
		this->setFlagH();
		return 4;
	}
	// CCF
	case 0x3F: {
		if (this->flagC()) {
			this->clearFlagC();
		} else {
			this->setFlagC();
		}
		this->clearFlagN();
		this->clearFlagH();
		return 4;
	}
	// SCF
	case 0x37: {
		this->setFlagC();
		this->clearFlagN();
		this->clearFlagH();
		return 4;
	}
	// NOP
	case 0x00: {
		return 4;
	}
	// HALT
	case 0x76: {
		this->halted = true;
		return 4;
	}
	// STOP - TODO: Check if anything else is needed to handle this properly
	case 0x10: {
		this->programCounter.increment(); // 00
		return 4;
	}
	// DI
	case 0xF3: {
		this->pendingDisableInterrupts = true;
		return 4;
	}
	// EI
	case 0xFB: {
		this->pendingEnableInterrupts = true;
		return 4;
	}
	// Rotates & Shifts
	// RLCA
	case 0x07: {
		uint8_t cycles = this->cpu8BitRegisterRLC(this->af.high);
		cycles -= 4;
		return cycles;
	}
	// RLA
	case 0x17: {
		uint8_t cycles = this->cpu8BitRegisterRL(this->af.high);
		cycles -= 4;
		return cycles;
	}
	// RRCA
	case 0x0F: {
		uint8_t cycles = this->cpu8BitRegisterRRC(this->af.high);
		cycles -= 4;
		return cycles;
	}
	// RRA
	case 0x1F: {
		uint8_t cycles = this->cpu8BitRegisterRR(this->af.high);
		cycles -= 4;
		return cycles;
	}
	// JP nn
	case 0xC3: {
		uint16_t address = this->memoryController->readMemory16Bit(this->programCounter.value());
		return this->cpu8BitJump(address);
	}
	// JP cc,nn
	case 0xC2: {
		return this->cpu8BitJumpConditional(this->flagZ() == false);
	}
	case 0xCA: {
		return this->cpu8BitJumpConditional(this->flagZ() == true);
	}
	case 0xD2: {
		return this->cpu8BitJumpConditional(this->flagC() == false);
	}
	case 0xDA: {
		return this->cpu8BitJumpConditional(this->flagC() == true);
	}
	// JP (HL)
	case 0xE9: {
		uint8_t cycles = this->cpu8BitJump(this->hl.value());
		cycles -= 8; // 4
		return cycles;
	}
	// JR n
	case 0x18: {
		return this->cpu8BitJumpAddConditional(true);
	}
	// JR cc,nn
	case 0x20: {
		return this->cpu8BitJumpAddConditional(this->flagZ() == false);
	}
	case 0x28: {
		return this->cpu8BitJumpAddConditional(this->flagZ() == true);
	}
	case 0x30: {
		return this->cpu8BitJumpAddConditional(this->flagC() == false);
	}
	case 0x38: {
		return this->cpu8BitJumpAddConditional(this->flagC() == true);
	}
	// CALL nn
	case 0xCD: {
		return this->cpu8BitCall(true);
	}
	// CALL cc,nn
	case 0xC4: {
		return this->cpu8BitCall(this->flagZ() == false);
	}
	case 0xCC: {
		return this->cpu8BitCall(this->flagZ() == true);
	}
	case 0xD4: {
		return this->cpu8BitCall(this->flagC() == false);
	}
	case 0xDC: {
		return this->cpu8BitCall(this->flagC() == true);
	}
	// RST n
	case 0xC7: {
		return this->cpu8BitRestart(0x00);
	}
	case 0xCF: {
		return this->cpu8BitRestart(0x08);
	}
	case 0xD7: {
		return this->cpu8BitRestart(0x10);
	}
	case 0xDF: {
		return this->cpu8BitRestart(0x18);
	}
	case 0xE7: {
		return this->cpu8BitRestart(0x20);
	}
	case 0xEF: {
		return this->cpu8BitRestart(0x28);
	}
	case 0xF7: {
		return this->cpu8BitRestart(0x30);
	}
	case 0xFF: {
		return this->cpu8BitRestart(0x38);
	}
	// RET
	case 0xC9: {
		return this->cpu8BitReturn(true);
	}
	// RET cc
	case 0xC0: {
		return this->cpu8BitReturn(this->flagZ() == false);
	}
	case 0xC8: {
		return this->cpu8BitReturn(this->flagZ() == true);
	}
	case 0xD0: {
		return this->cpu8BitReturn(this->flagC() == false);
	}
	case 0xD8: {
		return this->cpu8BitReturn(this->flagC() == true);
	}
	// RETI
	case 0xD9: {
		return this->cpu8BitReturnEnableInterrupts();
	}
	}
	return 0;
}

uint8_t CPUController::cpu8BitRegisterMemoryLoad(Register8Bit &r) {
    uint8_t value = this->memoryController->readMemory8Bit(this->programCounter.value());
	r.bits = value;
	this->programCounter.increment();
	return 8;
}

uint8_t CPUController::cpu8BitRegisterLoad(Register8Bit &r, Register8Bit v) {
	r.bits = v.bits;
    return 4;
}

uint8_t CPUController::cpu8BitRegisterMemoryWrite(uint16_t address, Register8Bit v) {
    this->memoryController->writeMemory(address, v.bits);
	return 8;
}

uint8_t CPUController::cpu8BitRegisterMemoryAddressLoad(Register8Bit &r, uint16_t address) {
    r.bits = this->memoryController->readMemory8Bit(address);
	return 8;
}

uint8_t CPUController::cpu16BitRegisterMemoryLoad(Register16Bit &r) {
    uint16_t value = this->memoryController->readMemory16Bit(this->programCounter.value());
	this->programCounter.increment();
	this->programCounter.increment();
	r.setValue(value);
	return 12;
}

uint8_t CPUController::cpu8BitRegisterAdd(Register8Bit &r, uint8_t addend, bool useCarry) {
    uint8_t augend = r.bits;
	if (useCarry && this->flagC()) {
		addend++;
	}
	uint16_t result = (augend&0xFF) + (addend&0xFF);
	r.bits = (result&0xFF);
	this->clearAllFlags();
	if (r.bits == 0) {
		this->setFlagZ();
	}
	if ((augend&0xF) + (addend&0xF) > 0xF) {
		this->setFlagH();
	}
	if (result > 0xFF) {
		this->setFlagC();
	}
	return 4;
}

uint8_t CPUController::cpu8BitRegisterSubtract(Register8Bit &r, uint8_t minuend, bool useCarry) {
    uint8_t subtrahend = r.bits;
	if (useCarry && this-flagC()) {
		minuend ++;
	}
	uint16_t result = (subtrahend&0xFF) - (minuend&0xFF);
	r.bits = result&0xFF;
	this->clearAllFlags();
	if (r.bits == 0) {
		this->setFlagZ();
	}
	this->setFlagN();
	if ((subtrahend&0xF) - (minuend&0xF) < 0X0) {
		this->setFlagH();
	}
	if (subtrahend < minuend) {
		this->flagC();
	}
	return 4;
}

uint8_t CPUController::cpu8BitAnd(uint8_t operand) {
	uint8_t result = this->af.high.bits & operand;
	this->af.high.bits = result;
	this->clearAllFlags();
	if (this->af.high.bits == 0x0) {
		this->setFlagZ();
	}
	this->setFlagH();
    return 4;
}

uint8_t CPUController::cpu8BitOr(uint8_t operand) {
    uint8_t result = this->af.high.bits | operand;
	this->af.high.bits = result;
	this->clearAllFlags();
	if (this->af.high.bits == 0x0) {
		this->setFlagZ();
	}
	return 4;
}

uint8_t CPUController::cpu8BitXor(uint8_t operand) {
    uint8_t result = this->af.high.bits ^ operand;
	this->af.high.bits = result;
	this->clearAllFlags();
	if (this->af.high.bits == 0x0) {
		this->setFlagZ();
	}
	return 4;
}

uint8_t CPUController::cpu8BitCompare(uint8_t operand) {
    uint8_t result = this->af.high.bits - operand;
	this->clearAllFlags();
	if (result == 0x0) {
		this->setFlagZ();
	}
	this->setFlagN();
	if ((this->af.high.bits&0xF) - (operand&0xF) < 0x0) {
		this->setFlagH();
	}
	if (this->af.high.bits < operand) {
		this->setFlagC();
	}
	return 4;
}

uint8_t CPUController::cpu8BitRegisterIncrement(Register8Bit &r) {
    uint8_t previous = r.bits;
	r.bits = r.bits + 1;
	if (r.bits == 0x0) {
		this->setFlagZ();
	} else {
		this->clearFlagZ();
	}
	this->clearFlagN();
	if ((previous&0xF) == 0xF) {
		this->setFlagH();
	} else {
		this->clearFlagH();
	}
	return 4;
}

uint8_t CPUController::cpu8BitIncrementMemoryAddress(uint16_t address) {
    uint8_t previous = this->memoryController->readMemory8Bit(address);
	uint8_t current = previous + 1;
	this->memoryController->writeMemory(address, current);
	if (current == 0x0) {
		this->setFlagZ();
	} else {
		this->clearFlagZ();
	}
	this->clearFlagN();
	if ((previous&0xF) == 0xF) {
		this->setFlagH();
	} else {
		this->clearFlagH();
	}
	return 12;
}

uint8_t CPUController::cpu8BitRegisterDecrement(Register8Bit &r) {
	uint8_t previous = r.bits;
	r.bits = r.bits - 1;
	if (r.bits == 0x0) {
		this->setFlagZ();
	} else {
		this->clearFlagZ();
	}
	this->setFlagN();
	if ((previous&0x0F) == 0x0) {
		this->setFlagH();
	} else {
		this->clearFlagH();
	}
	return 4;
}

uint8_t CPUController::cpu8BitDecrementMemoryAddress(uint16_t address) {
	uint8_t previous = this->memoryController->readMemory8Bit(address);
	uint8_t current = previous - 1;
	this->memoryController->writeMemory(address, current);
	if (current == 0x0) {
		this->setFlagZ();
	} else {
		this->clearFlagZ();
	}
	this->setFlagN();
	if ((previous&0x0F) == 0x0) {
		this->setFlagH();
	} else {
		this->clearFlagH();
	}
    return 12;
}

uint8_t CPUController::cpu16BitRegisterAdd(Register16Bit &r1, Register16Bit r2) {
    uint16_t augend = r1.value();
	uint16_t addend = r2.value();
	uint32_t result = (augend&0xFFFF) + (addend&0xFFFF);
	r1.setValue(result&0xFFFF);
	this->clearFlagN();
	if (result > 0xFFFF) {
		this->setFlagC();
	} else {
		this->clearFlagC();
	}
	if (((augend&0xFF00)&0xF)+((addend>>8)&0xF) != 0) {
		this->setFlagH();
	} else {
		this->clearFlagH();
	}
	return 8;
}

uint8_t CPUController::cpu16BitRegisterIncrement(Register16Bit &r) {
	r.setValue(r.value() + 1);
    return 8;
}

uint8_t CPUController::cpu16BitRegisterDecrement(Register16Bit &r) {
    r.setValue(r.value() - 1);
	return 8;
}

uint8_t CPUController::cpu8BitRegisterSwap(Register8Bit &r) {
	uint8_t result = (r.bits&0xF0)>>4 | (r.bits&0x0F)<<4;
	r.bits = result;
	this->clearAllFlags();
	if (result == 0x0) {
		this->setFlagZ();
	}
	return 8;
}

uint8_t CPUController::cpu8BitSwapMemoryAddress(uint16_t address)  {
	uint8_t value = this->memoryController->readMemory8Bit(address);
	uint8_t result = (value&0xF0)>>4 | (value&0x0F)<<4;
	this->memoryController->writeMemory(address, result);
	this->clearAllFlags();
	if (result == 0x0) {
		this->setFlagZ();
	}
    return 16;
}

uint8_t CPUController::cpuDDA() {
	uint16_t value = this->af.high.bits;
	if (this->flagN()) {
		if (this->flagH()) {
			value = (value - 0x06) & 0xFF;
		}
		if (this->flagC()) {
			value = value - 0x60;
		}
	} else {
		if (this->flagH() || (value & 0x0F) > 9) {
			value = value + 0x06;
		}
		if (this->flagC() || value > 0x9F) {
			value = value + 0x60;
		}
	}
	if (value >= 0x100) {
		this->setFlagC();
	}
	this->af.high.bits = value&0xFF;
	if (this->af.high.bits == 0) {
		this->setFlagZ();
	} else {
		this->clearFlagZ();
	}
	this->clearFlagZ();
    return 4;
}

uint8_t CPUController::cpu8BitRegisterRLC(Register8Bit &r) {
    bool test = testBit(r.bits, 7);
	r.bits = r.bits << 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
		r.bits = setBit(r.bits, 0);
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
	return 8;
}

uint8_t CPUController::cpu8BitRegisterRL(Register8Bit &r) {
	bool testCarry = this->flagC();
	bool test = testBit(r.bits, 7);
	r.bits = r.bits << 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (testCarry) {
		r.bits = setBit(r.bits, 0);
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
    return 8;
}

uint8_t CPUController::cpu8BitRegisterRRC(Register8Bit &r) {
	uint8_t test = testBit(r.bits, 0);
	r.bits = r.bits >> 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
		r.bits = setBit(r.bits, 7);
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
    return 8;
}

uint8_t CPUController::cpu8BitRegisterRR(Register8Bit &r) {
    bool testCarry = this->flagC();
	bool test = testBit(r.bits, 0);
	r.bits = r.bits >> 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (testCarry) {
		r.bits = setBit(r.bits, 7);
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
	return 8;
}

uint8_t CPUController::cpu8BitRegisterRLCMemoryAddress(uint16_t address) {
	uint8_t value = this->memoryController->readMemory8Bit(address);
	bool test = testBit(value, 7);
	value <<= 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
		value = setBit(value, 0);
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
    return 16;
}

uint8_t CPUController::cpu8BitRegisterRLMemoryAddress(uint16_t address) {
	uint8_t value = this->memoryController->readMemory8Bit(address);
	bool testCarry = this->flagC();
	bool test = testBit(value, 7);
	value <<= 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (testCarry) {
		value = setBit(value, 0);
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
    return 16;
}

uint8_t CPUController::cpu8BitRegisterRRCMemoryAddress(uint16_t address) {
    uint8_t value = this->memoryController->readMemory8Bit(address);
	bool test = testBit(value, 0);
	value >>= 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
		value = setBit(value, 7);
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
	return 16;
}

uint8_t CPUController::cpu8BitRegisterRRMemoryAddress(uint16_t address) {
	uint8_t value = this->memoryController->readMemory8Bit(address);
	bool testCarry = this->flagC();
	bool test = testBit(value, 0);
	value >>= 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (testCarry) {
		value = setBit(value, 7);
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
    return 16;
}

uint8_t CPUController::cpu8BitRegisterSLA(Register8Bit &r) {
	bool test = testBit(r.bits, 7);
	r.bits = r.bits << 1;
	if (test) {
		this->setFlagC();
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
    return 8;
}

uint8_t CPUController::cpu8BitRegisterSLAMemoryAddress(uint16_t address) {
    uint8_t value = this->memoryController->readMemory8Bit(address);
	bool test = testBit(value, 7);
	value <<= 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
	return 16;
}

uint8_t CPUController::cpu8BitRegisterSRA(Register8Bit &r) {
	bool testMSB = testBit(r.bits, 7);
	bool testLSB = testBit(r.bits, 0);
	r.bits = r.bits >> 1;
	this->clearAllFlags();
	if (testMSB) {
		r.bits = setBit(r.bits, 7);
	}
	if (testLSB) {
		this->setFlagC();
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
    return 8;
}

uint8_t CPUController::cpu8BitRegisterSRAMemoryAddress(uint16_t address) {
    uint8_t value = this->memoryController->readMemory8Bit(address);
	bool testMSB = testBit(value, 7);
	bool testLSB = testBit(value, 0);
	value >>= 1;
	this->clearAllFlags();
	if (testMSB) {
		value = setBit(value, 7);
	}
	if (testLSB) {
		this->setFlagC();
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
	return 16;
}

uint8_t CPUController::cpu8BitRegisterSRL(Register8Bit &r) {
	bool test = testBit(r.bits, 0);
	r.bits = r.bits >> 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (r.bits == 0x0) {
		this->setFlagZ();
	}
    return 8;
}

uint8_t CPUController::cpu8BitRegisterSRLMemoryAddress(uint16_t address) {
	uint8_t value = this->memoryController->readMemory8Bit(address);
	bool test = testBit(value, 0);
	value >>= 1;
	this->clearAllFlags();
	if (test) {
		this->setFlagC();
	}
	if (value == 0x0) {
		this->setFlagZ();
	}
	this->memoryController->writeMemory(address, value);
    return 16;
}

uint8_t CPUController::cpu8BitRegisterBit(Register8Bit &r, uint8_t position) {
	bool test = testBit(r.bits, position);
	if (test) {
		this->clearFlagZ();
	} else {
		this->clearFlagZ();
	}
	this->clearFlagN();
	this->setFlagH();
    return 8;
}

uint8_t CPUController::cpu8BitRegisterBitMemoryAddress(uint16_t address, uint8_t position) {
    uint8_t value = this->memoryController->readMemory8Bit(address);
	bool test = testBit(value, position);
	if (test) {
		this->clearFlagZ();
	} else {
		this->setFlagZ();
	}
	this->clearFlagN();
	this->setFlagH();
	return 16;
}

uint8_t CPUController::cpu8BitRegisterSet(Register8Bit &r, uint8_t position) {
	uint8_t value = setBit(r.bits, position);
	r.bits = value;
    return 8;
}

uint8_t CPUController::cpu8BitRegisterSetMemoryAddress(uint16_t address, uint8_t position) {
    uint8_t value = this->memoryController->readMemory8Bit(address);
	value = setBit(value, position);
	this->memoryController->writeMemory(address, value);
	return 16;
}

uint8_t CPUController::cpu8BitRegisterReset(Register8Bit &r, uint8_t position) {
    uint8_t value = clearBit(r.bits, position);
	r.bits = value;
	return 8;
}

uint8_t CPUController::cpu8BitRegisterResetMemoryAddress(uint16_t address, uint8_t position) {
    uint8_t value = this->memoryController->readMemory8Bit(address);
	value = clearBit(value, position);
	this->memoryController->writeMemory(address, value);
	return 16;
}

uint8_t CPUController::cpu8BitJump(uint16_t address) {
	this->programCounter.setValue(address);
    return 12;
}

uint8_t CPUController::cpu8BitJumpConditional(bool condition) {
    uint16_t address = this->memoryController->readMemory16Bit(this->programCounter.value());
	this->programCounter.increment();
	this->programCounter.increment();
	if (condition) {
		this->programCounter.setValue(address);
	}
	return 12;
}

uint8_t CPUController::cpu8BitJumpAddConditional(bool condition) {
    int8_t signedValue = this->memoryController->readMemory8Bit(this->programCounter.value());
	this->programCounter.increment();
	uint32_t address = this->programCounter.value() + signedValue;
	address &= 0x00FFFF;
	if (condition) {
		this->programCounter.setValue(address);
	}
	return 8;
}

uint8_t CPUController::cpu8BitCall(bool condition) {
    uint16_t address = this->memoryController->readMemory16Bit(this->programCounter.value());
	this->programCounter.increment();
	this->programCounter.increment();
	if (condition) {
		this->pushIntoStack(this->programCounter.value());
		this->programCounter.setValue(address);
	}
	return 12;
}

uint8_t CPUController::cpu8BitRestart(uint16_t address) {
    this->pushIntoStack(this->programCounter.value());
	this->programCounter.setValue(address);
	return 32;
}

uint8_t CPUController::cpu8BitReturn(bool condition) {
	uint8_t cycles = 8;
	if (!condition) {
		return cycles;
	}
	uint16_t address = this->popFromStack();
	this->programCounter.setValue(address);
    return cycles;
}

uint8_t CPUController::cpu8BitReturnEnableInterrupts() {
	this->programCounter.setValue(this->popFromStack());
	this->disableInterrupts = false;
    return 8;
}

bool CPUController::flagZ() {
	return testBit(this->af.low.bits, 7);
}

bool CPUController::flagN() {
	return testBit(this->af.low.bits, 6);
}

bool CPUController::flagH() {
	return testBit(this->af.low.bits, 5);
}

bool CPUController::flagC() {
	return testBit(this->af.low.bits, 4);
}

void CPUController::setFlagZ() {
	this->af.low.bits = setBit(this->af.low.bits, 7);
}

void CPUController::setFlagN() {
	this->af.low.bits = setBit(this->af.low.bits, 6);
}

void CPUController::setFlagH() {
	this->af.low.bits = setBit(this->af.low.bits, 5);
}

void CPUController::setFlagC() {
	this->af.low.bits = setBit(this->af.low.bits, 4);
}

void CPUController::clearFlagZ() {
	this->af.low.bits = clearBit(this->af.low.bits, 7);
}

void CPUController::clearFlagN() {
	this->af.low.bits = clearBit(this->af.low.bits, 6);
}

void CPUController::clearFlagH() {
	this->af.low.bits = clearBit(this->af.low.bits, 5);
}

void CPUController::clearFlagC() {
	this->af.low.bits = clearBit(this->af.low.bits, 4);
}

void CPUController::clearAllFlags() {
	this->clearFlagZ();
	this->clearFlagN();
	this->clearFlagH();
	this->clearFlagC();
}

void CPUController::pushIntoStack(uint16_t value) {
	uint8_t low = value & 0xFF;
	uint8_t high = value >> 8;
	this->stackPointer.decrement();
	this->memoryController->writeMemory(this->stackPointer.value(), high);
	this->stackPointer.decrement();
	this->memoryController->writeMemory(this->stackPointer.value(), low);
}

uint16_t CPUController::popFromStack() {
	uint16_t value = this->memoryController->readMemory16Bit(this->stackPointer.value());
	this->stackPointer.increment();
	this->stackPointer.increment();
	return value;
}

uint8_t CPUController::executeExtendedOpCode(uint8_t opCode) {
	switch (opCode) {
	// Miscellaneous
	// SWAP n
	case 0x37: {
		return this->cpu8BitRegisterSwap(this->af.high);
	}
	case 0x30: {
		return this->cpu8BitRegisterSwap(this->bc.high);
	}
	case 0x31: {
		return this->cpu8BitRegisterSwap(this->bc.low);
	}
	case 0x32: {
		return this->cpu8BitRegisterSwap(this->de.high);
	}
	case 0x33: {
		return this->cpu8BitRegisterSwap(this->de.low);
	}
	case 0x34: {
		return this->cpu8BitRegisterSwap(this->hl.high);
	}
	case 0x35: {
		return this->cpu8BitRegisterSwap(this->hl.low);
	}
	case 0x36: {
		return this->cpu8BitSwapMemoryAddress(this->hl.value());
	}
	// Rotates & Shifts
	// RLC n
	case 0x07: {
		return this->cpu8BitRegisterRLC(this->af.high);
	}
	case 0x00: {
		return this->cpu8BitRegisterRLC(this->bc.high);
	}
	case 0x01: {
		return this->cpu8BitRegisterRLC(this->bc.low);
	}
	case 0x02: {
		return this->cpu8BitRegisterRLC(this->de.high);
	}
	case 0x03: {
		return this->cpu8BitRegisterRLC(this->de.low);
	}
	case 0x04: {
		return this->cpu8BitRegisterRLC(this->hl.high);
	}
	case 0x05: {
		return this->cpu8BitRegisterRLC(this->hl.low);
	}
	case 0x06: {
		return this->cpu8BitRegisterRLCMemoryAddress(this->hl.value());
	}
	// RL n
	case 0x17: {
		return this->cpu8BitRegisterRL(this->af.high);
	}
	case 0x10: {
		return this->cpu8BitRegisterRL(this->bc.high);
	}
	case 0x11: {
		return this->cpu8BitRegisterRL(this->bc.low);
	}
	case 0x12: {
		return this->cpu8BitRegisterRL(this->de.high);
	}
	case 0x13: {
		return this->cpu8BitRegisterRL(this->de.low);
	}
	case 0x14: {
		return this->cpu8BitRegisterRL(this->hl.high);
	}
	case 0x15: {
		return this->cpu8BitRegisterRL(this->hl.low);
	}
	case 0x16: {
		return this->cpu8BitRegisterRLMemoryAddress(this->hl.value());
	}
	// RRC n
	case 0x0F: {
		return this->cpu8BitRegisterRRC(this->af.high);
	}
	case 0x08: {
		return this->cpu8BitRegisterRRC(this->bc.high);
	}
	case 0x09: {
		return this->cpu8BitRegisterRRC(this->bc.low);
	}
	case 0x0A: {
		return this->cpu8BitRegisterRRC(this->de.high);
	}
	case 0x0B: {
		return this->cpu8BitRegisterRRC(this->de.low);
	}
	case 0x0C: {
		return this->cpu8BitRegisterRRC(this->hl.high);
	}
	case 0x0D: {
		return this->cpu8BitRegisterRRC(this->hl.low);
	}
	case 0x0E: {
		return this->cpu8BitRegisterRRCMemoryAddress(this->hl.value());
	}
	// RR n
	case 0x1F: {
		return this->cpu8BitRegisterRR(this->af.high);
	}
	case 0x18: {
		return this->cpu8BitRegisterRR(this->bc.high);
	}
	case 0x19: {
		return this->cpu8BitRegisterRR(this->bc.low);
	}
	case 0x1A: {
		return this->cpu8BitRegisterRR(this->de.high);
	}
	case 0x1B: {
		return this->cpu8BitRegisterRR(this->de.low);
	}
	case 0x1C: {
		return this->cpu8BitRegisterRR(this->hl.high);
	}
	case 0x1D: {
		return this->cpu8BitRegisterRR(this->hl.low);
	}
	case 0x1E: {
		return this->cpu8BitRegisterRRMemoryAddress(this->hl.value());
	}
	// SLA n
	case 0x27: {
		return this->cpu8BitRegisterSLA(this->af.high);
	}
	case 0x20: {
		return this->cpu8BitRegisterSLA(this->bc.high);
	}
	case 0x21: {
		return this->cpu8BitRegisterSLA(this->bc.low);
	}
	case 0x22: {
		return this->cpu8BitRegisterSLA(this->de.high);
	}
	case 0x23: {
		return this->cpu8BitRegisterSLA(this->de.low);
	}
	case 0x24: {
		return this->cpu8BitRegisterSLA(this->hl.high);
	}
	case 0x25: {
		return this->cpu8BitRegisterSLA(this->hl.low);
	}
	case 0x26: {
		return this->cpu8BitRegisterSLAMemoryAddress(this->hl.value());
	}
	// SRA n
	case 0x2F: {
		return this->cpu8BitRegisterSRA(this->af.high);
	}
	case 0x28: {
		return this->cpu8BitRegisterSRA(this->bc.high);
	}
	case 0x29: {
		return this->cpu8BitRegisterSRA(this->bc.low);
	}
	case 0x2A: {
		return this->cpu8BitRegisterSRA(this->de.high);
	}
	case 0x2B: {
		return this->cpu8BitRegisterSRA(this->de.low);
	}
	case 0x2C: {
		return this->cpu8BitRegisterSRA(this->hl.high);
	}
	case 0x2D: {
		return this->cpu8BitRegisterSRA(this->hl.low);
	}
	case 0x2E: {
		return this->cpu8BitRegisterSRAMemoryAddress(this->hl.value());
	}
	// SRL n
	case 0x3F:{
		return this->cpu8BitRegisterSRL(this->af.high);
	}
	case 0x38: {
		return this->cpu8BitRegisterSRL(this->bc.high);
	}
	case 0x39: {
		return this->cpu8BitRegisterSRL(this->bc.low);
	}
	case 0x3A: {
		return this->cpu8BitRegisterSRL(this->de.high);
	}
	case 0x3B: {
		return this->cpu8BitRegisterSRL(this->de.low);
	}
	case 0x3C: {
		return this->cpu8BitRegisterSRL(this->hl.high);
	}
	case 0x3D: {
		return this->cpu8BitRegisterSRL(this->hl.low);
	}
	case 0x3E: {
		return this->cpu8BitRegisterSRLMemoryAddress(this->hl.value());
	}
	// BIT b,r
	// b=0
	case 0x47: {
		return this->cpu8BitRegisterBit(this->af.high, 0);
	}
	case 0x40: {
		return this->cpu8BitRegisterBit(this->bc.high, 0);
	}
	case 0x41: {
		return this->cpu8BitRegisterBit(this->bc.low, 0);
	}
	case 0x42: {
		return this->cpu8BitRegisterBit(this->de.high, 0);
	}
	case 0x43: {
		return this->cpu8BitRegisterBit(this->de.low, 0);
	}
	case 0x44: {
		return this->cpu8BitRegisterBit(this->hl.high, 0);
	}
	case 0x45: {
		return this->cpu8BitRegisterBit(this->hl.low, 0);
	}
	case 0x46: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 0);
	}
	// b=1
	case 0x4F: {
		return this->cpu8BitRegisterBit(this->af.high, 1);
	}
	case 0x48: {
		return this->cpu8BitRegisterBit(this->bc.high, 1);
	}
	case 0x49: {
		return this->cpu8BitRegisterBit(this->bc.low, 1);
	}
	case 0x4A: {
		return this->cpu8BitRegisterBit(this->de.high, 1);
	}
	case 0x4B: {
		return this->cpu8BitRegisterBit(this->de.low, 1);
	}
	case 0x4C: {
		return this->cpu8BitRegisterBit(this->hl.high, 1);
	}
	case 0x4D: {
		return this->cpu8BitRegisterBit(this->hl.low, 1);
	}
	case 0x4E: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 1);
	}
	// b=2
	case 0x57: {
		return this->cpu8BitRegisterBit(this->af.high, 2);
	}
	case 0x50: {
		return this->cpu8BitRegisterBit(this->bc.high, 2);
	}
	case 0x51: {
		return this->cpu8BitRegisterBit(this->bc.low, 2);
	}
	case 0x52: {
		return this->cpu8BitRegisterBit(this->de.high, 2);
	}
	case 0x53: {
		return this->cpu8BitRegisterBit(this->de.low, 2);
	}
	case 0x54: {
		return this->cpu8BitRegisterBit(this->hl.high, 2);
	}
	case 0x55: {
		return this->cpu8BitRegisterBit(this->hl.low, 2);
	}
	case 0x56: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 2);
	}
	// b=3
	case 0x5F: {
		return this->cpu8BitRegisterBit(this->af.high, 3);
	}
	case 0x58: {
		return this->cpu8BitRegisterBit(this->bc.high, 3);
	}
	case 0x59: {
		return this->cpu8BitRegisterBit(this->bc.low, 3);
	}
	case 0x5A: {
		return this->cpu8BitRegisterBit(this->de.high, 3);
	}
	case 0x5B: {
		return this->cpu8BitRegisterBit(this->de.low, 3);
	}
	case 0x5C: {
		return this->cpu8BitRegisterBit(this->hl.high, 3);
	}
	case 0x5D: {
		return this->cpu8BitRegisterBit(this->hl.low, 3);
	}
	case 0x5E: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 3);
	}
	// b=4
	case 0x67: {
		return this->cpu8BitRegisterBit(this->af.high, 4);
	}
	case 0x60: {
		return this->cpu8BitRegisterBit(this->bc.high, 4);
	}
	case 0x61: {
		return this->cpu8BitRegisterBit(this->bc.low, 4);
	}
	case 0x62: {
		return this->cpu8BitRegisterBit(this->de.high, 4);
	}
	case 0x63: {
		return this->cpu8BitRegisterBit(this->de.low, 4);
	}
	case 0x64: {
		return this->cpu8BitRegisterBit(this->hl.high, 4);
	}
	case 0x65: {
		return this->cpu8BitRegisterBit(this->hl.low, 4);
	}
	case 0x66: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 4);
	}
	// b=5
	case 0x6F: {
		return this->cpu8BitRegisterBit(this->af.high, 5);
	}
	case 0x68: {
		return this->cpu8BitRegisterBit(this->bc.high, 5);
	}
	case 0x69: {
		return this->cpu8BitRegisterBit(this->bc.low, 5);
	}
	case 0x6A: {
		return this->cpu8BitRegisterBit(this->de.high, 5);
	}
	case 0x6B: {
		return this->cpu8BitRegisterBit(this->de.low, 5);
	}
	case 0x6C: {
		return this->cpu8BitRegisterBit(this->hl.high, 5);
	}
	case 0x6D: {
		return this->cpu8BitRegisterBit(this->hl.low, 5);
	}
	case 0x6E: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 5);
	}
	// b=6
	case 0x77: {
		return this->cpu8BitRegisterBit(this->af.high, 6);
	}
	case 0x70: {
		return this->cpu8BitRegisterBit(this->bc.high, 6);
	}
	case 0x71: {
		return this->cpu8BitRegisterBit(this->bc.low, 6);
	}
	case 0x72: {
		return this->cpu8BitRegisterBit(this->de.high, 6);
	}
	case 0x73: {
		return this->cpu8BitRegisterBit(this->de.low, 6);
	}
	case 0x74: {
		return this->cpu8BitRegisterBit(this->hl.high, 6);
	}
	case 0x75: {
		return this->cpu8BitRegisterBit(this->hl.low, 6);
	}
	case 0x76: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 6);
	}
	// b=7
	case 0x7F: {
		return this->cpu8BitRegisterBit(this->af.high, 7);
	}
	case 0x78: {
		return this->cpu8BitRegisterBit(this->bc.high, 7);
	}
	case 0x79: {
		return this->cpu8BitRegisterBit(this->bc.low, 7);
	}
	case 0x7A: {
		return this->cpu8BitRegisterBit(this->de.high, 7);
	}
	case 0x7B: {
		return this->cpu8BitRegisterBit(this->de.low, 7);
	}
	case 0x7C: {
		return this->cpu8BitRegisterBit(this->hl.high, 7);
	}
	case 0x7D: {
		return this->cpu8BitRegisterBit(this->hl.low, 7);
	}
	case 0x7E: {
		return this->cpu8BitRegisterBitMemoryAddress(this->hl.value(), 7);
	}
	// SET b,r
	// b=0
	case 0xC7: {
		return this->cpu8BitRegisterSet(this->af.high, 0);
	}
	case 0xC0: {
		return this->cpu8BitRegisterSet(this->bc.high, 0);
	}
	case 0xC1: {
		return this->cpu8BitRegisterSet(this->bc.low, 0);
	}
	case 0xC2: {
		return this->cpu8BitRegisterSet(this->de.high, 0);
	}
	case 0xC3: {
		return this->cpu8BitRegisterSet(this->de.low, 0);
	}
	case 0xC4: {
		return this->cpu8BitRegisterSet(this->hl.high, 0);
	}
	case 0xC5: {
		return this->cpu8BitRegisterSet(this->hl.low, 0);
	}
	case 0xC6: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 0);
	}
	// b=1
	case 0xCF: {
		return this->cpu8BitRegisterSet(this->af.high, 1);
	}
	case 0xC8: {
		return this->cpu8BitRegisterSet(this->bc.high, 1);
	}
	case 0xC9: {
		return this->cpu8BitRegisterSet(this->bc.low, 1);
	}
	case 0xCA: {
		return this->cpu8BitRegisterSet(this->de.high, 1);
	}
	case 0xCB: {
		return this->cpu8BitRegisterSet(this->de.low, 1);
	}
	case 0xCC: {
		return this->cpu8BitRegisterSet(this->hl.high, 1);
	}
	case 0xCD: {
		return this->cpu8BitRegisterSet(this->hl.low, 1);
	}
	case 0xCE: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 1);
	}
	// b=2
	case 0xD7: {
		return this->cpu8BitRegisterSet(this->af.high, 2);
	}
	case 0xD0: {
		return this->cpu8BitRegisterSet(this->bc.high, 2);
	}
	case 0xD1: {
		return this->cpu8BitRegisterSet(this->bc.low, 2);
	}
	case 0xD2: {
		return this->cpu8BitRegisterSet(this->de.high, 2);
	}
	case 0xD3: {
		return this->cpu8BitRegisterSet(this->de.low, 2);
	}
	case 0xD4: {
		return this->cpu8BitRegisterSet(this->hl.high, 2);
	}
	case 0xD5: {
		return this->cpu8BitRegisterSet(this->hl.low, 2);
	}
	case 0xD6: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 2);
	}
	// b=3
	case 0xDF: {
		return this->cpu8BitRegisterSet(this->af.high, 3);
	}
	case 0xD8: {
		return this->cpu8BitRegisterSet(this->bc.high, 3);
	}
	case 0xD9: {
		return this->cpu8BitRegisterSet(this->bc.low, 3);
	}
	case 0xDA: {
		return this->cpu8BitRegisterSet(this->de.high, 3);
	}
	case 0xDB: {
		return this->cpu8BitRegisterSet(this->de.low, 3);
	}
	case 0xDC: {
		return this->cpu8BitRegisterSet(this->hl.high, 3);
	}
	case 0xDD: {
		return this->cpu8BitRegisterSet(this->hl.low, 3);
	}
	case 0xDE: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 3);
	}
	// b=4
	case 0xE7: {
		return this->cpu8BitRegisterSet(this->af.high, 4);
	}
	case 0xE0: {
		return this->cpu8BitRegisterSet(this->bc.high, 4);
	}
	case 0xE1: {
		return this->cpu8BitRegisterSet(this->bc.low, 4);
	}
	case 0xE2: {
		return this->cpu8BitRegisterSet(this->de.high, 4);
	}
	case 0xE3: {
		return this->cpu8BitRegisterSet(this->de.low, 4);
	}
	case 0xE4: {
		return this->cpu8BitRegisterSet(this->hl.high, 4);
	}
	case 0xE5: {
		return this->cpu8BitRegisterSet(this->hl.low, 4);
	}
	case 0xE6: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 4);
	}
	// b=5
	case 0xEF: {
		return this->cpu8BitRegisterSet(this->af.high, 5);
	}
	case 0xE8: {
		return this->cpu8BitRegisterSet(this->bc.high, 5);
	}
	case 0xE9: {
		return this->cpu8BitRegisterSet(this->bc.low, 5);
	}
	case 0xEA: {
		return this->cpu8BitRegisterSet(this->de.high, 5);
	}
	case 0xEB: {
		return this->cpu8BitRegisterSet(this->de.low, 5);
	}
	case 0xEC: {
		return this->cpu8BitRegisterSet(this->hl.high, 5);
	}
	case 0xED: {
		return this->cpu8BitRegisterSet(this->hl.low, 5);
	}
	case 0xEE: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 5);
	}
	// b=6
	case 0xF7: {
		return this->cpu8BitRegisterSet(this->af.high, 6);
	}
	case 0xF0: {
		return this->cpu8BitRegisterSet(this->bc.high, 6);
	}
	case 0xF1: {
		return this->cpu8BitRegisterSet(this->bc.low, 6);
	}
	case 0xF2: {
		return this->cpu8BitRegisterSet(this->de.high, 6);
	}
	case 0xF3: {
		return this->cpu8BitRegisterSet(this->de.low, 6);
	}
	case 0xF4: {
		return this->cpu8BitRegisterSet(this->hl.high, 6);
	}
	case 0xF5: {
		return this->cpu8BitRegisterSet(this->hl.low, 6);
	}
	case 0xF6: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 6);
	}
	// b=7
	case 0xFF: {
		return this->cpu8BitRegisterSet(this->af.high, 7);
	}
	case 0xF8: {
		return this->cpu8BitRegisterSet(this->bc.high, 7);
	}
	case 0xF9: {
		return this->cpu8BitRegisterSet(this->bc.low, 7);
	}
	case 0xFA: {
		return this->cpu8BitRegisterSet(this->de.high, 7);
	}
	case 0xFB: {
		return this->cpu8BitRegisterSet(this->de.low, 7);
	}
	case 0xFC: {
		return this->cpu8BitRegisterSet(this->hl.high, 7);
	}
	case 0xFD: {
		return this->cpu8BitRegisterSet(this->hl.low, 7);
	}
	case 0xFE: {
		return this->cpu8BitRegisterSetMemoryAddress(this->hl.value(), 7);
	}
	// RES b,r
	// b=0
	case 0x87: {
		return this->cpu8BitRegisterReset(this->af.high, 0);
	}
	case 0x80: {
		return this->cpu8BitRegisterReset(this->bc.high, 0);
	}
	case 0x81: {
		return this->cpu8BitRegisterReset(this->bc.low, 0);
	}
	case 0x82: {
		return this->cpu8BitRegisterReset(this->de.high, 0);
	}
	case 0x83: {
		return this->cpu8BitRegisterReset(this->de.low, 0);
	}
	case 0x84: {
		return this->cpu8BitRegisterReset(this->hl.high, 0);
	}
	case 0x85: {
		return this->cpu8BitRegisterReset(this->hl.low, 0);
	}
	case 0x86: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 0);
	}
	// b=1
	case 0x8F: {
		return this->cpu8BitRegisterReset(this->af.high, 1);
	}
	case 0x88: {
		return this->cpu8BitRegisterReset(this->bc.high, 1);
	}
	case 0x89: {
		return this->cpu8BitRegisterReset(this->bc.low, 1);
	}
	case 0x8A: {
		return this->cpu8BitRegisterReset(this->de.high, 1);
	}
	case 0x8B: {
		return this->cpu8BitRegisterReset(this->de.low, 1);
	}
	case 0x8C: {
		return this->cpu8BitRegisterReset(this->hl.high, 1);
	}
	case 0x8D: {
		return this->cpu8BitRegisterReset(this->hl.low, 1);
	}
	case 0x8E: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 1);
	}
	// b=2
	case 0x97: {
		return this->cpu8BitRegisterReset(this->af.high, 2);
	}
	case 0x90: {
		return this->cpu8BitRegisterReset(this->bc.high, 2);
	}
	case 0x91: {
		return this->cpu8BitRegisterReset(this->bc.low, 2);
	}
	case 0x92: {
		return this->cpu8BitRegisterReset(this->de.high, 2);
	}
	case 0x93: {
		return this->cpu8BitRegisterReset(this->de.low, 2);
	}
	case 0x94: {
		return this->cpu8BitRegisterReset(this->hl.high, 2);
	}
	case 0x95: {
		return this->cpu8BitRegisterReset(this->hl.low, 2);
	}
	case 0x96: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 2);
	}
	// b=3
	case 0x9F: {
		return this->cpu8BitRegisterReset(this->af.high, 3);
	}
	case 0x98: {
		return this->cpu8BitRegisterReset(this->bc.high, 3);
	}
	case 0x99: {
		return this->cpu8BitRegisterReset(this->bc.low, 3);
	}
	case 0x9A: {
		return this->cpu8BitRegisterReset(this->de.high, 3);
	}
	case 0x9B: {
		return this->cpu8BitRegisterReset(this->de.low, 3);
	}
	case 0x9C: {
		return this->cpu8BitRegisterReset(this->hl.high, 3);
	}
	case 0x9D: {
		return this->cpu8BitRegisterReset(this->hl.low, 3);
	}
	case 0x9E: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 3);
	}
	// b=4
	case 0xA7: {
		return this->cpu8BitRegisterReset(this->af.high, 4);
	}
	case 0xA0: {
		return this->cpu8BitRegisterReset(this->bc.high, 4);
	}
	case 0xA1: {
		return this->cpu8BitRegisterReset(this->bc.low, 4);
	}
	case 0xA2: {
		return this->cpu8BitRegisterReset(this->de.high, 4);
	}
	case 0xA3: {
		return this->cpu8BitRegisterReset(this->de.low, 4);
	}
	case 0xA4: {
		return this->cpu8BitRegisterReset(this->hl.high, 4);
	}
	case 0xA5: {
		return this->cpu8BitRegisterReset(this->hl.low, 4);
	}
	case 0xA6: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 4);
	}
	// b=5
	case 0xA8: {
		return this->cpu8BitRegisterReset(this->af.high, 5);
	}
	case 0xA9: {
		return this->cpu8BitRegisterReset(this->bc.high, 5);
	}
	case 0xAA: {
		return this->cpu8BitRegisterReset(this->bc.low, 5);
	}
	case 0xAB: {
		return this->cpu8BitRegisterReset(this->de.high, 5);
	}
	case 0xAC: {
		return this->cpu8BitRegisterReset(this->de.low, 5);
	}
	case 0xAD: {
		return this->cpu8BitRegisterReset(this->hl.high, 5);
	}
	case 0xAE: {
		return this->cpu8BitRegisterReset(this->hl.low, 5);
	}
	case 0xAF: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 5);
	}
	// b=6
	case 0xB7: {
		return this->cpu8BitRegisterReset(this->af.high, 6);
	}
	case 0xB0: {
		return this->cpu8BitRegisterReset(this->bc.high, 6);
	}
	case 0xB1: {
		return this->cpu8BitRegisterReset(this->bc.low, 6);
	}
	case 0xB2: {
		return this->cpu8BitRegisterReset(this->de.high, 6);
	}
	case 0xB3: {
		return this->cpu8BitRegisterReset(this->de.low, 6);
	}
	case 0xB4: {
		return this->cpu8BitRegisterReset(this->hl.high, 6);
	}
	case 0xB5: {
		return this->cpu8BitRegisterReset(this->hl.low, 6);
	}
	case 0xB6: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 6);
	}
	// b=7
	case 0xB8: {
		return this->cpu8BitRegisterReset(this->af.high, 7);
	}
	case 0xB9: {
		return this->cpu8BitRegisterReset(this->bc.high, 7);
	}
	case 0xBA: {
		return this->cpu8BitRegisterReset(this->bc.low, 7);
	}
	case 0xBB: {
		return this->cpu8BitRegisterReset(this->de.high, 7);
	}
	case 0xBC: {
		return this->cpu8BitRegisterReset(this->de.low, 7);
	}
	case 0xBD: {
		return this->cpu8BitRegisterReset(this->hl.high, 7);
	}
	case 0xBE: {
		return this->cpu8BitRegisterReset(this->hl.low, 7);
	}
	case 0xBF: {
		return this->cpu8BitRegisterResetMemoryAddress(this->hl.value(), 7);
	}
	}

	return 0;
}

uint8_t CPUController::flags() {
	return this->af.low.bits;
}

void CPUController::logRegisters() {
	std::ostringstream message;
	message << "AF: " << formatHexUInt16(this->af.value()) << ", BC: " << formatHexUInt16(this->bc.value()) << ", DE: " << formatHexUInt16(this->de.value()) << ", HL: " << formatHexUInt16(this->hl.value());
	this->logger->logMessage(message.str());
}
