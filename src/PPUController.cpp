#include "PPUController.hpp"
#include "Utils.hpp"

PPUController::PPUController() {

}

PPUController::~PPUController() {

}

void PPUController::drawScanline() {

}

void PPUController::renderTiles() {

}

void PPUController::renderSprites() {

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
