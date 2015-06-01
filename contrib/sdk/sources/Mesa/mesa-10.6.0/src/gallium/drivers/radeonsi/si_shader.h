/*
 * Copyright 2012 Advanced Micro Devices, Inc.
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
 *	Tom Stellard <thomas.stellard@amd.com>
 *	Michel Dänzer <michel.daenzer@amd.com>
 *      Christian König <christian.koenig@amd.com>
 */

#ifndef SI_SHADER_H
#define SI_SHADER_H

#include <llvm-c/Core.h> /* LLVMModuleRef */
#include "tgsi/tgsi_scan.h"
#include "si_state.h"

struct radeon_shader_binary;
struct radeon_shader_reloc;

#define SI_SGPR_RW_BUFFERS	0  /* rings (& stream-out, VS only) */
#define SI_SGPR_CONST		2
#define SI_SGPR_SAMPLER		4
#define SI_SGPR_RESOURCE	6
#define SI_SGPR_VERTEX_BUFFER	8  /* VS only */
#define SI_SGPR_BASE_VERTEX	10 /* VS only */
#define SI_SGPR_START_INSTANCE	11 /* VS only */
#define SI_SGPR_ALPHA_REF	8  /* PS only */

#define SI_VS_NUM_USER_SGPR	12
#define SI_GS_NUM_USER_SGPR	8
#define SI_GSCOPY_NUM_USER_SGPR	4
#define SI_PS_NUM_USER_SGPR	9

/* LLVM function parameter indices */
#define SI_PARAM_RW_BUFFERS	0
#define SI_PARAM_CONST		1
#define SI_PARAM_SAMPLER	2
#define SI_PARAM_RESOURCE	3

/* VS only parameters */
#define SI_PARAM_VERTEX_BUFFER	4
#define SI_PARAM_BASE_VERTEX	5
#define SI_PARAM_START_INSTANCE	6
/* the other VS parameters are assigned dynamically */

/* ES only parameters */
#define SI_PARAM_ES2GS_OFFSET	7

/* GS only parameters */
#define SI_PARAM_GS2VS_OFFSET	4
#define SI_PARAM_GS_WAVE_ID	5
#define SI_PARAM_VTX0_OFFSET	6
#define SI_PARAM_VTX1_OFFSET	7
#define SI_PARAM_PRIMITIVE_ID	8
#define SI_PARAM_VTX2_OFFSET	9
#define SI_PARAM_VTX3_OFFSET	10
#define SI_PARAM_VTX4_OFFSET	11
#define SI_PARAM_VTX5_OFFSET	12
#define SI_PARAM_GS_INSTANCE_ID	13

/* PS only parameters */
#define SI_PARAM_ALPHA_REF		4
#define SI_PARAM_PRIM_MASK		5
#define SI_PARAM_PERSP_SAMPLE		6
#define SI_PARAM_PERSP_CENTER		7
#define SI_PARAM_PERSP_CENTROID		8
#define SI_PARAM_PERSP_PULL_MODEL	9
#define SI_PARAM_LINEAR_SAMPLE		10
#define SI_PARAM_LINEAR_CENTER		11
#define SI_PARAM_LINEAR_CENTROID	12
#define SI_PARAM_LINE_STIPPLE_TEX	13
#define SI_PARAM_POS_X_FLOAT		14
#define SI_PARAM_POS_Y_FLOAT		15
#define SI_PARAM_POS_Z_FLOAT		16
#define SI_PARAM_POS_W_FLOAT		17
#define SI_PARAM_FRONT_FACE		18
#define SI_PARAM_ANCILLARY		19
#define SI_PARAM_SAMPLE_COVERAGE	20
#define SI_PARAM_POS_FIXED_PT		21

#define SI_NUM_PARAMS (SI_PARAM_POS_FIXED_PT + 1)

struct si_shader;

struct si_shader_selector {
	struct si_shader *current;

	struct tgsi_token       *tokens;
	struct pipe_stream_output_info  so;
	struct tgsi_shader_info		info;

	unsigned	num_shaders;

	/* PIPE_SHADER_[VERTEX|FRAGMENT|...] */
	unsigned	type;

	unsigned	gs_output_prim;
	unsigned	gs_max_out_vertices;
	uint64_t	gs_used_inputs; /* mask of "get_unique_index" bits */
};

union si_shader_key {
	struct {
		unsigned	export_16bpc:8;
		unsigned	last_cbuf:3;
		unsigned	color_two_side:1;
		unsigned	alpha_func:3;
		unsigned	alpha_to_one:1;
		unsigned	poly_stipple:1;
		unsigned	poly_line_smoothing:1;
	} ps;
	struct {
		unsigned	instance_divisors[SI_NUM_VERTEX_BUFFERS];
		/* The mask of "get_unique_index" bits, needed for ES,
		 * it describes how the ES->GS ring buffer is laid out. */
		uint64_t	gs_used_inputs;
		unsigned	as_es:1;
	} vs;
};

struct si_shader {
	struct si_shader_selector	*selector;
	struct si_shader		*next_variant;

	struct si_shader		*gs_copy_shader;
	struct si_pm4_state		*pm4;
	struct r600_resource		*bo;
	struct r600_resource		*scratch_bo;
	struct radeon_shader_binary	binary;
	unsigned			num_sgprs;
	unsigned			num_vgprs;
	unsigned			lds_size;
	unsigned			spi_ps_input_ena;
	unsigned			float_mode;
	unsigned			scratch_bytes_per_wave;
	unsigned			spi_shader_col_format;
	unsigned			spi_shader_z_format;
	unsigned			db_shader_control;
	unsigned			cb_shader_mask;
	union si_shader_key		key;

	unsigned		nparam;
	unsigned		vs_output_param_offset[PIPE_MAX_SHADER_OUTPUTS];
	unsigned		ps_input_param_offset[PIPE_MAX_SHADER_INPUTS];

	bool			uses_instanceid;
	unsigned		nr_pos_exports;
	bool			is_gs_copy_shader;
	bool			dx10_clamp_mode; /* convert NaNs to 0 */
};

static inline struct tgsi_shader_info *si_get_vs_info(struct si_context *sctx)
{
	return sctx->gs_shader ? &sctx->gs_shader->info
                               : &sctx->vs_shader->info;
}

static inline struct si_shader* si_get_vs_state(struct si_context *sctx)
{
	if (sctx->gs_shader)
		return sctx->gs_shader->current->gs_copy_shader;
	else
		return sctx->vs_shader->current;
}

/* radeonsi_shader.c */
int si_shader_create(struct si_screen *sscreen, LLVMTargetMachineRef tm,
		     struct si_shader *shader);
int si_compile_llvm(struct si_screen *sscreen, struct si_shader *shader,
		    LLVMTargetMachineRef tm, LLVMModuleRef mod);
void si_shader_destroy(struct pipe_context *ctx, struct si_shader *shader);
unsigned si_shader_io_get_unique_index(unsigned semantic_name, unsigned index);
int si_shader_binary_read(struct si_screen *sscreen, struct si_shader *shader,
		const struct radeon_shader_binary *binary);
void si_shader_apply_scratch_relocs(struct si_context *sctx,
			struct si_shader *shader,
			uint64_t scratch_va);
void si_shader_binary_read_config(const struct si_screen *sscreen,
				struct si_shader *shader,
				unsigned symbol_offset);

#endif
