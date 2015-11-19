#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
  int i;

  if(argc < 1){
    printf("usage: kill pid...\n");
    exit(0);
  }
  for(i=1; i<argc; i++)
    kill(atoi(argv[i]), SIGKILL);
  exit(0);
}
