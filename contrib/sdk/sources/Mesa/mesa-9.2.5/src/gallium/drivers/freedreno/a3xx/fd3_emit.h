/* -*- mode: C; c-file-style: "k&r"; tab-width 4; indent-tabs-mode: t; -*- */

/*
 * Copyright (C) 2013 Rob Clark <robclark@freedesktop.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Rob Clark <robclark@freedesktop.org>
 */

#ifndef FD3_EMIT_H
#define FD3_EMIT_H

#include "pipe/p_context.h"

#include "freedreno_context.h"
#include "fd3_util.h"


struct fd_ringbuffer;
enum adreno_state_block;

void fd3_emit_constant(struct fd_ringbuffer *ring,
		enum adreno_state_block sb,
		uint32_t regid, uint32_t offset, uint32_t sizedwords,
		const uint32_t *dwords, struct pipe_resource *prsc);

void fd3_emit_gmem_restore_tex(struct fd_ringbuffer *ring,
		struct pipe_surface *psurf);

/* NOTE: this just exists because we don't have proper vertex/vertexbuf
 * state objs for clear, and mem2gmem/gmem2mem operations..
 */
struct fd3_vertex_buf {
	unsigned offset, stride;
	struct pipe_resource *prsc;
	enum pipe_format format;
};

void fd3_emit_vertex_bufs(struct fd_ringbuffer *ring,
		struct fd_program_stateobj *prog,
		struct fd3_vertex_buf *vbufs, uint32_t n);
void fd3_emit_state(struct fd_context *ctx, uint32_t dirty);
void fd3_emit_restore(struct fd_context *ctx);


/* use RMW (read-modify-write) to update RB_RENDER_CONTROL since the
 * GMEM/binning code is deciding on the bin-width (and whether to
 * use binning) after the draw/clear state is emitted.
 */
static inline void
fd3_emit_rbrc_draw_state(struct fd_ringbuffer *ring, uint32_t val)
{
	OUT_PKT3(ring, CP_REG_RMW, 3);
	OUT_RING(ring, REG_A3XX_RB_RENDER_CONTROL);
	OUT_RING(ring, A3XX_RB_RENDER_CONTROL_BIN_WIDTH__MASK |
			A3XX_RB_RENDER_CONTROL_ENABLE_GMEM);
	OUT_RING(ring, val);
}

static inline void
fd3_emit_rbrc_tile_state(struct fd_ringbuffer *ring, uint32_t val)
{
	OUT_PKT3(ring, CP_REG_RMW, 3);
	OUT_RING(ring, REG_A3XX_RB_RENDER_CONTROL);
	OUT_RING(ring, ~(A3XX_RB_RENDER_CONTROL_BIN_WIDTH__MASK |
			A3XX_RB_RENDER_CONTROL_ENABLE_GMEM));
	OUT_RING(ring, val);
}

#endif /* FD3_EMIT_H */
