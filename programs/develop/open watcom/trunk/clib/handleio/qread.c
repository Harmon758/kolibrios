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
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "qread.h"
#include <stdio.h>
#include "kolibri.h"

typedef struct 
{
  char     *name;
  unsigned int offset;
}__file_handle;

int __qread( int handle, void *buffer, unsigned len )
{
    __file_handle *fh;
    unsigned amount_read=0;
    
    __handle_check( handle, -1 );
    fh = (__file_handle*) __getOSHandle( handle );
      
    if(read_file(fh->name,buffer,fh->offset,len,&amount_read))
    {
      if ( amount_read == 0)
        return (-1);   

    }
    fh->offset+=amount_read;
    return( amount_read );
};



