#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "chip8.h"

#include <SDL2/SDL.h>

extern uint8_t key[KEY_SIZE];
extern uint8_t pixels[SCR_WIDTH][SCR_LENGTH];
extern uint8_t drawFlag;

#define PIXEL_SIZE 10
#define SDL_WIDTH SCR_WIDTH * PIXEL_SIZE
#define SDL_LENGTH SCR_LENGTH * PIXEL_SIZE

uint8_t keymap[KEY_SIZE] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v
};

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No filename given. Please specify a rom to load like so:\n ./chip8 <path/to/rom>\n");
        exit(EXIT_FAILURE);
    }

    chip8_init();

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    {
        printf("SDL failed to initialize. SDL_ERROR: %s\n", SDL_GetError());
        exit(2);
    }

    SDL_Window* window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SDL_WIDTH, SDL_LENGTH, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("SDL failed to create window. SDL_ERROR: %s\n", SDL_GetError());
        exit(2);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        printf("SDL failed to create renderer. SDL_ERROR: %s\n", SDL_GetError());
        exit(2);
    }
    if (SDL_RenderSetLogicalSize(renderer, SDL_WIDTH, SDL_LENGTH) < 0)
    {
        printf("SDL failed set resolution for rendering. SDL_ERROR: %s\n", SDL_GetError());
        exit(2);
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCR_WIDTH, SCR_LENGTH);
    if (texture == NULL)
    {
        printf("SDL failed to create texture. SDL_ERROR: %s\n", SDL_GetError());
        exit(2);
    }

    chip8_load(argv[1]);

    uint32_t texture_map[SCR_SIZE];

    while(TRUE)
    {
        chip8_cycle();

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                exit(0);
            }

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }

                if (e.key.keysym.sym == SDLK_SPACE)
                {
                    chip8_init();
                    chip8_load(argv[1]);
                }

                for (int iter = 0; iter < KEY_SIZE; iter++)
                {
                    if (e.key.keysym.sym == keymap[iter])
                    {
                        key[iter] = 1;
                    }
                }
            }
            if (e.type == SDL_KEYUP)
            {
                for (int iter = 0; iter < KEY_SIZE; iter++)
                {
                    if (e.key.keysym.sym == keymap[iter])
                    {
                        key[iter] = 0;
                    }
                }
            }
        }

        if (drawFlag)
        {
            drawFlag = FALSE;
            for (int l_iter = 0; l_iter < SCR_LENGTH; ++l_iter)
            {
                for (int w_iter = 0; w_iter < SCR_WIDTH; ++w_iter)
                {
                    texture_map[w_iter + l_iter * 64] = (pixels[w_iter][l_iter] * 0x00FFFFFF) | 0xFF000000;
                }
            }

            SDL_UpdateTexture(texture, NULL, texture_map, SCR_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        usleep(1200);
    }

    return 0;
}
