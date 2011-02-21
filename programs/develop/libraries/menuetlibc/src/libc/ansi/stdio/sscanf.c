/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <stdio.h>
#include <stdarg.h>
#include <libc/file.h>
#include <libc/unconst.h>

int
sscanf(const char *str, const char *fmt, ...)
{
  int r;
  va_list a=0;
  FILE _strbuf;

  va_start(a, fmt);

  _strbuf._flag = _IOREAD|_IOSTRG;
  _strbuf._ptr = _strbuf._base = unconst(str, char *);
  _strbuf._cnt = 0;
  while (*str++)
    _strbuf._cnt++;
  _strbuf._bufsiz = _strbuf._cnt;
  r = _doscan(&_strbuf, fmt, a);
  va_end(a);
  return r;
}
