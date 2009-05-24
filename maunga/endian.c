/******************************************************
 * Ngaro
 *
 * Written by Charles Childers, released into the public
 * domain
 ******************************************************/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "functions.h"
#include "vm.h"

extern VM_STATE vm;

#define BIGENDIAN     0
#define LITTLEENDIAN  1

/* This is a slightly tricky bit of code to convert an image from one endian to the other. */
/* It's only recieved minimal testing, so I hope it actually works... */
void swapEndian()
{
  int endian, x, a;
  short int word = 0x0001;
  char *byte = (char *) &word;
  endian = (byte[0] ? LITTLEENDIAN : BIGENDIAN);

  if (endian == LITTLEENDIAN)
    printf("Converting to big endian\n");
  if (endian == BIGENDIAN)
    printf("Converting to little endian\n");

  for(a = 0; a < IMAGE_SIZE; a++)
  {
    x = vm.image[a];
    vm.image[a] = (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);
  }
  printf("Done!\n");
}

