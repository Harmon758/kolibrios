/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <libc/file.h>

int fputc(int c, FILE *fp)
{
 return __putc(c, fp);
}
