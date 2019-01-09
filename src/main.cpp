#include <iostream>
#include <string>
#include "Emulator.hpp"
#include "TimerController.hpp"
#include "MemoryController.hpp"
#include "InterruptController.hpp"
#include "CPUController.hpp"
#include "PPUController.hpp"
#include "JoypadController.hpp"

int main(int argc, char const *argv[])
{
    if (argc <= 1) {
        exit(1);
    }
    std::string gameArg = argv[1];
    MemoryController memoryController = MemoryController();
    CPUController cpuController = CPUController(memoryController);
    InterruptController interruptController = InterruptController(memoryController, cpuController);
    PPUController ppuController = PPUController(memoryController, interruptController);
    TimerController timerController = TimerController(memoryController, interruptController);
    memoryController.timerController = &timerController;
    JoypadController joypadController = JoypadController(interruptController, memoryController);
    memoryController.joypadController = &joypadController;
    Emulator emulator = Emulator(cpuController, timerController, interruptController, ppuController, memoryController);
    emulator.memoryController->loadCartridge(gameArg);
    while (true) {
        emulator.emulateFrame();
    }
    return 0;
}
