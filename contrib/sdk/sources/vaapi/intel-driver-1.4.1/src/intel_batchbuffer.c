/**************************************************************************                                                                                  
 *                                                                                                                                                           
 * Copyright 2006 Tungsten Graphics, Inc., Cedar Park, Texas.                                                                                                
 * All Rights Reserved.                                                                                                                                      
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
 * IN NO EVENT SHALL TUNGSTEN GRAPHICS AND/OR ITS SUPPLIERS BE LIABLE FOR                                                                                    
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,                                                                                  
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE                                                                                         
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                                                                    
 *                                                                                                                                                           
 **************************************************************************/      

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "intel_batchbuffer.h"

#define MAX_BATCH_SIZE		0x400000

static void 
intel_batchbuffer_reset(struct intel_batchbuffer *batch, int buffer_size)
{
    struct intel_driver_data *intel = batch->intel; 
    int batch_size = buffer_size;

    assert(batch->flag == I915_EXEC_RENDER ||
           batch->flag == I915_EXEC_BLT ||
           batch->flag == I915_EXEC_BSD ||
           batch->flag == I915_EXEC_VEBOX);

    dri_bo_unreference(batch->buffer);
    batch->buffer = dri_bo_alloc(intel->bufmgr, 
                                 "batch buffer",
                                 batch_size,
                                 0x1000);
    assert(batch->buffer);
    dri_bo_map(batch->buffer, 1);
    assert(batch->buffer->virtual);
    batch->map = batch->buffer->virtual;
    batch->size = batch_size;
    batch->ptr = batch->map;
    batch->atomic = 0;
}

static unsigned int
intel_batchbuffer_space(struct intel_batchbuffer *batch)
{
    return (batch->size - BATCH_RESERVED) - (batch->ptr - batch->map);
}


struct intel_batchbuffer * 
intel_batchbuffer_new(struct intel_driver_data *intel, int flag, int buffer_size)
{
    struct intel_batchbuffer *batch = calloc(1, sizeof(*batch));
    assert(flag == I915_EXEC_RENDER ||
           flag == I915_EXEC_BSD ||
           flag == I915_EXEC_BLT ||
           flag == I915_EXEC_VEBOX);

   if (!buffer_size || buffer_size < BATCH_SIZE) {
	buffer_size = BATCH_SIZE;
   }

   /* the buffer size can't exceed 4M */
   if (buffer_size > MAX_BATCH_SIZE) {
	buffer_size = MAX_BATCH_SIZE;
   }

    batch->intel = intel;
    batch->flag = flag;
    batch->run = drm_intel_bo_mrb_exec;

    if (IS_GEN6(intel->device_info) &&
        flag == I915_EXEC_RENDER)
        batch->wa_render_bo = dri_bo_alloc(intel->bufmgr,
                                           "wa scratch",
                                           4096,
                                           4096);
    else
        batch->wa_render_bo = NULL;

    intel_batchbuffer_reset(batch, buffer_size);

    return batch;
}

void intel_batchbuffer_free(struct intel_batchbuffer *batch)
{
    if (batch->map) {
        dri_bo_unmap(batch->buffer);
        batch->map = NULL;
    }

    dri_bo_unreference(batch->buffer);
    dri_bo_unreference(batch->wa_render_bo);
    free(batch);
}

void 
intel_batchbuffer_flush(struct intel_batchbuffer *batch)
{
    unsigned int used = batch->ptr - batch->map;

    if (used == 0) {
        return;
    }

    if ((used & 4) == 0) {
        *(unsigned int*)batch->ptr = 0;
        batch->ptr += 4;
    }

    *(unsigned int*)batch->ptr = MI_BATCH_BUFFER_END;
    batch->ptr += 4;
    dri_bo_unmap(batch->buffer);
    used = batch->ptr - batch->map;
    batch->run(batch->buffer, used, 0, 0, 0, batch->flag);
    intel_batchbuffer_reset(batch, batch->size);
}

void 
intel_batchbuffer_emit_dword(struct intel_batchbuffer *batch, unsigned int x)
{
    assert(intel_batchbuffer_space(batch) >= 4);
    *(unsigned int *)batch->ptr = x;
    batch->ptr += 4;
}

void 
intel_batchbuffer_emit_reloc(struct intel_batchbuffer *batch, dri_bo *bo, 
                                uint32_t read_domains, uint32_t write_domains, 
                                uint32_t delta)
{
    assert(batch->ptr - batch->map < batch->size);
    dri_bo_emit_reloc(batch->buffer, read_domains, write_domains,
                      delta, batch->ptr - batch->map, bo);
    intel_batchbuffer_emit_dword(batch, bo->offset + delta);
}

void 
intel_batchbuffer_require_space(struct intel_batchbuffer *batch,
                                   unsigned int size)
{
    assert(size < batch->size - 8);

    if (intel_batchbuffer_space(batch) < size) {
        intel_batchbuffer_flush(batch);
    }
}

void 
intel_batchbuffer_data(struct intel_batchbuffer *batch,
                          void *data,
                          unsigned int size)
{
    assert((size & 3) == 0);
    intel_batchbuffer_require_space(batch, size);

    assert(batch->ptr);
    memcpy(batch->ptr, data, size);
    batch->ptr += size;
}

void
intel_batchbuffer_emit_mi_flush(struct intel_batchbuffer *batch)
{
    struct intel_driver_data *intel = batch->intel; 

    if (IS_GEN6(intel->device_info) ||
        IS_GEN7(intel->device_info) ||
        IS_GEN8(intel->device_info)) {
        if (batch->flag == I915_EXEC_RENDER) {
            if (IS_GEN8(intel->device_info)) {
                BEGIN_BATCH(batch, 6);
                OUT_BATCH(batch, CMD_PIPE_CONTROL | (6 - 2));

                OUT_BATCH(batch,
                          CMD_PIPE_CONTROL_CS_STALL |
                          CMD_PIPE_CONTROL_WC_FLUSH |
                          CMD_PIPE_CONTROL_TC_FLUSH |
                          CMD_PIPE_CONTROL_DC_FLUSH |
                          CMD_PIPE_CONTROL_NOWRITE);
                OUT_BATCH(batch, 0); /* write address */
                OUT_BATCH(batch, 0);
                OUT_BATCH(batch, 0); /* write data */
                OUT_BATCH(batch, 0);
                ADVANCE_BATCH(batch);
            } else if (IS_GEN6(intel->device_info)) {
                assert(batch->wa_render_bo);

                BEGIN_BATCH(batch, 4 * 3);

                OUT_BATCH(batch, CMD_PIPE_CONTROL | (4 - 2));
                OUT_BATCH(batch,
                          CMD_PIPE_CONTROL_CS_STALL |
                          CMD_PIPE_CONTROL_STALL_AT_SCOREBOARD);
                OUT_BATCH(batch, 0); /* address */
                OUT_BATCH(batch, 0); /* write data */

                OUT_BATCH(batch, CMD_PIPE_CONTROL | (4 - 2));
                OUT_BATCH(batch, CMD_PIPE_CONTROL_WRITE_QWORD);
                OUT_RELOC(batch,
                          batch->wa_render_bo,
                          I915_GEM_DOMAIN_INSTRUCTION,
                          I915_GEM_DOMAIN_INSTRUCTION,
                          0);
                OUT_BATCH(batch, 0); /* write data */

                /* now finally the _real flush */
                OUT_BATCH(batch, CMD_PIPE_CONTROL | (4 - 2));
                OUT_BATCH(batch,
                          CMD_PIPE_CONTROL_WC_FLUSH |
                          CMD_PIPE_CONTROL_TC_FLUSH |
                          CMD_PIPE_CONTROL_NOWRITE);
                OUT_BATCH(batch, 0); /* write address */
                OUT_BATCH(batch, 0); /* write data */
                ADVANCE_BATCH(batch);
            } else {
                BEGIN_BATCH(batch, 4);
                OUT_BATCH(batch, CMD_PIPE_CONTROL | (4 - 2));

                OUT_BATCH(batch, 
                          CMD_PIPE_CONTROL_WC_FLUSH |
                          CMD_PIPE_CONTROL_TC_FLUSH |
                          CMD_PIPE_CONTROL_DC_FLUSH |
                          CMD_PIPE_CONTROL_NOWRITE);
                OUT_BATCH(batch, 0); /* write address */
                OUT_BATCH(batch, 0); /* write data */
                ADVANCE_BATCH(batch);
            }

        } else {
            if (batch->flag == I915_EXEC_BLT) {
                BEGIN_BLT_BATCH(batch, 4);
                OUT_BLT_BATCH(batch, MI_FLUSH_DW);
                OUT_BLT_BATCH(batch, 0);
                OUT_BLT_BATCH(batch, 0);
                OUT_BLT_BATCH(batch, 0);
                ADVANCE_BLT_BATCH(batch);
            }else if (batch->flag == I915_EXEC_VEBOX) {
                BEGIN_VEB_BATCH(batch, 4);
                OUT_VEB_BATCH(batch, MI_FLUSH_DW);
                OUT_VEB_BATCH(batch, 0);
                OUT_VEB_BATCH(batch, 0);
                OUT_VEB_BATCH(batch, 0);
                ADVANCE_VEB_BATCH(batch);
            } else {
                assert(batch->flag == I915_EXEC_BSD);
                BEGIN_BCS_BATCH(batch, 4);
                OUT_BCS_BATCH(batch, MI_FLUSH_DW | MI_FLUSH_DW_VIDEO_PIPELINE_CACHE_INVALIDATE);
                OUT_BCS_BATCH(batch, 0);
                OUT_BCS_BATCH(batch, 0);
                OUT_BCS_BATCH(batch, 0);
                ADVANCE_BCS_BATCH(batch);
            }
        }
    } else {
        if (batch->flag == I915_EXEC_RENDER) {
            BEGIN_BATCH(batch, 1);
            OUT_BATCH(batch, MI_FLUSH | MI_FLUSH_STATE_INSTRUCTION_CACHE_INVALIDATE);
            ADVANCE_BATCH(batch);		
         } else {
            assert(batch->flag == I915_EXEC_BSD);
            BEGIN_BCS_BATCH(batch, 1);
            OUT_BCS_BATCH(batch, MI_FLUSH | MI_FLUSH_STATE_INSTRUCTION_CACHE_INVALIDATE);
            ADVANCE_BCS_BATCH(batch);
        }
    }
}

void
intel_batchbuffer_begin_batch(struct intel_batchbuffer *batch, int total)
{
    batch->emit_total = total * 4;
    batch->emit_start = batch->ptr;
}

void
intel_batchbuffer_advance_batch(struct intel_batchbuffer *batch)
{
    assert(batch->emit_total == (batch->ptr - batch->emit_start));
}

void
intel_batchbuffer_check_batchbuffer_flag(struct intel_batchbuffer *batch, int flag)
{
    if (flag != I915_EXEC_RENDER &&
        flag != I915_EXEC_BLT &&
        flag != I915_EXEC_BSD &&
        flag != I915_EXEC_VEBOX)
        return;

    if (batch->flag == flag)
        return;

    intel_batchbuffer_flush(batch);
    batch->flag = flag;
}

int
intel_batchbuffer_check_free_space(struct intel_batchbuffer *batch, int size)
{
    return intel_batchbuffer_space(batch) >= size;
}

static void
intel_batchbuffer_start_atomic_helper(struct intel_batchbuffer *batch,
                                      int flag,
                                      unsigned int size)
{
    assert(!batch->atomic);
    intel_batchbuffer_check_batchbuffer_flag(batch, flag);
    intel_batchbuffer_require_space(batch, size);
    batch->atomic = 1;
}

void
intel_batchbuffer_start_atomic(struct intel_batchbuffer *batch, unsigned int size)
{
    intel_batchbuffer_start_atomic_helper(batch, I915_EXEC_RENDER, size);
}

void
intel_batchbuffer_start_atomic_blt(struct intel_batchbuffer *batch, unsigned int size)
{
    intel_batchbuffer_start_atomic_helper(batch, I915_EXEC_BLT, size);
}

void
intel_batchbuffer_start_atomic_bcs(struct intel_batchbuffer *batch, unsigned int size)
{
    intel_batchbuffer_start_atomic_helper(batch, I915_EXEC_BSD, size);
}

void
intel_batchbuffer_start_atomic_veb(struct intel_batchbuffer *batch, unsigned int size)
{
    intel_batchbuffer_start_atomic_helper(batch, I915_EXEC_VEBOX, size);
}


void
intel_batchbuffer_end_atomic(struct intel_batchbuffer *batch)
{
    assert(batch->atomic);
    batch->atomic = 0;
}

int
intel_batchbuffer_used_size(struct intel_batchbuffer *batch)
{
    return batch->ptr - batch->map;
}

void
intel_batchbuffer_align(struct intel_batchbuffer *batch, unsigned int alignedment)
{
    int used = batch->ptr - batch->map;
    int pad_size;

    assert((alignedment & 3) == 0);
    pad_size = ALIGN(used, alignedment) - used;
    assert((pad_size & 3) == 0);
    assert(intel_batchbuffer_space(batch) >= pad_size);

    while (pad_size >= 4) {
        intel_batchbuffer_emit_dword(batch, 0);
        pad_size -= 4;
    }
}

