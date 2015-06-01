/*
 * Copyright 2011 Advanced Micro Devices, Inc.
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
 * Authors: Tom Stellard <thomas.stellard@amd.com>
 *
 */

#ifndef RADEON_LLVM_H
#define RADEON_LLVM_H

#include <llvm-c/Core.h>
#include "gallivm/lp_bld_init.h"
#include "gallivm/lp_bld_tgsi.h"

#define RADEON_LLVM_MAX_INPUTS 32 * 4
#define RADEON_LLVM_MAX_OUTPUTS 32 * 4
#define RADEON_LLVM_MAX_ARRAYS 16

#define RADEON_LLVM_INITIAL_CF_DEPTH 4

#define RADEON_LLVM_MAX_SYSTEM_VALUES 4

struct radeon_llvm_branch {
	LLVMBasicBlockRef endif_block;
	LLVMBasicBlockRef if_block;
	LLVMBasicBlockRef else_block;
	unsigned has_else;
};

struct radeon_llvm_loop {
	LLVMBasicBlockRef loop_block;
	LLVMBasicBlockRef endloop_block;
};

struct radeon_llvm_context {

	struct lp_build_tgsi_soa_context soa;

	unsigned chip_class;
	unsigned type;
	unsigned face_gpr;
	unsigned two_side;
	unsigned clip_vertex;
	unsigned inputs_count;
	struct r600_shader_io * r600_inputs;
	struct r600_shader_io * r600_outputs;
	struct pipe_stream_output_info *stream_outputs;
	unsigned color_buffer_count;
	unsigned fs_color_all;
	unsigned alpha_to_one;
	unsigned has_txq_cube_array_z_comp;
	unsigned uses_tex_buffers;
	unsigned has_compressed_msaa_texturing;

	/*=== Front end configuration ===*/

	/* Special Intrinsics */

	/** Write to an output register: float store_output(float, i32) */
	const char * store_output_intr;

	/** Swizzle a vector value: <4 x float> swizzle(<4 x float>, i32)
	 * The swizzle is an unsigned integer that encodes a TGSI_SWIZZLE_* value
	 * in 2-bits.
	 * Swizzle{0-1} = X Channel
	 * Swizzle{2-3} = Y Channel
	 * Swizzle{4-5} = Z Channel
	 * Swizzle{6-7} = W Channel
	 */
	const char * swizzle_intr;

	/* Instructions that are not described by any of the TGSI opcodes. */

	/** This function is responsible for initilizing the inputs array and will be
	  * called once for each input declared in the TGSI shader.
	  */
	void (*load_input)(struct radeon_llvm_context *,
			unsigned input_index,
			const struct tgsi_full_declaration *decl);

	void (*load_system_value)(struct radeon_llvm_context *,
			unsigned index,
			const struct tgsi_full_declaration *decl);

	/** User data to use with the callbacks */
	void * userdata;

	/** This array contains the input values for the shader.  Typically these
	  * values will be in the form of a target intrinsic that will inform the
	  * backend how to load the actual inputs to the shader. 
	  */
	LLVMValueRef inputs[RADEON_LLVM_MAX_INPUTS];
	LLVMValueRef outputs[RADEON_LLVM_MAX_OUTPUTS][TGSI_NUM_CHANNELS];
	unsigned output_reg_count;

	/** This pointer is used to contain the temporary values.
	  * The amount of temporary used in tgsi can't be bound to a max value and
	  * thus we must allocate this array at runtime.
	  */
	LLVMValueRef *temps;
	unsigned temps_count;
	LLVMValueRef system_values[RADEON_LLVM_MAX_SYSTEM_VALUES];

	/*=== Private Members ===*/

	struct radeon_llvm_branch *branch;
	struct radeon_llvm_loop *loop;

	unsigned branch_depth;
	unsigned branch_depth_max;
	unsigned loop_depth;
	unsigned loop_depth_max;

	struct tgsi_declaration_range arrays[RADEON_LLVM_MAX_ARRAYS];
	unsigned num_arrays;

	LLVMValueRef main_fn;

	struct gallivm_state gallivm;
};

static inline LLVMTypeRef tgsi2llvmtype(
		struct lp_build_tgsi_context * bld_base,
		enum tgsi_opcode_type type)
{
	LLVMContextRef ctx = bld_base->base.gallivm->context;

	switch (type) {
	case TGSI_TYPE_UNSIGNED:
	case TGSI_TYPE_SIGNED:
		return LLVMInt32TypeInContext(ctx);
	case TGSI_TYPE_UNTYPED:
	case TGSI_TYPE_FLOAT:
		return LLVMFloatTypeInContext(ctx);
	default: break;
	}
	return 0;
}

static inline LLVMValueRef bitcast(
		struct lp_build_tgsi_context * bld_base,
		enum tgsi_opcode_type type,
		LLVMValueRef value
)
{
	LLVMBuilderRef builder = bld_base->base.gallivm->builder;
	LLVMTypeRef dst_type = tgsi2llvmtype(bld_base, type);

	if (dst_type)
		return LLVMBuildBitCast(builder, value, dst_type, "");
	else
		return value;
}


void radeon_llvm_emit_prepare_cube_coords(struct lp_build_tgsi_context * bld_base,
                                          struct lp_build_emit_data * emit_data,
                                          LLVMValueRef *coords_arg);

void radeon_llvm_context_init(struct radeon_llvm_context * ctx);

void radeon_llvm_create_func(struct radeon_llvm_context * ctx,
                             LLVMTypeRef *ParamTypes, unsigned ParamCount);

void radeon_llvm_dispose(struct radeon_llvm_context * ctx);

inline static struct radeon_llvm_context * radeon_llvm_context(
	struct lp_build_tgsi_context * bld_base)
{
	return (struct radeon_llvm_context*)bld_base;
}

unsigned radeon_llvm_reg_index_soa(unsigned index, unsigned chan);

void radeon_llvm_finalize_module(struct radeon_llvm_context * ctx);

LLVMValueRef
build_intrinsic(LLVMBuilderRef builder,
		const char *name,
		LLVMTypeRef ret_type,
		LLVMValueRef *args,
		unsigned num_args,
		LLVMAttribute attr);

void
build_tgsi_intrinsic_nomem(
		const struct lp_build_tgsi_action * action,
		struct lp_build_tgsi_context * bld_base,
		struct lp_build_emit_data * emit_data);



#endif /* RADEON_LLVM_H */
