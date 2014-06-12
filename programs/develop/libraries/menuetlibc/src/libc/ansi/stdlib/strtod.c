/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
#include <math.h>
#include <stdlib.h>
#include <libc/unconst.h>

double
strtod(const char *s, char **sret)
{
  long double r;		/* result */
  int e;			/* exponent */
  long double d;		/* scale */
  int sign;			/* +- 1.0 */
  int esign;
  int i;
  int flags=0;

  r = 0.0;
  sign = 1.0;
  e = 0;
  esign = 1;

  while ((*s == ' ') || (*s == '\t'))
    s++;

  if (*s == '+')
    s++;
  else if (*s == '-')
  {
    sign = -1;
    s++;
  }

  while ((*s >= '0') && (*s <= '9'))
  {
    flags |= 1;
    r *= 10.0;
    r += *s - '0';
    s++;
  }

  if (*s == '.')
  {
    d = 0.1L;
    s++;
    while ((*s >= '0') && (*s <= '9'))
    {
      flags |= 2;
      r += d * (*s - '0');
      s++;
      d *= 0.1L;
    }
  }

  if (flags == 0)
  {
    if (sret)
      *sret = unconst(s, char *);
    return 0;
  }

  if ((*s == 'e') || (*s == 'E'))
  {
    s++;
    if (*s == '+')
      s++;
    else if (*s == '-')
    {
      s++;
      esign = -1;
    }
    if ((*s < '0') || (*s > '9'))
    {
      if (sret)
	*sret = unconst(s, char *);
      return r;
    }

    while ((*s >= '0') && (*s <= '9'))
    {
      e *= 10;
      e += *s - '0';
      s++;
    }
  }

  if (esign < 0)
    for (i = 1; i <= e; i++)
      r *= 0.1L;
  else
    for (i = 1; i <= e; i++)
      r *= 10.0;

  if (sret)
    *sret = unconst(s, char *);
  return r * sign;
}
