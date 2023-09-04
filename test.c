#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "chip8.h"

#define PASS 1
#define FAIL 0

extern uint8_t memory[MEM_SIZE];
extern uint8_t V[REG_COUNT];
extern uint8_t font[FONT_SIZE];

void print_results(char* s, int result);
void test_init();
void test_load();

int main()
{
    //run tests
    test_init();
    test_load();
    return 0;
}

void test_init()
{
    int success = PASS;
    chip8_init();

    for (int i = 0; i < MEM_SIZE; i++)
    {
        if (i < FONT_SIZE)
        {
            if (memory[i] != font[i])
            {
                success = FAIL;
                break;
            }
        }
        else
        {
            if (memory[i] != 0)
            {
                success = FAIL;
                break;
            }
        }
    }
    print_results("init_memory_test", success);

    success = PASS;
    for (int i = 0; i < REG_COUNT; i++)
    {
        if (V[i] != 0)
        {
            success = FAIL;
            break;
        }
    }
    print_results("init_regs_test", success);
}

void test_load()
{
    //test empty filename
    int success = PASS;
    if (chip8_load("") == 1)
    {
        success = FAIL;
    }
    print_results("load_empty_filename_test", success);

    //test existing filename
    success = PASS;
    if (chip8_load("roms/pong.rom") != 1)
    {
        success = FAIL;
    }
    else
    {
        FILE* game = fopen("roms/pong.rom", "r");
        uint8_t game_text[MAX_PROGRAM_SIZE];
        memset(game_text, 0, sizeof(uint8_t)*MAX_PROGRAM_SIZE);
        fread(&game_text, 1, MAX_PROGRAM_SIZE, game);
        fclose(game);
        for (int i = 0; i < MAX_PROGRAM_SIZE; i++)
        {
            if (memory[i + PROGRAM_START] != game_text[i])
            {
                success = FAIL;
                break;
            }
        }
    }
    print_results("load_filename_test", success);
}

void print_results(char* test, int result)
{
    printf("-----------------------\n");
    printf("%s: %s\n", test, result == PASS ? "PASS" : "FAIL");
    printf("-----------------------\n");
}
