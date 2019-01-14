#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "Emulator.hpp"
#include "TimerController.hpp"
#include "MemoryController.hpp"
#include "InterruptController.hpp"
#include "CPUController.hpp"
#include "PPUController.hpp"
#include "JoypadController.hpp"
#include "Logger.hpp"

const static uint16_t width = 160;
const static uint16_t height = 144;
const static uint16_t scale = 5;

int main(int argc, char const *argv[])
{
    if (argc <= 1) {
        exit(1);
    }
    char *nanaDebug = std::getenv("NANA_DEBUG");
    bool enableDebug = false;
    if (nanaDebug != NULL) {
        enableDebug = true;
    }
    char *nanaMaxCycles = std::getenv("NANA_MAX_CYCLES");
    int maxCycles = 0;
    if (nanaMaxCycles != NULL) {
        maxCycles = strtol(nanaMaxCycles, nullptr, 10);
    }
    char *nanaMemoryAccessDebug = std::getenv("NANA_MEMORY_ACCESS_DEBUG");
    bool enableMemoryAccessDebug = false;
    if (nanaMemoryAccessDebug != NULL) {
        enableMemoryAccessDebug = true;
    }
    std::string gameArg = argv[1];
    Logger logger = Logger(enableDebug);
    logger.setupLogFile();
    MemoryController memoryController = MemoryController(logger, enableMemoryAccessDebug);
    CPUController cpuController = CPUController(memoryController, logger);
    InterruptController interruptController = InterruptController(memoryController, cpuController, logger);
    PPUController ppuController = PPUController(memoryController, interruptController, logger);
    TimerController timerController = TimerController(memoryController, interruptController, logger);
    memoryController.timerController = &timerController;
    JoypadController joypadController = JoypadController(interruptController, memoryController, logger);
    memoryController.joypadController = &joypadController;
    Emulator emulator = Emulator(cpuController, timerController, interruptController, ppuController, logger, maxCycles, memoryController);
    emulator.memoryController->loadCartridge(gameArg);

    if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("ナナ", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * scale, height * scale, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        exit(1);
    }
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    uint32_t format;
    SDL_QueryTexture(texture, &format, nullptr, nullptr, nullptr);
    while (true) {
        uint32_t initTicks = SDL_GetTicks();
        emulator.emulateFrame();
        uint32_t *pixels = nullptr;
        int pitch = 0;
        if (SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch) > 0) {
            exit(1);
        }
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                uint32_t pixelPosition = y * (pitch / sizeof(unsigned int)) + x;
                uint8_t r = ppuController.screenData[x][y][0];
                uint8_t g = ppuController.screenData[x][y][1];
                uint8_t b = ppuController.screenData[x][y][2];
                uint8_t rgb[4] = {r, g, b, 0xFF};
                uint32_t color = 0;
                memcpy(&color, rgb, sizeof(rgb));
                pixels[pixelPosition] = color;
            }
        }
        SDL_UnlockTexture(texture);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        uint32_t currentTicks = SDL_GetTicks();
        uint32_t frameTime = 1000 / 60;
        uint32_t currentFrameTime = currentTicks - initTicks;
        if (currentFrameTime > frameTime) {
            continue;
        } else {
            uint32_t delayTime = frameTime - currentFrameTime;
            SDL_Delay(delayTime);
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
