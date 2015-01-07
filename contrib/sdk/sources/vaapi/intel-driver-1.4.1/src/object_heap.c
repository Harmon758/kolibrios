/*
 * Copyright (c) 2007 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "object_heap.h"

#include "assert.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ASSERT	assert

#define LAST_FREE	-1
#define ALLOCATED	-2

/*
 * Expands the heap
 * Return 0 on success, -1 on error
 */
static int object_heap_expand( object_heap_p heap )
{
    int i;
    void *new_heap_index;
    int next_free;
    int new_heap_size = heap->heap_size + heap->heap_increment;
    int bucket_index = new_heap_size / heap->heap_increment - 1;

    if (bucket_index >= heap->num_buckets) {
        int new_num_buckets = heap->num_buckets + 8;
        void **new_bucket;

        new_bucket = realloc(heap->bucket, new_num_buckets * sizeof(void *));
        if (NULL == new_bucket) {
            return -1;
        }

        heap->num_buckets = new_num_buckets;
        heap->bucket = new_bucket;
    }

    new_heap_index = (void *) malloc( heap->heap_increment * heap->object_size );
    if ( NULL == new_heap_index )
    {
        return -1; /* Out of memory */
    }

    heap->bucket[bucket_index] = new_heap_index;
    next_free = heap->next_free;
    for(i = new_heap_size; i-- > heap->heap_size; )
    {
        object_base_p obj = (object_base_p) (new_heap_index + (i - heap->heap_size) * heap->object_size);
        obj->id = i + heap->id_offset;
        obj->next_free = next_free;
        next_free = i;
    }
    heap->next_free = next_free;
    heap->heap_size = new_heap_size;
    return 0; /* Success */
}

/*
 * Return 0 on success, -1 on error
 */
int object_heap_init( object_heap_p heap, int object_size, int id_offset)
{
    heap->object_size = object_size;
    heap->id_offset = id_offset & OBJECT_HEAP_OFFSET_MASK;
    heap->heap_size = 0;
    heap->heap_increment = 16;
    heap->next_free = LAST_FREE;
    heap->num_buckets = 0;
    heap->bucket = NULL;

    if (object_heap_expand(heap) == 0) {
        ASSERT(heap->heap_size);
        _i965InitMutex(&heap->mutex);
        return 0;
    } else {
        ASSERT(!heap->heap_size);
        ASSERT(!heap->bucket || !heap->bucket[0]);

        free(heap->bucket);

        return -1;
    }
}

/*
 * Allocates an object
 * Returns the object ID on success, returns -1 on error
 */
int object_heap_allocate( object_heap_p heap )
{
    object_base_p obj;
    int bucket_index, obj_index;

    _i965LockMutex(&heap->mutex);
    if ( LAST_FREE == heap->next_free )
    {
        if( -1 == object_heap_expand( heap ) )
        {
            _i965UnlockMutex(&heap->mutex);
            return -1; /* Out of memory */
        }
    }
    ASSERT( heap->next_free >= 0 );

    bucket_index = heap->next_free / heap->heap_increment;
    obj_index = heap->next_free % heap->heap_increment;

    obj = (object_base_p) (heap->bucket[bucket_index] + obj_index * heap->object_size);
    heap->next_free = obj->next_free;
    _i965UnlockMutex(&heap->mutex);
    
    obj->next_free = ALLOCATED;
    return obj->id;
}

/*
 * Lookup an object by object ID
 * Returns a pointer to the object on success, returns NULL on error
 */
object_base_p object_heap_lookup( object_heap_p heap, int id )
{
    object_base_p obj;
    int bucket_index, obj_index;

    _i965LockMutex(&heap->mutex);
    if ( (id < heap->id_offset) || (id > (heap->heap_size+heap->id_offset)) )
    {
        _i965UnlockMutex(&heap->mutex);
        return NULL;
    }
    id &= OBJECT_HEAP_ID_MASK;
    bucket_index = id / heap->heap_increment;
    obj_index = id % heap->heap_increment;
    obj = (object_base_p) (heap->bucket[bucket_index] + obj_index * heap->object_size);
    _i965UnlockMutex(&heap->mutex);

	/* Check if the object has in fact been allocated */
	if ( obj->next_free != ALLOCATED )
    {
        return NULL;
    }
    return obj;
}

/*
 * Iterate over all objects in the heap.
 * Returns a pointer to the first object on the heap, returns NULL if heap is empty.
 */
object_base_p object_heap_first( object_heap_p heap, object_heap_iterator *iter )
{
    *iter = -1;
    return object_heap_next( heap, iter );
}

/*
 * Iterate over all objects in the heap.
 * Returns a pointer to the next object on the heap, returns NULL if heap is empty.
 */
object_base_p object_heap_next( object_heap_p heap, object_heap_iterator *iter )
{
    object_base_p obj;
    int i = *iter + 1;
    int bucket_index, obj_index;

    _i965LockMutex(&heap->mutex);
    while ( i < heap->heap_size)
    {
        bucket_index = i / heap->heap_increment;
        obj_index = i % heap->heap_increment;

        obj = (object_base_p) (heap->bucket[bucket_index] + obj_index * heap->object_size);
        if (obj->next_free == ALLOCATED)
        {
            _i965UnlockMutex(&heap->mutex);
            *iter = i;
            return obj;
        }
        i++;
    }
    _i965UnlockMutex(&heap->mutex);
    *iter = i;
    return NULL;
}



/*
 * Frees an object
 */
void object_heap_free( object_heap_p heap, object_base_p obj )
{
    /* Don't complain about NULL pointers */
    if (NULL != obj)
    {
        /* Check if the object has in fact been allocated */
        ASSERT( obj->next_free == ALLOCATED );
    
        _i965LockMutex(&heap->mutex);
        obj->next_free = heap->next_free;
        heap->next_free = obj->id & OBJECT_HEAP_ID_MASK;
        _i965UnlockMutex(&heap->mutex);
    }
}

/*
 * Destroys a heap, the heap must be empty.
 */
void object_heap_destroy( object_heap_p heap )
{
    object_base_p obj;
    int i;
    int bucket_index, obj_index;

    if (heap->heap_size) {
        _i965DestroyMutex(&heap->mutex);

        /* Check if heap is empty */
        for (i = 0; i < heap->heap_size; i++)
        {
            /* Check if object is not still allocated */
            bucket_index = i / heap->heap_increment;
            obj_index = i % heap->heap_increment;
            obj = (object_base_p) (heap->bucket[bucket_index] + obj_index * heap->object_size);
            ASSERT( obj->next_free != ALLOCATED );
        }

        for (i = 0; i < heap->heap_size / heap->heap_increment; i++) {
            free(heap->bucket[i]);
        }

        free(heap->bucket);
    }

    heap->bucket = NULL;
    heap->heap_size = 0;
    heap->next_free = LAST_FREE;
}
