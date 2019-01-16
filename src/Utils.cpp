#include "Utils.hpp"

bool testBit(uint8_t n, unsigned int pos) {
    bool bit = (n >> pos) & 1U;
    return bit;
}

uint8_t setBit(uint8_t n, unsigned int pos) {
    n |= 1UL << pos;
    return n;
}

uint8_t clearBit(uint8_t n, unsigned int pos) {
    n &= ~(1UL << pos);
    return n;
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

std::string formatHexUInt32(uint32_t data) {
    char buffer[100];
    sprintf(buffer, "%#04x", data);
    return buffer;
}

std::string formatBinaryUint8(uint8_t data) {
    std::bitset<sizeof(char) * 8> bs(data);
    return bs.to_string().substr(0, 4);
}
