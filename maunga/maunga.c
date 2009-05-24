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

#include "functions.h"
#include "vm.h"

extern VM_STATE vm;


/******************************************************
 *|F| int main(int argc, char **argv)
 ******************************************************/
int main(int argc, char **argv)
{
  int a, i, trace, endian;

  trace = 0;
  endian = 0;

  strcpy(vm.filename, "retroImage");

  init_vm();
  dev_init(INPUT);

  /* Parse the command line arguments */
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--trace") == 0)
    {
      trace = 1;
    }
    else if (strcmp(argv[i], "--endian") == 0)
    {
      endian = 1;
    }
    else if (strcmp(argv[i], "--with") == 0)
    {
      i++; dev_include(argv[i]);
    }
    else if (strcmp(argv[i], "--help") == 0)
    {
      fprintf(stderr, "%s [options] [imagename]\n\r", argv[0]);
      fprintf(stderr, "Valid options are:\n\r");
      fprintf(stderr, "   --about        Display some information about Ngaro\n\r");
      fprintf(stderr, "   --trace        Execution trace\n\r");
      fprintf(stderr, "   --endian       Load an image with a different endianness\n\r");
      fprintf(stderr, "   --with [file]  Treat [file] as an input source\n\r");
      exit(0);
    }
    else if (strcmp(argv[i], "--about") == 0)
    {
      fprintf(stderr, "Ngaro, a simple virtual machine\n\r");
      fprintf(stderr, "Built on %s\n\r", __DATE__);
      fprintf(stderr, "Ngaro is released into the public domain. Use it as you see fit.\n\r");
      exit(0);
    }
    else
    {
      strcpy(vm.filename, argv[i]);
    }
  }

  dev_init(OUTPUT);

  /* Load the image */
  a = vm_load_image(vm.filename);

  if (a == -1)
    initial_image();

  /* Swap endian if --endian was passed */
  if (endian == 1)
    swapEndian();

  /* Process the image */
  if (trace == 0)
  {
    for (vm.ip = 0; vm.ip < IMAGE_SIZE; vm.ip++)
    {
      vm_process(vm.image[vm.ip]);
    }
  }
  else
  {
    for (vm.ip = 0; vm.ip < IMAGE_SIZE; vm.ip++)
    {
      display_instruction();
      vm_process(vm.image[vm.ip]);
    }
  }

  /* Once done, cleanup */
  dev_cleanup();
  return 0;
}
