/**************************************************************
 * A Simple Text Editor
 *
 * Written in 2003 by Tom Novelli
 * Updates in 2009 by Charles Childers
 **************************************************************
 * This should work on most VT100 compatible terminals under
 * Unix-like OSes with only minor modifications. The keyboard
 * controls are similar to Pico; to customize them, modify the
 * edit() function.
 *
 * It's fine for programming and notetaking, but it gets slower
 * when you edit large files, so it is limited to 1 meg per
 * file. The reason is, each file gets loaded into one
 * contiguous block of memory; every time you insert or delete
 * a character, it has to move everything beyond the cursor.
 * That said, it's relatively efficient when compared with any
 * GUI environment.
 **************************************************************
 * DATE        REVISION NOTES
 * 08/26/2003  Split off from ste.c; removed wrapping code.
 *             Fixed up Insert/Del functions. Added Alt-N/P
 *             scroll keys.
 * 08/23/2009  Imported to git repo; start updates
 *             Add support for tabs
 *             Fixed a crash when creating new files
 **************************************************************
 * TO DO
 * [ ] tie together Up/Lnup, Down/Lndn
 * [ ] debug scrolling commands & row/col tracking
 * [x] Handle tabs
 * [ ] Autoindent mode
 * [ ] Tab->space mode, plus conversion routines from
 *     misc/cformat.c
 * [ ] Tab width setting
 * [ ] Multiple files, up to 10 (switch w/ Alt-0 thru Alt-9,
 *     Alt-P and Alt-N)
 * [ ] Blocks: Mark/Cut/Copy/Paste... ^^ ^K ^U  or  ^^ ^X ^C
 *     ^V  or mouse
 * [ ] Shift text left/right... Alt-< and Alt-> (or ESC-','
 *     and ESC-'.')
 * [ ] Help screen... F1 or ^G
 * [ ] Mouse (low priority but should be easy)
 * [ ] Bookmarks?
 * [ ] "Highlight invisible chars w/ color" mode (tab, space,
 *     control chars)
 * [ ] Search/Replace
 * [ ] Undo/Redo
 * [x] Fix crash when creating a new file
 * [ ] Fix crash when editing an empty file
 *************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include "term.h"
#include <sys/ioctl.h>
#include <ctype.h>

char filename[256];
FILE *f;

/* Edit buffer (1M max.) */
char buf[4096*256];

/****************************************** DRAWING ROUTINES */
int w, h;      /* Screen size                                */
int cx, cy;    /* Cursor position (on screen)                */
int sx;        /* Scroll position (in non-wrapping mode)     */
int row, col;  /* File coordinates                           */

/* Window height (screen height minus status line, etc.)     */
#define WH (h-1)

//
// Mode flags
//
/* Mode Flags                                                */
int overtype = 0;       /* 0: Insert  1: Overtype            */
int autoindent = 1;     /* Auto-indention                    */
int tabmode = 1;        /* 0: Tabs->Spaces  1: Literal Tabs  */
int modified = 0;       /* Is file dirty?                    */

char *msgbuf;

char *draw_modes()
{
  static char s[8];
  s[0] = overtype   ? 'O' : 'I';
  s[1] = autoindent ? 'A' : 32;
  s[2] = tabmode    ? 'T' : 'S';
  s[3] = 32;
  s[4] = 32;
  s[5] = modified   ? '*' : 32;
  s[6] = 32;
  s[7] = 0;
  return s;
}

//
// Temp variables for rendering.. draw(), etc.
//
int x, y;        // Current Position
char *p;         // Buffer ptr
char *lptr[100]; // Ptr to each line on screen


void spaces(int n)
{
  int i;
  for (i = 0; i < n; i++)
    putchar(32);
  x += n;
}

void cursor()
{
  xy(cx + 1, cy + 1);
}

void status()
{
  xy(1, h - 1);
  puts("\e[0;7m");
  if (msgbuf)
    spaces(w - 1 - printf(" %s", msgbuf));
  else
  {
    spaces(w - 30 - printf(" %s%s  ", draw_modes(), *filename ? filename : "(Unnamed)"));
    printf("Line %-4d Col %-3d %p ", 1 + row + cy, 1 + sx + cx, lptr[cy]);
  }
  puts("\e[A\e[0m");
  cursor();
}

/*
  char *p;   // Start of line
  int draw;  // 0: output off (just scan)  1: output on
*/
char *drawline(char *p, int draw)
{
//
// Draw 1 line & return ptr to next line
//
  int x, c, t;
  for (x = 0; *p; x++)
  {
    c = *p++;
    t = 0;

    if (c == 10)     // EOL?
      break;
    if (draw && x >= sx && x < sx + w)                  // Output
      putchar(c);
  }
  return p;
}

void draw(int flag)
{
//
// Redraw the entire screen
//
  int x, y, c, t;
  char *p = lptr[0];

  if (flag)
    cls();
  for (y = 0; *p && y < WH; y++)
  {
    lptr[y] = p;
    p = drawline(p,flag);
    if (flag)
      putchar(10);
  }
  if (*p)
    lptr[y++] = p;
  lptr[y] = 0;

  if (flag)
    status();
}

char *curpos()
{
//
// Return a pointer to the cursor's position in the file
//
  return lptr[cy] + sx + cx;
}

char *eol(char *p)
{
  while (*p && *p != 10)
    p++;
  return p;
}

void scrollup(int n)
{
//
// Scroll up N lines (max.)
//
  char *a, *b, *c;
  a = lptr[0] - 1;
  if (n <= 0 || a < buf)
    return;
  do
  {
    if (*--a == 10)
      n--, row--;
  } while (n > 0 && a >= buf);
  lptr[0] = ++a;
  if (a == buf)
    row--;
}

//************************************* PROMPTS

void msg(char *s)
{
  msgbuf = s;
}

void clearmsg()
{
  if (msgbuf)
    msgbuf=0, status();
}

void promptmsg(char *s)
{
  xy(1, h - 1);
  puts("\e[0;1m");
  spaces(w - printf("%s", s));
  puts("\e[A\e[0m");
  printf("\e[%dC", (int)strlen(s));
}

int ync(char *s)
{
  promptmsg(s);
  for (;;)
    switch (tolower(getkey()))
    {
      case 'y': clearmsg(); return 1;
      case 'n': clearmsg(); return -1;
      case 'c':
      case CTL('c'): clearmsg(); return 0;
    }
}

void prompt(char *msg, char *buf)
{
//
// Edit string in 'buf'... example: prompt("Filename to save as: ", filename);
//
}

//************************************* FILE COMMANDS

void dirty()
{
//
// Mark file as modified
//
  int tmp = modified;
  modified = 1;
  if (!tmp)
    status();
}

void load(char *file) {
  int c;
  char *p = buf;
  struct stat st;

  if (-1 == stat(file, &st)) {
    if (errno==ENOENT) {
      buf[0] = 32;
      buf[1] = 0;
      msg("(New File)");
      goto done;
    }
  }
  else if (!S_ISREG(st.st_mode)) {
    printf("'%s' is not a regular file\n", file);
    exit(1);
  }

  if (!(f = fopen(file, "r"))) {
    printf("Couldn't open file '%s'\n", file);
    exit(1);
  }
  c = fgetc(f);
  while (!feof(f)) {
    *p++ = c;
    c = fgetc(f);
  }
  fclose(f);
  msg(0);

done:
  strcpy(filename, file);
  cx = cy = 0;
  sx = 0;
  lptr[0] = buf;
  row = col = 0;
  modified = 0;
}

void Save() {
  int fd = open(filename, O_WRONLY | O_CREAT, 0644);
  if (!fd)
  {
    printf("Error saving file '%s'!\n", filename);
    getkey();
    return;
  }
  write(fd, buf, strlen(buf));
  close(fd);

  modified = 0;
  status();
}

void SaveAs()
{
// get filename; Save();
}

void Open()
{
// get filename and load() it
}

void Close()
{
// confirm; release window; exit if none left
}

void Read()
{
//
// Insert File
//
// similar to Paste()
}

//************************************* MOVEMENT

void Lnup()
{
  scrollup(1);
  cy++;
  draw(1);
}

void Lndn()
{
  int i;
  xy(1, 1);  printf("\e[M");  // delete top line
  xy(1, WH); printf("\e[L");  // insert line at bottom
  for (i = 0; i < WH; i++)    // update lptr[]
    lptr[i] = lptr[i + 1];
  lptr[WH] = drawline(lptr[WH - 1], 1);
  row++;
  cy--;
  status();
}

void Pgup()
{
  scrollup(WH);
  draw(1);
}

void Pgdn()
{
  int i;
  if (!lptr[0])
    return;
  for (i = 1; i <= WH; i++)
    if (!lptr[i])
      goto cur;
  lptr[0] = lptr[WH];
  row += WH;
  draw(1);
  for (i = 1; i < cy; i++)
  if (!lptr[i])
  {
    row -= cy - i + 1;
cur:
    cy = i - 1;
    status();
    break;
  }
}

void Top()
{
  lptr[0] = buf;
  cx = cy = sx = row = col = 0;
  draw(1);
}

void Bottom()
{
  char *p = lptr[0];
  for (p = buf, row = 0; *p; p++)
    if (*p == 10)
      row++;
  lptr[0] = p;
  Pgup();
}

void Home() {
  if (sx + cx == 0)
  {
    char *p;
    for (p = curpos(); isspace(*p); p++, cx++)
      ; // Scan to first non-space char.
    status();
  }
  else
  {
    sx = cx = 0;
    draw(1);
  }
}

void End()
{
  char *p;
  for (cx = 0, p = curpos(); !(*p == 10 || *p == 13); p++, cx++)
    ; // Scan to end of line
  if (cx<w)
  {
    status();
  }
  else
  {
    sx += cx - w + 1;
    cx = w - 1;
    draw(1);
  }
}

void Up()
{
  if (cy)
  {
    cy--;
    status();
    return;
  }
  scrollup(1);
  draw(1);
  //
  // TBD: for faster update do something like this
  //        lptr[WH] ... lptr[1]=lptr[0]
  //        & use VT codes to insert/delete lines
  //
}

void Down()
{
  int i;

  if (!lptr[cy + 1])
    return;   // check for EOF
  if (cy < WH - 1)
  {
    cy++;
    status();
    return;
  }
  xy(1, 1);  printf("\e[M");  // delete top line
  xy(1, WH); printf("\e[L");  // insert line at bottom
  for (i = 0; i < WH; i++)    // update lptr[]
    lptr[i] = lptr[i + 1];
  lptr[WH] = drawline(lptr[WH - 1], 1);
  row++;
  status();
}

void Right()
{
  /* No scroll */
  if (cx < w - 1)
  {
    cx++;
    status();
    return;
  }

  /* Scroll right */
  sx++;
  draw(1);
}

void Left()
{
  if (cy == 0 && lptr[0] == buf && cx <= 0)
    return;
  if (cx)
  {
    cx--;
    status();
    return;
  }
  if (sx)
  {
    sx--;
    draw(1);
    return;
  }
  Up();
  End();
}

void Wordright()
{
  char *p = curpos();
  do
  {
    Right();
    p++;
  } while(isspace(p[0]) || !isspace(p[-1]));
}

void Wordleft()
{
  char *p = curpos();
  do
  {
    Left();
    p--;
  } while(isspace(p[0]) || !isspace(p[-1]));
}

//************************************* EDITING COMMANDS

void Del()
{
//
// Delete char. under cursor
//
  int redraw = 0;
  char *p = curpos(), *e = eol(lptr[cy]);

  if (p > e)
    p = e;
  if (*p==10)
    redraw=1;
  else
  {
    printf("\e[P");
    if (e - p > w - cx)
    {
      printf("\e7"); xy(w, cy + 1);
      putchar(lptr[cy][w]);
      printf("\e8");
    }
  }
  memmove(p, p + 1, strlen(p));

  draw(redraw);
  dirty();
}

void Back()
{
//
// Delete char. left of cursor
//
  Left();
  Del();
}

void Insert(c)
{
  if (c == 13)
    c = 10;
  if ((c > 31 && c != 127 && c < 256) || c == 10 || c == 9)
  {
    char *p = curpos(), *e = eol(lptr[cy]);
    if (p > e)
    {
      End();
      p = e;
    }
    if (!overtype || *p == 10 || *p == 0)
      memmove(p + 1, p, strlen(p));
    *p = c;
    if (c == 10)
    {
      cy++, cx = 0, row++;
      dirty();
      draw(1);
      return;
    }  // insert line
    else
      cx++;
    if (!overtype)
      printf("\e[@");
    putchar(c);
    status();
  }
  dirty();
  draw(0);
}

/****************************************** KEYBOARD HANDLER */
void edit()
{
  int c;
  for (;;)
  {
    c = getkey();
    clearmsg();
    switch (c)
    {
      case CTL('x'):
      case CTL('c'):
      case CTL('q'):
        if (modified)
        {
          int tmp = ync("Save changes? (Yes/No/Cancel): ");
          if (tmp == 0)
          {
            status();
            break;
          }
          if (tmp == 1)
            Save();
        }
        exit(0);
      case CTL('s'):
        Save();
        break;
      case CTL('l'):
        draw(1);
        break;
      case K_HOME:
      case CTL('a'):
        Home();
        break;
      case K_END:
      case CTL('e'):
        End();
        break;
      case K_PGUP:
      case CTL('y'):
        Pgup();
        break;
      case K_PGDN:
      case CTL('v'):
        Pgdn();
        break;
      case ALT('y'):
        Top();
        break;
      case ALT('v'):
        Bottom();
        break;
      case K_BS:
      case CTL('H'):
        Back();
        break;
      case K_DEL:
      case CTL('d'):
        Del();
        break;
      case ALT('p'):
        Lnup();
        break;
      case ALT('n'):
        Lndn();
        break;
      case K_UP:
      case CTL('p'):
        Up();
        break;
      case K_DOWN:
      case CTL('n'):
        Down();
        break;
      case K_RIGHT:
      case CTL('f'):
        Right();
        break;
      case K_LEFT:
      case CTL('b'):
        Left();
        break;
      case CTL('@'):
        Wordleft();
        break;
      case CTL('_'):
        Wordright();
        break;
      case ALT('i'):
        overtype = !overtype;
        status();
        break;
      default:  // Insert a character (or overwrite)
        Insert(c);
    } /* end switch */
  }
}

//************************************* INITIALIZATION
void getscreensize()
{
  struct winsize win;
  int fd;

  fd = open(ttyname(0), O_RDWR);
  if (fd == -1)
    return;
  if (ioctl(fd, TIOCGWINSZ, &win) == -1)
    return;
  w = win.ws_col;
  h = win.ws_row;
}

//************************************* CLEANUP

void quit()
{
  xy(1, h); printf("\e[2K");  // put cursor at bottom
  term_cleanup();
}

//************************************* TOP LEVEL

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Simple Text Editor\nUsage: sten <filename>\n");
    exit(1);
  }
  load(argv[1]);

  getscreensize();
  term_init();
  atexit((void *)quit);
  signal(SIGSEGV, (void *)quit);
  signal(SIGILL, (void *)quit);
  signal(SIGFPE, (void *)quit);
  signal(SIGTERM, (void *)quit);
  signal(SIGHUP, (void *)quit);

  draw(1);
  edit();
}

//************************************* END
