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
