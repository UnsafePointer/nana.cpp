#pragma once
#include <cstdint>

struct Register8Bit {
    uint8_t bits;
};

struct Register16Bit {
    Register8Bit low;
    Register8Bit high;

    uint16_t value();
    void setValue(uint16_t value);
    void increment();
    void decrement();
};
