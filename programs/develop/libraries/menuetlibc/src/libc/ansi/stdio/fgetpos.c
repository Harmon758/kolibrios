/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <errno.h>

int fgetpos(FILE *stream, fpos_t *pos)
{
 if (stream && pos)
 {
  *pos = (fpos_t)ftell(stream);
  return 0;
 }
 errno = EFAULT;
 return 1;
}
