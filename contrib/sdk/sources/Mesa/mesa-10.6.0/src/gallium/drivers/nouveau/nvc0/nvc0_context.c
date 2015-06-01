/*
 * Copyright 2010 Christoph Bumiller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pipe/p_defines.h"
#include "util/u_framebuffer.h"

#include "nvc0/nvc0_context.h"
#include "nvc0/nvc0_screen.h"
#include "nvc0/nvc0_resource.h"

static void
nvc0_flush(struct pipe_context *pipe,
           struct pipe_fence_handle **fence,
           unsigned flags)
{
   struct nvc0_context *nvc0 = nvc0_context(pipe);
   struct nouveau_screen *screen = &nvc0->screen->base;

   if (fence)
      nouveau_fence_ref(screen->fence.current, (struct nouveau_fence **)fence);

   PUSH_KICK(nvc0->base.pushbuf); /* fencing handled in kick_notify */

   nouveau_context_update_frame_stats(&nvc0->base);
}

static void
nvc0_texture_barrier(struct pipe_context *pipe)
{
   struct nouveau_pushbuf *push = nvc0_context(pipe)->base.pushbuf;

   IMMED_NVC0(push, NVC0_3D(SERIALIZE), 0);
   IMMED_NVC0(push, NVC0_3D(TEX_CACHE_CTL), 0);
}

static void
nvc0_memory_barrier(struct pipe_context *pipe, unsigned flags)
{
   struct nvc0_context *nvc0 = nvc0_context(pipe);
   int i, s;

   if (flags & PIPE_BARRIER_MAPPED_BUFFER) {
      for (i = 0; i < nvc0->num_vtxbufs; ++i) {
         if (!nvc0->vtxbuf[i].buffer)
            continue;
         if (nvc0->vtxbuf[i].buffer->flags & PIPE_RESOURCE_FLAG_MAP_PERSISTENT)
            nvc0->base.vbo_dirty = TRUE;
      }

      if (nvc0->idxbuf.buffer &&
          nvc0->idxbuf.buffer->flags & PIPE_RESOURCE_FLAG_MAP_PERSISTENT)
         nvc0->base.vbo_dirty = TRUE;

      for (s = 0; s < 5 && !nvc0->cb_dirty; ++s) {
         uint32_t valid = nvc0->constbuf_valid[s];

         while (valid && !nvc0->cb_dirty) {
            const unsigned i = ffs(valid) - 1;
            struct pipe_resource *res;

            valid &= ~(1 << i);
            if (nvc0->constbuf[s][i].user)
               continue;

            res = nvc0->constbuf[s][i].u.buf;
            if (!res)
               continue;

            if (res->flags & PIPE_RESOURCE_FLAG_MAP_PERSISTENT)
               nvc0->cb_dirty = TRUE;
         }
      }
   }
}

static void
nvc0_context_unreference_resources(struct nvc0_context *nvc0)
{
   unsigned s, i;

   nouveau_bufctx_del(&nvc0->bufctx_3d);
   nouveau_bufctx_del(&nvc0->bufctx);
   nouveau_bufctx_del(&nvc0->bufctx_cp);

   util_unreference_framebuffer_state(&nvc0->framebuffer);

   for (i = 0; i < nvc0->num_vtxbufs; ++i)
      pipe_resource_reference(&nvc0->vtxbuf[i].buffer, NULL);

   pipe_resource_reference(&nvc0->idxbuf.buffer, NULL);

   for (s = 0; s < 6; ++s) {
      for (i = 0; i < nvc0->num_textures[s]; ++i)
         pipe_sampler_view_reference(&nvc0->textures[s][i], NULL);

      for (i = 0; i < NVC0_MAX_PIPE_CONSTBUFS; ++i)
         if (!nvc0->constbuf[s][i].user)
            pipe_resource_reference(&nvc0->constbuf[s][i].u.buf, NULL);
   }

   for (s = 0; s < 2; ++s) {
      for (i = 0; i < NVC0_MAX_SURFACE_SLOTS; ++i)
         pipe_surface_reference(&nvc0->surfaces[s][i], NULL);
   }

   for (i = 0; i < nvc0->num_tfbbufs; ++i)
      pipe_so_target_reference(&nvc0->tfbbuf[i], NULL);

   for (i = 0; i < nvc0->global_residents.size / sizeof(struct pipe_resource *);
        ++i) {
      struct pipe_resource **res = util_dynarray_element(
         &nvc0->global_residents, struct pipe_resource *, i);
      pipe_resource_reference(res, NULL);
   }
   util_dynarray_fini(&nvc0->global_residents);
}

static void
nvc0_destroy(struct pipe_context *pipe)
{
   struct nvc0_context *nvc0 = nvc0_context(pipe);

   if (nvc0->screen->cur_ctx == nvc0) {
      nvc0->screen->cur_ctx = NULL;
      nvc0->screen->save_state = nvc0->state;
      nvc0->screen->save_state.tfb = NULL;
   }

   /* Unset bufctx, we don't want to revalidate any resources after the flush.
    * Other contexts will always set their bufctx again on action calls.
    */
   nouveau_pushbuf_bufctx(nvc0->base.pushbuf, NULL);
   nouveau_pushbuf_kick(nvc0->base.pushbuf, nvc0->base.pushbuf->channel);

   nvc0_context_unreference_resources(nvc0);
   nvc0_blitctx_destroy(nvc0);

   nouveau_context_destroy(&nvc0->base);
}

void
nvc0_default_kick_notify(struct nouveau_pushbuf *push)
{
   struct nvc0_screen *screen = push->user_priv;

   if (screen) {
      nouveau_fence_next(&screen->base);
      nouveau_fence_update(&screen->base, TRUE);
      if (screen->cur_ctx)
         screen->cur_ctx->state.flushed = TRUE;
      NOUVEAU_DRV_STAT(&screen->base, pushbuf_count, 1);
   }
}

static int
nvc0_invalidate_resource_storage(struct nouveau_context *ctx,
                                 struct pipe_resource *res,
                                 int ref)
{
   struct nvc0_context *nvc0 = nvc0_context(&ctx->pipe);
   unsigned s, i;

   if (res->bind & PIPE_BIND_RENDER_TARGET) {
      for (i = 0; i < nvc0->framebuffer.nr_cbufs; ++i) {
         if (nvc0->framebuffer.cbufs[i] &&
             nvc0->framebuffer.cbufs[i]->texture == res) {
            nvc0->dirty |= NVC0_NEW_FRAMEBUFFER;
            nouveau_bufctx_reset(nvc0->bufctx_3d, NVC0_BIND_FB);
            if (!--ref)
               return ref;
         }
      }
   }
   if (res->bind & PIPE_BIND_DEPTH_STENCIL) {
      if (nvc0->framebuffer.zsbuf &&
          nvc0->framebuffer.zsbuf->texture == res) {
         nvc0->dirty |= NVC0_NEW_FRAMEBUFFER;
         nouveau_bufctx_reset(nvc0->bufctx_3d, NVC0_BIND_FB);
         if (!--ref)
            return ref;
      }
   }

   if (res->bind & (PIPE_BIND_VERTEX_BUFFER |
                    PIPE_BIND_INDEX_BUFFER |
                    PIPE_BIND_CONSTANT_BUFFER |
                    PIPE_BIND_STREAM_OUTPUT |
                    PIPE_BIND_COMMAND_ARGS_BUFFER |
                    PIPE_BIND_SAMPLER_VIEW)) {
      for (i = 0; i < nvc0->num_vtxbufs; ++i) {
         if (nvc0->vtxbuf[i].buffer == res) {
            nvc0->dirty |= NVC0_NEW_ARRAYS;
            nouveau_bufctx_reset(nvc0->bufctx_3d, NVC0_BIND_VTX);
            if (!--ref)
               return ref;
         }
      }

      if (nvc0->idxbuf.buffer == res) {
         nvc0->dirty |= NVC0_NEW_IDXBUF;
         nouveau_bufctx_reset(nvc0->bufctx_3d, NVC0_BIND_IDX);
         if (!--ref)
            return ref;
      }

      for (s = 0; s < 5; ++s) {
      for (i = 0; i < nvc0->num_textures[s]; ++i) {
         if (nvc0->textures[s][i] &&
             nvc0->textures[s][i]->texture == res) {
            nvc0->textures_dirty[s] |= 1 << i;
            nvc0->dirty |= NVC0_NEW_TEXTURES;
            nouveau_bufctx_reset(nvc0->bufctx_3d, NVC0_BIND_TEX(s, i));
            if (!--ref)
               return ref;
         }
      }
      }

      for (s = 0; s < 5; ++s) {
      for (i = 0; i < NVC0_MAX_PIPE_CONSTBUFS; ++i) {
         if (!(nvc0->constbuf_valid[s] & (1 << i)))
            continue;
         if (!nvc0->constbuf[s][i].user &&
             nvc0->constbuf[s][i].u.buf == res) {
            nvc0->dirty |= NVC0_NEW_CONSTBUF;
            nvc0->constbuf_dirty[s] |= 1 << i;
            nouveau_bufctx_reset(nvc0->bufctx_3d, NVC0_BIND_CB(s, i));
            if (!--ref)
               return ref;
         }
      }
      }
   }

   return ref;
}

static void
nvc0_context_get_sample_position(struct pipe_context *, unsigned, unsigned,
                                 float *);

struct pipe_context *
nvc0_create(struct pipe_screen *pscreen, void *priv)
{
   struct nvc0_screen *screen = nvc0_screen(pscreen);
   struct nvc0_context *nvc0;
   struct pipe_context *pipe;
   int ret;
   uint32_t flags;

   nvc0 = CALLOC_STRUCT(nvc0_context);
   if (!nvc0)
      return NULL;
   pipe = &nvc0->base.pipe;

   if (!nvc0_blitctx_create(nvc0))
      goto out_err;

   nvc0->base.pushbuf = screen->base.pushbuf;
   nvc0->base.client = screen->base.client;

   ret = nouveau_bufctx_new(screen->base.client, 2, &nvc0->bufctx);
   if (!ret)
      ret = nouveau_bufctx_new(screen->base.client, NVC0_BIND_3D_COUNT,
                               &nvc0->bufctx_3d);
   if (!ret)
      ret = nouveau_bufctx_new(screen->base.client, NVC0_BIND_CP_COUNT,
                               &nvc0->bufctx_cp);
   if (ret)
      goto out_err;

   nvc0->screen = screen;
   nvc0->base.screen = &screen->base;

   pipe->screen = pscreen;
   pipe->priv = priv;

   pipe->destroy = nvc0_destroy;

   pipe->draw_vbo = nvc0_draw_vbo;
   pipe->clear = nvc0_clear;
   pipe->launch_grid = (nvc0->screen->base.class_3d >= NVE4_3D_CLASS) ?
      nve4_launch_grid : nvc0_launch_grid;

   pipe->flush = nvc0_flush;
   pipe->texture_barrier = nvc0_texture_barrier;
   pipe->memory_barrier = nvc0_memory_barrier;
   pipe->get_sample_position = nvc0_context_get_sample_position;

   if (!screen->cur_ctx) {
      nvc0->state = screen->save_state;
      screen->cur_ctx = nvc0;
      nouveau_pushbuf_bufctx(screen->base.pushbuf, nvc0->bufctx);
   }
   screen->base.pushbuf->kick_notify = nvc0_default_kick_notify;

   nvc0_init_query_functions(nvc0);
   nvc0_init_surface_functions(nvc0);
   nvc0_init_state_functions(nvc0);
   nvc0_init_transfer_functions(nvc0);
   nvc0_init_resource_functions(pipe);

   nvc0->base.invalidate_resource_storage = nvc0_invalidate_resource_storage;

   pipe->create_video_codec = nvc0_create_decoder;
   pipe->create_video_buffer = nvc0_video_buffer_create;

   /* shader builtin library is per-screen, but we need a context for m2mf */
   nvc0_program_library_upload(nvc0);

   /* add permanently resident buffers to bufctxts */

   flags = NOUVEAU_BO_VRAM | NOUVEAU_BO_RD;

   BCTX_REFN_bo(nvc0->bufctx_3d, SCREEN, flags, screen->text);
   BCTX_REFN_bo(nvc0->bufctx_3d, SCREEN, flags, screen->uniform_bo);
   BCTX_REFN_bo(nvc0->bufctx_3d, SCREEN, flags, screen->txc);
   if (screen->compute) {
      BCTX_REFN_bo(nvc0->bufctx_cp, CP_SCREEN, flags, screen->text);
      BCTX_REFN_bo(nvc0->bufctx_cp, CP_SCREEN, flags, screen->txc);
      BCTX_REFN_bo(nvc0->bufctx_cp, CP_SCREEN, flags, screen->parm);
   }

   flags = NOUVEAU_BO_VRAM | NOUVEAU_BO_RDWR;

   if (screen->poly_cache)
      BCTX_REFN_bo(nvc0->bufctx_3d, SCREEN, flags, screen->poly_cache);
   if (screen->compute)
      BCTX_REFN_bo(nvc0->bufctx_cp, CP_SCREEN, flags, screen->tls);

   flags = NOUVEAU_BO_GART | NOUVEAU_BO_WR;

   BCTX_REFN_bo(nvc0->bufctx_3d, SCREEN, flags, screen->fence.bo);
   BCTX_REFN_bo(nvc0->bufctx, FENCE, flags, screen->fence.bo);
   if (screen->compute)
      BCTX_REFN_bo(nvc0->bufctx_cp, CP_SCREEN, flags, screen->fence.bo);

   nvc0->base.scratch.bo_size = 2 << 20;

   memset(nvc0->tex_handles, ~0, sizeof(nvc0->tex_handles));

   util_dynarray_init(&nvc0->global_residents);

   return pipe;

out_err:
   if (nvc0) {
      if (nvc0->bufctx_3d)
         nouveau_bufctx_del(&nvc0->bufctx_3d);
      if (nvc0->bufctx_cp)
         nouveau_bufctx_del(&nvc0->bufctx_cp);
      if (nvc0->bufctx)
         nouveau_bufctx_del(&nvc0->bufctx);
      FREE(nvc0->blit);
      FREE(nvc0);
   }
   return NULL;
}

void
nvc0_bufctx_fence(struct nvc0_context *nvc0, struct nouveau_bufctx *bufctx,
                  boolean on_flush)
{
   struct nouveau_list *list = on_flush ? &bufctx->current : &bufctx->pending;
   struct nouveau_list *it;
   NOUVEAU_DRV_STAT_IFD(unsigned count = 0);

   for (it = list->next; it != list; it = it->next) {
      struct nouveau_bufref *ref = (struct nouveau_bufref *)it;
      struct nv04_resource *res = ref->priv;
      if (res)
         nvc0_resource_validate(res, (unsigned)ref->priv_data);
      NOUVEAU_DRV_STAT_IFD(count++);
   }
   NOUVEAU_DRV_STAT(&nvc0->screen->base, resource_validate_count, count);
}

static void
nvc0_context_get_sample_position(struct pipe_context *pipe,
                                 unsigned sample_count, unsigned sample_index,
                                 float *xy)
{
   static const uint8_t ms1[1][2] = { { 0x8, 0x8 } };
   static const uint8_t ms2[2][2] = {
      { 0x4, 0x4 }, { 0xc, 0xc } }; /* surface coords (0,0), (1,0) */
   static const uint8_t ms4[4][2] = {
      { 0x6, 0x2 }, { 0xe, 0x6 },   /* (0,0), (1,0) */
      { 0x2, 0xa }, { 0xa, 0xe } }; /* (0,1), (1,1) */
   static const uint8_t ms8[8][2] = {
      { 0x1, 0x7 }, { 0x5, 0x3 },   /* (0,0), (1,0) */
      { 0x3, 0xd }, { 0x7, 0xb },   /* (0,1), (1,1) */
      { 0x9, 0x5 }, { 0xf, 0x1 },   /* (2,0), (3,0) */
      { 0xb, 0xf }, { 0xd, 0x9 } }; /* (2,1), (3,1) */
#if 0
   /* NOTE: there are alternative modes for MS2 and MS8, currently not used */
   static const uint8_t ms8_alt[8][2] = {
      { 0x9, 0x5 }, { 0x7, 0xb },   /* (2,0), (1,1) */
      { 0xd, 0x9 }, { 0x5, 0x3 },   /* (3,1), (1,0) */
      { 0x3, 0xd }, { 0x1, 0x7 },   /* (0,1), (0,0) */
      { 0xb, 0xf }, { 0xf, 0x1 } }; /* (2,1), (3,0) */
#endif

   const uint8_t (*ptr)[2];

   switch (sample_count) {
   case 0:
   case 1: ptr = ms1; break;
   case 2: ptr = ms2; break;
   case 4: ptr = ms4; break;
   case 8: ptr = ms8; break;
   default:
      assert(0);
      return; /* bad sample count -> undefined locations */
   }
   xy[0] = ptr[sample_index][0] * 0.0625f;
   xy[1] = ptr[sample_index][1] * 0.0625f;
}
