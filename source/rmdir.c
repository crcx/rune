#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf("Usage: rmdir files...\n");
    exit(0);
  }

  for(i = 1; i < argc; i++){
    if(rmdir(argv[i]) < 0){
      printf("rmdir: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit(0);
}
