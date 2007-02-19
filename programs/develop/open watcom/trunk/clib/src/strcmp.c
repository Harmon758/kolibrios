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
#include "widechar.h"
#include <stdio.h>
#include "riscstr.h"
#include "xstring.h"

#undef  strcmp


/* return <0 if s<t, 0 if s==t, >0 if s>t */

#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 _WCRTLINK int __simple_wcscmp( const CHAR_TYPE *s, const CHAR_TYPE *t )
#else
 _WCRTLINK int __F_NAME(strcmp,wcscmp)( const CHAR_TYPE *s, const CHAR_TYPE *t )
#endif
    {
#if defined(__INLINE_FUNCTIONS__) && !defined(__WIDECHAR__) && defined(_M_IX86)
        return( _inline_strcmp( s, t ) );
#else
        for( ; *s == *t; s++, t++ )
            if( *s == NULLCHAR )
                return( 0 );
        return( *s - *t );
#endif
    }
