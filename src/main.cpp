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

#ifdef __SWITCH__
#include <switch.h>
#endif

const static uint16_t width = 1920 / 6; // = 320
const static uint16_t height = 1080 / 6; // = 180
const static uint16_t contentWidth = 160;
const static uint16_t contentHeight = 144;
const static uint16_t contentSpaceX = (width - contentWidth) / 2;
const static uint16_t contentSpaceY = (height - contentHeight) / 2;
const static uint16_t contentScale = 6;

using namespace std;

int main(int argc, char const *argv[])
{
    bool enableDebug = false;
    int maxCycles = 0;
    bool enableMemoryAccessDebug = false;
    string gameArg = "romfs:/roms/tetris.gb";

#ifdef __SWITCH__
    Result result = romfsInit();
    if (R_FAILED(result)) {
        cout << "romfs init failed" << endl;
    }
#else
    if (argc <= 1) {
        exit(1);
    }
    char *nanaDebug = getenv("NANA_DEBUG");
    if (nanaDebug != NULL) {
        enableDebug = true;
    }

    char *nanaMaxCycles = getenv("NANA_MAX_CYCLES");
    if (nanaMaxCycles != NULL) {
        maxCycles = strtol(nanaMaxCycles, nullptr, 10);
    }

    char *nanaMemoryAccessDebug = getenv("NANA_MEMORY_ACCESS_DEBUG");
    if (nanaMemoryAccessDebug != NULL) {
        enableMemoryAccessDebug = true;
    }
    gameArg = argv[1];
#endif

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

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        cout << "SDL init failed with error: " << SDL_GetError() << endl;
        exit(1337);
    }
    SDL_Window *window = SDL_CreateWindow("ナナ.cpp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * contentScale, height * contentScale, SDL_WINDOW_OPENGL);
    if (window == nullptr) {
        cout << "SDL create window failed with error: " << SDL_GetError() << endl;
        exit(1337);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) {
        cout << "SDL create renderer failed with error: " << SDL_GetError() << endl;
        exit(1337);
    }
    SDL_RenderSetScale(renderer, contentScale, contentScale);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    uint32_t format;
    SDL_QueryTexture(texture, &format, nullptr, nullptr, nullptr);

#ifdef __SWITCH__
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            cout << "SDL joystick open failed with error: " << SDL_GetError() << endl;
            exit(1337);
        }
    }
#endif

    bool quit = false;
    float fps = 59.73;
    float interval = 1000;
    interval /= fps;
    uint32_t initTicks = SDL_GetTicks();
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
#ifdef __SWITCH__
            joypadController.handleJoystickEvent(&event);
#else
            joypadController.handleKeyboardEvent(&event.key);
#endif
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        uint32_t currentTicks = SDL_GetTicks();
        if (initTicks + interval < currentTicks) {
            emulator.emulateFrame();
            uint32_t *pixels = nullptr;
            int pitch = 0;
            if (SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch) > 0) {
                exit(1);
            }
            for (int x = contentSpaceX; x < width - contentSpaceX; x++) {
                for (int y = contentSpaceY; y < height - contentSpaceY; y++) {
                    uint32_t pixelPosition = y * (pitch / sizeof(unsigned int)) + x;
                    uint8_t screenDataPositionX = x - contentSpaceX;
                    uint8_t screenDataPositionY = y - contentSpaceY;
                    uint8_t r = ppuController.screenData[screenDataPositionX][screenDataPositionY][0];
                    uint8_t g = ppuController.screenData[screenDataPositionX][screenDataPositionY][1];
                    uint8_t b = ppuController.screenData[screenDataPositionX][screenDataPositionY][2];
                    uint8_t rgb[4] = {0xFF, b, g, r};
                    uint32_t color = 0;
                    memcpy(&color, rgb, sizeof(rgb));
                    pixels[pixelPosition] = color;
                }
            }
            SDL_UnlockTexture(texture);
            if (SDL_RenderClear(renderer) != 0) {
                cout << "SDL render clear failed with error: " << SDL_GetError() << endl;
                exit(1337);
            }
            if (SDL_RenderCopy(renderer, texture, nullptr, nullptr) != 0) {
                cout << "SDL render clear failed with error: " << SDL_GetError() << endl;
                exit(1337);
            }
            SDL_RenderPresent(renderer);
            initTicks = SDL_GetTicks();
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
