#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER 4096

int main(int argc, char *argv[])
{
    int file = 0;
    int j, i = 1, count = 0;
    char buffer[MAX_BUFFER + 1];
    
    if (!(--argc))
        goto loop;
    
main_cat_loop:
    if ((file = open(argv[i++], O_RDONLY)) < 0)
        return 1;
    
loop:
    for (j = 0; j < MAX_BUFFER; j++)
        buffer[j] = '\0';
    if ((count = read(file, buffer, MAX_BUFFER)))
    {
        printf("%s", buffer);
        goto loop;
    }
    
    close(file);
    
    if (i <= argc)
        goto main_cat_loop;
    
    return 0;
}
