/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <libc/stubs.h>
#include <stdio.h>
#include <unistd.h>
#include <libc/file.h>

void rewind(FILE *f)
{
  fflush(f);
  lseek(fileno(f), 0L, SEEK_SET);
  f->_cnt = 0;
  f->_ptr = f->_base;
  f->_flag &= ~(_IOERR|_IOEOF);
  if (f->_flag & _IORW)
    f->_flag &= ~(_IOREAD|_IOWRT);
}
