#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

uint8_t memory[MEM_SIZE];
uint8_t V[REG_COUNT]; //Data Registers (V0-VF)
uint8_t key[KEY_SIZE]; //Tracks key presses
uint16_t I; //Address register
uint16_t PC; //Program counter
uint8_t delay_timer;
uint8_t sound_timer;

#define FONTSET_ADDRESS 0x0
#define FONTSET_BYTES_PER_CHAR 5
uint8_t font[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80, //F
};

void chip8_init()
{
    PC = PROGRAM_START;

    //initialize memory
    memset(memory, 0, sizeof(uint8_t) * MEM_SIZE);
    memset(V, 0, sizeof(uint8_t) * REG_COUNT);
    memset(key, 0, sizeof(uint8_t) * KEY_SIZE);

    //load font into memory
    for (int i = 0; i < FONT_SIZE; i++)
    {
        memory[FONTSET_ADDRESS + i] = font[i];
    }
}

int chip8_load(char* fname)
{
    //Read in file fname
    //Write file to memory starting at 0x200
    //Write MAX_PROGRAM_SIZE bytes
    FILE* game;

    game = fopen(fname, "r");

    if (game == NULL)
    {
        fprintf(stderr, "ERROR: Unable to open file: %s\n", fname);
        return -1;
    }

    fread(&memory[PROGRAM_START], 1, MAX_PROGRAM_SIZE, game);

    fclose(game);

    return 1;
}

void chip8_cycle()
{
    uint16_t opcode;
    uint16_t addr;
    uint8_t nn;
    uint8_t n;
    uint8_t x;
    uint8_t y;

    opcode = memory[PC] << 8 | memory[PC + 1];
    addr = opcode & 0x0FFF;      //lowest three bytes
    nn = opcode & 0x00FF;        //lowest two bytes
    n = opcode & 0x000F;         //lowest byte
    x = (opcode >> 8) & 0x000F;  //lower byte of upper bytes
    y = (opcode >> 4) & 0x000F;  //upper byte of lower bytes

    switch(opcode & 0xF000)
    {
        case 0x0000:
            switch(opcode & 0x00FF)
            {
                case 0x00E0:
                    //clear the screen
                    break;
                case 0x00EE:
                    //Return from a subroutine
                    break;
                default: //unknown opcode
		    break;
            }
            PC += 2;
            break;
        case 0x1000:
            //Jump to addr
            PC = addr;
            break;
        case 0x2000:
            //Execute subroutine starting at addr
            PC += 2;
            break;
        case 0x3000:
            //Skip following instruction if VX equals NN
            if (V[x] == nn)
                PC += 4;
            else
                PC += 2;
            break;
        case 0x4000:
            //Skip following instruction if VX does not equal NN
            if (V[x] != nn)
                PC += 4;
            else
                PC += 2;
            break;
        case 0x5000:
            //Skip following instruction if VX equals VY
            if (V[x] == V[y])
                PC += 4;
            else
                PC += 2;
            break;
        case 0x6000:
            //Store NN at VX
            V[x] = nn;
            PC += 2;
            break;
        case 0x7000:
            //Add NN to VX
            V[x] += nn;
            PC += 2;
            break;
        case 0x8000:
            switch(n)
            {
                case 0x0:
                    //Set VX to VY
                    V[x] = V[y];
                    break;
                case 0x1:
                    //Set VX to VX OR VY
                    V[x] |= V[y];
                    break;
                case 0x2:
                    //Set VX to VX AND VY
                    V[x] &= V[y];
                    break;
                case 0x3:
                    //Set VX to VX XOR VY
                    V[x] ^= V[y];
                    break;
                case 0x4:
                    //Set VX to VX plus VY and if carry occurs set VF to 01 else set VF to 00
                    if (((V[x] + V[y]) & 0xF00) > 0)
                        V[0xF] = 0x01;
                    else
                        V[0xF] = 0x00;
                    V[x] += V[y];
                    break;
                case 0x5:
                    //Set VX to VX minus VY and if borrow occurs set VF to 00 else set VF to 01
                    if (V[x] > V[y])
                        V[0xF] = 0x01;
                    else
                        V[0xF] = 0x00;
                    V[x] -= V[y];
                    break;
                case 0x6:
                    //Set VX to VY shifted right one and set VF to LSB prior to shift
                    V[0xF] = V[y] & 0x01;
                    V[x] = V[y] >> 1;
                    break;
                case 0x7:
                    //Set VX to VY minus VX and if borrow occurs set VF to 00 else set VF to 01
                    if (V[y] > V[x])
                        V[0xF] = 0x01;
                    else
                        V[0xF] = 0x00;
                    V[x] = V[y] - V[x];
                    break;
                case 0xE:
                    //Set VX to VY shifted left one and set VF to MSB prior to shift
                    V[0xF] = V[y] & 0x80;
                    V[x] = V[y] << 1;
                    break;
                default:
                    //unknown opcode
		    break;
            }
            PC += 2;
            break;
        case 0x9000:
            //Skip following instruction if VX does not equal VY
            if (V[x] != V[y])
                PC += 4;
            else
                PC += 2;
            break;
        case 0xA000:
            //Set register I to addr
            I = addr;
            PC += 2;
            break;
        case 0xB000:
            //Jump to address addr + V0
            PC = addr + V[0];
            break;
        case 0xC000:
            //Set VX to a rand num with mask of NN
            V[x] = (rand() % 256) & nn;
            PC += 2;
            break;
        case 0xD000:
            //draw a sprite at position VX, VY with N bytes of sprite data starting at the addr stored in I
            //if any set pixels are changed to unset, set VF to 01 else set VF to 00
            //draw_sprite(V[x], V[y], n)
            PC += 2;
            break;
        case 0xE000:
            switch(nn)
            {
                case 0x9E:
                    //Skip instruction if key VX is pressed
                    if (key[V[x]])
                        PC += 4;
                    else
                        PC += 2;
                    break;
                case 0xA1:
                    //Skip instruction if key VX is not pressed
                    if (!key[V[x]])
                        PC += 4;
                    else
                        PC += 2;
                    break;
            }
            break;
        case 0xF000:
            switch(nn)
            {
                case 0x07:
                    //store value of the delay timer in VX
                    V[x] = delay_timer;
                    PC += 2;
                    break;
                case 0x0A:
		{
                    //Wait for a keypress and store that key value at VX
                    uint8_t wait = TRUE;
                    uint8_t i;
                    while(wait)
                    {
                        for(i = 0; i < KEY_SIZE; i++)
                        {
                            if (key[i])
                            {
                                V[x] = i;
                                wait = FALSE;
                                break;
                            }
                        }
                    }
                    PC += 2;
                    break;
		}
                case 0x15:
                    //set delay timer to the value of VX
                    delay_timer = V[x];
                    PC += 2;
                    break;
                case 0x18:
                    //set sound timer to the value of VX
                    sound_timer = V[x];
                    PC += 2;
                    break;
                case 0x1E:
                    //Set register I to VX plus I
                    I += V[x];
                    PC += 2;
                    break;
                case 0x29:
                    //Set register I to the memory address of the sprite data corresponding to VX
                    I = V[x] * FONTSET_BYTES_PER_CHAR;
                    PC += 2;
                    break;
                case 0x33:
                    //Store the binary-coded decimal of VX at addresses I, I+1, and I+2
                    memory[I] = (V[x] % 1000) / 100;
                    memory[I + 1] = (V[x] % 100) / 10;
                    memory[I + 2] = (V[x] % 10);
                    PC += 2;
                    break;
                case 0x55:
                    for (uint8_t i = 0; i <= x; i++)
                    {
                        memory[I + i] = V[i];
                    }
                    I += x + 1;
                    PC += 2;
                    break;
                case 0x65:
                    for (uint8_t i = 0; i <= x; i++)
                    {
                        V[i] = memory[I + i];
                    }
                    I += x + 1;
                    PC += 2;
                    break;
                default:
                    //unknown opcode
		    break;
            }
            break;
        default:
            //unknown opcode
	    break;
    }
}
