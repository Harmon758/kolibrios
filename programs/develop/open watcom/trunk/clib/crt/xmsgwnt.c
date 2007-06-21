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
* Description:  Fatal runtime error handler for Win32.
*
****************************************************************************/


#include "variety.h"
#include "rtdata.h"
#include "exitwmsg.h"

void debug_out_str(const char* str);


_WCRTLINK void __exit_with_msg( char *msg, unsigned retcode )
{

    unsigned    len;
    char        *end;
 
    end = msg;
    for( len = 0; *end++ != '\0'; len++ )
        ;
    debug_out_str(msg);
    __exit( retcode );
}

_WCRTLINK void __fatal_runtime_error( char *msg, unsigned retcode )
{
    __exit_with_msg( msg, retcode );
    
}
