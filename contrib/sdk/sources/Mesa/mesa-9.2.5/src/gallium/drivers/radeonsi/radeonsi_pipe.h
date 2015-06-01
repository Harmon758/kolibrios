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
#ifndef RADEONSI_PIPE_H
#define RADEONSI_PIPE_H

#include "../../winsys/radeon/drm/radeon_winsys.h"

#include "pipe/p_state.h"
#include "pipe/p_screen.h"
#include "pipe/p_context.h"
#include "util/u_format.h"
#include "util/u_math.h"
#include "util/u_slab.h"
#include "r600.h"
#include "radeonsi_public.h"
#include "radeonsi_pm4.h"
#include "si_state.h"
#include "r600_resource.h"
#include "sid.h"

#ifdef PIPE_ARCH_BIG_ENDIAN
#define R600_BIG_ENDIAN 1
#else
#define R600_BIG_ENDIAN 0
#endif

#define R600_TRACE_CS 0
#define R600_TRACE_CS_DWORDS		6

struct si_pipe_compute;

struct r600_pipe_fences {
	struct si_resource		*bo;
	unsigned			*data;
	unsigned			next_index;
	/* linked list of preallocated blocks */
	struct list_head		blocks;
	/* linked list of freed fences */
	struct list_head		pool;
	pipe_mutex			mutex;
};

struct r600_screen {
	struct pipe_screen		screen;
	struct radeon_winsys		*ws;
	unsigned			family;
	enum chip_class			chip_class;
	struct radeon_info		info;
	struct r600_tiling_info		tiling_info;
	struct util_slab_mempool	pool_buffers;
	struct r600_pipe_fences		fences;
#if R600_TRACE_CS
	struct si_resource		*trace_bo;
	uint32_t			*trace_ptr;
	unsigned			cs_count;
#endif
};

struct si_pipe_sampler_view {
	struct pipe_sampler_view	base;
	struct si_resource		*resource;
	uint32_t			state[8];
};

struct si_pipe_sampler_state {
	uint32_t			val[4];
	uint32_t			border_color[4];
};

struct si_cs_shader_state {
	struct si_pipe_compute		*program;
};

/* needed for blitter save */
#define NUM_TEX_UNITS 16

struct r600_textures_info {
	struct si_pipe_sampler_view	*views[NUM_TEX_UNITS];
	struct si_pipe_sampler_state	*samplers[NUM_TEX_UNITS];
	unsigned			n_views;
	uint32_t			depth_texture_mask; /* which textures are depth */
	unsigned			n_samplers;
	bool				samplers_dirty;
	bool				is_array_sampler[NUM_TEX_UNITS];
};

struct r600_fence {
	struct pipe_reference		reference;
	unsigned			index; /* in the shared bo */
	struct si_resource            *sleep_bo;
	struct list_head		head;
};

#define FENCE_BLOCK_SIZE 16

struct r600_fence_block {
	struct r600_fence		fences[FENCE_BLOCK_SIZE];
	struct list_head		head;
};

#define R600_CONSTANT_ARRAY_SIZE 256
#define R600_RESOURCE_ARRAY_SIZE 160

struct r600_constbuf_state
{
	struct pipe_constant_buffer	cb[2];
	uint32_t			enabled_mask;
	uint32_t			dirty_mask;
};

struct r600_context {
	struct pipe_context		context;
	struct blitter_context		*blitter;
	enum radeon_family		family;
	enum chip_class			chip_class;
	void				*custom_dsa_flush_depth_stencil;
	void				*custom_dsa_flush_depth;
	void				*custom_dsa_flush_stencil;
	void				*custom_dsa_flush_inplace;
	struct r600_screen		*screen;
	struct radeon_winsys		*ws;
	struct si_vertex_element	*vertex_elements;
	struct pipe_framebuffer_state	framebuffer;
	unsigned			pa_sc_line_stipple;
	unsigned			pa_su_sc_mode_cntl;
	/* for saving when using blitter */
	struct pipe_stencil_ref		stencil_ref;
	struct si_pipe_shader_selector	*ps_shader;
	struct si_pipe_shader_selector	*vs_shader;
	struct si_cs_shader_state	cs_shader_state;
	struct pipe_query		*current_render_cond;
	unsigned			current_render_cond_mode;
	boolean				current_render_cond_cond;
	struct pipe_query		*saved_render_cond;
	unsigned			saved_render_cond_mode;
	boolean				saved_render_cond_cond;
	/* shader information */
	unsigned			sprite_coord_enable;
	unsigned			export_16bpc;
	struct r600_constbuf_state	constbuf_state[PIPE_SHADER_TYPES];
	struct r600_textures_info	vs_samplers;
	struct r600_textures_info	ps_samplers;
	struct si_resource		*border_color_table;
	unsigned			border_color_offset;

	struct u_upload_mgr	        *uploader;
	struct util_slab_mempool	pool_transfers;

	unsigned default_ps_gprs, default_vs_gprs;

	/* Below are variables from the old r600_context.
	 */
	struct radeon_winsys_cs	*cs;

	unsigned		pm4_dirty_cdwords;

	/* The list of active queries. Only one query of each type can be active. */
	struct list_head	active_query_list;
	unsigned		num_cs_dw_queries_suspend;
	unsigned		num_cs_dw_streamout_end;

	unsigned		backend_mask;
	unsigned                max_db; /* for OQ */
	unsigned		flags;
	boolean                 predicate_drawing;

	unsigned		num_so_targets;
	struct r600_so_target	*so_targets[PIPE_MAX_SO_BUFFERS];
	boolean			streamout_start;
	unsigned		streamout_append_bitmask;
	unsigned		*vs_so_stride_in_dw;
	unsigned		*vs_shader_so_strides;

	/* Vertex and index buffers. */
	bool			vertex_buffers_dirty;
	struct pipe_index_buffer index_buffer;
	struct pipe_vertex_buffer vertex_buffer[PIPE_MAX_ATTRIBS];
	unsigned		nr_vertex_buffers;

	/* With rasterizer discard, there doesn't have to be a pixel shader.
	 * In that case, we bind this one: */
	struct si_pipe_shader	*dummy_pixel_shader;

	/* SI state handling */
	union si_state	queued;
	union si_state	emitted;
};

/* r600_blit.c */
void si_init_blit_functions(struct r600_context *rctx);
void si_blit_uncompress_depth(struct pipe_context *ctx,
		struct r600_resource_texture *texture,
		struct r600_resource_texture *staging,
		unsigned first_level, unsigned last_level,
		unsigned first_layer, unsigned last_layer);
void si_flush_depth_textures(struct r600_context *rctx,
			     struct r600_textures_info *textures);

/* r600_buffer.c */
bool si_init_resource(struct r600_screen *rscreen,
		      struct si_resource *res,
		      unsigned size, unsigned alignment,
		      boolean use_reusable_pool, unsigned usage);
struct pipe_resource *si_buffer_create(struct pipe_screen *screen,
				       const struct pipe_resource *templ);
void r600_upload_index_buffer(struct r600_context *rctx,
			      struct pipe_index_buffer *ib, unsigned count);


/* r600_pipe.c */
void radeonsi_flush(struct pipe_context *ctx, struct pipe_fence_handle **fence,
		    unsigned flags);
const char *r600_get_llvm_processor_name(enum radeon_family family);

/* r600_query.c */
void r600_init_query_functions(struct r600_context *rctx);

/* r600_resource.c */
void r600_init_context_resource_functions(struct r600_context *r600);

/* r600_texture.c */
void r600_init_screen_texture_functions(struct pipe_screen *screen);
void si_init_surface_functions(struct r600_context *r600);

/* r600_translate.c */
void r600_translate_index_buffer(struct r600_context *r600,
				 struct pipe_index_buffer *ib,
				 unsigned count);

#if R600_TRACE_CS
void r600_trace_emit(struct r600_context *rctx);
#endif

/* radeonsi_compute.c */
void si_init_compute_functions(struct r600_context *rctx);

/* radeonsi_uvd.c */
struct pipe_video_decoder *radeonsi_uvd_create_decoder(struct pipe_context *context,
						       enum pipe_video_profile profile,
						       enum pipe_video_entrypoint entrypoint,
						       enum pipe_video_chroma_format chroma_format,
						       unsigned width, unsigned height,
						       unsigned max_references, bool expect_chunked_decode);

struct pipe_video_buffer *radeonsi_video_buffer_create(struct pipe_context *pipe,
						       const struct pipe_video_buffer *tmpl);

/*
 * common helpers
 */
static INLINE uint32_t S_FIXED(float value, uint32_t frac_bits)
{
	return value * (1 << frac_bits);
}
#define ALIGN_DIVUP(x, y) (((x) + (y) - 1) / (y))

static INLINE unsigned si_map_swizzle(unsigned swizzle)
{
	switch (swizzle) {
	case UTIL_FORMAT_SWIZZLE_Y:
		return V_008F0C_SQ_SEL_Y;
	case UTIL_FORMAT_SWIZZLE_Z:
		return V_008F0C_SQ_SEL_Z;
	case UTIL_FORMAT_SWIZZLE_W:
		return V_008F0C_SQ_SEL_W;
	case UTIL_FORMAT_SWIZZLE_0:
		return V_008F0C_SQ_SEL_0;
	case UTIL_FORMAT_SWIZZLE_1:
		return V_008F0C_SQ_SEL_1;
	default: /* UTIL_FORMAT_SWIZZLE_X */
		return V_008F0C_SQ_SEL_X;
	}
}

static inline unsigned r600_tex_aniso_filter(unsigned filter)
{
	if (filter <= 1)   return 0;
	if (filter <= 2)   return 1;
	if (filter <= 4)   return 2;
	if (filter <= 8)   return 3;
	 /* else */        return 4;
}

/* 12.4 fixed-point */
static INLINE unsigned r600_pack_float_12p4(float x)
{
	return x <= 0    ? 0 :
	       x >= 4096 ? 0xffff : x * 16;
}

static INLINE uint64_t r600_resource_va(struct pipe_screen *screen, struct pipe_resource *resource)
{
	struct r600_screen *rscreen = (struct r600_screen*)screen;
	struct si_resource *rresource = (struct si_resource*)resource;

	return rscreen->ws->buffer_get_virtual_address(rresource->cs_buf);
}

#endif
