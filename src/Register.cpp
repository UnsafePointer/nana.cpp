#include "Register.hpp"

uint16_t Register16Bit::value() {
    uint16_t low = this->low.bits;
    uint16_t high = this->high.bits << 8;
    return low | high;
}

void Register16Bit::setValue(uint16_t value) {
    this->low.bits = value & 0xFF;
    this->high.bits = value >> 8;
}

void Register16Bit::increment() {
    this->setValue(this->value() + 1);
}

void Register16Bit::decrement() {
    this->setValue(this->value() + 1);
}
