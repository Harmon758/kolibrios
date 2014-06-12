/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <stdarg.h>
#include <libc/file.h>

int
vprintf(const char *fmt, va_list ap)
{
  int len;

  len = _doprnt(fmt, ap, stdout);
  return (ferror(stdout) ? EOF : len);
}
