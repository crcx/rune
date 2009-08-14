#include <stdio.h>
#include "vm.h"
#include "functions.h"

void retro_init(VM *vm)
{
  retro_internal_init_vm(vm);
  retro_internal_dev_init(INPUT);
  retro_internal_dev_init(OUTPUT);
}

void retro_process(VM *vm)
{
  for (vm->ip = 0; vm->ip < IMAGE_SIZE; vm->ip++)
    retro_internal_vm_process(vm);
}

void retro_cleanup(VM *vm)
{
  retro_internal_dev_cleanup();
  free(vm);
}


