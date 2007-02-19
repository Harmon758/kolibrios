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
* Description:  Implementation of near malloc() and _nmalloc().
*
****************************************************************************/


//#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "extfunc.h"
#include "heapacc.h"
#include "heap.h"

#if defined(_M_IX86)
    #pragma aux (__outside_CLIB) __nmemneed;
#endif

mheapptr _WCNEAR __nheapbeg = NULL;

struct miniheapblkp _WCNEAR *__MiniHeapRover = NULL;
unsigned int   __LargestSizeB4MiniHeapRover = 0;



#if defined(__SMALL_DATA__)

_WCRTLINK void *malloc( size_t amount )
{
    return( _nmalloc( amount ) );
}

#endif


_WCRTLINK void _WCNEAR *_nmalloc( size_t amt )
{
    unsigned        largest;
    unsigned        size;
    unsigned        ptr;
    unsigned char   expanded;
    mheapptr        miniheap_ptr;

    if( (amt == 0) || (amt > -sizeof(struct heapblk)) ) {
        return( (void _WCNEAR *)NULL );
    }

    // Try to determine which miniheap to begin allocating from.
    // first, round up the amount
    size = (amt + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( size < FRL_SIZE ) {
        size = FRL_SIZE;
    }

    _AccessNHeap();
    ptr = NULL;
    expanded = 0;
    for(;;) {
        if( size > __LargestSizeB4MiniHeapRover ) {
            miniheap_ptr = __MiniHeapRover;
            if( miniheap_ptr == NULL ) {
                __LargestSizeB4MiniHeapRover = 0;
                miniheap_ptr = __nheapbeg;
            }
        } else {
            __LargestSizeB4MiniHeapRover = 0;   // force value to be updated
            miniheap_ptr = __nheapbeg;
        }
        for(;;) {
            if( miniheap_ptr == NULL ) {
                break;
            }
            __MiniHeapRover = miniheap_ptr;
            largest = miniheap_ptr->largest_blk;
            if( largest >= amt ) {
                ptr = __MemAllocator( amt, _DGroup(), (unsigned)miniheap_ptr );
                if( ptr != NULL ) {
                    goto lbl_release_heap;
                }
            }
            if( largest > __LargestSizeB4MiniHeapRover ) {
                __LargestSizeB4MiniHeapRover = largest;
            }
            miniheap_ptr = miniheap_ptr->next;
        }
        if( expanded || !__ExpandDGROUP( amt ) ) {
            if( !__nmemneed( amt ) ) {
                break;
            }
            expanded = 0;
        } else {
            expanded = 1;
        }
    }
lbl_release_heap:
    _ReleaseNHeap();
    return( (void _WCNEAR *)ptr );
}
