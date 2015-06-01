/*
 * Copyright (C) 2013 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Rob Clark <robclark@freedesktop.org>
 */

/**
 * This module converts provides a more convenient front-end to u_indices,
 * etc, utils to convert primitive types supported not supported by the
 * hardware.  It handles binding new index buffer state, and restoring
 * previous state after.  To use, put something like this at the front of
 * drivers pipe->draw_vbo():
 *
 *    // emulate unsupported primitives:
 *    if (info->mode needs emulating) {
 *       util_primconvert_save_index_buffer(ctx->primconvert, &ctx->indexbuf);
 *       util_primconvert_save_rasterizer_state(ctx->primconvert, ctx->rasterizer);
 *       util_primconvert_draw_vbo(ctx->primconvert, info);
 *       return;
 *    }
 *
 */

#include "pipe/p_state.h"
#include "util/u_draw.h"
#include "util/u_inlines.h"
#include "util/u_memory.h"
#include "util/u_upload_mgr.h"

#include "indices/u_indices.h"
#include "indices/u_primconvert.h"

struct primconvert_context
{
   struct pipe_context *pipe;
   struct pipe_index_buffer saved_ib;
   uint32_t primtypes_mask;
   unsigned api_pv;
   struct u_upload_mgr *upload;
};


struct primconvert_context *
util_primconvert_create(struct pipe_context *pipe, uint32_t primtypes_mask)
{
   struct primconvert_context *pc = CALLOC_STRUCT(primconvert_context);
   if (!pc)
      return NULL;
   pc->pipe = pipe;
   pc->primtypes_mask = primtypes_mask;
   return pc;
}

void
util_primconvert_destroy(struct primconvert_context *pc)
{
   if (pc->upload)
      u_upload_destroy(pc->upload);
   util_primconvert_save_index_buffer(pc, NULL);
   FREE(pc);
}

void
util_primconvert_save_index_buffer(struct primconvert_context *pc,
                                   const struct pipe_index_buffer *ib)
{
   if (ib) {
      pipe_resource_reference(&pc->saved_ib.buffer, ib->buffer);
      pc->saved_ib.index_size = ib->index_size;
      pc->saved_ib.offset = ib->offset;
      pc->saved_ib.user_buffer = ib->user_buffer;
   }
   else {
      pipe_resource_reference(&pc->saved_ib.buffer, NULL);
   }
}

void
util_primconvert_save_rasterizer_state(struct primconvert_context *pc,
                                       const struct pipe_rasterizer_state
                                       *rast)
{
   /* if we actually translated the provoking vertex for the buffer,
    * we would actually need to save/restore rasterizer state.  As
    * it is, we just need to make note of the pv.
    */
   pc->api_pv = rast->flatshade_first ? PV_FIRST : PV_LAST;
}

void
util_primconvert_draw_vbo(struct primconvert_context *pc,
                          const struct pipe_draw_info *info)
{
   struct pipe_index_buffer *ib = &pc->saved_ib;
   struct pipe_index_buffer new_ib;
   struct pipe_draw_info new_info;
   struct pipe_transfer *src_transfer = NULL;
   u_translate_func trans_func;
   u_generate_func gen_func;
   const void *src = NULL;
   void *dst;

   memset(&new_ib, 0, sizeof(new_ib));
   util_draw_init_info(&new_info);
   new_info.indexed = true;
   new_info.min_index = info->min_index;
   new_info.max_index = info->max_index;
   new_info.index_bias = info->index_bias;
   new_info.start_instance = info->start_instance;
   new_info.instance_count = info->instance_count;
   new_info.primitive_restart = info->primitive_restart;
   new_info.restart_index = info->restart_index;
   if (info->indexed) {
      u_index_translator(pc->primtypes_mask,
                         info->mode, pc->saved_ib.index_size, info->count,
                         pc->api_pv, pc->api_pv,
                         info->primitive_restart ? PR_ENABLE : PR_DISABLE,
                         &new_info.mode, &new_ib.index_size, &new_info.count,
                         &trans_func);
      src = ib->user_buffer;
      if (!src) {
         src = pipe_buffer_map(pc->pipe, ib->buffer,
                               PIPE_TRANSFER_READ, &src_transfer);
      }
      src = (const uint8_t *)src + ib->offset;
   }
   else {
      u_index_generator(pc->primtypes_mask,
                        info->mode, info->start, info->count,
                        pc->api_pv, pc->api_pv,
                        &new_info.mode, &new_ib.index_size, &new_info.count,
                        &gen_func);
   }

   if (!pc->upload) {
      pc->upload = u_upload_create(pc->pipe, 4096, 4, PIPE_BIND_INDEX_BUFFER);
   }

   u_upload_alloc(pc->upload, 0, new_ib.index_size * new_info.count,
                  &new_ib.offset, &new_ib.buffer, &dst);

   if (info->indexed) {
      trans_func(src, info->start, info->count, new_info.count, info->restart_index, dst);
   }
   else {
      gen_func(info->start, new_info.count, dst);
   }

   if (src_transfer)
      pipe_buffer_unmap(pc->pipe, src_transfer);

   u_upload_unmap(pc->upload);

   /* bind new index buffer: */
   pc->pipe->set_index_buffer(pc->pipe, &new_ib);

   /* to the translated draw: */
   pc->pipe->draw_vbo(pc->pipe, &new_info);

   /* and then restore saved ib: */
   pc->pipe->set_index_buffer(pc->pipe, ib);

   pipe_resource_reference(&new_ib.buffer, NULL);
}
