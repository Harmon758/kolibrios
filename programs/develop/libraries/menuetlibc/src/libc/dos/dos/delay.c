/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <dos.h>
#include <menuet/os.h>

void delay(unsigned msec)
{
 __menuet__delay100(msec);
}
