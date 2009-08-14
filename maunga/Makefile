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
default: static maunga

maunga:
	gcc -O3 -fomit-frame-pointer maunga.c libretro.a -lncurses -o maunga

static:
	cd vm && gcc -O3 -fomit-frame-pointer -c devices.c
	cd vm && gcc -O3 -fomit-frame-pointer -c endian.c
	cd vm && gcc -O3 -fomit-frame-pointer -c loader.c
	cd vm && gcc -O3 -fomit-frame-pointer -c retro.c
	cd vm && gcc -O3 -fomit-frame-pointer -c vm.c
	cd vm && ar rcs ../libretro.a devices.o endian.o loader.o retro.o vm.o
	cd vm && rm -f devices.o endian.o loader.o retro.o vm.o

clean:
	rm -f vm/*.o *.a maunga
