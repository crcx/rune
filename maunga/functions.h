#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

/* FILE: devices.c */
extern int dev_getch();
extern void dev_refresh();
extern void dev_include(char *s);
extern void dev_putch(int c);
extern void dev_init(int level);
extern void dev_cleanup();

/* FILE: disassemble.c */
extern void display_instruction();

/* FILE: endian.c */
extern void swapEndian();

/* FILE: initial_image.c */
extern void initial_image();

/* FILE: loader.c */
extern int vm_load_image(char *image);
extern int vm_save_image(char *image);

/* FILE: ngaro.c */
extern int main(int argc, char **argv);

/* FILE: video.c */
extern void drawpixel(int x, int color);
extern int update_display(void *unused);

/* FILE: vm.c */
extern void init_vm();
extern void vm_process();
extern void init_devices();
extern void cleanup_devices();

#endif
