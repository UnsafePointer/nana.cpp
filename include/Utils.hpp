#pragma once
#include <cstdint>
#include <string>
#include <bitset>

bool testBit(uint8_t n, unsigned int pos);

uint8_t setBit(uint8_t n, unsigned int pos);

uint8_t clearBit(uint8_t n, unsigned int pos);

bool getBit(uint8_t n, unsigned int pos);

std::string formatDecUInt8(uint8_t data);

std::string formatHexUInt8(uint8_t data);

std::string formatHexUInt16(uint16_t data);

std::string formatBinaryUint8(uint8_t data);
