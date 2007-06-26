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
* Description:  Implementation of sscanf_s() - safe formatted string input.
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include "widechar.h"
#include <stdio.h>
#include <stdarg.h>
#include <wchar.h>
#include "scanf.h"


static int cget_string( PTR_SCNF_SPECS specs )
{
    int     c;

    if( (c = *(specs->ptr)) != NULLCHAR ) {
        ++(specs->ptr);
    } else {
        c = EOF;
        specs->eoinp = 1;
    }
    return( c );
}


static void uncget_string( int c, PTR_SCNF_SPECS specs )
{
    --specs->ptr;
}



_WCRTLINK int __F_NAME(sscanf_s,swscanf_s)( const CHAR_TYPE *s, const CHAR_TYPE *format, ... )
{
    SCNF_SPECS      specs;
    const char      *msg;
    int             rc;
    va_list         args;

    /* Basic check for null pointers to see if we can continue */
    if( __check_constraint_nullptr_msg( msg, s )
     && __check_constraint_nullptr_msg( msg, format ) ) {

        specs.ptr        = (CHAR_TYPE *)s;
        specs.cget_rtn   = cget_string;
        specs.uncget_rtn = uncget_string;
        msg = NULL;
        va_start( args, format );
        rc = __F_NAME(__scnf_s,__wscnf_s)( (PTR_SCNF_SPECS)&specs, format, &msg, args );
        va_end( args );
        if( msg == NULL ) {
            /* no rt-constraint violation inside worker routine */
            return( rc );
        }
    }
    __rtct_fail( __func__, msg, NULL );
    return( __F_NAME(EOF,WEOF) );
}
