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
* Description:  Implementation of memcpy() and wmemcpy().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <string.h>
#include <wchar.h>
#include "xstring.h"


_WCRTLINK VOID_WC_TYPE *__F_NAME(memcpy,wmemcpy)( VOID_WC_TYPE *in_dst, const VOID_WC_TYPE *in_src, size_t len )
{
#if defined(__INLINE_FUNCTIONS__) && !defined(__WIDECHAR__) && defined(_M_IX86)
    return( _inline_memcpy( in_dst, in_src, len ) );
#else
    CHAR_TYPE           *dst = in_dst;
    const CHAR_TYPE     *src = in_src;

    for( ; len; --len ) {
        *dst++ = *src++;
    }
    return( in_dst );
#endif
}
