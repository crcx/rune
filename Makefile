CC = clang
FLAGS = -s -O3

default:
	@cd source && $(CC) $(FLAGS) cat.c -o ../bin/cat
	@cd source && $(CC) $(FLAGS) echo.c -o ../bin/echo
	@cd source && $(CC) $(FLAGS) grep.c -o ../bin/grep
	@cd source && $(CC) $(FLAGS) kill.c -o ../bin/kill
	@cd source && $(CC) $(FLAGS) ls.c -o ../bin/ls
	@cd source && $(CC) $(FLAGS) mkdir.c -o ../bin/mkdir
	@cd source && $(CC) $(FLAGS) rm.c -o ../bin/rm
	@cd source && $(CC) $(FLAGS) rmdir.c -o ../bin/rmdir
	@cd source && $(CC) $(FLAGS) sh.c -o ../bin/sh
	@cd source && $(CC) $(FLAGS) te.c -o ../bin/te
	@cd source && $(CC) $(FLAGS) wc.c -o ../bin/wc

clean:
	rm -f bin/*
