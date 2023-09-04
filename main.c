#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No filename given. Please specify a rom to load like so:\n ./chip8 <path/to/rom>\n");
        exit(EXIT_FAILURE);
    }

    chip8_init();

    chip8_load(argv[1]);

    return 0;
}
