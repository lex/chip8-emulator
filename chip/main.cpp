//
//  main.cpp
//  chip
//
//  Created by lex on 21/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstdint>
#include <array>

std::array<uint8_t, 4096> rom{0};

int readRom(const std::string& file) {
    std::ifstream i(file, std::ios::binary);

    if (!i.is_open()) {
        std::cout << "couldn't open file " << file << std::endl;
        return 1;
    }

    i.read(reinterpret_cast<char*>(&rom), sizeof rom);

    i.close();

    for (const auto& b : rom) {
        std::cout << b;
    }

    std::cout << std::endl;

    return 0;
}

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        std::cout << "need rom" << std::endl;
        return 1;
    }

    const std::string file = argv[1];

    int r = readRom(file);

    return r;
}
