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

// https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description
#define REGISTER_COUNT 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define FONTSET_SIZE 80
#define FONTSET_CHARACTER_SIZE 5
#define FONTSET_ADDRESS 0
#define F 15
#define PROGRAM_START_ADDRESS 512
#define MAXIMUM_GAME_SIZE MEMORY_SIZE - PROGRAM_START_ADDRESS
#define VIDEO_MEMORY_COLUMNS 64
#define VIDEO_MEMORY_ROWS 32
#define KEY_COUNT 16

using VideoMemory = std::array<std::array<uint8_t, VIDEO_MEMORY_COLUMNS>, VIDEO_MEMORY_ROWS>;
using PressedKeys = std::array<bool, KEY_COUNT>;

class Chip {
public:
    bool ReadRom(const std::string& file);
    void Initialize();
    void Step();
    void Stop();
    void Resume();
    const VideoMemory& GetVideoMemory() const;
    bool debugging = false;
    void SetPressedKeys(const PressedKeys& keys);
    bool readInputs = false;

private:
    const std::array<uint8_t, FONTSET_SIZE> FONTSET{
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
    std::array<uint8_t, REGISTER_COUNT> V{0};

    // CHIP-8 was most commonly implemented on 4K systems. These machines had 4096 (0x1000) memory locations, all of which are 8 bits.
    std::array<uint8_t, MEMORY_SIZE> memory{0};

    // Original CHIP-8 Display resolution is 64×32 pixels, and color is monochrome.
    VideoMemory videoMemory{0};

    // The original 1802 version allocated 48 bytes for up to 24 levels of nesting; modern implementations normally have at least 16 levels.
    std::array<uint16_t, STACK_SIZE> stack;

    // 16bit register (For memory address) (Similar to void pointer)
    uint16_t I = 0;
    uint16_t PC = 0;
    uint16_t SP = 0;

    // Delay timer: This timer is intended to be used for timing the events of games. Its value can be set and read.
    uint8_t delayTimer = 0;

    // Sound timer: This timer is used for sound effects. When its value is nonzero, a beeping sound is made.
    uint8_t soundTimer = 0;

    PressedKeys pressedKeys{false};

    void UnimplementedOpcode(const uint16_t& opcode) const;
    void UnknownOpcode(const uint16_t& opcode) const;
    void ClearScreen();
    void DrawSprite(const uint8_t& x, const uint8_t& y, const uint8_t& height);
};

#endif /* chip_hpp */
