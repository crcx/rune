/******************************************************
 * Ngaro
 *
 * Written by Charles Childers, released into the public
 * domain
 ******************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vm/retro.h"


/******************************************************
 * Main entry point into the VM
 ******************************************************/
int main(int argc, char **argv)
{
  int a, i, trace, endian, minimal;

  RETRO *VM = malloc(sizeof(RETRO));

  retro_init(VM);
  retro_load(VM, "retroImage");
  retro_process(VM);
  retro_cleanup(VM);
  return 0;
}
