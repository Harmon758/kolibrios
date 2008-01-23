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
#include <mbstring.h>
#include "farfunc.h"



/****
***** Convert a multibyte character to a wide character.
****/

_WCRTLINK int _NEARFAR(mbtowc,_fmbtowc)( wchar_t _FFAR *pwc, const char _FFAR *ch, size_t n )
{

    /*** Catch special cases ***/
    if( ch == NULL )  return( 0 );
    if( n == 0 )  return( -1 );
    if( *ch == '\0' ) {
        if( pwc != NULL )  *pwc = L'\0';
        return( 0 );
    }
    if( _ismbblead( ch[0] )  &&  ch[1] == '\0' )  return( -1 ); /* invalid */

    /*** Convert the character ***/
        if( _ismbblead(*ch) && n>=2 ) {         /* lead byte present? */
            if( pwc != NULL ) {
                *pwc = (((wchar_t)ch[0])<<8) |  /* convert to lead:trail */
                        (wchar_t)ch[1];
            }
            return( 2 );                        /* return char size */
        } else if( !_ismbblead(*ch) ) {
            if( pwc != NULL ) {
                *pwc = (wchar_t)ch[0];          /* convert to 00:byte */
            }
            return( 1 );                        /* return char size */
        } else {
            return( -1 );                       /* n==1, but char 2 bytes */
        }
}
