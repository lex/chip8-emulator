//
//  main.cpp
//  chip
//
//  Created by lex on 21/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <SDL.h>

#include "chip.hpp"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cout << "need rom" << std::endl;
        return 1;
    }

    const std::string file = argv[1];

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);

    if (window == nullptr || renderer == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_RenderSetScale(renderer, 10, 10);

    Chip chip;

    if (!chip.ReadRom(file)) {
        return 1;
    }

    chip.Initialize();

    SDL_Event event;
    bool running = true;

    PressedKeys keys{false};

    const uint32_t targetMilliseconds = static_cast<uint32_t>(1.0f / 60.0f * 1000.0f);

    while (running) {
        const uint32_t startTicks = SDL_GetTicks();

        SDL_PumpEvents();
        SDL_RenderClear(renderer);

        if (chip.readInputs) {
            std::fill(keys.begin(), keys.end(), false);
            chip.readInputs = false;
        }

        while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                    chip.Step();
                    break;

                case SDLK_ESCAPE:
                    chip.Initialize();
                    break;

                case SDLK_BACKSPACE:
                    chip.debugging = !chip.debugging;
                    break;

                case SDLK_1:
                    keys.at(0x1) = true;
                    break;

                case SDLK_2:
                    keys.at(0x2) = true;
                    break;

                case SDLK_3:
                    keys.at(0x3) = true;
                    break;

                case SDLK_4:
                    keys.at(0xC) = true;
                    break;

                case SDLK_q:
                    keys.at(0x4) = true;
                    break;

                case SDLK_w:
                    keys.at(0x5) = true;
                    break;

                case SDLK_e:
                    keys.at(0x6) = true;
                    break;

                case SDLK_r:
                    keys.at(0xD) = true;
                    break;

                case SDLK_a:
                    keys.at(0x7) = true;
                    break;

                case SDLK_s:
                    keys.at(0x8) = true;
                    break;

                case SDLK_d:
                    keys.at(0x9) = true;
                    break;

                case SDLK_f:
                    keys.at(0xE) = true;
                    break;

                case SDLK_z:
                    keys.at(0xA) = true;
                    break;

                case SDLK_x:
                    keys.at(0x0) = true;
                    break;

                case SDLK_c:
                    keys.at(0xB) = true;
                    break;

                case SDLK_v:
                    keys.at(0xF) = true;
                    break;

                case SDLK_RIGHT:
                    keys.at(6) = true;
                    break;

                case SDLK_LEFT:
                    keys.at(5) = true;
                    break;

                case SDLK_UP:
                    keys.at(4) = true;
                    break;

                case SDLK_DOWN:
                    keys.at(1) = true;
                    break;
                }
            }
        }

        chip.SetPressedKeys(keys);

        if (!chip.debugging) {
            chip.Step();
        }

        // draw the screen
        int x = 0;
        int y = 0;

        for (auto& row : chip.GetVideoMemory()) {
            for (auto& pixel : row) {
                if (pixel == 1) {
                    SDL_RenderDrawPoint(renderer, x, y);
                }

                ++x;
            }

            x = 0;
            ++y;
        }

        SDL_RenderPresent(renderer);

        // keep it around 60 Hz
        const uint32_t endTicks = SDL_GetTicks();
        const uint32_t dt = std::min(endTicks - startTicks, targetMilliseconds);

        SDL_Delay(targetMilliseconds - dt);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
