/**************************************************************************
 *
 * Copyright 2006 VMware, Inc.
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
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include <GL/gl.h>
#include <GL/internal/dri_interface.h>

#include "intel_batchbuffer.h"
#include "intel_mipmap_tree.h"
#include "intel_resolve_map.h"
#include "intel_tex.h"
#include "intel_blit.h"
#include "intel_fbo.h"

#include "brw_blorp.h"
#include "brw_context.h"

#include "main/enums.h"
#include "main/fbobject.h"
#include "main/formats.h"
#include "main/glformats.h"
#include "main/texcompress_etc.h"
#include "main/teximage.h"
#include "main/streaming-load-memcpy.h"
#include "x86/common_x86_asm.h"

#define FILE_DEBUG_FLAG DEBUG_MIPTREE

static bool
intel_miptree_alloc_mcs(struct brw_context *brw,
                        struct intel_mipmap_tree *mt,
                        GLuint num_samples);

/**
 * Determine which MSAA layout should be used by the MSAA surface being
 * created, based on the chip generation and the surface type.
 */
static enum intel_msaa_layout
compute_msaa_layout(struct brw_context *brw, mesa_format format, GLenum target,
                    bool disable_aux_buffers)
{
   /* Prior to Gen7, all MSAA surfaces used IMS layout. */
   if (brw->gen < 7)
      return INTEL_MSAA_LAYOUT_IMS;

   /* In Gen7, IMS layout is only used for depth and stencil buffers. */
   switch (_mesa_get_format_base_format(format)) {
   case GL_DEPTH_COMPONENT:
   case GL_STENCIL_INDEX:
   case GL_DEPTH_STENCIL:
      return INTEL_MSAA_LAYOUT_IMS;
   default:
      /* From the Ivy Bridge PRM, Vol4 Part1 p77 ("MCS Enable"):
       *
       *   This field must be set to 0 for all SINT MSRTs when all RT channels
       *   are not written
       *
       * In practice this means that we have to disable MCS for all signed
       * integer MSAA buffers.  The alternative, to disable MCS only when one
       * of the render target channels is disabled, is impractical because it
       * would require converting between CMS and UMS MSAA layouts on the fly,
       * which is expensive.
       */
      if (brw->gen == 7 && _mesa_get_format_datatype(format) == GL_INT) {
         return INTEL_MSAA_LAYOUT_UMS;
      } else if (disable_aux_buffers) {
         /* We can't use the CMS layout because it uses an aux buffer, the MCS
          * buffer. So fallback to UMS, which is identical to CMS without the
          * MCS. */
         return INTEL_MSAA_LAYOUT_UMS;
      } else {
         return INTEL_MSAA_LAYOUT_CMS;
      }
   }
}


/**
 * For single-sampled render targets ("non-MSRT"), the MCS buffer is a
 * scaled-down bitfield representation of the color buffer which is capable of
 * recording when blocks of the color buffer are equal to the clear value.
 * This function returns the block size that will be used by the MCS buffer
 * corresponding to a certain color miptree.
 *
 * From the Ivy Bridge PRM, Vol2 Part1 11.7 "MCS Buffer for Render Target(s)",
 * beneath the "Fast Color Clear" bullet (p327):
 *
 *     The following table describes the RT alignment
 *
 *                       Pixels  Lines
 *         TiledY RT CL
 *             bpp
 *              32          8      4
 *              64          4      4
 *             128          2      4
 *         TiledX RT CL
 *             bpp
 *              32         16      2
 *              64          8      2
 *             128          4      2
 *
 * This alignment has the following uses:
 *
 * - For figuring out the size of the MCS buffer.  Each 4k tile in the MCS
 *   buffer contains 128 blocks horizontally and 256 blocks vertically.
 *
 * - For figuring out alignment restrictions for a fast clear operation.  Fast
 *   clear operations must always clear aligned multiples of 16 blocks
 *   horizontally and 32 blocks vertically.
 *
 * - For scaling down the coordinates sent through the render pipeline during
 *   a fast clear.  X coordinates must be scaled down by 8 times the block
 *   width, and Y coordinates by 16 times the block height.
 *
 * - For scaling down the coordinates sent through the render pipeline during
 *   a "Render Target Resolve" operation.  X coordinates must be scaled down
 *   by half the block width, and Y coordinates by half the block height.
 */
void
intel_get_non_msrt_mcs_alignment(struct brw_context *brw,
                                 struct intel_mipmap_tree *mt,
                                 unsigned *width_px, unsigned *height)
{
   switch (mt->tiling) {
   default:
      unreachable("Non-MSRT MCS requires X or Y tiling");
      /* In release builds, fall through */
   case I915_TILING_Y:
      *width_px = 32 / mt->cpp;
      *height = 4;
      break;
   case I915_TILING_X:
      *width_px = 64 / mt->cpp;
      *height = 2;
   }
}


/**
 * For a single-sampled render target ("non-MSRT"), determine if an MCS buffer
 * can be used.
 *
 * From the Ivy Bridge PRM, Vol2 Part1 11.7 "MCS Buffer for Render Target(s)",
 * beneath the "Fast Color Clear" bullet (p326):
 *
 *     - Support is limited to tiled render targets.
 *     - Support is for non-mip-mapped and non-array surface types only.
 *
 * And then later, on p327:
 *
 *     - MCS buffer for non-MSRT is supported only for RT formats 32bpp,
 *       64bpp, and 128bpp.
 */
bool
intel_is_non_msrt_mcs_buffer_supported(struct brw_context *brw,
                                       struct intel_mipmap_tree *mt)
{
   /* MCS support does not exist prior to Gen7 */
   if (brw->gen < 7)
      return false;

   if (mt->disable_aux_buffers)
      return false;

   /* MCS is only supported for color buffers */
   switch (_mesa_get_format_base_format(mt->format)) {
   case GL_DEPTH_COMPONENT:
   case GL_DEPTH_STENCIL:
   case GL_STENCIL_INDEX:
      return false;
   }

   if (mt->tiling != I915_TILING_X &&
       mt->tiling != I915_TILING_Y)
      return false;
   if (mt->cpp != 4 && mt->cpp != 8 && mt->cpp != 16)
      return false;
   if (mt->first_level != 0 || mt->last_level != 0)
      return false;
   if (mt->physical_depth0 != 1)
      return false;

   /* There's no point in using an MCS buffer if the surface isn't in a
    * renderable format.
    */
   if (!brw->format_supported_as_render_target[mt->format])
      return false;

   return true;
}


/**
 * Determine depth format corresponding to a depth+stencil format,
 * for separate stencil.
 */
mesa_format
intel_depth_format_for_depthstencil_format(mesa_format format) {
   switch (format) {
   case MESA_FORMAT_Z24_UNORM_S8_UINT:
      return MESA_FORMAT_Z24_UNORM_X8_UINT;
   case MESA_FORMAT_Z32_FLOAT_S8X24_UINT:
      return MESA_FORMAT_Z_FLOAT32;
   default:
      return format;
   }
}


/**
 * @param for_bo Indicates that the caller is
 *        intel_miptree_create_for_bo(). If true, then do not create
 *        \c stencil_mt.
 */
static struct intel_mipmap_tree *
intel_miptree_create_layout(struct brw_context *brw,
                            GLenum target,
                            mesa_format format,
                            GLuint first_level,
                            GLuint last_level,
                            GLuint width0,
                            GLuint height0,
                            GLuint depth0,
                            bool for_bo,
                            GLuint num_samples,
                            bool force_all_slices_at_each_lod,
                            bool disable_aux_buffers)
{
   struct intel_mipmap_tree *mt = calloc(sizeof(*mt), 1);
   if (!mt)
      return NULL;

   DBG("%s target %s format %s level %d..%d slices %d <-- %p\n", __func__,
       _mesa_lookup_enum_by_nr(target),
       _mesa_get_format_name(format),
       first_level, last_level, depth0, mt);

   if (target == GL_TEXTURE_1D_ARRAY) {
      /* For a 1D Array texture the OpenGL API will treat the height0
       * parameter as the number of array slices. For Intel hardware, we treat
       * the 1D array as a 2D Array with a height of 1.
       *
       * So, when we first come through this path to create a 1D Array
       * texture, height0 stores the number of slices, and depth0 is 1. In
       * this case, we want to swap height0 and depth0.
       *
       * Since some miptrees will be created based on the base miptree, we may
       * come through this path and see height0 as 1 and depth0 being the
       * number of slices. In this case we don't need to do the swap.
       */
      assert(height0 == 1 || depth0 == 1);
      if (height0 > 1) {
         depth0 = height0;
         height0 = 1;
      }
   }

   mt->target = target;
   mt->format = format;
   mt->first_level = first_level;
   mt->last_level = last_level;
   mt->logical_width0 = width0;
   mt->logical_height0 = height0;
   mt->logical_depth0 = depth0;
   mt->fast_clear_state = INTEL_FAST_CLEAR_STATE_NO_MCS;
   mt->disable_aux_buffers = disable_aux_buffers;
   exec_list_make_empty(&mt->hiz_map);

   /* The cpp is bytes per (1, blockheight)-sized block for compressed
    * textures.  This is why you'll see divides by blockheight all over
    */
   unsigned bw, bh;
   _mesa_get_format_block_size(format, &bw, &bh);
   assert(_mesa_get_format_bytes(mt->format) % bw == 0);
   mt->cpp = _mesa_get_format_bytes(mt->format) / bw;

   mt->num_samples = num_samples;
   mt->compressed = _mesa_is_format_compressed(format);
   mt->msaa_layout = INTEL_MSAA_LAYOUT_NONE;
   mt->refcount = 1;

   if (num_samples > 1) {
      /* Adjust width/height/depth for MSAA */
      mt->msaa_layout = compute_msaa_layout(brw, format,
                                            mt->target, mt->disable_aux_buffers);
      if (mt->msaa_layout == INTEL_MSAA_LAYOUT_IMS) {
         /* From the Ivybridge PRM, Volume 1, Part 1, page 108:
          * "If the surface is multisampled and it is a depth or stencil
          *  surface or Multisampled Surface StorageFormat in SURFACE_STATE is
          *  MSFMT_DEPTH_STENCIL, WL and HL must be adjusted as follows before
          *  proceeding:
          *
          *  +----------------------------------------------------------------+
          *  | Num Multisamples |        W_l =         |        H_l =         |
          *  +----------------------------------------------------------------+
          *  |         2        | ceiling(W_l / 2) * 4 | H_l (no adjustment)  |
          *  |         4        | ceiling(W_l / 2) * 4 | ceiling(H_l / 2) * 4 |
          *  |         8        | ceiling(W_l / 2) * 8 | ceiling(H_l / 2) * 4 |
          *  |        16        | ceiling(W_l / 2) * 8 | ceiling(H_l / 2) * 8 |
          *  +----------------------------------------------------------------+
          * "
          *
          * Note that MSFMT_DEPTH_STENCIL just means the IMS (interleaved)
          * format rather than UMS/CMS (array slices).  The Sandybridge PRM,
          * Volume 1, Part 1, Page 111 has the same formula for 4x MSAA.
          *
          * Another more complicated explanation for these adjustments comes
          * from the Sandybridge PRM, volume 4, part 1, page 31:
          *
          *     "Any of the other messages (sample*, LOD, load4) used with a
          *      (4x) multisampled surface will in-effect sample a surface with
          *      double the height and width as that indicated in the surface
          *      state. Each pixel position on the original-sized surface is
          *      replaced with a 2x2 of samples with the following arrangement:
          *
          *         sample 0 sample 2
          *         sample 1 sample 3"
          *
          * Thus, when sampling from a multisampled texture, it behaves as
          * though the layout in memory for (x,y,sample) is:
          *
          *      (0,0,0) (0,0,2)   (1,0,0) (1,0,2)
          *      (0,0,1) (0,0,3)   (1,0,1) (1,0,3)
          *
          *      (0,1,0) (0,1,2)   (1,1,0) (1,1,2)
          *      (0,1,1) (0,1,3)   (1,1,1) (1,1,3)
          *
          * However, the actual layout of multisampled data in memory is:
          *
          *      (0,0,0) (1,0,0)   (0,0,1) (1,0,1)
          *      (0,1,0) (1,1,0)   (0,1,1) (1,1,1)
          *
          *      (0,0,2) (1,0,2)   (0,0,3) (1,0,3)
          *      (0,1,2) (1,1,2)   (0,1,3) (1,1,3)
          *
          * This pattern repeats for each 2x2 pixel block.
          *
          * As a result, when calculating the size of our 4-sample buffer for
          * an odd width or height, we have to align before scaling up because
          * sample 3 is in that bottom right 2x2 block.
          */
         switch (num_samples) {
         case 2:
            assert(brw->gen >= 8);
            width0 = ALIGN(width0, 2) * 2;
            height0 = ALIGN(height0, 2);
            break;
         case 4:
            width0 = ALIGN(width0, 2) * 2;
            height0 = ALIGN(height0, 2) * 2;
            break;
         case 8:
            width0 = ALIGN(width0, 2) * 4;
            height0 = ALIGN(height0, 2) * 2;
            break;
         default:
            /* num_samples should already have been quantized to 0, 1, 2, 4, or
             * 8.
             */
            unreachable("not reached");
         }
      } else {
         /* Non-interleaved */
         depth0 *= num_samples;
      }
   }

   /* Set array_layout to ALL_SLICES_AT_EACH_LOD when array_spacing_lod0 can
    * be used. array_spacing_lod0 is only used for non-IMS MSAA surfaces on
    * Gen 7 and 8. On Gen 8 and 9 this layout is not available but it is still
    * used on Gen8 to make it pick a qpitch value which doesn't include space
    * for the mipmaps. On Gen9 this is not necessary because it will
    * automatically pick a packed qpitch value whenever mt->first_level ==
    * mt->last_level.
    * TODO: can we use it elsewhere?
    * TODO: also disable this on Gen8 and pick the qpitch value like Gen9
    */
   if (brw->gen >= 9) {
      mt->array_layout = ALL_LOD_IN_EACH_SLICE;
   } else {
      switch (mt->msaa_layout) {
      case INTEL_MSAA_LAYOUT_NONE:
      case INTEL_MSAA_LAYOUT_IMS:
         mt->array_layout = ALL_LOD_IN_EACH_SLICE;
         break;
      case INTEL_MSAA_LAYOUT_UMS:
      case INTEL_MSAA_LAYOUT_CMS:
         mt->array_layout = ALL_SLICES_AT_EACH_LOD;
         break;
      }
   }

   if (target == GL_TEXTURE_CUBE_MAP) {
      assert(depth0 == 1);
      depth0 = 6;
   }

   mt->physical_width0 = width0;
   mt->physical_height0 = height0;
   mt->physical_depth0 = depth0;

   if (!for_bo &&
       _mesa_get_format_base_format(format) == GL_DEPTH_STENCIL &&
       (brw->must_use_separate_stencil ||
	(brw->has_separate_stencil &&
         intel_miptree_wants_hiz_buffer(brw, mt)))) {
      const bool force_all_slices_at_each_lod = brw->gen == 6;
      mt->stencil_mt = intel_miptree_create(brw,
                                            mt->target,
                                            MESA_FORMAT_S_UINT8,
                                            mt->first_level,
                                            mt->last_level,
                                            mt->logical_width0,
                                            mt->logical_height0,
                                            mt->logical_depth0,
                                            true,
                                            num_samples,
                                            INTEL_MIPTREE_TILING_ANY,
                                            force_all_slices_at_each_lod);
      if (!mt->stencil_mt) {
	 intel_miptree_release(&mt);
	 return NULL;
      }

      /* Fix up the Z miptree format for how we're splitting out separate
       * stencil.  Gen7 expects there to be no stencil bits in its depth buffer.
       */
      mt->format = intel_depth_format_for_depthstencil_format(mt->format);
      mt->cpp = 4;

      if (format == mt->format) {
         _mesa_problem(NULL, "Unknown format %s in separate stencil mt\n",
                       _mesa_get_format_name(mt->format));
      }
   }

   if (force_all_slices_at_each_lod)
      mt->array_layout = ALL_SLICES_AT_EACH_LOD;

   brw_miptree_layout(brw, mt);

   if (mt->disable_aux_buffers)
      assert(mt->msaa_layout != INTEL_MSAA_LAYOUT_CMS);

   return mt;
}

/**
 * \brief Helper function for intel_miptree_create().
 */
static uint32_t
intel_miptree_choose_tiling(struct brw_context *brw,
                            mesa_format format,
                            uint32_t width0,
                            uint32_t num_samples,
                            enum intel_miptree_tiling_mode requested,
                            struct intel_mipmap_tree *mt)
{
   if (format == MESA_FORMAT_S_UINT8) {
      /* The stencil buffer is W tiled. However, we request from the kernel a
       * non-tiled buffer because the GTT is incapable of W fencing.
       */
      return I915_TILING_NONE;
   }

   /* Some usages may want only one type of tiling, like depth miptrees (Y
    * tiled), or temporary BOs for uploading data once (linear).
    */
   switch (requested) {
   case INTEL_MIPTREE_TILING_ANY:
      break;
   case INTEL_MIPTREE_TILING_Y:
      return I915_TILING_Y;
   case INTEL_MIPTREE_TILING_NONE:
      return I915_TILING_NONE;
   }

   if (num_samples > 1) {
      /* From p82 of the Sandy Bridge PRM, dw3[1] of SURFACE_STATE ("Tiled
       * Surface"):
       *
       *   [DevSNB+]: For multi-sample render targets, this field must be
       *   1. MSRTs can only be tiled.
       *
       * Our usual reason for preferring X tiling (fast blits using the
       * blitting engine) doesn't apply to MSAA, since we'll generally be
       * downsampling or upsampling when blitting between the MSAA buffer
       * and another buffer, and the blitting engine doesn't support that.
       * So use Y tiling, since it makes better use of the cache.
       */
      return I915_TILING_Y;
   }

   GLenum base_format = _mesa_get_format_base_format(format);
   if (base_format == GL_DEPTH_COMPONENT ||
       base_format == GL_DEPTH_STENCIL_EXT)
      return I915_TILING_Y;

   /* 1D textures (and 1D array textures) don't get any benefit from tiling,
    * in fact it leads to a less efficient use of memory space and bandwidth
    * due to tile alignment.
    */
   if (mt->logical_height0 == 1)
      return I915_TILING_NONE;

   int minimum_pitch = mt->total_width * mt->cpp;

   /* If the width is much smaller than a tile, don't bother tiling. */
   if (minimum_pitch < 64)
      return I915_TILING_NONE;

   if (ALIGN(minimum_pitch, 512) >= 32768 ||
       mt->total_width >= 32768 || mt->total_height >= 32768) {
      perf_debug("%dx%d miptree too large to blit, falling back to untiled",
                 mt->total_width, mt->total_height);
      return I915_TILING_NONE;
   }

   /* Pre-gen6 doesn't have BLORP to handle Y-tiling, so use X-tiling. */
   if (brw->gen < 6)
      return I915_TILING_X;

   /* From the Sandybridge PRM, Volume 1, Part 2, page 32:
    * "NOTE: 128BPE Format Color Buffer ( render target ) MUST be either TileX
    *  or Linear."
    * 128 bits per pixel translates to 16 bytes per pixel. This is necessary
    * all the way back to 965, but is permitted on Gen7+.
    */
   if (brw->gen < 7 && mt->cpp >= 16)
      return I915_TILING_X;

   /* From the Ivy Bridge PRM, Vol4 Part1 2.12.2.1 (SURFACE_STATE for most
    * messages), on p64, under the heading "Surface Vertical Alignment":
    *
    *     This field must be set to VALIGN_4 for all tiled Y Render Target
    *     surfaces.
    *
    * So if the surface is renderable and uses a vertical alignment of 2,
    * force it to be X tiled.  This is somewhat conservative (it's possible
    * that the client won't ever render to this surface), but it's difficult
    * to know that ahead of time.  And besides, since we use a vertical
    * alignment of 4 as often as we can, this shouldn't happen very often.
    */
   if (brw->gen == 7 && mt->align_h == 2 &&
       brw->format_supported_as_render_target[format]) {
      return I915_TILING_X;
   }

   return I915_TILING_Y | I915_TILING_X;
}


/**
 * Choose an appropriate uncompressed format for a requested
 * compressed format, if unsupported.
 */
mesa_format
intel_lower_compressed_format(struct brw_context *brw, mesa_format format)
{
   /* No need to lower ETC formats on these platforms,
    * they are supported natively.
    */
   if (brw->gen >= 8 || brw->is_baytrail)
      return format;

   switch (format) {
   case MESA_FORMAT_ETC1_RGB8:
      return MESA_FORMAT_R8G8B8X8_UNORM;
   case MESA_FORMAT_ETC2_RGB8:
      return MESA_FORMAT_R8G8B8X8_UNORM;
   case MESA_FORMAT_ETC2_SRGB8:
   case MESA_FORMAT_ETC2_SRGB8_ALPHA8_EAC:
   case MESA_FORMAT_ETC2_SRGB8_PUNCHTHROUGH_ALPHA1:
      return MESA_FORMAT_B8G8R8A8_SRGB;
   case MESA_FORMAT_ETC2_RGBA8_EAC:
   case MESA_FORMAT_ETC2_RGB8_PUNCHTHROUGH_ALPHA1:
      return MESA_FORMAT_R8G8B8A8_UNORM;
   case MESA_FORMAT_ETC2_R11_EAC:
      return MESA_FORMAT_R_UNORM16;
   case MESA_FORMAT_ETC2_SIGNED_R11_EAC:
      return MESA_FORMAT_R_SNORM16;
   case MESA_FORMAT_ETC2_RG11_EAC:
      return MESA_FORMAT_R16G16_UNORM;
   case MESA_FORMAT_ETC2_SIGNED_RG11_EAC:
      return MESA_FORMAT_R16G16_SNORM;
   default:
      /* Non ETC1 / ETC2 format */
      return format;
   }
}


struct intel_mipmap_tree *
intel_miptree_create(struct brw_context *brw,
		     GLenum target,
		     mesa_format format,
		     GLuint first_level,
		     GLuint last_level,
		     GLuint width0,
		     GLuint height0,
		     GLuint depth0,
		     bool expect_accelerated_upload,
                     GLuint num_samples,
                     enum intel_miptree_tiling_mode requested_tiling,
                     bool force_all_slices_at_each_lod)
{
   struct intel_mipmap_tree *mt;
   mesa_format tex_format = format;
   mesa_format etc_format = MESA_FORMAT_NONE;
   GLuint total_width, total_height;

   format = intel_lower_compressed_format(brw, format);

   etc_format = (format != tex_format) ? tex_format : MESA_FORMAT_NONE;

   mt = intel_miptree_create_layout(brw, target, format,
				      first_level, last_level, width0,
				      height0, depth0,
                                    false, num_samples,
                                    force_all_slices_at_each_lod,
                                    false /*disable_aux_buffers*/);
   /*
    * pitch == 0 || height == 0  indicates the null texture
    */
   if (!mt || !mt->total_width || !mt->total_height) {
      intel_miptree_release(&mt);
      return NULL;
   }

   total_width = mt->total_width;
   total_height = mt->total_height;

   if (format == MESA_FORMAT_S_UINT8) {
      /* Align to size of W tile, 64x64. */
      total_width = ALIGN(total_width, 64);
      total_height = ALIGN(total_height, 64);
   }

   uint32_t tiling = intel_miptree_choose_tiling(brw, format, width0,
                                                 num_samples, requested_tiling,
                                                 mt);
   bool y_or_x = false;

   if (tiling == (I915_TILING_Y | I915_TILING_X)) {
      y_or_x = true;
      mt->tiling = I915_TILING_Y;
   } else {
      mt->tiling = tiling;
   }

   unsigned long pitch;
   mt->etc_format = etc_format;
   mt->bo = drm_intel_bo_alloc_tiled(brw->bufmgr, "miptree",
                                     total_width, total_height, mt->cpp,
                                     &mt->tiling, &pitch,
                                     (expect_accelerated_upload ?
                                      BO_ALLOC_FOR_RENDER : 0));
   mt->pitch = pitch;

   /* If the BO is too large to fit in the aperture, we need to use the
    * BLT engine to support it.  Prior to Sandybridge, the BLT paths can't
    * handle Y-tiling, so we need to fall back to X.
    */
   if (brw->gen < 6 && y_or_x && mt->bo->size >= brw->max_gtt_map_object_size) {
      perf_debug("%dx%d miptree larger than aperture; falling back to X-tiled\n",
                 mt->total_width, mt->total_height);

      mt->tiling = I915_TILING_X;
      drm_intel_bo_unreference(mt->bo);
      mt->bo = drm_intel_bo_alloc_tiled(brw->bufmgr, "miptree",
                                        total_width, total_height, mt->cpp,
                                        &mt->tiling, &pitch,
                                        (expect_accelerated_upload ?
                                         BO_ALLOC_FOR_RENDER : 0));
      mt->pitch = pitch;
   }

   mt->offset = 0;

   if (!mt->bo) {
       intel_miptree_release(&mt);
       return NULL;
   }


   if (mt->msaa_layout == INTEL_MSAA_LAYOUT_CMS) {
      if (!intel_miptree_alloc_mcs(brw, mt, num_samples)) {
         intel_miptree_release(&mt);
         return NULL;
      }
   }

   /* If this miptree is capable of supporting fast color clears, set
    * fast_clear_state appropriately to ensure that fast clears will occur.
    * Allocation of the MCS miptree will be deferred until the first fast
    * clear actually occurs.
    */
   if (intel_is_non_msrt_mcs_buffer_supported(brw, mt))
      mt->fast_clear_state = INTEL_FAST_CLEAR_STATE_RESOLVED;

   return mt;
}

struct intel_mipmap_tree *
intel_miptree_create_for_bo(struct brw_context *brw,
                            drm_intel_bo *bo,
                            mesa_format format,
                            uint32_t offset,
                            uint32_t width,
                            uint32_t height,
                            uint32_t depth,
                            int pitch,
                            bool disable_aux_buffers)
{
   struct intel_mipmap_tree *mt;
   uint32_t tiling, swizzle;
   GLenum target;

   drm_intel_bo_get_tiling(bo, &tiling, &swizzle);

   /* Nothing will be able to use this miptree with the BO if the offset isn't
    * aligned.
    */
   if (tiling != I915_TILING_NONE)
      assert(offset % 4096 == 0);

   /* miptrees can't handle negative pitch.  If you need flipping of images,
    * that's outside of the scope of the mt.
    */
   assert(pitch >= 0);

   target = depth > 1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;

   mt = intel_miptree_create_layout(brw, target, format,
                                    0, 0,
                                    width, height, depth,
                                    true, 0, false,
                                    disable_aux_buffers);
   if (!mt)
      return NULL;

   drm_intel_bo_reference(bo);
   mt->bo = bo;
   mt->pitch = pitch;
   mt->offset = offset;
   mt->tiling = tiling;

   return mt;
}

/**
 * For a singlesample renderbuffer, this simply wraps the given BO with a
 * miptree.
 *
 * For a multisample renderbuffer, this wraps the window system's
 * (singlesample) BO with a singlesample miptree attached to the
 * intel_renderbuffer, then creates a multisample miptree attached to irb->mt
 * that will contain the actual rendering (which is lazily resolved to
 * irb->singlesample_mt).
 */
void
intel_update_winsys_renderbuffer_miptree(struct brw_context *intel,
                                         struct intel_renderbuffer *irb,
                                         drm_intel_bo *bo,
                                         uint32_t width, uint32_t height,
                                         uint32_t pitch)
{
   struct intel_mipmap_tree *singlesample_mt = NULL;
   struct intel_mipmap_tree *multisample_mt = NULL;
   struct gl_renderbuffer *rb = &irb->Base.Base;
   mesa_format format = rb->Format;
   int num_samples = rb->NumSamples;

   /* Only the front and back buffers, which are color buffers, are allocated
    * through the image loader.
    */
   assert(_mesa_get_format_base_format(format) == GL_RGB ||
          _mesa_get_format_base_format(format) == GL_RGBA);

   singlesample_mt = intel_miptree_create_for_bo(intel,
                                                 bo,
                                                 format,
                                                 0,
                                                 width,
                                                 height,
                                                 1,
                                                 pitch,
                                                 false);
   if (!singlesample_mt)
      goto fail;

   /* If this miptree is capable of supporting fast color clears, set
    * mcs_state appropriately to ensure that fast clears will occur.
    * Allocation of the MCS miptree will be deferred until the first fast
    * clear actually occurs.
    */
   if (intel_is_non_msrt_mcs_buffer_supported(intel, singlesample_mt))
      singlesample_mt->fast_clear_state = INTEL_FAST_CLEAR_STATE_RESOLVED;

   if (num_samples == 0) {
      intel_miptree_release(&irb->mt);
      irb->mt = singlesample_mt;

      assert(!irb->singlesample_mt);
   } else {
      intel_miptree_release(&irb->singlesample_mt);
      irb->singlesample_mt = singlesample_mt;

      if (!irb->mt ||
          irb->mt->logical_width0 != width ||
          irb->mt->logical_height0 != height) {
         multisample_mt = intel_miptree_create_for_renderbuffer(intel,
                                                                format,
                                                                width,
                                                                height,
                                                                num_samples);
         if (!multisample_mt)
            goto fail;

         irb->need_downsample = false;
         intel_miptree_release(&irb->mt);
         irb->mt = multisample_mt;
      }
   }
   return;

fail:
   intel_miptree_release(&irb->singlesample_mt);
   intel_miptree_release(&irb->mt);
   return;
}

struct intel_mipmap_tree*
intel_miptree_create_for_renderbuffer(struct brw_context *brw,
                                      mesa_format format,
                                      uint32_t width,
                                      uint32_t height,
                                      uint32_t num_samples)
{
   struct intel_mipmap_tree *mt;
   uint32_t depth = 1;
   bool ok;
   GLenum target = num_samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

   mt = intel_miptree_create(brw, target, format, 0, 0,
			     width, height, depth, true, num_samples,
                             INTEL_MIPTREE_TILING_ANY, false);
   if (!mt)
      goto fail;

   if (intel_miptree_wants_hiz_buffer(brw, mt)) {
      ok = intel_miptree_alloc_hiz(brw, mt);
      if (!ok)
         goto fail;
   }

   return mt;

fail:
   intel_miptree_release(&mt);
   return NULL;
}

void
intel_miptree_reference(struct intel_mipmap_tree **dst,
                        struct intel_mipmap_tree *src)
{
   if (*dst == src)
      return;

   intel_miptree_release(dst);

   if (src) {
      src->refcount++;
      DBG("%s %p refcount now %d\n", __func__, src, src->refcount);
   }

   *dst = src;
}


void
intel_miptree_release(struct intel_mipmap_tree **mt)
{
   if (!*mt)
      return;

   DBG("%s %p refcount will be %d\n", __func__, *mt, (*mt)->refcount - 1);
   if (--(*mt)->refcount <= 0) {
      GLuint i;

      DBG("%s deleting %p\n", __func__, *mt);

      drm_intel_bo_unreference((*mt)->bo);
      intel_miptree_release(&(*mt)->stencil_mt);
      if ((*mt)->hiz_buf) {
         if ((*mt)->hiz_buf->mt)
            intel_miptree_release(&(*mt)->hiz_buf->mt);
         else
            drm_intel_bo_unreference((*mt)->hiz_buf->bo);
         free((*mt)->hiz_buf);
      }
      intel_miptree_release(&(*mt)->mcs_mt);
      intel_resolve_map_clear(&(*mt)->hiz_map);

      for (i = 0; i < MAX_TEXTURE_LEVELS; i++) {
	 free((*mt)->level[i].slice);
      }

      free(*mt);
   }
   *mt = NULL;
}

void
intel_miptree_get_dimensions_for_image(struct gl_texture_image *image,
                                       int *width, int *height, int *depth)
{
   switch (image->TexObject->Target) {
   case GL_TEXTURE_1D_ARRAY:
      *width = image->Width;
      *height = 1;
      *depth = image->Height;
      break;
   default:
      *width = image->Width;
      *height = image->Height;
      *depth = image->Depth;
      break;
   }
}

/**
 * Can the image be pulled into a unified mipmap tree?  This mirrors
 * the completeness test in a lot of ways.
 *
 * Not sure whether I want to pass gl_texture_image here.
 */
bool
intel_miptree_match_image(struct intel_mipmap_tree *mt,
                          struct gl_texture_image *image)
{
   struct intel_texture_image *intelImage = intel_texture_image(image);
   GLuint level = intelImage->base.Base.Level;
   int width, height, depth;

   /* glTexImage* choose the texture object based on the target passed in, and
    * objects can't change targets over their lifetimes, so this should be
    * true.
    */
   assert(image->TexObject->Target == mt->target);

   mesa_format mt_format = mt->format;
   if (mt->format == MESA_FORMAT_Z24_UNORM_X8_UINT && mt->stencil_mt)
      mt_format = MESA_FORMAT_Z24_UNORM_S8_UINT;
   if (mt->format == MESA_FORMAT_Z_FLOAT32 && mt->stencil_mt)
      mt_format = MESA_FORMAT_Z32_FLOAT_S8X24_UINT;
   if (mt->etc_format != MESA_FORMAT_NONE)
      mt_format = mt->etc_format;

   if (image->TexFormat != mt_format)
      return false;

   intel_miptree_get_dimensions_for_image(image, &width, &height, &depth);

   if (mt->target == GL_TEXTURE_CUBE_MAP)
      depth = 6;

   int level_depth = mt->level[level].depth;
   if (mt->num_samples > 1) {
      switch (mt->msaa_layout) {
      case INTEL_MSAA_LAYOUT_NONE:
      case INTEL_MSAA_LAYOUT_IMS:
         break;
      case INTEL_MSAA_LAYOUT_UMS:
      case INTEL_MSAA_LAYOUT_CMS:
         level_depth /= mt->num_samples;
         break;
      }
   }

   /* Test image dimensions against the base level image adjusted for
    * minification.  This will also catch images not present in the
    * tree, changed targets, etc.
    */
   if (width != minify(mt->logical_width0, level - mt->first_level) ||
       height != minify(mt->logical_height0, level - mt->first_level) ||
       depth != level_depth) {
      return false;
   }

   if (image->NumSamples != mt->num_samples)
      return false;

   return true;
}


void
intel_miptree_set_level_info(struct intel_mipmap_tree *mt,
			     GLuint level,
			     GLuint x, GLuint y, GLuint d)
{
   mt->level[level].depth = d;
   mt->level[level].level_x = x;
   mt->level[level].level_y = y;

   DBG("%s level %d, depth %d, offset %d,%d\n", __func__,
       level, d, x, y);

   assert(mt->level[level].slice == NULL);

   mt->level[level].slice = calloc(d, sizeof(*mt->level[0].slice));
   mt->level[level].slice[0].x_offset = mt->level[level].level_x;
   mt->level[level].slice[0].y_offset = mt->level[level].level_y;
}


void
intel_miptree_set_image_offset(struct intel_mipmap_tree *mt,
			       GLuint level, GLuint img,
			       GLuint x, GLuint y)
{
   if (img == 0 && level == 0)
      assert(x == 0 && y == 0);

   assert(img < mt->level[level].depth);

   mt->level[level].slice[img].x_offset = mt->level[level].level_x + x;
   mt->level[level].slice[img].y_offset = mt->level[level].level_y + y;

   DBG("%s level %d img %d pos %d,%d\n",
       __func__, level, img,
       mt->level[level].slice[img].x_offset,
       mt->level[level].slice[img].y_offset);
}

void
intel_miptree_get_image_offset(const struct intel_mipmap_tree *mt,
			       GLuint level, GLuint slice,
			       GLuint *x, GLuint *y)
{
   assert(slice < mt->level[level].depth);

   *x = mt->level[level].slice[slice].x_offset;
   *y = mt->level[level].slice[slice].y_offset;
}

/**
 * This function computes masks that may be used to select the bits of the X
 * and Y coordinates that indicate the offset within a tile.  If the BO is
 * untiled, the masks are set to 0.
 */
void
intel_miptree_get_tile_masks(const struct intel_mipmap_tree *mt,
                             uint32_t *mask_x, uint32_t *mask_y,
                             bool map_stencil_as_y_tiled)
{
   int cpp = mt->cpp;
   uint32_t tiling = mt->tiling;

   if (map_stencil_as_y_tiled)
      tiling = I915_TILING_Y;

   switch (tiling) {
   default:
      unreachable("not reached");
   case I915_TILING_NONE:
      *mask_x = *mask_y = 0;
      break;
   case I915_TILING_X:
      *mask_x = 512 / cpp - 1;
      *mask_y = 7;
      break;
   case I915_TILING_Y:
      *mask_x = 128 / cpp - 1;
      *mask_y = 31;
      break;
   }
}

/**
 * Compute the offset (in bytes) from the start of the BO to the given x
 * and y coordinate.  For tiled BOs, caller must ensure that x and y are
 * multiples of the tile size.
 */
uint32_t
intel_miptree_get_aligned_offset(const struct intel_mipmap_tree *mt,
                                 uint32_t x, uint32_t y,
                                 bool map_stencil_as_y_tiled)
{
   int cpp = mt->cpp;
   uint32_t pitch = mt->pitch;
   uint32_t tiling = mt->tiling;

   if (map_stencil_as_y_tiled) {
      tiling = I915_TILING_Y;

      /* When mapping a W-tiled stencil buffer as Y-tiled, each 64-high W-tile
       * gets transformed into a 32-high Y-tile.  Accordingly, the pitch of
       * the resulting surface is twice the pitch of the original miptree,
       * since each row in the Y-tiled view corresponds to two rows in the
       * actual W-tiled surface.  So we need to correct the pitch before
       * computing the offsets.
       */
      pitch *= 2;
   }

   switch (tiling) {
   default:
      unreachable("not reached");
   case I915_TILING_NONE:
      return y * pitch + x * cpp;
   case I915_TILING_X:
      assert((x % (512 / cpp)) == 0);
      assert((y % 8) == 0);
      return y * pitch + x / (512 / cpp) * 4096;
   case I915_TILING_Y:
      assert((x % (128 / cpp)) == 0);
      assert((y % 32) == 0);
      return y * pitch + x / (128 / cpp) * 4096;
   }
}

/**
 * Rendering with tiled buffers requires that the base address of the buffer
 * be aligned to a page boundary.  For renderbuffers, and sometimes with
 * textures, we may want the surface to point at a texture image level that
 * isn't at a page boundary.
 *
 * This function returns an appropriately-aligned base offset
 * according to the tiling restrictions, plus any required x/y offset
 * from there.
 */
uint32_t
intel_miptree_get_tile_offsets(const struct intel_mipmap_tree *mt,
                               GLuint level, GLuint slice,
                               uint32_t *tile_x,
                               uint32_t *tile_y)
{
   uint32_t x, y;
   uint32_t mask_x, mask_y;

   intel_miptree_get_tile_masks(mt, &mask_x, &mask_y, false);
   intel_miptree_get_image_offset(mt, level, slice, &x, &y);

   *tile_x = x & mask_x;
   *tile_y = y & mask_y;

   return intel_miptree_get_aligned_offset(mt, x & ~mask_x, y & ~mask_y, false);
}

static void
intel_miptree_copy_slice_sw(struct brw_context *brw,
                            struct intel_mipmap_tree *dst_mt,
                            struct intel_mipmap_tree *src_mt,
                            int level,
                            int slice,
                            int width,
                            int height)
{
   void *src, *dst;
   ptrdiff_t src_stride, dst_stride;
   int cpp = dst_mt->cpp;

   intel_miptree_map(brw, src_mt,
                     level, slice,
                     0, 0,
                     width, height,
                     GL_MAP_READ_BIT | BRW_MAP_DIRECT_BIT,
                     &src, &src_stride);

   intel_miptree_map(brw, dst_mt,
                     level, slice,
                     0, 0,
                     width, height,
                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT |
                     BRW_MAP_DIRECT_BIT,
                     &dst, &dst_stride);

   DBG("sw blit %s mt %p %p/%"PRIdPTR" -> %s mt %p %p/%"PRIdPTR" (%dx%d)\n",
       _mesa_get_format_name(src_mt->format),
       src_mt, src, src_stride,
       _mesa_get_format_name(dst_mt->format),
       dst_mt, dst, dst_stride,
       width, height);

   int row_size = cpp * width;
   if (src_stride == row_size &&
       dst_stride == row_size) {
      memcpy(dst, src, row_size * height);
   } else {
      for (int i = 0; i < height; i++) {
         memcpy(dst, src, row_size);
         dst += dst_stride;
         src += src_stride;
      }
   }

   intel_miptree_unmap(brw, dst_mt, level, slice);
   intel_miptree_unmap(brw, src_mt, level, slice);

   /* Don't forget to copy the stencil data over, too.  We could have skipped
    * passing BRW_MAP_DIRECT_BIT, but that would have meant intel_miptree_map
    * shuffling the two data sources in/out of temporary storage instead of
    * the direct mapping we get this way.
    */
   if (dst_mt->stencil_mt) {
      assert(src_mt->stencil_mt);
      intel_miptree_copy_slice_sw(brw, dst_mt->stencil_mt, src_mt->stencil_mt,
                                  level, slice, width, height);
   }
}

static void
intel_miptree_copy_slice(struct brw_context *brw,
			 struct intel_mipmap_tree *dst_mt,
			 struct intel_mipmap_tree *src_mt,
			 int level,
			 int face,
			 int depth)

{
   mesa_format format = src_mt->format;
   uint32_t width = minify(src_mt->physical_width0, level - src_mt->first_level);
   uint32_t height = minify(src_mt->physical_height0, level - src_mt->first_level);
   int slice;

   if (face > 0)
      slice = face;
   else
      slice = depth;

   assert(depth < src_mt->level[level].depth);
   assert(src_mt->format == dst_mt->format);

   if (dst_mt->compressed) {
      height = ALIGN(height, dst_mt->align_h) / dst_mt->align_h;
      width = ALIGN(width, dst_mt->align_w);
   }

   /* If it's a packed depth/stencil buffer with separate stencil, the blit
    * below won't apply since we can't do the depth's Y tiling or the
    * stencil's W tiling in the blitter.
    */
   if (src_mt->stencil_mt) {
      intel_miptree_copy_slice_sw(brw,
                                  dst_mt, src_mt,
                                  level, slice,
                                  width, height);
      return;
   }

   uint32_t dst_x, dst_y, src_x, src_y;
   intel_miptree_get_image_offset(dst_mt, level, slice, &dst_x, &dst_y);
   intel_miptree_get_image_offset(src_mt, level, slice, &src_x, &src_y);

   DBG("validate blit mt %s %p %d,%d/%d -> mt %s %p %d,%d/%d (%dx%d)\n",
       _mesa_get_format_name(src_mt->format),
       src_mt, src_x, src_y, src_mt->pitch,
       _mesa_get_format_name(dst_mt->format),
       dst_mt, dst_x, dst_y, dst_mt->pitch,
       width, height);

   if (!intel_miptree_blit(brw,
                           src_mt, level, slice, 0, 0, false,
                           dst_mt, level, slice, 0, 0, false,
                           width, height, GL_COPY)) {
      perf_debug("miptree validate blit for %s failed\n",
                 _mesa_get_format_name(format));

      intel_miptree_copy_slice_sw(brw, dst_mt, src_mt, level, slice,
                                  width, height);
   }
}

/**
 * Copies the image's current data to the given miptree, and associates that
 * miptree with the image.
 *
 * If \c invalidate is true, then the actual image data does not need to be
 * copied, but the image still needs to be associated to the new miptree (this
 * is set to true if we're about to clear the image).
 */
void
intel_miptree_copy_teximage(struct brw_context *brw,
			    struct intel_texture_image *intelImage,
			    struct intel_mipmap_tree *dst_mt,
                            bool invalidate)
{
   struct intel_mipmap_tree *src_mt = intelImage->mt;
   struct intel_texture_object *intel_obj =
      intel_texture_object(intelImage->base.Base.TexObject);
   int level = intelImage->base.Base.Level;
   int face = intelImage->base.Base.Face;

   GLuint depth;
   if (intel_obj->base.Target == GL_TEXTURE_1D_ARRAY)
      depth = intelImage->base.Base.Height;
   else
      depth = intelImage->base.Base.Depth;

   if (!invalidate) {
      for (int slice = 0; slice < depth; slice++) {
         intel_miptree_copy_slice(brw, dst_mt, src_mt, level, face, slice);
      }
   }

   intel_miptree_reference(&intelImage->mt, dst_mt);
   intel_obj->needs_validate = true;
}

static bool
intel_miptree_alloc_mcs(struct brw_context *brw,
                        struct intel_mipmap_tree *mt,
                        GLuint num_samples)
{
   assert(brw->gen >= 7); /* MCS only used on Gen7+ */
   assert(mt->mcs_mt == NULL);
   assert(!mt->disable_aux_buffers);

   /* Choose the correct format for the MCS buffer.  All that really matters
    * is that we allocate the right buffer size, since we'll always be
    * accessing this miptree using MCS-specific hardware mechanisms, which
    * infer the correct format based on num_samples.
    */
   mesa_format format;
   switch (num_samples) {
   case 2:
   case 4:
      /* 8 bits/pixel are required for MCS data when using 4x MSAA (2 bits for
       * each sample).
       */
      format = MESA_FORMAT_R_UNORM8;
      break;
   case 8:
      /* 32 bits/pixel are required for MCS data when using 8x MSAA (3 bits
       * for each sample, plus 8 padding bits).
       */
      format = MESA_FORMAT_R_UINT32;
      break;
   default:
      unreachable("Unrecognized sample count in intel_miptree_alloc_mcs");
   };

   /* From the Ivy Bridge PRM, Vol4 Part1 p76, "MCS Base Address":
    *
    *     "The MCS surface must be stored as Tile Y."
    */
   mt->mcs_mt = intel_miptree_create(brw,
                                     mt->target,
                                     format,
                                     mt->first_level,
                                     mt->last_level,
                                     mt->logical_width0,
                                     mt->logical_height0,
                                     mt->logical_depth0,
                                     true,
                                     0 /* num_samples */,
                                     INTEL_MIPTREE_TILING_Y,
                                     false);

   /* From the Ivy Bridge PRM, Vol 2 Part 1 p326:
    *
    *     When MCS buffer is enabled and bound to MSRT, it is required that it
    *     is cleared prior to any rendering.
    *
    * Since we don't use the MCS buffer for any purpose other than rendering,
    * it makes sense to just clear it immediately upon allocation.
    *
    * Note: the clear value for MCS buffers is all 1's, so we memset to 0xff.
    */
   void *data = intel_miptree_map_raw(brw, mt->mcs_mt);
   memset(data, 0xff, mt->mcs_mt->total_height * mt->mcs_mt->pitch);
   intel_miptree_unmap_raw(brw, mt->mcs_mt);
   mt->fast_clear_state = INTEL_FAST_CLEAR_STATE_CLEAR;

   return mt->mcs_mt;
}


bool
intel_miptree_alloc_non_msrt_mcs(struct brw_context *brw,
                                 struct intel_mipmap_tree *mt)
{
   assert(mt->mcs_mt == NULL);
   assert(!mt->disable_aux_buffers);

   /* The format of the MCS buffer is opaque to the driver; all that matters
    * is that we get its size and pitch right.  We'll pretend that the format
    * is R32.  Since an MCS tile covers 128 blocks horizontally, and a Y-tiled
    * R32 buffer is 32 pixels across, we'll need to scale the width down by
    * the block width and then a further factor of 4.  Since an MCS tile
    * covers 256 blocks vertically, and a Y-tiled R32 buffer is 32 rows high,
    * we'll need to scale the height down by the block height and then a
    * further factor of 8.
    */
   const mesa_format format = MESA_FORMAT_R_UINT32;
   unsigned block_width_px;
   unsigned block_height;
   intel_get_non_msrt_mcs_alignment(brw, mt, &block_width_px, &block_height);
   unsigned width_divisor = block_width_px * 4;
   unsigned height_divisor = block_height * 8;
   unsigned mcs_width =
      ALIGN(mt->logical_width0, width_divisor) / width_divisor;
   unsigned mcs_height =
      ALIGN(mt->logical_height0, height_divisor) / height_divisor;
   assert(mt->logical_depth0 == 1);
   mt->mcs_mt = intel_miptree_create(brw,
                                     mt->target,
                                     format,
                                     mt->first_level,
                                     mt->last_level,
                                     mcs_width,
                                     mcs_height,
                                     mt->logical_depth0,
                                     true,
                                     0 /* num_samples */,
                                     INTEL_MIPTREE_TILING_Y,
                                     false);

   return mt->mcs_mt;
}


/**
 * Helper for intel_miptree_alloc_hiz() that sets
 * \c mt->level[level].has_hiz. Return true if and only if
 * \c has_hiz was set.
 */
static bool
intel_miptree_level_enable_hiz(struct brw_context *brw,
                               struct intel_mipmap_tree *mt,
                               uint32_t level)
{
   assert(mt->hiz_buf);

   if (brw->gen >= 8 || brw->is_haswell) {
      uint32_t width = minify(mt->physical_width0, level);
      uint32_t height = minify(mt->physical_height0, level);

      /* Disable HiZ for LOD > 0 unless the width is 8 aligned
       * and the height is 4 aligned. This allows our HiZ support
       * to fulfill Haswell restrictions for HiZ ops. For LOD == 0,
       * we can grow the width & height to allow the HiZ op to
       * force the proper size alignments.
       */
      if (level > 0 && ((width & 7) || (height & 3))) {
         DBG("mt %p level %d: HiZ DISABLED\n", mt, level);
         return false;
      }
   }

   DBG("mt %p level %d: HiZ enabled\n", mt, level);
   mt->level[level].has_hiz = true;
   return true;
}


/**
 * Helper for intel_miptree_alloc_hiz() that determines the required hiz
 * buffer dimensions and allocates a bo for the hiz buffer.
 */
static struct intel_miptree_aux_buffer *
intel_gen7_hiz_buf_create(struct brw_context *brw,
                          struct intel_mipmap_tree *mt)
{
   unsigned z_width = mt->logical_width0;
   unsigned z_height = mt->logical_height0;
   const unsigned z_depth = MAX2(mt->logical_depth0, 1);
   unsigned hz_width, hz_height;
   struct intel_miptree_aux_buffer *buf = calloc(sizeof(*buf), 1);

   if (!buf)
      return NULL;

   /* Gen7 PRM Volume 2, Part 1, 11.5.3 "Hierarchical Depth Buffer" documents
    * adjustments required for Z_Height and Z_Width based on multisampling.
    */
   switch (mt->num_samples) {
   case 0:
   case 1:
      break;
   case 2:
   case 4:
      z_width *= 2;
      z_height *= 2;
      break;
   case 8:
      z_width *= 4;
      z_height *= 2;
      break;
   default:
      unreachable("unsupported sample count");
   }

   const unsigned vertical_align = 8; /* 'j' in the docs */
   const unsigned H0 = z_height;
   const unsigned h0 = ALIGN(H0, vertical_align);
   const unsigned h1 = ALIGN(minify(H0, 1), vertical_align);
   const unsigned Z0 = z_depth;

   /* HZ_Width (bytes) = ceiling(Z_Width / 16) * 16 */
   hz_width = ALIGN(z_width, 16);

   if (mt->target == GL_TEXTURE_3D) {
      unsigned H_i = H0;
      unsigned Z_i = Z0;
      hz_height = 0;
      for (int level = mt->first_level; level <= mt->last_level; ++level) {
         unsigned h_i = ALIGN(H_i, vertical_align);
         /* sum(i=0 to m; h_i * max(1, floor(Z_Depth/2**i))) */
         hz_height += h_i * Z_i;
         H_i = minify(H_i, 1);
         Z_i = minify(Z_i, 1);
      }
      /* HZ_Height =
       *    (1/2) * sum(i=0 to m; h_i * max(1, floor(Z_Depth/2**i)))
       */
      hz_height = DIV_ROUND_UP(hz_height, 2);
   } else {
      const unsigned hz_qpitch = h0 + h1 + (12 * vertical_align);
      if (mt->target == GL_TEXTURE_CUBE_MAP_ARRAY ||
          mt->target == GL_TEXTURE_CUBE_MAP) {
         /* HZ_Height (rows) = Ceiling ( ( Q_pitch * Z_depth * 6/2) /8 ) * 8 */
         hz_height = DIV_ROUND_UP(hz_qpitch * Z0 * 6, 2 * 8) * 8;
      } else {
         /* HZ_Height (rows) = Ceiling ( ( Q_pitch * Z_depth/2) /8 ) * 8 */
         hz_height = DIV_ROUND_UP(hz_qpitch * Z0, 2 * 8) * 8;
      }
   }

   unsigned long pitch;
   uint32_t tiling = I915_TILING_Y;
   buf->bo = drm_intel_bo_alloc_tiled(brw->bufmgr, "hiz",
                                      hz_width, hz_height, 1,
                                      &tiling, &pitch,
                                      BO_ALLOC_FOR_RENDER);
   if (!buf->bo) {
      free(buf);
      return NULL;
   } else if (tiling != I915_TILING_Y) {
      drm_intel_bo_unreference(buf->bo);
      free(buf);
      return NULL;
   }

   buf->pitch = pitch;

   return buf;
}


/**
 * Helper for intel_miptree_alloc_hiz() that determines the required hiz
 * buffer dimensions and allocates a bo for the hiz buffer.
 */
static struct intel_miptree_aux_buffer *
intel_gen8_hiz_buf_create(struct brw_context *brw,
                          struct intel_mipmap_tree *mt)
{
   unsigned z_width = mt->logical_width0;
   unsigned z_height = mt->logical_height0;
   const unsigned z_depth = MAX2(mt->logical_depth0, 1);
   unsigned hz_width, hz_height;
   struct intel_miptree_aux_buffer *buf = calloc(sizeof(*buf), 1);

   if (!buf)
      return NULL;

   /* Gen7 PRM Volume 2, Part 1, 11.5.3 "Hierarchical Depth Buffer" documents
    * adjustments required for Z_Height and Z_Width based on multisampling.
    */
   switch (mt->num_samples) {
   case 0:
   case 1:
      break;
   case 2:
   case 4:
      z_width *= 2;
      z_height *= 2;
      break;
   case 8:
      z_width *= 4;
      z_height *= 2;
      break;
   default:
      unreachable("unsupported sample count");
   }

   const unsigned vertical_align = 8; /* 'j' in the docs */
   const unsigned H0 = z_height;
   const unsigned h0 = ALIGN(H0, vertical_align);
   const unsigned h1 = ALIGN(minify(H0, 1), vertical_align);
   const unsigned Z0 = z_depth;

   /* HZ_Width (bytes) = ceiling(Z_Width / 16) * 16 */
   hz_width = ALIGN(z_width, 16);

   unsigned H_i = H0;
   unsigned Z_i = Z0;
   unsigned sum_h_i = 0;
   unsigned hz_height_3d_sum = 0;
   for (int level = mt->first_level; level <= mt->last_level; ++level) {
      unsigned i = level - mt->first_level;
      unsigned h_i = ALIGN(H_i, vertical_align);
      /* sum(i=2 to m; h_i) */
      if (i >= 2) {
         sum_h_i += h_i;
      }
      /* sum(i=0 to m; h_i * max(1, floor(Z_Depth/2**i))) */
      hz_height_3d_sum += h_i * Z_i;
      H_i = minify(H_i, 1);
      Z_i = minify(Z_i, 1);
   }
   /* HZ_QPitch = h0 + max(h1, sum(i=2 to m; h_i)) */
   buf->qpitch = h0 + MAX2(h1, sum_h_i);

   if (mt->target == GL_TEXTURE_3D) {
      /* (1/2) * sum(i=0 to m; h_i * max(1, floor(Z_Depth/2**i))) */
      hz_height = DIV_ROUND_UP(hz_height_3d_sum, 2);
   } else {
      /* HZ_Height (rows) = ceiling( (HZ_QPitch/2)/8) *8 * Z_Depth */
      hz_height = DIV_ROUND_UP(buf->qpitch, 2 * 8) * 8 * Z0;
      if (mt->target == GL_TEXTURE_CUBE_MAP_ARRAY ||
          mt->target == GL_TEXTURE_CUBE_MAP) {
         /* HZ_Height (rows) = ceiling( (HZ_QPitch/2)/8) *8 * 6 * Z_Depth
          *
          * We can can just take our hz_height calculation from above, and
          * multiply by 6 for the cube map and cube map array types.
          */
         hz_height *= 6;
      }
   }

   unsigned long pitch;
   uint32_t tiling = I915_TILING_Y;
   buf->bo = drm_intel_bo_alloc_tiled(brw->bufmgr, "hiz",
                                      hz_width, hz_height, 1,
                                      &tiling, &pitch,
                                      BO_ALLOC_FOR_RENDER);
   if (!buf->bo) {
      free(buf);
      return NULL;
   } else if (tiling != I915_TILING_Y) {
      drm_intel_bo_unreference(buf->bo);
      free(buf);
      return NULL;
   }

   buf->pitch = pitch;

   return buf;
}


static struct intel_miptree_aux_buffer *
intel_hiz_miptree_buf_create(struct brw_context *brw,
                             struct intel_mipmap_tree *mt)
{
   struct intel_miptree_aux_buffer *buf = calloc(sizeof(*buf), 1);
   const bool force_all_slices_at_each_lod = brw->gen == 6;

   if (!buf)
      return NULL;

   buf->mt = intel_miptree_create(brw,
                                  mt->target,
                                  mt->format,
                                  mt->first_level,
                                  mt->last_level,
                                  mt->logical_width0,
                                  mt->logical_height0,
                                  mt->logical_depth0,
                                  true,
                                  mt->num_samples,
                                  INTEL_MIPTREE_TILING_ANY,
                                  force_all_slices_at_each_lod);
   if (!buf->mt) {
      free(buf);
      return NULL;
   }

   buf->bo = buf->mt->bo;
   buf->pitch = buf->mt->pitch;
   buf->qpitch = buf->mt->qpitch;

   return buf;
}

bool
intel_miptree_wants_hiz_buffer(struct brw_context *brw,
                               struct intel_mipmap_tree *mt)
{
   if (!brw->has_hiz)
      return false;

   if (mt->hiz_buf != NULL)
      return false;

   if (mt->disable_aux_buffers)
      return false;

   switch (mt->format) {
   case MESA_FORMAT_Z_FLOAT32:
   case MESA_FORMAT_Z32_FLOAT_S8X24_UINT:
   case MESA_FORMAT_Z24_UNORM_X8_UINT:
   case MESA_FORMAT_Z24_UNORM_S8_UINT:
   case MESA_FORMAT_Z_UNORM16:
      return true;
   default:
      return false;
   }
}

bool
intel_miptree_alloc_hiz(struct brw_context *brw,
			struct intel_mipmap_tree *mt)
{
   assert(mt->hiz_buf == NULL);
   assert(!mt->disable_aux_buffers);

   if (brw->gen == 7) {
      mt->hiz_buf = intel_gen7_hiz_buf_create(brw, mt);
   } else if (brw->gen >= 8) {
      mt->hiz_buf = intel_gen8_hiz_buf_create(brw, mt);
   } else {
      mt->hiz_buf = intel_hiz_miptree_buf_create(brw, mt);
   }

   if (!mt->hiz_buf)
      return false;

   /* Mark that all slices need a HiZ resolve. */
   for (int level = mt->first_level; level <= mt->last_level; ++level) {
      if (!intel_miptree_level_enable_hiz(brw, mt, level))
         continue;

      for (int layer = 0; layer < mt->level[level].depth; ++layer) {
         struct intel_resolve_map *m = malloc(sizeof(struct intel_resolve_map));
         exec_node_init(&m->link);
         m->level = level;
         m->layer = layer;
         m->need = GEN6_HIZ_OP_HIZ_RESOLVE;

         exec_list_push_tail(&mt->hiz_map, &m->link);
      }
   }

   return true;
}

/**
 * Does the miptree slice have hiz enabled?
 */
bool
intel_miptree_level_has_hiz(struct intel_mipmap_tree *mt, uint32_t level)
{
   intel_miptree_check_level_layer(mt, level, 0);
   return mt->level[level].has_hiz;
}

void
intel_miptree_slice_set_needs_hiz_resolve(struct intel_mipmap_tree *mt,
					  uint32_t level,
					  uint32_t layer)
{
   if (!intel_miptree_level_has_hiz(mt, level))
      return;

   intel_resolve_map_set(&mt->hiz_map,
			 level, layer, GEN6_HIZ_OP_HIZ_RESOLVE);
}


void
intel_miptree_slice_set_needs_depth_resolve(struct intel_mipmap_tree *mt,
                                            uint32_t level,
                                            uint32_t layer)
{
   if (!intel_miptree_level_has_hiz(mt, level))
      return;

   intel_resolve_map_set(&mt->hiz_map,
			 level, layer, GEN6_HIZ_OP_DEPTH_RESOLVE);
}

void
intel_miptree_set_all_slices_need_depth_resolve(struct intel_mipmap_tree *mt,
                                                uint32_t level)
{
   uint32_t layer;
   uint32_t end_layer = mt->level[level].depth;

   for (layer = 0; layer < end_layer; layer++) {
      intel_miptree_slice_set_needs_depth_resolve(mt, level, layer);
   }
}

static bool
intel_miptree_slice_resolve(struct brw_context *brw,
			    struct intel_mipmap_tree *mt,
			    uint32_t level,
			    uint32_t layer,
			    enum gen6_hiz_op need)
{
   intel_miptree_check_level_layer(mt, level, layer);

   struct intel_resolve_map *item =
	 intel_resolve_map_get(&mt->hiz_map, level, layer);

   if (!item || item->need != need)
      return false;

   intel_hiz_exec(brw, mt, level, layer, need);
   intel_resolve_map_remove(item);
   return true;
}

bool
intel_miptree_slice_resolve_hiz(struct brw_context *brw,
				struct intel_mipmap_tree *mt,
				uint32_t level,
				uint32_t layer)
{
   return intel_miptree_slice_resolve(brw, mt, level, layer,
				      GEN6_HIZ_OP_HIZ_RESOLVE);
}

bool
intel_miptree_slice_resolve_depth(struct brw_context *brw,
				  struct intel_mipmap_tree *mt,
				  uint32_t level,
				  uint32_t layer)
{
   return intel_miptree_slice_resolve(brw, mt, level, layer,
				      GEN6_HIZ_OP_DEPTH_RESOLVE);
}

static bool
intel_miptree_all_slices_resolve(struct brw_context *brw,
				 struct intel_mipmap_tree *mt,
				 enum gen6_hiz_op need)
{
   bool did_resolve = false;

   foreach_list_typed_safe(struct intel_resolve_map, map, link, &mt->hiz_map) {
      if (map->need != need)
	 continue;

      intel_hiz_exec(brw, mt, map->level, map->layer, need);
      intel_resolve_map_remove(map);
      did_resolve = true;
   }

   return did_resolve;
}

bool
intel_miptree_all_slices_resolve_hiz(struct brw_context *brw,
				     struct intel_mipmap_tree *mt)
{
   return intel_miptree_all_slices_resolve(brw, mt,
					   GEN6_HIZ_OP_HIZ_RESOLVE);
}

bool
intel_miptree_all_slices_resolve_depth(struct brw_context *brw,
				       struct intel_mipmap_tree *mt)
{
   return intel_miptree_all_slices_resolve(brw, mt,
					   GEN6_HIZ_OP_DEPTH_RESOLVE);
}


void
intel_miptree_resolve_color(struct brw_context *brw,
                            struct intel_mipmap_tree *mt)
{
   switch (mt->fast_clear_state) {
   case INTEL_FAST_CLEAR_STATE_NO_MCS:
   case INTEL_FAST_CLEAR_STATE_RESOLVED:
      /* No resolve needed */
      break;
   case INTEL_FAST_CLEAR_STATE_UNRESOLVED:
   case INTEL_FAST_CLEAR_STATE_CLEAR:
      /* Fast color clear resolves only make sense for non-MSAA buffers. */
      if (mt->msaa_layout == INTEL_MSAA_LAYOUT_NONE)
         brw_meta_resolve_color(brw, mt);
      break;
   }
}


/**
 * Make it possible to share the BO backing the given miptree with another
 * process or another miptree.
 *
 * Fast color clears are unsafe with shared buffers, so we need to resolve and
 * then discard the MCS buffer, if present.  We also set the fast_clear_state
 * to INTEL_FAST_CLEAR_STATE_NO_MCS to ensure that no MCS buffer gets
 * allocated in the future.
 */
void
intel_miptree_make_shareable(struct brw_context *brw,
                             struct intel_mipmap_tree *mt)
{
   /* MCS buffers are also used for multisample buffers, but we can't resolve
    * away a multisample MCS buffer because it's an integral part of how the
    * pixel data is stored.  Fortunately this code path should never be
    * reached for multisample buffers.
    */
   assert(mt->msaa_layout == INTEL_MSAA_LAYOUT_NONE);

   if (mt->mcs_mt) {
      intel_miptree_resolve_color(brw, mt);
      intel_miptree_release(&mt->mcs_mt);
      mt->fast_clear_state = INTEL_FAST_CLEAR_STATE_NO_MCS;
   }
}


/**
 * \brief Get pointer offset into stencil buffer.
 *
 * The stencil buffer is W tiled. Since the GTT is incapable of W fencing, we
 * must decode the tile's layout in software.
 *
 * See
 *   - PRM, 2011 Sandy Bridge, Volume 1, Part 2, Section 4.5.2.1 W-Major Tile
 *     Format.
 *   - PRM, 2011 Sandy Bridge, Volume 1, Part 2, Section 4.5.3 Tiling Algorithm
 *
 * Even though the returned offset is always positive, the return type is
 * signed due to
 *    commit e8b1c6d6f55f5be3bef25084fdd8b6127517e137
 *    mesa: Fix return type of  _mesa_get_format_bytes() (#37351)
 */
static intptr_t
intel_offset_S8(uint32_t stride, uint32_t x, uint32_t y, bool swizzled)
{
   uint32_t tile_size = 4096;
   uint32_t tile_width = 64;
   uint32_t tile_height = 64;
   uint32_t row_size = 64 * stride;

   uint32_t tile_x = x / tile_width;
   uint32_t tile_y = y / tile_height;

   /* The byte's address relative to the tile's base addres. */
   uint32_t byte_x = x % tile_width;
   uint32_t byte_y = y % tile_height;

   uintptr_t u = tile_y * row_size
               + tile_x * tile_size
               + 512 * (byte_x / 8)
               +  64 * (byte_y / 8)
               +  32 * ((byte_y / 4) % 2)
               +  16 * ((byte_x / 4) % 2)
               +   8 * ((byte_y / 2) % 2)
               +   4 * ((byte_x / 2) % 2)
               +   2 * (byte_y % 2)
               +   1 * (byte_x % 2);

   if (swizzled) {
      /* adjust for bit6 swizzling */
      if (((byte_x / 8) % 2) == 1) {
	 if (((byte_y / 8) % 2) == 0) {
	    u += 64;
	 } else {
	    u -= 64;
	 }
      }
   }

   return u;
}

void
intel_miptree_updownsample(struct brw_context *brw,
                           struct intel_mipmap_tree *src,
                           struct intel_mipmap_tree *dst)
{
   if (brw->gen < 8) {
      brw_blorp_blit_miptrees(brw,
                              src, 0 /* level */, 0 /* layer */, src->format,
                              dst, 0 /* level */, 0 /* layer */, dst->format,
                              0, 0,
                              src->logical_width0, src->logical_height0,
                              0, 0,
                              dst->logical_width0, dst->logical_height0,
                              GL_NEAREST, false, false /*mirror x, y*/);
   } else if (src->format == MESA_FORMAT_S_UINT8) {
      brw_meta_stencil_updownsample(brw, src, dst);
   } else {
      brw_meta_updownsample(brw, src, dst);
   }

   if (src->stencil_mt) {
      if (brw->gen >= 8) {
         brw_meta_stencil_updownsample(brw, src->stencil_mt, dst);
         return;
      }

      brw_blorp_blit_miptrees(brw,
                              src->stencil_mt, 0 /* level */, 0 /* layer */,
                              src->stencil_mt->format,
                              dst->stencil_mt, 0 /* level */, 0 /* layer */,
                              dst->stencil_mt->format,
                              0, 0,
                              src->logical_width0, src->logical_height0,
                              0, 0,
                              dst->logical_width0, dst->logical_height0,
                              GL_NEAREST, false, false /*mirror x, y*/);
   }
}

void *
intel_miptree_map_raw(struct brw_context *brw, struct intel_mipmap_tree *mt)
{
   /* CPU accesses to color buffers don't understand fast color clears, so
    * resolve any pending fast color clears before we map.
    */
   intel_miptree_resolve_color(brw, mt);

   drm_intel_bo *bo = mt->bo;

   if (drm_intel_bo_references(brw->batch.bo, bo))
      intel_batchbuffer_flush(brw);

   if (mt->tiling != I915_TILING_NONE)
      brw_bo_map_gtt(brw, bo, "miptree");
   else
      brw_bo_map(brw, bo, true, "miptree");

   return bo->virtual;
}

void
intel_miptree_unmap_raw(struct brw_context *brw,
                        struct intel_mipmap_tree *mt)
{
   drm_intel_bo_unmap(mt->bo);
}

static void
intel_miptree_map_gtt(struct brw_context *brw,
		      struct intel_mipmap_tree *mt,
		      struct intel_miptree_map *map,
		      unsigned int level, unsigned int slice)
{
   unsigned int bw, bh;
   void *base;
   unsigned int image_x, image_y;
   intptr_t x = map->x;
   intptr_t y = map->y;

   /* For compressed formats, the stride is the number of bytes per
    * row of blocks.  intel_miptree_get_image_offset() already does
    * the divide.
    */
   _mesa_get_format_block_size(mt->format, &bw, &bh);
   assert(y % bh == 0);
   y /= bh;

   base = intel_miptree_map_raw(brw, mt) + mt->offset;

   if (base == NULL)
      map->ptr = NULL;
   else {
      /* Note that in the case of cube maps, the caller must have passed the
       * slice number referencing the face.
      */
      intel_miptree_get_image_offset(mt, level, slice, &image_x, &image_y);
      x += image_x;
      y += image_y;

      map->stride = mt->pitch;
      map->ptr = base + y * map->stride + x * mt->cpp;
   }

   DBG("%s: %d,%d %dx%d from mt %p (%s) "
       "%"PRIiPTR",%"PRIiPTR" = %p/%d\n", __func__,
       map->x, map->y, map->w, map->h,
       mt, _mesa_get_format_name(mt->format),
       x, y, map->ptr, map->stride);
}

static void
intel_miptree_unmap_gtt(struct brw_context *brw,
			struct intel_mipmap_tree *mt,
			struct intel_miptree_map *map,
			unsigned int level,
			unsigned int slice)
{
   intel_miptree_unmap_raw(brw, mt);
}

static void
intel_miptree_map_blit(struct brw_context *brw,
		       struct intel_mipmap_tree *mt,
		       struct intel_miptree_map *map,
		       unsigned int level, unsigned int slice)
{
   map->mt = intel_miptree_create(brw, GL_TEXTURE_2D, mt->format,
                                  0, 0,
                                  map->w, map->h, 1,
                                  false, 0,
                                  INTEL_MIPTREE_TILING_NONE,
                                  false);
   if (!map->mt) {
      fprintf(stderr, "Failed to allocate blit temporary\n");
      goto fail;
   }
   map->stride = map->mt->pitch;

   /* One of either READ_BIT or WRITE_BIT or both is set.  READ_BIT implies no
    * INVALIDATE_RANGE_BIT.  WRITE_BIT needs the original values read in unless
    * invalidate is set, since we'll be writing the whole rectangle from our
    * temporary buffer back out.
    */
   if (!(map->mode & GL_MAP_INVALIDATE_RANGE_BIT)) {
      if (!intel_miptree_blit(brw,
                              mt, level, slice,
                              map->x, map->y, false,
                              map->mt, 0, 0,
                              0, 0, false,
                              map->w, map->h, GL_COPY)) {
         fprintf(stderr, "Failed to blit\n");
         goto fail;
      }
   }

   map->ptr = intel_miptree_map_raw(brw, map->mt);

   DBG("%s: %d,%d %dx%d from mt %p (%s) %d,%d = %p/%d\n", __func__,
       map->x, map->y, map->w, map->h,
       mt, _mesa_get_format_name(mt->format),
       level, slice, map->ptr, map->stride);

   return;

fail:
   intel_miptree_release(&map->mt);
   map->ptr = NULL;
   map->stride = 0;
}

static void
intel_miptree_unmap_blit(struct brw_context *brw,
			 struct intel_mipmap_tree *mt,
			 struct intel_miptree_map *map,
			 unsigned int level,
			 unsigned int slice)
{
   struct gl_context *ctx = &brw->ctx;

   intel_miptree_unmap_raw(brw, map->mt);

   if (map->mode & GL_MAP_WRITE_BIT) {
      bool ok = intel_miptree_blit(brw,
                                   map->mt, 0, 0,
                                   0, 0, false,
                                   mt, level, slice,
                                   map->x, map->y, false,
                                   map->w, map->h, GL_COPY);
      WARN_ONCE(!ok, "Failed to blit from linear temporary mapping");
   }

   intel_miptree_release(&map->mt);
}

/**
 * "Map" a buffer by copying it to an untiled temporary using MOVNTDQA.
 */
#if defined(USE_SSE41)
static void
intel_miptree_map_movntdqa(struct brw_context *brw,
                           struct intel_mipmap_tree *mt,
                           struct intel_miptree_map *map,
                           unsigned int level, unsigned int slice)
{
   assert(map->mode & GL_MAP_READ_BIT);
   assert(!(map->mode & GL_MAP_WRITE_BIT));

   DBG("%s: %d,%d %dx%d from mt %p (%s) %d,%d = %p/%d\n", __func__,
       map->x, map->y, map->w, map->h,
       mt, _mesa_get_format_name(mt->format),
       level, slice, map->ptr, map->stride);

   /* Map the original image */
   uint32_t image_x;
   uint32_t image_y;
   intel_miptree_get_image_offset(mt, level, slice, &image_x, &image_y);
   image_x += map->x;
   image_y += map->y;

   void *src = intel_miptree_map_raw(brw, mt);
   if (!src)
      return;
   src += image_y * mt->pitch;
   src += image_x * mt->cpp;

   /* Due to the pixel offsets for the particular image being mapped, our
    * src pointer may not be 16-byte aligned.  However, if the pitch is
    * divisible by 16, then the amount by which it's misaligned will remain
    * consistent from row to row.
    */
   assert((mt->pitch % 16) == 0);
   const int misalignment = ((uintptr_t) src) & 15;

   /* Create an untiled temporary buffer for the mapping. */
   const unsigned width_bytes = _mesa_format_row_stride(mt->format, map->w);

   map->stride = ALIGN(misalignment + width_bytes, 16);

   map->buffer = _mesa_align_malloc(map->stride * map->h, 16);
   /* Offset the destination so it has the same misalignment as src. */
   map->ptr = map->buffer + misalignment;

   assert((((uintptr_t) map->ptr) & 15) == misalignment);

   for (uint32_t y = 0; y < map->h; y++) {
      void *dst_ptr = map->ptr + y * map->stride;
      void *src_ptr = src + y * mt->pitch;

      _mesa_streaming_load_memcpy(dst_ptr, src_ptr, width_bytes);
   }

   intel_miptree_unmap_raw(brw, mt);
}

static void
intel_miptree_unmap_movntdqa(struct brw_context *brw,
                             struct intel_mipmap_tree *mt,
                             struct intel_miptree_map *map,
                             unsigned int level,
                             unsigned int slice)
{
   _mesa_align_free(map->buffer);
   map->buffer = NULL;
   map->ptr = NULL;
}
#endif

static void
intel_miptree_map_s8(struct brw_context *brw,
		     struct intel_mipmap_tree *mt,
		     struct intel_miptree_map *map,
		     unsigned int level, unsigned int slice)
{
   map->stride = map->w;
   map->buffer = map->ptr = malloc(map->stride * map->h);
   if (!map->buffer)
      return;

   /* One of either READ_BIT or WRITE_BIT or both is set.  READ_BIT implies no
    * INVALIDATE_RANGE_BIT.  WRITE_BIT needs the original values read in unless
    * invalidate is set, since we'll be writing the whole rectangle from our
    * temporary buffer back out.
    */
   if (!(map->mode & GL_MAP_INVALIDATE_RANGE_BIT)) {
      uint8_t *untiled_s8_map = map->ptr;
      uint8_t *tiled_s8_map = intel_miptree_map_raw(brw, mt);
      unsigned int image_x, image_y;

      intel_miptree_get_image_offset(mt, level, slice, &image_x, &image_y);

      for (uint32_t y = 0; y < map->h; y++) {
	 for (uint32_t x = 0; x < map->w; x++) {
	    ptrdiff_t offset = intel_offset_S8(mt->pitch,
	                                       x + image_x + map->x,
	                                       y + image_y + map->y,
					       brw->has_swizzling);
	    untiled_s8_map[y * map->w + x] = tiled_s8_map[offset];
	 }
      }

      intel_miptree_unmap_raw(brw, mt);

      DBG("%s: %d,%d %dx%d from mt %p %d,%d = %p/%d\n", __func__,
	  map->x, map->y, map->w, map->h,
	  mt, map->x + image_x, map->y + image_y, map->ptr, map->stride);
   } else {
      DBG("%s: %d,%d %dx%d from mt %p = %p/%d\n", __func__,
	  map->x, map->y, map->w, map->h,
	  mt, map->ptr, map->stride);
   }
}

static void
intel_miptree_unmap_s8(struct brw_context *brw,
		       struct intel_mipmap_tree *mt,
		       struct intel_miptree_map *map,
		       unsigned int level,
		       unsigned int slice)
{
   if (map->mode & GL_MAP_WRITE_BIT) {
      unsigned int image_x, image_y;
      uint8_t *untiled_s8_map = map->ptr;
      uint8_t *tiled_s8_map = intel_miptree_map_raw(brw, mt);

      intel_miptree_get_image_offset(mt, level, slice, &image_x, &image_y);

      for (uint32_t y = 0; y < map->h; y++) {
	 for (uint32_t x = 0; x < map->w; x++) {
	    ptrdiff_t offset = intel_offset_S8(mt->pitch,
	                                       x + map->x,
	                                       y + map->y,
					       brw->has_swizzling);
	    tiled_s8_map[offset] = untiled_s8_map[y * map->w + x];
	 }
      }

      intel_miptree_unmap_raw(brw, mt);
   }

   free(map->buffer);
}

static void
intel_miptree_map_etc(struct brw_context *brw,
                      struct intel_mipmap_tree *mt,
                      struct intel_miptree_map *map,
                      unsigned int level,
                      unsigned int slice)
{
   assert(mt->etc_format != MESA_FORMAT_NONE);
   if (mt->etc_format == MESA_FORMAT_ETC1_RGB8) {
      assert(mt->format == MESA_FORMAT_R8G8B8X8_UNORM);
   }

   assert(map->mode & GL_MAP_WRITE_BIT);
   assert(map->mode & GL_MAP_INVALIDATE_RANGE_BIT);

   map->stride = _mesa_format_row_stride(mt->etc_format, map->w);
   map->buffer = malloc(_mesa_format_image_size(mt->etc_format,
                                                map->w, map->h, 1));
   map->ptr = map->buffer;
}

static void
intel_miptree_unmap_etc(struct brw_context *brw,
                        struct intel_mipmap_tree *mt,
                        struct intel_miptree_map *map,
                        unsigned int level,
                        unsigned int slice)
{
   uint32_t image_x;
   uint32_t image_y;
   intel_miptree_get_image_offset(mt, level, slice, &image_x, &image_y);

   image_x += map->x;
   image_y += map->y;

   uint8_t *dst = intel_miptree_map_raw(brw, mt)
                + image_y * mt->pitch
                + image_x * mt->cpp;

   if (mt->etc_format == MESA_FORMAT_ETC1_RGB8)
      _mesa_etc1_unpack_rgba8888(dst, mt->pitch,
                                 map->ptr, map->stride,
                                 map->w, map->h);
   else
      _mesa_unpack_etc2_format(dst, mt->pitch,
                               map->ptr, map->stride,
                               map->w, map->h, mt->etc_format);

   intel_miptree_unmap_raw(brw, mt);
   free(map->buffer);
}

/**
 * Mapping function for packed depth/stencil miptrees backed by real separate
 * miptrees for depth and stencil.
 *
 * On gen7, and to support HiZ pre-gen7, we have to have the stencil buffer
 * separate from the depth buffer.  Yet at the GL API level, we have to expose
 * packed depth/stencil textures and FBO attachments, and Mesa core expects to
 * be able to map that memory for texture storage and glReadPixels-type
 * operations.  We give Mesa core that access by mallocing a temporary and
 * copying the data between the actual backing store and the temporary.
 */
static void
intel_miptree_map_depthstencil(struct brw_context *brw,
			       struct intel_mipmap_tree *mt,
			       struct intel_miptree_map *map,
			       unsigned int level, unsigned int slice)
{
   struct intel_mipmap_tree *z_mt = mt;
   struct intel_mipmap_tree *s_mt = mt->stencil_mt;
   bool map_z32f_x24s8 = mt->format == MESA_FORMAT_Z_FLOAT32;
   int packed_bpp = map_z32f_x24s8 ? 8 : 4;

   map->stride = map->w * packed_bpp;
   map->buffer = map->ptr = malloc(map->stride * map->h);
   if (!map->buffer)
      return;

   /* One of either READ_BIT or WRITE_BIT or both is set.  READ_BIT implies no
    * INVALIDATE_RANGE_BIT.  WRITE_BIT needs the original values read in unless
    * invalidate is set, since we'll be writing the whole rectangle from our
    * temporary buffer back out.
    */
   if (!(map->mode & GL_MAP_INVALIDATE_RANGE_BIT)) {
      uint32_t *packed_map = map->ptr;
      uint8_t *s_map = intel_miptree_map_raw(brw, s_mt);
      uint32_t *z_map = intel_miptree_map_raw(brw, z_mt);
      unsigned int s_image_x, s_image_y;
      unsigned int z_image_x, z_image_y;

      intel_miptree_get_image_offset(s_mt, level, slice,
				     &s_image_x, &s_image_y);
      intel_miptree_get_image_offset(z_mt, level, slice,
				     &z_image_x, &z_image_y);

      for (uint32_t y = 0; y < map->h; y++) {
	 for (uint32_t x = 0; x < map->w; x++) {
	    int map_x = map->x + x, map_y = map->y + y;
	    ptrdiff_t s_offset = intel_offset_S8(s_mt->pitch,
						 map_x + s_image_x,
						 map_y + s_image_y,
						 brw->has_swizzling);
	    ptrdiff_t z_offset = ((map_y + z_image_y) *
                                  (z_mt->pitch / 4) +
				  (map_x + z_image_x));
	    uint8_t s = s_map[s_offset];
	    uint32_t z = z_map[z_offset];

	    if (map_z32f_x24s8) {
	       packed_map[(y * map->w + x) * 2 + 0] = z;
	       packed_map[(y * map->w + x) * 2 + 1] = s;
	    } else {
	       packed_map[y * map->w + x] = (s << 24) | (z & 0x00ffffff);
	    }
	 }
      }

      intel_miptree_unmap_raw(brw, s_mt);
      intel_miptree_unmap_raw(brw, z_mt);

      DBG("%s: %d,%d %dx%d from z mt %p %d,%d, s mt %p %d,%d = %p/%d\n",
	  __func__,
	  map->x, map->y, map->w, map->h,
	  z_mt, map->x + z_image_x, map->y + z_image_y,
	  s_mt, map->x + s_image_x, map->y + s_image_y,
	  map->ptr, map->stride);
   } else {
      DBG("%s: %d,%d %dx%d from mt %p = %p/%d\n", __func__,
	  map->x, map->y, map->w, map->h,
	  mt, map->ptr, map->stride);
   }
}

static void
intel_miptree_unmap_depthstencil(struct brw_context *brw,
				 struct intel_mipmap_tree *mt,
				 struct intel_miptree_map *map,
				 unsigned int level,
				 unsigned int slice)
{
   struct intel_mipmap_tree *z_mt = mt;
   struct intel_mipmap_tree *s_mt = mt->stencil_mt;
   bool map_z32f_x24s8 = mt->format == MESA_FORMAT_Z_FLOAT32;

   if (map->mode & GL_MAP_WRITE_BIT) {
      uint32_t *packed_map = map->ptr;
      uint8_t *s_map = intel_miptree_map_raw(brw, s_mt);
      uint32_t *z_map = intel_miptree_map_raw(brw, z_mt);
      unsigned int s_image_x, s_image_y;
      unsigned int z_image_x, z_image_y;

      intel_miptree_get_image_offset(s_mt, level, slice,
				     &s_image_x, &s_image_y);
      intel_miptree_get_image_offset(z_mt, level, slice,
				     &z_image_x, &z_image_y);

      for (uint32_t y = 0; y < map->h; y++) {
	 for (uint32_t x = 0; x < map->w; x++) {
	    ptrdiff_t s_offset = intel_offset_S8(s_mt->pitch,
						 x + s_image_x + map->x,
						 y + s_image_y + map->y,
						 brw->has_swizzling);
	    ptrdiff_t z_offset = ((y + z_image_y + map->y) *
                                  (z_mt->pitch / 4) +
				  (x + z_image_x + map->x));

	    if (map_z32f_x24s8) {
	       z_map[z_offset] = packed_map[(y * map->w + x) * 2 + 0];
	       s_map[s_offset] = packed_map[(y * map->w + x) * 2 + 1];
	    } else {
	       uint32_t packed = packed_map[y * map->w + x];
	       s_map[s_offset] = packed >> 24;
	       z_map[z_offset] = packed;
	    }
	 }
      }

      intel_miptree_unmap_raw(brw, s_mt);
      intel_miptree_unmap_raw(brw, z_mt);

      DBG("%s: %d,%d %dx%d from z mt %p (%s) %d,%d, s mt %p %d,%d = %p/%d\n",
	  __func__,
	  map->x, map->y, map->w, map->h,
	  z_mt, _mesa_get_format_name(z_mt->format),
	  map->x + z_image_x, map->y + z_image_y,
	  s_mt, map->x + s_image_x, map->y + s_image_y,
	  map->ptr, map->stride);
   }

   free(map->buffer);
}

/**
 * Create and attach a map to the miptree at (level, slice). Return the
 * attached map.
 */
static struct intel_miptree_map*
intel_miptree_attach_map(struct intel_mipmap_tree *mt,
                         unsigned int level,
                         unsigned int slice,
                         unsigned int x,
                         unsigned int y,
                         unsigned int w,
                         unsigned int h,
                         GLbitfield mode)
{
   struct intel_miptree_map *map = calloc(1, sizeof(*map));

   if (!map)
      return NULL;

   assert(mt->level[level].slice[slice].map == NULL);
   mt->level[level].slice[slice].map = map;

   map->mode = mode;
   map->x = x;
   map->y = y;
   map->w = w;
   map->h = h;

   return map;
}

/**
 * Release the map at (level, slice).
 */
static void
intel_miptree_release_map(struct intel_mipmap_tree *mt,
                         unsigned int level,
                         unsigned int slice)
{
   struct intel_miptree_map **map;

   map = &mt->level[level].slice[slice].map;
   free(*map);
   *map = NULL;
}

static bool
can_blit_slice(struct intel_mipmap_tree *mt,
               unsigned int level, unsigned int slice)
{
   uint32_t image_x;
   uint32_t image_y;
   intel_miptree_get_image_offset(mt, level, slice, &image_x, &image_y);
   if (image_x >= 32768 || image_y >= 32768)
      return false;

   /* See intel_miptree_blit() for details on the 32k pitch limit. */
   if (mt->pitch >= 32768)
      return false;

   return true;
}

static bool
use_intel_mipree_map_blit(struct brw_context *brw,
                          struct intel_mipmap_tree *mt,
                          GLbitfield mode,
                          unsigned int level,
                          unsigned int slice)
{
   if (brw->has_llc &&
      /* It's probably not worth swapping to the blit ring because of
       * all the overhead involved.
       */
       !(mode & GL_MAP_WRITE_BIT) &&
       !mt->compressed &&
       (mt->tiling == I915_TILING_X ||
        /* Prior to Sandybridge, the blitter can't handle Y tiling */
        (brw->gen >= 6 && mt->tiling == I915_TILING_Y)) &&
       can_blit_slice(mt, level, slice))
      return true;

   if (mt->tiling != I915_TILING_NONE &&
       mt->bo->size >= brw->max_gtt_map_object_size) {
      assert(can_blit_slice(mt, level, slice));
      return true;
   }

   return false;
}

/**
 * Parameter \a out_stride has type ptrdiff_t not because the buffer stride may
 * exceed 32 bits but to diminish the likelihood subtle bugs in pointer
 * arithmetic overflow.
 *
 * If you call this function and use \a out_stride, then you're doing pointer
 * arithmetic on \a out_ptr. The type of \a out_stride doesn't prevent all
 * bugs.  The caller must still take care to avoid 32-bit overflow errors in
 * all arithmetic expressions that contain buffer offsets and pixel sizes,
 * which usually have type uint32_t or GLuint.
 */
void
intel_miptree_map(struct brw_context *brw,
                  struct intel_mipmap_tree *mt,
                  unsigned int level,
                  unsigned int slice,
                  unsigned int x,
                  unsigned int y,
                  unsigned int w,
                  unsigned int h,
                  GLbitfield mode,
                  void **out_ptr,
                  ptrdiff_t *out_stride)
{
   struct intel_miptree_map *map;

   assert(mt->num_samples <= 1);

   map = intel_miptree_attach_map(mt, level, slice, x, y, w, h, mode);
   if (!map){
      *out_ptr = NULL;
      *out_stride = 0;
      return;
   }

   intel_miptree_slice_resolve_depth(brw, mt, level, slice);
   if (map->mode & GL_MAP_WRITE_BIT) {
      intel_miptree_slice_set_needs_hiz_resolve(mt, level, slice);
   }

   if (mt->format == MESA_FORMAT_S_UINT8) {
      intel_miptree_map_s8(brw, mt, map, level, slice);
   } else if (mt->etc_format != MESA_FORMAT_NONE &&
              !(mode & BRW_MAP_DIRECT_BIT)) {
      intel_miptree_map_etc(brw, mt, map, level, slice);
   } else if (mt->stencil_mt && !(mode & BRW_MAP_DIRECT_BIT)) {
      intel_miptree_map_depthstencil(brw, mt, map, level, slice);
   } else if (use_intel_mipree_map_blit(brw, mt, mode, level, slice)) {
      intel_miptree_map_blit(brw, mt, map, level, slice);
#if defined(USE_SSE41)
   } else if (!(mode & GL_MAP_WRITE_BIT) && !mt->compressed && cpu_has_sse4_1) {
      intel_miptree_map_movntdqa(brw, mt, map, level, slice);
#endif
   } else {
      intel_miptree_map_gtt(brw, mt, map, level, slice);
   }

   *out_ptr = map->ptr;
   *out_stride = map->stride;

   if (map->ptr == NULL)
      intel_miptree_release_map(mt, level, slice);
}

void
intel_miptree_unmap(struct brw_context *brw,
                    struct intel_mipmap_tree *mt,
                    unsigned int level,
                    unsigned int slice)
{
   struct intel_miptree_map *map = mt->level[level].slice[slice].map;

   assert(mt->num_samples <= 1);

   if (!map)
      return;

   DBG("%s: mt %p (%s) level %d slice %d\n", __func__,
       mt, _mesa_get_format_name(mt->format), level, slice);

   if (mt->format == MESA_FORMAT_S_UINT8) {
      intel_miptree_unmap_s8(brw, mt, map, level, slice);
   } else if (mt->etc_format != MESA_FORMAT_NONE &&
              !(map->mode & BRW_MAP_DIRECT_BIT)) {
      intel_miptree_unmap_etc(brw, mt, map, level, slice);
   } else if (mt->stencil_mt && !(map->mode & BRW_MAP_DIRECT_BIT)) {
      intel_miptree_unmap_depthstencil(brw, mt, map, level, slice);
   } else if (map->mt) {
      intel_miptree_unmap_blit(brw, mt, map, level, slice);
#if defined(USE_SSE41)
   } else if (map->buffer && cpu_has_sse4_1) {
      intel_miptree_unmap_movntdqa(brw, mt, map, level, slice);
#endif
   } else {
      intel_miptree_unmap_gtt(brw, mt, map, level, slice);
   }

   intel_miptree_release_map(mt, level, slice);
}
