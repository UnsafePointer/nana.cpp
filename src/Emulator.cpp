#include "Emulator.hpp"
#include <iostream>
#include <fstream>

Emulator::Emulator() {
}

Emulator::~Emulator() {
}

void Emulator::loadCartridge(std::string filename) {
    std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (!file.is_open()) {
        exit(2);
    }
    std::streampos size = file.tellg();
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(this->cartridge), size);
    file.close();

    for (int i = 0; i < 0x8000; i++) {
        this->ROM[i] = this->cartridge[i];
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
