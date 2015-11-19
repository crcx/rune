#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
  int i;

  for(i = 1; i < argc; i++)
    printf("%s%s", argv[i], i+1 < argc ? " " : "\n");
  exit(0);
}
