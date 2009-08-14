# .----,   .-----  -------  .----,    ,---,
# |     \  |          |     |     \  |     |
# |-----/  |---       |     |-----/  |     |
# |  \     |          |     |  \     |     |
# |   \    |          |     |   \    |     |
# |    \   `-----     |     |    \    `---'
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Written by Charles Childers
# This code is gifted to the public domain.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
default: static

static:
	gcc -O3 -fomit-frame-pointer -c devices.c
	gcc -O3 -fomit-frame-pointer -c endian.c
	gcc -O3 -fomit-frame-pointer -c loader.c
	gcc -O3 -fomit-frame-pointer -c retro.c
	gcc -O3 -fomit-frame-pointer -c vm.c
	ar rcs libretro.a devices.o endian.o loader.o retro.o vm.o
	@rm -f devices.o endian.o loader.o retro.o vm.o

clean:
	@rm -f *.o *.a
