/* Copyright (C) 2021 Logaev Maxim (turbocat2001), GPLv2 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "conio.h"
#include <sys/ksys.h>
#include <errno.h>
#include <limits.h>
//#include "format_print.h"

int vprintf ( const char * format, va_list arg )
{
  int len = 0;
  char *s = malloc(STDIO_MAX_MEM);
  if(!s){
    errno = ENOMEM;
    return errno;
  }
  __con_init();
  len = vsnprintf(s, STDIO_MAX_MEM, format, arg);
  __con_write_string(s, len);
  free(s);
  return(len);
}
