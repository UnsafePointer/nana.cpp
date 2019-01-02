#pragma once
#include <cstdint>

const static uint16_t CurrentScanlineRegisterAddress = 0xFF44;

class PPU {

public:
    PPU();
    ~PPU();
};
