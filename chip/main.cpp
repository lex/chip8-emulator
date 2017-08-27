//
//  main.cpp
//  chip
//
//  Created by lex on 21/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#include <iostream>
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

    while (running) {
        SDL_RenderClear(renderer);

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        chip.Step();
                        break;

                    case SDLK_r:
                        chip.Initialize();
                        break;

                    case SDLK_d:
                        chip.debugging = !chip.debugging;
                        break;

                }
            }
        }

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

        // try to keep it around 60 Hz
        // seems like drawing the screen is already
        // taking way longer than it should though
        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
