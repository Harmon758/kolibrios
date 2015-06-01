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
 */

#include "sid.h"
#include "si_pipe.h"
#include "radeon/r600_cs.h"

#include "util/u_format.h"

static unsigned si_array_mode(unsigned mode)
{
	switch (mode) {
	case RADEON_SURF_MODE_LINEAR_ALIGNED:
		return V_009910_ARRAY_LINEAR_ALIGNED;
	case RADEON_SURF_MODE_1D:
		return V_009910_ARRAY_1D_TILED_THIN1;
	case RADEON_SURF_MODE_2D:
		return V_009910_ARRAY_2D_TILED_THIN1;
	default:
	case RADEON_SURF_MODE_LINEAR:
		return V_009910_ARRAY_LINEAR_GENERAL;
	}
}

static uint32_t si_micro_tile_mode(struct si_screen *sscreen, unsigned tile_mode)
{
	if (sscreen->b.info.si_tile_mode_array_valid) {
		uint32_t gb_tile_mode = sscreen->b.info.si_tile_mode_array[tile_mode];

		return G_009910_MICRO_TILE_MODE(gb_tile_mode);
	}

	/* The kernel cannod return the tile mode array. Guess? */
	return V_009910_ADDR_SURF_THIN_MICRO_TILING;
}

static void si_dma_copy_buffer(struct si_context *ctx,
				struct pipe_resource *dst,
				struct pipe_resource *src,
				uint64_t dst_offset,
				uint64_t src_offset,
				uint64_t size)
{
	struct radeon_winsys_cs *cs = ctx->b.rings.dma.cs;
	unsigned i, ncopy, csize, max_csize, sub_cmd, shift;
	struct r600_resource *rdst = (struct r600_resource*)dst;
	struct r600_resource *rsrc = (struct r600_resource*)src;

	/* Mark the buffer range of destination as valid (initialized),
	 * so that transfer_map knows it should wait for the GPU when mapping
	 * that range. */
	util_range_add(&rdst->valid_buffer_range, dst_offset,
		       dst_offset + size);

	dst_offset += rdst->gpu_address;
	src_offset += rsrc->gpu_address;

	/* see if we use dword or byte copy */
	if (!(dst_offset % 4) && !(src_offset % 4) && !(size % 4)) {
		size >>= 2;
		sub_cmd = SI_DMA_COPY_DWORD_ALIGNED;
		shift = 2;
		max_csize = SI_DMA_COPY_MAX_SIZE_DW;
	} else {
		sub_cmd = SI_DMA_COPY_BYTE_ALIGNED;
		shift = 0;
		max_csize = SI_DMA_COPY_MAX_SIZE;
	}
	ncopy = (size / max_csize) + !!(size % max_csize);

	r600_need_dma_space(&ctx->b, ncopy * 5);

	r600_context_bo_reloc(&ctx->b, &ctx->b.rings.dma, rsrc, RADEON_USAGE_READ,
			      RADEON_PRIO_MIN);
	r600_context_bo_reloc(&ctx->b, &ctx->b.rings.dma, rdst, RADEON_USAGE_WRITE,
			      RADEON_PRIO_MIN);

	for (i = 0; i < ncopy; i++) {
		csize = size < max_csize ? size : max_csize;
		cs->buf[cs->cdw++] = SI_DMA_PACKET(SI_DMA_PACKET_COPY, sub_cmd, csize);
		cs->buf[cs->cdw++] = dst_offset & 0xffffffff;
		cs->buf[cs->cdw++] = src_offset & 0xffffffff;
		cs->buf[cs->cdw++] = (dst_offset >> 32UL) & 0xff;
		cs->buf[cs->cdw++] = (src_offset >> 32UL) & 0xff;
		dst_offset += csize << shift;
		src_offset += csize << shift;
		size -= csize;
	}
}

static void si_dma_copy_tile(struct si_context *ctx,
			     struct pipe_resource *dst,
			     unsigned dst_level,
			     unsigned dst_x,
			     unsigned dst_y,
			     unsigned dst_z,
			     struct pipe_resource *src,
			     unsigned src_level,
			     unsigned src_x,
			     unsigned src_y,
			     unsigned src_z,
			     unsigned copy_height,
			     unsigned pitch,
			     unsigned bpp)
{
	struct radeon_winsys_cs *cs = ctx->b.rings.dma.cs;
	struct si_screen *sscreen = ctx->screen;
	struct r600_texture *rsrc = (struct r600_texture*)src;
	struct r600_texture *rdst = (struct r600_texture*)dst;
	struct r600_texture *rlinear, *rtiled;
	unsigned linear_lvl, tiled_lvl;
	unsigned array_mode, lbpp, pitch_tile_max, slice_tile_max, size;
	unsigned ncopy, height, cheight, detile, i, src_mode, dst_mode;
	unsigned linear_x, linear_y, linear_z,  tiled_x, tiled_y, tiled_z;
	unsigned sub_cmd, bank_h, bank_w, mt_aspect, nbanks, tile_split, mt;
	uint64_t base, addr;
	unsigned pipe_config, tile_mode_index;

	dst_mode = rdst->surface.level[dst_level].mode;
	src_mode = rsrc->surface.level[src_level].mode;
	/* downcast linear aligned to linear to simplify test */
	src_mode = src_mode == RADEON_SURF_MODE_LINEAR_ALIGNED ? RADEON_SURF_MODE_LINEAR : src_mode;
	dst_mode = dst_mode == RADEON_SURF_MODE_LINEAR_ALIGNED ? RADEON_SURF_MODE_LINEAR : dst_mode;
	assert(dst_mode != src_mode);

	sub_cmd = SI_DMA_COPY_TILED;
	lbpp = util_logbase2(bpp);
	pitch_tile_max = ((pitch / bpp) / 8) - 1;

	detile = dst_mode == RADEON_SURF_MODE_LINEAR;
	rlinear = detile ? rdst : rsrc;
	rtiled = detile ? rsrc : rdst;
	linear_lvl = detile ? dst_level : src_level;
	tiled_lvl = detile ? src_level : dst_level;
	linear_x = detile ? dst_x : src_x;
	linear_y = detile ? dst_y : src_y;
	linear_z = detile ? dst_z : src_z;
	tiled_x = detile ? src_x : dst_x;
	tiled_y = detile ? src_y : dst_y;
	tiled_z = detile ? src_z : dst_z;

	assert(!util_format_is_depth_and_stencil(rtiled->resource.b.b.format));

	array_mode = si_array_mode(rtiled->surface.level[tiled_lvl].mode);
	slice_tile_max = (rtiled->surface.level[tiled_lvl].nblk_x *
			  rtiled->surface.level[tiled_lvl].nblk_y) / (8*8) - 1;
	/* linear height must be the same as the slice tile max height, it's ok even
	 * if the linear destination/source have smaller heigh as the size of the
	 * dma packet will be using the copy_height which is always smaller or equal
	 * to the linear height
	 */
	height = rtiled->surface.level[tiled_lvl].nblk_y;
	base = rtiled->surface.level[tiled_lvl].offset;
	addr = rlinear->surface.level[linear_lvl].offset;
	addr += rlinear->surface.level[linear_lvl].slice_size * linear_z;
	addr += linear_y * pitch + linear_x * bpp;
	bank_h = cik_bank_wh(rtiled->surface.bankh);
	bank_w = cik_bank_wh(rtiled->surface.bankw);
	mt_aspect = cik_macro_tile_aspect(rtiled->surface.mtilea);
	tile_split = cik_tile_split(rtiled->surface.tile_split);
	tile_mode_index = si_tile_mode_index(rtiled, tiled_lvl, false);
	nbanks = si_num_banks(sscreen, rtiled);
	base += rtiled->resource.gpu_address;
	addr += rlinear->resource.gpu_address;

	pipe_config = cik_db_pipe_config(sscreen, tile_mode_index);
	mt = si_micro_tile_mode(sscreen, tile_mode_index);
	size = (copy_height * pitch) / 4;
	ncopy = (size / SI_DMA_COPY_MAX_SIZE_DW) + !!(size % SI_DMA_COPY_MAX_SIZE_DW);
	r600_need_dma_space(&ctx->b, ncopy * 9);

	r600_context_bo_reloc(&ctx->b, &ctx->b.rings.dma, &rsrc->resource,
			      RADEON_USAGE_READ, RADEON_PRIO_MIN);
	r600_context_bo_reloc(&ctx->b, &ctx->b.rings.dma, &rdst->resource,
			      RADEON_USAGE_WRITE, RADEON_PRIO_MIN);

	for (i = 0; i < ncopy; i++) {
		cheight = copy_height;
		if (((cheight * pitch) / 4) > SI_DMA_COPY_MAX_SIZE_DW) {
			cheight = (SI_DMA_COPY_MAX_SIZE_DW * 4) / pitch;
		}
		size = (cheight * pitch) / 4;
		cs->buf[cs->cdw++] = SI_DMA_PACKET(SI_DMA_PACKET_COPY, sub_cmd, size);
		cs->buf[cs->cdw++] = base >> 8;
		cs->buf[cs->cdw++] = (detile << 31) | (array_mode << 27) |
					(lbpp << 24) | (bank_h << 21) |
					(bank_w << 18) | (mt_aspect << 16);
		cs->buf[cs->cdw++] = (pitch_tile_max << 0) | ((height - 1) << 16);
		cs->buf[cs->cdw++] = (slice_tile_max << 0) | (pipe_config << 26);
		cs->buf[cs->cdw++] = (tiled_x << 0) | (tiled_z << 18);
		cs->buf[cs->cdw++] = (tiled_y << 0) | (tile_split << 21) | (nbanks << 25) | (mt << 27);
		cs->buf[cs->cdw++] = addr & 0xfffffffc;
		cs->buf[cs->cdw++] = (addr >> 32UL) & 0xff;
		copy_height -= cheight;
		addr += cheight * pitch;
		tiled_y += cheight;
	}
}

void si_dma_copy(struct pipe_context *ctx,
		 struct pipe_resource *dst,
		 unsigned dst_level,
		 unsigned dstx, unsigned dsty, unsigned dstz,
		 struct pipe_resource *src,
		 unsigned src_level,
		 const struct pipe_box *src_box)
{
	struct si_context *sctx = (struct si_context *)ctx;
	struct r600_texture *rsrc = (struct r600_texture*)src;
	struct r600_texture *rdst = (struct r600_texture*)dst;
	unsigned dst_pitch, src_pitch, bpp, dst_mode, src_mode;
	unsigned src_w, dst_w;
	unsigned src_x, src_y;
	unsigned dst_x = dstx, dst_y = dsty, dst_z = dstz;

	if (sctx->b.rings.dma.cs == NULL) {
		goto fallback;
	}

	/* TODO: Implement DMA copy for CIK */
	if (sctx->b.chip_class >= CIK) {
		goto fallback;
	}

	if (dst->target == PIPE_BUFFER && src->target == PIPE_BUFFER) {
		si_dma_copy_buffer(sctx, dst, src, dst_x, src_box->x, src_box->width);
		return;
	}

	/* XXX: Using the asynchronous DMA engine for multi-dimensional
	 * operations seems to cause random GPU lockups for various people.
	 * While the root cause for this might need to be fixed in the kernel,
	 * let's disable it for now.
	 *
	 * Before re-enabling this, please make sure you can hit all newly
	 * enabled paths in your testing, preferably with both piglit and real
	 * world apps, and get in touch with people on the bug reports below
	 * for stability testing.
	 *
	 * https://bugs.freedesktop.org/show_bug.cgi?id=85647
	 * https://bugs.freedesktop.org/show_bug.cgi?id=83500
	 */
	goto fallback;

	if (src->format != dst->format || src_box->depth > 1 ||
	    rdst->dirty_level_mask != 0 ||
	    rdst->cmask.size || rdst->fmask.size ||
	    rsrc->cmask.size || rsrc->fmask.size) {
		goto fallback;
	}

	if (rsrc->dirty_level_mask) {
		ctx->flush_resource(ctx, src);
	}

	src_x = util_format_get_nblocksx(src->format, src_box->x);
	dst_x = util_format_get_nblocksx(src->format, dst_x);
	src_y = util_format_get_nblocksy(src->format, src_box->y);
	dst_y = util_format_get_nblocksy(src->format, dst_y);

	bpp = rdst->surface.bpe;
	dst_pitch = rdst->surface.level[dst_level].pitch_bytes;
	src_pitch = rsrc->surface.level[src_level].pitch_bytes;
	src_w = rsrc->surface.level[src_level].npix_x;
	dst_w = rdst->surface.level[dst_level].npix_x;

	dst_mode = rdst->surface.level[dst_level].mode;
	src_mode = rsrc->surface.level[src_level].mode;
	/* downcast linear aligned to linear to simplify test */
	src_mode = src_mode == RADEON_SURF_MODE_LINEAR_ALIGNED ? RADEON_SURF_MODE_LINEAR : src_mode;
	dst_mode = dst_mode == RADEON_SURF_MODE_LINEAR_ALIGNED ? RADEON_SURF_MODE_LINEAR : dst_mode;

	if (src_pitch != dst_pitch || src_box->x || dst_x || src_w != dst_w ||
	    src_box->width != src_w ||
	    src_box->height != rsrc->surface.level[src_level].npix_y ||
	    src_box->height != rdst->surface.level[dst_level].npix_y ||
	    rsrc->surface.level[src_level].nblk_y !=
	    rdst->surface.level[dst_level].nblk_y) {
		/* FIXME si can do partial blit */
		goto fallback;
	}
	/* the x test here are currently useless (because we don't support partial blit)
	 * but keep them around so we don't forget about those
	 */
	if ((src_pitch % 8) || (src_box->x % 8) || (dst_x % 8) ||
	    (src_box->y % 8) || (dst_y % 8) || (src_box->height % 8)) {
		goto fallback;
	}

	if (src_mode == dst_mode) {
		uint64_t dst_offset, src_offset;
		/* simple dma blit would do NOTE code here assume :
		 *   src_box.x/y == 0
		 *   dst_x/y == 0
		 *   dst_pitch == src_pitch
		 */
		src_offset= rsrc->surface.level[src_level].offset;
		src_offset += rsrc->surface.level[src_level].slice_size * src_box->z;
		src_offset += src_y * src_pitch + src_x * bpp;
		dst_offset = rdst->surface.level[dst_level].offset;
		dst_offset += rdst->surface.level[dst_level].slice_size * dst_z;
		dst_offset += dst_y * dst_pitch + dst_x * bpp;
		si_dma_copy_buffer(sctx, dst, src, dst_offset, src_offset,
				   rsrc->surface.level[src_level].slice_size);
	} else {
		si_dma_copy_tile(sctx, dst, dst_level, dst_x, dst_y, dst_z,
				 src, src_level, src_x, src_y, src_box->z,
				 src_box->height / rsrc->surface.blk_h,
				 dst_pitch, bpp);
	}
	return;

fallback:
	si_resource_copy_region(ctx, dst, dst_level, dstx, dsty, dstz,
				src, src_level, src_box);
}
