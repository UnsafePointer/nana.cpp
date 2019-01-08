#include "PPUController.hpp"
#include "Utils.hpp"

const static uint16_t viewingAreaPositionYAddress = 0xFF42;
const static uint16_t viewingAreaPositionXAddress = 0xFF43;
const static uint16_t windowPositionYAddress = 0xFF4A;
const static uint16_t windowPositionXAddress = 0xFF4B;

PPUController::PPUController() {

}

PPUController::~PPUController() {

}

void PPUController::drawScanline() {
    uint8_t value = this->memoryController.readMemory8Bit(LCDControllerAddress);
    if (testBit(value, 0)) {
        this->renderTiles();
    }
    if (testBit(value, 1)) {
        this->renderSprites();
    }
}

void PPUController::renderTiles() {
    uint16_t tileData = 0;
    uint16_t backgroundMemory = 0;
    bool useUnsigned = false;

    uint8_t viewingAreaPositionY = this->memoryController.readMemory8Bit(viewingAreaPositionYAddress);
    uint8_t viewingAreaPositionX = this->memoryController.readMemory8Bit(viewingAreaPositionXAddress);
    uint8_t windowPositionY = this->memoryController.readMemory8Bit(windowPositionYAddress);
    uint8_t windowPositionX = this->memoryController.readMemory8Bit(windowPositionXAddress) - 7;

    uint8_t lcdController = this->memoryController.readMemory8Bit(LCDControllerAddress);
    uint8_t currentScanline = this->memoryController.readMemory8Bit(CurrentScanlineRegisterAddress);

    bool usingWindow = false;

    if (testBit(lcdController, 5)) {
        if (windowPositionY <= currentScanline) {
            usingWindow = true;
        }
    }

    if (testBit(lcdController, 4)) {
        tileData = 0x8000;
    } else {
        tileData = 0x8800;
        useUnsigned = false;
    }

    if (usingWindow) {
        if (testBit(lcdController, 6)) {
            backgroundMemory = 0x9C00;
        } else {
            backgroundMemory = 0x9800;
        }
    } else {
        if (testBit(lcdController, 3)) {
            backgroundMemory = 0x9C00;
        } else {
            backgroundMemory = 0x9800;
        }
    }

    uint8_t positionY = 0;

    if (usingWindow) {
        positionY = currentScanline - windowPositionY;
    } else {
        positionY = viewingAreaPositionY + currentScanline;
    }

    uint16_t titleRow = positionY/8 * 32;

    for (uint8_t pixel = 0; pixel < 160; pixel++) {
        uint8_t positionX = pixel + viewingAreaPositionX;

        if (usingWindow) {
            if (pixel >= windowPositionX) {
                positionX = pixel - windowPositionX;
            }
        }

        uint16_t tileColumn = positionX / 8;
        uint16_t tileAddress = backgroundMemory + titleRow + tileColumn;

        uint16_t tileLocation = tileData;
        if (useUnsigned) {
            uint8_t tileNumber = this->memoryController.readMemory8Bit(tileAddress);
            tileLocation += tileNumber * 16;
        } else {
            int8_t tileNumber = this->memoryController.readMemory8Bit(tileAddress);
            tileLocation += (tileNumber + 128) * 16;
        }

        uint8_t line = positionY % 8;
        line *= 2;

        uint16_t address1 = tileLocation + line;
        uint8_t data1 = this->memoryController.readMemory8Bit(address1);
        uint16_t address2 = tileLocation + line + 1;
        uint8_t data2 = this->memoryController.readMemory8Bit(address2);

        uint8_t colorBit = positionX % 8;
        colorBit -= 7;
        colorBit *= 1;

        uint8_t colorValue = getBit(data2, colorBit);
        colorValue <<= 1;
        colorValue |= getBit(data1, colorBit);

        uint8_t color = this->getColor(colorValue, 0xFF47);

        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;

        switch (color) {
        case 0: {
            red = 0xFF;
            green = 0xFF;
            blue = 0xFF;
            break;
        }
        case 1: {
            red = 0xCC;
            green = 0xCC;
            blue = 0xCC;
            break;
        }
        case 2: {
            red = 0x77;
            green = 0x77;
            blue = 0x77;
            break;
        }
        }

        if (currentScanline >= 143 || pixel > 159) {
            continue;
        }

        this->screenData[pixel][currentScanline][0] = red;
        this->screenData[pixel][currentScanline][1] = green;
        this->screenData[pixel][currentScanline][2] = blue;
    }
}

void PPUController::renderSprites() {
    uint8_t lcdController = this->memoryController.readMemory8Bit(LCDControllerAddress);
    uint8_t currentScanline = this->memoryController.readMemory8Bit(CurrentScanlineRegisterAddress);
    bool use8x16 = false;
    if (testBit(lcdController, 2)) {
        use8x16 = true;
    }

    for (uint8_t sprite = 0; sprite < 40; sprite++) {
        uint16_t spriteIndex = sprite * 4;
        uint8_t positionY = this->memoryController.readMemory8Bit(0xFE00+spriteIndex) - 16;
        uint8_t positionX = this->memoryController.readMemory8Bit(0xFE00+spriteIndex + 1) - 8;
        uint8_t tileLocation = this->memoryController.readMemory8Bit(0xFE00 + spriteIndex + 2);
        uint8_t attributes = this->memoryController.readMemory8Bit(0xFE00 + spriteIndex + 3);

        bool flipY = testBit(attributes, 6);
        bool flipX = testBit(attributes, 5);

        uint8_t height = 8;
        if (use8x16) {
            height = 16;
        }

        if (currentScanline >= positionY && currentScanline < positionY + height) {
            int32_t line = currentScanline - positionY;

            if (flipY) {
                line -= height;
                line *= -1;
            }

            line *= 2;
            int32_t dataAddress = (0x8000 + (tileLocation * 16)) + line;
            uint8_t data1 = this->memoryController.readMemory8Bit(dataAddress);
            uint8_t data2 = this->memoryController.readMemory8Bit(dataAddress + 1);

            for (int8_t tilePixel = 7; tilePixel >= 0; tilePixel--) {
                uint8_t colorBit = tilePixel;
                if (flipX) {
                    colorBit -= 7;
                    colorBit *= 1;
                }

                uint8_t colorValue = getBit(data2, colorBit);
                colorValue <<= 1;
                colorValue |= getBit(data1, colorBit);

                uint16_t colorAddress = 0xFF48;
                if (testBit(attributes, 4)) {
                    colorAddress = 0xFF49;
                }

                uint8_t color = this->getColor(colorValue, colorAddress);

                if (color == 0) {
                    continue;
                }

                uint8_t red = 0;
                uint8_t green = 0;
                uint8_t blue = 0;

                switch (color) {
                case 0: {
                    red = 0xFF;
                    green = 0xFF;
                    blue = 0xFF;
                    break;
                }
                case 1: {
                    red = 0xCC;
                    green = 0xCC;
                    blue = 0xCC;
                    break;
                }
                case 2: {
                    red = 0x77;
                    green = 0x77;
                    blue = 0x77;
                    break;
                }
                }

                uint8_t pixelX = 0 - tilePixel;
                pixelX += 7;

                uint8_t pixel = positionX + pixelX;

                if (currentScanline > 143 || pixel > 159) {
                    continue;
                }

                if (testBit(attributes, 7)) {
                    if (this->screenData[pixel][currentScanline][0] != 255 || this->screenData[pixel][currentScanline][1] != 255 || this->screenData[pixel][currentScanline][2] != 255) {
                        continue;
                    }
                }

                this->screenData[pixel][currentScanline][0] = red;
                this->screenData[pixel][currentScanline][1] = green;
                this->screenData[pixel][currentScanline][2] = blue;
            }
        }
    }
}

void PPUController::updateLCDStatus() {
    uint8_t status = this->memoryController.readMemory8Bit(LCDStatusRegisterAddress);
    if (!this->isLCDEnabled()) {
        this->scanlineRenderCyclesCounter = 456;
        this->memoryController.writeMemoryAvoidingTraps(CurrentScanlineRegisterAddress, 0);
        status &= 252;
        status = setBit(status, 0);
        this->memoryController.writeMemory(LCDStatusRegisterAddress, status);
        return;
    }

    uint8_t currentScanline = this->memoryController.readMemory8Bit(CurrentScanlineRegisterAddress);
    uint8_t currentMode = status & 0x3;

    uint8_t mode = 0;
    bool requestInterrupt = false;

    if (currentScanline >= 144) {
        mode = 1;
        status = setBit(status, 0);
        status = clearBit(status, 1);
        requestInterrupt = testBit(status, 4);
    } else {
        int16_t mode2Bounds = 456 - 80;
        int16_t mode3Bounds = mode2Bounds - 172;

        if (this->scanlineRenderCyclesCounter >= mode2Bounds) {
            mode = 2;
            status = clearBit(status, 0);
            status = setBit(status, 1);
            requestInterrupt = testBit(status, 5);
        } else if (this->scanlineRenderCyclesCounter >= mode3Bounds) {
            mode = 3;
            status = setBit(status, 0);
            status = clearBit(status, 1);
        } else {
            mode = 0;
            status = clearBit(status, 0);
            status = clearBit(status, 1);
            requestInterrupt = testBit(status, 3);
        }
    }

    if (requestInterrupt && mode != currentMode) {
        this->interruptController.requestInterrupt(1);
    }

    uint8_t coincidenceScanline = this->memoryController.readMemory8Bit(CoincidenceFlagAddress);
    if (currentScanline == coincidenceScanline) {
        status = setBit(status, 2);
        if (testBit(status, 6)) {
            this->interruptController.requestInterrupt(1);
        }
    } else {
        status = clearBit(status, 2);
    }

    this->memoryController.writeMemory(LCDStatusRegisterAddress, status);
}

bool PPUController::isLCDEnabled() {
    bool value = this->memoryController.readMemory8Bit(LCDControllerAddress);
    return testBit(value, 7);
}

void PPUController::updateScreen(uint8_t cycles) {
    this->updateLCDStatus();

    if (!this->isLCDEnabled()) {
        return;
    }

    this->scanlineRenderCyclesCounter -= cycles;
    if (this->scanlineRenderCyclesCounter > 0) {
        return;
    }

    uint8_t currentScanline = this->memoryController.readMemory8Bit(CurrentScanlineRegisterAddress);
    currentScanline += 1;
    this->memoryController.writeMemoryAvoidingTraps(CurrentScanlineRegisterAddress, currentScanline);
    this->scanlineRenderCyclesCounter = 456;

    if (currentScanline == 144) {
        this->interruptController.requestInterrupt(0);
    } else if (currentScanline >= 153) {
        this->memoryController.writeMemoryAvoidingTraps(CurrentScanlineRegisterAddress, 0);
    } else if (currentScanline < 144) {
        this->drawScanline();
    }
}

uint8_t PPUController::getColor(uint8_t color, uint16_t address) {
    uint8_t high = 0;
    uint8_t low = 0;
    uint8_t palette = this->memoryController.readMemory8Bit(address);

    switch (color) {
    case 0: {
        high = 1;
        low = 0;
        break;
    }
    case 1: {
        high = 3;
        low = 2;
        break;
    }
    case 2: {
        high = 5;
        low = 4;
        break;
    }
    case 3: {
        high = 7;
        low = 6;
        break;
    }
    }

    uint8_t result = getBit(palette, high);
    result <<= 1;
    result |= getBit(palette, low);

    return result;
}
