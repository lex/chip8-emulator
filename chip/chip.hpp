//
//  chip.hpp
//  chip
//
//  Created by lex on 22/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#ifndef chip_hpp
#define chip_hpp

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <array>

class Chip {
public:
    bool ReadRom(const std::string& file);
    void Initialize();
    void Step();

private:
    const size_t MAXIMUM_GAME_SIZE = 4096 - 512;

    // The VF register doubles as a flag for some instructions
    const uint8_t F = 0xF;

    const std::array<uint8_t, 80> FONTSET = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // CHIP-8 has 16 8-bit data registers named from V0 to VF.
    std::array<uint8_t, 16> V{0};
    // CHIP-8 was most commonly implemented on 4K systems. These machines had 4096 (0x1000) memory locations, all of which are 8 bits.
    std::array<uint8_t, 4096> memory{0};
    // The original 1802 version allocated 48 bytes for up to 24 levels of nesting; modern implementations normally have at least 16 levels.
    std::array<uint16_t, 16> stack;

    // 16bit register (For memory address) (Similar to void pointer)
    uint16_t I = 0;
    uint16_t PC = 0;
    uint16_t SP = 0;

    // Delay timer: This timer is intended to be used for timing the events of games. Its value can be set and read.
    uint8_t delayTimer = 0;
    // Sound timer: This timer is used for sound effects. When its value is nonzero, a beeping sound is made.
    uint8_t soundTimer = 0;

    void UnimplementedOpcode(const uint16_t& opcode) const;
    void UnknownOpcode(const uint16_t& opcode) const;
};

#endif /* chip_hpp */
