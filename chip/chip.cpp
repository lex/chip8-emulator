//
//  chip.cpp
//  chip
//
//  Created by lex on 22/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#include "chip.hpp"

std::array<uint8_t, 4096> rom{0};

int Chip::readRom(const std::string& file) {
    std::ifstream is(file, std::ios::binary | std::ios::ate);

    if (!is.is_open()) {
        std::cout << "couldn't open file " << file << std::endl;
        return 1;
    }

    size_t size = is.tellg();
    std::cout << "rom size: " << size << std::endl;

    if (size > MAXIMUM_GAME_SIZE) {
        std::cout << "rom too large: " << size << " bytes" << std::endl;
        is.close();
        return 1;
    }

    is.seekg(0);

    is.read(reinterpret_cast<char*>(&rom[512]), size);

    is.close();

    int g = 0;

    for (const auto& b : rom) {
        std::cout << g++ << ": ";
        std::cout << std::hex << std::uppercase << static_cast<int>(b);
        std::cout << std::dec << std::nouppercase << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
