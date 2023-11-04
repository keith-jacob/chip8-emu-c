A chip8 emulator written in C

Utilizes SDL to draw the screen

Running 'make' will compile the unit tests and main chip8 program.
Running 'make chip8' will compile just the main chip8 program.
Running 'make test' will compile just the unit tests.
Running 'make clean' will remove the chip8 and test executables.

To use:
Run './chip8 <pat/to/file.rom>'

To run the unit tests, run './test'

Tests are not complete and only test some of the functionality.
Tests are still needed for the chip8_cycle method.
The chip8 init tests need to be updated as well.
