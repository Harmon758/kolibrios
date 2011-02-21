/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <libc/file.h>

int
getw(FILE *f)
{
  int i;
  char *p;
  int w;

  p = (char *)&w;
  for (i=sizeof(int); --i>=0;)
    *p++ = getc(f);
  if (feof(f))
    return EOF;
  return w;
}
