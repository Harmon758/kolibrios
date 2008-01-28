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
* Description:  Implementation of near _heapmin() and _nheapmin().
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include "heap.h"
#include "heapacc.h"
#if defined(__DOS_EXT__)
// #include "extender.h"
#endif
#if defined(__WINDOWS_286__) || defined(__NT__)
 #include "kolibri.h"
#endif
#if defined(__OS2__)
// #include <wos2.h>
#endif
#if defined(__WINDOWS_386__)
// extern int __pascal DPMIFree(unsigned long);   // windows extender function
#endif
#if defined(__CALL21__)
// #include "tinyio.h"
#endif

#if defined(__SMALL_DATA__)

_WCRTLINK int _heapshrink( void )
{
    return( _nheapshrink() );
}

_WCRTLINK int _heapmin( void )
{
    return( _nheapshrink() );
}

#endif

_WCRTLINK int _nheapmin( void )
{
    return( _nheapshrink() );
}

#if defined(__WARP__)        || \
    defined(__WINDOWS_286__) || \
    defined(__WINDOWS_386__) || \
    defined(__NT__)          || \
    defined(__CALL21__)

static void __ReleaseMiniHeap( mheapptr mhp )
{
    mheapptr pprev;
    mheapptr pnext;

    pprev = mhp->prev;
    pnext = mhp->next;
    if( user_free( mhp ) == 1 ) {
        if( pprev == NULL ) {
            __nheapbeg = pnext;
        } else {
            pprev->next = pnext;
        }
        if( pnext != NULL )  pnext->prev = pprev;
    } //else: do not unlink if the memory cannot be freed successfully
}

#endif

_WCRTLINK int _nheapshrink( void )
{
    mheapptr mhp;
#if !defined(__WARP__)        && \
    !defined(__WINDOWS_286__) && \
    !defined(__WINDOWS_386__) && \
    !defined(__NT__)          && \
    !defined(__CALL21__)
    // Shrink by adjusting _curbrk

    frlptr last_free;
    frlptr end_tag;
    unsigned new_brk;

    _AccessNHeap();
    #if defined(__DOS_EXT__)
    if( !_IsRationalZeroBase() && !_IsCodeBuilder() ) {
    #endif
        if( __nheapbeg == NULL ) {
            _ReleaseNHeap();
            return( 0 ); // No near heap, can't shrink
        }
        /* Goto the end of miniheaplist (if there's more than 1 blk) */
        for( mhp = __nheapbeg; mhp->next; mhp = mhp->next );
        /* check that last free block is at end of heap */
        last_free = mhp->freehead.prev;
        end_tag = (frlptr) ( (PTR)last_free + last_free->len );
        if( end_tag->len != END_TAG ) {
            _ReleaseNHeap();
            return( 0 );
        }
        if( end_tag != (frlptr) ((PTR)mhp + mhp->len ) ) {
            _ReleaseNHeap();
            return( 0 );
        }
        #if defined(__DOS_EXT__)
        // only shrink if we can shave off at least 4k
        if( last_free->len < 0x1000 ) {
            _ReleaseNHeap();
            return( 0 );
        }
        #else
        if( last_free->len <= sizeof( frl ) ) {
            _ReleaseNHeap();
            return( 0 );
        }
        #endif
        /* make sure there hasn't been an external change in _curbrk */
        if( sbrk( 0 ) != &(end_tag->prev) ) {
            _ReleaseNHeap();
            return( 0 );
        }
        /* calculate adjustment factor */
        if( mhp->len-last_free->len > sizeof( struct miniheapblkp ) ) {
            // this miniheapblk is still being used
            #if defined(__DOS_EXT__)
                frlptr new_last_free;
                new_last_free = (frlptr)((((unsigned)last_free + 0xfff) & ~0xfff) - TAG_SIZE);
                if( new_last_free == last_free ) {
            #endif
                    // remove entire entry
                    mhp->len -= last_free->len;
                    --mhp->numfree;
                    // Relink the freelist entries, and update the rover
                    mhp->freehead.prev = last_free->prev;
                    last_free->prev->next = &mhp->freehead;
                    if( mhp->rover == last_free ) mhp->rover = last_free->prev;
            #if defined(__DOS_EXT__)
                } else {
                    // just shrink the last free entry
                    mhp->len -= last_free->len;
                    last_free->len = (PTR)new_last_free - (PTR)last_free;
                    mhp->len += last_free->len;
                    last_free = new_last_free;
                }
            #endif
            last_free->len = END_TAG;
            new_brk = (unsigned) ((PTR)last_free + TAG_SIZE );
        } else {
            // we can remove this miniheapblk
            if( mhp->prev ) { // Not the first miniheapblk
                mhp->prev->next = NULL;
                new_brk = (unsigned)mhp;//->prev + (unsigned)mhp->prev->len;
            } else { // Is the first miniheapblk
                new_brk = (unsigned)__nheapbeg;
                __nheapbeg = NULL;
            }
            // Update rover info
            if( __MiniHeapRover == mhp ) {
                __MiniHeapRover = __nheapbeg;
                __LargestSizeB4MiniHeapRover = 0;
            }
        }

        if( __brk( new_brk ) == (void _WCNEAR *) -1 ) {
            _ReleaseNHeap();
            return( -1 );
        }
        _ReleaseNHeap();
        return( 0 );
    #if defined(__DOS_EXT__)
    }
    __FreeDPMIBlocks(); // For RSI/zero-base and Intel CB
    _ReleaseNHeap();
    return( 0 );
    #endif
#else
    // Shrink by releasing mini-heaps
    {
        mheapptr pnext;

        _AccessNHeap();
        for( mhp = __nheapbeg; mhp; mhp = pnext ) {
            pnext = mhp->next;
            if( mhp->len - sizeof(struct miniheapblkp) ==
                (mhp->freehead.prev)->len ) __ReleaseMiniHeap( mhp );
        }
        _ReleaseNHeap();
        return( 0 );
    }
#endif
}
