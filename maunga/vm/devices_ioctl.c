/******************************************************
 * Ngaro
 *
 * Written by Charles Childers, released into the public
 * domain
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>

struct termios retro_internal_new_termios, retro_internal_old_termios;
FILE *retro_internal_input[8];
int retro_internal_isp=0;


/******************************************************
 * Display a character.
 *
 * Will clear the display if a negative value is passed
 ******************************************************/
void retro_internal_dev_putch(int c)
{
  if (c > 0)
  {
    putchar((char)c);
  }
  else
  {
    printf("\033[2J\033[1;1H");
  }
}


/******************************************************
 * Update the display
 ******************************************************/
void retro_internal_dev_refresh()
{
}



/******************************************************
 * Get input from an input source
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

  return c;
}


/******************************************************
 * Add a file to the input stack
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
 * Initialize real hardware devices
 ******************************************************/
void retro_internal_dev_init(int level)
{
  if (level == 2)
  {
    tcgetattr(0, &retro_internal_old_termios);
    retro_internal_new_termios = retro_internal_old_termios;
    retro_internal_new_termios.c_iflag &= ~(BRKINT+ISTRIP+IXON+IXOFF);
    retro_internal_new_termios.c_iflag |= (IGNBRK+IGNPAR);
    retro_internal_new_termios.c_lflag &= ~(ICANON+ISIG+IEXTEN+ECHO);
    retro_internal_new_termios.c_cc[VMIN] = 1;
    retro_internal_new_termios.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &retro_internal_new_termios);
  }
  if (level == 1)
  {
    retro_internal_isp = 0;
    retro_internal_input[retro_internal_isp] = stdin;
  }
}


/******************************************************
 * Restore real hardware device settings
 ******************************************************/
void retro_internal_dev_cleanup()
{
  tcsetattr(0, TCSANOW, &retro_internal_old_termios);
}
