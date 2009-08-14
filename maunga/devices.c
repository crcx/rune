/******************************************************
 * Ngaro
 *
 * Written by Charles Childers, released into the public
 * domain
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <curses.h>

#include "functions.h"
#include "vm.h"

FILE *retro_internal_input[64];
int retro_internal_isp=0;


/******************************************************
 * Display a character.
 *
 * Will clear the display if a negative value is passed
 ******************************************************/
void retro_internal_dev_putch(int c)
{
  if (c >= 0)
  {
    addch((char)c);
  }
  else
  {
    clear();
  }
}


/******************************************************
 * Update the display
 ******************************************************/
void retro_internal_dev_refresh()
{
  refresh();
}


/******************************************************
 * Read a character from an input source
 ******************************************************/
int retro_internal_dev_getch()
{
  int c;

  if ((c = getc(retro_internal_input[retro_internal_isp])) == EOF && retro_internal_input[retro_internal_isp] != stdin)
  {
    fclose(retro_internal_input[retro_internal_isp]);
    retro_internal_isp--;
    return 0;
  }

  if (c == EOF && retro_internal_input[retro_internal_isp] == stdin)
  {
    exit(0);
  }

  if (retro_internal_input[retro_internal_isp] != stdin)
  {
    if (c == 10 || c == 13 || c == 9)
      c = 32;
  }

  if (c == 10)
    c = 0;

  return c;
}


/******************************************************
 * Add a file to the input source list
 ******************************************************/
void retro_include(char *s)
{
  FILE *file;

  file = fopen(s, "r");

  if (file)
  {
    retro_internal_isp++;
    retro_internal_input[retro_internal_isp] = file;
  }
}


/******************************************************
 * Prepare real I/O hardware for the emulator
 ******************************************************/
void retro_internal_dev_init(int level)
{
  if (level == OUTPUT)
  {
    initscr();                /* initialize the curses library */
    cbreak();                 /* take input chars one at a time, no wait for \n */
    scrollok(stdscr, TRUE);   /* Allow the display to scroll */
  }
  if (level == INPUT)
  {
    retro_internal_isp = 0;
    retro_internal_input[retro_internal_isp] = stdin;
  }
}


/******************************************************
 * Cleanup real I/O hardware settings
 ******************************************************/
void retro_internal_dev_cleanup()
{
  endwin();
}
