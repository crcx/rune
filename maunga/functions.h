#ifndef _RETRO_H
#define _RETRO_H

#include "vm.h"

int retro_internal_dev_getch();
void retro_internal_dev_refresh();
void retro_internal_dev_include(char *s);
void retro_internal_dev_putch(int c);
void retro_internal_dev_init(int level);
void retro_internal_dev_cleanup();
void retro_internal_display_instruction();
void retro_internal_swapEndian(VM *vm);
void retro_internal_vm_process(VM *vm);
void retro_internal_init_devices(VM *vm);
void retro_internal_cleanup_devices(VM *vm);

/* Public Functions */
void retro_init(VM *vm);
void retro_process(VM *vm);
int retro_load(VM *vm, char *image);
int retro_save(VM *vm, char *image);

#endif
