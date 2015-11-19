CC = clang
FLAGS = -s -O3

default:
	@cd source && $(CC) $(FLAGS) cat.c -o ../bin/cat
	@cd source && $(CC) $(FLAGS) ls.c -o ../bin/ls
	@cd source && $(CC) $(FLAGS) sh.c -o ../bin/sh
	@cd source && $(CC) $(FLAGS) te.c -o ../bin/te
	@cd source && $(CC) $(FLAGS) wc.c -o ../bin/wc

clean:
	@rm -f bin/cat
	@rm -f bin/ls
	@rm -f bin/sh
	@rm -f bin/te
	@rm -f bin/wc
