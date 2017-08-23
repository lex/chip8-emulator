//
//  main.cpp
//  chip
//
//  Created by lex on 21/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#include <iostream>
#include "chip.hpp"

int main(int argc, const char * argv[]) {
    if (argc < 2) {
        std::cout << "need rom" << std::endl;
        return 1;
    }

    const std::string file = argv[1];

    Chip chip;

    if (!chip.readRom(file)) {
        return 1;
    }

    chip.initialize();

    while (true) {
        chip.step();
    }

    return 0;
}
