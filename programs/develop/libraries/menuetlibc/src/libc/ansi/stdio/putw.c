/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <libc/file.h>

int
putw(int w, FILE *f)
{
  char *p;
  int i;

  p = (char *)&w;
  for (i=sizeof(int); --i>=0;)
    putc(*p++, f);
  return ferror(f);
}
