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
#define _INITRANDNEXT(p)
#if defined(__OS2__) || defined(__NT__) || defined(__NETWARE__)
    // OS/2, NT and NETWARE versions are identical
    // note that NETWARE is always multi-threaded
    #if defined(__SW_BM)

        #include "thread.h"
        #if 0
            // who ever allocates the thread_data should initialize __randnext
            if( __THREADDATAPTR->__randnextinit == 0 ) { \
                __THREADDATAPTR->__randnextinit = 1; \
                __THREADDATAPTR->__randnext = 1; \
            }
        #endif
        #define _RANDNEXT       (__THREADDATAPTR->__randnext)

    #else

        static  unsigned long int next = 1;
        #define _RANDNEXT       next

    #endif
#else
    static  unsigned long int next = 1;
    #define _RANDNEXT next
#endif
