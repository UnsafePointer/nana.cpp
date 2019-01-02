#pragma once
#include <cstdint>

const static uint16_t DividerRegisterAddress = 0xFF04;
const static uint16_t TimerControllerAddress = 0xFF07;

class TimerController {

public:
    TimerController();
    ~TimerController();
};
