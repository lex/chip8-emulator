//
//  chip.cpp
//  chip
//
//  Created by lex on 22/08/2017.
//  Copyright © 2017 lex. All rights reserved.
//

#include "chip.hpp"

bool Chip::ReadRom(const std::string& file) {
    std::ifstream is(file, std::ios::binary | std::ios::ate);

    if (!is.is_open()) {
        std::cout << "couldn't open file " << file << std::endl;
        return false;
    }

    const size_t size = is.tellg();
    std::cout << "rom size: " << size << " bytes" << std::endl;

    if (size > MAXIMUM_GAME_SIZE) {
        std::cout << "rom too large: " << size << " bytes" << std::endl;
        is.close();
        return false;
    }

    is.seekg(0);

    is.read(reinterpret_cast<char*>(&memory.at(PROGRAM_START_ADDRESS)), size);

    is.close();

    return true;
}

void Chip::Initialize() {
    PC = PROGRAM_START_ADDRESS;
    I = 0;
    SP = 0;
    delayTimer = 0;
    soundTimer = 0;

    ClearScreen();
    std::copy_n(FONTSET.begin(), FONTSET_SIZE, memory.begin());
}

void Chip::ClearScreen() {
    for (auto& row : videoMemory) {
        std::fill(row.begin(), row.end(), 0);
    }

    if (debugging) {
        std::cout << "Screen cleared" << std::endl;
    }
}

const VideoMemory& Chip::GetVideoMemory() const {
    return videoMemory;
}

void Chip::DrawSprite(const uint8_t &x, const uint8_t &y, const uint8_t &height) {
    V[F] = 0;

    for (int byteIndex = 0; byteIndex < height; ++byteIndex) {
        const uint8_t byte = memory[I + byteIndex];

        for (int bitIndex = 0; bitIndex < 8; ++bitIndex) {
            const uint8_t bit = (byte >> bitIndex) & 0x1;

            const size_t memoryX = (x + (7 - bitIndex)) % VIDEO_MEMORY_COLUMNS;
            const size_t memoryY = (y + byteIndex) % VIDEO_MEMORY_ROWS;

            const uint8_t oldBit = videoMemory.at(memoryY).at(memoryX);

            // Sprite pixels that are set flip the color of the corresponding screen pixel, while unset sprite pixels do nothing.
            videoMemory.at(memoryY).at(memoryX) = oldBit ^ bit;

            // The carry flag (VF) is set to 1 if any screen pixels are flipped from set to unset when a sprite is drawn and set to 0 otherwise.
            if (oldBit == 1 && bit == 1) {
                V[F] = 1;
            }

            if (debugging && V[F] == 1) {
                std::cout << "Collision detected at (" << memoryX << ", " << memoryY << ")" << std::endl;
            }
        }
    }
}

void Chip::Step() {
    const uint16_t opcode = memory[PC] << 8 | memory[PC + 1];

    // https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
    // NNN: address
    // NN: 8-bit constant
    // N: 4-bit constant
    // X and Y: 4-bit register identifier
    const uint16_t NNN = opcode & 0x0FFF;
    const uint8_t NN = opcode & 0x00FF;
    const uint8_t N = opcode & 0x00F;
    const uint8_t X = (opcode >> 8) & 0x000F;
    const uint8_t Y = (opcode >> 4) & 0x000F;

    if (debugging) {
        std::cout << "PC: " << PC << " Opcode: ";
        std::cout << std::hex << std::uppercase << static_cast<int>(opcode) << std::endl;
        std::cout << std::dec << std::nouppercase << std::endl;
    }

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (NN) {
                case 0x00E0:
                    // Clears the screen.
                    ClearScreen();
                    PC += 2;
                    break;

                case 0x00EE:
                    // Returns from a subroutine.
                    PC = stack.at(--SP);
                    break;

                default:
                    UnknownOpcode(opcode);
                    break;
            }

            break;

        case 0x1000:
            // Jumps to address NNN.
            PC = NNN;
            break;

        case 0x2000:
            // Calls subroutine at NNN.
            stack.at(SP) = PC + 2;
            ++SP;
            PC = NNN;
            break;

        case 0x3000:
            // Skips the next instruction if VX equals NN.
            PC += (V[X] == NN ? 4 : 2);
            break;

        case 0x4000:
            // Skips the next instruction if VX doesn't equal NN.
            PC += (V[X] != NN ? 4 : 2);
            break;

        case 0x5000:
            // Skips the next instruction if VX equals VY.
            PC += (V[X] == V[Y] ? 4 : 2);
            break;

        case 0x6000:
            // Sets VX to NN.
            V[X] = NN;
            PC += 2;
            break;

        case 0x7000:
            // Adds NN to VX.
            V[X] += NN;
            PC += 2;
            break;

        case 0x8000:
            switch (N) {
                case 0x0:
                    // Sets VX to the value of VY.
                    V[X] = V[Y];
                    PC += 2;
                    break;

                case 0x1:
                    // Sets VX to VX or VY.
                    V[X] = V[X] | V[Y];
                    PC += 2;
                    break;

                case 0x2:
                    // Sets VX to VX and VY.
                    V[X] = V[X] & V[Y];
                    PC += 2;
                    break;

                case 0x3:
                    // Sets VX to VX xor VY.
                    V[X] = V[X] ^ V[Y];
                    PC += 2;
                    break;

                case 0x4:
                    // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    V[F] = (static_cast<int>(V[X]) + static_cast<int>(V[Y]) > 255 ? 1 : 0);
                    V[X] += V[Y];
                    PC += 2;
                    break;

                case 0x5:
                    // VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    V[F] = (V[X] > V[Y] ? 1 : 0);
                    V[X] -= V[Y];
                    PC += 2;
                    break;

                case 0x6:
                    // Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
                    V[F] = V[X] & 0x1;
                    V[X] = V[X] >> 1;
                    PC += 2;
                    break;

                case 0x7:
                    // Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    V[F] = (V[X] < V[Y] ? 1 : 0);
                    V[X] = V[Y] - V[X];
                    PC += 2;
                    break;

                case 0xE:
                    // Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
                    V[F] = (V[X] >> 7) & 0x1;
                    V[X] = V[X] << 1;
                    PC += 2;
                    break;

                default:
                    UnknownOpcode(opcode);
                    break;
            }

            break;

        case 0x9000:
            switch (N) {
                case 0:
                    // Skips the next instruction if VX doesn't equal VY.
                    PC += (V[X] != V[Y] ? 4 : 2);
                    break;

                default:
                    UnknownOpcode(opcode);
                    break;
            }

            break;

        case 0xA000:
            // Sets I to the address NNN.
            I = NNN;
            PC += 2;
            break;

        case 0xB000:
            // Jumps to the address NNN plus V0.
            PC = NNN + V[0x0];
            break;

        case 0xC000:
            // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            V[X] = (std::rand() % 256) & NN;
            PC += 2;
            break;

        case 0xD000:
            // Draws a sprite at coordinate (VX, VY)
            DrawSprite(V[X], V[Y], N);
            PC += 2;
            break;

        case 0xE000:
            switch (NN) {
                case 0x9E:
                    // Skips the next instruction if the key stored in VX is pressed.
                    UnimplementedOpcode(opcode);
                    PC += 2;
                    break;

                case 0xA1:
                    // Skips the next instruction if the key stored in VX isn't pressed.
                    UnimplementedOpcode(opcode);
                    PC += 4;
                    break;

            }

            break;

        case 0xF000:
            switch (NN) {
                case 0x07:
                    // Sets VX to the value of the delay timer.
                    V[X] = delayTimer;
                    PC += 2;
                    break;

                case 0x0A:
                    // A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
                    UnimplementedOpcode(opcode);
                    PC += 2;
                    break;

                case 0x15:
                    // Sets the delay timer to VX.
                    delayTimer = V[X];
                    PC += 2;
                    break;

                case 0x18:
                    // Sets the sound timer to VX.
                    soundTimer = V[X];
                    PC += 2;
                    break;

                case 0x1E:
                    // Adds VX to I.
                    I += V[X];
                    PC += 2;
                    break;

                case 0x29:
                    // Sets I to the location of the sprite for the character in VX.
                    I = FONTSET_CHARACTER_SIZE * V[X];
                    PC += 2;
                    break;

                case 0x33:
                    // Stores the binary-coded decimal representation of VX
                    memory[I] = (V[X] % 1000) / 100;
                    memory[I + 1] = (V[X] % 100) / 10;
                    memory[I + 2] = (V[X] % 10);
                    PC += 2;
                    break;

                case 0x55:
                    // Stores V0 to VX (including VX) in memory starting at address I.
                    for (int i = 0; i < X; ++i) {
                        memory.at(I + i) = V[i];
                    }

                    I += X + 1;
                    PC += 2;
                    break;

                case 0x65:
                    // Fills V0 to VX (including VX) with values from memory starting at address I.
                    for (int i = 0; i < X; ++i) {
                        V[i] = memory.at(I + i);
                    }

                    I += X + 1;
                    PC += 2;
                    break;
            }

            break;

        default:
            UnknownOpcode(opcode);
            break;
    }

    if (delayTimer > 0) {
        --delayTimer;
    }

    if (soundTimer > 0) {
        --soundTimer;
    }
}

void Chip::UnimplementedOpcode(const uint16_t& opcode) const {
    std::cout << "Unimplemented opcode: "
              << std::hex << std::uppercase << static_cast<int>(opcode)
              << std::dec << std::nouppercase << std::endl;
}

void Chip::UnknownOpcode(const uint16_t& opcode) const {
    std::cout << "Unknown opcode: "
              << std::hex << std::uppercase << static_cast<int>(opcode)
              << std::dec << std::nouppercase << std::endl;
}
