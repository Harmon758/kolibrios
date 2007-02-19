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
* Description:  Near heap expansion routines.
*
****************************************************************************/


//#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"
#if defined(__DOS_EXT__)
 #include "extender.h"
#endif

#if defined(__SMALL_DATA__)

_WCRTLINK void *_expand( void *stg, size_t amount )
{
    return( _nexpand( stg, amount ) );
}

#endif

#if defined(__AXP__) || defined(__PPC__)
    #define _SEGMENT int
#else
    #define _SEGMENT __segment
#endif

int __HeapManager_expand( _SEGMENT seg,
                          unsigned offset,
                          size_t req_size,
                          size_t *growth_size )
{
    #if defined(M_I86)
        typedef struct freelistp __based(seg) *fptr;
        typedef char __based(void) *cptr;

        struct miniheapblkp __based(seg) *hblk;
    #else
        typedef struct freelistp _WCNEAR *fptr;
        typedef char _WCNEAR *cptr;

        mheapptr hblk;
    #endif
    fptr        p1;
    fptr        p2;
    fptr        pnext;
    fptr        pprev;
    size_t      new_size;
    size_t      old_size;
    size_t      free_size;

    /* round (new_size + tag) to multiple of pointer size */
    new_size = (req_size + TAG_SIZE + ROUND_SIZE) & ~ROUND_SIZE;
    if( new_size < req_size ) new_size = ~0; //go for max
    if( new_size < FRL_SIZE ) {
        new_size = FRL_SIZE;
    }
    p1 = (fptr) ((cptr)offset - TAG_SIZE);
    old_size = p1->len & ~1;
    if( new_size > old_size ) {
        /* enlarging the current allocation */
        p2 = (fptr) ((cptr)p1 + old_size);
        *growth_size = new_size - old_size;
        for(;;) {
            free_size = p2->len;
            if( p2->len == END_TAG ) {
                return( __HM_TRYGROW );
            } else if( free_size & 1 ) { /* next piece is allocated */
                break;
            } else {
                pnext = p2->next;
                pprev = p2->prev;

                if( seg == _DGroup() ) { // near heap
                    for( hblk = __nheapbeg; hblk->next; hblk = hblk->next ) {
                        if( (fptr)hblk <= (fptr)offset &&
                            (fptr)((PTR)hblk+hblk->len) > (fptr)offset ) break;
                    }
                }
                #if defined(M_I86)
                    else {      // Based heap
                        hblk = 0;
                    }
                #endif

                if( hblk->rover == p2 ) { /* 09-feb-91 */
                    hblk->rover = p2->prev;
                }
                if( free_size < *growth_size  ||
                    free_size - *growth_size < FRL_SIZE ) {
                    /* unlink small free block */
                    pprev->next = pnext;
                    pnext->prev = pprev;
                    p1->len += free_size;
                    hblk->numfree--;
                    if( free_size >= *growth_size ) {
                        return( __HM_SUCCESS );
                    }
                    *growth_size -= free_size;
                    p2 = (fptr) ((cptr)p2 + free_size);
                } else {
                    p2 = (fptr) ((cptr)p2 + *growth_size);
                    p2->len = free_size - *growth_size;
                    p2->prev = pprev;
                    p2->next = pnext;
                    pprev->next = p2;
                    pnext->prev = p2;
                    p1->len += *growth_size;
                    return( __HM_SUCCESS );
                }
            }
        }
        /* no suitable free blocks behind, have to move block */
        return( __HM_FAIL );
    } else {
        /* shrinking the current allocation */
        if( old_size - new_size >= FRL_SIZE ) {
            /* block big enough to split */
            p1->len = new_size | 1;
            p1 = (fptr) ((cptr)p1 + new_size);
            p1->len = (old_size - new_size) | 1;
            if( seg == _DGroup() ) { // near heap
                for( hblk = __nheapbeg; hblk->next; hblk = hblk->next ) {
                    if( (fptr)hblk <= (fptr)offset &&
                        (fptr)((PTR)hblk+hblk->len) > (fptr)offset ) break;
                }
            }
            #if defined(M_I86)
                else    // Based heap
                    hblk = 0;
            #endif
            /* _bfree will decrement 'numalloc' 08-jul-91 */
            hblk->numalloc++;
            #if defined(M_I86)
                _bfree( seg, (cptr)p1 + TAG_SIZE );
                /* free the top portion */
            #else
                _nfree( (cptr)p1 + TAG_SIZE );
            #endif
        }
    }
    return( __HM_SUCCESS );
}


_WCRTLINK void _WCNEAR *_nexpand( void _WCNEAR *stg, size_t req_size )
    {
        struct {
            unsigned expanded : 1;
        } flags;
        int retval;
        size_t growth_size;

        flags.expanded = 0;
        _AccessNHeap();
        for( ;; ) {
            retval = __HeapManager_expand( _DGroup(),
                                           (unsigned) stg,
                                           req_size,
                                           &growth_size );
            if( retval == __HM_SUCCESS ) {
                _ReleaseNHeap();
                return( stg );
            }
            if( retval == __HM_FAIL || !__IsCtsNHeap() ) break;
            if( retval == __HM_TRYGROW ) {
                if( flags.expanded ) break;
                if( __ExpandDGROUP( growth_size ) == 0 ) {
                    break;
                }
                flags.expanded = 1;
            }
        }
        _ReleaseNHeap();
        return( NULL );
    }
