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

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 640

size_t mapKey(SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_1:
            return 0x1;
        case SDLK_2:
            return 0x2;
        case SDLK_3:
            return 0x3;
        case SDLK_4:
            return 0xC;
        case SDLK_q:
            return 0x4;
        case SDLK_w:
            return 0x5;
        case SDLK_e:
            return 0x6;
        case SDLK_r:
            return 0xD;
        case SDLK_a:
            return 0x7;
        case SDLK_s:
            return 0x8;
        case SDLK_d:
            return 0x9;
        case SDLK_f:
            return 0xE;
        case SDLK_z:
            return 0xA;
        case SDLK_x:
            return 0x0;
        case SDLK_c:
            return 0xB;
        case SDLK_v:
            return 0xF;
        default:
            return 0x10;
    }
}

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


    Chip chip;

    if (!chip.ReadRom(file)) {
        return 1;
    }

    chip.Initialize();

    const uint32_t targetMilliseconds = static_cast<uint32_t>(1.0f / 500.0f * 1000.0f);

    SDL_Event event;
    SDL_Texture* texture;
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
    bool running = true;

    while (running) {
        const uint32_t startTicks = SDL_GetTicks();

        SDL_PumpEvents();

        SDL_SetRenderTarget(renderer, texture);
        SDL_RenderSetScale(renderer, 20, 20);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 224, 119, 32, 255);

        // handle input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        // fix this, timers will break
                        chip.Step(startTicks);
                        break;

                    case SDLK_ESCAPE:
                        chip.Initialize();
                        break;

                    case SDLK_BACKSPACE:
                        chip.debugging = !chip.debugging;
                        break;

                    default:
                        const size_t key = mapKey(event.key.keysym.sym);

                        if (key <= 0xF) {
                            chip.SetKeyState(key, event.type == SDL_KEYDOWN);
                        }

                        break;
                }
            }
        }

        if (!chip.debugging) {
            chip.Step(startTicks);
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

        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // keep it around 500 Hz
        const uint32_t endTicks = SDL_GetTicks();
        const uint32_t dt = std::min(endTicks - startTicks, targetMilliseconds);

        SDL_Delay(targetMilliseconds - dt);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
