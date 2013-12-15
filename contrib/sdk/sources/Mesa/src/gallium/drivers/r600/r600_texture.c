/*
 * Copyright 2010 Jerome Glisse <glisse@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *      Jerome Glisse
 *      Corbin Simpson
 */
#include "r600_formats.h"
#include "evergreen_compute.h"
#include "r600d.h"

#include <errno.h>
#include "util/u_format_s3tc.h"
#include "util/u_memory.h"


/* Same as resource_copy_region, except that both upsampling and downsampling are allowed. */
static void r600_copy_region_with_blit(struct pipe_context *pipe,
				       struct pipe_resource *dst,
                                       unsigned dst_level,
                                       unsigned dstx, unsigned dsty, unsigned dstz,
                                       struct pipe_resource *src,
                                       unsigned src_level,
                                       const struct pipe_box *src_box)
{
	struct pipe_blit_info blit;

	memset(&blit, 0, sizeof(blit));
	blit.src.resource = src;
	blit.src.format = src->format;
	blit.src.level = src_level;
	blit.src.box = *src_box;
	blit.dst.resource = dst;
	blit.dst.format = dst->format;
	blit.dst.level = dst_level;
	blit.dst.box.x = dstx;
	blit.dst.box.y = dsty;
	blit.dst.box.z = dstz;
	blit.dst.box.width = src_box->width;
	blit.dst.box.height = src_box->height;
	blit.dst.box.depth = src_box->depth;
	blit.mask = util_format_get_mask(src->format) &
		    util_format_get_mask(dst->format);
	blit.filter = PIPE_TEX_FILTER_NEAREST;

	if (blit.mask) {
		pipe->blit(pipe, &blit);
	}
}

/* Copy from a full GPU texture to a transfer's staging one. */
static void r600_copy_to_staging_texture(struct pipe_context *ctx, struct r600_transfer *rtransfer)
{
	struct r600_context *rctx = (struct r600_context*)ctx;
	struct pipe_transfer *transfer = (struct pipe_transfer*)rtransfer;
	struct pipe_resource *dst = &rtransfer->staging->b.b;
	struct pipe_resource *src = transfer->resource;

	if (src->nr_samples > 1) {
		r600_copy_region_with_blit(ctx, dst, 0, 0, 0, 0,
					   src, transfer->level, &transfer->box);
		return;
	}

	if (!rctx->screen->dma_blit(ctx, dst, 0, 0, 0, 0,
				    src, transfer->level,
				    &transfer->box)) {
		ctx->resource_copy_region(ctx, dst, 0, 0, 0, 0,
					  src, transfer->level, &transfer->box);
	}
}

/* Copy from a transfer's staging texture to a full GPU one. */
static void r600_copy_from_staging_texture(struct pipe_context *ctx, struct r600_transfer *rtransfer)
{
	struct r600_context *rctx = (struct r600_context*)ctx;
	struct pipe_transfer *transfer = (struct pipe_transfer*)rtransfer;
	struct pipe_resource *dst = transfer->resource;
	struct pipe_resource *src = &rtransfer->staging->b.b;
	struct pipe_box sbox;

	u_box_3d(0, 0, 0, transfer->box.width, transfer->box.height, transfer->box.depth, &sbox);

	if (dst->nr_samples > 1) {
		r600_copy_region_with_blit(ctx, dst, transfer->level,
					   transfer->box.x, transfer->box.y, transfer->box.z,
					   src, 0, &sbox);
		return;
	}

	if (!rctx->screen->dma_blit(ctx, dst, transfer->level,
				    transfer->box.x, transfer->box.y, transfer->box.z,
				    src, 0, &sbox)) {
		ctx->resource_copy_region(ctx, dst, transfer->level,
					  transfer->box.x, transfer->box.y, transfer->box.z,
					  src, 0, &sbox);
	}
}

static unsigned r600_texture_get_offset(struct r600_texture *rtex, unsigned level,
					const struct pipe_box *box)
{
	enum pipe_format format = rtex->resource.b.b.format;

	return rtex->surface.level[level].offset +
	       box->z * rtex->surface.level[level].slice_size +
	       box->y / util_format_get_blockheight(format) * rtex->surface.level[level].pitch_bytes +
	       box->x / util_format_get_blockwidth(format) * util_format_get_blocksize(format);
}

static int r600_init_surface(struct r600_screen *rscreen,
			     struct radeon_surface *surface,
			     const struct pipe_resource *ptex,
			     unsigned array_mode,
			     bool is_flushed_depth)
{
	const struct util_format_description *desc =
		util_format_description(ptex->format);
	bool is_depth, is_stencil;

	is_depth = util_format_has_depth(desc);
	is_stencil = util_format_has_stencil(desc);

	surface->npix_x = ptex->width0;
	surface->npix_y = ptex->height0;
	surface->npix_z = ptex->depth0;
	surface->blk_w = util_format_get_blockwidth(ptex->format);
	surface->blk_h = util_format_get_blockheight(ptex->format);
	surface->blk_d = 1;
	surface->array_size = 1;
	surface->last_level = ptex->last_level;

	if (rscreen->chip_class >= EVERGREEN && !is_flushed_depth &&
	    ptex->format == PIPE_FORMAT_Z32_FLOAT_S8X24_UINT) {
		surface->bpe = 4; /* stencil is allocated separately on evergreen */
	} else {
		surface->bpe = util_format_get_blocksize(ptex->format);
		/* align byte per element on dword */
		if (surface->bpe == 3) {
			surface->bpe = 4;
		}
	}

	surface->nsamples = ptex->nr_samples ? ptex->nr_samples : 1;
	surface->flags = 0;

	switch (array_mode) {
	case V_038000_ARRAY_1D_TILED_THIN1:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_1D, MODE);
		break;
	case V_038000_ARRAY_2D_TILED_THIN1:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_2D, MODE);
		break;
	case V_038000_ARRAY_LINEAR_ALIGNED:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_LINEAR_ALIGNED, MODE);
		break;
	case V_038000_ARRAY_LINEAR_GENERAL:
	default:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_LINEAR, MODE);
		break;
	}
	switch (ptex->target) {
	case PIPE_TEXTURE_1D:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_TYPE_1D, TYPE);
		break;
	case PIPE_TEXTURE_RECT:
	case PIPE_TEXTURE_2D:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_TYPE_2D, TYPE);
		break;
	case PIPE_TEXTURE_3D:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_TYPE_3D, TYPE);
		break;
	case PIPE_TEXTURE_1D_ARRAY:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_TYPE_1D_ARRAY, TYPE);
		surface->array_size = ptex->array_size;
		break;
	case PIPE_TEXTURE_2D_ARRAY:
	case PIPE_TEXTURE_CUBE_ARRAY: /* cube array layout like 2d layout for now */
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_TYPE_2D_ARRAY, TYPE);
		surface->array_size = ptex->array_size;
		break;
	case PIPE_TEXTURE_CUBE:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_TYPE_CUBEMAP, TYPE);
		break;
	case PIPE_BUFFER:
	default:
		return -EINVAL;
	}
	if (ptex->bind & PIPE_BIND_SCANOUT) {
		surface->flags |= RADEON_SURF_SCANOUT;
	}

	if (!is_flushed_depth && is_depth) {
		surface->flags |= RADEON_SURF_ZBUFFER;

		if (is_stencil) {
			surface->flags |= RADEON_SURF_SBUFFER |
                                          RADEON_SURF_HAS_SBUFFER_MIPTREE;
		}
	}
	return 0;
}

static int r600_setup_surface(struct pipe_screen *screen,
			      struct r600_texture *rtex,
			      unsigned pitch_in_bytes_override)
{
	struct pipe_resource *ptex = &rtex->resource.b.b;
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	unsigned i;
	int r;

	r = rscreen->ws->surface_init(rscreen->ws, &rtex->surface);
	if (r) {
		return r;
	}
	rtex->size = rtex->surface.bo_size;
	if (pitch_in_bytes_override && pitch_in_bytes_override != rtex->surface.level[0].pitch_bytes) {
		/* old ddx on evergreen over estimate alignment for 1d, only 1 level
		 * for those
		 */
		rtex->surface.level[0].nblk_x = pitch_in_bytes_override / rtex->surface.bpe;
		rtex->surface.level[0].pitch_bytes = pitch_in_bytes_override;
		rtex->surface.level[0].slice_size = pitch_in_bytes_override * rtex->surface.level[0].nblk_y;
		if (rtex->surface.flags & RADEON_SURF_SBUFFER) {
			rtex->surface.stencil_offset =
			rtex->surface.stencil_level[0].offset = rtex->surface.level[0].slice_size;
		}
	}
	for (i = 0; i <= ptex->last_level; i++) {
		switch (rtex->surface.level[i].mode) {
		case RADEON_SURF_MODE_LINEAR_ALIGNED:
			rtex->array_mode[i] = V_038000_ARRAY_LINEAR_ALIGNED;
			break;
		case RADEON_SURF_MODE_1D:
			rtex->array_mode[i] = V_038000_ARRAY_1D_TILED_THIN1;
			break;
		case RADEON_SURF_MODE_2D:
			rtex->array_mode[i] = V_038000_ARRAY_2D_TILED_THIN1;
			break;
		default:
		case RADEON_SURF_MODE_LINEAR:
			rtex->array_mode[i] = 0;
			break;
		}
	}
	return 0;
}

static boolean r600_texture_get_handle(struct pipe_screen* screen,
					struct pipe_resource *ptex,
					struct winsys_handle *whandle)
{
	struct r600_texture *rtex = (struct r600_texture*)ptex;
	struct r600_resource *resource = &rtex->resource;
	struct radeon_surface *surface = &rtex->surface;
	struct r600_screen *rscreen = (struct r600_screen*)screen;

	rscreen->ws->buffer_set_tiling(resource->buf,
				       NULL,
				       surface->level[0].mode >= RADEON_SURF_MODE_1D ?
				       RADEON_LAYOUT_TILED : RADEON_LAYOUT_LINEAR,
				       surface->level[0].mode >= RADEON_SURF_MODE_2D ?
				       RADEON_LAYOUT_TILED : RADEON_LAYOUT_LINEAR,
				       surface->bankw, surface->bankh,
				       surface->tile_split,
				       surface->stencil_tile_split,
				       surface->mtilea,
				       rtex->surface.level[0].pitch_bytes);

	return rscreen->ws->buffer_get_handle(resource->buf,
					      rtex->surface.level[0].pitch_bytes, whandle);
}

static void r600_texture_destroy(struct pipe_screen *screen,
				 struct pipe_resource *ptex)
{
	struct r600_texture *rtex = (struct r600_texture*)ptex;
	struct r600_resource *resource = &rtex->resource;

	if (rtex->flushed_depth_texture)
		pipe_resource_reference((struct pipe_resource **)&rtex->flushed_depth_texture, NULL);

        pipe_resource_reference((struct pipe_resource**)&rtex->htile, NULL);
	pb_reference(&resource->buf, NULL);
	FREE(rtex);
}

static const struct u_resource_vtbl r600_texture_vtbl;

/* The number of samples can be specified independently of the texture. */
void r600_texture_get_fmask_info(struct r600_screen *rscreen,
				 struct r600_texture *rtex,
				 unsigned nr_samples,
				 struct r600_fmask_info *out)
{
	/* FMASK is allocated like an ordinary texture. */
	struct radeon_surface fmask = rtex->surface;

	memset(out, 0, sizeof(*out));

	fmask.bo_alignment = 0;
	fmask.bo_size = 0;
	fmask.nsamples = 1;
	fmask.flags |= RADEON_SURF_FMASK;

	switch (nr_samples) {
	case 2:
	case 4:
		fmask.bpe = 1;
		fmask.bankh = 4;
		break;
	case 8:
		fmask.bpe = 4;
		break;
	default:
		R600_ERR("Invalid sample count for FMASK allocation.\n");
		return;
	}

	/* Overallocate FMASK on R600-R700 to fix colorbuffer corruption.
	 * This can be fixed by writing a separate FMASK allocator specifically
	 * for R600-R700 asics. */
	if (rscreen->chip_class <= R700) {
		fmask.bpe *= 2;
	}

	if (rscreen->ws->surface_init(rscreen->ws, &fmask)) {
		R600_ERR("Got error in surface_init while allocating FMASK.\n");
		return;
	}

	assert(fmask.level[0].mode == RADEON_SURF_MODE_2D);

	out->slice_tile_max = (fmask.level[0].nblk_x * fmask.level[0].nblk_y) / 64;
	if (out->slice_tile_max)
		out->slice_tile_max -= 1;

	out->bank_height = fmask.bankh;
	out->alignment = MAX2(256, fmask.bo_alignment);
	out->size = fmask.bo_size;
}

static void r600_texture_allocate_fmask(struct r600_screen *rscreen,
					struct r600_texture *rtex)
{
	struct r600_fmask_info fmask;

	r600_texture_get_fmask_info(rscreen, rtex,
				    rtex->resource.b.b.nr_samples, &fmask);

	rtex->fmask_bank_height = fmask.bank_height;
	rtex->fmask_slice_tile_max = fmask.slice_tile_max;
	rtex->fmask_offset = align(rtex->size, fmask.alignment);
	rtex->fmask_size = fmask.size;
	rtex->size = rtex->fmask_offset + rtex->fmask_size;
#if 0
	printf("FMASK width=%u, height=%i, bits=%u, size=%u\n",
	       fmask.npix_x, fmask.npix_y, fmask.bpe * fmask.nsamples, rtex->fmask_size);
#endif
}

void r600_texture_get_cmask_info(struct r600_screen *rscreen,
				 struct r600_texture *rtex,
				 struct r600_cmask_info *out)
{
	unsigned cmask_tile_width = 8;
	unsigned cmask_tile_height = 8;
	unsigned cmask_tile_elements = cmask_tile_width * cmask_tile_height;
	unsigned element_bits = 4;
	unsigned cmask_cache_bits = 1024;
	unsigned num_pipes = rscreen->tiling_info.num_channels;
	unsigned pipe_interleave_bytes = rscreen->tiling_info.group_bytes;

	unsigned elements_per_macro_tile = (cmask_cache_bits / element_bits) * num_pipes;
	unsigned pixels_per_macro_tile = elements_per_macro_tile * cmask_tile_elements;
	unsigned sqrt_pixels_per_macro_tile = sqrt(pixels_per_macro_tile);
	unsigned macro_tile_width = util_next_power_of_two(sqrt_pixels_per_macro_tile);
	unsigned macro_tile_height = pixels_per_macro_tile / macro_tile_width;

	unsigned pitch_elements = align(rtex->surface.npix_x, macro_tile_width);
	unsigned height = align(rtex->surface.npix_y, macro_tile_height);

	unsigned base_align = num_pipes * pipe_interleave_bytes;
	unsigned slice_bytes =
		((pitch_elements * height * element_bits + 7) / 8) / cmask_tile_elements;

	assert(macro_tile_width % 128 == 0);
	assert(macro_tile_height % 128 == 0);

	out->slice_tile_max = ((pitch_elements * height) / (128*128)) - 1;
	out->alignment = MAX2(256, base_align);
	out->size = rtex->surface.array_size * align(slice_bytes, base_align);
}

static void r600_texture_allocate_cmask(struct r600_screen *rscreen,
					struct r600_texture *rtex)
{
	struct r600_cmask_info cmask;

	r600_texture_get_cmask_info(rscreen, rtex, &cmask);

	rtex->cmask_slice_tile_max = cmask.slice_tile_max;
	rtex->cmask_offset = align(rtex->size, cmask.alignment);
	rtex->cmask_size = cmask.size;
	rtex->size = rtex->cmask_offset + rtex->cmask_size;
#if 0
	printf("CMASK: macro tile width = %u, macro tile height = %u, "
	       "pitch elements = %u, height = %u, slice tile max = %u\n",
	       macro_tile_width, macro_tile_height, pitch_elements, height,
	       rtex->cmask_slice_tile_max);
#endif
}

static struct r600_texture *
r600_texture_create_object(struct pipe_screen *screen,
			   const struct pipe_resource *base,
			   unsigned pitch_in_bytes_override,
			   struct pb_buffer *buf,
			   struct radeon_surface *surface)
{
	struct r600_texture *rtex;
	struct r600_resource *resource;
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	int r;

	rtex = CALLOC_STRUCT(r600_texture);
	if (rtex == NULL)
		return NULL;

	resource = &rtex->resource;
	resource->b.b = *base;
	resource->b.vtbl = &r600_texture_vtbl;
	pipe_reference_init(&resource->b.b.reference, 1);
	resource->b.b.screen = screen;
	rtex->pitch_override = pitch_in_bytes_override;

	/* don't include stencil-only formats which we don't support for rendering */
	rtex->is_depth = util_format_has_depth(util_format_description(rtex->resource.b.b.format));

	rtex->surface = *surface;
	r = r600_setup_surface(screen, rtex,
			       pitch_in_bytes_override);
	if (r) {
		FREE(rtex);
		return NULL;
	}

	if (base->nr_samples > 1 && !rtex->is_depth && !buf) {
		r600_texture_allocate_fmask(rscreen, rtex);
		r600_texture_allocate_cmask(rscreen, rtex);
	}

	if (!rtex->is_depth && base->nr_samples > 1 &&
	    (!rtex->fmask_size || !rtex->cmask_size)) {
		FREE(rtex);
		return NULL;
	}

	/* Tiled depth textures utilize the non-displayable tile order. */
	rtex->non_disp_tiling = rtex->is_depth && rtex->surface.level[0].mode >= RADEON_SURF_MODE_1D;

	/* only enable hyperz for PIPE_TEXTURE_2D not for PIPE_TEXTURE_2D_ARRAY
	 * Thought it might still be interessting to use hyperz for texture
	 * array without using fast clear features
	 */
	rtex->htile = NULL;
	if (!(base->flags & (R600_RESOURCE_FLAG_TRANSFER | R600_RESOURCE_FLAG_FLUSHED_DEPTH)) &&
	    util_format_is_depth_or_stencil(base->format) &&
	    rscreen->info.drm_minor >= 26 &&
	    !(rscreen->debug_flags & DBG_NO_HYPERZ) &&
	    base->target == PIPE_TEXTURE_2D &&
	    rtex->surface.level[0].nblk_x >= 32 &&
	    rtex->surface.level[0].nblk_y >= 32) {
		unsigned sw = rtex->surface.level[0].nblk_x * rtex->surface.blk_w;
		unsigned sh = rtex->surface.level[0].nblk_y * rtex->surface.blk_h;
		unsigned htile_size;
		unsigned npipes = rscreen->info.r600_num_tile_pipes;

		/* this alignment and htile size only apply to linear htile buffer */
		sw = align(sw, 16 << 3);
		sh = align(sh, npipes << 3);
		htile_size = (sw >> 3) * (sh >> 3) * 4;
		/* must be aligned with 2K * npipes */
		htile_size = align(htile_size, (2 << 10) * npipes);

		rtex->htile = (struct r600_resource*)pipe_buffer_create(&rscreen->screen, PIPE_BIND_CUSTOM,
									PIPE_USAGE_STATIC, htile_size);
		if (rtex->htile == NULL) {
			/* this is not a fatal error as we can still keep rendering
			 * without htile buffer
			 */
			R600_ERR("r600: failed to create bo for htile buffers\n");
		} else {
			r600_screen_clear_buffer(rscreen, &rtex->htile->b.b, 0, htile_size, 0);
		}
	}

	/* Now create the backing buffer. */
	if (!buf) {
		unsigned base_align = rtex->surface.bo_alignment;
		unsigned usage = R600_TEX_IS_TILED(rtex, 0) ? PIPE_USAGE_STATIC : base->usage;

		if (!r600_init_resource(rscreen, resource, rtex->size, base_align, FALSE, usage)) {
			FREE(rtex);
			return NULL;
		}
	} else {
		/* This is usually the window framebuffer. We want it in VRAM, always. */
		resource->buf = buf;
		resource->cs_buf = rscreen->ws->buffer_get_cs_handle(buf);
		resource->domains = RADEON_DOMAIN_VRAM;
	}

	if (rtex->cmask_size) {
		/* Initialize the cmask to 0xCC (= compressed state). */
		r600_screen_clear_buffer(rscreen, &rtex->resource.b.b,
					 rtex->cmask_offset, rtex->cmask_size, 0xCC);
	}

	if (rscreen->debug_flags & DBG_VM) {
		fprintf(stderr, "VM start=0x%llX  end=0x%llX | Texture %ix%ix%i, %i levels, %i samples, %s\n",
			r600_resource_va(screen, &rtex->resource.b.b),
			r600_resource_va(screen, &rtex->resource.b.b) + rtex->resource.buf->size,
			base->width0, base->height0, util_max_layer(base, 0)+1, base->last_level+1,
			base->nr_samples ? base->nr_samples : 1, util_format_short_name(base->format));
	}

	if (rscreen->debug_flags & DBG_TEX_DEPTH && rtex->is_depth && rtex->non_disp_tiling) {
		printf("Texture: npix_x=%u, npix_y=%u, npix_z=%u, blk_w=%u, "
		       "blk_h=%u, blk_d=%u, array_size=%u, last_level=%u, "
		       "bpe=%u, nsamples=%u, flags=%u\n",
		       rtex->surface.npix_x, rtex->surface.npix_y,
		       rtex->surface.npix_z, rtex->surface.blk_w,
		       rtex->surface.blk_h, rtex->surface.blk_d,
		       rtex->surface.array_size, rtex->surface.last_level,
		       rtex->surface.bpe, rtex->surface.nsamples,
		       rtex->surface.flags);
		if (rtex->surface.flags & RADEON_SURF_ZBUFFER) {
			for (int i = 0; i <= rtex->surface.last_level; i++) {
				printf("  Z %i: offset=%llu, slice_size=%llu, npix_x=%u, "
				       "npix_y=%u, npix_z=%u, nblk_x=%u, nblk_y=%u, "
				       "nblk_z=%u, pitch_bytes=%u, mode=%u\n",
				       i, (unsigned long long)rtex->surface.level[i].offset,
				       (unsigned long long)rtex->surface.level[i].slice_size,
				       u_minify(rtex->resource.b.b.width0, i),
				       u_minify(rtex->resource.b.b.height0, i),
				       u_minify(rtex->resource.b.b.depth0, i),
				       rtex->surface.level[i].nblk_x,
				       rtex->surface.level[i].nblk_y,
				       rtex->surface.level[i].nblk_z,
				       rtex->surface.level[i].pitch_bytes,
				       rtex->surface.level[i].mode);
			}
		}
		if (rtex->surface.flags & RADEON_SURF_SBUFFER) {
			for (int i = 0; i <= rtex->surface.last_level; i++) {
				printf("  S %i: offset=%llu, slice_size=%llu, npix_x=%u, "
				       "npix_y=%u, npix_z=%u, nblk_x=%u, nblk_y=%u, "
				       "nblk_z=%u, pitch_bytes=%u, mode=%u\n",
				       i, (unsigned long long)rtex->surface.stencil_level[i].offset,
				       (unsigned long long)rtex->surface.stencil_level[i].slice_size,
                                       u_minify(rtex->resource.b.b.width0, i),
				       u_minify(rtex->resource.b.b.height0, i),
				       u_minify(rtex->resource.b.b.depth0, i),
				       rtex->surface.stencil_level[i].nblk_x,
				       rtex->surface.stencil_level[i].nblk_y,
				       rtex->surface.stencil_level[i].nblk_z,
				       rtex->surface.stencil_level[i].pitch_bytes,
				       rtex->surface.stencil_level[i].mode);
			}
		}
	}
	return rtex;
}

struct pipe_resource *r600_texture_create(struct pipe_screen *screen,
						const struct pipe_resource *templ)
{
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	struct radeon_surface surface;
	const struct util_format_description *desc = util_format_description(templ->format);
	unsigned array_mode;
	int r;

	/* Default tiling mode for staging textures. */
	array_mode = V_038000_ARRAY_LINEAR_ALIGNED;

	/* Tiling doesn't work with the 422 (SUBSAMPLED) formats. That's not an issue,
	 * because 422 formats are used for videos, which prefer linear buffers
	 * for fast uploads anyway. */
	if (!(templ->flags & R600_RESOURCE_FLAG_TRANSFER) &&
	    desc->layout != UTIL_FORMAT_LAYOUT_SUBSAMPLED) {
		if (templ->flags & R600_RESOURCE_FLAG_FORCE_TILING) {
			array_mode = V_038000_ARRAY_2D_TILED_THIN1;
		} else if (!(templ->bind & PIPE_BIND_SCANOUT) &&
			   templ->usage != PIPE_USAGE_STAGING &&
			   templ->usage != PIPE_USAGE_STREAM &&
			   templ->target != PIPE_TEXTURE_1D &&
			   templ->target != PIPE_TEXTURE_1D_ARRAY &&
			   templ->height0 > 3) {
			array_mode = V_038000_ARRAY_2D_TILED_THIN1;
		} else if (util_format_is_compressed(templ->format)) {
			array_mode = V_038000_ARRAY_1D_TILED_THIN1;
		}
	}

	r = r600_init_surface(rscreen, &surface, templ, array_mode,
			      templ->flags & R600_RESOURCE_FLAG_FLUSHED_DEPTH);
	if (r) {
		return NULL;
	}
	r = rscreen->ws->surface_best(rscreen->ws, &surface);
	if (r) {
		return NULL;
	}
	return (struct pipe_resource *)r600_texture_create_object(screen, templ,
								  0, NULL, &surface);
}

struct pipe_surface *r600_create_surface_custom(struct pipe_context *pipe,
						struct pipe_resource *texture,
						const struct pipe_surface *templ,
						unsigned width, unsigned height)
{
	struct r600_surface *surface = CALLOC_STRUCT(r600_surface);

        assert(templ->u.tex.first_layer <= util_max_layer(texture, templ->u.tex.level));
        assert(templ->u.tex.last_layer <= util_max_layer(texture, templ->u.tex.level));
	assert(templ->u.tex.first_layer == templ->u.tex.last_layer);
	if (surface == NULL)
		return NULL;
	pipe_reference_init(&surface->base.reference, 1);
	pipe_resource_reference(&surface->base.texture, texture);
	surface->base.context = pipe;
	surface->base.format = templ->format;
	surface->base.width = width;
	surface->base.height = height;
	surface->base.u = templ->u;
	return &surface->base;
}

static struct pipe_surface *r600_create_surface(struct pipe_context *pipe,
						struct pipe_resource *tex,
						const struct pipe_surface *templ)
{
	unsigned level = templ->u.tex.level;

	return r600_create_surface_custom(pipe, tex, templ,
                                          u_minify(tex->width0, level),
					  u_minify(tex->height0, level));
}

static void r600_surface_destroy(struct pipe_context *pipe,
				 struct pipe_surface *surface)
{
	struct r600_surface *surf = (struct r600_surface*)surface;
	pipe_resource_reference((struct pipe_resource**)&surf->cb_buffer_fmask, NULL);
	pipe_resource_reference((struct pipe_resource**)&surf->cb_buffer_cmask, NULL);
	pipe_resource_reference(&surface->texture, NULL);
	FREE(surface);
}

struct pipe_resource *r600_texture_from_handle(struct pipe_screen *screen,
					       const struct pipe_resource *templ,
					       struct winsys_handle *whandle)
{
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	struct pb_buffer *buf = NULL;
	unsigned stride = 0;
	unsigned array_mode = 0;
	enum radeon_bo_layout micro, macro;
	struct radeon_surface surface;
	int r;

	/* Support only 2D textures without mipmaps */
	if ((templ->target != PIPE_TEXTURE_2D && templ->target != PIPE_TEXTURE_RECT) ||
	      templ->depth0 != 1 || templ->last_level != 0)
		return NULL;

	buf = rscreen->ws->buffer_from_handle(rscreen->ws, whandle, &stride);
	if (!buf)
		return NULL;

	rscreen->ws->buffer_get_tiling(buf, &micro, &macro,
				       &surface.bankw, &surface.bankh,
				       &surface.tile_split,
				       &surface.stencil_tile_split,
				       &surface.mtilea);

	if (macro == RADEON_LAYOUT_TILED)
		array_mode = V_0280A0_ARRAY_2D_TILED_THIN1;
	else if (micro == RADEON_LAYOUT_TILED)
		array_mode = V_0280A0_ARRAY_1D_TILED_THIN1;
	else
		array_mode = V_038000_ARRAY_LINEAR_ALIGNED;

	r = r600_init_surface(rscreen, &surface, templ, array_mode, false);
	if (r) {
		return NULL;
	}
	return (struct pipe_resource *)r600_texture_create_object(screen, templ,
								  stride, buf, &surface);
}

bool r600_init_flushed_depth_texture(struct pipe_context *ctx,
				     struct pipe_resource *texture,
				     struct r600_texture **staging)
{
	struct r600_texture *rtex = (struct r600_texture*)texture;
	struct pipe_resource resource;
	struct r600_texture **flushed_depth_texture = staging ?
			staging : &rtex->flushed_depth_texture;

	if (!staging && rtex->flushed_depth_texture)
		return true; /* it's ready */

	resource.target = texture->target;
	resource.format = texture->format;
	resource.width0 = texture->width0;
	resource.height0 = texture->height0;
	resource.depth0 = texture->depth0;
	resource.array_size = texture->array_size;
	resource.last_level = texture->last_level;
	resource.nr_samples = texture->nr_samples;
	resource.usage = staging ? PIPE_USAGE_STAGING : PIPE_USAGE_STATIC;
	resource.bind = texture->bind & ~PIPE_BIND_DEPTH_STENCIL;
	resource.flags = texture->flags | R600_RESOURCE_FLAG_FLUSHED_DEPTH;

	if (staging)
		resource.flags |= R600_RESOURCE_FLAG_TRANSFER;

	*flushed_depth_texture = (struct r600_texture *)ctx->screen->resource_create(ctx->screen, &resource);
	if (*flushed_depth_texture == NULL) {
		R600_ERR("failed to create temporary texture to hold flushed depth\n");
		return false;
	}

	(*flushed_depth_texture)->is_flushing_texture = TRUE;
	(*flushed_depth_texture)->non_disp_tiling = false;
	return true;
}

/**
 * Initialize the pipe_resource descriptor to be of the same size as the box,
 * which is supposed to hold a subregion of the texture "orig" at the given
 * mipmap level.
 */
static void r600_init_temp_resource_from_box(struct pipe_resource *res,
					     struct pipe_resource *orig,
					     const struct pipe_box *box,
					     unsigned level, unsigned flags)
{
	memset(res, 0, sizeof(*res));
	res->format = orig->format;
	res->width0 = box->width;
	res->height0 = box->height;
	res->depth0 = 1;
	res->array_size = 1;
	res->usage = flags & R600_RESOURCE_FLAG_TRANSFER ? PIPE_USAGE_STAGING : PIPE_USAGE_STATIC;
	res->flags = flags;

	/* We must set the correct texture target and dimensions for a 3D box. */
	if (box->depth > 1 && util_max_layer(orig, level) > 0)
		res->target = orig->target;
	else
		res->target = PIPE_TEXTURE_2D;

	switch (res->target) {
	case PIPE_TEXTURE_1D_ARRAY:
	case PIPE_TEXTURE_2D_ARRAY:
	case PIPE_TEXTURE_CUBE_ARRAY:
		res->array_size = box->depth;
		break;
	case PIPE_TEXTURE_3D:
		res->depth0 = box->depth;
		break;
	default:;
	}
}

static void *r600_texture_transfer_map(struct pipe_context *ctx,
				       struct pipe_resource *texture,
				       unsigned level,
				       unsigned usage,
				       const struct pipe_box *box,
				       struct pipe_transfer **ptransfer)
{
	struct r600_context *rctx = (struct r600_context*)ctx;
	struct r600_texture *rtex = (struct r600_texture*)texture;
	struct r600_transfer *trans;
	boolean use_staging_texture = FALSE;
	struct r600_resource *buf;
	unsigned offset = 0;
	char *map;

	if ((texture->bind & PIPE_BIND_GLOBAL) && texture->target == PIPE_BUFFER) {
		return r600_compute_global_transfer_map(ctx, texture, level, usage, box, ptransfer);
	}

	/* We cannot map a tiled texture directly because the data is
	 * in a different order, therefore we do detiling using a blit.
	 *
	 * Also, use a temporary in GTT memory for read transfers, as
	 * the CPU is much happier reading out of cached system memory
	 * than uncached VRAM.
	 */
	if (R600_TEX_IS_TILED(rtex, level)) {
		use_staging_texture = TRUE;
	}

	/* Use a staging texture for uploads if the underlying BO is busy. */
	if (!(usage & PIPE_TRANSFER_READ) &&
	    (r600_rings_is_buffer_referenced(rctx, rtex->resource.cs_buf, RADEON_USAGE_READWRITE) ||
	     rctx->ws->buffer_is_busy(rtex->resource.buf, RADEON_USAGE_READWRITE))) {
		use_staging_texture = TRUE;
	}

	if (texture->flags & R600_RESOURCE_FLAG_TRANSFER) {
		use_staging_texture = FALSE;
	}

	if (use_staging_texture && (usage & PIPE_TRANSFER_MAP_DIRECTLY)) {
		return NULL;
	}

	trans = CALLOC_STRUCT(r600_transfer);
	if (trans == NULL)
		return NULL;
	trans->transfer.resource = texture;
	trans->transfer.level = level;
	trans->transfer.usage = usage;
	trans->transfer.box = *box;

	if (rtex->is_depth) {
		struct r600_texture *staging_depth;

		if (rtex->resource.b.b.nr_samples > 1) {
			/* MSAA depth buffers need to be converted to single sample buffers.
			 *
			 * Mapping MSAA depth buffers can occur if ReadPixels is called
			 * with a multisample GLX visual.
			 *
			 * First downsample the depth buffer to a temporary texture,
			 * then decompress the temporary one to staging.
			 *
			 * Only the region being mapped is transfered.
			 */
			struct pipe_resource resource;

			r600_init_temp_resource_from_box(&resource, texture, box, level, 0);

			if (!r600_init_flushed_depth_texture(ctx, &resource, &staging_depth)) {
				R600_ERR("failed to create temporary texture to hold untiled copy\n");
				FREE(trans);
				return NULL;
			}

			if (usage & PIPE_TRANSFER_READ) {
				struct pipe_resource *temp = ctx->screen->resource_create(ctx->screen, &resource);

				r600_copy_region_with_blit(ctx, temp, 0, 0, 0, 0, texture, level, box);
				r600_blit_decompress_depth(ctx, (struct r600_texture*)temp, staging_depth,
							   0, 0, 0, box->depth, 0, 0);
				pipe_resource_reference((struct pipe_resource**)&temp, NULL);
			}
		}
		else {
			/* XXX: only readback the rectangle which is being mapped? */
			/* XXX: when discard is true, no need to read back from depth texture */
			if (!r600_init_flushed_depth_texture(ctx, texture, &staging_depth)) {
				R600_ERR("failed to create temporary texture to hold untiled copy\n");
				FREE(trans);
				return NULL;
			}

			r600_blit_decompress_depth(ctx, rtex, staging_depth,
						   level, level,
						   box->z, box->z + box->depth - 1,
						   0, 0);

			offset = r600_texture_get_offset(staging_depth, level, box);
		}

		trans->transfer.stride = staging_depth->surface.level[level].pitch_bytes;
                trans->transfer.layer_stride = staging_depth->surface.level[level].slice_size;
		trans->staging = (struct r600_resource*)staging_depth;
	} else if (use_staging_texture) {
		struct pipe_resource resource;
		struct r600_texture *staging;

		r600_init_temp_resource_from_box(&resource, texture, box, level,
						 R600_RESOURCE_FLAG_TRANSFER);

		/* Create the temporary texture. */
		staging = (struct r600_texture*)ctx->screen->resource_create(ctx->screen, &resource);
		if (staging == NULL) {
			R600_ERR("failed to create temporary texture to hold untiled copy\n");
			FREE(trans);
			return NULL;
		}
		trans->staging = &staging->resource;
		trans->transfer.stride = staging->surface.level[0].pitch_bytes;
		trans->transfer.layer_stride = staging->surface.level[0].slice_size;
		if (usage & PIPE_TRANSFER_READ) {
			r600_copy_to_staging_texture(ctx, trans);
		}
	} else {
		/* the resource is mapped directly */
		trans->transfer.stride = rtex->surface.level[level].pitch_bytes;
		trans->transfer.layer_stride = rtex->surface.level[level].slice_size;
		offset = r600_texture_get_offset(rtex, level, box);
	}

	if (trans->staging) {
		buf = trans->staging;
	} else {
		buf = &rtex->resource;
	}

	if (!(map = r600_buffer_mmap_sync_with_rings(rctx, buf, usage))) {
		pipe_resource_reference((struct pipe_resource**)&trans->staging, NULL);
		FREE(trans);
		return NULL;
	}

	*ptransfer = &trans->transfer;
	return map + offset;
}

static void r600_texture_transfer_unmap(struct pipe_context *ctx,
					struct pipe_transfer* transfer)
{
	struct r600_transfer *rtransfer = (struct r600_transfer*)transfer;
	struct r600_context *rctx = (struct r600_context*)ctx;
	struct radeon_winsys_cs_handle *buf;
	struct pipe_resource *texture = transfer->resource;
	struct r600_texture *rtex = (struct r600_texture*)texture;

	if ((transfer->resource->bind & PIPE_BIND_GLOBAL) && transfer->resource->target == PIPE_BUFFER) {
		return r600_compute_global_transfer_unmap(ctx, transfer);
	}

	if (rtransfer->staging) {
		buf = ((struct r600_resource *)rtransfer->staging)->cs_buf;
	} else {
		buf = ((struct r600_resource *)transfer->resource)->cs_buf;
	}
	rctx->ws->buffer_unmap(buf);

	if ((transfer->usage & PIPE_TRANSFER_WRITE) && rtransfer->staging) {
		if (rtex->is_depth && rtex->resource.b.b.nr_samples <= 1) {
			ctx->resource_copy_region(ctx, texture, transfer->level,
						  transfer->box.x, transfer->box.y, transfer->box.z,
						  &rtransfer->staging->b.b, transfer->level,
						  &transfer->box);
		} else {
			r600_copy_from_staging_texture(ctx, rtransfer);
		}
	}

	if (rtransfer->staging)
		pipe_resource_reference((struct pipe_resource**)&rtransfer->staging, NULL);

	FREE(transfer);
}

void r600_init_surface_functions(struct r600_context *r600)
{
	r600->context.create_surface = r600_create_surface;
	r600->context.surface_destroy = r600_surface_destroy;
}

unsigned r600_get_swizzle_combined(const unsigned char *swizzle_format,
				   const unsigned char *swizzle_view,
				   boolean vtx)
{
	unsigned i;
	unsigned char swizzle[4];
	unsigned result = 0;
	const uint32_t tex_swizzle_shift[4] = {
		16, 19, 22, 25,
	};
	const uint32_t vtx_swizzle_shift[4] = {
		3, 6, 9, 12,
	};
	const uint32_t swizzle_bit[4] = {
		0, 1, 2, 3,
	};
	const uint32_t *swizzle_shift = tex_swizzle_shift;

	if (vtx)
		swizzle_shift = vtx_swizzle_shift;

	if (swizzle_view) {
		util_format_compose_swizzles(swizzle_format, swizzle_view, swizzle);
	} else {
		memcpy(swizzle, swizzle_format, 4);
	}

	/* Get swizzle. */
	for (i = 0; i < 4; i++) {
		switch (swizzle[i]) {
		case UTIL_FORMAT_SWIZZLE_Y:
			result |= swizzle_bit[1] << swizzle_shift[i];
			break;
		case UTIL_FORMAT_SWIZZLE_Z:
			result |= swizzle_bit[2] << swizzle_shift[i];
			break;
		case UTIL_FORMAT_SWIZZLE_W:
			result |= swizzle_bit[3] << swizzle_shift[i];
			break;
		case UTIL_FORMAT_SWIZZLE_0:
			result |= V_038010_SQ_SEL_0 << swizzle_shift[i];
			break;
		case UTIL_FORMAT_SWIZZLE_1:
			result |= V_038010_SQ_SEL_1 << swizzle_shift[i];
			break;
		default: /* UTIL_FORMAT_SWIZZLE_X */
			result |= swizzle_bit[0] << swizzle_shift[i];
		}
	}
	return result;
}

/* texture format translate */
uint32_t r600_translate_texformat(struct pipe_screen *screen,
				  enum pipe_format format,
				  const unsigned char *swizzle_view,
				  uint32_t *word4_p, uint32_t *yuv_format_p)
{
	struct r600_screen *rscreen = (struct r600_screen *)screen;
	uint32_t result = 0, word4 = 0, yuv_format = 0;
	const struct util_format_description *desc;
	boolean uniform = TRUE;
	bool enable_s3tc = rscreen->info.drm_minor >= 9;
	bool is_srgb_valid = FALSE;
	const unsigned char swizzle_xxxx[4] = {0, 0, 0, 0};
	const unsigned char swizzle_yyyy[4] = {1, 1, 1, 1};

	int i;
	const uint32_t sign_bit[4] = {
		S_038010_FORMAT_COMP_X(V_038010_SQ_FORMAT_COMP_SIGNED),
		S_038010_FORMAT_COMP_Y(V_038010_SQ_FORMAT_COMP_SIGNED),
		S_038010_FORMAT_COMP_Z(V_038010_SQ_FORMAT_COMP_SIGNED),
		S_038010_FORMAT_COMP_W(V_038010_SQ_FORMAT_COMP_SIGNED)
	};
	desc = util_format_description(format);

	/* Depth and stencil swizzling is handled separately. */
	if (desc->colorspace != UTIL_FORMAT_COLORSPACE_ZS) {
		word4 |= r600_get_swizzle_combined(desc->swizzle, swizzle_view, FALSE);
	}

	/* Colorspace (return non-RGB formats directly). */
	switch (desc->colorspace) {
	/* Depth stencil formats */
	case UTIL_FORMAT_COLORSPACE_ZS:
		switch (format) {
		/* Depth sampler formats. */
		case PIPE_FORMAT_Z16_UNORM:
			word4 |= r600_get_swizzle_combined(swizzle_xxxx, swizzle_view, FALSE);
			result = FMT_16;
			goto out_word4;
		case PIPE_FORMAT_Z24X8_UNORM:
		case PIPE_FORMAT_Z24_UNORM_S8_UINT:
			word4 |= r600_get_swizzle_combined(swizzle_xxxx, swizzle_view, FALSE);
			result = FMT_8_24;
			goto out_word4;
		case PIPE_FORMAT_X8Z24_UNORM:
		case PIPE_FORMAT_S8_UINT_Z24_UNORM:
			if (rscreen->chip_class < EVERGREEN)
				goto out_unknown;
			word4 |= r600_get_swizzle_combined(swizzle_yyyy, swizzle_view, FALSE);
			result = FMT_24_8;
			goto out_word4;
		case PIPE_FORMAT_Z32_FLOAT:
			word4 |= r600_get_swizzle_combined(swizzle_xxxx, swizzle_view, FALSE);
			result = FMT_32_FLOAT;
			goto out_word4;
		case PIPE_FORMAT_Z32_FLOAT_S8X24_UINT:
			word4 |= r600_get_swizzle_combined(swizzle_xxxx, swizzle_view, FALSE);
			result = FMT_X24_8_32_FLOAT;
			goto out_word4;
		/* Stencil sampler formats. */
		case PIPE_FORMAT_S8_UINT:
			word4 |= S_038010_NUM_FORMAT_ALL(V_038010_SQ_NUM_FORMAT_INT);
			word4 |= r600_get_swizzle_combined(swizzle_xxxx, swizzle_view, FALSE);
			result = FMT_8;
			goto out_word4;
		case PIPE_FORMAT_X24S8_UINT:
			word4 |= S_038010_NUM_FORMAT_ALL(V_038010_SQ_NUM_FORMAT_INT);
			word4 |= r600_get_swizzle_combined(swizzle_yyyy, swizzle_view, FALSE);
			result = FMT_8_24;
			goto out_word4;
		case PIPE_FORMAT_S8X24_UINT:
			if (rscreen->chip_class < EVERGREEN)
				goto out_unknown;
			word4 |= S_038010_NUM_FORMAT_ALL(V_038010_SQ_NUM_FORMAT_INT);
			word4 |= r600_get_swizzle_combined(swizzle_xxxx, swizzle_view, FALSE);
			result = FMT_24_8;
			goto out_word4;
		case PIPE_FORMAT_X32_S8X24_UINT:
			word4 |= S_038010_NUM_FORMAT_ALL(V_038010_SQ_NUM_FORMAT_INT);
			word4 |= r600_get_swizzle_combined(swizzle_yyyy, swizzle_view, FALSE);
			result = FMT_X24_8_32_FLOAT;
			goto out_word4;
		default:
			goto out_unknown;
		}

	case UTIL_FORMAT_COLORSPACE_YUV:
		yuv_format |= (1 << 30);
		switch (format) {
		case PIPE_FORMAT_UYVY:
		case PIPE_FORMAT_YUYV:
		default:
			break;
		}
		goto out_unknown; /* XXX */

	case UTIL_FORMAT_COLORSPACE_SRGB:
		word4 |= S_038010_FORCE_DEGAMMA(1);
		break;

	default:
		break;
	}

	if (desc->layout == UTIL_FORMAT_LAYOUT_RGTC) {
		if (!enable_s3tc)
			goto out_unknown;

		switch (format) {
		case PIPE_FORMAT_RGTC1_SNORM:
		case PIPE_FORMAT_LATC1_SNORM:
			word4 |= sign_bit[0];
		case PIPE_FORMAT_RGTC1_UNORM:
		case PIPE_FORMAT_LATC1_UNORM:
			result = FMT_BC4;
			goto out_word4;
		case PIPE_FORMAT_RGTC2_SNORM:
		case PIPE_FORMAT_LATC2_SNORM:
			word4 |= sign_bit[0] | sign_bit[1];
		case PIPE_FORMAT_RGTC2_UNORM:
		case PIPE_FORMAT_LATC2_UNORM:
			result = FMT_BC5;
			goto out_word4;
		default:
			goto out_unknown;
		}
	}

	if (desc->layout == UTIL_FORMAT_LAYOUT_S3TC) {

		if (!enable_s3tc)
			goto out_unknown;

		if (!util_format_s3tc_enabled) {
			goto out_unknown;
		}

		switch (format) {
		case PIPE_FORMAT_DXT1_RGB:
		case PIPE_FORMAT_DXT1_RGBA:
		case PIPE_FORMAT_DXT1_SRGB:
		case PIPE_FORMAT_DXT1_SRGBA:
			result = FMT_BC1;
			is_srgb_valid = TRUE;
			goto out_word4;
		case PIPE_FORMAT_DXT3_RGBA:
		case PIPE_FORMAT_DXT3_SRGBA:
			result = FMT_BC2;
			is_srgb_valid = TRUE;
			goto out_word4;
		case PIPE_FORMAT_DXT5_RGBA:
		case PIPE_FORMAT_DXT5_SRGBA:
			result = FMT_BC3;
			is_srgb_valid = TRUE;
			goto out_word4;
		default:
			goto out_unknown;
		}
	}

	if (desc->layout == UTIL_FORMAT_LAYOUT_SUBSAMPLED) {
		switch (format) {
		case PIPE_FORMAT_R8G8_B8G8_UNORM:
		case PIPE_FORMAT_G8R8_B8R8_UNORM:
			result = FMT_GB_GR;
			goto out_word4;
		case PIPE_FORMAT_G8R8_G8B8_UNORM:
		case PIPE_FORMAT_R8G8_R8B8_UNORM:
			result = FMT_BG_RG;
			goto out_word4;
		default:
			goto out_unknown;
		}
	}

	if (format == PIPE_FORMAT_R9G9B9E5_FLOAT) {
		result = FMT_5_9_9_9_SHAREDEXP;
		goto out_word4;
	} else if (format == PIPE_FORMAT_R11G11B10_FLOAT) {
		result = FMT_10_11_11_FLOAT;
		goto out_word4;
	}


	for (i = 0; i < desc->nr_channels; i++) {
		if (desc->channel[i].type == UTIL_FORMAT_TYPE_SIGNED) {
			word4 |= sign_bit[i];
		}
	}

	/* R8G8Bx_SNORM - XXX CxV8U8 */

	/* See whether the components are of the same size. */
	for (i = 1; i < desc->nr_channels; i++) {
		uniform = uniform && desc->channel[0].size == desc->channel[i].size;
	}

	/* Non-uniform formats. */
	if (!uniform) {
		if (desc->colorspace != UTIL_FORMAT_COLORSPACE_SRGB &&
		    desc->channel[0].pure_integer)
			word4 |= S_038010_NUM_FORMAT_ALL(V_038010_SQ_NUM_FORMAT_INT);
		switch(desc->nr_channels) {
		case 3:
			if (desc->channel[0].size == 5 &&
			    desc->channel[1].size == 6 &&
			    desc->channel[2].size == 5) {
				result = FMT_5_6_5;
				goto out_word4;
			}
			goto out_unknown;
		case 4:
			if (desc->channel[0].size == 5 &&
			    desc->channel[1].size == 5 &&
			    desc->channel[2].size == 5 &&
			    desc->channel[3].size == 1) {
				result = FMT_1_5_5_5;
				goto out_word4;
			}
			if (desc->channel[0].size == 10 &&
			    desc->channel[1].size == 10 &&
			    desc->channel[2].size == 10 &&
			    desc->channel[3].size == 2) {
				result = FMT_2_10_10_10;
				goto out_word4;
			}
			goto out_unknown;
		}
		goto out_unknown;
	}

	/* Find the first non-VOID channel. */
	for (i = 0; i < 4; i++) {
		if (desc->channel[i].type != UTIL_FORMAT_TYPE_VOID) {
			break;
		}
	}

	if (i == 4)
		goto out_unknown;

	/* uniform formats */
	switch (desc->channel[i].type) {
	case UTIL_FORMAT_TYPE_UNSIGNED:
	case UTIL_FORMAT_TYPE_SIGNED:
#if 0
		if (!desc->channel[i].normalized &&
		    desc->colorspace != UTIL_FORMAT_COLORSPACE_SRGB) {
			goto out_unknown;
		}
#endif
		if (desc->colorspace != UTIL_FORMAT_COLORSPACE_SRGB &&
		    desc->channel[i].pure_integer)
			word4 |= S_038010_NUM_FORMAT_ALL(V_038010_SQ_NUM_FORMAT_INT);

		switch (desc->channel[i].size) {
		case 4:
			switch (desc->nr_channels) {
			case 2:
				result = FMT_4_4;
				goto out_word4;
			case 4:
				result = FMT_4_4_4_4;
				goto out_word4;
			}
			goto out_unknown;
		case 8:
			switch (desc->nr_channels) {
			case 1:
				result = FMT_8;
				goto out_word4;
			case 2:
				result = FMT_8_8;
				goto out_word4;
			case 4:
				result = FMT_8_8_8_8;
				is_srgb_valid = TRUE;
				goto out_word4;
			}
			goto out_unknown;
		case 16:
			switch (desc->nr_channels) {
			case 1:
				result = FMT_16;
				goto out_word4;
			case 2:
				result = FMT_16_16;
				goto out_word4;
			case 4:
				result = FMT_16_16_16_16;
				goto out_word4;
			}
			goto out_unknown;
		case 32:
			switch (desc->nr_channels) {
			case 1:
				result = FMT_32;
				goto out_word4;
			case 2:
				result = FMT_32_32;
				goto out_word4;
			case 4:
				result = FMT_32_32_32_32;
				goto out_word4;
			}
		}
		goto out_unknown;

	case UTIL_FORMAT_TYPE_FLOAT:
		switch (desc->channel[i].size) {
		case 16:
			switch (desc->nr_channels) {
			case 1:
				result = FMT_16_FLOAT;
				goto out_word4;
			case 2:
				result = FMT_16_16_FLOAT;
				goto out_word4;
			case 4:
				result = FMT_16_16_16_16_FLOAT;
				goto out_word4;
			}
			goto out_unknown;
		case 32:
			switch (desc->nr_channels) {
			case 1:
				result = FMT_32_FLOAT;
				goto out_word4;
			case 2:
				result = FMT_32_32_FLOAT;
				goto out_word4;
			case 4:
				result = FMT_32_32_32_32_FLOAT;
				goto out_word4;
			}
		}
		goto out_unknown;
	}

out_word4:

	if (desc->colorspace == UTIL_FORMAT_COLORSPACE_SRGB && !is_srgb_valid)
		return ~0;
	if (word4_p)
		*word4_p = word4;
	if (yuv_format_p)
		*yuv_format_p = yuv_format;
	return result;
out_unknown:
	/* R600_ERR("Unable to handle texformat %d %s\n", format, util_format_name(format)); */
	return ~0;
}

static const struct u_resource_vtbl r600_texture_vtbl =
{
	r600_texture_get_handle,	/* get_handle */
	r600_texture_destroy,		/* resource_destroy */
	r600_texture_transfer_map,	/* transfer_map */
	NULL,				/* transfer_flush_region */
	r600_texture_transfer_unmap,	/* transfer_unmap */
	NULL				/* transfer_inline_write */
};
