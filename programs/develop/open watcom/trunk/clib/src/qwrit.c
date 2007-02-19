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
#include <stdio.h>
#include <errno.h>

//#if defined(__NT__)
//#elif defined(__OS2__)
//#else
//    #include "tinyio.h"
//#endif

#include "iomode.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "rtdata.h"
#include "seterrno.h"
//#include "defwin.h"
#include "qwrite.h"

/*
    Use caution when setting the file pointer in a multithreaded
    application. You must synchronize access to shared resources. For
    example, an application whose threads share a file handle, update the
    file pointer, and read from the file must protect this sequence by
    using a critical section object or a mutex object.
 */


int __qwrite( int handle, const void *buffer, unsigned len )
{
    int             atomic;
#if defined(__NT__)
    DWORD           len_written;
    HANDLE          h;
    int             error;
    
#elif defined(__WARP__)
#elif defined(__OS2_286__)
#else
#endif
#if !defined(__NT__)
    tiny_ret_t      rc;
#endif

    __handle_check( handle, -1 );

#if defined(__NT__)
    h = __getOSHandle( handle );
#endif
    atomic = 0;
    if( __GetIOMode( handle ) & _APPEND )
    {
        _AccessFileH( handle );
        atomic = 1;
#if defined(__NT__)
//        if( SetFilePointer( h, 0, NULL, FILE_END ) == -1 )
//        {
//            error = GetLastError();
//            _ReleaseFileH( handle );
//            return( __set_errno_dos( error ) );
//        }
#elif defined(__OS2__)
#else
        rc = TinySeek( handle, 0L, SEEK_END );
#endif
#if !defined(__NT__)
        if( TINY_ERROR( rc ) ) {
            _ReleaseFileH( handle );
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
#endif
    }
#if defined(__NT__)

//    if( !WriteFile( h, buffer, len, &len_written, NULL ) )
//    {
//        error = GetLastError();
//        if( atomic == 1 ) {
//            _ReleaseFileH( handle );
//        }
//        return( __set_errno_dos( error ) );
//    }
    
#elif defined(__OS2__)
#elif defined(__WINDOWS_386__)
#else
    rc = TinyWrite( handle, buffer, len );
    len_written = TINY_LINFO( rc );
#endif

#if !defined(__NT__)
    if( TINY_ERROR( rc ) ) {
        if( atomic == 1 ) {
            _ReleaseFileH( handle );
        }
        return( __set_errno_dos( TINY_INFO( rc ) ) );
    }
#endif
    if( len_written != len ) {
        __set_errno( ENOSPC );
    }
    if( atomic == 1 )
    {
        _ReleaseFileH( handle );
    }
    return( len_written );
}
