/**********************************************************
 * Copyright 2008-2009 VMware, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************/

#include "svga_cmd.h"

#include "pipe/p_state.h"
#include "pipe/p_defines.h"
#include "util/u_inlines.h"
#include "os/os_thread.h"
#include "util/u_format.h"
#include "util/u_math.h"
#include "util/u_memory.h"

#include "svga_format.h"
#include "svga_screen.h"
#include "svga_context.h"
#include "svga_resource_texture.h"
#include "svga_surface.h"
#include "svga_debug.h"


void
svga_texture_copy_handle(struct svga_context *svga,
                         struct svga_winsys_surface *src_handle,
                         unsigned src_x, unsigned src_y, unsigned src_z,
                         unsigned src_level, unsigned src_face,
                         struct svga_winsys_surface *dst_handle,
                         unsigned dst_x, unsigned dst_y, unsigned dst_z,
                         unsigned dst_level, unsigned dst_face,
                         unsigned width, unsigned height, unsigned depth)
{
   struct svga_surface dst, src;
   enum pipe_error ret;
   SVGA3dCopyBox box, *boxes;

   assert(svga);

   src.handle = src_handle;
   src.real_level = src_level;
   src.real_face = src_face;
   src.real_zslice = 0;

   dst.handle = dst_handle;
   dst.real_level = dst_level;
   dst.real_face = dst_face;
   dst.real_zslice = 0;

   box.x = dst_x;
   box.y = dst_y;
   box.z = dst_z;
   box.w = width;
   box.h = height;
   box.d = depth;
   box.srcx = src_x;
   box.srcy = src_y;
   box.srcz = src_z;

/*
   SVGA_DBG(DEBUG_VIEWS, "mipcopy src: %p %u (%ux%ux%u), dst: %p %u (%ux%ux%u)\n",
            src_handle, src_level, src_x, src_y, src_z,
            dst_handle, dst_level, dst_x, dst_y, dst_z);
*/

   ret = SVGA3D_BeginSurfaceCopy(svga->swc,
                                 &src.base,
                                 &dst.base,
                                 &boxes, 1);
   if (ret != PIPE_OK) {
      svga_context_flush(svga, NULL);
      ret = SVGA3D_BeginSurfaceCopy(svga->swc,
                                    &src.base,
                                    &dst.base,
                                    &boxes, 1);
      assert(ret == PIPE_OK);
   }
   *boxes = box;
   SVGA_FIFOCommitAll(svga->swc);
}


struct svga_winsys_surface *
svga_texture_view_surface(struct svga_context *svga,
                          struct svga_texture *tex,
                          SVGA3dSurfaceFlags flags,
                          SVGA3dSurfaceFormat format,
                          unsigned start_mip,
                          unsigned num_mip,
                          int face_pick,
                          int zslice_pick,
                          struct svga_host_surface_cache_key *key) /* OUT */
{
   struct svga_screen *ss = svga_screen(svga->pipe.screen);
   struct svga_winsys_surface *handle;
   uint32_t i, j;
   unsigned z_offset = 0;

   SVGA_DBG(DEBUG_PERF, 
            "svga: Create surface view: face %d zslice %d mips %d..%d\n",
            face_pick, zslice_pick, start_mip, start_mip+num_mip-1);

   key->flags = flags;
   key->format = format;
   key->numMipLevels = num_mip;
   key->size.width = u_minify(tex->b.b.width0, start_mip);
   key->size.height = u_minify(tex->b.b.height0, start_mip);
   key->size.depth = zslice_pick < 0 ? u_minify(tex->b.b.depth0, start_mip) : 1;
   key->cachable = 1;
   
   if (tex->b.b.target == PIPE_TEXTURE_CUBE && face_pick < 0) {
      key->flags |= SVGA3D_SURFACE_CUBEMAP;
      key->numFaces = 6;
   } else {
      key->numFaces = 1;
   }

   if (key->format == SVGA3D_FORMAT_INVALID) {
      key->cachable = 0;
      return NULL;
   }

   SVGA_DBG(DEBUG_DMA, "surface_create for texture view\n");
   handle = svga_screen_surface_create(ss, key);
   if (!handle) {
      key->cachable = 0;
      return NULL;
   }

   SVGA_DBG(DEBUG_DMA, " --> got sid %p (texture view)\n", handle);

   if (face_pick < 0)
      face_pick = 0;

   if (zslice_pick >= 0)
      z_offset = zslice_pick;

   for (i = 0; i < key->numMipLevels; i++) {
      for (j = 0; j < key->numFaces; j++) {
         if (svga_is_texture_level_defined(tex, j + face_pick, i + start_mip)) {
            unsigned depth = (zslice_pick < 0 ?
                              u_minify(tex->b.b.depth0, i + start_mip) :
                              1);

            svga_texture_copy_handle(svga,
                                     tex->handle, 
                                     0, 0, z_offset, 
                                     i + start_mip, 
                                     j + face_pick,
                                     handle, 0, 0, 0, i, j,
                                     u_minify(tex->b.b.width0, i + start_mip),
                                     u_minify(tex->b.b.height0, i + start_mip),
                                     depth);
         }
      }
   }

   return handle;
}


static struct pipe_surface *
svga_create_surface(struct pipe_context *pipe,
                    struct pipe_resource *pt,
                    const struct pipe_surface *surf_tmpl)
{
   struct svga_context *svga = svga_context(pipe);
   struct svga_texture *tex = svga_texture(pt);
   struct pipe_screen *screen = pipe->screen;
   struct svga_screen *ss = svga_screen(screen);
   struct svga_surface *s;
   unsigned face, zslice;
   boolean view = FALSE;
   SVGA3dSurfaceFlags flags;
   SVGA3dSurfaceFormat format;

   assert(surf_tmpl->u.tex.first_layer == surf_tmpl->u.tex.last_layer);

   s = CALLOC_STRUCT(svga_surface);
   if (!s)
      return NULL;

   if (pt->target == PIPE_TEXTURE_CUBE) {
      face = surf_tmpl->u.tex.first_layer;
      zslice = 0;
   }
   else {
      face = 0;
      zslice = surf_tmpl->u.tex.first_layer;
   }

   pipe_reference_init(&s->base.reference, 1);
   pipe_resource_reference(&s->base.texture, pt);
   s->base.context = pipe;
   s->base.format = surf_tmpl->format;
   s->base.width = u_minify(pt->width0, surf_tmpl->u.tex.level);
   s->base.height = u_minify(pt->height0, surf_tmpl->u.tex.level);
   s->base.u.tex.level = surf_tmpl->u.tex.level;
   s->base.u.tex.first_layer = surf_tmpl->u.tex.first_layer;
   s->base.u.tex.last_layer = surf_tmpl->u.tex.last_layer;

   if (util_format_is_depth_or_stencil(surf_tmpl->format)) {
      flags = SVGA3D_SURFACE_HINT_DEPTHSTENCIL;
   }
   else {
      flags = SVGA3D_SURFACE_HINT_RENDERTARGET;
   }

   format = svga_translate_format(ss, surf_tmpl->format, 0);
   assert(format != SVGA3D_FORMAT_INVALID);

   if (svga_screen(screen)->debug.force_surface_view)
      view = TRUE;

   /* Currently only used for compressed textures */
   if (format != svga_translate_format(ss, surf_tmpl->format, 0)) {
      view = TRUE;
   }

   if (surf_tmpl->u.tex.level != 0 &&
       svga_screen(screen)->debug.force_level_surface_view)
      view = TRUE;

   if (pt->target == PIPE_TEXTURE_3D)
      view = TRUE;

   if (svga_screen(screen)->debug.no_surface_view)
      view = FALSE;

   if (view) {
      SVGA_DBG(DEBUG_VIEWS, "svga: Surface view: yes %p, level %u face %u z %u, %p\n",
               pt, surf_tmpl->u.tex.level, face, zslice, s);

      s->handle = svga_texture_view_surface(svga, tex, flags, format,
                                            surf_tmpl->u.tex.level,
                                            1, face, zslice, &s->key);
      s->real_face = 0;
      s->real_level = 0;
      s->real_zslice = 0;
   } else {
      SVGA_DBG(DEBUG_VIEWS, "svga: Surface view: no %p, level %u, face %u, z %u, %p\n",
               pt, surf_tmpl->u.tex.level, face, zslice, s);

      memset(&s->key, 0, sizeof s->key);
      s->handle = tex->handle;
      s->real_face = face;
      s->real_zslice = zslice;
      s->real_level = surf_tmpl->u.tex.level;
   }

   return &s->base;
}


static void
svga_surface_destroy(struct pipe_context *pipe,
                     struct pipe_surface *surf)
{
   struct svga_surface *s = svga_surface(surf);
   struct svga_texture *t = svga_texture(surf->texture);
   struct svga_screen *ss = svga_screen(surf->texture->screen);

   if (s->handle != t->handle) {
      SVGA_DBG(DEBUG_DMA, "unref sid %p (tex surface)\n", s->handle);
      svga_screen_surface_destroy(ss, &s->key, &s->handle);
   }

   pipe_resource_reference(&surf->texture, NULL);
   FREE(surf);
}


static void 
svga_mark_surface_dirty(struct pipe_surface *surf)
{
   struct svga_surface *s = svga_surface(surf);

   if (!s->dirty) {
      struct svga_texture *tex = svga_texture(surf->texture);

      s->dirty = TRUE;

      if (s->handle == tex->handle) {
         /* hmm so 3d textures always have all their slices marked ? */
         if (surf->texture->target == PIPE_TEXTURE_CUBE)
            svga_define_texture_level(tex, surf->u.tex.first_layer,
                                      surf->u.tex.level);
         else
            svga_define_texture_level(tex, 0, surf->u.tex.level);
      }
      else {
         /* this will happen later in svga_propagate_surface */
      }

      /* Increment the view_age and texture age for this surface's mipmap
       * level so that any sampler views into the texture are re-validated too.
       */
      svga_age_texture_view(tex, surf->u.tex.level);
   }
}


void
svga_mark_surfaces_dirty(struct svga_context *svga)
{
   struct svga_screen *svgascreen = svga_screen(svga->pipe.screen);
   unsigned i;

   for (i = 0; i < svgascreen->max_color_buffers; i++) {
      if (svga->curr.framebuffer.cbufs[i])
         svga_mark_surface_dirty(svga->curr.framebuffer.cbufs[i]);
   }
   if (svga->curr.framebuffer.zsbuf)
      svga_mark_surface_dirty(svga->curr.framebuffer.zsbuf);
}


/**
 * Progagate any changes from surfaces to texture.
 * pipe is optional context to inline the blit command in.
 */
void
svga_propagate_surface(struct svga_context *svga, struct pipe_surface *surf)
{
   struct svga_surface *s = svga_surface(surf);
   struct svga_texture *tex = svga_texture(surf->texture);
   struct svga_screen *ss = svga_screen(surf->texture->screen);
   unsigned zslice, face;

   if (!s->dirty)
      return;

   if (surf->texture->target == PIPE_TEXTURE_CUBE) {
      zslice = 0;
      face = surf->u.tex.first_layer;
   }
   else {
      zslice = surf->u.tex.first_layer;
      face = 0;
   }

   s->dirty = FALSE;
   ss->texture_timestamp++;
   svga_age_texture_view(tex, surf->u.tex.level);

   if (s->handle != tex->handle) {
      SVGA_DBG(DEBUG_VIEWS,
               "svga: Surface propagate: tex %p, level %u, from %p\n",
               tex, surf->u.tex.level, surf);
      svga_texture_copy_handle(svga,
                               s->handle, 0, 0, 0, s->real_level, s->real_face,
                               tex->handle, 0, 0, zslice, surf->u.tex.level, face,
                               u_minify(tex->b.b.width0, surf->u.tex.level),
                               u_minify(tex->b.b.height0, surf->u.tex.level), 1);
      svga_define_texture_level(tex, face, surf->u.tex.level);
   }
}


/**
 * Check if we should call svga_propagate_surface on the surface.
 */
boolean
svga_surface_needs_propagation(const struct pipe_surface *surf)
{
   const struct svga_surface *s = svga_surface_const(surf);
   struct svga_texture *tex = svga_texture(surf->texture);

   return s->dirty && s->handle != tex->handle;
}



void
svga_init_surface_functions(struct svga_context *svga)
{
   svga->pipe.create_surface = svga_create_surface;
   svga->pipe.surface_destroy = svga_surface_destroy;
}
