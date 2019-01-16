#include "MemoryController.hpp"
#include <iostream>
#include <fstream>
#include "Utils.hpp"
#include "TimerController.hpp"
#include "PPUController.hpp"
#include "JoypadController.hpp"

const static uint16_t RamBankSize = 0x2000;
const static uint16_t RomBankSize = 0x4000;
const static uint16_t DMATransferAddress = 0xFF46;

MemoryController::MemoryController(Logger &logger, bool enableMemoryAccessDebug) : cartridge(), ram(), rom(), logger(&logger), enableMemoryAccessDebug(enableMemoryAccessDebug) {
    this->cartridgeType = ROMOnly;
    this->currentRomBank = 1;
    this->currentRamBank = 0;
    this->ramBankEnabled = false;
    this->romBankEnabled = false;
    this->rom[0xFF00] = 0xFF;
	this->rom[0xFF05] = 0x00;
	this->rom[0xFF06] = 0x00;
	this->rom[0xFF07] = 0x00;
	this->rom[0xFF10] = 0x80;
	this->rom[0xFF11] = 0xBF;
	this->rom[0xFF12] = 0xF3;
	this->rom[0xFF14] = 0xBF;
	this->rom[0xFF16] = 0x3F;
	this->rom[0xFF17] = 0x00;
	this->rom[0xFF19] = 0xBF;
	this->rom[0xFF1A] = 0x7F;
	this->rom[0xFF1B] = 0xFF;
	this->rom[0xFF1C] = 0x9F;
	this->rom[0xFF1E] = 0xBF;
	this->rom[0xFF20] = 0xFF;
	this->rom[0xFF21] = 0x00;
	this->rom[0xFF22] = 0x00;
	this->rom[0xFF23] = 0xBF;
	this->rom[0xFF24] = 0x77;
	this->rom[0xFF25] = 0xF3;
	this->rom[0xFF26] = 0xF1;
	this->rom[0xFF40] = 0x91;
	this->rom[0xFF42] = 0x00;
	this->rom[0xFF43] = 0x00;
	this->rom[0xFF45] = 0x00;
	this->rom[0xFF47] = 0xFC;
	this->rom[0xFF48] = 0xFF;
	this->rom[0xFF49] = 0xFF;
	this->rom[0xFF4A] = 0x00;
	this->rom[0xFF4B] = 0x00;
	this->rom[0xFFFF] = 0x00;
}

MemoryController::~MemoryController() {

}

void MemoryController::loadCartridge(std::string filename) {
    std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (!file.is_open()) {
        exit(2);
    }
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(this->cartridge), size);
    file.close();

    for (int i = 0; i < 0x8000; i++) {
        this->rom[i] = this->cartridge[i];
    }

    uint8_t cartridgeTypeDefinition = this->cartridge[0x0147];
    switch (cartridgeTypeDefinition) {
        case 1:
        case 2:
        case 3:
            this->cartridgeType = MBC1;
            break;
        case 4:
        case 5:
            this->cartridgeType = MBC2;
            break;
    }

    std::ostringstream message;
    message << "Cartridge type: " << (int)cartridgeTypeDefinition;
    this->logger->logMessage(message.str());
}

void MemoryController::writeMemory(uint16_t address, uint8_t data) {
    if (this->enableMemoryAccessDebug) {
        std::ostringstream message;
        message << "Write: " << formatHexUInt16(address) << ", Value: " << formatHexUInt8(data);
        this->logger->logMessage(message.str());
    }
    if (address <= 0x7FFF) {
        this->handleMemoryBanking(address, data);
        return;
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        if (this->ramBankEnabled) {
            uint16_t bankAddress = address - 0xBFFF;
            this->ram[bankAddress+currentRamBank*RamBankSize] = data;
        }
    } else if (address >= 0xE000 && address <= 0xFDFF) {
        this->rom[address] = data;
        this->writeMemory(address-0x2000, data);
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {
        return;
    } else if (address >= 0xFF10 && address <= 0xFF3F) {
        this->rom[address] = data;
        // TODO: this->handleSound(address, data);
    } else if (address == DividerRegisterAddress) {
        this->rom[address] = 0;
    } else if (address == TimerControllerAddress) {
        uint8_t currentClockFrequency = this->timerController->clockFrequency();
        this->rom[address] = data;
        uint8_t newClockFrequency = this->timerController->clockFrequency();
        if (currentClockFrequency != newClockFrequency) {
            this->timerController->setTimerCycleCounter();
        }
    } else if (address == CurrentScanlineRegisterAddress) {
        this->rom[address] = 0;
    } else if (address == DMATransferAddress) {
        this->dmaTransfer(data);
    } else {
        this->rom[address] = data;
    }
}

void MemoryController::writeMemoryAvoidingTraps(uint16_t address, uint8_t data) {
    this->rom[address] = data;
}

uint8_t MemoryController::readMemory8Bit(uint16_t address) {
    if (address >= 0x4000 && address <= 0x7FFF) {
        uint32_t bankAddress = address;
        bankAddress += (this->currentRomBank - 1) * RomBankSize;
        uint8_t value = this->cartridge[bankAddress];
        if (this->enableMemoryAccessDebug) {
            std::ostringstream message;
            message << "Reading " << formatHexUInt16(address) << " (" << formatHexUInt32(bankAddress) << ") from ROM at bank " << (int)this->currentRomBank << ": " << formatHexUInt8(value);
            this->logger->logMessage(message.str());
        }
        return value;
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        uint32_t bankAddress = address - 0xA000;
        uint8_t value = this->ram[bankAddress + currentRamBank*RamBankSize];
        if (this->enableMemoryAccessDebug) {
            std::ostringstream message;
            message << "Reading " << formatHexUInt16(address) << " from RAM at bank " << (int)this->currentRamBank << ": " << formatHexUInt8(value);
            this->logger->logMessage(message.str());
        }
        return value;
    } else if (address == JoypadRegisterAddress) {
        uint8_t value = this->joypadController->getJoypadState();
        return value;
    }

    uint8_t value = this->rom[address];
    if (this->enableMemoryAccessDebug) {
        std::ostringstream message;
        message << "Reading " << formatHexUInt16(address) << " from ROM: " << formatHexUInt8(value);
        this->logger->logMessage(message.str());
    }
    return value;
}

uint8_t MemoryController::readMemoryAvoidingTraps(uint16_t address) {
    return this->rom[address];
}

uint16_t MemoryController::readMemory16Bit(uint16_t address) {
    uint16_t high = this->readMemory8Bit(address + 1);
    high <<= 8;
    uint16_t low = this->readMemory8Bit(address);
    return low | high;
}

void MemoryController::handleMemoryBanking(uint16_t address, uint8_t data) {
    if (address <= 0x2000) {
        if (this->cartridgeType != ROMOnly) {
            this->enableRamBanking(address, data);
        }
    } else if (address >= 0x2000 && address < 0x4000) {
        if (this->cartridgeType != ROMOnly) {
            this->changeLowRomBank(data);
        }
    } else if (address >= 0x4000 && address < 0x6000) {
        if (this->cartridgeType == MBC1) {
            if (this->romBankEnabled) {
                this->changeHighRomBank(data);
            } else {
                this->changeRamBank(data);
            }
        }
    } else if (address >= 0x6000 && address < 0x8000) {
        if (this->cartridgeType == MBC1) {
            this->selectMemoryBankingMode(data);
        }
    }
}

void MemoryController::enableRamBanking(uint16_t address, uint8_t data) {
    if (this->cartridgeType == MBC2) {
        if (testBit(address, 4)) {
            return;
        }
    }

    this->ramBankEnabled = (data & 0xF) == 0xA;
}

void MemoryController::changeLowRomBank(uint8_t data) {
    if (this->cartridgeType == MBC2) {
        this->currentRomBank = data & 0xF;
        if (this->currentRomBank == 0) {
            this->currentRomBank++;
        }
    } else {
        uint8_t test = data & 31;
        this->currentRomBank &= 224;
        this->currentRomBank |= test;
        if (this->currentRomBank == 0) {
            this->currentRomBank++;
        }
    }
    if (this->enableMemoryAccessDebug) {
        std::ostringstream message;
        message << "Current ROM bank: " << (int)this->currentRomBank;
        this->logger->logMessage(message.str());
    }
}

void MemoryController::changeHighRomBank(uint8_t data) {
    this->currentRomBank &= 31;
    data &= 224;
    this->currentRomBank |= data;
    if (this->currentRomBank == 0) {
        this->currentRomBank++;
    }
    if (this->enableMemoryAccessDebug) {
        std::ostringstream message;
        message << "Current ROM bank: " << (int)this->currentRomBank;
        this->logger->logMessage(message.str());
    }
}

void MemoryController::changeRamBank(uint8_t data) {
    this->currentRamBank = data & 0x3;
    if (this->enableMemoryAccessDebug) {
        std::ostringstream message;
        message << "Current RAM bank: " << (int)this->currentRamBank;
        this->logger->logMessage(message.str());
    }
}

void MemoryController::selectMemoryBankingMode(uint8_t data) {
    if ((data & 0x1) == 0) {
        this->romBankEnabled = true;
    } else {
        this->romBankEnabled = false;
    }
    if (this->romBankEnabled) {
        this->currentRamBank = 0;
    }
    if (this->enableMemoryAccessDebug) {
        std::string enabled =  this->romBankEnabled ? "enabled" : "disabled";
        std::ostringstream message;
        message << "ROM bank enabled: " << enabled;
        this->logger->logMessage(message.str());
    }
}

void MemoryController::dmaTransfer(uint8_t data) {
    if (this->enableMemoryAccessDebug) {
        std::ostringstream message;
        message << "DMA Transfer: " << formatHexUInt8(data);
        this->logger->logMessage(message.str());
    }
    uint16_t address = data << 8;
    for (uint8_t i = 0; i < 0xA0; i++) {
        uint8_t data = this->readMemory8Bit(address + i);
        this->writeMemory(0xFE00+i, data);
    }
}
