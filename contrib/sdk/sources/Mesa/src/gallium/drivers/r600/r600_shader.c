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
 */
#include "r600_sq.h"
#include "r600_llvm.h"
#include "r600_formats.h"
#include "r600_opcodes.h"
#include "r600_shader.h"
#include "r600d.h"

#include "sb/sb_public.h"

#include "pipe/p_shader_tokens.h"
#include "tgsi/tgsi_info.h"
#include "tgsi/tgsi_parse.h"
#include "tgsi/tgsi_scan.h"
#include "tgsi/tgsi_dump.h"
#include "util/u_memory.h"
#include "util/u_math.h"
#include <stdio.h>
#include <errno.h>

/* CAYMAN notes 
Why CAYMAN got loops for lots of instructions is explained here.

-These 8xx t-slot only ops are implemented in all vector slots.
MUL_LIT, FLT_TO_UINT, INT_TO_FLT, UINT_TO_FLT
These 8xx t-slot only opcodes become vector ops, with all four 
slots expecting the arguments on sources a and b. Result is 
broadcast to all channels.
MULLO_INT, MULHI_INT, MULLO_UINT, MULHI_UINT
These 8xx t-slot only opcodes become vector ops in the z, y, and 
x slots.
EXP_IEEE, LOG_IEEE/CLAMPED, RECIP_IEEE/CLAMPED/FF/INT/UINT/_64/CLAMPED_64
RECIPSQRT_IEEE/CLAMPED/FF/_64/CLAMPED_64
SQRT_IEEE/_64
SIN/COS
The w slot may have an independent co-issued operation, or if the 
result is required to be in the w slot, the opcode above may be 
issued in the w slot as well.
The compiler must issue the source argument to slots z, y, and x
*/

static int r600_shader_from_tgsi(struct r600_screen *rscreen,
				 struct r600_pipe_shader *pipeshader,
				 struct r600_shader_key key);

static void r600_add_gpr_array(struct r600_shader *ps, int start_gpr,
                           int size, unsigned comp_mask) {

	if (!size)
		return;

	if (ps->num_arrays == ps->max_arrays) {
		ps->max_arrays += 64;
		ps->arrays = realloc(ps->arrays, ps->max_arrays *
		                     sizeof(struct r600_shader_array));
	}

	int n = ps->num_arrays;
	++ps->num_arrays;

	ps->arrays[n].comp_mask = comp_mask;
	ps->arrays[n].gpr_start = start_gpr;
	ps->arrays[n].gpr_count = size;
}

static unsigned tgsi_get_processor_type(const struct tgsi_token *tokens)
{
	struct tgsi_parse_context parse;

	if (tgsi_parse_init( &parse, tokens ) != TGSI_PARSE_OK) {
		debug_printf("tgsi_parse_init() failed in %s:%i!\n", __func__, __LINE__);
		return ~0;
	}
	return parse.FullHeader.Processor.Processor;
}

static bool r600_can_dump_shader(struct r600_screen *rscreen, unsigned processor_type)
{
	switch (processor_type) {
	case TGSI_PROCESSOR_VERTEX:
		return (rscreen->debug_flags & DBG_VS) != 0;
	case TGSI_PROCESSOR_GEOMETRY:
		return (rscreen->debug_flags & DBG_GS) != 0;
	case TGSI_PROCESSOR_FRAGMENT:
		return (rscreen->debug_flags & DBG_PS) != 0;
	case TGSI_PROCESSOR_COMPUTE:
		return (rscreen->debug_flags & DBG_CS) != 0;
	default:
		return false;
	}
}

static void r600_dump_streamout(struct pipe_stream_output_info *so)
{
	unsigned i;

	fprintf(stderr, "STREAMOUT\n");
	for (i = 0; i < so->num_outputs; i++) {
		unsigned mask = ((1 << so->output[i].num_components) - 1) <<
				so->output[i].start_component;
		fprintf(stderr, "  %i: MEM_STREAM0_BUF%i[%i..%i] <- OUT[%i].%s%s%s%s%s\n",
			i, so->output[i].output_buffer,
			so->output[i].dst_offset, so->output[i].dst_offset + so->output[i].num_components - 1,
			so->output[i].register_index,
			mask & 1 ? "x" : "",
		        mask & 2 ? "y" : "",
		        mask & 4 ? "z" : "",
		        mask & 8 ? "w" : "",
			so->output[i].dst_offset < so->output[i].start_component ? " (will lower)" : "");
	}
}

int r600_pipe_shader_create(struct pipe_context *ctx,
			    struct r600_pipe_shader *shader,
			    struct r600_shader_key key)
{
	struct r600_context *rctx = (struct r600_context *)ctx;
	struct r600_pipe_shader_selector *sel = shader->selector;
	int r, i;
	uint32_t *ptr;
	bool dump = r600_can_dump_shader(rctx->screen, tgsi_get_processor_type(sel->tokens));
	unsigned use_sb = rctx->screen->debug_flags & DBG_SB;
	unsigned sb_disasm = use_sb || (rctx->screen->debug_flags & DBG_SB_DISASM);

	shader->shader.bc.isa = rctx->isa;

	if (dump) {
		fprintf(stderr, "--------------------------------------------------------------\n");
		tgsi_dump(sel->tokens, 0);

		if (sel->so.num_outputs) {
			r600_dump_streamout(&sel->so);
		}
	}
	r = r600_shader_from_tgsi(rctx->screen, shader, key);
	if (r) {
		R600_ERR("translation from TGSI failed !\n");
		return r;
	}

	/* Check if the bytecode has already been built.  When using the llvm
	 * backend, r600_shader_from_tgsi() will take care of building the
	 * bytecode.
	 */
	if (!shader->shader.bc.bytecode) {
		r = r600_bytecode_build(&shader->shader.bc);
		if (r) {
			R600_ERR("building bytecode failed !\n");
			return r;
		}
	}

	if (dump && !sb_disasm) {
		fprintf(stderr, "--------------------------------------------------------------\n");
		r600_bytecode_disasm(&shader->shader.bc);
		fprintf(stderr, "______________________________________________________________\n");
	} else if ((dump && sb_disasm) || use_sb) {
		r = r600_sb_bytecode_process(rctx, &shader->shader.bc, &shader->shader,
		                             dump, use_sb);
		if (r) {
			R600_ERR("r600_sb_bytecode_process failed !\n");
			return r;
		}
	}

	/* Store the shader in a buffer. */
	if (shader->bo == NULL) {
		shader->bo = (struct r600_resource*)
			pipe_buffer_create(ctx->screen, PIPE_BIND_CUSTOM, PIPE_USAGE_IMMUTABLE, shader->shader.bc.ndw * 4);
		if (shader->bo == NULL) {
			return -ENOMEM;
		}
		ptr = r600_buffer_mmap_sync_with_rings(rctx, shader->bo, PIPE_TRANSFER_WRITE);
		if (R600_BIG_ENDIAN) {
			for (i = 0; i < shader->shader.bc.ndw; ++i) {
				ptr[i] = util_bswap32(shader->shader.bc.bytecode[i]);
			}
		} else {
			memcpy(ptr, shader->shader.bc.bytecode, shader->shader.bc.ndw * sizeof(*ptr));
		}
		rctx->ws->buffer_unmap(shader->bo->cs_buf);
	}

	/* Build state. */
	switch (shader->shader.processor_type) {
	case TGSI_PROCESSOR_VERTEX:
		if (rctx->chip_class >= EVERGREEN) {
			evergreen_update_vs_state(ctx, shader);
		} else {
			r600_update_vs_state(ctx, shader);
		}
		break;
	case TGSI_PROCESSOR_FRAGMENT:
		if (rctx->chip_class >= EVERGREEN) {
			evergreen_update_ps_state(ctx, shader);
		} else {
			r600_update_ps_state(ctx, shader);
		}
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

void r600_pipe_shader_destroy(struct pipe_context *ctx, struct r600_pipe_shader *shader)
{
	pipe_resource_reference((struct pipe_resource**)&shader->bo, NULL);
	r600_bytecode_clear(&shader->shader.bc);
	r600_release_command_buffer(&shader->command_buffer);
}

/*
 * tgsi -> r600 shader
 */
struct r600_shader_tgsi_instruction;

struct r600_shader_src {
	unsigned				sel;
	unsigned				swizzle[4];
	unsigned				neg;
	unsigned				abs;
	unsigned				rel;
	unsigned				kc_bank;
	uint32_t				value[4];
};

struct r600_shader_ctx {
	struct tgsi_shader_info			info;
	struct tgsi_parse_context		parse;
	const struct tgsi_token			*tokens;
	unsigned				type;
	unsigned				file_offset[TGSI_FILE_COUNT];
	unsigned				temp_reg;
	struct r600_shader_tgsi_instruction	*inst_info;
	struct r600_bytecode			*bc;
	struct r600_shader			*shader;
	struct r600_shader_src			src[4];
	uint32_t				*literals;
	uint32_t				nliterals;
	uint32_t				max_driver_temp_used;
	boolean use_llvm;
	/* needed for evergreen interpolation */
	boolean                                 input_centroid;
	boolean                                 input_linear;
	boolean                                 input_perspective;
	int					num_interp_gpr;
	int					face_gpr;
	int					colors_used;
	boolean                 clip_vertex_write;
	unsigned                cv_output;
	int					fragcoord_input;
	int					native_integers;
};

struct r600_shader_tgsi_instruction {
	unsigned	tgsi_opcode;
	unsigned	is_op3;
	unsigned	op;
	int (*process)(struct r600_shader_ctx *ctx);
};

static struct r600_shader_tgsi_instruction r600_shader_tgsi_instruction[], eg_shader_tgsi_instruction[], cm_shader_tgsi_instruction[];
static int tgsi_helper_tempx_replicate(struct r600_shader_ctx *ctx);
static inline void callstack_push(struct r600_shader_ctx *ctx, unsigned reason);
static void fc_pushlevel(struct r600_shader_ctx *ctx, int type);
static int tgsi_else(struct r600_shader_ctx *ctx);
static int tgsi_endif(struct r600_shader_ctx *ctx);
static int tgsi_bgnloop(struct r600_shader_ctx *ctx);
static int tgsi_endloop(struct r600_shader_ctx *ctx);
static int tgsi_loop_brk_cont(struct r600_shader_ctx *ctx);

static int tgsi_is_supported(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *i = &ctx->parse.FullToken.FullInstruction;
	int j;

	if (i->Instruction.NumDstRegs > 1) {
		R600_ERR("too many dst (%d)\n", i->Instruction.NumDstRegs);
		return -EINVAL;
	}
	if (i->Instruction.Predicate) {
		R600_ERR("predicate unsupported\n");
		return -EINVAL;
	}
#if 0
	if (i->Instruction.Label) {
		R600_ERR("label unsupported\n");
		return -EINVAL;
	}
#endif
	for (j = 0; j < i->Instruction.NumSrcRegs; j++) {
		if (i->Src[j].Register.Dimension) {
		   if (i->Src[j].Register.File != TGSI_FILE_CONSTANT) {
			   R600_ERR("unsupported src %d (dimension %d)\n", j,
				    i->Src[j].Register.Dimension);
			   return -EINVAL;
		   }
		}
	}
	for (j = 0; j < i->Instruction.NumDstRegs; j++) {
		if (i->Dst[j].Register.Dimension) {
			R600_ERR("unsupported dst (dimension)\n");
			return -EINVAL;
		}
	}
	return 0;
}

static void evergreen_interp_assign_ij_index(struct r600_shader_ctx *ctx,
		int input)
{
	int ij_index = 0;

	if (ctx->shader->input[input].interpolate == TGSI_INTERPOLATE_PERSPECTIVE) {
		if (ctx->shader->input[input].centroid)
			ij_index++;
	} else if (ctx->shader->input[input].interpolate == TGSI_INTERPOLATE_LINEAR) {
		/* if we have perspective add one */
		if (ctx->input_perspective)  {
			ij_index++;
			/* if we have perspective centroid */
			if (ctx->input_centroid)
				ij_index++;
		}
		if (ctx->shader->input[input].centroid)
			ij_index++;
	}

	ctx->shader->input[input].ij_index = ij_index;
}

static int evergreen_interp_alu(struct r600_shader_ctx *ctx, int input)
{
	int i, r;
	struct r600_bytecode_alu alu;
	int gpr = 0, base_chan = 0;
	int ij_index = ctx->shader->input[input].ij_index;

	/* work out gpr and base_chan from index */
	gpr = ij_index / 2;
	base_chan = (2 * (ij_index % 2)) + 1;

	for (i = 0; i < 8; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		if (i < 4)
			alu.op = ALU_OP2_INTERP_ZW;
		else
			alu.op = ALU_OP2_INTERP_XY;

		if ((i > 1) && (i < 6)) {
			alu.dst.sel = ctx->shader->input[input].gpr;
			alu.dst.write = 1;
		}

		alu.dst.chan = i % 4;

		alu.src[0].sel = gpr;
		alu.src[0].chan = (base_chan - (i % 2));

		alu.src[1].sel = V_SQ_ALU_SRC_PARAM_BASE + ctx->shader->input[input].lds_pos;

		alu.bank_swizzle_force = SQ_ALU_VEC_210;
		if ((i % 4) == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int evergreen_interp_flat(struct r600_shader_ctx *ctx, int input)
{
	int i, r;
	struct r600_bytecode_alu alu;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_INTERP_LOAD_P0;

		alu.dst.sel = ctx->shader->input[input].gpr;
		alu.dst.write = 1;

		alu.dst.chan = i;

		alu.src[0].sel = V_SQ_ALU_SRC_PARAM_BASE + ctx->shader->input[input].lds_pos;
		alu.src[0].chan = i;

		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

/*
 * Special export handling in shaders
 *
 * shader export ARRAY_BASE for EXPORT_POS:
 * 60 is position
 * 61 is misc vector
 * 62, 63 are clip distance vectors
 *
 * The use of the values exported in 61-63 are controlled by PA_CL_VS_OUT_CNTL:
 * VS_OUT_MISC_VEC_ENA - enables the use of all fields in export 61
 * USE_VTX_POINT_SIZE - point size in the X channel of export 61
 * USE_VTX_EDGE_FLAG - edge flag in the Y channel of export 61
 * USE_VTX_RENDER_TARGET_INDX - render target index in the Z channel of export 61
 * USE_VTX_VIEWPORT_INDX - viewport index in the W channel of export 61
 * USE_VTX_KILL_FLAG - kill flag in the Z channel of export 61 (mutually
 * exclusive from render target index)
 * VS_OUT_CCDIST0_VEC_ENA/VS_OUT_CCDIST1_VEC_ENA - enable clip distance vectors
 *
 *
 * shader export ARRAY_BASE for EXPORT_PIXEL:
 * 0-7 CB targets
 * 61 computed Z vector
 *
 * The use of the values exported in the computed Z vector are controlled
 * by DB_SHADER_CONTROL:
 * Z_EXPORT_ENABLE - Z as a float in RED
 * STENCIL_REF_EXPORT_ENABLE - stencil ref as int in GREEN
 * COVERAGE_TO_MASK_ENABLE - alpha to mask in ALPHA
 * MASK_EXPORT_ENABLE - pixel sample mask in BLUE
 * DB_SOURCE_FORMAT - export control restrictions
 *
 */


/* Map name/sid pair from tgsi to the 8-bit semantic index for SPI setup */
static int r600_spi_sid(struct r600_shader_io * io)
{
	int index, name = io->name;

	/* These params are handled differently, they don't need
	 * semantic indices, so we'll use 0 for them.
	 */
	if (name == TGSI_SEMANTIC_POSITION ||
		name == TGSI_SEMANTIC_PSIZE ||
		name == TGSI_SEMANTIC_FACE)
		index = 0;
	else {
		if (name == TGSI_SEMANTIC_GENERIC) {
			/* For generic params simply use sid from tgsi */
			index = io->sid;
		} else {
			/* For non-generic params - pack name and sid into 8 bits */
			index = 0x80 | (name<<3) | (io->sid);
		}

		/* Make sure that all really used indices have nonzero value, so
		 * we can just compare it to 0 later instead of comparing the name
		 * with different values to detect special cases. */
		index++;
	}

	return index;
};

/* turn input into interpolate on EG */
static int evergreen_interp_input(struct r600_shader_ctx *ctx, int index)
{
	int r = 0;

	if (ctx->shader->input[index].spi_sid) {
		ctx->shader->input[index].lds_pos = ctx->shader->nlds++;
		if (ctx->shader->input[index].interpolate > 0) {
			evergreen_interp_assign_ij_index(ctx, index);
			if (!ctx->use_llvm)
				r = evergreen_interp_alu(ctx, index);
		} else {
			if (!ctx->use_llvm)
				r = evergreen_interp_flat(ctx, index);
		}
	}
	return r;
}

static int select_twoside_color(struct r600_shader_ctx *ctx, int front, int back)
{
	struct r600_bytecode_alu alu;
	int i, r;
	int gpr_front = ctx->shader->input[front].gpr;
	int gpr_back = ctx->shader->input[back].gpr;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(alu));
		alu.op = ALU_OP3_CNDGT;
		alu.is_op3 = 1;
		alu.dst.write = 1;
		alu.dst.sel = gpr_front;
		alu.src[0].sel = ctx->face_gpr;
		alu.src[1].sel = gpr_front;
		alu.src[2].sel = gpr_back;

		alu.dst.chan = i;
		alu.src[1].chan = i;
		alu.src[2].chan = i;
		alu.last = (i==3);

		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;
	}

	return 0;
}

static int tgsi_declaration(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_declaration *d = &ctx->parse.FullToken.FullDeclaration;
	int r, i, j, count = d->Range.Last - d->Range.First + 1;

	switch (d->Declaration.File) {
	case TGSI_FILE_INPUT:
		i = ctx->shader->ninput;
                assert(i < Elements(ctx->shader->input));
		ctx->shader->ninput += count;
		ctx->shader->input[i].name = d->Semantic.Name;
		ctx->shader->input[i].sid = d->Semantic.Index;
		ctx->shader->input[i].interpolate = d->Interp.Interpolate;
		ctx->shader->input[i].centroid = d->Interp.Centroid;
		ctx->shader->input[i].gpr = ctx->file_offset[TGSI_FILE_INPUT] + d->Range.First;
		if (ctx->type == TGSI_PROCESSOR_FRAGMENT) {
			ctx->shader->input[i].spi_sid = r600_spi_sid(&ctx->shader->input[i]);
			switch (ctx->shader->input[i].name) {
			case TGSI_SEMANTIC_FACE:
				ctx->face_gpr = ctx->shader->input[i].gpr;
				break;
			case TGSI_SEMANTIC_COLOR:
				ctx->colors_used++;
				break;
			case TGSI_SEMANTIC_POSITION:
				ctx->fragcoord_input = i;
				break;
			}
			if (ctx->bc->chip_class >= EVERGREEN) {
				if ((r = evergreen_interp_input(ctx, i)))
					return r;
			}
		}
		for (j = 1; j < count; ++j) {
			ctx->shader->input[i + j] = ctx->shader->input[i];
			ctx->shader->input[i + j].gpr += j;
		}
		break;
	case TGSI_FILE_OUTPUT:
		i = ctx->shader->noutput++;
                assert(i < Elements(ctx->shader->output));
		ctx->shader->output[i].name = d->Semantic.Name;
		ctx->shader->output[i].sid = d->Semantic.Index;
		ctx->shader->output[i].gpr = ctx->file_offset[TGSI_FILE_OUTPUT] + d->Range.First;
		ctx->shader->output[i].interpolate = d->Interp.Interpolate;
		ctx->shader->output[i].write_mask = d->Declaration.UsageMask;
		if (ctx->type == TGSI_PROCESSOR_VERTEX) {
			ctx->shader->output[i].spi_sid = r600_spi_sid(&ctx->shader->output[i]);
			switch (d->Semantic.Name) {
			case TGSI_SEMANTIC_CLIPDIST:
				ctx->shader->clip_dist_write |= d->Declaration.UsageMask << (d->Semantic.Index << 2);
				break;
			case TGSI_SEMANTIC_PSIZE:
				ctx->shader->vs_out_misc_write = 1;
				ctx->shader->vs_out_point_size = 1;
				break;
			case TGSI_SEMANTIC_CLIPVERTEX:
				ctx->clip_vertex_write = TRUE;
				ctx->cv_output = i;
				break;
			}
		} else if (ctx->type == TGSI_PROCESSOR_FRAGMENT) {
			switch (d->Semantic.Name) {
			case TGSI_SEMANTIC_COLOR:
				ctx->shader->nr_ps_max_color_exports++;
				break;
			}
		}
		break;
	case TGSI_FILE_TEMPORARY:
		if (ctx->info.indirect_files & (1 << TGSI_FILE_TEMPORARY)) {
			if (d->Array.ArrayID) {
				r600_add_gpr_array(ctx->shader,
				               ctx->file_offset[TGSI_FILE_TEMPORARY] +
								   d->Range.First,
				               d->Range.Last - d->Range.First + 1, 0x0F);
			}
		}
		break;

	case TGSI_FILE_CONSTANT:
	case TGSI_FILE_SAMPLER:
	case TGSI_FILE_ADDRESS:
		break;

	case TGSI_FILE_SYSTEM_VALUE:
		if (d->Semantic.Name == TGSI_SEMANTIC_INSTANCEID) {
			if (!ctx->native_integers) {
				struct r600_bytecode_alu alu;
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));

				alu.op = ALU_OP1_INT_TO_FLT;
				alu.src[0].sel = 0;
				alu.src[0].chan = 3;

				alu.dst.sel = 0;
				alu.dst.chan = 3;
				alu.dst.write = 1;
				alu.last = 1;

				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
			break;
		} else if (d->Semantic.Name == TGSI_SEMANTIC_VERTEXID)
			break;
	default:
		R600_ERR("unsupported file %d declaration\n", d->Declaration.File);
		return -EINVAL;
	}
	return 0;
}

static int r600_get_temp(struct r600_shader_ctx *ctx)
{
	return ctx->temp_reg + ctx->max_driver_temp_used++;
}

/*
 * for evergreen we need to scan the shader to find the number of GPRs we need to
 * reserve for interpolation.
 *
 * we need to know if we are going to emit
 * any centroid inputs
 * if perspective and linear are required
*/
static int evergreen_gpr_count(struct r600_shader_ctx *ctx)
{
	int i;
	int num_baryc;

	ctx->input_linear = FALSE;
	ctx->input_perspective = FALSE;
	ctx->input_centroid = FALSE;
	ctx->num_interp_gpr = 1;

	/* any centroid inputs */
	for (i = 0; i < ctx->info.num_inputs; i++) {
		/* skip position/face */
		if (ctx->info.input_semantic_name[i] == TGSI_SEMANTIC_POSITION ||
		    ctx->info.input_semantic_name[i] == TGSI_SEMANTIC_FACE)
			continue;
		if (ctx->info.input_interpolate[i] == TGSI_INTERPOLATE_LINEAR)
			ctx->input_linear = TRUE;
		if (ctx->info.input_interpolate[i] == TGSI_INTERPOLATE_PERSPECTIVE)
			ctx->input_perspective = TRUE;
		if (ctx->info.input_centroid[i])
			ctx->input_centroid = TRUE;
	}

	num_baryc = 0;
	/* ignoring sample for now */
	if (ctx->input_perspective)
		num_baryc++;
	if (ctx->input_linear)
		num_baryc++;
	if (ctx->input_centroid)
		num_baryc *= 2;

	ctx->num_interp_gpr += (num_baryc + 1) >> 1;

	/* XXX PULL MODEL and LINE STIPPLE, FIXED PT POS */
	return ctx->num_interp_gpr;
}

static void tgsi_src(struct r600_shader_ctx *ctx,
		     const struct tgsi_full_src_register *tgsi_src,
		     struct r600_shader_src *r600_src)
{
	memset(r600_src, 0, sizeof(*r600_src));
	r600_src->swizzle[0] = tgsi_src->Register.SwizzleX;
	r600_src->swizzle[1] = tgsi_src->Register.SwizzleY;
	r600_src->swizzle[2] = tgsi_src->Register.SwizzleZ;
	r600_src->swizzle[3] = tgsi_src->Register.SwizzleW;
	r600_src->neg = tgsi_src->Register.Negate;
	r600_src->abs = tgsi_src->Register.Absolute;

	if (tgsi_src->Register.File == TGSI_FILE_IMMEDIATE) {
		int index;
		if ((tgsi_src->Register.SwizzleX == tgsi_src->Register.SwizzleY) &&
			(tgsi_src->Register.SwizzleX == tgsi_src->Register.SwizzleZ) &&
			(tgsi_src->Register.SwizzleX == tgsi_src->Register.SwizzleW)) {

			index = tgsi_src->Register.Index * 4 + tgsi_src->Register.SwizzleX;
			r600_bytecode_special_constants(ctx->literals[index], &r600_src->sel, &r600_src->neg);
			if (r600_src->sel != V_SQ_ALU_SRC_LITERAL)
				return;
		}
		index = tgsi_src->Register.Index;
		r600_src->sel = V_SQ_ALU_SRC_LITERAL;
		memcpy(r600_src->value, ctx->literals + index * 4, sizeof(r600_src->value));
	} else if (tgsi_src->Register.File == TGSI_FILE_SYSTEM_VALUE) {
		if (ctx->info.system_value_semantic_name[tgsi_src->Register.Index] == TGSI_SEMANTIC_INSTANCEID) {
			r600_src->swizzle[0] = 3;
			r600_src->swizzle[1] = 3;
			r600_src->swizzle[2] = 3;
			r600_src->swizzle[3] = 3;
			r600_src->sel = 0;
		} else if (ctx->info.system_value_semantic_name[tgsi_src->Register.Index] == TGSI_SEMANTIC_VERTEXID) {
			r600_src->swizzle[0] = 0;
			r600_src->swizzle[1] = 0;
			r600_src->swizzle[2] = 0;
			r600_src->swizzle[3] = 0;
			r600_src->sel = 0;
		}
	} else {
		if (tgsi_src->Register.Indirect)
			r600_src->rel = V_SQ_REL_RELATIVE;
		r600_src->sel = tgsi_src->Register.Index;
		r600_src->sel += ctx->file_offset[tgsi_src->Register.File];
	}
	if (tgsi_src->Register.File == TGSI_FILE_CONSTANT) {
		if (tgsi_src->Register.Dimension) {
			r600_src->kc_bank = tgsi_src->Dimension.Index;
		}
	}
}

static int tgsi_fetch_rel_const(struct r600_shader_ctx *ctx, unsigned int cb_idx, unsigned int offset, unsigned int dst_reg)
{
	struct r600_bytecode_vtx vtx;
	unsigned int ar_reg;
	int r;

	if (offset) {
		struct r600_bytecode_alu alu;

		memset(&alu, 0, sizeof(alu));

		alu.op = ALU_OP2_ADD_INT;
		alu.src[0].sel = ctx->bc->ar_reg;

		alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
		alu.src[1].value = offset;

		alu.dst.sel = dst_reg;
		alu.dst.write = 1;
		alu.last = 1;

		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		ar_reg = dst_reg;
	} else {
		ar_reg = ctx->bc->ar_reg;
	}

	memset(&vtx, 0, sizeof(vtx));
	vtx.buffer_id = cb_idx;
	vtx.fetch_type = 2;		/* VTX_FETCH_NO_INDEX_OFFSET */
	vtx.src_gpr = ar_reg;
	vtx.mega_fetch_count = 16;
	vtx.dst_gpr = dst_reg;
	vtx.dst_sel_x = 0;		/* SEL_X */
	vtx.dst_sel_y = 1;		/* SEL_Y */
	vtx.dst_sel_z = 2;		/* SEL_Z */
	vtx.dst_sel_w = 3;		/* SEL_W */
	vtx.data_format = FMT_32_32_32_32_FLOAT;
	vtx.num_format_all = 2;		/* NUM_FORMAT_SCALED */
	vtx.format_comp_all = 1;	/* FORMAT_COMP_SIGNED */
	vtx.srf_mode_all = 1;		/* SRF_MODE_NO_ZERO */
	vtx.endian = r600_endian_swap(32);

	if ((r = r600_bytecode_add_vtx(ctx->bc, &vtx)))
		return r;

	return 0;
}

static int tgsi_split_constant(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, j, k, nconst, r;

	for (i = 0, nconst = 0; i < inst->Instruction.NumSrcRegs; i++) {
		if (inst->Src[i].Register.File == TGSI_FILE_CONSTANT) {
			nconst++;
		}
		tgsi_src(ctx, &inst->Src[i], &ctx->src[i]);
	}
	for (i = 0, j = nconst - 1; i < inst->Instruction.NumSrcRegs; i++) {
		if (inst->Src[i].Register.File != TGSI_FILE_CONSTANT) {
			continue;
		}

		if (ctx->src[i].rel) {
			int treg = r600_get_temp(ctx);
			if ((r = tgsi_fetch_rel_const(ctx, ctx->src[i].kc_bank, ctx->src[i].sel - 512, treg)))
				return r;

			ctx->src[i].kc_bank = 0;
			ctx->src[i].sel = treg;
			ctx->src[i].rel = 0;
			j--;
		} else if (j > 0) {
			int treg = r600_get_temp(ctx);
			for (k = 0; k < 4; k++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_MOV;
				alu.src[0].sel = ctx->src[i].sel;
				alu.src[0].chan = k;
				alu.src[0].rel = ctx->src[i].rel;
				alu.dst.sel = treg;
				alu.dst.chan = k;
				alu.dst.write = 1;
				if (k == 3)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
			ctx->src[i].sel = treg;
			ctx->src[i].rel =0;
			j--;
		}
	}
	return 0;
}

/* need to move any immediate into a temp - for trig functions which use literal for PI stuff */
static int tgsi_split_literal_constant(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, j, k, nliteral, r;

	for (i = 0, nliteral = 0; i < inst->Instruction.NumSrcRegs; i++) {
		if (ctx->src[i].sel == V_SQ_ALU_SRC_LITERAL) {
			nliteral++;
		}
	}
	for (i = 0, j = nliteral - 1; i < inst->Instruction.NumSrcRegs; i++) {
		if (j > 0 && ctx->src[i].sel == V_SQ_ALU_SRC_LITERAL) {
			int treg = r600_get_temp(ctx);
			for (k = 0; k < 4; k++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_MOV;
				alu.src[0].sel = ctx->src[i].sel;
				alu.src[0].chan = k;
				alu.src[0].value = ctx->src[i].value[k];
				alu.dst.sel = treg;
				alu.dst.chan = k;
				alu.dst.write = 1;
				if (k == 3)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
			ctx->src[i].sel = treg;
			j--;
		}
	}
	return 0;
}

static int process_twoside_color_inputs(struct r600_shader_ctx *ctx)
{
	int i, r, count = ctx->shader->ninput;

	for (i = 0; i < count; i++) {
		if (ctx->shader->input[i].name == TGSI_SEMANTIC_COLOR) {
			r = select_twoside_color(ctx, i, ctx->shader->input[i].back_color_input);
			if (r)
				return r;
		}
	}
	return 0;
}


static int r600_shader_from_tgsi(struct r600_screen *rscreen,
				 struct r600_pipe_shader *pipeshader,
				 struct r600_shader_key key)
{
	struct r600_shader *shader = &pipeshader->shader;
	struct tgsi_token *tokens = pipeshader->selector->tokens;
	struct pipe_stream_output_info so = pipeshader->selector->so;
	struct tgsi_full_immediate *immediate;
	struct tgsi_full_property *property;
	struct r600_shader_ctx ctx;
	struct r600_bytecode_output output[32];
	unsigned output_done, noutput;
	unsigned opcode;
	int i, j, k, r = 0;
	int next_pixel_base = 0, next_pos_base = 60, next_param_base = 0;
	/* Declarations used by llvm code */
	bool use_llvm = false;
	bool indirect_gprs;

#ifdef R600_USE_LLVM
	use_llvm = !(rscreen->debug_flags & DBG_NO_LLVM);
#endif
	ctx.bc = &shader->bc;
	ctx.shader = shader;
	ctx.native_integers = true;

	r600_bytecode_init(ctx.bc, rscreen->chip_class, rscreen->family,
			   rscreen->has_compressed_msaa_texturing);
	ctx.tokens = tokens;
	tgsi_scan_shader(tokens, &ctx.info);
	shader->indirect_files = ctx.info.indirect_files;
	indirect_gprs = ctx.info.indirect_files & ~(1 << TGSI_FILE_CONSTANT);
	tgsi_parse_init(&ctx.parse, tokens);
	ctx.type = ctx.parse.FullHeader.Processor.Processor;
	shader->processor_type = ctx.type;
	ctx.bc->type = shader->processor_type;

	ctx.face_gpr = -1;
	ctx.fragcoord_input = -1;
	ctx.colors_used = 0;
	ctx.clip_vertex_write = 0;

	shader->nr_ps_color_exports = 0;
	shader->nr_ps_max_color_exports = 0;

	shader->two_side = key.color_two_side;

	/* register allocations */
	/* Values [0,127] correspond to GPR[0..127].
	 * Values [128,159] correspond to constant buffer bank 0
	 * Values [160,191] correspond to constant buffer bank 1
	 * Values [256,511] correspond to cfile constants c[0..255]. (Gone on EG)
	 * Values [256,287] correspond to constant buffer bank 2 (EG)
	 * Values [288,319] correspond to constant buffer bank 3 (EG)
	 * Other special values are shown in the list below.
	 * 244  ALU_SRC_1_DBL_L: special constant 1.0 double-float, LSW. (RV670+)
	 * 245  ALU_SRC_1_DBL_M: special constant 1.0 double-float, MSW. (RV670+)
	 * 246  ALU_SRC_0_5_DBL_L: special constant 0.5 double-float, LSW. (RV670+)
	 * 247  ALU_SRC_0_5_DBL_M: special constant 0.5 double-float, MSW. (RV670+)
	 * 248	SQ_ALU_SRC_0: special constant 0.0.
	 * 249	SQ_ALU_SRC_1: special constant 1.0 float.
	 * 250	SQ_ALU_SRC_1_INT: special constant 1 integer.
	 * 251	SQ_ALU_SRC_M_1_INT: special constant -1 integer.
	 * 252	SQ_ALU_SRC_0_5: special constant 0.5 float.
	 * 253	SQ_ALU_SRC_LITERAL: literal constant.
	 * 254	SQ_ALU_SRC_PV: previous vector result.
	 * 255	SQ_ALU_SRC_PS: previous scalar result.
	 */
	for (i = 0; i < TGSI_FILE_COUNT; i++) {
		ctx.file_offset[i] = 0;
	}

#ifdef R600_USE_LLVM
	if (use_llvm && ctx.info.indirect_files && (ctx.info.indirect_files & (1 << TGSI_FILE_CONSTANT)) != ctx.info.indirect_files) {
		fprintf(stderr, "Warning: R600 LLVM backend does not support "
				"indirect adressing.  Falling back to TGSI "
				"backend.\n");
		use_llvm = 0;
	}
#endif
	if (ctx.type == TGSI_PROCESSOR_VERTEX) {
		ctx.file_offset[TGSI_FILE_INPUT] = 1;
		if (!use_llvm) {
			r600_bytecode_add_cfinst(ctx.bc, CF_OP_CALL_FS);
		}
	}
	if (ctx.type == TGSI_PROCESSOR_FRAGMENT && ctx.bc->chip_class >= EVERGREEN) {
		ctx.file_offset[TGSI_FILE_INPUT] = evergreen_gpr_count(&ctx);
	}
	ctx.use_llvm = use_llvm;

	if (use_llvm) {
		ctx.file_offset[TGSI_FILE_OUTPUT] =
			ctx.file_offset[TGSI_FILE_INPUT];
	} else {
	   ctx.file_offset[TGSI_FILE_OUTPUT] =
			ctx.file_offset[TGSI_FILE_INPUT] +
			ctx.info.file_max[TGSI_FILE_INPUT] + 1;
	}
	ctx.file_offset[TGSI_FILE_TEMPORARY] = ctx.file_offset[TGSI_FILE_OUTPUT] +
						ctx.info.file_max[TGSI_FILE_OUTPUT] + 1;

	/* Outside the GPR range. This will be translated to one of the
	 * kcache banks later. */
	ctx.file_offset[TGSI_FILE_CONSTANT] = 512;

	ctx.file_offset[TGSI_FILE_IMMEDIATE] = V_SQ_ALU_SRC_LITERAL;
	ctx.bc->ar_reg = ctx.file_offset[TGSI_FILE_TEMPORARY] +
			ctx.info.file_max[TGSI_FILE_TEMPORARY] + 1;
	ctx.temp_reg = ctx.bc->ar_reg + 1;

	if (indirect_gprs) {
		shader->max_arrays = 0;
		shader->num_arrays = 0;

		if (ctx.info.indirect_files & (1 << TGSI_FILE_INPUT)) {
			r600_add_gpr_array(shader, ctx.file_offset[TGSI_FILE_INPUT],
			                   ctx.file_offset[TGSI_FILE_OUTPUT] -
			                   ctx.file_offset[TGSI_FILE_INPUT],
			                   0x0F);
		}
		if (ctx.info.indirect_files & (1 << TGSI_FILE_OUTPUT)) {
			r600_add_gpr_array(shader, ctx.file_offset[TGSI_FILE_OUTPUT],
			                   ctx.file_offset[TGSI_FILE_TEMPORARY] -
			                   ctx.file_offset[TGSI_FILE_OUTPUT],
			                   0x0F);
		}
	}

	ctx.nliterals = 0;
	ctx.literals = NULL;
	shader->fs_write_all = FALSE;
	while (!tgsi_parse_end_of_tokens(&ctx.parse)) {
		tgsi_parse_token(&ctx.parse);
		switch (ctx.parse.FullToken.Token.Type) {
		case TGSI_TOKEN_TYPE_IMMEDIATE:
			immediate = &ctx.parse.FullToken.FullImmediate;
			ctx.literals = realloc(ctx.literals, (ctx.nliterals + 1) * 16);
			if(ctx.literals == NULL) {
				r = -ENOMEM;
				goto out_err;
			}
			ctx.literals[ctx.nliterals * 4 + 0] = immediate->u[0].Uint;
			ctx.literals[ctx.nliterals * 4 + 1] = immediate->u[1].Uint;
			ctx.literals[ctx.nliterals * 4 + 2] = immediate->u[2].Uint;
			ctx.literals[ctx.nliterals * 4 + 3] = immediate->u[3].Uint;
			ctx.nliterals++;
			break;
		case TGSI_TOKEN_TYPE_DECLARATION:
			r = tgsi_declaration(&ctx);
			if (r)
				goto out_err;
			break;
		case TGSI_TOKEN_TYPE_INSTRUCTION:
			break;
		case TGSI_TOKEN_TYPE_PROPERTY:
			property = &ctx.parse.FullToken.FullProperty;
			switch (property->Property.PropertyName) {
			case TGSI_PROPERTY_FS_COLOR0_WRITES_ALL_CBUFS:
				if (property->u[0].Data == 1)
					shader->fs_write_all = TRUE;
				break;
			case TGSI_PROPERTY_VS_PROHIBIT_UCPS:
				/* we don't need this one */
				break;
			}
			break;
		default:
			R600_ERR("unsupported token type %d\n", ctx.parse.FullToken.Token.Type);
			r = -EINVAL;
			goto out_err;
		}
	}
	
	/* Process two side if needed */
	if (shader->two_side && ctx.colors_used) {
		int i, count = ctx.shader->ninput;
		unsigned next_lds_loc = ctx.shader->nlds;

		/* additional inputs will be allocated right after the existing inputs,
		 * we won't need them after the color selection, so we don't need to
		 * reserve these gprs for the rest of the shader code and to adjust
		 * output offsets etc. */
		int gpr = ctx.file_offset[TGSI_FILE_INPUT] +
				ctx.info.file_max[TGSI_FILE_INPUT] + 1;

		if (ctx.face_gpr == -1) {
			i = ctx.shader->ninput++;
			ctx.shader->input[i].name = TGSI_SEMANTIC_FACE;
			ctx.shader->input[i].spi_sid = 0;
			ctx.shader->input[i].gpr = gpr++;
			ctx.face_gpr = ctx.shader->input[i].gpr;
		}

		for (i = 0; i < count; i++) {
			if (ctx.shader->input[i].name == TGSI_SEMANTIC_COLOR) {
				int ni = ctx.shader->ninput++;
				memcpy(&ctx.shader->input[ni],&ctx.shader->input[i], sizeof(struct r600_shader_io));
				ctx.shader->input[ni].name = TGSI_SEMANTIC_BCOLOR;
				ctx.shader->input[ni].spi_sid = r600_spi_sid(&ctx.shader->input[ni]);
				ctx.shader->input[ni].gpr = gpr++;
				// TGSI to LLVM needs to know the lds position of inputs.
				// Non LLVM path computes it later (in process_twoside_color)
				ctx.shader->input[ni].lds_pos = next_lds_loc++;
				ctx.shader->input[i].back_color_input = ni;
				if (ctx.bc->chip_class >= EVERGREEN) {
					if ((r = evergreen_interp_input(&ctx, ni)))
						return r;
				}
			}
		}
	}

/* LLVM backend setup */
#ifdef R600_USE_LLVM
	if (use_llvm) {
		struct radeon_llvm_context radeon_llvm_ctx;
		LLVMModuleRef mod;
		bool dump = r600_can_dump_shader(rscreen, ctx.type);
		boolean use_kill = false;

		memset(&radeon_llvm_ctx, 0, sizeof(radeon_llvm_ctx));
		radeon_llvm_ctx.type = ctx.type;
		radeon_llvm_ctx.two_side = shader->two_side;
		radeon_llvm_ctx.face_gpr = ctx.face_gpr;
		radeon_llvm_ctx.r600_inputs = ctx.shader->input;
		radeon_llvm_ctx.r600_outputs = ctx.shader->output;
		radeon_llvm_ctx.color_buffer_count = MAX2(key.nr_cbufs , 1);
		radeon_llvm_ctx.chip_class = ctx.bc->chip_class;
		radeon_llvm_ctx.fs_color_all = shader->fs_write_all && (rscreen->chip_class >= EVERGREEN);
		radeon_llvm_ctx.stream_outputs = &so;
		radeon_llvm_ctx.clip_vertex = ctx.cv_output;
		radeon_llvm_ctx.alpha_to_one = key.alpha_to_one;
		mod = r600_tgsi_llvm(&radeon_llvm_ctx, tokens);
		ctx.shader->has_txq_cube_array_z_comp = radeon_llvm_ctx.has_txq_cube_array_z_comp;

		if (r600_llvm_compile(mod, rscreen->family, ctx.bc, &use_kill, dump)) {
			radeon_llvm_dispose(&radeon_llvm_ctx);
			use_llvm = 0;
			fprintf(stderr, "R600 LLVM backend failed to compile "
				"shader.  Falling back to TGSI\n");
		} else {
			ctx.file_offset[TGSI_FILE_OUTPUT] =
					ctx.file_offset[TGSI_FILE_INPUT];
		}
		if (use_kill)
			ctx.shader->uses_kill = use_kill;
		radeon_llvm_dispose(&radeon_llvm_ctx);
	}
#endif
/* End of LLVM backend setup */

	if (shader->fs_write_all && rscreen->chip_class >= EVERGREEN)
		shader->nr_ps_max_color_exports = 8;

	if (!use_llvm) {
		if (ctx.fragcoord_input >= 0) {
			if (ctx.bc->chip_class == CAYMAN) {
				for (j = 0 ; j < 4; j++) {
					struct r600_bytecode_alu alu;
					memset(&alu, 0, sizeof(struct r600_bytecode_alu));
					alu.op = ALU_OP1_RECIP_IEEE;
					alu.src[0].sel = shader->input[ctx.fragcoord_input].gpr;
					alu.src[0].chan = 3;

					alu.dst.sel = shader->input[ctx.fragcoord_input].gpr;
					alu.dst.chan = j;
					alu.dst.write = (j == 3);
					alu.last = 1;
					if ((r = r600_bytecode_add_alu(ctx.bc, &alu)))
						return r;
				}
			} else {
				struct r600_bytecode_alu alu;
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_RECIP_IEEE;
				alu.src[0].sel = shader->input[ctx.fragcoord_input].gpr;
				alu.src[0].chan = 3;

				alu.dst.sel = shader->input[ctx.fragcoord_input].gpr;
				alu.dst.chan = 3;
				alu.dst.write = 1;
				alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx.bc, &alu)))
					return r;
			}
		}

		if (shader->two_side && ctx.colors_used) {
			if ((r = process_twoside_color_inputs(&ctx)))
				return r;
		}

		tgsi_parse_init(&ctx.parse, tokens);
		while (!tgsi_parse_end_of_tokens(&ctx.parse)) {
			tgsi_parse_token(&ctx.parse);
			switch (ctx.parse.FullToken.Token.Type) {
			case TGSI_TOKEN_TYPE_INSTRUCTION:
				r = tgsi_is_supported(&ctx);
				if (r)
					goto out_err;
				ctx.max_driver_temp_used = 0;
				/* reserve first tmp for everyone */
				r600_get_temp(&ctx);

				opcode = ctx.parse.FullToken.FullInstruction.Instruction.Opcode;
				if ((r = tgsi_split_constant(&ctx)))
					goto out_err;
				if ((r = tgsi_split_literal_constant(&ctx)))
					goto out_err;
				if (ctx.bc->chip_class == CAYMAN)
					ctx.inst_info = &cm_shader_tgsi_instruction[opcode];
				else if (ctx.bc->chip_class >= EVERGREEN)
					ctx.inst_info = &eg_shader_tgsi_instruction[opcode];
				else
					ctx.inst_info = &r600_shader_tgsi_instruction[opcode];
				r = ctx.inst_info->process(&ctx);
				if (r)
					goto out_err;
				break;
			default:
				break;
			}
		}
	}

	/* Reset the temporary register counter. */
	ctx.max_driver_temp_used = 0;

	noutput = shader->noutput;

	if (ctx.clip_vertex_write) {
		unsigned clipdist_temp[2];

		clipdist_temp[0] = r600_get_temp(&ctx);
		clipdist_temp[1] = r600_get_temp(&ctx);

		/* need to convert a clipvertex write into clipdistance writes and not export
		   the clip vertex anymore */

		memset(&shader->output[noutput], 0, 2*sizeof(struct r600_shader_io));
		shader->output[noutput].name = TGSI_SEMANTIC_CLIPDIST;
		shader->output[noutput].gpr = clipdist_temp[0];
		noutput++;
		shader->output[noutput].name = TGSI_SEMANTIC_CLIPDIST;
		shader->output[noutput].gpr = clipdist_temp[1];
		noutput++;

		/* reset spi_sid for clipvertex output to avoid confusing spi */
		shader->output[ctx.cv_output].spi_sid = 0;

		shader->clip_dist_write = 0xFF;

		for (i = 0; i < 8; i++) {
			int oreg = i >> 2;
			int ochan = i & 3;

			for (j = 0; j < 4; j++) {
				struct r600_bytecode_alu alu;
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_DOT4;
				alu.src[0].sel = shader->output[ctx.cv_output].gpr;
				alu.src[0].chan = j;

				alu.src[1].sel = 512 + i;
				alu.src[1].kc_bank = R600_UCP_CONST_BUFFER;
				alu.src[1].chan = j;

				alu.dst.sel = clipdist_temp[oreg];
				alu.dst.chan = j;
				alu.dst.write = (j == ochan);
				if (j == 3)
					alu.last = 1;
				if (!use_llvm)
					r = r600_bytecode_add_alu(ctx.bc, &alu);
				if (r)
					return r;
			}
		}
	}

	/* Add stream outputs. */
	if (ctx.type == TGSI_PROCESSOR_VERTEX && so.num_outputs && !use_llvm) {
		unsigned so_gpr[PIPE_MAX_SHADER_OUTPUTS];

		/* Sanity checking. */
		if (so.num_outputs > PIPE_MAX_SHADER_OUTPUTS) {
			R600_ERR("Too many stream outputs: %d\n", so.num_outputs);
			r = -EINVAL;
			goto out_err;
		}
		for (i = 0; i < so.num_outputs; i++) {
			if (so.output[i].output_buffer >= 4) {
				R600_ERR("Exceeded the max number of stream output buffers, got: %d\n",
					 so.output[i].output_buffer);
				r = -EINVAL;
				goto out_err;
			}
		}

		/* Initialize locations where the outputs are stored. */
		for (i = 0; i < so.num_outputs; i++) {
			so_gpr[i] = shader->output[so.output[i].register_index].gpr;

			/* Lower outputs with dst_offset < start_component.
			 *
			 * We can only output 4D vectors with a write mask, e.g. we can
			 * only output the W component at offset 3, etc. If we want
			 * to store Y, Z, or W at buffer offset 0, we need to use MOV
			 * to move it to X and output X. */
			if (so.output[i].dst_offset < so.output[i].start_component) {
				unsigned tmp = r600_get_temp(&ctx);

				for (j = 0; j < so.output[i].num_components; j++) {
					struct r600_bytecode_alu alu;
					memset(&alu, 0, sizeof(struct r600_bytecode_alu));
					alu.op = ALU_OP1_MOV;
					alu.src[0].sel = so_gpr[i];
					alu.src[0].chan = so.output[i].start_component + j;

					alu.dst.sel = tmp;
					alu.dst.chan = j;
					alu.dst.write = 1;
					if (j == so.output[i].num_components - 1)
						alu.last = 1;
					r = r600_bytecode_add_alu(ctx.bc, &alu);
					if (r)
						return r;
				}
				so.output[i].start_component = 0;
				so_gpr[i] = tmp;
			}
		}

		/* Write outputs to buffers. */
		for (i = 0; i < so.num_outputs; i++) {
			struct r600_bytecode_output output;

			memset(&output, 0, sizeof(struct r600_bytecode_output));
			output.gpr = so_gpr[i];
			output.elem_size = so.output[i].num_components;
			output.array_base = so.output[i].dst_offset - so.output[i].start_component;
			output.type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_WRITE;
			output.burst_count = 1;
			output.barrier = 1;
			/* array_size is an upper limit for the burst_count
			 * with MEM_STREAM instructions */
			output.array_size = 0xFFF;
			output.comp_mask = ((1 << so.output[i].num_components) - 1) << so.output[i].start_component;
			if (ctx.bc->chip_class >= EVERGREEN) {
				switch (so.output[i].output_buffer) {
				case 0:
					output.op = CF_OP_MEM_STREAM0_BUF0;
					break;
				case 1:
					output.op = CF_OP_MEM_STREAM0_BUF1;
					break;
				case 2:
					output.op = CF_OP_MEM_STREAM0_BUF2;
					break;
				case 3:
					output.op = CF_OP_MEM_STREAM0_BUF3;
					break;
				}
			} else {
				switch (so.output[i].output_buffer) {
				case 0:
					output.op = CF_OP_MEM_STREAM0;
					break;
				case 1:
					output.op = CF_OP_MEM_STREAM1;
					break;
				case 2:
					output.op = CF_OP_MEM_STREAM2;
					break;
				case 3:
					output.op = CF_OP_MEM_STREAM3;
					break;
				}
			}
			r = r600_bytecode_add_output(ctx.bc, &output);
			if (r)
				goto out_err;
		}
	}

	/* export output */
	for (i = 0, j = 0; i < noutput; i++, j++) {
		memset(&output[j], 0, sizeof(struct r600_bytecode_output));
		output[j].gpr = shader->output[i].gpr;
		output[j].elem_size = 3;
		output[j].swizzle_x = 0;
		output[j].swizzle_y = 1;
		output[j].swizzle_z = 2;
		output[j].swizzle_w = 3;
		output[j].burst_count = 1;
		output[j].barrier = 1;
		output[j].type = -1;
		output[j].op = CF_OP_EXPORT;
		switch (ctx.type) {
		case TGSI_PROCESSOR_VERTEX:
			switch (shader->output[i].name) {
			case TGSI_SEMANTIC_POSITION:
				output[j].array_base = next_pos_base++;
				output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS;
				break;

			case TGSI_SEMANTIC_PSIZE:
				output[j].array_base = next_pos_base++;
				output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS;
				break;
			case TGSI_SEMANTIC_CLIPVERTEX:
				j--;
				break;
			case TGSI_SEMANTIC_CLIPDIST:
				output[j].array_base = next_pos_base++;
				output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS;
				/* spi_sid is 0 for clipdistance outputs that were generated
				 * for clipvertex - we don't need to pass them to PS */
				if (shader->output[i].spi_sid) {
					j++;
					/* duplicate it as PARAM to pass to the pixel shader */
					memcpy(&output[j], &output[j-1], sizeof(struct r600_bytecode_output));
					output[j].array_base = next_param_base++;
					output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM;
				}
				break;
			case TGSI_SEMANTIC_FOG:
				output[j].swizzle_y = 4; /* 0 */
				output[j].swizzle_z = 4; /* 0 */
				output[j].swizzle_w = 5; /* 1 */
				break;
			}
			break;
		case TGSI_PROCESSOR_FRAGMENT:
			if (shader->output[i].name == TGSI_SEMANTIC_COLOR) {
				/* never export more colors than the number of CBs */
				if (next_pixel_base && next_pixel_base >= key.nr_cbufs) {
					/* skip export */
					j--;
					continue;
				}
				output[j].swizzle_w = key.alpha_to_one ? 5 : 3;
				output[j].array_base = next_pixel_base++;
				output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL;
				shader->nr_ps_color_exports++;
				if (shader->fs_write_all && (rscreen->chip_class >= EVERGREEN)) {
					for (k = 1; k < key.nr_cbufs; k++) {
						j++;
						memset(&output[j], 0, sizeof(struct r600_bytecode_output));
						output[j].gpr = shader->output[i].gpr;
						output[j].elem_size = 3;
						output[j].swizzle_x = 0;
						output[j].swizzle_y = 1;
						output[j].swizzle_z = 2;
						output[j].swizzle_w = key.alpha_to_one ? 5 : 3;
						output[j].burst_count = 1;
						output[j].barrier = 1;
						output[j].array_base = next_pixel_base++;
						output[j].op = CF_OP_EXPORT;
						output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL;
						shader->nr_ps_color_exports++;
					}
				}
			} else if (shader->output[i].name == TGSI_SEMANTIC_POSITION) {
				output[j].array_base = 61;
				output[j].swizzle_x = 2;
				output[j].swizzle_y = 7;
				output[j].swizzle_z = output[j].swizzle_w = 7;
				output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL;
			} else if (shader->output[i].name == TGSI_SEMANTIC_STENCIL) {
				output[j].array_base = 61;
				output[j].swizzle_x = 7;
				output[j].swizzle_y = 1;
				output[j].swizzle_z = output[j].swizzle_w = 7;
				output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL;
			} else {
				R600_ERR("unsupported fragment output name %d\n", shader->output[i].name);
				r = -EINVAL;
				goto out_err;
			}
			break;
		default:
			R600_ERR("unsupported processor type %d\n", ctx.type);
			r = -EINVAL;
			goto out_err;
		}

		if (output[j].type==-1) {
			output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM;
			output[j].array_base = next_param_base++;
		}
	}

        /* add fake position export */
	if (ctx.type == TGSI_PROCESSOR_VERTEX && next_pos_base == 60) {
			memset(&output[j], 0, sizeof(struct r600_bytecode_output));
			output[j].gpr = 0;
			output[j].elem_size = 3;
			output[j].swizzle_x = 7;
			output[j].swizzle_y = 7;
			output[j].swizzle_z = 7;
			output[j].swizzle_w = 7;
			output[j].burst_count = 1;
			output[j].barrier = 1;
			output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_POS;
			output[j].array_base = next_pos_base;
			output[j].op = CF_OP_EXPORT;
			j++;
	}

	/* add fake param output for vertex shader if no param is exported */
	if (ctx.type == TGSI_PROCESSOR_VERTEX && next_param_base == 0) {
			memset(&output[j], 0, sizeof(struct r600_bytecode_output));
			output[j].gpr = 0;
			output[j].elem_size = 3;
			output[j].swizzle_x = 7;
			output[j].swizzle_y = 7;
			output[j].swizzle_z = 7;
			output[j].swizzle_w = 7;
			output[j].burst_count = 1;
			output[j].barrier = 1;
			output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PARAM;
			output[j].array_base = 0;
			output[j].op = CF_OP_EXPORT;
			j++;
	}

	/* add fake pixel export */
	if (ctx.type == TGSI_PROCESSOR_FRAGMENT && next_pixel_base == 0) {
		memset(&output[j], 0, sizeof(struct r600_bytecode_output));
		output[j].gpr = 0;
		output[j].elem_size = 3;
		output[j].swizzle_x = 7;
		output[j].swizzle_y = 7;
		output[j].swizzle_z = 7;
		output[j].swizzle_w = 7;
		output[j].burst_count = 1;
		output[j].barrier = 1;
		output[j].type = V_SQ_CF_ALLOC_EXPORT_WORD0_SQ_EXPORT_PIXEL;
		output[j].array_base = 0;
		output[j].op = CF_OP_EXPORT;
		j++;
	}

	noutput = j;

	/* set export done on last export of each type */
	for (i = noutput - 1, output_done = 0; i >= 0; i--) {
		if (ctx.bc->chip_class < CAYMAN) {
			if (i == (noutput - 1)) {
				output[i].end_of_program = 1;
			}
		}
		if (!(output_done & (1 << output[i].type))) {
			output_done |= (1 << output[i].type);
			output[i].op = CF_OP_EXPORT_DONE;
		}
	}
	/* add output to bytecode */
	if (!use_llvm) {
		for (i = 0; i < noutput; i++) {
			r = r600_bytecode_add_output(ctx.bc, &output[i]);
			if (r)
				goto out_err;
		}
	}
	/* add program end */
	if (!use_llvm && ctx.bc->chip_class == CAYMAN)
		cm_bytecode_add_cf_end(ctx.bc);

	/* check GPR limit - we have 124 = 128 - 4
	 * (4 are reserved as alu clause temporary registers) */
	if (ctx.bc->ngpr > 124) {
		R600_ERR("GPR limit exceeded - shader requires %d registers\n", ctx.bc->ngpr);
		r = -ENOMEM;
		goto out_err;
	}

	free(ctx.literals);
	tgsi_parse_free(&ctx.parse);
	return 0;
out_err:
	free(ctx.literals);
	tgsi_parse_free(&ctx.parse);
	return r;
}

static int tgsi_unsupported(struct r600_shader_ctx *ctx)
{
	R600_ERR("%s tgsi opcode unsupported\n",
		 tgsi_get_opcode_name(ctx->inst_info->tgsi_opcode));
	return -EINVAL;
}

static int tgsi_end(struct r600_shader_ctx *ctx)
{
	return 0;
}

static void r600_bytecode_src(struct r600_bytecode_alu_src *bc_src,
			const struct r600_shader_src *shader_src,
			unsigned chan)
{
	bc_src->sel = shader_src->sel;
	bc_src->chan = shader_src->swizzle[chan];
	bc_src->neg = shader_src->neg;
	bc_src->abs = shader_src->abs;
	bc_src->rel = shader_src->rel;
	bc_src->value = shader_src->value[bc_src->chan];
	bc_src->kc_bank = shader_src->kc_bank;
}

static void r600_bytecode_src_set_abs(struct r600_bytecode_alu_src *bc_src)
{
	bc_src->abs = 1;
	bc_src->neg = 0;
}

static void r600_bytecode_src_toggle_neg(struct r600_bytecode_alu_src *bc_src)
{
	bc_src->neg = !bc_src->neg;
}

static void tgsi_dst(struct r600_shader_ctx *ctx,
		     const struct tgsi_full_dst_register *tgsi_dst,
		     unsigned swizzle,
		     struct r600_bytecode_alu_dst *r600_dst)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;

	r600_dst->sel = tgsi_dst->Register.Index;
	r600_dst->sel += ctx->file_offset[tgsi_dst->Register.File];
	r600_dst->chan = swizzle;
	r600_dst->write = 1;
	if (tgsi_dst->Register.Indirect)
		r600_dst->rel = V_SQ_REL_RELATIVE;
	if (inst->Instruction.Saturate) {
		r600_dst->clamp = 1;
	}
}

static int tgsi_last_instruction(unsigned writemask)
{
	int i, lasti = 0;

	for (i = 0; i < 4; i++) {
		if (writemask & (1 << i)) {
			lasti = i;
		}
	}
	return lasti;
}

static int tgsi_op2_s(struct r600_shader_ctx *ctx, int swap, int trans_only)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, j, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		alu.op = ctx->inst_info->op;
		if (!swap) {
			for (j = 0; j < inst->Instruction.NumSrcRegs; j++) {
				r600_bytecode_src(&alu.src[j], &ctx->src[j], i);
			}
		} else {
			r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
			r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
		}
		/* handle some special cases */
		switch (ctx->inst_info->tgsi_opcode) {
		case TGSI_OPCODE_SUB:
			r600_bytecode_src_toggle_neg(&alu.src[1]);
			break;
		case TGSI_OPCODE_ABS:
			r600_bytecode_src_set_abs(&alu.src[0]);
			break;
		default:
			break;
		}
		if (i == lasti || trans_only) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_op2(struct r600_shader_ctx *ctx)
{
	return tgsi_op2_s(ctx, 0, 0);
}

static int tgsi_op2_swap(struct r600_shader_ctx *ctx)
{
	return tgsi_op2_s(ctx, 1, 0);
}

static int tgsi_op2_trans(struct r600_shader_ctx *ctx)
{
	return tgsi_op2_s(ctx, 0, 1);
}

static int tgsi_ineg(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	for (i = 0; i < lasti + 1; i++) {

		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;

		alu.src[0].sel = V_SQ_ALU_SRC_0;

		r600_bytecode_src(&alu.src[1], &ctx->src[0], i);

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		if (i == lasti) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;

}

static int cayman_emit_float_instr(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	int i, j, r;
	struct r600_bytecode_alu alu;
	int last_slot = (inst->Dst[0].Register.WriteMask & 0x8) ? 4 : 3;
	
	for (i = 0 ; i < last_slot; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;
		for (j = 0; j < inst->Instruction.NumSrcRegs; j++) {
			r600_bytecode_src(&alu.src[j], &ctx->src[j], 0);

			/* RSQ should take the absolute value of src */
			if (ctx->inst_info->tgsi_opcode == TGSI_OPCODE_RSQ) {
				r600_bytecode_src_set_abs(&alu.src[j]);
			}
		}
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.write = (inst->Dst[0].Register.WriteMask >> i) & 1;

		if (i == last_slot - 1)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int cayman_mul_int_instr(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	int i, j, k, r;
	struct r600_bytecode_alu alu;
	int last_slot = (inst->Dst[0].Register.WriteMask & 0x8) ? 4 : 3;
	for (k = 0; k < last_slot; k++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << k)))
			continue;

		for (i = 0 ; i < 4; i++) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ctx->inst_info->op;
			for (j = 0; j < inst->Instruction.NumSrcRegs; j++) {
				r600_bytecode_src(&alu.src[j], &ctx->src[j], k);
			}
			tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
			alu.dst.write = (i == k);
			if (i == 3)
				alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}
	return 0;
}

/*
 * r600 - trunc to -PI..PI range
 * r700 - normalize by dividing by 2PI
 * see fdo bug 27901
 */
static int tgsi_setup_trig(struct r600_shader_ctx *ctx)
{
	static float half_inv_pi = 1.0 /(3.1415926535 * 2);
	static float double_pi = 3.1415926535 * 2;
	static float neg_pi = -3.1415926535;

	int r;
	struct r600_bytecode_alu alu;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP3_MULADD;
	alu.is_op3 = 1;

	alu.dst.chan = 0;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;

	r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);

	alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
	alu.src[1].chan = 0;
	alu.src[1].value = *(uint32_t *)&half_inv_pi;
	alu.src[2].sel = V_SQ_ALU_SRC_0_5;
	alu.src[2].chan = 0;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP1_FRACT;

	alu.dst.chan = 0;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;

	alu.src[0].sel = ctx->temp_reg;
	alu.src[0].chan = 0;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP3_MULADD;
	alu.is_op3 = 1;

	alu.dst.chan = 0;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;

	alu.src[0].sel = ctx->temp_reg;
	alu.src[0].chan = 0;

	alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
	alu.src[1].chan = 0;
	alu.src[2].sel = V_SQ_ALU_SRC_LITERAL;
	alu.src[2].chan = 0;

	if (ctx->bc->chip_class == R600) {
		alu.src[1].value = *(uint32_t *)&double_pi;
		alu.src[2].value = *(uint32_t *)&neg_pi;
	} else {
		alu.src[1].sel = V_SQ_ALU_SRC_1;
		alu.src[2].sel = V_SQ_ALU_SRC_0_5;
		alu.src[2].neg = 1;
	}

	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	return 0;
}

static int cayman_trig(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int last_slot = (inst->Dst[0].Register.WriteMask & 0x8) ? 4 : 3;
	int i, r;

	r = tgsi_setup_trig(ctx);
	if (r)
		return r;


	for (i = 0; i < last_slot; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;
		alu.dst.chan = i;

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.write = (inst->Dst[0].Register.WriteMask >> i) & 1;

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = 0;
		if (i == last_slot - 1)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_trig(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	r = tgsi_setup_trig(ctx);
	if (r)
		return r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ctx->inst_info->op;
	alu.dst.chan = 0;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;

	alu.src[0].sel = ctx->temp_reg;
	alu.src[0].chan = 0;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	/* replicate result */
	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP1_MOV;

		alu.src[0].sel = ctx->temp_reg;
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		if (i == lasti)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_scs(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;

	/* We'll only need the trig stuff if we are going to write to the
	 * X or Y components of the destination vector.
	 */
	if (likely(inst->Dst[0].Register.WriteMask & TGSI_WRITEMASK_XY)) {
		r = tgsi_setup_trig(ctx);
		if (r)
			return r;
	}

	/* dst.x = COS */
	if (inst->Dst[0].Register.WriteMask & TGSI_WRITEMASK_X) {
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0 ; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_COS;
				tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

				if (i == 0)
					alu.dst.write = 1;
				else
					alu.dst.write = 0;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 0;
				if (i == 2)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_COS;
			tgsi_dst(ctx, &inst->Dst[0], 0, &alu.dst);

			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 0;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}

	/* dst.y = SIN */
	if (inst->Dst[0].Register.WriteMask & TGSI_WRITEMASK_Y) {
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0 ; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_SIN;
				tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
				if (i == 1)
					alu.dst.write = 1;
				else
					alu.dst.write = 0;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 0;
				if (i == 2)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_SIN;
			tgsi_dst(ctx, &inst->Dst[0], 1, &alu.dst);

			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 0;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}

	/* dst.z = 0.0; */
	if (inst->Dst[0].Register.WriteMask & TGSI_WRITEMASK_Z) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_MOV;

		tgsi_dst(ctx, &inst->Dst[0], 2, &alu.dst);

		alu.src[0].sel = V_SQ_ALU_SRC_0;
		alu.src[0].chan = 0;

		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* dst.w = 1.0; */
	if (inst->Dst[0].Register.WriteMask & TGSI_WRITEMASK_W) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_MOV;

		tgsi_dst(ctx, &inst->Dst[0], 3, &alu.dst);

		alu.src[0].sel = V_SQ_ALU_SRC_1;
		alu.src[0].chan = 0;

		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	return 0;
}

static int tgsi_kill(struct r600_shader_ctx *ctx)
{
	struct r600_bytecode_alu alu;
	int i, r;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;

		alu.dst.chan = i;

		alu.src[0].sel = V_SQ_ALU_SRC_0;

		if (ctx->inst_info->tgsi_opcode == TGSI_OPCODE_KILL) {
			alu.src[1].sel = V_SQ_ALU_SRC_1;
			alu.src[1].neg = 1;
		} else {
			r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
		}
		if (i == 3) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* kill must be last in ALU */
	ctx->bc->force_add_cf = 1;
	ctx->shader->uses_kill = TRUE;
	return 0;
}

static int tgsi_lit(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int r;

	/* tmp.x = max(src.y, 0.0) */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP2_MAX;
	r600_bytecode_src(&alu.src[0], &ctx->src[0], 1);
	alu.src[1].sel  = V_SQ_ALU_SRC_0; /*0.0*/
	alu.src[1].chan = 1;

	alu.dst.sel = ctx->temp_reg;
	alu.dst.chan = 0;
	alu.dst.write = 1;

	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	if (inst->Dst[0].Register.WriteMask & (1 << 2))
	{
		int chan;
		int sel;
		int i;

		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				/* tmp.z = log(tmp.x) */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_LOG_CLAMPED;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 0;
				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 2) {
					alu.dst.write = 1;
					alu.last = 1;
				} else
					alu.dst.write = 0;
				
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			/* tmp.z = log(tmp.x) */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_LOG_CLAMPED;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 0;
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 2;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		chan = alu.dst.chan;
		sel = alu.dst.sel;

		/* tmp.x = amd MUL_LIT(tmp.z, src.w, src.x ) */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_MUL_LIT;
		alu.src[0].sel  = sel;
		alu.src[0].chan = chan;
		r600_bytecode_src(&alu.src[1], &ctx->src[0], 3);
		r600_bytecode_src(&alu.src[2], &ctx->src[0], 0);
		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 0;
		alu.dst.write = 1;
		alu.is_op3 = 1;
		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;

		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				/* dst.z = exp(tmp.x) */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_EXP_IEEE;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 0;
				tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
				if (i == 2) {
					alu.dst.write = 1;
					alu.last = 1;
				} else
					alu.dst.write = 0;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			/* dst.z = exp(tmp.x) */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_EXP_IEEE;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 0;
			tgsi_dst(ctx, &inst->Dst[0], 2, &alu.dst);
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}

	/* dst.x, <- 1.0  */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP1_MOV;
	alu.src[0].sel  = V_SQ_ALU_SRC_1; /*1.0*/
	alu.src[0].chan = 0;
	tgsi_dst(ctx, &inst->Dst[0], 0, &alu.dst);
	alu.dst.write = (inst->Dst[0].Register.WriteMask >> 0) & 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	/* dst.y = max(src.x, 0.0) */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP2_MAX;
	r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
	alu.src[1].sel  = V_SQ_ALU_SRC_0; /*0.0*/
	alu.src[1].chan = 0;
	tgsi_dst(ctx, &inst->Dst[0], 1, &alu.dst);
	alu.dst.write = (inst->Dst[0].Register.WriteMask >> 1) & 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	/* dst.w, <- 1.0  */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP1_MOV;
	alu.src[0].sel  = V_SQ_ALU_SRC_1;
	alu.src[0].chan = 0;
	tgsi_dst(ctx, &inst->Dst[0], 3, &alu.dst);
	alu.dst.write = (inst->Dst[0].Register.WriteMask >> 3) & 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	return 0;
}

static int tgsi_rsq(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));

	/* XXX:
	 * For state trackers other than OpenGL, we'll want to use
	 * _RECIPSQRT_IEEE instead.
	 */
	alu.op = ALU_OP1_RECIPSQRT_CLAMPED;

	for (i = 0; i < inst->Instruction.NumSrcRegs; i++) {
		r600_bytecode_src(&alu.src[i], &ctx->src[i], 0);
		r600_bytecode_src_set_abs(&alu.src[i]);
	}
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	/* replicate result */
	return tgsi_helper_tempx_replicate(ctx);
}

static int tgsi_helper_tempx_replicate(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.src[0].sel = ctx->temp_reg;
		alu.op = ALU_OP1_MOV;
		alu.dst.chan = i;
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.write = (inst->Dst[0].Register.WriteMask >> i) & 1;
		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_trans_srcx_replicate(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ctx->inst_info->op;
	for (i = 0; i < inst->Instruction.NumSrcRegs; i++) {
		r600_bytecode_src(&alu.src[i], &ctx->src[i], 0);
	}
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	/* replicate result */
	return tgsi_helper_tempx_replicate(ctx);
}

static int cayman_pow(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	int i, r;
	struct r600_bytecode_alu alu;
	int last_slot = (inst->Dst[0].Register.WriteMask & 0x8) ? 4 : 3;

	for (i = 0; i < 3; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP1_LOG_IEEE;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		alu.dst.write = 1;
		if (i == 2)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* b * LOG2(a) */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP2_MUL;
	r600_bytecode_src(&alu.src[0], &ctx->src[1], 0);
	alu.src[1].sel = ctx->temp_reg;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	for (i = 0; i < last_slot; i++) {
		/* POW(a,b) = EXP2(b * LOG2(a))*/
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP1_EXP_IEEE;
		alu.src[0].sel = ctx->temp_reg;

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.write = (inst->Dst[0].Register.WriteMask >> i) & 1;
		if (i == last_slot - 1)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_pow(struct r600_shader_ctx *ctx)
{
	struct r600_bytecode_alu alu;
	int r;

	/* LOG2(a) */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP1_LOG_IEEE;
	r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	/* b * LOG2(a) */
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP2_MUL;
	r600_bytecode_src(&alu.src[0], &ctx->src[1], 0);
	alu.src[1].sel = ctx->temp_reg;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	/* POW(a,b) = EXP2(b * LOG2(a))*/
	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP1_EXP_IEEE;
	alu.src[0].sel = ctx->temp_reg;
	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	return tgsi_helper_tempx_replicate(ctx);
}

static int tgsi_divmod(struct r600_shader_ctx *ctx, int mod, int signed_op)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r, j;
	unsigned write_mask = inst->Dst[0].Register.WriteMask;
	int tmp0 = ctx->temp_reg;
	int tmp1 = r600_get_temp(ctx);
	int tmp2 = r600_get_temp(ctx);
	int tmp3 = r600_get_temp(ctx);
	/* Unsigned path:
	 *
	 * we need to represent src1 as src2*q + r, where q - quotient, r - remainder
	 *
	 * 1. tmp0.x = rcp (src2)     = 2^32/src2 + e, where e is rounding error
	 * 2. tmp0.z = lo (tmp0.x * src2)
	 * 3. tmp0.w = -tmp0.z
	 * 4. tmp0.y = hi (tmp0.x * src2)
	 * 5. tmp0.z = (tmp0.y == 0 ? tmp0.w : tmp0.z)      = abs(lo(rcp*src2))
	 * 6. tmp0.w = hi (tmp0.z * tmp0.x)    = e, rounding error
	 * 7. tmp1.x = tmp0.x - tmp0.w
	 * 8. tmp1.y = tmp0.x + tmp0.w
	 * 9. tmp0.x = (tmp0.y == 0 ? tmp1.y : tmp1.x)
	 * 10. tmp0.z = hi(tmp0.x * src1)     = q
	 * 11. tmp0.y = lo (tmp0.z * src2)     = src2*q = src1 - r
	 *
	 * 12. tmp0.w = src1 - tmp0.y       = r
	 * 13. tmp1.x = tmp0.w >= src2		= r >= src2 (uint comparison)
	 * 14. tmp1.y = src1 >= tmp0.y      = r >= 0 (uint comparison)
	 *
	 * if DIV
	 *
	 *   15. tmp1.z = tmp0.z + 1			= q + 1
	 *   16. tmp1.w = tmp0.z - 1			= q - 1
	 *
	 * else MOD
	 *
	 *   15. tmp1.z = tmp0.w - src2			= r - src2
	 *   16. tmp1.w = tmp0.w + src2			= r + src2
	 *
	 * endif
	 *
	 * 17. tmp1.x = tmp1.x & tmp1.y
	 *
	 * DIV: 18. tmp0.z = tmp1.x==0 ? tmp0.z : tmp1.z
	 * MOD: 18. tmp0.z = tmp1.x==0 ? tmp0.w : tmp1.z
	 *
	 * 19. tmp0.z = tmp1.y==0 ? tmp1.w : tmp0.z
	 * 20. dst = src2==0 ? MAX_UINT : tmp0.z
	 *
	 * Signed path:
	 *
	 * Same as unsigned, using abs values of the operands,
	 * and fixing the sign of the result in the end.
	 */

	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		if (signed_op) {

			/* tmp2.x = -src0 */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_SUB_INT;

			alu.dst.sel = tmp2;
			alu.dst.chan = 0;
			alu.dst.write = 1;

			alu.src[0].sel = V_SQ_ALU_SRC_0;

			r600_bytecode_src(&alu.src[1], &ctx->src[0], i);

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

			/* tmp2.y = -src1 */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_SUB_INT;

			alu.dst.sel = tmp2;
			alu.dst.chan = 1;
			alu.dst.write = 1;

			alu.src[0].sel = V_SQ_ALU_SRC_0;

			r600_bytecode_src(&alu.src[1], &ctx->src[1], i);

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

			/* tmp2.z sign bit is set if src0 and src2 signs are different */
			/* it will be a sign of the quotient */
			if (!mod) {

				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_XOR_INT;

				alu.dst.sel = tmp2;
				alu.dst.chan = 2;
				alu.dst.write = 1;

				r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
				r600_bytecode_src(&alu.src[1], &ctx->src[1], i);

				alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}

			/* tmp2.x = |src0| */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP3_CNDGE_INT;
			alu.is_op3 = 1;

			alu.dst.sel = tmp2;
			alu.dst.chan = 0;
			alu.dst.write = 1;

			r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
			r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
			alu.src[2].sel = tmp2;
			alu.src[2].chan = 0;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

			/* tmp2.y = |src1| */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP3_CNDGE_INT;
			alu.is_op3 = 1;

			alu.dst.sel = tmp2;
			alu.dst.chan = 1;
			alu.dst.write = 1;

			r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
			r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
			alu.src[2].sel = tmp2;
			alu.src[2].chan = 1;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

		}

		/* 1. tmp0.x = rcp_u (src2)     = 2^32/src2 + e, where e is rounding error */
		if (ctx->bc->chip_class == CAYMAN) {
			/* tmp3.x = u2f(src2) */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_UINT_TO_FLT;

			alu.dst.sel = tmp3;
			alu.dst.chan = 0;
			alu.dst.write = 1;

			if (signed_op) {
				alu.src[0].sel = tmp2;
				alu.src[0].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
			}

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

			/* tmp0.x = recip(tmp3.x) */
			for (j = 0 ; j < 3; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_RECIP_IEEE;

				alu.dst.sel = tmp0;
				alu.dst.chan = j;
				alu.dst.write = (j == 0);

				alu.src[0].sel = tmp3;
				alu.src[0].chan = 0;

				if (j == 2)
					alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}

			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MUL;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 0;

			alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
			alu.src[1].value = 0x4f800000;

			alu.dst.sel = tmp3;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;

			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_FLT_TO_UINT;
		  
			alu.dst.sel = tmp0;
			alu.dst.chan = 0;
			alu.dst.write = 1;

			alu.src[0].sel = tmp3;
			alu.src[0].chan = 0;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_RECIP_UINT;

			alu.dst.sel = tmp0;
			alu.dst.chan = 0;
			alu.dst.write = 1;

			if (signed_op) {
				alu.src[0].sel = tmp2;
				alu.src[0].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
			}

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;
		}

		/* 2. tmp0.z = lo (tmp0.x * src2) */
		if (ctx->bc->chip_class == CAYMAN) {
			for (j = 0 ; j < 4; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_MULLO_UINT;

				alu.dst.sel = tmp0;
				alu.dst.chan = j;
				alu.dst.write = (j == 2);

				alu.src[0].sel = tmp0;
				alu.src[0].chan = 0;
				if (signed_op) {
					alu.src[1].sel = tmp2;
					alu.src[1].chan = 1;
				} else {
					r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
				}

				alu.last = (j == 3);
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MULLO_UINT;

			alu.dst.sel = tmp0;
			alu.dst.chan = 2;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 0;
			if (signed_op) {
				alu.src[1].sel = tmp2;
				alu.src[1].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
			}
			
			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;
		}

		/* 3. tmp0.w = -tmp0.z */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_SUB_INT;

		alu.dst.sel = tmp0;
		alu.dst.chan = 3;
		alu.dst.write = 1;

		alu.src[0].sel = V_SQ_ALU_SRC_0;
		alu.src[1].sel = tmp0;
		alu.src[1].chan = 2;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 4. tmp0.y = hi (tmp0.x * src2) */
		if (ctx->bc->chip_class == CAYMAN) {
			for (j = 0 ; j < 4; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_MULHI_UINT;

				alu.dst.sel = tmp0;
				alu.dst.chan = j;
				alu.dst.write = (j == 1);

				alu.src[0].sel = tmp0;
				alu.src[0].chan = 0;

				if (signed_op) {
					alu.src[1].sel = tmp2;
					alu.src[1].chan = 1;
				} else {
					r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
				}
				alu.last = (j == 3);
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MULHI_UINT;

			alu.dst.sel = tmp0;
			alu.dst.chan = 1;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 0;

			if (signed_op) {
				alu.src[1].sel = tmp2;
				alu.src[1].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
			}

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;
		}

		/* 5. tmp0.z = (tmp0.y == 0 ? tmp0.w : tmp0.z)      = abs(lo(rcp*src)) */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDE_INT;
		alu.is_op3 = 1;

		alu.dst.sel = tmp0;
		alu.dst.chan = 2;
		alu.dst.write = 1;

		alu.src[0].sel = tmp0;
		alu.src[0].chan = 1;
		alu.src[1].sel = tmp0;
		alu.src[1].chan = 3;
		alu.src[2].sel = tmp0;
		alu.src[2].chan = 2;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 6. tmp0.w = hi (tmp0.z * tmp0.x)    = e, rounding error */
		if (ctx->bc->chip_class == CAYMAN) {
			for (j = 0 ; j < 4; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_MULHI_UINT;

				alu.dst.sel = tmp0;
				alu.dst.chan = j;
				alu.dst.write = (j == 3);

				alu.src[0].sel = tmp0;
				alu.src[0].chan = 2;

				alu.src[1].sel = tmp0;
				alu.src[1].chan = 0;

				alu.last = (j == 3);
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MULHI_UINT;

			alu.dst.sel = tmp0;
			alu.dst.chan = 3;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 2;

			alu.src[1].sel = tmp0;
			alu.src[1].chan = 0;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;
		}

		/* 7. tmp1.x = tmp0.x - tmp0.w */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_SUB_INT;

		alu.dst.sel = tmp1;
		alu.dst.chan = 0;
		alu.dst.write = 1;

		alu.src[0].sel = tmp0;
		alu.src[0].chan = 0;
		alu.src[1].sel = tmp0;
		alu.src[1].chan = 3;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 8. tmp1.y = tmp0.x + tmp0.w */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_ADD_INT;

		alu.dst.sel = tmp1;
		alu.dst.chan = 1;
		alu.dst.write = 1;

		alu.src[0].sel = tmp0;
		alu.src[0].chan = 0;
		alu.src[1].sel = tmp0;
		alu.src[1].chan = 3;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 9. tmp0.x = (tmp0.y == 0 ? tmp1.y : tmp1.x) */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDE_INT;
		alu.is_op3 = 1;

		alu.dst.sel = tmp0;
		alu.dst.chan = 0;
		alu.dst.write = 1;

		alu.src[0].sel = tmp0;
		alu.src[0].chan = 1;
		alu.src[1].sel = tmp1;
		alu.src[1].chan = 1;
		alu.src[2].sel = tmp1;
		alu.src[2].chan = 0;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 10. tmp0.z = hi(tmp0.x * src1)     = q */
		if (ctx->bc->chip_class == CAYMAN) {
			for (j = 0 ; j < 4; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_MULHI_UINT;

				alu.dst.sel = tmp0;
				alu.dst.chan = j;
				alu.dst.write = (j == 2);

				alu.src[0].sel = tmp0;
				alu.src[0].chan = 0;

				if (signed_op) {
					alu.src[1].sel = tmp2;
					alu.src[1].chan = 0;
				} else {
					r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
				}

				alu.last = (j == 3);
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MULHI_UINT;

			alu.dst.sel = tmp0;
			alu.dst.chan = 2;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 0;

			if (signed_op) {
				alu.src[1].sel = tmp2;
				alu.src[1].chan = 0;
			} else {
				r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
			}

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;
		}

		/* 11. tmp0.y = lo (src2 * tmp0.z)     = src2*q = src1 - r */
		if (ctx->bc->chip_class == CAYMAN) {
			for (j = 0 ; j < 4; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_MULLO_UINT;

				alu.dst.sel = tmp0;
				alu.dst.chan = j;
				alu.dst.write = (j == 1);

				if (signed_op) {
					alu.src[0].sel = tmp2;
					alu.src[0].chan = 1;
				} else {
					r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
				}

				alu.src[1].sel = tmp0;
				alu.src[1].chan = 2;

				alu.last = (j == 3);
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MULLO_UINT;

			alu.dst.sel = tmp0;
			alu.dst.chan = 1;
			alu.dst.write = 1;

			if (signed_op) {
				alu.src[0].sel = tmp2;
				alu.src[0].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
			}
			
			alu.src[1].sel = tmp0;
			alu.src[1].chan = 2;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;
		}

		/* 12. tmp0.w = src1 - tmp0.y       = r */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_SUB_INT;

		alu.dst.sel = tmp0;
		alu.dst.chan = 3;
		alu.dst.write = 1;

		if (signed_op) {
			alu.src[0].sel = tmp2;
			alu.src[0].chan = 0;
		} else {
			r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		}

		alu.src[1].sel = tmp0;
		alu.src[1].chan = 1;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 13. tmp1.x = tmp0.w >= src2		= r >= src2 */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_SETGE_UINT;

		alu.dst.sel = tmp1;
		alu.dst.chan = 0;
		alu.dst.write = 1;

		alu.src[0].sel = tmp0;
		alu.src[0].chan = 3;
		if (signed_op) {
			alu.src[1].sel = tmp2;
			alu.src[1].chan = 1;
		} else {
			r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
		}

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 14. tmp1.y = src1 >= tmp0.y       = r >= 0 */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_SETGE_UINT;

		alu.dst.sel = tmp1;
		alu.dst.chan = 1;
		alu.dst.write = 1;

		if (signed_op) {
			alu.src[0].sel = tmp2;
			alu.src[0].chan = 0;
		} else {
			r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		}

		alu.src[1].sel = tmp0;
		alu.src[1].chan = 1;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		if (mod) { /* UMOD */

			/* 15. tmp1.z = tmp0.w - src2			= r - src2 */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_SUB_INT;

			alu.dst.sel = tmp1;
			alu.dst.chan = 2;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 3;

			if (signed_op) {
				alu.src[1].sel = tmp2;
				alu.src[1].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
			}

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

			/* 16. tmp1.w = tmp0.w + src2			= r + src2 */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_ADD_INT;

			alu.dst.sel = tmp1;
			alu.dst.chan = 3;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 3;
			if (signed_op) {
				alu.src[1].sel = tmp2;
				alu.src[1].chan = 1;
			} else {
				r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
			}

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

		} else { /* UDIV */

			/* 15. tmp1.z = tmp0.z + 1       = q + 1       DIV */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_ADD_INT;

			alu.dst.sel = tmp1;
			alu.dst.chan = 2;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 2;
			alu.src[1].sel = V_SQ_ALU_SRC_1_INT;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

			/* 16. tmp1.w = tmp0.z - 1			= q - 1 */
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_ADD_INT;

			alu.dst.sel = tmp1;
			alu.dst.chan = 3;
			alu.dst.write = 1;

			alu.src[0].sel = tmp0;
			alu.src[0].chan = 2;
			alu.src[1].sel = V_SQ_ALU_SRC_M_1_INT;

			alu.last = 1;
			if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
				return r;

		}

		/* 17. tmp1.x = tmp1.x & tmp1.y */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_AND_INT;

		alu.dst.sel = tmp1;
		alu.dst.chan = 0;
		alu.dst.write = 1;

		alu.src[0].sel = tmp1;
		alu.src[0].chan = 0;
		alu.src[1].sel = tmp1;
		alu.src[1].chan = 1;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 18. tmp0.z = tmp1.x==0 ? tmp0.z : tmp1.z    DIV */
		/* 18. tmp0.z = tmp1.x==0 ? tmp0.w : tmp1.z    MOD */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDE_INT;
		alu.is_op3 = 1;

		alu.dst.sel = tmp0;
		alu.dst.chan = 2;
		alu.dst.write = 1;

		alu.src[0].sel = tmp1;
		alu.src[0].chan = 0;
		alu.src[1].sel = tmp0;
		alu.src[1].chan = mod ? 3 : 2;
		alu.src[2].sel = tmp1;
		alu.src[2].chan = 2;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		/* 19. tmp0.z = tmp1.y==0 ? tmp1.w : tmp0.z */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDE_INT;
		alu.is_op3 = 1;

		if (signed_op) {
			alu.dst.sel = tmp0;
			alu.dst.chan = 2;
			alu.dst.write = 1;
		} else {
			tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		}

		alu.src[0].sel = tmp1;
		alu.src[0].chan = 1;
		alu.src[1].sel = tmp1;
		alu.src[1].chan = 3;
		alu.src[2].sel = tmp0;
		alu.src[2].chan = 2;

		alu.last = 1;
		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		if (signed_op) {

			/* fix the sign of the result */

			if (mod) {

				/* tmp0.x = -tmp0.z */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_SUB_INT;

				alu.dst.sel = tmp0;
				alu.dst.chan = 0;
				alu.dst.write = 1;

				alu.src[0].sel = V_SQ_ALU_SRC_0;
				alu.src[1].sel = tmp0;
				alu.src[1].chan = 2;

				alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;

				/* sign of the remainder is the same as the sign of src0 */
				/* tmp0.x = src0>=0 ? tmp0.z : tmp0.x */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP3_CNDGE_INT;
				alu.is_op3 = 1;

				tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

				r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
				alu.src[1].sel = tmp0;
				alu.src[1].chan = 2;
				alu.src[2].sel = tmp0;
				alu.src[2].chan = 0;

				alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;

			} else {

				/* tmp0.x = -tmp0.z */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_SUB_INT;

				alu.dst.sel = tmp0;
				alu.dst.chan = 0;
				alu.dst.write = 1;

				alu.src[0].sel = V_SQ_ALU_SRC_0;
				alu.src[1].sel = tmp0;
				alu.src[1].chan = 2;

				alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;

				/* fix the quotient sign (same as the sign of src0*src1) */
				/* tmp0.x = tmp2.z>=0 ? tmp0.z : tmp0.x */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP3_CNDGE_INT;
				alu.is_op3 = 1;

				tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

				alu.src[0].sel = tmp2;
				alu.src[0].chan = 2;
				alu.src[1].sel = tmp0;
				alu.src[1].chan = 2;
				alu.src[2].sel = tmp0;
				alu.src[2].chan = 0;

				alu.last = 1;
				if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
					return r;
			}
		}
	}
	return 0;
}

static int tgsi_udiv(struct r600_shader_ctx *ctx)
{
	return tgsi_divmod(ctx, 0, 0);
}

static int tgsi_umod(struct r600_shader_ctx *ctx)
{
	return tgsi_divmod(ctx, 1, 0);
}

static int tgsi_idiv(struct r600_shader_ctx *ctx)
{
	return tgsi_divmod(ctx, 0, 1);
}

static int tgsi_imod(struct r600_shader_ctx *ctx)
{
	return tgsi_divmod(ctx, 1, 1);
}


static int tgsi_f2i(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	unsigned write_mask = inst->Dst[0].Register.WriteMask;
	int last_inst = tgsi_last_instruction(write_mask);

	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP1_TRUNC;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		alu.dst.write = 1;

		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		if (i == last_inst)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = i;

		if (i == last_inst || alu.op == ALU_OP1_FLT_TO_UINT)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	return 0;
}

static int tgsi_iabs(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	unsigned write_mask = inst->Dst[0].Register.WriteMask;
	int last_inst = tgsi_last_instruction(write_mask);

	/* tmp = -src */
	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_SUB_INT;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		alu.dst.write = 1;

		r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
		alu.src[0].sel = V_SQ_ALU_SRC_0;

		if (i == last_inst)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* dst = (src >= 0 ? src : tmp) */
	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGE_INT;
		alu.is_op3 = 1;
		alu.dst.write = 1;

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
		alu.src[2].sel = ctx->temp_reg;
		alu.src[2].chan = i;

		if (i == last_inst)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_issg(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	unsigned write_mask = inst->Dst[0].Register.WriteMask;
	int last_inst = tgsi_last_instruction(write_mask);

	/* tmp = (src >= 0 ? src : -1) */
	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGE_INT;
		alu.is_op3 = 1;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		alu.dst.write = 1;

		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
		alu.src[2].sel = V_SQ_ALU_SRC_M_1_INT;

		if (i == last_inst)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* dst = (tmp > 0 ? 1 : tmp) */
	for (i = 0; i < 4; i++) {
		if (!(write_mask & (1<<i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGT_INT;
		alu.is_op3 = 1;
		alu.dst.write = 1;

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = i;

		alu.src[1].sel = V_SQ_ALU_SRC_1_INT;

		alu.src[2].sel = ctx->temp_reg;
		alu.src[2].chan = i;

		if (i == last_inst)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}



static int tgsi_ssg(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;

	/* tmp = (src > 0 ? 1 : src) */
	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGT;
		alu.is_op3 = 1;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;

		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		alu.src[1].sel = V_SQ_ALU_SRC_1;
		r600_bytecode_src(&alu.src[2], &ctx->src[0], i);

		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* dst = (-tmp > 0 ? -1 : tmp) */
	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGT;
		alu.is_op3 = 1;
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = i;
		alu.src[0].neg = 1;

		alu.src[1].sel = V_SQ_ALU_SRC_1;
		alu.src[1].neg = 1;

		alu.src[2].sel = ctx->temp_reg;
		alu.src[2].chan = i;

		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_helper_copy(struct r600_shader_ctx *ctx, struct tgsi_full_instruction *inst)
{
	struct r600_bytecode_alu alu;
	int i, r;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		if (!(inst->Dst[0].Register.WriteMask & (1 << i))) {
			alu.op = ALU_OP0_NOP;
			alu.dst.chan = i;
		} else {
			alu.op = ALU_OP1_MOV;
			tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = i;
		}
		if (i == 3) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_op3(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, j, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;
		for (j = 0; j < inst->Instruction.NumSrcRegs; j++) {
			r600_bytecode_src(&alu.src[j], &ctx->src[j], i);
		}

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.chan = i;
		alu.dst.write = 1;
		alu.is_op3 = 1;
		if (i == lasti) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_dp(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, j, r;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ctx->inst_info->op;
		for (j = 0; j < inst->Instruction.NumSrcRegs; j++) {
			r600_bytecode_src(&alu.src[j], &ctx->src[j], i);
		}

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.chan = i;
		alu.dst.write = (inst->Dst[0].Register.WriteMask >> i) & 1;
		/* handle some special cases */
		switch (ctx->inst_info->tgsi_opcode) {
		case TGSI_OPCODE_DP2:
			if (i > 1) {
				alu.src[0].sel = alu.src[1].sel = V_SQ_ALU_SRC_0;
				alu.src[0].chan = alu.src[1].chan = 0;
			}
			break;
		case TGSI_OPCODE_DP3:
			if (i > 2) {
				alu.src[0].sel = alu.src[1].sel = V_SQ_ALU_SRC_0;
				alu.src[0].chan = alu.src[1].chan = 0;
			}
			break;
		case TGSI_OPCODE_DPH:
			if (i == 3) {
				alu.src[0].sel = V_SQ_ALU_SRC_1;
				alu.src[0].chan = 0;
				alu.src[0].neg = 0;
			}
			break;
		default:
			break;
		}
		if (i == 3) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static inline boolean tgsi_tex_src_requires_loading(struct r600_shader_ctx *ctx,
						    unsigned index)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	return 	(inst->Src[index].Register.File != TGSI_FILE_TEMPORARY &&
		inst->Src[index].Register.File != TGSI_FILE_INPUT &&
		inst->Src[index].Register.File != TGSI_FILE_OUTPUT) ||
		ctx->src[index].neg || ctx->src[index].abs;
}

static inline unsigned tgsi_tex_get_src_gpr(struct r600_shader_ctx *ctx,
					unsigned index)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	return ctx->file_offset[inst->Src[index].Register.File] + inst->Src[index].Register.Index;
}

static int do_vtx_fetch_inst(struct r600_shader_ctx *ctx, boolean src_requires_loading)
{
	struct r600_bytecode_vtx vtx;
	struct r600_bytecode_alu alu;
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	int src_gpr, r, i;
	int id = tgsi_tex_get_src_gpr(ctx, 1);

	src_gpr = tgsi_tex_get_src_gpr(ctx, 0);
	if (src_requires_loading) {
		for (i = 0; i < 4; i++) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_MOV;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = i;
			if (i == 3)
				alu.last = 1;
			alu.dst.write = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
		src_gpr = ctx->temp_reg;
	}

	memset(&vtx, 0, sizeof(vtx));
	vtx.op = FETCH_OP_VFETCH;
	vtx.buffer_id = id + R600_MAX_CONST_BUFFERS;
	vtx.fetch_type = 2;		/* VTX_FETCH_NO_INDEX_OFFSET */
	vtx.src_gpr = src_gpr;
	vtx.mega_fetch_count = 16;
	vtx.dst_gpr = ctx->file_offset[inst->Dst[0].Register.File] + inst->Dst[0].Register.Index;
	vtx.dst_sel_x = (inst->Dst[0].Register.WriteMask & 1) ? 0 : 7;		/* SEL_X */
	vtx.dst_sel_y = (inst->Dst[0].Register.WriteMask & 2) ? 1 : 7;		/* SEL_Y */
	vtx.dst_sel_z = (inst->Dst[0].Register.WriteMask & 4) ? 2 : 7;		/* SEL_Z */
	vtx.dst_sel_w = (inst->Dst[0].Register.WriteMask & 8) ? 3 : 7;		/* SEL_W */
	vtx.use_const_fields = 1;
	vtx.srf_mode_all = 1;		/* SRF_MODE_NO_ZERO */

	if ((r = r600_bytecode_add_vtx(ctx->bc, &vtx)))
		return r;

	if (ctx->bc->chip_class >= EVERGREEN)
		return 0;

	for (i = 0; i < 4; i++) {
		int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_AND_INT;

		alu.dst.chan = i;
		alu.dst.sel = vtx.dst_gpr;
		alu.dst.write = 1;

		alu.src[0].sel = vtx.dst_gpr;
		alu.src[0].chan = i;

		alu.src[1].sel = 512 + (id * 2);
		alu.src[1].chan = i % 4;
		alu.src[1].kc_bank = R600_BUFFER_INFO_CONST_BUFFER;

		if (i == lasti)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	if (inst->Dst[0].Register.WriteMask & 3) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_OR_INT;

		alu.dst.chan = 3;
		alu.dst.sel = vtx.dst_gpr;
		alu.dst.write = 1;

		alu.src[0].sel = vtx.dst_gpr;
		alu.src[0].chan = 3;

		alu.src[1].sel = 512 + (id * 2) + 1;
		alu.src[1].chan = 0;
		alu.src[1].kc_bank = R600_BUFFER_INFO_CONST_BUFFER;

		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int r600_do_buffer_txq(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int r;
	int id = tgsi_tex_get_src_gpr(ctx, 1);

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = ALU_OP1_MOV;

	if (ctx->bc->chip_class >= EVERGREEN) {
		alu.src[0].sel = 512 + (id / 4);
		alu.src[0].chan = id % 4;
	} else {
		/* r600 we have them at channel 2 of the second dword */
		alu.src[0].sel = 512 + (id * 2) + 1;
		alu.src[0].chan = 1;
	}
	alu.src[0].kc_bank = R600_BUFFER_INFO_CONST_BUFFER;
	tgsi_dst(ctx, &inst->Dst[0], 0, &alu.dst);
	alu.last = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;
	return 0;
}

static int tgsi_tex(struct r600_shader_ctx *ctx)
{
	static float one_point_five = 1.5f;
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_tex tex;
	struct r600_bytecode_alu alu;
	unsigned src_gpr;
	int r, i, j;
	int opcode;
	bool read_compressed_msaa = ctx->bc->has_compressed_msaa_texturing &&
				    inst->Instruction.Opcode == TGSI_OPCODE_TXF &&
				    (inst->Texture.Texture == TGSI_TEXTURE_2D_MSAA ||
				     inst->Texture.Texture == TGSI_TEXTURE_2D_ARRAY_MSAA);

	/* Texture fetch instructions can only use gprs as source.
	 * Also they cannot negate the source or take the absolute value */
	const boolean src_requires_loading = (inst->Instruction.Opcode != TGSI_OPCODE_TXQ_LZ &&
                                              tgsi_tex_src_requires_loading(ctx, 0)) ||
					     read_compressed_msaa;
	boolean src_loaded = FALSE;
	unsigned sampler_src_reg = inst->Instruction.Opcode == TGSI_OPCODE_TXQ_LZ ? 0 : 1;
	int8_t offset_x = 0, offset_y = 0, offset_z = 0;
	boolean has_txq_cube_array_z = false;

	if (inst->Instruction.Opcode == TGSI_OPCODE_TXQ &&
	    ((inst->Texture.Texture == TGSI_TEXTURE_CUBE_ARRAY ||
	      inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY)))
		if (inst->Dst[0].Register.WriteMask & 4) {
			ctx->shader->has_txq_cube_array_z_comp = true;
			has_txq_cube_array_z = true;
		}

	if (inst->Instruction.Opcode == TGSI_OPCODE_TEX2 ||
	    inst->Instruction.Opcode == TGSI_OPCODE_TXB2 ||
	    inst->Instruction.Opcode == TGSI_OPCODE_TXL2)
		sampler_src_reg = 2;

	src_gpr = tgsi_tex_get_src_gpr(ctx, 0);

	if (inst->Texture.Texture == TGSI_TEXTURE_BUFFER) {
		if (inst->Instruction.Opcode == TGSI_OPCODE_TXQ) {
			ctx->shader->uses_tex_buffers = true;
			return r600_do_buffer_txq(ctx);
		}
		else if (inst->Instruction.Opcode == TGSI_OPCODE_TXF) {
			if (ctx->bc->chip_class < EVERGREEN)
				ctx->shader->uses_tex_buffers = true;
			return do_vtx_fetch_inst(ctx, src_requires_loading);
		}
	}

	if (inst->Instruction.Opcode == TGSI_OPCODE_TXF) {
		/* get offset values */
		if (inst->Texture.NumOffsets) {
			assert(inst->Texture.NumOffsets == 1);

			offset_x = ctx->literals[inst->TexOffsets[0].Index + inst->TexOffsets[0].SwizzleX] << 1;
			offset_y = ctx->literals[inst->TexOffsets[0].Index + inst->TexOffsets[0].SwizzleY] << 1;
			offset_z = ctx->literals[inst->TexOffsets[0].Index + inst->TexOffsets[0].SwizzleZ] << 1;
		}
	} else if (inst->Instruction.Opcode == TGSI_OPCODE_TXD) {
		/* TGSI moves the sampler to src reg 3 for TXD */
		sampler_src_reg = 3;

		for (i = 1; i < 3; i++) {
			/* set gradients h/v */
			memset(&tex, 0, sizeof(struct r600_bytecode_tex));
			tex.op = (i == 1) ? FETCH_OP_SET_GRADIENTS_H :
				FETCH_OP_SET_GRADIENTS_V;
			tex.sampler_id = tgsi_tex_get_src_gpr(ctx, sampler_src_reg);
			tex.resource_id = tex.sampler_id + R600_MAX_CONST_BUFFERS;

			if (tgsi_tex_src_requires_loading(ctx, i)) {
				tex.src_gpr = r600_get_temp(ctx);
				tex.src_sel_x = 0;
				tex.src_sel_y = 1;
				tex.src_sel_z = 2;
				tex.src_sel_w = 3;

				for (j = 0; j < 4; j++) {
					memset(&alu, 0, sizeof(struct r600_bytecode_alu));
					alu.op = ALU_OP1_MOV;
                                        r600_bytecode_src(&alu.src[0], &ctx->src[i], j);
                                        alu.dst.sel = tex.src_gpr;
                                        alu.dst.chan = j;
                                        if (j == 3)
                                                alu.last = 1;
                                        alu.dst.write = 1;
                                        r = r600_bytecode_add_alu(ctx->bc, &alu);
                                        if (r)
                                                return r;
				}

			} else {
				tex.src_gpr = tgsi_tex_get_src_gpr(ctx, i);
				tex.src_sel_x = ctx->src[i].swizzle[0];
				tex.src_sel_y = ctx->src[i].swizzle[1];
				tex.src_sel_z = ctx->src[i].swizzle[2];
				tex.src_sel_w = ctx->src[i].swizzle[3];
				tex.src_rel = ctx->src[i].rel;
			}
			tex.dst_gpr = ctx->temp_reg; /* just to avoid confusing the asm scheduler */
			tex.dst_sel_x = tex.dst_sel_y = tex.dst_sel_z = tex.dst_sel_w = 7;
			if (inst->Texture.Texture != TGSI_TEXTURE_RECT) {
				tex.coord_type_x = 1;
				tex.coord_type_y = 1;
				tex.coord_type_z = 1;
				tex.coord_type_w = 1;
			}
			r = r600_bytecode_add_tex(ctx->bc, &tex);
			if (r)
				return r;
		}
	} else if (inst->Instruction.Opcode == TGSI_OPCODE_TXP) {
		int out_chan;
		/* Add perspective divide */
		if (ctx->bc->chip_class == CAYMAN) {
			out_chan = 2;
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_RECIP_IEEE;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 3);

				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 2)
					alu.last = 1;
				if (out_chan == i)
					alu.dst.write = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}

		} else {
			out_chan = 3;
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_RECIP_IEEE;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], 3);

			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = out_chan;
			alu.last = 1;
			alu.dst.write = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		for (i = 0; i < 3; i++) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MUL;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = out_chan;
			r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = i;
			alu.dst.write = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP1_MOV;
		alu.src[0].sel = V_SQ_ALU_SRC_1;
		alu.src[0].chan = 0;
		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 3;
		alu.last = 1;
		alu.dst.write = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
		src_loaded = TRUE;
		src_gpr = ctx->temp_reg;
	}

	if ((inst->Texture.Texture == TGSI_TEXTURE_CUBE ||
	     inst->Texture.Texture == TGSI_TEXTURE_CUBE_ARRAY ||
	     inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE ||
	     inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY) &&
	    inst->Instruction.Opcode != TGSI_OPCODE_TXQ &&
	    inst->Instruction.Opcode != TGSI_OPCODE_TXQ_LZ) {

		static const unsigned src0_swizzle[] = {2, 2, 0, 1};
		static const unsigned src1_swizzle[] = {1, 0, 2, 2};

		/* tmp1.xyzw = CUBE(R0.zzxy, R0.yxzz) */
		for (i = 0; i < 4; i++) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_CUBE;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], src0_swizzle[i]);
			r600_bytecode_src(&alu.src[1], &ctx->src[0], src1_swizzle[i]);
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = i;
			if (i == 3)
				alu.last = 1;
			alu.dst.write = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		/* tmp1.z = RCP_e(|tmp1.z|) */
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_RECIP_IEEE;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 2;
				alu.src[0].abs = 1;
				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 2)
					alu.dst.write = 1;
				if (i == 2)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_RECIP_IEEE;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 2;
			alu.src[0].abs = 1;
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 2;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		/* MULADD R0.x,  R0.x,  PS1,  (0x3FC00000, 1.5f).x
		 * MULADD R0.y,  R0.y,  PS1,  (0x3FC00000, 1.5f).x
		 * muladd has no writemask, have to use another temp
		 */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_MULADD;
		alu.is_op3 = 1;

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = 0;
		alu.src[1].sel = ctx->temp_reg;
		alu.src[1].chan = 2;

		alu.src[2].sel = V_SQ_ALU_SRC_LITERAL;
		alu.src[2].chan = 0;
		alu.src[2].value = *(uint32_t *)&one_point_five;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 0;
		alu.dst.write = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_MULADD;
		alu.is_op3 = 1;

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = 1;
		alu.src[1].sel = ctx->temp_reg;
		alu.src[1].chan = 2;

		alu.src[2].sel = V_SQ_ALU_SRC_LITERAL;
		alu.src[2].chan = 0;
		alu.src[2].value = *(uint32_t *)&one_point_five;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 1;
		alu.dst.write = 1;

		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
		/* write initial compare value into Z component 
		  - W src 0 for shadow cube
		  - X src 1 for shadow cube array */
		if (inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE ||
		    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_MOV;
			if (inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY)
				r600_bytecode_src(&alu.src[0], &ctx->src[1], 0);
			else
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 3);
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 2;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		if (inst->Texture.Texture == TGSI_TEXTURE_CUBE_ARRAY ||
		    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY) {
			if (ctx->bc->chip_class >= EVERGREEN) {
				int mytmp = r600_get_temp(ctx);
				static const float eight = 8.0f;
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_MOV;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 3;
				alu.dst.sel = mytmp;
				alu.dst.chan = 0;
				alu.dst.write = 1;
				alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;

				/* have to multiply original layer by 8 and add to face id (temp.w) in Z */
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP3_MULADD;
				alu.is_op3 = 1;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 3);
				alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
				alu.src[1].chan = 0;
				alu.src[1].value = *(uint32_t *)&eight;
				alu.src[2].sel = mytmp;
				alu.src[2].chan = 0;
				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = 3;
				alu.dst.write = 1;
				alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			} else if (ctx->bc->chip_class < EVERGREEN) {
				memset(&tex, 0, sizeof(struct r600_bytecode_tex));
				tex.op = FETCH_OP_SET_CUBEMAP_INDEX;
				tex.sampler_id = tgsi_tex_get_src_gpr(ctx, sampler_src_reg);
				tex.resource_id = tex.sampler_id + R600_MAX_CONST_BUFFERS;
				tex.src_gpr = r600_get_temp(ctx);
				tex.src_sel_x = 0;
				tex.src_sel_y = 0;
				tex.src_sel_z = 0;
				tex.src_sel_w = 0;
				tex.dst_sel_x = tex.dst_sel_y = tex.dst_sel_z = tex.dst_sel_w = 7;
				tex.coord_type_x = 1;
				tex.coord_type_y = 1;
				tex.coord_type_z = 1;
				tex.coord_type_w = 1;
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_MOV;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 3);
				alu.dst.sel = tex.src_gpr;
				alu.dst.chan = 0;
				alu.last = 1;
				alu.dst.write = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
					
				r = r600_bytecode_add_tex(ctx->bc, &tex);
				if (r)
					return r;
			}

		}

		/* for cube forms of lod and bias we need to route things */
		if (inst->Instruction.Opcode == TGSI_OPCODE_TXB ||
		    inst->Instruction.Opcode == TGSI_OPCODE_TXL ||
		    inst->Instruction.Opcode == TGSI_OPCODE_TXB2 ||
		    inst->Instruction.Opcode == TGSI_OPCODE_TXL2) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_MOV;
			if (inst->Instruction.Opcode == TGSI_OPCODE_TXB2 ||
			    inst->Instruction.Opcode == TGSI_OPCODE_TXL2)
				r600_bytecode_src(&alu.src[0], &ctx->src[1], 0);
			else
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 3);
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 2;
			alu.last = 1;
			alu.dst.write = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		src_loaded = TRUE;
		src_gpr = ctx->temp_reg;
	}

	if (src_requires_loading && !src_loaded) {
		for (i = 0; i < 4; i++) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_MOV;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = i;
			if (i == 3)
				alu.last = 1;
			alu.dst.write = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
		src_loaded = TRUE;
		src_gpr = ctx->temp_reg;
	}

	/* Obtain the sample index for reading a compressed MSAA color texture.
	 * To read the FMASK, we use the ldfptr instruction, which tells us
	 * where the samples are stored.
	 * For uncompressed 8x MSAA surfaces, ldfptr should return 0x76543210,
	 * which is the identity mapping. Each nibble says which physical sample
	 * should be fetched to get that sample.
	 *
	 * Assume src.z contains the sample index. It should be modified like this:
	 *   src.z = (ldfptr() >> (src.z * 4)) & 0xF;
	 * Then fetch the texel with src.
	 */
	if (read_compressed_msaa) {
		unsigned sample_chan = 3;
		unsigned temp = r600_get_temp(ctx);
		assert(src_loaded);

		/* temp.w = ldfptr() */
		memset(&tex, 0, sizeof(struct r600_bytecode_tex));
		tex.op = FETCH_OP_LD;
		tex.inst_mod = 1; /* to indicate this is ldfptr */
		tex.sampler_id = tgsi_tex_get_src_gpr(ctx, sampler_src_reg);
		tex.resource_id = tex.sampler_id + R600_MAX_CONST_BUFFERS;
		tex.src_gpr = src_gpr;
		tex.dst_gpr = temp;
		tex.dst_sel_x = 7; /* mask out these components */
		tex.dst_sel_y = 7;
		tex.dst_sel_z = 7;
		tex.dst_sel_w = 0; /* store X */
		tex.src_sel_x = 0;
		tex.src_sel_y = 1;
		tex.src_sel_z = 2;
		tex.src_sel_w = 3;
		tex.offset_x = offset_x;
		tex.offset_y = offset_y;
		tex.offset_z = offset_z;
		r = r600_bytecode_add_tex(ctx->bc, &tex);
		if (r)
			return r;

		/* temp.x = sample_index*4 */
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0 ; i < 4; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP2_MULLO_INT;
				alu.src[0].sel = src_gpr;
				alu.src[0].chan = sample_chan;
				alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
				alu.src[1].value = 4;
				alu.dst.sel = temp;
				alu.dst.chan = i;
				alu.dst.write = i == 0;
				if (i == 3)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_MULLO_INT;
			alu.src[0].sel = src_gpr;
			alu.src[0].chan = sample_chan;
			alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
			alu.src[1].value = 4;
			alu.dst.sel = temp;
			alu.dst.chan = 0;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		/* sample_index = temp.w >> temp.x */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_LSHR_INT;
		alu.src[0].sel = temp;
		alu.src[0].chan = 3;
		alu.src[1].sel = temp;
		alu.src[1].chan = 0;
		alu.dst.sel = src_gpr;
		alu.dst.chan = sample_chan;
		alu.dst.write = 1;
		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;

		/* sample_index & 0xF */
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_AND_INT;
		alu.src[0].sel = src_gpr;
		alu.src[0].chan = sample_chan;
		alu.src[1].sel = V_SQ_ALU_SRC_LITERAL;
		alu.src[1].value = 0xF;
		alu.dst.sel = src_gpr;
		alu.dst.chan = sample_chan;
		alu.dst.write = 1;
		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
#if 0
		/* visualize the FMASK */
		for (i = 0; i < 4; i++) {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_INT_TO_FLT;
			alu.src[0].sel = src_gpr;
			alu.src[0].chan = sample_chan;
			alu.dst.sel = ctx->file_offset[inst->Dst[0].Register.File] + inst->Dst[0].Register.Index;
			alu.dst.chan = i;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
		return 0;
#endif
	}

	/* does this shader want a num layers from TXQ for a cube array? */
	if (has_txq_cube_array_z) {
		int id = tgsi_tex_get_src_gpr(ctx, sampler_src_reg);
		
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP1_MOV;

		alu.src[0].sel = 512 + (id / 4);
		alu.src[0].kc_bank = R600_TXQ_CONST_BUFFER;
		alu.src[0].chan = id % 4;
		tgsi_dst(ctx, &inst->Dst[0], 2, &alu.dst);
		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
		/* disable writemask from texture instruction */
		inst->Dst[0].Register.WriteMask &= ~4;
	}

	opcode = ctx->inst_info->op;
	if (inst->Texture.Texture == TGSI_TEXTURE_SHADOW1D ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOW2D ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOWRECT ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOW1D_ARRAY ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOW2D_ARRAY ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY) {
		switch (opcode) {
		case FETCH_OP_SAMPLE:
			opcode = FETCH_OP_SAMPLE_C;
			break;
		case FETCH_OP_SAMPLE_L:
			opcode = FETCH_OP_SAMPLE_C_L;
			break;
		case FETCH_OP_SAMPLE_LB:
			opcode = FETCH_OP_SAMPLE_C_LB;
			break;
		case FETCH_OP_SAMPLE_G:
			opcode = FETCH_OP_SAMPLE_C_G;
			break;
		}
	}

	memset(&tex, 0, sizeof(struct r600_bytecode_tex));
	tex.op = opcode;

	tex.sampler_id = tgsi_tex_get_src_gpr(ctx, sampler_src_reg);
	tex.resource_id = tex.sampler_id + R600_MAX_CONST_BUFFERS;
	tex.src_gpr = src_gpr;
	tex.dst_gpr = ctx->file_offset[inst->Dst[0].Register.File] + inst->Dst[0].Register.Index;
	tex.dst_sel_x = (inst->Dst[0].Register.WriteMask & 1) ? 0 : 7;
	tex.dst_sel_y = (inst->Dst[0].Register.WriteMask & 2) ? 1 : 7;
	tex.dst_sel_z = (inst->Dst[0].Register.WriteMask & 4) ? 2 : 7;
	tex.dst_sel_w = (inst->Dst[0].Register.WriteMask & 8) ? 3 : 7;

	if (inst->Instruction.Opcode == TGSI_OPCODE_TXQ_LZ) {
		tex.src_sel_x = 4;
		tex.src_sel_y = 4;
		tex.src_sel_z = 4;
		tex.src_sel_w = 4;
	} else if (src_loaded) {
		tex.src_sel_x = 0;
		tex.src_sel_y = 1;
		tex.src_sel_z = 2;
		tex.src_sel_w = 3;
	} else {
		tex.src_sel_x = ctx->src[0].swizzle[0];
		tex.src_sel_y = ctx->src[0].swizzle[1];
		tex.src_sel_z = ctx->src[0].swizzle[2];
		tex.src_sel_w = ctx->src[0].swizzle[3];
		tex.src_rel = ctx->src[0].rel;
	}

	if (inst->Texture.Texture == TGSI_TEXTURE_CUBE ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE ||
	    inst->Texture.Texture == TGSI_TEXTURE_CUBE_ARRAY ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY) {
		tex.src_sel_x = 1;
		tex.src_sel_y = 0;
		tex.src_sel_z = 3;
		tex.src_sel_w = 2; /* route Z compare or Lod value into W */
	}

	if (inst->Texture.Texture != TGSI_TEXTURE_RECT &&
	    inst->Texture.Texture != TGSI_TEXTURE_SHADOWRECT) {
		tex.coord_type_x = 1;
		tex.coord_type_y = 1;
	}
	tex.coord_type_z = 1;
	tex.coord_type_w = 1;

	tex.offset_x = offset_x;
	tex.offset_y = offset_y;
	tex.offset_z = offset_z;

	/* Put the depth for comparison in W.
	 * TGSI_TEXTURE_SHADOW2D_ARRAY already has the depth in W.
	 * Some instructions expect the depth in Z. */
	if ((inst->Texture.Texture == TGSI_TEXTURE_SHADOW1D ||
	     inst->Texture.Texture == TGSI_TEXTURE_SHADOW2D ||
	     inst->Texture.Texture == TGSI_TEXTURE_SHADOWRECT ||
	     inst->Texture.Texture == TGSI_TEXTURE_SHADOW1D_ARRAY) &&
	    opcode != FETCH_OP_SAMPLE_C_L &&
	    opcode != FETCH_OP_SAMPLE_C_LB) {
		tex.src_sel_w = tex.src_sel_z;
	}

	if (inst->Texture.Texture == TGSI_TEXTURE_1D_ARRAY ||
	    inst->Texture.Texture == TGSI_TEXTURE_SHADOW1D_ARRAY) {
		if (opcode == FETCH_OP_SAMPLE_C_L ||
		    opcode == FETCH_OP_SAMPLE_C_LB) {
			/* the array index is read from Y */
			tex.coord_type_y = 0;
		} else {
			/* the array index is read from Z */
			tex.coord_type_z = 0;
			tex.src_sel_z = tex.src_sel_y;
		}
	} else if (inst->Texture.Texture == TGSI_TEXTURE_2D_ARRAY ||
		   inst->Texture.Texture == TGSI_TEXTURE_SHADOW2D_ARRAY ||
		   ((inst->Texture.Texture == TGSI_TEXTURE_CUBE_ARRAY ||
		    inst->Texture.Texture == TGSI_TEXTURE_SHADOWCUBE_ARRAY) &&
		    (ctx->bc->chip_class >= EVERGREEN)))
		/* the array index is read from Z */
		tex.coord_type_z = 0;

	/* mask unused source components */
	if (opcode == FETCH_OP_SAMPLE) {
		switch (inst->Texture.Texture) {
		case TGSI_TEXTURE_2D:
		case TGSI_TEXTURE_RECT:
			tex.src_sel_z = 7;
			tex.src_sel_w = 7;
			break;
		case TGSI_TEXTURE_1D_ARRAY:
			tex.src_sel_y = 7;
			tex.src_sel_w = 7;
			break;
		case TGSI_TEXTURE_1D:
			tex.src_sel_y = 7;
			tex.src_sel_z = 7;
			tex.src_sel_w = 7;
			break;
		}
	}

	r = r600_bytecode_add_tex(ctx->bc, &tex);
	if (r)
		return r;

	/* add shadow ambient support  - gallium doesn't do it yet */
	return 0;
}

static int tgsi_lrp(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);
	unsigned i;
	int r;

	/* optimize if it's just an equal balance */
	if (ctx->src[0].sel == V_SQ_ALU_SRC_0_5) {
		for (i = 0; i < lasti + 1; i++) {
			if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
				continue;

			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP2_ADD;
			r600_bytecode_src(&alu.src[0], &ctx->src[1], i);
			r600_bytecode_src(&alu.src[1], &ctx->src[2], i);
			alu.omod = 3;
			tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
			alu.dst.chan = i;
			if (i == lasti) {
				alu.last = 1;
			}
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
		return 0;
	}

	/* 1 - src0 */
	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_ADD;
		alu.src[0].sel = V_SQ_ALU_SRC_1;
		alu.src[0].chan = 0;
		r600_bytecode_src(&alu.src[1], &ctx->src[0], i);
		r600_bytecode_src_toggle_neg(&alu.src[1]);
		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		if (i == lasti) {
			alu.last = 1;
		}
		alu.dst.write = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* (1 - src0) * src2 */
	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_MUL;
		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = i;
		r600_bytecode_src(&alu.src[1], &ctx->src[2], i);
		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		if (i == lasti) {
			alu.last = 1;
		}
		alu.dst.write = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* src0 * src1 + (1 - src0) * src2 */
	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_MULADD;
		alu.is_op3 = 1;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
		alu.src[2].sel = ctx->temp_reg;
		alu.src[2].chan = i;

		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.chan = i;
		if (i == lasti) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_cmp(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGE;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		r600_bytecode_src(&alu.src[1], &ctx->src[2], i);
		r600_bytecode_src(&alu.src[2], &ctx->src[1], i);
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.chan = i;
		alu.dst.write = 1;
		alu.is_op3 = 1;
		if (i == lasti)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_ucmp(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_CNDGE_INT;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		r600_bytecode_src(&alu.src[1], &ctx->src[2], i);
		r600_bytecode_src(&alu.src[2], &ctx->src[1], i);
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.chan = i;
		alu.dst.write = 1;
		alu.is_op3 = 1;
		if (i == lasti)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int tgsi_xpd(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	static const unsigned int src0_swizzle[] = {2, 0, 1};
	static const unsigned int src1_swizzle[] = {1, 2, 0};
	struct r600_bytecode_alu alu;
	uint32_t use_temp = 0;
	int i, r;

	if (inst->Dst[0].Register.WriteMask != 0xf)
		use_temp = 1;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP2_MUL;
		if (i < 3) {
			r600_bytecode_src(&alu.src[0], &ctx->src[0], src0_swizzle[i]);
			r600_bytecode_src(&alu.src[1], &ctx->src[1], src1_swizzle[i]);
		} else {
			alu.src[0].sel = V_SQ_ALU_SRC_0;
			alu.src[0].chan = i;
			alu.src[1].sel = V_SQ_ALU_SRC_0;
			alu.src[1].chan = i;
		}

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = i;
		alu.dst.write = 1;

		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		alu.op = ALU_OP3_MULADD;

		if (i < 3) {
			r600_bytecode_src(&alu.src[0], &ctx->src[0], src1_swizzle[i]);
			r600_bytecode_src(&alu.src[1], &ctx->src[1], src0_swizzle[i]);
		} else {
			alu.src[0].sel = V_SQ_ALU_SRC_0;
			alu.src[0].chan = i;
			alu.src[1].sel = V_SQ_ALU_SRC_0;
			alu.src[1].chan = i;
		}

		alu.src[2].sel = ctx->temp_reg;
		alu.src[2].neg = 1;
		alu.src[2].chan = i;

		if (use_temp)
			alu.dst.sel = ctx->temp_reg;
		else
			tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		alu.dst.chan = i;
		alu.dst.write = 1;
		alu.is_op3 = 1;
		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	if (use_temp)
		return tgsi_helper_copy(ctx, inst);
	return 0;
}

static int tgsi_exp(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int r;
	int i;

	/* result.x = 2^floor(src); */
	if (inst->Dst[0].Register.WriteMask & 1) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_FLOOR;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 0;
		alu.dst.write = 1;
		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;

		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				alu.op = ALU_OP1_EXP_IEEE;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 0;

				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				alu.dst.write = i == 0;
				alu.last = i == 2;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			alu.op = ALU_OP1_EXP_IEEE;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 0;

			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 0;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}

	/* result.y = tmp - floor(tmp); */
	if ((inst->Dst[0].Register.WriteMask >> 1) & 1) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_FRACT;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);

		alu.dst.sel = ctx->temp_reg;
#if 0
		r = tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);
		if (r)
			return r;
#endif
		alu.dst.write = 1;
		alu.dst.chan = 1;

		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* result.z = RoughApprox2ToX(tmp);*/
	if ((inst->Dst[0].Register.WriteMask >> 2) & 0x1) {
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_EXP_IEEE;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);

				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 2) {
					alu.dst.write = 1;
					alu.last = 1;
				}

				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_EXP_IEEE;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);

			alu.dst.sel = ctx->temp_reg;
			alu.dst.write = 1;
			alu.dst.chan = 2;

			alu.last = 1;

			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}

	/* result.w = 1.0;*/
	if ((inst->Dst[0].Register.WriteMask >> 3) & 0x1) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_MOV;
		alu.src[0].sel = V_SQ_ALU_SRC_1;
		alu.src[0].chan = 0;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 3;
		alu.dst.write = 1;
		alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return tgsi_helper_copy(ctx, inst);
}

static int tgsi_log(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int r;
	int i;

	/* result.x = floor(log2(|src|)); */
	if (inst->Dst[0].Register.WriteMask & 1) {
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));

				alu.op = ALU_OP1_LOG_IEEE;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
				r600_bytecode_src_set_abs(&alu.src[0]);
			
				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 0) 
					alu.dst.write = 1;
				if (i == 2)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}

		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));

			alu.op = ALU_OP1_LOG_IEEE;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
			r600_bytecode_src_set_abs(&alu.src[0]);
			
			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 0;
			alu.dst.write = 1;
			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		alu.op = ALU_OP1_FLOOR;
		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = 0;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 0;
		alu.dst.write = 1;
		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* result.y = |src.x| / (2 ^ floor(log2(|src.x|))); */
	if ((inst->Dst[0].Register.WriteMask >> 1) & 1) {

		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));

				alu.op = ALU_OP1_LOG_IEEE;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
				r600_bytecode_src_set_abs(&alu.src[0]);

				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 1)
					alu.dst.write = 1;
				if (i == 2)
					alu.last = 1;
				
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;	
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));

			alu.op = ALU_OP1_LOG_IEEE;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
			r600_bytecode_src_set_abs(&alu.src[0]);

			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 1;
			alu.dst.write = 1;
			alu.last = 1;

			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_FLOOR;
		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = 1;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 1;
		alu.dst.write = 1;
		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;

		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_EXP_IEEE;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 1;

				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 1)
					alu.dst.write = 1;
				if (i == 2)
					alu.last = 1;

				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_EXP_IEEE;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 1;

			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 1;
			alu.dst.write = 1;
			alu.last = 1;

			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));
				alu.op = ALU_OP1_RECIP_IEEE;
				alu.src[0].sel = ctx->temp_reg;
				alu.src[0].chan = 1;

				alu.dst.sel = ctx->temp_reg;
				alu.dst.chan = i;
				if (i == 1)
					alu.dst.write = 1;
				if (i == 2)
					alu.last = 1;
				
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));
			alu.op = ALU_OP1_RECIP_IEEE;
			alu.src[0].sel = ctx->temp_reg;
			alu.src[0].chan = 1;

			alu.dst.sel = ctx->temp_reg;
			alu.dst.chan = 1;
			alu.dst.write = 1;
			alu.last = 1;

			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP2_MUL;

		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
		r600_bytecode_src_set_abs(&alu.src[0]);

		alu.src[1].sel = ctx->temp_reg;
		alu.src[1].chan = 1;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 1;
		alu.dst.write = 1;
		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	/* result.z = log2(|src|);*/
	if ((inst->Dst[0].Register.WriteMask >> 2) & 1) {
		if (ctx->bc->chip_class == CAYMAN) {
			for (i = 0; i < 3; i++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));

				alu.op = ALU_OP1_LOG_IEEE;
				r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
				r600_bytecode_src_set_abs(&alu.src[0]);

				alu.dst.sel = ctx->temp_reg;
				if (i == 2)
					alu.dst.write = 1;
				alu.dst.chan = i;
				if (i == 2)
					alu.last = 1;

				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));

			alu.op = ALU_OP1_LOG_IEEE;
			r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
			r600_bytecode_src_set_abs(&alu.src[0]);

			alu.dst.sel = ctx->temp_reg;
			alu.dst.write = 1;
			alu.dst.chan = 2;
			alu.last = 1;

			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}

	/* result.w = 1.0; */
	if ((inst->Dst[0].Register.WriteMask >> 3) & 1) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP1_MOV;
		alu.src[0].sel = V_SQ_ALU_SRC_1;
		alu.src[0].chan = 0;

		alu.dst.sel = ctx->temp_reg;
		alu.dst.chan = 3;
		alu.dst.write = 1;
		alu.last = 1;

		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}

	return tgsi_helper_copy(ctx, inst);
}

static int tgsi_eg_arl(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));

	switch (inst->Instruction.Opcode) {
	case TGSI_OPCODE_ARL:
		alu.op = ALU_OP1_FLT_TO_INT_FLOOR;
		break;
	case TGSI_OPCODE_ARR:
		alu.op = ALU_OP1_FLT_TO_INT;
		break;
	case TGSI_OPCODE_UARL:
		alu.op = ALU_OP1_MOV;
		break;
	default:
		assert(0);
		return -1;
	}

	r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
	alu.last = 1;
	alu.dst.sel = ctx->bc->ar_reg;
	alu.dst.write = 1;
	r = r600_bytecode_add_alu(ctx->bc, &alu);
	if (r)
		return r;

	ctx->bc->ar_loaded = 0;
	return 0;
}
static int tgsi_r600_arl(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int r;

	switch (inst->Instruction.Opcode) {
	case TGSI_OPCODE_ARL:
		memset(&alu, 0, sizeof(alu));
		alu.op = ALU_OP1_FLOOR;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
		alu.dst.sel = ctx->bc->ar_reg;
		alu.dst.write = 1;
		alu.last = 1;

		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;

		memset(&alu, 0, sizeof(alu));
		alu.op = ALU_OP1_FLT_TO_INT;
		alu.src[0].sel = ctx->bc->ar_reg;
		alu.dst.sel = ctx->bc->ar_reg;
		alu.dst.write = 1;
		alu.last = 1;

		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;
		break;
	case TGSI_OPCODE_ARR:
		memset(&alu, 0, sizeof(alu));
		alu.op = ALU_OP1_FLT_TO_INT;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
		alu.dst.sel = ctx->bc->ar_reg;
		alu.dst.write = 1;
		alu.last = 1;

		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;
		break;
	case TGSI_OPCODE_UARL:
		memset(&alu, 0, sizeof(alu));
		alu.op = ALU_OP1_MOV;
		r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
		alu.dst.sel = ctx->bc->ar_reg;
		alu.dst.write = 1;
		alu.last = 1;

		if ((r = r600_bytecode_add_alu(ctx->bc, &alu)))
			return r;
		break;
	default:
		assert(0);
		return -1;
	}

	ctx->bc->ar_loaded = 0;
	return 0;
}

static int tgsi_opdst(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, r = 0;

	for (i = 0; i < 4; i++) {
		memset(&alu, 0, sizeof(struct r600_bytecode_alu));

		alu.op = ALU_OP2_MUL;
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		if (i == 0 || i == 3) {
			alu.src[0].sel = V_SQ_ALU_SRC_1;
		} else {
			r600_bytecode_src(&alu.src[0], &ctx->src[0], i);
		}

		if (i == 0 || i == 2) {
			alu.src[1].sel = V_SQ_ALU_SRC_1;
		} else {
			r600_bytecode_src(&alu.src[1], &ctx->src[1], i);
		}
		if (i == 3)
			alu.last = 1;
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static int emit_logic_pred(struct r600_shader_ctx *ctx, int opcode, int alu_type)
{
	struct r600_bytecode_alu alu;
	int r;

	memset(&alu, 0, sizeof(struct r600_bytecode_alu));
	alu.op = opcode;
	alu.execute_mask = 1;
	alu.update_pred = 1;

	alu.dst.sel = ctx->temp_reg;
	alu.dst.write = 1;
	alu.dst.chan = 0;

	r600_bytecode_src(&alu.src[0], &ctx->src[0], 0);
	alu.src[1].sel = V_SQ_ALU_SRC_0;
	alu.src[1].chan = 0;

	alu.last = 1;

	r = r600_bytecode_add_alu_type(ctx->bc, &alu, alu_type);
	if (r)
		return r;
	return 0;
}

static int pops(struct r600_shader_ctx *ctx, int pops)
{
	unsigned force_pop = ctx->bc->force_add_cf;

	if (!force_pop) {
		int alu_pop = 3;
		if (ctx->bc->cf_last) {
			if (ctx->bc->cf_last->op == CF_OP_ALU)
				alu_pop = 0;
			else if (ctx->bc->cf_last->op == CF_OP_ALU_POP_AFTER)
				alu_pop = 1;
		}
		alu_pop += pops;
		if (alu_pop == 1) {
			ctx->bc->cf_last->op = CF_OP_ALU_POP_AFTER;
			ctx->bc->force_add_cf = 1;
		} else if (alu_pop == 2) {
			ctx->bc->cf_last->op = CF_OP_ALU_POP2_AFTER;
			ctx->bc->force_add_cf = 1;
		} else {
			force_pop = 1;
		}
	}

	if (force_pop) {
		r600_bytecode_add_cfinst(ctx->bc, CF_OP_POP);
		ctx->bc->cf_last->pop_count = pops;
		ctx->bc->cf_last->cf_addr = ctx->bc->cf_last->id + 2;
	}

	return 0;
}

static inline void callstack_update_max_depth(struct r600_shader_ctx *ctx,
                                              unsigned reason)
{
	struct r600_stack_info *stack = &ctx->bc->stack;
	unsigned elements, entries;

	unsigned entry_size = stack->entry_size;

	elements = (stack->loop + stack->push_wqm ) * entry_size;
	elements += stack->push;

	switch (ctx->bc->chip_class) {
	case R600:
	case R700:
		/* pre-r8xx: if any non-WQM PUSH instruction is invoked, 2 elements on
		 * the stack must be reserved to hold the current active/continue
		 * masks */
		if (reason == FC_PUSH_VPM) {
			elements += 2;
		}
		break;

	case CAYMAN:
		/* r9xx: any stack operation on empty stack consumes 2 additional
		 * elements */
		elements += 2;

		/* fallthrough */
		/* FIXME: do the two elements added above cover the cases for the
		 * r8xx+ below? */

	case EVERGREEN:
		/* r8xx+: 2 extra elements are not always required, but one extra
		 * element must be added for each of the following cases:
		 * 1. There is an ALU_ELSE_AFTER instruction at the point of greatest
		 *    stack usage.
		 *    (Currently we don't use ALU_ELSE_AFTER.)
		 * 2. There are LOOP/WQM frames on the stack when any flavor of non-WQM
		 *    PUSH instruction executed.
		 *
		 *    NOTE: it seems we also need to reserve additional element in some
		 *    other cases, e.g. when we have 4 levels of PUSH_VPM in the shader,
		 *    then STACK_SIZE should be 2 instead of 1 */
		if (reason == FC_PUSH_VPM) {
			elements += 1;
		}
		break;

	default:
		assert(0);
		break;
	}

	/* NOTE: it seems STACK_SIZE is interpreted by hw as if entry_size is 4
	 * for all chips, so we use 4 in the final formula, not the real entry_size
	 * for the chip */
	entry_size = 4;

	entries = (elements + (entry_size - 1)) / entry_size;

	if (entries > stack->max_entries)
		stack->max_entries = entries;
}

static inline void callstack_pop(struct r600_shader_ctx *ctx, unsigned reason)
{
	switch(reason) {
	case FC_PUSH_VPM:
		--ctx->bc->stack.push;
		assert(ctx->bc->stack.push >= 0);
		break;
	case FC_PUSH_WQM:
		--ctx->bc->stack.push_wqm;
		assert(ctx->bc->stack.push_wqm >= 0);
		break;
	case FC_LOOP:
		--ctx->bc->stack.loop;
		assert(ctx->bc->stack.loop >= 0);
		break;
	default:
		assert(0);
		break;
	}
}

static inline void callstack_push(struct r600_shader_ctx *ctx, unsigned reason)
{
	switch (reason) {
	case FC_PUSH_VPM:
		++ctx->bc->stack.push;
		break;
	case FC_PUSH_WQM:
		++ctx->bc->stack.push_wqm;
	case FC_LOOP:
		++ctx->bc->stack.loop;
		break;
	default:
		assert(0);
	}

	callstack_update_max_depth(ctx, reason);
}

static void fc_set_mid(struct r600_shader_ctx *ctx, int fc_sp)
{
	struct r600_cf_stack_entry *sp = &ctx->bc->fc_stack[fc_sp];

	sp->mid = realloc((void *)sp->mid,
						sizeof(struct r600_bytecode_cf *) * (sp->num_mid + 1));
	sp->mid[sp->num_mid] = ctx->bc->cf_last;
	sp->num_mid++;
}

static void fc_pushlevel(struct r600_shader_ctx *ctx, int type)
{
	ctx->bc->fc_sp++;
	ctx->bc->fc_stack[ctx->bc->fc_sp].type = type;
	ctx->bc->fc_stack[ctx->bc->fc_sp].start = ctx->bc->cf_last;
}

static void fc_poplevel(struct r600_shader_ctx *ctx)
{
	struct r600_cf_stack_entry *sp = &ctx->bc->fc_stack[ctx->bc->fc_sp];
	free(sp->mid);
	sp->mid = NULL;
	sp->num_mid = 0;
	sp->start = NULL;
	sp->type = 0;
	ctx->bc->fc_sp--;
}

#if 0
static int emit_return(struct r600_shader_ctx *ctx)
{
	r600_bytecode_add_cfinst(ctx->bc, CF_OP_RETURN));
	return 0;
}

static int emit_jump_to_offset(struct r600_shader_ctx *ctx, int pops, int offset)
{

	r600_bytecode_add_cfinst(ctx->bc, CF_OP_JUMP));
	ctx->bc->cf_last->pop_count = pops;
	/* XXX work out offset */
	return 0;
}

static int emit_setret_in_loop_flag(struct r600_shader_ctx *ctx, unsigned flag_value)
{
	return 0;
}

static void emit_testflag(struct r600_shader_ctx *ctx)
{

}

static void emit_return_on_flag(struct r600_shader_ctx *ctx, unsigned ifidx)
{
	emit_testflag(ctx);
	emit_jump_to_offset(ctx, 1, 4);
	emit_setret_in_loop_flag(ctx, V_SQ_ALU_SRC_0);
	pops(ctx, ifidx + 1);
	emit_return(ctx);
}

static void break_loop_on_flag(struct r600_shader_ctx *ctx, unsigned fc_sp)
{
	emit_testflag(ctx);

	r600_bytecode_add_cfinst(ctx->bc, ctx->inst_info->op);
	ctx->bc->cf_last->pop_count = 1;

	fc_set_mid(ctx, fc_sp);

	pops(ctx, 1);
}
#endif

static int emit_if(struct r600_shader_ctx *ctx, int opcode)
{
	int alu_type = CF_OP_ALU_PUSH_BEFORE;

	/* There is a hardware bug on Cayman where a BREAK/CONTINUE followed by
	 * LOOP_STARTxxx for nested loops may put the branch stack into a state
	 * such that ALU_PUSH_BEFORE doesn't work as expected. Workaround this
	 * by replacing the ALU_PUSH_BEFORE with a PUSH + ALU */
	if (ctx->bc->chip_class == CAYMAN && ctx->bc->stack.loop > 1) {
		r600_bytecode_add_cfinst(ctx->bc, CF_OP_PUSH);
		ctx->bc->cf_last->cf_addr = ctx->bc->cf_last->id + 2;
		alu_type = CF_OP_ALU;
	}

	emit_logic_pred(ctx, opcode, alu_type);

	r600_bytecode_add_cfinst(ctx->bc, CF_OP_JUMP);

	fc_pushlevel(ctx, FC_IF);

	callstack_push(ctx, FC_PUSH_VPM);
	return 0;
}

static int tgsi_if(struct r600_shader_ctx *ctx)
{
	return emit_if(ctx, ALU_OP2_PRED_SETNE);
}

static int tgsi_uif(struct r600_shader_ctx *ctx)
{
	return emit_if(ctx, ALU_OP2_PRED_SETNE_INT);
}

static int tgsi_else(struct r600_shader_ctx *ctx)
{
	r600_bytecode_add_cfinst(ctx->bc, CF_OP_ELSE);
	ctx->bc->cf_last->pop_count = 1;

	fc_set_mid(ctx, ctx->bc->fc_sp);
	ctx->bc->fc_stack[ctx->bc->fc_sp].start->cf_addr = ctx->bc->cf_last->id;
	return 0;
}

static int tgsi_endif(struct r600_shader_ctx *ctx)
{
	pops(ctx, 1);
	if (ctx->bc->fc_stack[ctx->bc->fc_sp].type != FC_IF) {
		R600_ERR("if/endif unbalanced in shader\n");
		return -1;
	}

	if (ctx->bc->fc_stack[ctx->bc->fc_sp].mid == NULL) {
		ctx->bc->fc_stack[ctx->bc->fc_sp].start->cf_addr = ctx->bc->cf_last->id + 2;
		ctx->bc->fc_stack[ctx->bc->fc_sp].start->pop_count = 1;
	} else {
		ctx->bc->fc_stack[ctx->bc->fc_sp].mid[0]->cf_addr = ctx->bc->cf_last->id + 2;
	}
	fc_poplevel(ctx);

	callstack_pop(ctx, FC_PUSH_VPM);
	return 0;
}

static int tgsi_bgnloop(struct r600_shader_ctx *ctx)
{
	/* LOOP_START_DX10 ignores the LOOP_CONFIG* registers, so it is not
	 * limited to 4096 iterations, like the other LOOP_* instructions. */
	r600_bytecode_add_cfinst(ctx->bc, CF_OP_LOOP_START_DX10);

	fc_pushlevel(ctx, FC_LOOP);

	/* check stack depth */
	callstack_push(ctx, FC_LOOP);
	return 0;
}

static int tgsi_endloop(struct r600_shader_ctx *ctx)
{
	int i;

	r600_bytecode_add_cfinst(ctx->bc, CF_OP_LOOP_END);

	if (ctx->bc->fc_stack[ctx->bc->fc_sp].type != FC_LOOP) {
		R600_ERR("loop/endloop in shader code are not paired.\n");
		return -EINVAL;
	}

	/* fixup loop pointers - from r600isa
	   LOOP END points to CF after LOOP START,
	   LOOP START point to CF after LOOP END
	   BRK/CONT point to LOOP END CF
	*/
	ctx->bc->cf_last->cf_addr = ctx->bc->fc_stack[ctx->bc->fc_sp].start->id + 2;

	ctx->bc->fc_stack[ctx->bc->fc_sp].start->cf_addr = ctx->bc->cf_last->id + 2;

	for (i = 0; i < ctx->bc->fc_stack[ctx->bc->fc_sp].num_mid; i++) {
		ctx->bc->fc_stack[ctx->bc->fc_sp].mid[i]->cf_addr = ctx->bc->cf_last->id;
	}
	/* XXX add LOOPRET support */
	fc_poplevel(ctx);
	callstack_pop(ctx, FC_LOOP);
	return 0;
}

static int tgsi_loop_brk_cont(struct r600_shader_ctx *ctx)
{
	unsigned int fscp;

	for (fscp = ctx->bc->fc_sp; fscp > 0; fscp--)
	{
		if (FC_LOOP == ctx->bc->fc_stack[fscp].type)
			break;
	}

	if (fscp == 0) {
		R600_ERR("Break not inside loop/endloop pair\n");
		return -EINVAL;
	}

	r600_bytecode_add_cfinst(ctx->bc, ctx->inst_info->op);

	fc_set_mid(ctx, fscp);

	return 0;
}

static int tgsi_umad(struct r600_shader_ctx *ctx)
{
	struct tgsi_full_instruction *inst = &ctx->parse.FullToken.FullInstruction;
	struct r600_bytecode_alu alu;
	int i, j, k, r;
	int lasti = tgsi_last_instruction(inst->Dst[0].Register.WriteMask);

	/* src0 * src1 */
	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		if (ctx->bc->chip_class == CAYMAN) {
			for (j = 0 ; j < 4; j++) {
				memset(&alu, 0, sizeof(struct r600_bytecode_alu));

				alu.op = ALU_OP2_MULLO_UINT;
				for (k = 0; k < inst->Instruction.NumSrcRegs; k++) {
					r600_bytecode_src(&alu.src[k], &ctx->src[k], i);
				}
				tgsi_dst(ctx, &inst->Dst[0], j, &alu.dst);
				alu.dst.sel = ctx->temp_reg;
				alu.dst.write = (j == i);
				if (j == 3)
					alu.last = 1;
				r = r600_bytecode_add_alu(ctx->bc, &alu);
				if (r)
					return r;
			}
		} else {
			memset(&alu, 0, sizeof(struct r600_bytecode_alu));

			alu.dst.chan = i;
			alu.dst.sel = ctx->temp_reg;
			alu.dst.write = 1;

			alu.op = ALU_OP2_MULLO_UINT;
			for (j = 0; j < 2; j++) {
				r600_bytecode_src(&alu.src[j], &ctx->src[j], i);
			}

			alu.last = 1;
			r = r600_bytecode_add_alu(ctx->bc, &alu);
			if (r)
				return r;
		}
	}


	for (i = 0; i < lasti + 1; i++) {
		if (!(inst->Dst[0].Register.WriteMask & (1 << i)))
			continue;

		memset(&alu, 0, sizeof(struct r600_bytecode_alu));
		tgsi_dst(ctx, &inst->Dst[0], i, &alu.dst);

		alu.op = ALU_OP2_ADD_INT;

		alu.src[0].sel = ctx->temp_reg;
		alu.src[0].chan = i;
		
		r600_bytecode_src(&alu.src[1], &ctx->src[2], i);
		if (i == lasti) {
			alu.last = 1;
		}
		r = r600_bytecode_add_alu(ctx->bc, &alu);
		if (r)
			return r;
	}
	return 0;
}

static struct r600_shader_tgsi_instruction r600_shader_tgsi_instruction[] = {
	{TGSI_OPCODE_ARL,	0, ALU_OP0_NOP, tgsi_r600_arl},
	{TGSI_OPCODE_MOV,	0, ALU_OP1_MOV, tgsi_op2},
	{TGSI_OPCODE_LIT,	0, ALU_OP0_NOP, tgsi_lit},

	/* XXX:
	 * For state trackers other than OpenGL, we'll want to use
	 * _RECIP_IEEE instead.
	 */
	{TGSI_OPCODE_RCP,	0, ALU_OP1_RECIP_CLAMPED, tgsi_trans_srcx_replicate},

	{TGSI_OPCODE_RSQ,	0, ALU_OP0_NOP, tgsi_rsq},
	{TGSI_OPCODE_EXP,	0, ALU_OP0_NOP, tgsi_exp},
	{TGSI_OPCODE_LOG,	0, ALU_OP0_NOP, tgsi_log},
	{TGSI_OPCODE_MUL,	0, ALU_OP2_MUL, tgsi_op2},
	{TGSI_OPCODE_ADD,	0, ALU_OP2_ADD, tgsi_op2},
	{TGSI_OPCODE_DP3,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_DP4,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_DST,	0, ALU_OP0_NOP, tgsi_opdst},
	{TGSI_OPCODE_MIN,	0, ALU_OP2_MIN, tgsi_op2},
	{TGSI_OPCODE_MAX,	0, ALU_OP2_MAX, tgsi_op2},
	{TGSI_OPCODE_SLT,	0, ALU_OP2_SETGT, tgsi_op2_swap},
	{TGSI_OPCODE_SGE,	0, ALU_OP2_SETGE, tgsi_op2},
	{TGSI_OPCODE_MAD,	1, ALU_OP3_MULADD, tgsi_op3},
	{TGSI_OPCODE_SUB,	0, ALU_OP2_ADD, tgsi_op2},
	{TGSI_OPCODE_LRP,	0, ALU_OP0_NOP, tgsi_lrp},
	{TGSI_OPCODE_CND,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{20,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DP2A,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{22,			0, ALU_OP0_NOP, tgsi_unsupported},
	{23,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_FRC,	0, ALU_OP1_FRACT, tgsi_op2},
	{TGSI_OPCODE_CLAMP,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_FLR,	0, ALU_OP1_FLOOR, tgsi_op2},
	{TGSI_OPCODE_ROUND,	0, ALU_OP1_RNDNE, tgsi_op2},
	{TGSI_OPCODE_EX2,	0, ALU_OP1_EXP_IEEE, tgsi_trans_srcx_replicate},
	{TGSI_OPCODE_LG2,	0, ALU_OP1_LOG_IEEE, tgsi_trans_srcx_replicate},
	{TGSI_OPCODE_POW,	0, ALU_OP0_NOP, tgsi_pow},
	{TGSI_OPCODE_XPD,	0, ALU_OP0_NOP, tgsi_xpd},
	/* gap */
	{32,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ABS,	0, ALU_OP1_MOV, tgsi_op2},
	{TGSI_OPCODE_RCC,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DPH,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_COS,	0, ALU_OP1_COS, tgsi_trig},
	{TGSI_OPCODE_DDX,	0, FETCH_OP_GET_GRADIENTS_H, tgsi_tex},
	{TGSI_OPCODE_DDY,	0, FETCH_OP_GET_GRADIENTS_V, tgsi_tex},
	{TGSI_OPCODE_KILL,	0, ALU_OP2_KILLGT, tgsi_kill},  /* unconditional kill */
	{TGSI_OPCODE_PK2H,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK2US,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK4B,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK4UB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_RFL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SEQ,	0, ALU_OP2_SETE, tgsi_op2},
	{TGSI_OPCODE_SFL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SGT,	0, ALU_OP2_SETGT, tgsi_op2},
	{TGSI_OPCODE_SIN,	0, ALU_OP1_SIN, tgsi_trig},
	{TGSI_OPCODE_SLE,	0, ALU_OP2_SETGE, tgsi_op2_swap},
	{TGSI_OPCODE_SNE,	0, ALU_OP2_SETNE, tgsi_op2},
	{TGSI_OPCODE_STR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TEX,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_TXD,	0, FETCH_OP_SAMPLE_G, tgsi_tex},
	{TGSI_OPCODE_TXP,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_UP2H,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP2US,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP4B,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP4UB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_X2D,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ARA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ARR,	0, ALU_OP0_NOP, tgsi_r600_arl},
	{TGSI_OPCODE_BRA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CAL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_RET,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SSG,	0, ALU_OP0_NOP, tgsi_ssg},
	{TGSI_OPCODE_CMP,	0, ALU_OP0_NOP, tgsi_cmp},
	{TGSI_OPCODE_SCS,	0, ALU_OP0_NOP, tgsi_scs},
	{TGSI_OPCODE_TXB,	0, FETCH_OP_SAMPLE_LB, tgsi_tex},
	{TGSI_OPCODE_NRM,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DIV,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DP2,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_TXL,	0, FETCH_OP_SAMPLE_L, tgsi_tex},
	{TGSI_OPCODE_BRK,	0, CF_OP_LOOP_BREAK, tgsi_loop_brk_cont},
	{TGSI_OPCODE_IF,	0, ALU_OP0_NOP, tgsi_if},
	{TGSI_OPCODE_UIF,	0, ALU_OP0_NOP, tgsi_uif},
	{76,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ELSE,	0, ALU_OP0_NOP, tgsi_else},
	{TGSI_OPCODE_ENDIF,	0, ALU_OP0_NOP, tgsi_endif},
	/* gap */
	{79,			0, ALU_OP0_NOP, tgsi_unsupported},
	{80,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PUSHA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_POPA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CEIL,	0, ALU_OP1_CEIL, tgsi_op2},
	{TGSI_OPCODE_I2F,	0, ALU_OP1_INT_TO_FLT, tgsi_op2_trans},
	{TGSI_OPCODE_NOT,	0, ALU_OP1_NOT_INT, tgsi_op2},
	{TGSI_OPCODE_TRUNC,	0, ALU_OP1_TRUNC, tgsi_op2},
	{TGSI_OPCODE_SHL,	0, ALU_OP2_LSHL_INT, tgsi_op2_trans},
	/* gap */
	{88,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_AND,	0, ALU_OP2_AND_INT, tgsi_op2},
	{TGSI_OPCODE_OR,	0, ALU_OP2_OR_INT, tgsi_op2},
	{TGSI_OPCODE_MOD,	0, ALU_OP0_NOP, tgsi_imod},
	{TGSI_OPCODE_XOR,	0, ALU_OP2_XOR_INT, tgsi_op2},
	{TGSI_OPCODE_SAD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TXF,	0, FETCH_OP_LD, tgsi_tex},
	{TGSI_OPCODE_TXQ,	0, FETCH_OP_GET_TEXTURE_RESINFO, tgsi_tex},
	{TGSI_OPCODE_CONT,	0, CF_OP_LOOP_CONTINUE, tgsi_loop_brk_cont},
	{TGSI_OPCODE_EMIT,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDPRIM,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BGNLOOP,	0, ALU_OP0_NOP, tgsi_bgnloop},
	{TGSI_OPCODE_BGNSUB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDLOOP,	0, ALU_OP0_NOP, tgsi_endloop},
	{TGSI_OPCODE_ENDSUB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TXQ_LZ,	0, FETCH_OP_GET_TEXTURE_RESINFO, tgsi_tex},
	/* gap */
	{104,			0, ALU_OP0_NOP, tgsi_unsupported},
	{105,			0, ALU_OP0_NOP, tgsi_unsupported},
	{106,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_NOP,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{108,			0, ALU_OP0_NOP, tgsi_unsupported},
	{109,			0, ALU_OP0_NOP, tgsi_unsupported},
	{110,			0, ALU_OP0_NOP, tgsi_unsupported},
	{111,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_NRM4,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CALLNZ,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{114,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BREAKC,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_KILL_IF,	0, ALU_OP2_KILLGT, tgsi_kill},  /* conditional kill */
	{TGSI_OPCODE_END,	0, ALU_OP0_NOP, tgsi_end},  /* aka HALT */
	/* gap */
	{118,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_F2I,	0, ALU_OP1_FLT_TO_INT, tgsi_op2_trans},
	{TGSI_OPCODE_IDIV,	0, ALU_OP0_NOP, tgsi_idiv},
	{TGSI_OPCODE_IMAX,	0, ALU_OP2_MAX_INT, tgsi_op2},
	{TGSI_OPCODE_IMIN,	0, ALU_OP2_MIN_INT, tgsi_op2},
	{TGSI_OPCODE_INEG,	0, ALU_OP2_SUB_INT, tgsi_ineg},
	{TGSI_OPCODE_ISGE,	0, ALU_OP2_SETGE_INT, tgsi_op2},
	{TGSI_OPCODE_ISHR,	0, ALU_OP2_ASHR_INT, tgsi_op2_trans},
	{TGSI_OPCODE_ISLT,	0, ALU_OP2_SETGT_INT, tgsi_op2_swap},
	{TGSI_OPCODE_F2U,	0, ALU_OP1_FLT_TO_UINT, tgsi_op2_trans},
	{TGSI_OPCODE_U2F,	0, ALU_OP1_UINT_TO_FLT, tgsi_op2_trans},
	{TGSI_OPCODE_UADD,	0, ALU_OP2_ADD_INT, tgsi_op2},
	{TGSI_OPCODE_UDIV,	0, ALU_OP0_NOP, tgsi_udiv},
	{TGSI_OPCODE_UMAD,	0, ALU_OP0_NOP, tgsi_umad},
	{TGSI_OPCODE_UMAX,	0, ALU_OP2_MAX_UINT, tgsi_op2},
	{TGSI_OPCODE_UMIN,	0, ALU_OP2_MIN_UINT, tgsi_op2},
	{TGSI_OPCODE_UMOD,	0, ALU_OP0_NOP, tgsi_umod},
	{TGSI_OPCODE_UMUL,	0, ALU_OP2_MULLO_UINT, tgsi_op2_trans},
	{TGSI_OPCODE_USEQ,	0, ALU_OP2_SETE_INT, tgsi_op2},
	{TGSI_OPCODE_USGE,	0, ALU_OP2_SETGE_UINT, tgsi_op2},
	{TGSI_OPCODE_USHR,	0, ALU_OP2_LSHR_INT, tgsi_op2_trans},
	{TGSI_OPCODE_USLT,	0, ALU_OP2_SETGT_UINT, tgsi_op2_swap},
	{TGSI_OPCODE_USNE,	0, ALU_OP2_SETNE_INT, tgsi_op2_swap},
	{TGSI_OPCODE_SWITCH,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CASE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DEFAULT,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDSWITCH,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE,    0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_I,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_I_MS, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_B,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_C,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_C_LZ, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_D,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_L,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_GATHER4,   0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SVIEWINFO,	0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_POS, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_INFO, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_UARL,      0, ALU_OP1_MOVA_INT, tgsi_r600_arl},
	{TGSI_OPCODE_UCMP,	0, ALU_OP0_NOP, tgsi_ucmp},
	{TGSI_OPCODE_IABS,      0, 0, tgsi_iabs},
	{TGSI_OPCODE_ISSG,      0, 0, tgsi_issg},
	{TGSI_OPCODE_LOAD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_STORE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_MFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_LFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BARRIER,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUADD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMXCHG,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMCAS,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMAND,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMOR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMXOR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUMIN,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUMAX,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMIMIN,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMIMAX,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TEX2,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_TXB2,	0, FETCH_OP_SAMPLE_LB, tgsi_tex},
	{TGSI_OPCODE_TXL2,	0, FETCH_OP_SAMPLE_L, tgsi_tex},
	{TGSI_OPCODE_LAST,	0, ALU_OP0_NOP, tgsi_unsupported},
};

static struct r600_shader_tgsi_instruction eg_shader_tgsi_instruction[] = {
	{TGSI_OPCODE_ARL,	0, ALU_OP0_NOP, tgsi_eg_arl},
	{TGSI_OPCODE_MOV,	0, ALU_OP1_MOV, tgsi_op2},
	{TGSI_OPCODE_LIT,	0, ALU_OP0_NOP, tgsi_lit},
	{TGSI_OPCODE_RCP,	0, ALU_OP1_RECIP_IEEE, tgsi_trans_srcx_replicate},
	{TGSI_OPCODE_RSQ,	0, ALU_OP1_RECIPSQRT_IEEE, tgsi_rsq},
	{TGSI_OPCODE_EXP,	0, ALU_OP0_NOP, tgsi_exp},
	{TGSI_OPCODE_LOG,	0, ALU_OP0_NOP, tgsi_log},
	{TGSI_OPCODE_MUL,	0, ALU_OP2_MUL, tgsi_op2},
	{TGSI_OPCODE_ADD,	0, ALU_OP2_ADD, tgsi_op2},
	{TGSI_OPCODE_DP3,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_DP4,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_DST,	0, ALU_OP0_NOP, tgsi_opdst},
	{TGSI_OPCODE_MIN,	0, ALU_OP2_MIN, tgsi_op2},
	{TGSI_OPCODE_MAX,	0, ALU_OP2_MAX, tgsi_op2},
	{TGSI_OPCODE_SLT,	0, ALU_OP2_SETGT, tgsi_op2_swap},
	{TGSI_OPCODE_SGE,	0, ALU_OP2_SETGE, tgsi_op2},
	{TGSI_OPCODE_MAD,	1, ALU_OP3_MULADD, tgsi_op3},
	{TGSI_OPCODE_SUB,	0, ALU_OP2_ADD, tgsi_op2},
	{TGSI_OPCODE_LRP,	0, ALU_OP0_NOP, tgsi_lrp},
	{TGSI_OPCODE_CND,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{20,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DP2A,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{22,			0, ALU_OP0_NOP, tgsi_unsupported},
	{23,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_FRC,	0, ALU_OP1_FRACT, tgsi_op2},
	{TGSI_OPCODE_CLAMP,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_FLR,	0, ALU_OP1_FLOOR, tgsi_op2},
	{TGSI_OPCODE_ROUND,	0, ALU_OP1_RNDNE, tgsi_op2},
	{TGSI_OPCODE_EX2,	0, ALU_OP1_EXP_IEEE, tgsi_trans_srcx_replicate},
	{TGSI_OPCODE_LG2,	0, ALU_OP1_LOG_IEEE, tgsi_trans_srcx_replicate},
	{TGSI_OPCODE_POW,	0, ALU_OP0_NOP, tgsi_pow},
	{TGSI_OPCODE_XPD,	0, ALU_OP0_NOP, tgsi_xpd},
	/* gap */
	{32,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ABS,	0, ALU_OP1_MOV, tgsi_op2},
	{TGSI_OPCODE_RCC,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DPH,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_COS,	0, ALU_OP1_COS, tgsi_trig},
	{TGSI_OPCODE_DDX,	0, FETCH_OP_GET_GRADIENTS_H, tgsi_tex},
	{TGSI_OPCODE_DDY,	0, FETCH_OP_GET_GRADIENTS_V, tgsi_tex},
	{TGSI_OPCODE_KILL,	0, ALU_OP2_KILLGT, tgsi_kill},  /* unconditional kill */
	{TGSI_OPCODE_PK2H,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK2US,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK4B,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK4UB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_RFL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SEQ,	0, ALU_OP2_SETE, tgsi_op2},
	{TGSI_OPCODE_SFL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SGT,	0, ALU_OP2_SETGT, tgsi_op2},
	{TGSI_OPCODE_SIN,	0, ALU_OP1_SIN, tgsi_trig},
	{TGSI_OPCODE_SLE,	0, ALU_OP2_SETGE, tgsi_op2_swap},
	{TGSI_OPCODE_SNE,	0, ALU_OP2_SETNE, tgsi_op2},
	{TGSI_OPCODE_STR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TEX,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_TXD,	0, FETCH_OP_SAMPLE_G, tgsi_tex},
	{TGSI_OPCODE_TXP,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_UP2H,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP2US,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP4B,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP4UB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_X2D,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ARA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ARR,	0, ALU_OP0_NOP, tgsi_eg_arl},
	{TGSI_OPCODE_BRA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CAL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_RET,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SSG,	0, ALU_OP0_NOP, tgsi_ssg},
	{TGSI_OPCODE_CMP,	0, ALU_OP0_NOP, tgsi_cmp},
	{TGSI_OPCODE_SCS,	0, ALU_OP0_NOP, tgsi_scs},
	{TGSI_OPCODE_TXB,	0, FETCH_OP_SAMPLE_LB, tgsi_tex},
	{TGSI_OPCODE_NRM,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DIV,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DP2,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_TXL,	0, FETCH_OP_SAMPLE_L, tgsi_tex},
	{TGSI_OPCODE_BRK,	0, CF_OP_LOOP_BREAK, tgsi_loop_brk_cont},
	{TGSI_OPCODE_IF,	0, ALU_OP0_NOP, tgsi_if},
	{TGSI_OPCODE_UIF,	0, ALU_OP0_NOP, tgsi_uif},
	{76,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ELSE,	0, ALU_OP0_NOP, tgsi_else},
	{TGSI_OPCODE_ENDIF,	0, ALU_OP0_NOP, tgsi_endif},
	/* gap */
	{79,			0, ALU_OP0_NOP, tgsi_unsupported},
	{80,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PUSHA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_POPA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CEIL,	0, ALU_OP1_CEIL, tgsi_op2},
	{TGSI_OPCODE_I2F,	0, ALU_OP1_INT_TO_FLT, tgsi_op2_trans},
	{TGSI_OPCODE_NOT,	0, ALU_OP1_NOT_INT, tgsi_op2},
	{TGSI_OPCODE_TRUNC,	0, ALU_OP1_TRUNC, tgsi_op2},
	{TGSI_OPCODE_SHL,	0, ALU_OP2_LSHL_INT, tgsi_op2},
	/* gap */
	{88,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_AND,	0, ALU_OP2_AND_INT, tgsi_op2},
	{TGSI_OPCODE_OR,	0, ALU_OP2_OR_INT, tgsi_op2},
	{TGSI_OPCODE_MOD,	0, ALU_OP0_NOP, tgsi_imod},
	{TGSI_OPCODE_XOR,	0, ALU_OP2_XOR_INT, tgsi_op2},
	{TGSI_OPCODE_SAD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TXF,	0, FETCH_OP_LD, tgsi_tex},
	{TGSI_OPCODE_TXQ,	0, FETCH_OP_GET_TEXTURE_RESINFO, tgsi_tex},
	{TGSI_OPCODE_CONT,	0, CF_OP_LOOP_CONTINUE, tgsi_loop_brk_cont},
	{TGSI_OPCODE_EMIT,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDPRIM,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BGNLOOP,	0, ALU_OP0_NOP, tgsi_bgnloop},
	{TGSI_OPCODE_BGNSUB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDLOOP,	0, ALU_OP0_NOP, tgsi_endloop},
	{TGSI_OPCODE_ENDSUB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TXQ_LZ,	0, FETCH_OP_GET_TEXTURE_RESINFO, tgsi_tex},
	/* gap */
	{104,			0, ALU_OP0_NOP, tgsi_unsupported},
	{105,			0, ALU_OP0_NOP, tgsi_unsupported},
	{106,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_NOP,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{108,			0, ALU_OP0_NOP, tgsi_unsupported},
	{109,			0, ALU_OP0_NOP, tgsi_unsupported},
	{110,			0, ALU_OP0_NOP, tgsi_unsupported},
	{111,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_NRM4,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CALLNZ,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{114,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BREAKC,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_KILL_IF,	0, ALU_OP2_KILLGT, tgsi_kill},  /* conditional kill */
	{TGSI_OPCODE_END,	0, ALU_OP0_NOP, tgsi_end},  /* aka HALT */
	/* gap */
	{118,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_F2I,	0, ALU_OP1_FLT_TO_INT, tgsi_f2i},
	{TGSI_OPCODE_IDIV,	0, ALU_OP0_NOP, tgsi_idiv},
	{TGSI_OPCODE_IMAX,	0, ALU_OP2_MAX_INT, tgsi_op2},
	{TGSI_OPCODE_IMIN,	0, ALU_OP2_MIN_INT, tgsi_op2},
	{TGSI_OPCODE_INEG,	0, ALU_OP2_SUB_INT, tgsi_ineg},
	{TGSI_OPCODE_ISGE,	0, ALU_OP2_SETGE_INT, tgsi_op2},
	{TGSI_OPCODE_ISHR,	0, ALU_OP2_ASHR_INT, tgsi_op2},
	{TGSI_OPCODE_ISLT,	0, ALU_OP2_SETGT_INT, tgsi_op2_swap},
	{TGSI_OPCODE_F2U,	0, ALU_OP1_FLT_TO_UINT, tgsi_f2i},
	{TGSI_OPCODE_U2F,	0, ALU_OP1_UINT_TO_FLT, tgsi_op2_trans},
	{TGSI_OPCODE_UADD,	0, ALU_OP2_ADD_INT, tgsi_op2},
	{TGSI_OPCODE_UDIV,	0, ALU_OP0_NOP, tgsi_udiv},
	{TGSI_OPCODE_UMAD,	0, ALU_OP0_NOP, tgsi_umad},
	{TGSI_OPCODE_UMAX,	0, ALU_OP2_MAX_UINT, tgsi_op2},
	{TGSI_OPCODE_UMIN,	0, ALU_OP2_MIN_UINT, tgsi_op2},
	{TGSI_OPCODE_UMOD,	0, ALU_OP0_NOP, tgsi_umod},
	{TGSI_OPCODE_UMUL,	0, ALU_OP2_MULLO_UINT, tgsi_op2_trans},
	{TGSI_OPCODE_USEQ,	0, ALU_OP2_SETE_INT, tgsi_op2},
	{TGSI_OPCODE_USGE,	0, ALU_OP2_SETGE_UINT, tgsi_op2},
	{TGSI_OPCODE_USHR,	0, ALU_OP2_LSHR_INT, tgsi_op2},
	{TGSI_OPCODE_USLT,	0, ALU_OP2_SETGT_UINT, tgsi_op2_swap},
	{TGSI_OPCODE_USNE,	0, ALU_OP2_SETNE_INT, tgsi_op2},
	{TGSI_OPCODE_SWITCH,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CASE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DEFAULT,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDSWITCH,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE,    0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_I,      0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_I_MS,   0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_B,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_C,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_C_LZ, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_D,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_L,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_GATHER4,   0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SVIEWINFO,	0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_POS, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_INFO, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_UARL,      0, ALU_OP1_MOVA_INT, tgsi_eg_arl},
	{TGSI_OPCODE_UCMP,	0, ALU_OP0_NOP, tgsi_ucmp},
	{TGSI_OPCODE_IABS,      0, 0, tgsi_iabs},
	{TGSI_OPCODE_ISSG,      0, 0, tgsi_issg},
	{TGSI_OPCODE_LOAD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_STORE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_MFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_LFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BARRIER,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUADD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMXCHG,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMCAS,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMAND,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMOR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMXOR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUMIN,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUMAX,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMIMIN,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMIMAX,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TEX2,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_TXB2,	0, FETCH_OP_SAMPLE_LB, tgsi_tex},
	{TGSI_OPCODE_TXL2,	0, FETCH_OP_SAMPLE_L, tgsi_tex},
	{TGSI_OPCODE_LAST,	0, ALU_OP0_NOP, tgsi_unsupported},
};

static struct r600_shader_tgsi_instruction cm_shader_tgsi_instruction[] = {
	{TGSI_OPCODE_ARL,	0, ALU_OP0_NOP, tgsi_eg_arl},
	{TGSI_OPCODE_MOV,	0, ALU_OP1_MOV, tgsi_op2},
	{TGSI_OPCODE_LIT,	0, ALU_OP0_NOP, tgsi_lit},
	{TGSI_OPCODE_RCP,	0, ALU_OP1_RECIP_IEEE, cayman_emit_float_instr},
	{TGSI_OPCODE_RSQ,	0, ALU_OP1_RECIPSQRT_IEEE, cayman_emit_float_instr},
	{TGSI_OPCODE_EXP,	0, ALU_OP0_NOP, tgsi_exp},
	{TGSI_OPCODE_LOG,	0, ALU_OP0_NOP, tgsi_log},
	{TGSI_OPCODE_MUL,	0, ALU_OP2_MUL, tgsi_op2},
	{TGSI_OPCODE_ADD,	0, ALU_OP2_ADD, tgsi_op2},
	{TGSI_OPCODE_DP3,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_DP4,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_DST,	0, ALU_OP0_NOP, tgsi_opdst},
	{TGSI_OPCODE_MIN,	0, ALU_OP2_MIN, tgsi_op2},
	{TGSI_OPCODE_MAX,	0, ALU_OP2_MAX, tgsi_op2},
	{TGSI_OPCODE_SLT,	0, ALU_OP2_SETGT, tgsi_op2_swap},
	{TGSI_OPCODE_SGE,	0, ALU_OP2_SETGE, tgsi_op2},
	{TGSI_OPCODE_MAD,	1, ALU_OP3_MULADD, tgsi_op3},
	{TGSI_OPCODE_SUB,	0, ALU_OP2_ADD, tgsi_op2},
	{TGSI_OPCODE_LRP,	0, ALU_OP0_NOP, tgsi_lrp},
	{TGSI_OPCODE_CND,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{20,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DP2A,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{22,			0, ALU_OP0_NOP, tgsi_unsupported},
	{23,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_FRC,	0, ALU_OP1_FRACT, tgsi_op2},
	{TGSI_OPCODE_CLAMP,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_FLR,	0, ALU_OP1_FLOOR, tgsi_op2},
	{TGSI_OPCODE_ROUND,	0, ALU_OP1_RNDNE, tgsi_op2},
	{TGSI_OPCODE_EX2,	0, ALU_OP1_EXP_IEEE, cayman_emit_float_instr},
	{TGSI_OPCODE_LG2,	0, ALU_OP1_LOG_IEEE, cayman_emit_float_instr},
	{TGSI_OPCODE_POW,	0, ALU_OP0_NOP, cayman_pow},
	{TGSI_OPCODE_XPD,	0, ALU_OP0_NOP, tgsi_xpd},
	/* gap */
	{32,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ABS,	0, ALU_OP1_MOV, tgsi_op2},
	{TGSI_OPCODE_RCC,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DPH,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_COS,	0, ALU_OP1_COS, cayman_trig},
	{TGSI_OPCODE_DDX,	0, FETCH_OP_GET_GRADIENTS_H, tgsi_tex},
	{TGSI_OPCODE_DDY,	0, FETCH_OP_GET_GRADIENTS_V, tgsi_tex},
	{TGSI_OPCODE_KILL,	0, ALU_OP2_KILLGT, tgsi_kill},  /* unconditional kill */
	{TGSI_OPCODE_PK2H,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK2US,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK4B,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PK4UB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_RFL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SEQ,	0, ALU_OP2_SETE, tgsi_op2},
	{TGSI_OPCODE_SFL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SGT,	0, ALU_OP2_SETGT, tgsi_op2},
	{TGSI_OPCODE_SIN,	0, ALU_OP1_SIN, cayman_trig},
	{TGSI_OPCODE_SLE,	0, ALU_OP2_SETGE, tgsi_op2_swap},
	{TGSI_OPCODE_SNE,	0, ALU_OP2_SETNE, tgsi_op2},
	{TGSI_OPCODE_STR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TEX,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_TXD,	0, FETCH_OP_SAMPLE_G, tgsi_tex},
	{TGSI_OPCODE_TXP,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_UP2H,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP2US,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP4B,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_UP4UB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_X2D,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ARA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ARR,	0, ALU_OP0_NOP, tgsi_eg_arl},
	{TGSI_OPCODE_BRA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CAL,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_RET,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SSG,	0, ALU_OP0_NOP, tgsi_ssg},
	{TGSI_OPCODE_CMP,	0, ALU_OP0_NOP, tgsi_cmp},
	{TGSI_OPCODE_SCS,	0, ALU_OP0_NOP, tgsi_scs},
	{TGSI_OPCODE_TXB,	0, FETCH_OP_SAMPLE_LB, tgsi_tex},
	{TGSI_OPCODE_NRM,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DIV,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DP2,	0, ALU_OP2_DOT4, tgsi_dp},
	{TGSI_OPCODE_TXL,	0, FETCH_OP_SAMPLE_L, tgsi_tex},
	{TGSI_OPCODE_BRK,	0, CF_OP_LOOP_BREAK, tgsi_loop_brk_cont},
	{TGSI_OPCODE_IF,	0, ALU_OP0_NOP, tgsi_if},
	{TGSI_OPCODE_UIF,	0, ALU_OP0_NOP, tgsi_uif},
	{76,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ELSE,	0, ALU_OP0_NOP, tgsi_else},
	{TGSI_OPCODE_ENDIF,	0, ALU_OP0_NOP, tgsi_endif},
	/* gap */
	{79,			0, ALU_OP0_NOP, tgsi_unsupported},
	{80,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_PUSHA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_POPA,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CEIL,	0, ALU_OP1_CEIL, tgsi_op2},
	{TGSI_OPCODE_I2F,	0, ALU_OP1_INT_TO_FLT, tgsi_op2},
	{TGSI_OPCODE_NOT,	0, ALU_OP1_NOT_INT, tgsi_op2},
	{TGSI_OPCODE_TRUNC,	0, ALU_OP1_TRUNC, tgsi_op2},
	{TGSI_OPCODE_SHL,	0, ALU_OP2_LSHL_INT, tgsi_op2},
	/* gap */
	{88,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_AND,	0, ALU_OP2_AND_INT, tgsi_op2},
	{TGSI_OPCODE_OR,	0, ALU_OP2_OR_INT, tgsi_op2},
	{TGSI_OPCODE_MOD,	0, ALU_OP0_NOP, tgsi_imod},
	{TGSI_OPCODE_XOR,	0, ALU_OP2_XOR_INT, tgsi_op2},
	{TGSI_OPCODE_SAD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TXF,	0, FETCH_OP_LD, tgsi_tex},
	{TGSI_OPCODE_TXQ,	0, FETCH_OP_GET_TEXTURE_RESINFO, tgsi_tex},
	{TGSI_OPCODE_CONT,	0, CF_OP_LOOP_CONTINUE, tgsi_loop_brk_cont},
	{TGSI_OPCODE_EMIT,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDPRIM,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BGNLOOP,	0, ALU_OP0_NOP, tgsi_bgnloop},
	{TGSI_OPCODE_BGNSUB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDLOOP,	0, ALU_OP0_NOP, tgsi_endloop},
	{TGSI_OPCODE_ENDSUB,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TXQ_LZ,	0, FETCH_OP_GET_TEXTURE_RESINFO, tgsi_tex},
	/* gap */
	{104,			0, ALU_OP0_NOP, tgsi_unsupported},
	{105,			0, ALU_OP0_NOP, tgsi_unsupported},
	{106,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_NOP,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{108,			0, ALU_OP0_NOP, tgsi_unsupported},
	{109,			0, ALU_OP0_NOP, tgsi_unsupported},
	{110,			0, ALU_OP0_NOP, tgsi_unsupported},
	{111,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_NRM4,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CALLNZ,	0, ALU_OP0_NOP, tgsi_unsupported},
	/* gap */
	{114,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BREAKC,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_KILL_IF,	0, ALU_OP2_KILLGT, tgsi_kill},  /* conditional kill */
	{TGSI_OPCODE_END,	0, ALU_OP0_NOP, tgsi_end},  /* aka HALT */
	/* gap */
	{118,			0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_F2I,	0, ALU_OP1_FLT_TO_INT, tgsi_op2},
	{TGSI_OPCODE_IDIV,	0, ALU_OP0_NOP, tgsi_idiv},
	{TGSI_OPCODE_IMAX,	0, ALU_OP2_MAX_INT, tgsi_op2},
	{TGSI_OPCODE_IMIN,	0, ALU_OP2_MIN_INT, tgsi_op2},
	{TGSI_OPCODE_INEG,	0, ALU_OP2_SUB_INT, tgsi_ineg},
	{TGSI_OPCODE_ISGE,	0, ALU_OP2_SETGE_INT, tgsi_op2},
	{TGSI_OPCODE_ISHR,	0, ALU_OP2_ASHR_INT, tgsi_op2},
	{TGSI_OPCODE_ISLT,	0, ALU_OP2_SETGT_INT, tgsi_op2_swap},
	{TGSI_OPCODE_F2U,	0, ALU_OP1_FLT_TO_UINT, tgsi_op2},
	{TGSI_OPCODE_U2F,	0, ALU_OP1_UINT_TO_FLT, tgsi_op2},
	{TGSI_OPCODE_UADD,	0, ALU_OP2_ADD_INT, tgsi_op2},
	{TGSI_OPCODE_UDIV,	0, ALU_OP0_NOP, tgsi_udiv},
	{TGSI_OPCODE_UMAD,	0, ALU_OP0_NOP, tgsi_umad},
	{TGSI_OPCODE_UMAX,	0, ALU_OP2_MAX_UINT, tgsi_op2},
	{TGSI_OPCODE_UMIN,	0, ALU_OP2_MIN_UINT, tgsi_op2},
	{TGSI_OPCODE_UMOD,	0, ALU_OP0_NOP, tgsi_umod},
	{TGSI_OPCODE_UMUL,	0, ALU_OP2_MULLO_INT, cayman_mul_int_instr},
	{TGSI_OPCODE_USEQ,	0, ALU_OP2_SETE_INT, tgsi_op2},
	{TGSI_OPCODE_USGE,	0, ALU_OP2_SETGE_UINT, tgsi_op2},
	{TGSI_OPCODE_USHR,	0, ALU_OP2_LSHR_INT, tgsi_op2},
	{TGSI_OPCODE_USLT,	0, ALU_OP2_SETGT_UINT, tgsi_op2_swap},
	{TGSI_OPCODE_USNE,	0, ALU_OP2_SETNE_INT, tgsi_op2},
	{TGSI_OPCODE_SWITCH,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_CASE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_DEFAULT,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ENDSWITCH,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE,    0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_I,      0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_I_MS,   0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_B,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_C,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_C_LZ, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_D,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_L,  0, 0, tgsi_unsupported},
	{TGSI_OPCODE_GATHER4,   0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SVIEWINFO,	0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_POS, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_SAMPLE_INFO, 0, 0, tgsi_unsupported},
	{TGSI_OPCODE_UARL,      0, ALU_OP1_MOVA_INT, tgsi_eg_arl},
	{TGSI_OPCODE_UCMP,	0, ALU_OP0_NOP, tgsi_ucmp},
	{TGSI_OPCODE_IABS,      0, 0, tgsi_iabs},
	{TGSI_OPCODE_ISSG,      0, 0, tgsi_issg},
	{TGSI_OPCODE_LOAD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_STORE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_MFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_LFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_SFENCE,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_BARRIER,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUADD,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMXCHG,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMCAS,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMAND,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMOR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMXOR,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUMIN,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMUMAX,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMIMIN,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_ATOMIMAX,	0, ALU_OP0_NOP, tgsi_unsupported},
	{TGSI_OPCODE_TEX2,	0, FETCH_OP_SAMPLE, tgsi_tex},
	{TGSI_OPCODE_TXB2,	0, FETCH_OP_SAMPLE_LB, tgsi_tex},
	{TGSI_OPCODE_TXL2,	0, FETCH_OP_SAMPLE_L, tgsi_tex},
	{TGSI_OPCODE_LAST,	0, ALU_OP0_NOP, tgsi_unsupported},
};
