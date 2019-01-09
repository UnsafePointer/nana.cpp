#pragma once
#include <cstdint>
#include <string>

class JoypadController;

enum cartridge_type_t {
    ROMOnly = 0,
    MBC1 = 1,
    MBC2 = 2,
};

class MemoryController {
private:
    uint8_t cartridge[0x200000];
    uint8_t ram[0x10000];
    uint8_t rom[0x8000];
    cartridge_type_t cartridgeType;
    bool ramBankEnabled;
    bool romBankEnabled;
    uint8_t currentRamBank;
    uint8_t currentRomBank;

    void handleMemoryBanking(uint16_t address, uint8_t data);
    void enableRamBanking(uint16_t address, uint8_t data);
    void changeLowRomBank(uint8_t data);
    void changeHighRomBank(uint8_t data);
    void changeRamBank(uint8_t data);
    void selectMemoryBankingMode(uint8_t data);
    void dmaTransfer(uint8_t data);
public:
    MemoryController();
    ~MemoryController();

    JoypadController *joypadController;

    void loadCartridge(std::string filename);
    void writeMemory(uint16_t address, uint8_t data);
    void writeMemoryAvoidingTraps(uint16_t address, uint8_t data);
    uint8_t readMemory8Bit(uint16_t address);
    uint8_t readMemoryAvoidingTraps(uint16_t address);
    uint16_t readMemory16Bit(uint16_t address);
};
