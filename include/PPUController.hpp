#pragma once
#include <cstdint>
#include "MemoryController.hpp"
#include "InterruptController.hpp"

const static uint16_t LCDControllerAddress = 0xFF40;
const static uint16_t LCDStatusRegisterAddress = 0xFF41;
const static uint16_t CurrentScanlineRegisterAddress = 0xFF44;
const static uint16_t CoincidenceFlagAddress = 0xFF45;

class PPUController {
private:
    int16_t scanlineRenderCyclesCounter;
    MemoryController memoryController;
    InterruptController interruptController;
    void drawScanline();
    void renderTiles();
    void renderSprites();
    void updateLCDStatus();
    bool isLCDEnabled();
public:
    PPUController();
    ~PPUController();
    void updateScreen(uint8_t cycles);
};
