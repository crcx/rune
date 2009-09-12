default:
	@cd source && gcc cat.c -o ../bin/cat
	@cd source && gcc ls.c -o ../bin/ls
	@cd source && gcc sh.c -o ../bin/sh
	@cd source && gcc te.c -o ../bin/te
	@cd source && gcc wc.c -o ../bin/wc

clean:
	@rm -f bin/cat
	@rm -f bin/ls
	@rm -f bin/sh
	@rm -f bin/te
	@rm -f bin/wc
