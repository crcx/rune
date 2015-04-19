#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define MAX_BUFFER 1024

unsigned int int2str(char *dest, unsigned int i)
{
    register unsigned int len, tmp;
    for (len = 1, tmp = i; tmp > 9; ++len)
	tmp /= 10;
    if (dest)
	for (tmp = i, dest += len; tmp; tmp /= 10)
	    *--dest = (tmp % 10) + '0';
    return len;
}

int main(int argc, char *argv[])
{
    int fd = 0, read_count, i, total;
    char *buffer = (char *) alloca(MAX_BUFFER), *beg_buffer = buffer;
    char output[1024], tmp[256];
    unsigned int chr[256] = { 0 }, words = 0;

    argv++;
  main_loop:
//    if ((fd = open(*argv, O_RDONLY)) < 0)
    if ((fd = open(*argv, O_RDONLY, 0777)) < 0)
	return 1;

  read_loop:
    read_count = read(fd, buffer, MAX_BUFFER);
    for (i = 0; i < read_count; i++) {
	if ((*buffer) != ' ')
	    chr[*buffer]++;
	else {
	    if (*(buffer - 1) != ' ' && *(buffer + 1) != ' ')
		words++;
	    chr[*buffer]++;
	}
	*buffer++;
    }
    buffer = beg_buffer;
    if (read_count)
	goto read_loop;

    close(fd);

    total = 0;
    for (i = 0; i < 256; i++)
	total += chr[i];

    i = int2str(output, chr['\n']);
    write(STDOUT, output, i);
    write(STDOUT, "\t", 1);
    i = int2str(output, chr['\n'] + words - 1);
    write(STDOUT, output, i);
    write(STDOUT, "\t", 1);
    i = int2str(output, total);
    write(STDOUT, output, i);
    write(STDOUT, "\t", 1);
    write(STDOUT, *argv, strlen(*argv));
    write(STDOUT, "\n", 1);


    total = words = 0;
    for (i = 0; i < 256; i++)
	chr[i] = 0;

    argv++;
    if (--argc)
	goto main_loop;


    return 0;
}
