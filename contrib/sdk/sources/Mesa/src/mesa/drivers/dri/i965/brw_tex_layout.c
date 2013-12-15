/*
 * Copyright 2006 Tungsten Graphics, Inc., Cedar Park, Texas.
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
 * \author Keith Whitwell <keith@tungstengraphics.com>
 * \author Michel Dänzer <michel@tungstengraphics.com>
 */

#include "intel_mipmap_tree.h"
#include "brw_context.h"
#include "main/macros.h"

#define FILE_DEBUG_FLAG DEBUG_MIPTREE

static unsigned int
intel_horizontal_texture_alignment_unit(struct brw_context *brw,
                                       gl_format format)
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
    if (_mesa_is_format_compressed(format)) {
       /* The hardware alignment requirements for compressed textures
        * happen to match the block boundaries.
        */
      unsigned int i, j;
      _mesa_get_format_block_size(format, &i, &j);
      return i;
    }

   if (format == MESA_FORMAT_S8)
      return 8;

   /* The depth alignment requirements in the table above are for rendering to
    * depth miplevels using the LOD control fields.  We don't use LOD control
    * fields, and instead use page offsets plus intra-tile x/y offsets, which
    * require that the low 3 bits are zero.  To reduce the number of x/y
    * offset workaround blits we do, align the X to 8, which depth texturing
    * can handle (sadly, it can't handle 8 in the Y direction).
    */
   if (brw->gen >= 7 &&
       _mesa_get_format_base_format(format) == GL_DEPTH_COMPONENT)
      return 8;

   return 4;
}

static unsigned int
intel_vertical_texture_alignment_unit(struct brw_context *brw,
                                     gl_format format)
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
    * | All Others                             |  2  |  2  |  2  |  2  |  2  |
    * +----------------------------------------------------------------------+
    *
    * On SNB+, non-special cases can be overridden by setting the SURFACE_STATE
    * "Surface Vertical Alignment" field to VALIGN_2 or VALIGN_4.
    *
    * We currently don't support multisampling.
    */
   if (_mesa_is_format_compressed(format))
      return 4;

   if (format == MESA_FORMAT_S8)
      return brw->gen >= 7 ? 8 : 4;

   GLenum base_format = _mesa_get_format_base_format(format);

   if (brw->gen >= 6 &&
       (base_format == GL_DEPTH_COMPONENT ||
	base_format == GL_DEPTH_STENCIL)) {
      return 4;
   }

   return 2;
}

static void
brw_miptree_layout_2d(struct intel_mipmap_tree *mt)
{
   unsigned x = 0;
   unsigned y = 0;
   unsigned width = mt->physical_width0;
   unsigned height = mt->physical_height0;
   unsigned depth = mt->physical_depth0; /* number of array layers. */

   mt->total_width = mt->physical_width0;

   if (mt->compressed) {
       mt->total_width = ALIGN(mt->physical_width0, mt->align_w);
   }

   /* May need to adjust width to accomodate the placement of
    * the 2nd mipmap.  This occurs when the alignment
    * constraints of mipmap placement push the right edge of the
    * 2nd mipmap out past the width of its parent.
    */
   if (mt->first_level != mt->last_level) {
       unsigned mip1_width;

       if (mt->compressed) {
          mip1_width = ALIGN(minify(mt->physical_width0, 1), mt->align_w) +
             ALIGN(minify(mt->physical_width0, 2), mt->align_w);
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

      intel_miptree_set_level_info(mt, level, x, y, width,
				   height, depth);

      img_height = ALIGN(height, mt->align_h);
      if (mt->compressed)
	 img_height /= mt->align_h;

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

static void
brw_miptree_layout_texture_array(struct brw_context *brw,
				 struct intel_mipmap_tree *mt)
{
   unsigned qpitch = 0;
   int h0, h1;

   h0 = ALIGN(mt->physical_height0, mt->align_h);
   h1 = ALIGN(minify(mt->physical_height0, 1), mt->align_h);
   if (mt->array_spacing_lod0)
      qpitch = h0;
   else
      qpitch = (h0 + h1 + (brw->gen >= 7 ? 12 : 11) * mt->align_h);
   if (mt->compressed)
      qpitch /= 4;

   brw_miptree_layout_2d(mt);

   for (unsigned level = mt->first_level; level <= mt->last_level; level++) {
      for (int q = 0; q < mt->physical_depth0; q++) {
	 intel_miptree_set_image_offset(mt, level, q, 0, q * qpitch);
      }
   }
   mt->total_height = qpitch * mt->physical_depth0;

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

      intel_miptree_set_level_info(mt, level, 0, 0, WL, HL, DL);

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
   mt->align_w = intel_horizontal_texture_alignment_unit(brw, mt->format);
   mt->align_h = intel_vertical_texture_alignment_unit(brw, mt->format);

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
         brw_miptree_layout_2d(mt);
         break;
      }
      break;
   }
   DBG("%s: %dx%dx%d\n", __FUNCTION__,
       mt->total_width, mt->total_height, mt->cpp);
}

