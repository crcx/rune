#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

int size(char *fn);

int main(int argc, char *argv[])
{
    struct dirent **names;
    int n, i;
    if (argc < 2)
        argv[1] = ".";
    n = scandir(argv[1], &names, 0, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        for (i = 0; i < n; i++)
        {
            printf("%s\t(%d)\n", names[i]->d_name, size(names[i]->d_name));
            free(names[i]);
        }
        free(names);
    }
}


int size(char *fn)
{
    struct stat buf;
    stat(fn, &buf);
    return buf.st_size;
}
