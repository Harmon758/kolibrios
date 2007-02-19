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
* Description:  Implementation of __set_errno().
*
****************************************************************************/


#ifdef __WATCOMC__

#include "variety.h"
#include <stdlib.h>
#include <errno.h>
#include "rtdata.h"
#include "seterrno.h"

_WCRTLINK void __set_errno( unsigned int err )
{
    _RWD_errno = err;
}

_WCRTLINK void __set_EDOM()
{
    __set_errno( EDOM );
}

_WCRTLINK void __set_ERANGE()
{
    __set_errno( ERANGE );
}

_WCRTLINK int __set_EINVAL()
{
    __set_errno( EINVAL );
    return( -1 );
}

//#if !defined(__UNIX__) && !defined(__NETWARE__)
//_WCRTLINK void __set_doserrno( unsigned int err )
//{
//    _RWD_doserrno = err;
//}
//#endif

#else

#include <errno.h>
void __set_errno( unsigned int err )
{
    errno = err;
}

#endif
