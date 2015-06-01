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
#include <errno.h>
#include "pipe/p_screen.h"
#include "util/u_format.h"
#include "util/u_format_s3tc.h"
#include "util/u_math.h"
#include "util/u_inlines.h"
#include "util/u_memory.h"
#include "pipebuffer/pb_buffer.h"
#include "radeonsi_pipe.h"
#include "r600_resource.h"
#include "sid.h"

/* Copy from a full GPU texture to a transfer's staging one. */
static void r600_copy_to_staging_texture(struct pipe_context *ctx, struct r600_transfer *rtransfer)
{
	struct pipe_transfer *transfer = (struct pipe_transfer*)rtransfer;
	struct pipe_resource *texture = transfer->resource;

	ctx->resource_copy_region(ctx, rtransfer->staging,
				0, 0, 0, 0, texture, transfer->level,
				&transfer->box);
}

/* Copy from a transfer's staging texture to a full GPU one. */
static void r600_copy_from_staging_texture(struct pipe_context *ctx, struct r600_transfer *rtransfer)
{
	struct pipe_transfer *transfer = (struct pipe_transfer*)rtransfer;
	struct pipe_resource *texture = transfer->resource;
	struct pipe_box sbox;

	u_box_3d(0, 0, 0, transfer->box.width, transfer->box.height, transfer->box.depth, &sbox);

	ctx->resource_copy_region(ctx, texture, transfer->level,
				  transfer->box.x, transfer->box.y, transfer->box.z,
				  rtransfer->staging,
				  0, &sbox);
}

static unsigned r600_texture_get_offset(struct r600_resource_texture *rtex,
					unsigned level, unsigned layer)
{
	return rtex->surface.level[level].offset +
	       layer * rtex->surface.level[level].slice_size;
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

	if (!is_flushed_depth &&
	    ptex->format == PIPE_FORMAT_Z32_FLOAT_S8X24_UINT) {
		surface->bpe = 4; /* stencil is allocated separately on evergreen */
	} else {
		surface->bpe = util_format_get_blocksize(ptex->format);
		/* align byte per element on dword */
		if (surface->bpe == 3) {
			surface->bpe = 4;
		}
	}

	surface->nsamples = 1;
	surface->flags = 0;
	switch (array_mode) {
	case V_009910_ARRAY_1D_TILED_THIN1:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_1D, MODE);
		break;
	case V_009910_ARRAY_2D_TILED_THIN1:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_2D, MODE);
		break;
	case V_009910_ARRAY_LINEAR_ALIGNED:
		surface->flags |= RADEON_SURF_SET(RADEON_SURF_MODE_LINEAR_ALIGNED, MODE);
		break;
	case V_009910_ARRAY_LINEAR_GENERAL:
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
	surface->flags |= RADEON_SURF_HAS_TILE_MODE_INDEX;
	return 0;
}

static int r600_setup_surface(struct pipe_screen *screen,
			      struct r600_resource_texture *rtex,
			      unsigned array_mode,
			      unsigned pitch_in_bytes_override)
{
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	int r;

	r = rscreen->ws->surface_init(rscreen->ws, &rtex->surface);
	if (r) {
		return r;
	}
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
	return 0;
}

static boolean r600_texture_get_handle(struct pipe_screen* screen,
					struct pipe_resource *ptex,
					struct winsys_handle *whandle)
{
	struct r600_resource_texture *rtex = (struct r600_resource_texture*)ptex;
	struct si_resource *resource = &rtex->resource;
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
				       surface->level[0].pitch_bytes);

	return rscreen->ws->buffer_get_handle(resource->buf,
					      surface->level[0].pitch_bytes, whandle);
}

static void r600_texture_destroy(struct pipe_screen *screen,
				 struct pipe_resource *ptex)
{
	struct r600_resource_texture *rtex = (struct r600_resource_texture*)ptex;
	struct si_resource *resource = &rtex->resource;

	if (rtex->flushed_depth_texture)
		si_resource_reference((struct si_resource **)&rtex->flushed_depth_texture, NULL);

	pb_reference(&resource->buf, NULL);
	FREE(rtex);
}

static void *si_texture_transfer_map(struct pipe_context *ctx,
				     struct pipe_resource *texture,
				     unsigned level,
				     unsigned usage,
				     const struct pipe_box *box,
				     struct pipe_transfer **ptransfer)
{
	struct r600_context *rctx = (struct r600_context *)ctx;
	struct r600_resource_texture *rtex = (struct r600_resource_texture*)texture;
	struct r600_transfer *trans;
	boolean use_staging_texture = FALSE;
	struct radeon_winsys_cs_handle *buf;
	enum pipe_format format = texture->format;
	unsigned offset = 0;
	char *map;

	/* We cannot map a tiled texture directly because the data is
	 * in a different order, therefore we do detiling using a blit.
	 *
	 * Also, use a temporary in GTT memory for read transfers, as
	 * the CPU is much happier reading out of cached system memory
	 * than uncached VRAM.
	 */
	if (rtex->surface.level[level].mode != RADEON_SURF_MODE_LINEAR_ALIGNED &&
	    rtex->surface.level[level].mode != RADEON_SURF_MODE_LINEAR)
		use_staging_texture = TRUE;

	/* XXX: Use a staging texture for uploads if the underlying BO
	 * is busy.  No interface for checking that currently? so do
	 * it eagerly whenever the transfer doesn't require a readback
	 * and might block.
	 */
	if ((usage & PIPE_TRANSFER_WRITE) &&
			!(usage & (PIPE_TRANSFER_READ |
					PIPE_TRANSFER_DONTBLOCK |
					PIPE_TRANSFER_UNSYNCHRONIZED)))
		use_staging_texture = TRUE;

	if (texture->flags & R600_RESOURCE_FLAG_TRANSFER)
		use_staging_texture = FALSE;

	if (use_staging_texture && (usage & PIPE_TRANSFER_MAP_DIRECTLY))
		return NULL;

	trans = CALLOC_STRUCT(r600_transfer);
	if (trans == NULL)
		return NULL;
	pipe_resource_reference(&trans->transfer.resource, texture);
	trans->transfer.level = level;
	trans->transfer.usage = usage;
	trans->transfer.box = *box;
	if (rtex->is_depth) {
		/* XXX: only readback the rectangle which is being mapped?
		*/
		/* XXX: when discard is true, no need to read back from depth texture
		*/
		struct r600_resource_texture *staging_depth;

		if (!r600_init_flushed_depth_texture(ctx, texture, &staging_depth)) {
			R600_ERR("failed to create temporary texture to hold untiled copy\n");
			pipe_resource_reference(&trans->transfer.resource, NULL);
			FREE(trans);
			return NULL;
		}
		si_blit_uncompress_depth(ctx, rtex, staging_depth,
					 level, level,
					 box->z, box->z + box->depth - 1);
		trans->transfer.stride = staging_depth->surface.level[level].pitch_bytes;
		trans->transfer.layer_stride = staging_depth->surface.level[level].slice_size;
		trans->offset = r600_texture_get_offset(staging_depth, level, box->z);

		trans->staging = &staging_depth->resource.b.b;
	} else if (use_staging_texture) {
		struct pipe_resource resource;
		struct r600_resource_texture *staging;

		memset(&resource, 0, sizeof(resource));
		resource.format = texture->format;
		resource.width0 = box->width;
		resource.height0 = box->height;
		resource.depth0 = 1;
		resource.array_size = 1;
		resource.usage = PIPE_USAGE_STAGING;
		resource.flags = R600_RESOURCE_FLAG_TRANSFER;

		/* We must set the correct texture target and dimensions if needed for a 3D transfer. */
		if (box->depth > 1 && util_max_layer(texture, level) > 0)
			resource.target = texture->target;
		else
			resource.target = PIPE_TEXTURE_2D;

		switch (resource.target) {
		case PIPE_TEXTURE_1D_ARRAY:
		case PIPE_TEXTURE_2D_ARRAY:
		case PIPE_TEXTURE_CUBE_ARRAY:
			resource.array_size = box->depth;
			break;
		case PIPE_TEXTURE_3D:
			resource.depth0 = box->depth;
			break;
		default:;
		}
		/* Create the temporary texture. */
		staging = (struct r600_resource_texture*)ctx->screen->resource_create(ctx->screen, &resource);
		if (staging == NULL) {
			R600_ERR("failed to create temporary texture to hold untiled copy\n");
			pipe_resource_reference(&trans->transfer.resource, NULL);
			FREE(trans);
			return NULL;
		}

		trans->staging = &staging->resource.b.b;
		trans->transfer.stride = staging->surface.level[0].pitch_bytes;
		trans->transfer.layer_stride = staging->surface.level[0].slice_size;
		if (usage & PIPE_TRANSFER_READ) {
			r600_copy_to_staging_texture(ctx, trans);
			/* Always referenced in the blit. */
			radeonsi_flush(ctx, NULL, 0);
		}
	} else {
		trans->transfer.stride = rtex->surface.level[level].pitch_bytes;
		trans->transfer.layer_stride = rtex->surface.level[level].slice_size;
		trans->offset = r600_texture_get_offset(rtex, level, box->z);
	}

	if (trans->staging) {
		buf = si_resource(trans->staging)->cs_buf;
	} else {
		buf = rtex->resource.cs_buf;
	}

	if (rtex->is_depth || !trans->staging)
		offset = trans->offset +
			box->y / util_format_get_blockheight(format) * trans->transfer.stride +
			box->x / util_format_get_blockwidth(format) * util_format_get_blocksize(format);

	if (!(map = rctx->ws->buffer_map(buf, rctx->cs, usage))) {
		pipe_resource_reference(&trans->staging, NULL);
		pipe_resource_reference(&trans->transfer.resource, NULL);
		FREE(trans);
		return NULL;
	}

	*ptransfer = &trans->transfer;
	return map + offset;
}

static void si_texture_transfer_unmap(struct pipe_context *ctx,
				      struct pipe_transfer* transfer)
{
	struct r600_transfer *rtransfer = (struct r600_transfer*)transfer;
	struct r600_context *rctx = (struct r600_context*)ctx;
	struct radeon_winsys_cs_handle *buf;
	struct pipe_resource *texture = transfer->resource;
	struct r600_resource_texture *rtex = (struct r600_resource_texture*)texture;

	if (rtransfer->staging) {
		buf = si_resource(rtransfer->staging)->cs_buf;
	} else {
		buf = si_resource(transfer->resource)->cs_buf;
	}
	rctx->ws->buffer_unmap(buf);

	if ((transfer->usage & PIPE_TRANSFER_WRITE) && rtransfer->staging) {
		if (rtex->is_depth) {
			ctx->resource_copy_region(ctx, texture, transfer->level,
						  transfer->box.x, transfer->box.y, transfer->box.z,
						  &si_resource(rtransfer->staging)->b.b, transfer->level,
						  &transfer->box);
		} else {
			r600_copy_from_staging_texture(ctx, rtransfer);
		}
	}

	if (rtransfer->staging)
		pipe_resource_reference((struct pipe_resource**)&rtransfer->staging, NULL);

	pipe_resource_reference(&transfer->resource, NULL);
	FREE(transfer);
}

static const struct u_resource_vtbl r600_texture_vtbl =
{
	r600_texture_get_handle,	/* get_handle */
	r600_texture_destroy,		/* resource_destroy */
	si_texture_transfer_map,	/* transfer_map */
	u_default_transfer_flush_region,/* transfer_flush_region */
	si_texture_transfer_unmap,	/* transfer_unmap */
	NULL	/* transfer_inline_write */
};

DEBUG_GET_ONCE_BOOL_OPTION(print_texdepth, "RADEON_PRINT_TEXDEPTH", FALSE);

static struct r600_resource_texture *
r600_texture_create_object(struct pipe_screen *screen,
			   const struct pipe_resource *base,
			   unsigned array_mode,
			   unsigned pitch_in_bytes_override,
			   unsigned max_buffer_size,
			   struct pb_buffer *buf,
			   boolean alloc_bo,
			   struct radeon_surface *surface)
{
	struct r600_resource_texture *rtex;
	struct si_resource *resource;
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	int r;

	rtex = CALLOC_STRUCT(r600_resource_texture);
	if (rtex == NULL)
		return NULL;

	resource = &rtex->resource;
	resource->b.b = *base;
	resource->b.vtbl = &r600_texture_vtbl;
	pipe_reference_init(&resource->b.b.reference, 1);
	resource->b.b.screen = screen;
	rtex->pitch_override = pitch_in_bytes_override;
	rtex->real_format = base->format;

	/* don't include stencil-only formats which we don't support for rendering */
	rtex->is_depth = util_format_has_depth(util_format_description(rtex->resource.b.b.format));

	rtex->surface = *surface;
	r = r600_setup_surface(screen, rtex, array_mode, pitch_in_bytes_override);
	if (r) {
		FREE(rtex);
		return NULL;
	}

	/* Now create the backing buffer. */
	if (!buf && alloc_bo) {
		unsigned base_align = rtex->surface.bo_alignment;
		unsigned size = rtex->surface.bo_size;

		base_align = rtex->surface.bo_alignment;
		if (!si_init_resource(rscreen, resource, size, base_align, FALSE, base->usage)) {
			FREE(rtex);
			return NULL;
		}
	} else if (buf) {
		resource->buf = buf;
		resource->cs_buf = rscreen->ws->buffer_get_cs_handle(buf);
		resource->domains = RADEON_DOMAIN_GTT | RADEON_DOMAIN_VRAM;
	}

	if (debug_get_option_print_texdepth() && rtex->is_depth) {
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
				       i, rtex->surface.level[i].offset,
				       rtex->surface.level[i].slice_size,
				       rtex->surface.level[i].npix_x,
				       rtex->surface.level[i].npix_y,
				       rtex->surface.level[i].npix_z,
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
				       i, rtex->surface.stencil_level[i].offset,
				       rtex->surface.stencil_level[i].slice_size,
				       rtex->surface.stencil_level[i].npix_x,
				       rtex->surface.stencil_level[i].npix_y,
				       rtex->surface.stencil_level[i].npix_z,
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

struct pipe_resource *si_texture_create(struct pipe_screen *screen,
					const struct pipe_resource *templ)
{
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	struct radeon_surface surface;
	unsigned array_mode = V_009910_ARRAY_LINEAR_ALIGNED;
	int r;

	if (!(templ->flags & R600_RESOURCE_FLAG_TRANSFER) &&
	    !(templ->bind & PIPE_BIND_SCANOUT)) {
		if (util_format_is_compressed(templ->format)) {
			array_mode = V_009910_ARRAY_1D_TILED_THIN1;
		} else {
			if (rscreen->chip_class >= CIK)
				array_mode = V_009910_ARRAY_1D_TILED_THIN1; /* XXX fix me */
			else
				array_mode = V_009910_ARRAY_2D_TILED_THIN1;
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
	return (struct pipe_resource *)r600_texture_create_object(screen, templ, array_mode,
								  0, 0, NULL, TRUE, &surface);
}

static struct pipe_surface *r600_create_surface(struct pipe_context *pipe,
						struct pipe_resource *texture,
						const struct pipe_surface *surf_tmpl)
{
	struct r600_resource_texture *rtex = (struct r600_resource_texture*)texture;
	struct r600_surface *surface = CALLOC_STRUCT(r600_surface);
	unsigned level = surf_tmpl->u.tex.level;

	assert(surf_tmpl->u.tex.first_layer <= util_max_layer(texture, surf_tmpl->u.tex.level));
	assert(surf_tmpl->u.tex.last_layer <= util_max_layer(texture, surf_tmpl->u.tex.level));
	assert(surf_tmpl->u.tex.first_layer == surf_tmpl->u.tex.last_layer);
	if (surface == NULL)
		return NULL;
	/* XXX no offset */
/*	offset = r600_texture_get_offset(rtex, level, surf_tmpl->u.tex.first_layer);*/
	pipe_reference_init(&surface->base.reference, 1);
	pipe_resource_reference(&surface->base.texture, texture);
	surface->base.context = pipe;
	surface->base.format = surf_tmpl->format;
	surface->base.width = rtex->surface.level[level].npix_x;
	surface->base.height = rtex->surface.level[level].npix_y;
	surface->base.texture = texture;
	surface->base.u.tex.first_layer = surf_tmpl->u.tex.first_layer;
	surface->base.u.tex.last_layer = surf_tmpl->u.tex.last_layer;
	surface->base.u.tex.level = level;

	return &surface->base;
}

static void r600_surface_destroy(struct pipe_context *pipe,
				 struct pipe_surface *surface)
{
	pipe_resource_reference(&surface->texture, NULL);
	FREE(surface);
}

struct pipe_resource *si_texture_from_handle(struct pipe_screen *screen,
					     const struct pipe_resource *templ,
					     struct winsys_handle *whandle)
{
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	struct pb_buffer *buf = NULL;
	unsigned stride = 0;
	unsigned array_mode = V_009910_ARRAY_LINEAR_ALIGNED;
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
		array_mode = V_009910_ARRAY_2D_TILED_THIN1;
	else if (micro == RADEON_LAYOUT_TILED)
		array_mode = V_009910_ARRAY_1D_TILED_THIN1;
	else
		array_mode = V_009910_ARRAY_LINEAR_ALIGNED;

	r = r600_init_surface(rscreen, &surface, templ, array_mode, false);
	if (r) {
		return NULL;
	}
	/* always set the scanout flags */
	surface.flags |= RADEON_SURF_SCANOUT;
	return (struct pipe_resource *)r600_texture_create_object(screen, templ, array_mode,
								  stride, 0, buf, FALSE, &surface);
}

bool r600_init_flushed_depth_texture(struct pipe_context *ctx,
				     struct pipe_resource *texture,
				     struct r600_resource_texture **staging)
{
	struct r600_resource_texture *rtex = (struct r600_resource_texture*)texture;
	struct pipe_resource resource;
	struct r600_resource_texture **flushed_depth_texture = staging ?
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
	resource.usage = staging ? PIPE_USAGE_DYNAMIC : PIPE_USAGE_DEFAULT;
	resource.bind = texture->bind & ~PIPE_BIND_DEPTH_STENCIL;
	resource.flags = texture->flags | R600_RESOURCE_FLAG_FLUSHED_DEPTH;

	if (staging)
		resource.flags |= R600_RESOURCE_FLAG_TRANSFER;
	else
		rtex->dirty_db_mask = (1 << (resource.last_level+1)) - 1;

	*flushed_depth_texture = (struct r600_resource_texture *)ctx->screen->resource_create(ctx->screen, &resource);
	if (*flushed_depth_texture == NULL) {
		R600_ERR("failed to create temporary texture to hold flushed depth\n");
		return false;
	}

	(*flushed_depth_texture)->is_flushing_texture = TRUE;
	return true;
}

void si_init_surface_functions(struct r600_context *r600)
{
	r600->context.create_surface = r600_create_surface;
	r600->context.surface_destroy = r600_surface_destroy;
}
