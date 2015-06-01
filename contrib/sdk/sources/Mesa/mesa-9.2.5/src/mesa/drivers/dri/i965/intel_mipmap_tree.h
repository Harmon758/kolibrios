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

#ifndef INTEL_MIPMAP_TREE_H
#define INTEL_MIPMAP_TREE_H

#include <assert.h>

#include "intel_regions.h"
#include "intel_resolve_map.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A layer on top of the intel_regions code which adds:
 *
 * - Code to size and layout a region to hold a set of mipmaps.
 * - Query to determine if a new image fits in an existing tree.
 * - More refcounting 
 *     - maybe able to remove refcounting from intel_region?
 * - ?
 *
 * The fixed mipmap layout of intel hardware where one offset
 * specifies the position of all images in a mipmap hierachy
 * complicates the implementation of GL texture image commands,
 * compared to hardware where each image is specified with an
 * independent offset.
 *
 * In an ideal world, each texture object would be associated with a
 * single bufmgr buffer or 2d intel_region, and all the images within
 * the texture object would slot into the tree as they arrive.  The
 * reality can be a little messier, as images can arrive from the user
 * with sizes that don't fit in the existing tree, or in an order
 * where the tree layout cannot be guessed immediately.  
 * 
 * This structure encodes an idealized mipmap tree.  The GL image
 * commands build these where possible, otherwise store the images in
 * temporary system buffers.
 */

struct intel_resolve_map;
struct intel_texture_image;

/**
 * When calling intel_miptree_map() on an ETC-transcoded-to-RGB miptree or a
 * depthstencil-split-to-separate-stencil miptree, we'll normally make a
 * tmeporary and recreate the kind of data requested by Mesa core, since we're
 * satisfying some glGetTexImage() request or something.
 *
 * However, occasionally you want to actually map the miptree's current data
 * without transcoding back.  This flag to intel_miptree_map() gets you that.
 */
#define BRW_MAP_DIRECT_BIT	0x80000000

struct intel_miptree_map {
   /** Bitfield of GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, GL_MAP_INVALIDATE_BIT */
   GLbitfield mode;
   /** Region of interest for the map. */
   int x, y, w, h;
   /** Possibly malloced temporary buffer for the mapping. */
   void *buffer;
   /** Possible pointer to a temporary linear miptree for the mapping. */
   struct intel_mipmap_tree *mt;
   /** Pointer to the start of (map_x, map_y) returned by the mapping. */
   void *ptr;
   /** Stride of the mapping. */
   int stride;

   /**
    * intel_mipmap_tree::singlesample_mt is temporary storage that persists
    * only for the duration of the map.
    */
   bool singlesample_mt_is_tmp;
};

/**
 * Describes the location of each texture image within a texture region.
 */
struct intel_mipmap_level
{
   /** Offset to this miptree level, used in computing x_offset. */
   GLuint level_x;
   /** Offset to this miptree level, used in computing y_offset. */
   GLuint level_y;
   GLuint width;
   GLuint height;

   /**
    * \brief Number of 2D slices in this miplevel.
    *
    * The exact semantics of depth varies according to the texture target:
    *    - For GL_TEXTURE_CUBE_MAP, depth is 6.
    *    - For GL_TEXTURE_2D_ARRAY, depth is the number of array slices. It is
    *      identical for all miplevels in the texture.
    *    - For GL_TEXTURE_3D, it is the texture's depth at this miplevel. Its
    *      value, like width and height, varies with miplevel.
    *    - For other texture types, depth is 1.
    */
   GLuint depth;

   /**
    * \brief List of 2D images in this mipmap level.
    *
    * This may be a list of cube faces, array slices in 2D array texture, or
    * layers in a 3D texture. The list's length is \c depth.
    */
   struct intel_mipmap_slice {
      /**
       * \name Offset to slice
       * \{
       *
       * Hardware formats are so diverse that that there is no unified way to
       * compute the slice offsets, so we store them in this table.
       *
       * The (x, y) offset to slice \c s at level \c l relative the miptrees
       * base address is
       * \code
       *     x = mt->level[l].slice[s].x_offset
       *     y = mt->level[l].slice[s].y_offset
       */
      GLuint x_offset;
      GLuint y_offset;
      /** \} */

      /**
       * Mapping information. Persistent for the duration of
       * intel_miptree_map/unmap on this slice.
       */
      struct intel_miptree_map *map;

      /**
       * \brief Is HiZ enabled for this slice?
       *
       * If \c mt->level[l].slice[s].has_hiz is set, then (1) \c mt->hiz_mt
       * has been allocated and (2) the HiZ memory corresponding to this slice
       * resides at \c mt->hiz_mt->level[l].slice[s].
       */
      bool has_hiz;
   } *slice;
};

/**
 * Enum for keeping track of the different MSAA layouts supported by Gen7.
 */
enum intel_msaa_layout
{
   /**
    * Ordinary surface with no MSAA.
    */
   INTEL_MSAA_LAYOUT_NONE,

   /**
    * Interleaved Multisample Surface.  The additional samples are
    * accommodated by scaling up the width and the height of the surface so
    * that all the samples corresponding to a pixel are located at nearby
    * memory locations.
    */
   INTEL_MSAA_LAYOUT_IMS,

   /**
    * Uncompressed Multisample Surface.  The surface is stored as a 2D array,
    * with array slice n containing all pixel data for sample n.
    */
   INTEL_MSAA_LAYOUT_UMS,

   /**
    * Compressed Multisample Surface.  The surface is stored as in
    * INTEL_MSAA_LAYOUT_UMS, but there is an additional buffer called the MCS
    * (Multisample Control Surface) buffer.  Each pixel in the MCS buffer
    * indicates the mapping from sample number to array slice.  This allows
    * the common case (where all samples constituting a pixel have the same
    * color value) to be stored efficiently by just using a single array
    * slice.
    */
   INTEL_MSAA_LAYOUT_CMS,
};


/**
 * Enum for keeping track of the state of an MCS buffer associated with a
 * miptree.  This determines when fast clear related operations are needed.
 *
 * Fast clear works by deferring the memory writes that would be used to clear
 * the buffer, so that instead of performing them at the time of the clear
 * operation, the hardware automatically performs them at the time that the
 * buffer is later accessed for rendering.  The MCS buffer keeps track of
 * which regions of the buffer still have pending clear writes.
 *
 * This enum keeps track of the driver's knowledge of the state of the MCS
 * buffer.
 *
 * MCS buffers only exist on Gen7+.
 */
enum intel_mcs_state
{
   /**
    * There is no MCS buffer for this miptree, and one should never be
    * allocated.
    */
   INTEL_MCS_STATE_NONE,

   /**
    * An MCS buffer exists for this miptree, and it is used for MSAA purposes.
    */
   INTEL_MCS_STATE_MSAA,

   /**
    * No deferred clears are pending for this miptree, and the contents of the
    * color buffer are entirely correct.  An MCS buffer may or may not exist
    * for this miptree.  If it does exist, it is entirely in the "no deferred
    * clears pending" state.  If it does not exist, it will be created the
    * first time a fast color clear is executed.
    *
    * In this state, the color buffer can be used for purposes other than
    * rendering without needing a render target resolve.
    */
   INTEL_MCS_STATE_RESOLVED,

   /**
    * An MCS buffer exists for this miptree, and deferred clears are pending
    * for some regions of the color buffer, as indicated by the MCS buffer.
    * The contents of the color buffer are only correct for the regions where
    * the MCS buffer doesn't indicate a deferred clear.
    *
    * In this state, a render target resolve must be performed before the
    * color buffer can be used for purposes other than rendering.
    */
   INTEL_MCS_STATE_UNRESOLVED,

   /**
    * An MCS buffer exists for this miptree, and deferred clears are pending
    * for the entire color buffer, and the contents of the MCS buffer reflect
    * this.  The contents of the color buffer are undefined.
    *
    * In this state, a render target resolve must be performed before the
    * color buffer can be used for purposes other than rendering.
    *
    * If the client attempts to clear a buffer which is already in this state,
    * the clear can be safely skipped, since the buffer is already clear.
    */
   INTEL_MCS_STATE_CLEAR,
};

struct intel_mipmap_tree
{
   /* Effectively the key:
    */
   GLenum target;

   /**
    * Generally, this is just the same as the gl_texture_image->TexFormat or
    * gl_renderbuffer->Format.
    *
    * However, for textures and renderbuffers with packed depth/stencil formats
    * on hardware where we want or need to use separate stencil, there will be
    * two miptrees for storing the data.  If the depthstencil texture or rb is
    * MESA_FORMAT_Z32_FLOAT_X24S8, then mt->format will be
    * MESA_FORMAT_Z32_FLOAT, otherwise for MESA_FORMAT_S8_Z24 objects it will be
    * MESA_FORMAT_X8_Z24.
    *
    * For ETC1/ETC2 textures, this is one of the uncompressed mesa texture
    * formats if the hardware lacks support for ETC1/ETC2. See @ref wraps_etc.
    */
   gl_format format;

   /** This variable stores the value of ETC compressed texture format */
   gl_format etc_format;

   /**
    * The X offset of each image in the miptree must be aligned to this.
    * See the comments in brw_tex_layout.c.
    */
   unsigned int align_w;
   unsigned int align_h; /**< \see align_w */

   GLuint first_level;
   GLuint last_level;

   /**
    * Level zero image dimensions.  These dimensions correspond to the
    * physical layout of data in memory.  Accordingly, they account for the
    * extra width, height, and or depth that must be allocated in order to
    * accommodate multisample formats, and they account for the extra factor
    * of 6 in depth that must be allocated in order to accommodate cubemap
    * textures.
    */
   GLuint physical_width0, physical_height0, physical_depth0;

   GLuint cpp;
   GLuint num_samples;
   bool compressed;

   /**
    * Level zero image dimensions.  These dimensions correspond to the
    * logical width, height, and depth of the region as seen by client code.
    * Accordingly, they do not account for the extra width, height, and/or
    * depth that must be allocated in order to accommodate multisample
    * formats, nor do they account for the extra factor of 6 in depth that
    * must be allocated in order to accommodate cubemap textures.
    */
   uint32_t logical_width0, logical_height0, logical_depth0;

   /**
    * For 1D array, 2D array, cube, and 2D multisampled surfaces on Gen7: true
    * if the surface only contains LOD 0, and hence no space is for LOD's
    * other than 0 in between array slices.
    *
    * Corresponds to the surface_array_spacing bit in gen7_surface_state.
    */
   bool array_spacing_lod0;

   /**
    * MSAA layout used by this buffer.
    */
   enum intel_msaa_layout msaa_layout;

   /* Derived from the above:
    */
   GLuint total_width;
   GLuint total_height;

   /* The 3DSTATE_CLEAR_PARAMS value associated with the last depth clear to
    * this depth mipmap tree, if any.
    */
   uint32_t depth_clear_value;

   /* Includes image offset tables:
    */
   struct intel_mipmap_level level[MAX_TEXTURE_LEVELS];

   /* The data is held here:
    */
   struct intel_region *region;

   /* Offset into region bo where miptree starts:
    */
   uint32_t offset;

   /**
    * \brief Singlesample miptree.
    *
    * This is used under two cases.
    *
    * --- Case 1: As persistent singlesample storage for multisample window
    *  system front and back buffers ---
    *
    * Suppose that the window system FBO was created with a multisample
    * config.  Let `back_irb` be the `intel_renderbuffer` for the FBO's back
    * buffer. Then `back_irb` contains two miptrees: a parent multisample
    * miptree (back_irb->mt) and a child singlesample miptree
    * (back_irb->mt->singlesample_mt).  The DRM buffer shared with DRI2
    * belongs to `back_irb->mt->singlesample_mt` and contains singlesample
    * data.  The singlesample miptree is created at the same time as and
    * persists for the lifetime of its parent multisample miptree.
    *
    * When access to the singlesample data is needed, such as at
    * eglSwapBuffers and glReadPixels, an automatic downsample occurs from
    * `back_rb->mt` to `back_rb->mt->singlesample_mt` when necessary.
    *
    * This description of the back buffer applies analogously to the front
    * buffer.
    *
    *
    * --- Case 2: As temporary singlesample storage for mapping multisample
    *  miptrees ---
    *
    * Suppose the intel_miptree_map is called on a multisample miptree, `mt`,
    * for which case 1 does not apply (that is, `mt` does not belong to
    * a front or back buffer).  Then `mt->singlesample_mt` is null at the
    * start of the call. intel_miptree_map will create a temporary
    * singlesample miptree, store it at `mt->singlesample_mt`, downsample from
    * `mt` to `mt->singlesample_mt` if necessary, then map
    * `mt->singlesample_mt`. The temporary miptree is later deleted during
    * intel_miptree_unmap.
    */
   struct intel_mipmap_tree *singlesample_mt;

   /**
    * \brief A downsample is needed from this miptree to singlesample_mt.
    */
   bool need_downsample;

   /**
    * \brief HiZ miptree
    *
    * The hiz miptree contains the miptree's hiz buffer. To allocate the hiz
    * miptree, use intel_miptree_alloc_hiz().
    *
    * To determine if hiz is enabled, do not check this pointer. Instead, use
    * intel_miptree_slice_has_hiz().
    */
   struct intel_mipmap_tree *hiz_mt;

   /**
    * \brief Map of miptree slices to needed resolves.
    *
    * This is used only when the miptree has a child HiZ miptree.
    *
    * Let \c mt be a depth miptree with HiZ enabled. Then the resolve map is
    * \c mt->hiz_map. The resolve map of the child HiZ miptree, \c
    * mt->hiz_mt->hiz_map, is unused.
    */
   struct intel_resolve_map hiz_map;

   /**
    * \brief Stencil miptree for depthstencil textures.
    *
    * This miptree is used for depthstencil textures and renderbuffers that
    * require separate stencil.  It always has the true copy of the stencil
    * bits, regardless of mt->format.
    *
    * \see intel_miptree_map_depthstencil()
    * \see intel_miptree_unmap_depthstencil()
    */
   struct intel_mipmap_tree *stencil_mt;

   /**
    * \brief MCS miptree.
    *
    * This miptree contains the "multisample control surface", which stores
    * the necessary information to implement compressed MSAA
    * (INTEL_MSAA_FORMAT_CMS) and "fast color clear" behaviour on Gen7+.
    *
    * NULL if no MCS miptree is in use for this surface.
    */
   struct intel_mipmap_tree *mcs_mt;

   /**
    * MCS state for this buffer.
    */
   enum intel_mcs_state mcs_state;

   /**
    * The SURFACE_STATE bits associated with the last fast color clear to this
    * color mipmap tree, if any.
    *
    * This value will only ever contain ones in bits 28-31, so it is safe to
    * OR into dword 7 of SURFACE_STATE.
    */
   uint32_t fast_clear_color_value;

   /* These are also refcounted:
    */
   GLuint refcount;
};

enum intel_miptree_tiling_mode {
   INTEL_MIPTREE_TILING_ANY,
   INTEL_MIPTREE_TILING_Y,
   INTEL_MIPTREE_TILING_NONE,
};

bool
intel_is_non_msrt_mcs_buffer_supported(struct brw_context *brw,
                                       struct intel_mipmap_tree *mt);

void
intel_get_non_msrt_mcs_alignment(struct brw_context *brw,
                                 struct intel_mipmap_tree *mt,
                                 unsigned *width_px, unsigned *height);

bool
intel_miptree_alloc_non_msrt_mcs(struct brw_context *brw,
                                 struct intel_mipmap_tree *mt);

struct intel_mipmap_tree *intel_miptree_create(struct brw_context *brw,
                                               GLenum target,
					       gl_format format,
                                               GLuint first_level,
                                               GLuint last_level,
                                               GLuint width0,
                                               GLuint height0,
                                               GLuint depth0,
					       bool expect_accelerated_upload,
                                               GLuint num_samples,
                                               enum intel_miptree_tiling_mode);

struct intel_mipmap_tree *
intel_miptree_create_layout(struct brw_context *brw,
                            GLenum target,
                            gl_format format,
                            GLuint first_level,
                            GLuint last_level,
                            GLuint width0,
                            GLuint height0,
                            GLuint depth0,
                            bool for_bo,
                            GLuint num_samples);

struct intel_mipmap_tree *
intel_miptree_create_for_bo(struct brw_context *brw,
                            drm_intel_bo *bo,
                            gl_format format,
                            uint32_t offset,
                            uint32_t width,
                            uint32_t height,
                            int pitch,
                            uint32_t tiling);

struct intel_mipmap_tree*
intel_miptree_create_for_dri2_buffer(struct brw_context *brw,
                                     unsigned dri_attachment,
                                     gl_format format,
                                     uint32_t num_samples,
                                     struct intel_region *region);

/**
 * Create a miptree appropriate as the storage for a non-texture renderbuffer.
 * The miptree has the following properties:
 *     - The target is GL_TEXTURE_2D.
 *     - There are no levels other than the base level 0.
 *     - Depth is 1.
 */
struct intel_mipmap_tree*
intel_miptree_create_for_renderbuffer(struct brw_context *brw,
                                      gl_format format,
                                      uint32_t width,
                                      uint32_t height,
                                      uint32_t num_samples);

/** \brief Assert that the level and layer are valid for the miptree. */
static inline void
intel_miptree_check_level_layer(struct intel_mipmap_tree *mt,
                                uint32_t level,
                                uint32_t layer)
{
   assert(level >= mt->first_level);
   assert(level <= mt->last_level);
   assert(layer < mt->level[level].depth);
}

void intel_miptree_reference(struct intel_mipmap_tree **dst,
                             struct intel_mipmap_tree *src);

void intel_miptree_release(struct intel_mipmap_tree **mt);

/* Check if an image fits an existing mipmap tree layout
 */
bool intel_miptree_match_image(struct intel_mipmap_tree *mt,
                                    struct gl_texture_image *image);

void
intel_miptree_get_image_offset(struct intel_mipmap_tree *mt,
			       GLuint level, GLuint slice,
			       GLuint *x, GLuint *y);

void
intel_miptree_get_dimensions_for_image(struct gl_texture_image *image,
                                       int *width, int *height, int *depth);

uint32_t
intel_miptree_get_tile_offsets(struct intel_mipmap_tree *mt,
                               GLuint level, GLuint slice,
                               uint32_t *tile_x,
                               uint32_t *tile_y);

void intel_miptree_set_level_info(struct intel_mipmap_tree *mt,
                                  GLuint level,
                                  GLuint x, GLuint y,
                                  GLuint w, GLuint h, GLuint d);

void intel_miptree_set_image_offset(struct intel_mipmap_tree *mt,
                                    GLuint level,
                                    GLuint img, GLuint x, GLuint y);

void
intel_miptree_copy_teximage(struct brw_context *brw,
                            struct intel_texture_image *intelImage,
                            struct intel_mipmap_tree *dst_mt, bool invalidate);

bool
intel_miptree_alloc_mcs(struct brw_context *brw,
                        struct intel_mipmap_tree *mt,
                        GLuint num_samples);

/**
 * \name Miptree HiZ functions
 * \{
 *
 * It is safe to call the "slice_set_need_resolve" and "slice_resolve"
 * functions on a miptree without HiZ. In that case, each function is a no-op.
 */

/**
 * \brief Allocate the miptree's embedded HiZ miptree.
 * \see intel_mipmap_tree:hiz_mt
 * \return false if allocation failed
 */

bool
intel_miptree_alloc_hiz(struct brw_context *brw,
			struct intel_mipmap_tree *mt);

bool
intel_miptree_slice_has_hiz(struct intel_mipmap_tree *mt,
                            uint32_t level,
                            uint32_t layer);

void
intel_miptree_slice_set_needs_hiz_resolve(struct intel_mipmap_tree *mt,
                                          uint32_t level,
					  uint32_t depth);
void
intel_miptree_slice_set_needs_depth_resolve(struct intel_mipmap_tree *mt,
                                            uint32_t level,
					    uint32_t depth);

/**
 * \return false if no resolve was needed
 */
bool
intel_miptree_slice_resolve_hiz(struct brw_context *brw,
				struct intel_mipmap_tree *mt,
				unsigned int level,
				unsigned int depth);

/**
 * \return false if no resolve was needed
 */
bool
intel_miptree_slice_resolve_depth(struct brw_context *brw,
				  struct intel_mipmap_tree *mt,
				  unsigned int level,
				  unsigned int depth);

/**
 * \return false if no resolve was needed
 */
bool
intel_miptree_all_slices_resolve_hiz(struct brw_context *brw,
				     struct intel_mipmap_tree *mt);

/**
 * \return false if no resolve was needed
 */
bool
intel_miptree_all_slices_resolve_depth(struct brw_context *brw,
				       struct intel_mipmap_tree *mt);

/**\}*/

/**
 * Update the fast clear state for a miptree to indicate that it has been used
 * for rendering.
 */
static inline void
intel_miptree_used_for_rendering(struct intel_mipmap_tree *mt)
{
   /* If the buffer was previously in fast clear state, change it to
    * unresolved state, since it won't be guaranteed to be clear after
    * rendering occurs.
    */
   if (mt->mcs_state == INTEL_MCS_STATE_CLEAR)
      mt->mcs_state = INTEL_MCS_STATE_UNRESOLVED;
}

void
intel_miptree_resolve_color(struct brw_context *brw,
                            struct intel_mipmap_tree *mt);

void
intel_miptree_make_shareable(struct brw_context *brw,
                             struct intel_mipmap_tree *mt);

void
intel_miptree_downsample(struct brw_context *brw,
                         struct intel_mipmap_tree *mt);

void
intel_miptree_upsample(struct brw_context *brw,
                       struct intel_mipmap_tree *mt);

void brw_miptree_layout(struct brw_context *brw, struct intel_mipmap_tree *mt);

void *intel_miptree_map_raw(struct brw_context *brw,
                            struct intel_mipmap_tree *mt);

void intel_miptree_unmap_raw(struct brw_context *brw,
                             struct intel_mipmap_tree *mt);

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
		  int *out_stride);

void
intel_miptree_unmap(struct brw_context *brw,
		    struct intel_mipmap_tree *mt,
		    unsigned int level,
		    unsigned int slice);

void
intel_hiz_exec(struct brw_context *brw, struct intel_mipmap_tree *mt,
	       unsigned int level, unsigned int layer, enum gen6_hiz_op op);

#ifdef __cplusplus
}
#endif

#endif
