#include "Utils.hpp"

bool testBit(uint8_t n, unsigned int pos) {
    uint8_t mask = 1 << pos;
    return (n & mask) > 0;
}

uint8_t setBit(uint8_t n, unsigned int pos) {
    uint8_t mask = 1 << pos;
    return n |= mask;
}

uint8_t clearBit(uint8_t n, unsigned int pos) {
    uint8_t mask = (1 << pos);
    mask ^= mask;
    return n & mask;
}

bool getBit(uint8_t n, unsigned int pos) {
    uint8_t mask = 1 << pos;
    return (n & mask) == 0;
}

std::string formatDecUInt8(uint8_t data) {
    char buffer[100];
    sprintf(buffer, "%02d", data);
    return buffer;
}

std::string formatHexUInt8(uint8_t data) {
    char buffer[100];
    sprintf(buffer, "0x%02x", data);
    return buffer;
}

std::string formatHexUInt16(uint16_t data) {
    char buffer[100];
    sprintf(buffer, "0x%04x", data);
    return buffer;
}

std::string formatBinaryUint8(uint8_t data) {
    std::bitset<sizeof(char) * 8> bs(data);
    return bs.to_string().substr(0, 4);
}
