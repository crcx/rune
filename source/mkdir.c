#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>


int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf("Usage: mkdir files...\n");
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if(mkdir(argv[i], S_IRWXU|S_IRWXG|S_IRWXO) < 0){
      printf("mkdir: %s failed to create\n", argv[i]);
      break;
    }
  }

  exit(0);
}
