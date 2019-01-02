#pragma once
#include "Register.hpp"

class CPU {
private:
    Register16Bit af;
    Register16Bit bc;
    Register16Bit de;
    Register16Bit hl;

    Register16Bit programCounter;
public:
    CPU();
    ~CPU();
};
