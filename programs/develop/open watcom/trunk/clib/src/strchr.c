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
* Description:  Implementation of strchr() and wcschr().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <string.h>
#include "riscstr.h"

#if defined(M_I86) && !defined(__WIDECHAR__)

extern  char * _scan1();

/* use scan1 to find the char we are looking for */

#if defined(__SMALL_DATA__)
#pragma aux    _scan1 = 0xad            /* L1:lodsw  */\
                        0x38 0xd0       /* cmp al,dl */\
                        0x74 0x20       /* je L3     */\
                        0x84 0xc0       /* test al,al*/\
                        0x74 0x19       /* je L2     */\
                        0x38 0xd4       /* cmp ah,dl */\
                        0x74 0x19       /* je L4     */\
                        0x84 0xe4       /* test ah,ah*/\
                        0x74 0x11       /* je L2     */\
                        0xad            /* lodsw     */\
                        0x38 0xd0       /* cmp al,dl */\
                        0x74 0x0f       /* je L3     */\
                        0x84 0xc0       /* test al,al*/\
                        0x74 0x08       /* je L2     */\
                        0x38 0xd4       /* cmp ah,dl */\
                        0x74 0x08       /* je L4     */\
                        0x84 0xe4       /* test ah,ah*/\
                        0x75 0xde       /* jne L1    */\
                        0x31 0xf6       /* L2:xor si,si */\
                        0xa9            /* test ax,... */\
                        0x4e            /* L3:dec si */\
                        0x4e            /* L4:dec si */\
                        parm caller [si] [dl]\
                        value [si]\
                        modify [ax si];
#else
#pragma aux    _scan1 = \
                        0x1e            /* push ds */ \
                        0x8e 0xd9       /* mov ds,cx */ \
                        0x8c 0xda       /* mov dx,ds */\
                        0xad            /* L1:lodsw  */\
                        0x38 0xd8       /* cmp al,bl */\
                        0x74 0x22       /* je L3     */\
                        0x84 0xc0       /* test al,al*/\
                        0x74 0x19       /* je L2     */\
                        0x38 0xdc       /* cmp ah,bl */\
                        0x74 0x1b       /* je L4     */\
                        0x84 0xe4       /* test ah,ah*/\
                        0x74 0x11       /* je L2     */\
                        0xad            /* lodsw     */\
                        0x38 0xd8       /* cmp al,bl */\
                        0x74 0x11       /* je L3     */\
                        0x84 0xc0       /* test al,al*/\
                        0x74 0x08       /* je L2     */\
                        0x38 0xdc       /* cmp ah,bl */\
                        0x74 0x0a       /* je L4     */\
                        0x84 0xe4       /* test ah,ah*/\
                        0x75 0xde       /* jne L1    */\
                        0x31 0xf6       /* L2:xor si,si*/\
                        0x89 0xf2       /* mov dx,si */\
                        0xa9            /* test ax,... */\
                        0x4e            /* L3:dec si */\
                        0x4e            /* L4:dec si */\
                        0x1f            /* pop ds */ \
                        parm caller [cx si] [bl]\
                        value [dx si]\
                        modify [ax dx si];
#endif
#endif


/* locate the first occurrence of c in the initial n characters of the
   string pointed to by s. The terminating null character is considered
   to be part of the string.
   If the character c is not found, NULL is returned.
*/
#undef  strchr


#if defined(__RISCSTR__) && defined(__WIDECHAR__)
 _WCRTLINK CHAR_TYPE *__simple_wcschr( const CHAR_TYPE *s, INTCHAR_TYPE c )
#else
 _WCRTLINK CHAR_TYPE *__F_NAME(strchr,wcschr)( const CHAR_TYPE *s, INTCHAR_TYPE c )
#endif
    {
        CHAR_TYPE cc = c;
        do {
            if( *s == cc ) return( (CHAR_TYPE *)s );
        } while( *s++ != NULLCHAR );
        return( NULL );
    }
