CC = gcc
CCFLAGS += -Wall -Wextra -g

all: chip8 test

chip8: chip8.c main.c
	$(CC) $(CCFLAGS) -o $@ $^

test: chip8.c test.c 
	$(CC) $(CCFLAGS) -o $@ $^

clean:
	-@rm chip8 test 2>/dev/null || true
