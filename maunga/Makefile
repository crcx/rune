CURSES = -lncurses
CC = gcc
CFLAGS = -Wall -O3 -fomit-frame-pointer

default: retro
.PHONY: default

# Most files can be compiled using CFLAGS...
OFILES = devices.o disassemble.o endian.o loader.o maunga.o
$(OFILES) vm.o: %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ... and initial_image.o compiles much faster without -O3.
initial_image.o: initial_image.c
	$(CC) -Wall -c $< -o $@

# Both executables link the same way, but the dependencies differ.
retro: $(OFILES) initial_image.o vm.o
	$(CC) $(CURSES) $^ -o $@

clean:
	rm -f *.o
