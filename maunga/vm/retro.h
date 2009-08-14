#ifndef _RETRO_H
#define _RETRO_H

#define IMAGE_SIZE   5000000
#define STACK_DEPTH      100
#define ADDRESSES       1024

typedef struct {
  int sp;
  int rsp;
  int ip;
  int data[STACK_DEPTH];
  int address[ADDRESSES];
  int ports[1024];
  int image[IMAGE_SIZE];
  int padding;
  char filename[2048];
} RETRO;

void retro_init(RETRO *vm);
void retro_process(RETRO *vm);
void retro_cleanup(RETRO *vm);
int retro_load(RETRO *vm, char *image);
int retro_save(RETRO *vm, char *image);
void retro_include(char *filename);
#endif
