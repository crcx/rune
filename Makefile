default:
	@cd source && gcc cat.c -o ../bin/cat
	@cd source && gcc ls.c -o ../bin/ls
	@cd source && gcc te.c -o ../bin/te

clean:
	@rm -f bin/cat
	@rm -f bin/ls
	@rm -f bin/te

