// chip8.h
// This file includes the chip8 function declarations, variables, and constants

#ifndef CHIP8_H
#define CHIP8_H

#define MEM_SIZE 4096
#define REG_COUNT 16 
#define PROGRAM_START 0x200
#define PROGRAM_END   0x1000
#define MAX_PROGRAM_SIZE (PROGRAM_END - PROGRAM_START)
#define KEY_SIZE 16
#define STACK_SIZE 16
#define SCR_WIDTH 64
#define SCR_LENGTH 32
#define SCR_SIZE (SCR_WIDTH * SCR_LENGTH)
#define FONT_SIZE 80
#define WHITE 1
#define BLACK 0

#define TRUE 1
#define FALSE 0

void chip8_init();

int chip8_load(char* filename);

void chip8_cycle();

#endif
