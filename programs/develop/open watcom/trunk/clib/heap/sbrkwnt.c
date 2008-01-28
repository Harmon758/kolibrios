/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <dos.h>
#include "kolibri.h"

void* user_alloc(unsigned size);

extern  unsigned                _curbrk;
extern  unsigned                _STACKTOP;

_WCRTLINK void _WCNEAR *sbrk( int increment )
{
  if( increment > 0 )
  {
    void* p;

    increment = ( increment + 0x0fff ) & ~0x0fff;
    //p = LocalAlloc( LMEM_FIXED, increment );
    //p = VirtualAlloc(NULL, increment, MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    p = user_alloc(increment);
    if( p != NULL ) return( p );
     errno = ENOMEM;
  }
  else
  {
    errno = EINVAL;
  }
  return( (void *) -1 );
}
