#pragma once
#include <cstdint>

const static uint16_t JoypadRegisterAddress = 0xFF00;

class JoypadController {

public:
    JoypadController();
    ~JoypadController();
};
