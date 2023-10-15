A chip8 emulator written in C

Running 'make' will compile the unit tests and main chip8 program.
Running 'make chip8' will compile just the main chip8 program.
Running 'make test' will compile just the unit tests.
Running 'make clean' will remove the chip8 and test executables.

To use:
Run './chip8 <pat/to/file.rom>'

To run the unit tests, run './test'


chip8.c TODO:
1. Write tests for each OPCODE
2. Set up call stack
3. Write draw_sprite function

main.c TODO:
1. Setup screen
2. Setup draw functions
3. Handle keypresses
4. Call chip8_cycle
