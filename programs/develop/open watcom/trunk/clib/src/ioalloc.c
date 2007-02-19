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
* Description:  Platform independent __ioalloc() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include "liballoc.h"
#include <unistd.h>
#include "rtdata.h"
#include "streamio.h"


void __ioalloc( FILE *fp )
{
    __chktty( fp );                                 /* JBS 28-aug-90 */
    if( fp->_bufsize == 0 ) {
        if( fp->_flag & _IOLBF ) {
            fp->_bufsize = 134;
        } else if( fp->_flag & _IONBF ) {
            /* Use small but reasonably sized buffer; otherwise we will end
             * up calling into the OS for every character, completely killing
             * performance on unbuffered stream output through printf() etc.,
             * especially in extended DOS because of mode switches.
             */
            fp->_bufsize = 64;
        } else {
            fp->_bufsize = BUFSIZ;
        }
    }
    _FP_BASE(fp) = lib_malloc( fp->_bufsize );
    if( _FP_BASE(fp) == NULL ) {
        fp->_flag &= ~(_IONBF | _IOLBF | _IOFBF);
        fp->_flag |= _IONBF;        /* can't get big buffer */
        _FP_BASE(fp) = (char *)&(fp->_ungotten);
        fp->_bufsize = 1;
    } else {
        fp->_flag |= _BIGBUF;       /* got big buffer */
    }
    fp->_ptr = _FP_BASE(fp);
    fp->_cnt = 0;
}
