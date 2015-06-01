/*
 * Copyright 2006 VMware, Inc.
 * Copyright © 2006 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER(S) AND/OR ITS SUPPLIERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * \file brw_tex_layout.cpp
 *
 * Code to lay out images in a mipmap tree.
 *
 * \author Keith Whitwell <keithw@vmware.com>
 * \author Michel Dänzer <daenzer@vmware.com>
 */

#include "intel_mipmap_tree.h"
#include "brw_context.h"
#include "main/macros.h"
#include "main/glformats.h"

#define FILE_DEBUG_FLAG DEBUG_MIPTREE

static unsigned int
intel_horizontal_texture_alignment_unit(struct brw_context *brw,
                                        struct intel_mipmap_tree *mt)
{
   /**
    * From the "Alignment Unit Size" section of various specs, namely:
    * - Gen3 Spec: "Memory Data Formats" Volume,         Section 1.20.1.4
    * - i965 and G45 PRMs:             Volume 1,         Section 6.17.3.4.
    * - Ironlake and Sandybridge PRMs: Volume 1, Part 1, Section 7.18.3.4
    * - BSpec (for Ivybridge and slight variations in separate stencil)
    *
    * +----------------------------------------------------------------------+
    * |                                        | alignment unit width  ("i") |
    * | Surface Property                       |-----------------------------|
    * |                                        | 915 | 965 | ILK | SNB | IVB |
    * +----------------------------------------------------------------------+
    * | YUV 4:2:2 format                       |  8  |  4  |  4  |  4  |  4  |
    * | BC1-5 compressed format (DXTn/S3TC)    |  4  |  4  |  4  |  4  |  4  |
    * | FXT1  compressed format                |  8  |  8  |  8  |  8  |  8  |
    * | Depth Buffer (16-bit)                  |  4  |  4  |  4  |  4  |  8  |
    * | Depth Buffer (other)                   |  4  |  4  |  4  |  4  |  4  |
    * | Separate Stencil Buffer                | N/A | N/A |  8  |  8  |  8  |
    * | All Others                             |  4  |  4  |  4  |  4  |  4  |
    * +----------------------------------------------------------------------+
    *
    * On IVB+, non-special cases can be overridden by setting the SURFACE_STATE
    * "Surface Horizontal Alignment" field to HALIGN_4 or HALIGN_8.
    */
    if (_mesa_is_format_compressed(mt->format)) {
       /* The hardware alignment requirements for compressed textures
        * happen to match the block boundaries.
        */
      unsigned int i, j;
      _mesa_get_format_block_size(mt->format, &i, &j);

      /* On Gen9+ we can pick our own alignment for compressed textures but it
       * has to be a multiple of the block size. The minimum alignment we can
       * pick is 4 so we effectively have to align to 4 times the block
       * size
       */
      if (brw->gen >= 9)
         return i * 4;
      else
         return i;
    }

   if (mt->format == MESA_FORMAT_S_UINT8)
      return 8;

   if (brw->gen >= 7 && mt->format == MESA_FORMAT_Z_UNORM16)
      return 8;

   if (brw->gen == 8 && mt->mcs_mt && mt->num_samples <= 1)
      return 16;

   return 4;
}

static unsigned int
intel_vertical_texture_alignment_unit(struct brw_context *brw,
                                      mesa_format format, bool multisampled)
{
   /**
    * From the "Alignment Unit Size" section of various specs, namely:
    * - Gen3 Spec: "Memory Data Formats" Volume,         Section 1.20.1.4
    * - i965 and G45 PRMs:             Volume 1,         Section 6.17.3.4.
    * - Ironlake and Sandybridge PRMs: Volume 1, Part 1, Section 7.18.3.4
    * - BSpec (for Ivybridge and slight variations in separate stencil)
    *
    * +----------------------------------------------------------------------+
    * |                                        | alignment unit height ("j") |
    * | Surface Property                       |-----------------------------|
    * |                                        | 915 | 965 | ILK | SNB | IVB |
    * +----------------------------------------------------------------------+
    * | BC1-5 compressed format (DXTn/S3TC)    |  4  |  4  |  4  |  4  |  4  |
    * | FXT1  compressed format                |  4  |  4  |  4  |  4  |  4  |
    * | Depth Buffer                           |  2  |  2  |  2  |  4  |  4  |
    * | Separate Stencil Buffer                | N/A | N/A | N/A |  4  |  8  |
    * | Multisampled (4x or 8x) render target  | N/A | N/A | N/A |  4  |  4  |
    * | All Others                             |  2  |  2  |  2  |  *  |  *  |
    * +----------------------------------------------------------------------+
    *
    * Where "*" means either VALIGN_2 or VALIGN_4 depending on the setting of
    * the SURFACE_STATE "Surface Vertical Alignment" field.
    */
   if (_mesa_is_format_compressed(format))
      /* See comment above for the horizontal alignment */
      return brw->gen >= 9 ? 16 : 4;

   if (format == MESA_FORMAT_S_UINT8)
      return brw->gen >= 7 ? 8 : 4;

   /* Broadwell only supports VALIGN of 4, 8, and 16.  The BSpec says 4
    * should always be used, except for stencil buffers, which should be 8.
    */
   if (brw->gen >= 8)
      return 4;

   if (multisampled)
      return 4;

   GLenum base_format = _mesa_get_format_base_format(format);

   if (brw->gen >= 6 &&
       (base_format == GL_DEPTH_COMPONENT ||
	base_format == GL_DEPTH_STENCIL)) {
      return 4;
   }

   if (brw->gen == 7) {
      /* On Gen7, we prefer a vertical alignment of 4 when possible, because
       * that allows Y tiled render targets.
       *
       * From the Ivy Bridge PRM, Vol4 Part1 2.12.2.1 (SURFACE_STATE for most
       * messages), on p64, under the heading "Surface Vertical Alignment":
       *
       *     Value of 1 [VALIGN_4] is not supported for format YCRCB_NORMAL
       *     (0x182), YCRCB_SWAPUVY (0x183), YCRCB_SWAPUV (0x18f), YCRCB_SWAPY
       *     (0x190)
       *
       *     VALIGN_4 is not supported for surface format R32G32B32_FLOAT.
       */
      if (base_format == GL_YCBCR_MESA || format == MESA_FORMAT_RGB_FLOAT32)
         return 2;

      return 4;
   }

   return 2;
}

static void
gen9_miptree_layout_1d(struct intel_mipmap_tree *mt)
{
   unsigned x = 0;
   unsigned width = mt->physical_width0;
   unsigned depth = mt->physical_depth0; /* number of array layers. */

   /* When this layout is used the horizontal alignment is fixed at 64 and the
    * hardware ignores the value given in the surface state
    */
   const unsigned int align_w = 64;

   mt->total_height = mt->physical_height0;
   mt->total_width = 0;

   for (unsigned level = mt->first_level; level <= mt->last_level; level++) {
      unsigned img_width;

      intel_miptree_set_level_info(mt, level, x, 0, depth);

      img_width = ALIGN(width, align_w);

      mt->total_width = MAX2(mt->total_width, x + img_width);

      x += img_width;

      width = minify(width, 1);
   }
}

static void
brw_miptree_layout_2d(struct intel_mipmap_tree *mt)
{
   unsigned x = 0;
   unsigned y = 0;
   unsigned width = mt->physical_width0;
   unsigned height = mt->physical_height0;
   unsigned depth = mt->physical_depth0; /* number of array layers. */
   unsigned int bw, bh;

   _mesa_get_format_block_size(mt->format, &bw, &bh);

   mt->total_width = mt->physical_width0;

   if (mt->compressed) {
       mt->total_width = ALIGN(mt->physical_width0, mt->align_w);
   }

   /* May need to adjust width to accommodate the placement of
    * the 2nd mipmap.  This occurs when the alignment
    * constraints of mipmap placement push the right edge of the
    * 2nd mipmap out past the width of its parent.
    */
   if (mt->first_level != mt->last_level) {
       unsigned mip1_width;

       if (mt->compressed) {
          mip1_width = ALIGN(minify(mt->physical_width0, 1), mt->align_w) +
             ALIGN(minify(mt->physical_width0, 2), bw);
       } else {
          mip1_width = ALIGN(minify(mt->physical_width0, 1), mt->align_w) +
             minify(mt->physical_width0, 2);
       }

       if (mip1_width > mt->total_width) {
           mt->total_width = mip1_width;
       }
   }

   mt->total_height = 0;

   for (unsigned level = mt->first_level; level <= mt->last_level; level++) {
      unsigned img_height;

      intel_miptree_set_level_info(mt, level, x, y, depth);

      img_height = ALIGN(height, mt->align_h);
      if (mt->compressed)
	 img_height /= bh;

      if (mt->array_layout == ALL_SLICES_AT_EACH_LOD) {
         /* Compact arrays with separated miplevels */
         img_height *= depth;
      }

      /* Because the images are packed better, the final offset
       * might not be the maximal one:
       */
      mt->total_height = MAX2(mt->total_height, y + img_height);

      /* Layout_below: step right after second mipmap.
       */
      if (level == mt->first_level + 1) {
	 x += ALIGN(width, mt->align_w);
      } else {
	 y += img_height;
      }

      width  = minify(width, 1);
      height = minify(height, 1);

      if (mt->target == GL_TEXTURE_3D)
         depth = minify(depth, 1);
   }
}

unsigned
brw_miptree_get_horizontal_slice_pitch(const struct brw_context *brw,
                                       const struct intel_mipmap_tree *mt,
                                       unsigned level)
{
   assert(brw->gen < 9);

   if (mt->target == GL_TEXTURE_3D ||
       (brw->gen == 4 && mt->target == GL_TEXTURE_CUBE_MAP)) {
      return ALIGN(minify(mt->physical_width0, level), mt->align_w);
   } else {
      return 0;
   }
}

unsigned
brw_miptree_get_vertical_slice_pitch(const struct brw_context *brw,
                                     const struct intel_mipmap_tree *mt,
                                     unsigned level)
{
   if (brw->gen >= 9) {
      /* ALL_SLICES_AT_EACH_LOD isn't supported on Gen8+ but this code will
       * effectively end up with a packed qpitch anyway whenever
       * mt->first_level == mt->last_level.
       */
      assert(mt->array_layout != ALL_SLICES_AT_EACH_LOD);

      /* On Gen9 we can pick whatever qpitch we like as long as it's aligned
       * to the vertical alignment so we don't need to add any extra rows.
       */
      unsigned qpitch = mt->total_height;

      /* If the surface might be used as a stencil buffer or HiZ buffer then
       * it needs to be a multiple of 8.
       */
      const GLenum base_format = _mesa_get_format_base_format(mt->format);
      if (_mesa_is_depth_or_stencil_format(base_format))
         qpitch = ALIGN(qpitch, 8);

      /* 3D textures need to be aligned to the tile height. At this point we
       * don't know which tiling will be used so let's just align it to 32
       */
      if (mt->target == GL_TEXTURE_3D)
         qpitch = ALIGN(qpitch, 32);

      return qpitch;

   } else if (mt->target == GL_TEXTURE_3D ||
              (brw->gen == 4 && mt->target == GL_TEXTURE_CUBE_MAP) ||
              mt->array_layout == ALL_SLICES_AT_EACH_LOD) {
      return ALIGN(minify(mt->physical_height0, level), mt->align_h);

   } else {
      const unsigned h0 = ALIGN(mt->physical_height0, mt->align_h);
      const unsigned h1 = ALIGN(minify(mt->physical_height0, 1), mt->align_h);

      return h0 + h1 + (brw->gen >= 7 ? 12 : 11) * mt->align_h;
   }
}

static void
align_cube(struct intel_mipmap_tree *mt)
{
   /* The 965's sampler lays cachelines out according to how accesses
    * in the texture surfaces run, so they may be "vertical" through
    * memory.  As a result, the docs say in Surface Padding Requirements:
    * Sampling Engine Surfaces that two extra rows of padding are required.
    */
   if (mt->target == GL_TEXTURE_CUBE_MAP)
      mt->total_height += 2;
}

static bool
use_linear_1d_layout(struct brw_context *brw,
                     struct intel_mipmap_tree *mt)
{
   /* On Gen9+ the mipmap levels of a 1D surface are all laid out in a
    * horizontal line. This isn't done for depth/stencil buffers however
    * because those will be using a tiled layout
    */
   if (brw->gen >= 9 &&
       (mt->target == GL_TEXTURE_1D ||
        mt->target == GL_TEXTURE_1D_ARRAY)) {
      GLenum base_format = _mesa_get_format_base_format(mt->format);

      if (base_format != GL_DEPTH_COMPONENT &&
          base_format != GL_DEPTH_STENCIL &&
          base_format != GL_STENCIL_INDEX)
         return true;
   }

   return false;
}

static void
brw_miptree_layout_texture_array(struct brw_context *brw,
				 struct intel_mipmap_tree *mt)
{
   unsigned height = mt->physical_height0;
   bool layout_1d = use_linear_1d_layout(brw, mt);
   int physical_qpitch;

   if (layout_1d)
      gen9_miptree_layout_1d(mt);
   else
      brw_miptree_layout_2d(mt);

   if (layout_1d) {
      physical_qpitch = 1;
      /* When using the horizontal layout the qpitch specifies the distance in
       * pixels between array slices. The total_width is forced to be a
       * multiple of the horizontal alignment in brw_miptree_layout_1d (in
       * this case it's always 64). The vertical alignment is ignored.
       */
      mt->qpitch = mt->total_width;
   } else {
      mt->qpitch = brw_miptree_get_vertical_slice_pitch(brw, mt, 0);
      /* Unlike previous generations the qpitch is a multiple of the
       * compressed block size on Gen9 so physical_qpitch matches mt->qpitch.
       */
      physical_qpitch = (mt->compressed && brw->gen < 9 ? mt->qpitch / 4 :
                         mt->qpitch);
   }

   for (unsigned level = mt->first_level; level <= mt->last_level; level++) {
      unsigned img_height;
      img_height = ALIGN(height, mt->align_h);
      if (mt->compressed)
         img_height /= mt->align_h;

      for (int q = 0; q < mt->level[level].depth; q++) {
         if (mt->array_layout == ALL_SLICES_AT_EACH_LOD) {
            intel_miptree_set_image_offset(mt, level, q, 0, q * img_height);
         } else {
            intel_miptree_set_image_offset(mt, level, q, 0, q * physical_qpitch);
         }
      }
      height = minify(height, 1);
   }
   if (mt->array_layout == ALL_LOD_IN_EACH_SLICE)
      mt->total_height = physical_qpitch * mt->physical_depth0;

   align_cube(mt);
}

static void
brw_miptree_layout_texture_3d(struct brw_context *brw,
                              struct intel_mipmap_tree *mt)
{
   unsigned yscale = mt->compressed ? 4 : 1;

   mt->total_width = 0;
   mt->total_height = 0;

   unsigned ysum = 0;
   for (unsigned level = mt->first_level; level <= mt->last_level; level++) {
      unsigned WL = MAX2(mt->physical_width0 >> level, 1);
      unsigned HL = MAX2(mt->physical_height0 >> level, 1);
      unsigned DL = MAX2(mt->physical_depth0 >> level, 1);
      unsigned wL = ALIGN(WL, mt->align_w);
      unsigned hL = ALIGN(HL, mt->align_h);

      if (mt->target == GL_TEXTURE_CUBE_MAP)
         DL = 6;

      intel_miptree_set_level_info(mt, level, 0, 0, DL);

      for (unsigned q = 0; q < DL; q++) {
         unsigned x = (q % (1 << level)) * wL;
         unsigned y = ysum + (q >> level) * hL;

         intel_miptree_set_image_offset(mt, level, q, x, y / yscale);
         mt->total_width = MAX2(mt->total_width, x + wL);
         mt->total_height = MAX2(mt->total_height, (y + hL) / yscale);
      }

      ysum += ALIGN(DL, 1 << level) / (1 << level) * hL;
   }

   align_cube(mt);
}

void
brw_miptree_layout(struct brw_context *brw, struct intel_mipmap_tree *mt)
{
   bool multisampled = mt->num_samples > 1;
   bool gen6_hiz_or_stencil = false;

   if (brw->gen == 6 && mt->array_layout == ALL_SLICES_AT_EACH_LOD) {
      const GLenum base_format = _mesa_get_format_base_format(mt->format);
      gen6_hiz_or_stencil = _mesa_is_depth_or_stencil_format(base_format);
   }

   if (gen6_hiz_or_stencil) {
      /* On gen6, we use ALL_SLICES_AT_EACH_LOD for stencil/hiz because the
       * hardware doesn't support multiple mip levels on stencil/hiz.
       *
       * PRM Vol 2, Part 1, 7.5.3 Hierarchical Depth Buffer:
       * "The hierarchical depth buffer does not support the LOD field"
       *
       * PRM Vol 2, Part 1, 7.5.4.1 Separate Stencil Buffer:
       * "The stencil depth buffer does not support the LOD field"
       */
      if (mt->format == MESA_FORMAT_S_UINT8) {
         /* Stencil uses W tiling, so we force W tiling alignment for the
          * ALL_SLICES_AT_EACH_LOD miptree layout.
          */
         mt->align_w = 64;
         mt->align_h = 64;
      } else {
         /* Depth uses Y tiling, so we force need Y tiling alignment for the
          * ALL_SLICES_AT_EACH_LOD miptree layout.
          */
         mt->align_w = 128 / mt->cpp;
         mt->align_h = 32;
      }
   } else {
      mt->align_w = intel_horizontal_texture_alignment_unit(brw, mt);
      mt->align_h =
         intel_vertical_texture_alignment_unit(brw, mt->format, multisampled);
   }

   switch (mt->target) {
   case GL_TEXTURE_CUBE_MAP:
      if (brw->gen == 4) {
         /* Gen4 stores cube maps as 3D textures. */
         assert(mt->physical_depth0 == 6);
         brw_miptree_layout_texture_3d(brw, mt);
      } else {
         /* All other hardware stores cube maps as 2D arrays. */
	 brw_miptree_layout_texture_array(brw, mt);
      }
      break;

   case GL_TEXTURE_3D:
      if (brw->gen >= 9)
         brw_miptree_layout_texture_array(brw, mt);
      else
         brw_miptree_layout_texture_3d(brw, mt);
      break;

   case GL_TEXTURE_1D_ARRAY:
   case GL_TEXTURE_2D_ARRAY:
   case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
   case GL_TEXTURE_CUBE_MAP_ARRAY:
      brw_miptree_layout_texture_array(brw, mt);
      break;

   default:
      switch (mt->msaa_layout) {
      case INTEL_MSAA_LAYOUT_UMS:
      case INTEL_MSAA_LAYOUT_CMS:
         brw_miptree_layout_texture_array(brw, mt);
         break;
      case INTEL_MSAA_LAYOUT_NONE:
      case INTEL_MSAA_LAYOUT_IMS:
         if (use_linear_1d_layout(brw, mt))
            gen9_miptree_layout_1d(mt);
         else
            brw_miptree_layout_2d(mt);
         break;
      }
      break;
   }
   DBG("%s: %dx%dx%d\n", __func__,
       mt->total_width, mt->total_height, mt->cpp);

   /* On Gen9+ the alignment values are expressed in multiples of the block
    * size
    */
   if (brw->gen >= 9) {
      unsigned int i, j;
      _mesa_get_format_block_size(mt->format, &i, &j);
      mt->align_w /= i;
      mt->align_h /= j;
   }
}

