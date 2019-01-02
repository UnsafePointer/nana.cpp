#include "Memory.hpp"
#include <iostream>
#include <fstream>
#include "Utils.hpp"
#include "TimerController.hpp"
#include "PPU.hpp"
#include "JoypadController.hpp"

const static uint16_t RamBankSize = 0x2000;
const static uint16_t RomBankSize = 0x4000;
const static uint16_t DMATransferAddress = 0xFF46;

Memory::Memory() {
    this->cartridgeType = ROMOnly;
}

Memory::~Memory() {

}

void Memory::loadCartridge(std::string filename) {
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
}

void Memory::writeMemory(uint16_t address, uint8_t data) {
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
        // TODO: Handle clock frequency change
    } else if (address == CurrentScanlineRegisterAddress) {
        this->rom[address] = 0;
    } else if (address == DMATransferAddress) {
        // TODO: this->dmaTransfer(data);
    } else {
        this->rom[address] = data;
    }
}

uint8_t Memory::readMemory(uint16_t address) {
    if (address >= 0x4000 && address <= 0x7FFF) {
        uint32_t bankAddress = address;
        bankAddress += (this->currentRomBank - 1) * RomBankSize;
        uint8_t value = this->cartridge[bankAddress];
        return value;
    } else if (address >= 0xA000 && address <= 0xBFFF) {
        uint32_t bankAddress = address - 0xA000;
        uint8_t value = this->cartridge[bankAddress + currentRamBank*RamBankSize];
        return value;
    } else if (address == JoypadRegisterAddress) {
        // TODO: uint8_t value = this->joypadController.getJoypadState();
        // return value;
        return 0;
    }

    uint8_t value = this->rom[address];
    return value;
}

void Memory::handleMemoryBanking(uint16_t address, uint8_t data) {
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

void Memory::enableRamBanking(uint16_t address, uint8_t data) {
    if (this->cartridgeType == MBC2) {
        if (testBit(address, 4)) {
            return;
        }
    }

    this->ramBankEnabled = (data & 0xF) == 0xA;
}

void Memory::changeLowRomBank(uint8_t data) {
    if (this->cartridgeType == MBC2) {
        this->currentRomBank = data & 0xF;
        if (this->currentRomBank == 0) {
            this->currentRomBank++;
        }
    } else {
        uint8_t test = data & 0x31;
        this->currentRomBank &= 244;
        this->currentRamBank |= test;
        if (this->currentRomBank == 0) {
            this->currentRomBank++;
        }
    }
}

void Memory::changeHighRomBank(uint8_t data) {
    this->currentRomBank &= 31;
    data &= 224;
    this->currentRomBank |= data;
    if (this->currentRomBank == 0) {
        this->currentRomBank++;
    }
}

void Memory::changeRamBank(uint8_t data) {
    this->currentRamBank = data & 0x3;
}

void Memory::selectMemoryBankingMode(uint8_t data) {
    if ((data & 0x1) == 0) {
        this->romBankEnabled = true;
    } else {
        this->romBankEnabled = false;
    }
    if (this->romBankEnabled) {
        this->currentRamBank = 0;
    }
}
