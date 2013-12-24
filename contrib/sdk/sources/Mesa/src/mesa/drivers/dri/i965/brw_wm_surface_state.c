/*
 Copyright (C) Intel Corp.  2006.  All Rights Reserved.
 Intel funded Tungsten Graphics (http://www.tungstengraphics.com) to
 develop this 3D driver.

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice (including the
 next paragraph) shall be included in all copies or substantial
 portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 **********************************************************************/
 /*
  * Authors:
  *   Keith Whitwell <keith@tungstengraphics.com>
  */


#include "main/context.h"
#include "main/blend.h"
#include "main/mtypes.h"
#include "main/samplerobj.h"
#include "program/prog_parameter.h"

#include "intel_mipmap_tree.h"
#include "intel_batchbuffer.h"
#include "intel_tex.h"
#include "intel_fbo.h"
#include "intel_buffer_objects.h"

#include "brw_context.h"
#include "brw_state.h"
#include "brw_defines.h"
#include "brw_wm.h"

GLuint
translate_tex_target(GLenum target)
{
   switch (target) {
   case GL_TEXTURE_1D:
   case GL_TEXTURE_1D_ARRAY_EXT:
      return BRW_SURFACE_1D;

   case GL_TEXTURE_RECTANGLE_NV:
      return BRW_SURFACE_2D;

   case GL_TEXTURE_2D:
   case GL_TEXTURE_2D_ARRAY_EXT:
   case GL_TEXTURE_EXTERNAL_OES:
   case GL_TEXTURE_2D_MULTISAMPLE:
   case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
      return BRW_SURFACE_2D;

   case GL_TEXTURE_3D:
      return BRW_SURFACE_3D;

   case GL_TEXTURE_CUBE_MAP:
   case GL_TEXTURE_CUBE_MAP_ARRAY:
      return BRW_SURFACE_CUBE;

   default:
      assert(0);
      return 0;
   }
}

uint32_t
brw_get_surface_tiling_bits(uint32_t tiling)
{
   switch (tiling) {
   case I915_TILING_X:
      return BRW_SURFACE_TILED;
   case I915_TILING_Y:
      return BRW_SURFACE_TILED | BRW_SURFACE_TILED_Y;
   default:
      return 0;
   }
}


uint32_t
brw_get_surface_num_multisamples(unsigned num_samples)
{
   if (num_samples > 1)
      return BRW_SURFACE_MULTISAMPLECOUNT_4;
   else
      return BRW_SURFACE_MULTISAMPLECOUNT_1;
}


/**
 * Compute the combination of DEPTH_TEXTURE_MODE and EXT_texture_swizzle
 * swizzling.
 */
int
brw_get_texture_swizzle(const struct gl_context *ctx,
                        const struct gl_texture_object *t)
{
   const struct gl_texture_image *img = t->Image[0][t->BaseLevel];

   int swizzles[SWIZZLE_NIL + 1] = {
      SWIZZLE_X,
      SWIZZLE_Y,
      SWIZZLE_Z,
      SWIZZLE_W,
      SWIZZLE_ZERO,
      SWIZZLE_ONE,
      SWIZZLE_NIL
   };

   if (img->_BaseFormat == GL_DEPTH_COMPONENT ||
       img->_BaseFormat == GL_DEPTH_STENCIL) {
      GLenum depth_mode = t->DepthMode;

      /* In ES 3.0, DEPTH_TEXTURE_MODE is expected to be GL_RED for textures
       * with depth component data specified with a sized internal format.
       * Otherwise, it's left at the old default, GL_LUMINANCE.
       */
      if (_mesa_is_gles3(ctx) &&
          img->InternalFormat != GL_DEPTH_COMPONENT &&
          img->InternalFormat != GL_DEPTH_STENCIL) {
         depth_mode = GL_RED;
      }

      switch (depth_mode) {
      case GL_ALPHA:
         swizzles[0] = SWIZZLE_ZERO;
         swizzles[1] = SWIZZLE_ZERO;
         swizzles[2] = SWIZZLE_ZERO;
         swizzles[3] = SWIZZLE_X;
         break;
      case GL_LUMINANCE:
         swizzles[0] = SWIZZLE_X;
         swizzles[1] = SWIZZLE_X;
         swizzles[2] = SWIZZLE_X;
         swizzles[3] = SWIZZLE_ONE;
         break;
      case GL_INTENSITY:
         swizzles[0] = SWIZZLE_X;
         swizzles[1] = SWIZZLE_X;
         swizzles[2] = SWIZZLE_X;
         swizzles[3] = SWIZZLE_X;
         break;
      case GL_RED:
         swizzles[0] = SWIZZLE_X;
         swizzles[1] = SWIZZLE_ZERO;
         swizzles[2] = SWIZZLE_ZERO;
         swizzles[3] = SWIZZLE_ONE;
         break;
      }
   }

   /* If the texture's format is alpha-only, force R, G, and B to
    * 0.0. Similarly, if the texture's format has no alpha channel,
    * force the alpha value read to 1.0. This allows for the
    * implementation to use an RGBA texture for any of these formats
    * without leaking any unexpected values.
    */
   switch (img->_BaseFormat) {
   case GL_ALPHA:
      swizzles[0] = SWIZZLE_ZERO;
      swizzles[1] = SWIZZLE_ZERO;
      swizzles[2] = SWIZZLE_ZERO;
      break;
   case GL_RED:
   case GL_RG:
   case GL_RGB:
      if (_mesa_get_format_bits(img->TexFormat, GL_ALPHA_BITS) > 0)
         swizzles[3] = SWIZZLE_ONE;
      break;
   }

   return MAKE_SWIZZLE4(swizzles[GET_SWZ(t->_Swizzle, 0)],
                        swizzles[GET_SWZ(t->_Swizzle, 1)],
                        swizzles[GET_SWZ(t->_Swizzle, 2)],
                        swizzles[GET_SWZ(t->_Swizzle, 3)]);
}


static void
brw_update_buffer_texture_surface(struct gl_context *ctx,
                                  unsigned unit,
                                  uint32_t *binding_table,
                                  unsigned surf_index)
{
   struct brw_context *brw = brw_context(ctx);
   struct gl_texture_object *tObj = ctx->Texture.Unit[unit]._Current;
   uint32_t *surf;
   struct intel_buffer_object *intel_obj =
      intel_buffer_object(tObj->BufferObject);
   drm_intel_bo *bo = intel_obj ? intel_obj->buffer : NULL;
   gl_format format = tObj->_BufferObjectFormat;
   uint32_t brw_format = brw_format_for_mesa_format(format);
   int texel_size = _mesa_get_format_bytes(format);

   if (brw_format == 0 && format != MESA_FORMAT_RGBA_FLOAT32) {
      _mesa_problem(NULL, "bad format %s for texture buffer\n",
		    _mesa_get_format_name(format));
   }

   surf = brw_state_batch(brw, AUB_TRACE_SURFACE_STATE,
			  6 * 4, 32, &binding_table[surf_index]);

   surf[0] = (BRW_SURFACE_BUFFER << BRW_SURFACE_TYPE_SHIFT |
	      (brw_format_for_mesa_format(format) << BRW_SURFACE_FORMAT_SHIFT));

   if (brw->gen >= 6)
      surf[0] |= BRW_SURFACE_RC_READ_WRITE;

   if (bo) {
      surf[1] = bo->offset; /* reloc */

      /* Emit relocation to surface contents. */
      drm_intel_bo_emit_reloc(brw->batch.bo,
			      binding_table[surf_index] + 4,
			      bo, 0, I915_GEM_DOMAIN_SAMPLER, 0);

      int w = intel_obj->Base.Size / texel_size;
      surf[2] = ((w & 0x7f) << BRW_SURFACE_WIDTH_SHIFT |
		 ((w >> 7) & 0x1fff) << BRW_SURFACE_HEIGHT_SHIFT);
      surf[3] = (((w >> 20) & 0x7f) << BRW_SURFACE_DEPTH_SHIFT |
		 (texel_size - 1) << BRW_SURFACE_PITCH_SHIFT);
   } else {
      surf[1] = 0;
      surf[2] = 0;
      surf[3] = 0;
   }

   surf[4] = 0;
   surf[5] = 0;
}

static void
brw_update_texture_surface(struct gl_context *ctx,
                           unsigned unit,
                           uint32_t *binding_table,
                           unsigned surf_index)
{
   struct brw_context *brw = brw_context(ctx);
   struct gl_texture_object *tObj = ctx->Texture.Unit[unit]._Current;
   struct intel_texture_object *intelObj = intel_texture_object(tObj);
   struct intel_mipmap_tree *mt = intelObj->mt;
   struct gl_texture_image *firstImage = tObj->Image[0][tObj->BaseLevel];
   struct gl_sampler_object *sampler = _mesa_get_samplerobj(ctx, unit);
   uint32_t *surf;
   uint32_t tile_x, tile_y;

   /* BRW_NEW_UNIFORM_BUFFER */
   if (tObj->Target == GL_TEXTURE_BUFFER) {
      brw_update_buffer_texture_surface(ctx, unit, binding_table, surf_index);
      return;
   }

   surf = brw_state_batch(brw, AUB_TRACE_SURFACE_STATE,
			  6 * 4, 32, &binding_table[surf_index]);

   surf[0] = (translate_tex_target(tObj->Target) << BRW_SURFACE_TYPE_SHIFT |
	      BRW_SURFACE_MIPMAPLAYOUT_BELOW << BRW_SURFACE_MIPLAYOUT_SHIFT |
	      BRW_SURFACE_CUBEFACE_ENABLES |
	      (translate_tex_format(brw,
                                    mt->format,
				    tObj->DepthMode,
				    sampler->sRGBDecode) <<
	       BRW_SURFACE_FORMAT_SHIFT));

   surf[1] = intelObj->mt->region->bo->offset + intelObj->mt->offset; /* reloc */
   surf[1] += intel_miptree_get_tile_offsets(intelObj->mt, firstImage->Level, 0,
                                             &tile_x, &tile_y);

   surf[2] = ((intelObj->_MaxLevel - tObj->BaseLevel) << BRW_SURFACE_LOD_SHIFT |
	      (mt->logical_width0 - 1) << BRW_SURFACE_WIDTH_SHIFT |
	      (mt->logical_height0 - 1) << BRW_SURFACE_HEIGHT_SHIFT);

   surf[3] = (brw_get_surface_tiling_bits(intelObj->mt->region->tiling) |
	      (mt->logical_depth0 - 1) << BRW_SURFACE_DEPTH_SHIFT |
	      (intelObj->mt->region->pitch - 1) <<
	      BRW_SURFACE_PITCH_SHIFT);

   surf[4] = brw_get_surface_num_multisamples(intelObj->mt->num_samples);

   assert(brw->has_surface_tile_offset || (tile_x == 0 && tile_y == 0));
   /* Note that the low bits of these fields are missing, so
    * there's the possibility of getting in trouble.
    */
   assert(tile_x % 4 == 0);
   assert(tile_y % 2 == 0);
   surf[5] = ((tile_x / 4) << BRW_SURFACE_X_OFFSET_SHIFT |
	      (tile_y / 2) << BRW_SURFACE_Y_OFFSET_SHIFT |
	      (mt->align_h == 4 ? BRW_SURFACE_VERTICAL_ALIGN_ENABLE : 0));

   /* Emit relocation to surface contents */
   drm_intel_bo_emit_reloc(brw->batch.bo,
			   binding_table[surf_index] + 4,
			   intelObj->mt->region->bo,
                           surf[1] - intelObj->mt->region->bo->offset,
			   I915_GEM_DOMAIN_SAMPLER, 0);
}

/**
 * Create the constant buffer surface.  Vertex/fragment shader constants will be
 * read from this buffer with Data Port Read instructions/messages.
 */
static void
brw_create_constant_surface(struct brw_context *brw,
			    drm_intel_bo *bo,
			    uint32_t offset,
			    uint32_t size,
			    uint32_t *out_offset,
                            bool dword_pitch)
{
   uint32_t stride = dword_pitch ? 4 : 16;
   uint32_t elements = ALIGN(size, stride) / stride;
   const GLint w = elements - 1;
   uint32_t *surf;

   surf = brw_state_batch(brw, AUB_TRACE_SURFACE_STATE,
			  6 * 4, 32, out_offset);

   surf[0] = (BRW_SURFACE_BUFFER << BRW_SURFACE_TYPE_SHIFT |
	      BRW_SURFACE_MIPMAPLAYOUT_BELOW << BRW_SURFACE_MIPLAYOUT_SHIFT |
	      BRW_SURFACEFORMAT_R32G32B32A32_FLOAT << BRW_SURFACE_FORMAT_SHIFT);

   if (brw->gen >= 6)
      surf[0] |= BRW_SURFACE_RC_READ_WRITE;

   surf[1] = bo->offset + offset; /* reloc */

   surf[2] = ((w & 0x7f) << BRW_SURFACE_WIDTH_SHIFT |
	      ((w >> 7) & 0x1fff) << BRW_SURFACE_HEIGHT_SHIFT);

   surf[3] = (((w >> 20) & 0x7f) << BRW_SURFACE_DEPTH_SHIFT |
	      (stride - 1) << BRW_SURFACE_PITCH_SHIFT);

   surf[4] = 0;
   surf[5] = 0;

   /* Emit relocation to surface contents.  The 965 PRM, Volume 4, section
    * 5.1.2 "Data Cache" says: "the data cache does not exist as a separate
    * physical cache.  It is mapped in hardware to the sampler cache."
    */
   drm_intel_bo_emit_reloc(brw->batch.bo,
			   *out_offset + 4,
			   bo, offset,
			   I915_GEM_DOMAIN_SAMPLER, 0);
}

/**
 * Set up a binding table entry for use by stream output logic (transform
 * feedback).
 *
 * buffer_size_minus_1 must me less than BRW_MAX_NUM_BUFFER_ENTRIES.
 */
void
brw_update_sol_surface(struct brw_context *brw,
                       struct gl_buffer_object *buffer_obj,
                       uint32_t *out_offset, unsigned num_vector_components,
                       unsigned stride_dwords, unsigned offset_dwords)
{
   struct intel_buffer_object *intel_bo = intel_buffer_object(buffer_obj);
   drm_intel_bo *bo = intel_bufferobj_buffer(brw, intel_bo, INTEL_WRITE_PART);
   uint32_t *surf = brw_state_batch(brw, AUB_TRACE_SURFACE_STATE, 6 * 4, 32,
                                    out_offset);
   uint32_t pitch_minus_1 = 4*stride_dwords - 1;
   uint32_t offset_bytes = 4 * offset_dwords;
   size_t size_dwords = buffer_obj->Size / 4;
   uint32_t buffer_size_minus_1, width, height, depth, surface_format;

   /* FIXME: can we rely on core Mesa to ensure that the buffer isn't
    * too big to map using a single binding table entry?
    */
   assert((size_dwords - offset_dwords) / stride_dwords
          <= BRW_MAX_NUM_BUFFER_ENTRIES);

   if (size_dwords > offset_dwords + num_vector_components) {
      /* There is room for at least 1 transform feedback output in the buffer.
       * Compute the number of additional transform feedback outputs the
       * buffer has room for.
       */
      buffer_size_minus_1 =
         (size_dwords - offset_dwords - num_vector_components) / stride_dwords;
   } else {
      /* There isn't even room for a single transform feedback output in the
       * buffer.  We can't configure the binding table entry to prevent output
       * entirely; we'll have to rely on the geometry shader to detect
       * overflow.  But to minimize the damage in case of a bug, set up the
       * binding table entry to just allow a single output.
       */
      buffer_size_minus_1 = 0;
   }
   width = buffer_size_minus_1 & 0x7f;
   height = (buffer_size_minus_1 & 0xfff80) >> 7;
   depth = (buffer_size_minus_1 & 0x7f00000) >> 20;

   switch (num_vector_components) {
   case 1:
      surface_format = BRW_SURFACEFORMAT_R32_FLOAT;
      break;
   case 2:
      surface_format = BRW_SURFACEFORMAT_R32G32_FLOAT;
      break;
   case 3:
      surface_format = BRW_SURFACEFORMAT_R32G32B32_FLOAT;
      break;
   case 4:
      surface_format = BRW_SURFACEFORMAT_R32G32B32A32_FLOAT;
      break;
   default:
      assert(!"Invalid vector size for transform feedback output");
      surface_format = BRW_SURFACEFORMAT_R32_FLOAT;
      break;
   }

   surf[0] = BRW_SURFACE_BUFFER << BRW_SURFACE_TYPE_SHIFT |
      BRW_SURFACE_MIPMAPLAYOUT_BELOW << BRW_SURFACE_MIPLAYOUT_SHIFT |
      surface_format << BRW_SURFACE_FORMAT_SHIFT |
      BRW_SURFACE_RC_READ_WRITE;
   surf[1] = bo->offset + offset_bytes; /* reloc */
   surf[2] = (width << BRW_SURFACE_WIDTH_SHIFT |
	      height << BRW_SURFACE_HEIGHT_SHIFT);
   surf[3] = (depth << BRW_SURFACE_DEPTH_SHIFT |
              pitch_minus_1 << BRW_SURFACE_PITCH_SHIFT);
   surf[4] = 0;
   surf[5] = 0;

   /* Emit relocation to surface contents. */
   drm_intel_bo_emit_reloc(brw->batch.bo,
			   *out_offset + 4,
			   bo, offset_bytes,
			   I915_GEM_DOMAIN_RENDER, I915_GEM_DOMAIN_RENDER);
}

/* Creates a new WM constant buffer reflecting the current fragment program's
 * constants, if needed by the fragment program.
 *
 * Otherwise, constants go through the CURBEs using the brw_constant_buffer
 * state atom.
 */
static void
brw_upload_wm_pull_constants(struct brw_context *brw)
{
   struct gl_context *ctx = &brw->ctx;
   /* BRW_NEW_FRAGMENT_PROGRAM */
   struct brw_fragment_program *fp =
      (struct brw_fragment_program *) brw->fragment_program;
   struct gl_program_parameter_list *params = fp->program.Base.Parameters;
   const int size = brw->wm.prog_data->nr_pull_params * sizeof(float);
   const int surf_index = SURF_INDEX_FRAG_CONST_BUFFER;
   float *constants;
   unsigned int i;

   _mesa_load_state_parameters(ctx, params);

   /* CACHE_NEW_WM_PROG */
   if (brw->wm.prog_data->nr_pull_params == 0) {
      if (brw->wm.const_bo) {
	 drm_intel_bo_unreference(brw->wm.const_bo);
	 brw->wm.const_bo = NULL;
	 brw->wm.surf_offset[surf_index] = 0;
	 brw->state.dirty.brw |= BRW_NEW_SURFACES;
      }
      return;
   }

   drm_intel_bo_unreference(brw->wm.const_bo);
   brw->wm.const_bo = drm_intel_bo_alloc(brw->bufmgr, "WM const bo",
					 size, 64);

   /* _NEW_PROGRAM_CONSTANTS */
   drm_intel_gem_bo_map_gtt(brw->wm.const_bo);
   constants = brw->wm.const_bo->virtual;
   for (i = 0; i < brw->wm.prog_data->nr_pull_params; i++) {
      constants[i] = *brw->wm.prog_data->pull_param[i];
   }
   drm_intel_gem_bo_unmap_gtt(brw->wm.const_bo);

   brw->vtbl.create_constant_surface(brw, brw->wm.const_bo, 0, size,
                                     &brw->wm.surf_offset[surf_index],
                                     true);

   brw->state.dirty.brw |= BRW_NEW_SURFACES;
}

const struct brw_tracked_state brw_wm_pull_constants = {
   .dirty = {
      .mesa = (_NEW_PROGRAM_CONSTANTS),
      .brw = (BRW_NEW_BATCH | BRW_NEW_FRAGMENT_PROGRAM),
      .cache = CACHE_NEW_WM_PROG,
   },
   .emit = brw_upload_wm_pull_constants,
};

static void
brw_update_null_renderbuffer_surface(struct brw_context *brw, unsigned int unit)
{
   /* From the Sandy bridge PRM, Vol4 Part1 p71 (Surface Type: Programming
    * Notes):
    *
    *     A null surface will be used in instances where an actual surface is
    *     not bound. When a write message is generated to a null surface, no
    *     actual surface is written to. When a read message (including any
    *     sampling engine message) is generated to a null surface, the result
    *     is all zeros. Note that a null surface type is allowed to be used
    *     with all messages, even if it is not specificially indicated as
    *     supported. All of the remaining fields in surface state are ignored
    *     for null surfaces, with the following exceptions:
    *
    *     - [DevSNB+]: Width, Height, Depth, and LOD fields must match the
    *       depth buffer’s corresponding state for all render target surfaces,
    *       including null.
    *
    *     - Surface Format must be R8G8B8A8_UNORM.
    */
   struct gl_context *ctx = &brw->ctx;
   uint32_t *surf;
   unsigned surface_type = BRW_SURFACE_NULL;
   drm_intel_bo *bo = NULL;
   unsigned pitch_minus_1 = 0;
   uint32_t multisampling_state = 0;

   /* _NEW_BUFFERS */
   const struct gl_framebuffer *fb = ctx->DrawBuffer;

   surf = brw_state_batch(brw, AUB_TRACE_SURFACE_STATE,
			  6 * 4, 32, &brw->wm.surf_offset[unit]);

   if (fb->Visual.samples > 1) {
      /* On Gen6, null render targets seem to cause GPU hangs when
       * multisampling.  So work around this problem by rendering into dummy
       * color buffer.
       *
       * To decrease the amount of memory needed by the workaround buffer, we
       * set its pitch to 128 bytes (the width of a Y tile).  This means that
       * the amount of memory needed for the workaround buffer is
       * (width_in_tiles + height_in_tiles - 1) tiles.
       *
       * Note that since the workaround buffer will be interpreted by the
       * hardware as an interleaved multisampled buffer, we need to compute
       * width_in_tiles and height_in_tiles by dividing the width and height
       * by 16 rather than the normal Y-tile size of 32.
       */
      unsigned width_in_tiles = ALIGN(fb->Width, 16) / 16;
      unsigned height_in_tiles = ALIGN(fb->Height, 16) / 16;
      unsigned size_needed = (width_in_tiles + height_in_tiles - 1) * 4096;
      brw_get_scratch_bo(brw, &brw->wm.multisampled_null_render_target_bo,
                         size_needed);
      bo = brw->wm.multisampled_null_render_target_bo;
      surface_type = BRW_SURFACE_2D;
      pitch_minus_1 = 127;
      multisampling_state =
         brw_get_surface_num_multisamples(fb->Visual.samples);
   }

   surf[0] = (surface_type << BRW_SURFACE_TYPE_SHIFT |
	      BRW_SURFACEFORMAT_B8G8R8A8_UNORM << BRW_SURFACE_FORMAT_SHIFT);
   if (brw->gen < 6) {
      surf[0] |= (1 << BRW_SURFACE_WRITEDISABLE_R_SHIFT |
		  1 << BRW_SURFACE_WRITEDISABLE_G_SHIFT |
		  1 << BRW_SURFACE_WRITEDISABLE_B_SHIFT |
		  1 << BRW_SURFACE_WRITEDISABLE_A_SHIFT);
   }
   surf[1] = bo ? bo->offset : 0;
   surf[2] = ((fb->Width - 1) << BRW_SURFACE_WIDTH_SHIFT |
              (fb->Height - 1) << BRW_SURFACE_HEIGHT_SHIFT);

   /* From Sandy bridge PRM, Vol4 Part1 p82 (Tiled Surface: Programming
    * Notes):
    *
    *     If Surface Type is SURFTYPE_NULL, this field must be TRUE
    */
   surf[3] = (BRW_SURFACE_TILED | BRW_SURFACE_TILED_Y |
              pitch_minus_1 << BRW_SURFACE_PITCH_SHIFT);
   surf[4] = multisampling_state;
   surf[5] = 0;

   if (bo) {
      drm_intel_bo_emit_reloc(brw->batch.bo,
                              brw->wm.surf_offset[unit] + 4,
                              bo, 0,
                              I915_GEM_DOMAIN_RENDER, I915_GEM_DOMAIN_RENDER);
   }
}

/**
 * Sets up a surface state structure to point at the given region.
 * While it is only used for the front/back buffer currently, it should be
 * usable for further buffers when doing ARB_draw_buffer support.
 */
static void
brw_update_renderbuffer_surface(struct brw_context *brw,
				struct gl_renderbuffer *rb,
				bool layered,
				unsigned int unit)
{
   struct gl_context *ctx = &brw->ctx;
   struct intel_renderbuffer *irb = intel_renderbuffer(rb);
   struct intel_mipmap_tree *mt = irb->mt;
   struct intel_region *region;
   uint32_t *surf;
   uint32_t tile_x, tile_y;
   uint32_t format = 0;
   /* _NEW_BUFFERS */
   gl_format rb_format = _mesa_get_render_format(ctx, intel_rb_format(irb));

   assert(!layered);

   if (rb->TexImage && !brw->has_surface_tile_offset) {
      intel_renderbuffer_get_tile_offsets(irb, &tile_x, &tile_y);

      if (tile_x != 0 || tile_y != 0) {
	 /* Original gen4 hardware couldn't draw to a non-tile-aligned
	  * destination in a miptree unless you actually setup your renderbuffer
	  * as a miptree and used the fragile lod/array_index/etc. controls to
	  * select the image.  So, instead, we just make a new single-level
	  * miptree and render into that.
	  */
	 intel_renderbuffer_move_to_temp(brw, irb, false);
	 mt = irb->mt;
      }
   }

   intel_miptree_used_for_rendering(irb->mt);

   region = irb->mt->region;

   surf = brw_state_batch(brw, AUB_TRACE_SURFACE_STATE,
			  6 * 4, 32, &brw->wm.surf_offset[unit]);

   format = brw->render_target_format[rb_format];
   if (unlikely(!brw->format_supported_as_render_target[rb_format])) {
      _mesa_problem(ctx, "%s: renderbuffer format %s unsupported\n",
                    __FUNCTION__, _mesa_get_format_name(rb_format));
   }

   surf[0] = (BRW_SURFACE_2D << BRW_SURFACE_TYPE_SHIFT |
	      format << BRW_SURFACE_FORMAT_SHIFT);

   /* reloc */
   surf[1] = (intel_renderbuffer_get_tile_offsets(irb, &tile_x, &tile_y) +
	      region->bo->offset);

   surf[2] = ((rb->Width - 1) << BRW_SURFACE_WIDTH_SHIFT |
	      (rb->Height - 1) << BRW_SURFACE_HEIGHT_SHIFT);

   surf[3] = (brw_get_surface_tiling_bits(region->tiling) |
	      (region->pitch - 1) << BRW_SURFACE_PITCH_SHIFT);

   surf[4] = brw_get_surface_num_multisamples(mt->num_samples);

   assert(brw->has_surface_tile_offset || (tile_x == 0 && tile_y == 0));
   /* Note that the low bits of these fields are missing, so
    * there's the possibility of getting in trouble.
    */
   assert(tile_x % 4 == 0);
   assert(tile_y % 2 == 0);
   surf[5] = ((tile_x / 4) << BRW_SURFACE_X_OFFSET_SHIFT |
	      (tile_y / 2) << BRW_SURFACE_Y_OFFSET_SHIFT |
	      (mt->align_h == 4 ? BRW_SURFACE_VERTICAL_ALIGN_ENABLE : 0));

   if (brw->gen < 6) {
      /* _NEW_COLOR */
      if (!ctx->Color.ColorLogicOpEnabled &&
	  (ctx->Color.BlendEnabled & (1 << unit)))
	 surf[0] |= BRW_SURFACE_BLEND_ENABLED;

      if (!ctx->Color.ColorMask[unit][0])
	 surf[0] |= 1 << BRW_SURFACE_WRITEDISABLE_R_SHIFT;
      if (!ctx->Color.ColorMask[unit][1])
	 surf[0] |= 1 << BRW_SURFACE_WRITEDISABLE_G_SHIFT;
      if (!ctx->Color.ColorMask[unit][2])
	 surf[0] |= 1 << BRW_SURFACE_WRITEDISABLE_B_SHIFT;

      /* As mentioned above, disable writes to the alpha component when the
       * renderbuffer is XRGB.
       */
      if (ctx->DrawBuffer->Visual.alphaBits == 0 ||
	  !ctx->Color.ColorMask[unit][3]) {
	 surf[0] |= 1 << BRW_SURFACE_WRITEDISABLE_A_SHIFT;
      }
   }
#if 0
   printf("brw_update_renderbuffer_surface\n"
          "bind bo(handle=%d format=%d width=%d height=%d\n"
          "pitch=%d, tiling=%d\n"
          "ss[0] %x ss[1] %x ss[2] %x ss[3] %x ss[4] %x ss[5] %x\n",
	      region->bo->handle, format, rb->Width, rb->Height,
          region->pitch, region->tiling,
          surf[0],surf[1],surf[2],surf[3], surf[4],surf[5]);
#endif

   drm_intel_bo_emit_reloc(brw->batch.bo,
			   brw->wm.surf_offset[unit] + 4,
			   region->bo,
			   surf[1] - region->bo->offset,
			   I915_GEM_DOMAIN_RENDER,
			   I915_GEM_DOMAIN_RENDER);
}

/**
 * Construct SURFACE_STATE objects for renderbuffers/draw buffers.
 */
static void
brw_update_renderbuffer_surfaces(struct brw_context *brw)
{
    struct gl_context *ctx = &brw->ctx;
    GLuint i;

   /* _NEW_BUFFERS | _NEW_COLOR */
   /* Update surfaces for drawing buffers */
   if (ctx->DrawBuffer->_NumColorDrawBuffers >= 1) {
      for (i = 0; i < ctx->DrawBuffer->_NumColorDrawBuffers; i++) {
	 if (intel_renderbuffer(ctx->DrawBuffer->_ColorDrawBuffers[i])) {
                brw->vtbl.update_renderbuffer_surface(brw, ctx->DrawBuffer->_ColorDrawBuffers[i],
                                                      ctx->DrawBuffer->Layered, i);
            } else {
                brw->vtbl.update_null_renderbuffer_surface(brw, i);
            }
        }
    } else {
      brw->vtbl.update_null_renderbuffer_surface(brw, 0);
    }
    brw->state.dirty.brw |= BRW_NEW_SURFACES;
}

const struct brw_tracked_state brw_renderbuffer_surfaces = {
   .dirty = {
      .mesa = (_NEW_COLOR |
               _NEW_BUFFERS),
      .brw = BRW_NEW_BATCH,
      .cache = 0
   },
   .emit = brw_update_renderbuffer_surfaces,
};

const struct brw_tracked_state gen6_renderbuffer_surfaces = {
   .dirty = {
      .mesa = _NEW_BUFFERS,
      .brw = BRW_NEW_BATCH,
      .cache = 0
   },
   .emit = brw_update_renderbuffer_surfaces,
};

/**
 * Construct SURFACE_STATE objects for enabled textures.
 */
static void
brw_update_texture_surfaces(struct brw_context *brw)
{
   struct gl_context *ctx = &brw->ctx;

   /* BRW_NEW_VERTEX_PROGRAM and BRW_NEW_FRAGMENT_PROGRAM:
    * Unfortunately, we're stuck using the gl_program structs until the
    * ARB_fragment_program front-end gets converted to GLSL IR.  These
    * have the downside that SamplerUnits is split and only contains the
    * mappings for samplers active in that stage.
    */
   struct gl_program *vs = (struct gl_program *) brw->vertex_program;
   struct gl_program *fs = (struct gl_program *) brw->fragment_program;

   unsigned num_samplers = _mesa_fls(vs->SamplersUsed | fs->SamplersUsed);

   for (unsigned s = 0; s < num_samplers; s++) {
      brw->vs.surf_offset[SURF_INDEX_VS_TEXTURE(s)] = 0;
      brw->wm.surf_offset[SURF_INDEX_TEXTURE(s)] = 0;

      if (vs->SamplersUsed & (1 << s)) {
         const unsigned unit = vs->SamplerUnits[s];

         /* _NEW_TEXTURE */
         if (ctx->Texture.Unit[unit]._ReallyEnabled) {
            brw->vtbl.update_texture_surface(ctx, unit,
                                             brw->vs.surf_offset,
                                             SURF_INDEX_VS_TEXTURE(s));
         }
      }

      if (fs->SamplersUsed & (1 << s)) {
         const unsigned unit = fs->SamplerUnits[s];

         /* _NEW_TEXTURE */
         if (ctx->Texture.Unit[unit]._ReallyEnabled) {
            brw->vtbl.update_texture_surface(ctx, unit,
                                             brw->wm.surf_offset,
                                             SURF_INDEX_TEXTURE(s));
         }
      }
   }

   brw->state.dirty.brw |= BRW_NEW_SURFACES;
}

const struct brw_tracked_state brw_texture_surfaces = {
   .dirty = {
      .mesa = _NEW_TEXTURE,
      .brw = BRW_NEW_BATCH |
             BRW_NEW_UNIFORM_BUFFER |
             BRW_NEW_VERTEX_PROGRAM |
             BRW_NEW_FRAGMENT_PROGRAM,
      .cache = 0
   },
   .emit = brw_update_texture_surfaces,
};

void
brw_upload_ubo_surfaces(struct brw_context *brw,
			struct gl_shader *shader,
			uint32_t *surf_offsets)
{
   struct gl_context *ctx = &brw->ctx;

   if (!shader)
      return;

   for (int i = 0; i < shader->NumUniformBlocks; i++) {
      struct gl_uniform_buffer_binding *binding;
      struct intel_buffer_object *intel_bo;

      binding = &ctx->UniformBufferBindings[shader->UniformBlocks[i].Binding];
      intel_bo = intel_buffer_object(binding->BufferObject);
      drm_intel_bo *bo = intel_bufferobj_buffer(brw, intel_bo, INTEL_READ);

      /* Because behavior for referencing outside of the binding's size in the
       * glBindBufferRange case is undefined, we can just bind the whole buffer
       * glBindBufferBase wants and be a correct implementation.
       */
      brw->vtbl.create_constant_surface(brw, bo, binding->Offset,
                                        bo->size - binding->Offset,
                                        &surf_offsets[i],
                                        shader->Type == GL_FRAGMENT_SHADER);
   }

   if (shader->NumUniformBlocks)
      brw->state.dirty.brw |= BRW_NEW_SURFACES;
}

static void
brw_upload_wm_ubo_surfaces(struct brw_context *brw)
{
   struct gl_context *ctx = &brw->ctx;
   /* _NEW_PROGRAM */
   struct gl_shader_program *prog = ctx->Shader._CurrentFragmentProgram;

   if (!prog)
      return;

   brw_upload_ubo_surfaces(brw, prog->_LinkedShaders[MESA_SHADER_FRAGMENT],
			   &brw->wm.surf_offset[SURF_INDEX_WM_UBO(0)]);
}

const struct brw_tracked_state brw_wm_ubo_surfaces = {
   .dirty = {
      .mesa = _NEW_PROGRAM,
      .brw = BRW_NEW_BATCH | BRW_NEW_UNIFORM_BUFFER,
      .cache = 0,
   },
   .emit = brw_upload_wm_ubo_surfaces,
};

/**
 * Constructs the binding table for the WM surface state, which maps unit
 * numbers to surface state objects.
 */
static void
brw_upload_wm_binding_table(struct brw_context *brw)
{
   uint32_t *bind;
   int i;

   if (INTEL_DEBUG & DEBUG_SHADER_TIME) {
      gen7_create_shader_time_surface(brw, &brw->wm.surf_offset[SURF_INDEX_WM_SHADER_TIME]);
   }

   /* Might want to calculate nr_surfaces first, to avoid taking up so much
    * space for the binding table.
    */
   bind = brw_state_batch(brw, AUB_TRACE_BINDING_TABLE,
			  sizeof(uint32_t) * BRW_MAX_WM_SURFACES,
			  32, &brw->wm.bind_bo_offset);

   /* BRW_NEW_SURFACES */
   for (i = 0; i < BRW_MAX_WM_SURFACES; i++) {
      bind[i] = brw->wm.surf_offset[i];
   }

   brw->state.dirty.brw |= BRW_NEW_PS_BINDING_TABLE;
}

const struct brw_tracked_state brw_wm_binding_table = {
   .dirty = {
      .mesa = 0,
      .brw = (BRW_NEW_BATCH |
	      BRW_NEW_SURFACES),
      .cache = 0
   },
   .emit = brw_upload_wm_binding_table,
};

void
gen4_init_vtable_surface_functions(struct brw_context *brw)
{
   brw->vtbl.update_texture_surface = brw_update_texture_surface;
   brw->vtbl.update_renderbuffer_surface = brw_update_renderbuffer_surface;
   brw->vtbl.update_null_renderbuffer_surface =
      brw_update_null_renderbuffer_surface;
   brw->vtbl.create_constant_surface = brw_create_constant_surface;
}
