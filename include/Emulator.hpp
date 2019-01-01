#pragma once
#include <cstdint>
#include <string>
#include "Register.hpp"

enum cartridge_type_t { MBC1, MBC2 };

class Emulator {
private:
    Register16Bit AF;
    Register16Bit BC;
    Register16Bit DE;
    Register16Bit HL;

    Register16Bit programCounter;

    uint8_t cartridge[0x200000];
    uint8_t RAM[0x10000];
    uint8_t ROM[0x8000];
    cartridge_type_t cartridgeType;
public:
    Emulator();
    ~Emulator();
    void loadCartridge(std::string filename);
};
