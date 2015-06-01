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

#include <stdarg.h>

#include "pipe/p_state.h"
#include "util/u_string.h"
#include "util/u_memory.h"
#include "util/u_inlines.h"
#include "tgsi/tgsi_lowering.h"
#include "tgsi/tgsi_parse.h"
#include "tgsi/tgsi_ureg.h"
#include "tgsi/tgsi_info.h"
#include "tgsi/tgsi_strings.h"
#include "tgsi/tgsi_dump.h"
#include "tgsi/tgsi_scan.h"

#include "freedreno_util.h"

#include "ir3_compiler.h"
#include "ir3_shader.h"

#include "instr-a3xx.h"
#include "ir3.h"

struct ir3_compile_context {
	const struct tgsi_token *tokens;
	bool free_tokens;
	struct ir3 *ir;
	struct ir3_shader_variant *so;
	uint16_t integer_s;

	struct ir3_block *block;
	struct ir3_instruction *current_instr;

	/* we need to defer updates to block->outputs[] until the end
	 * of an instruction (so we don't see new value until *after*
	 * the src registers are processed)
	 */
	struct {
		struct ir3_instruction *instr, **instrp;
	} output_updates[64];
	unsigned num_output_updates;

	/* are we in a sequence of "atomic" instructions?
	 */
	bool atomic;

	/* For fragment shaders, from the hw perspective the only
	 * actual input is r0.xy position register passed to bary.f.
	 * But TGSI doesn't know that, it still declares things as
	 * IN[] registers.  So we do all the input tracking normally
	 * and fix things up after compile_instructions()
	 *
	 * NOTE that frag_pos is the hardware position (possibly it
	 * is actually an index or tag or some such.. it is *not*
	 * values that can be directly used for gl_FragCoord..)
	 */
	struct ir3_instruction *frag_pos, *frag_face, *frag_coord[4];

	/* For vertex shaders, keep track of the system values sources */
	struct ir3_instruction *vertex_id, *basevertex, *instance_id;

	struct tgsi_parse_context parser;
	unsigned type;

	struct tgsi_shader_info info;

	/* hmm, would be nice if tgsi_scan_shader figured this out
	 * for us:
	 */
	struct {
		unsigned first, last;
		struct ir3_instruction *fanin;
	} array[MAX_ARRAYS];
	uint32_t array_dirty;
	/* offset into array[], per file, of first array info */
	uint8_t array_offsets[TGSI_FILE_COUNT];

	/* for calculating input/output positions/linkages: */
	unsigned next_inloc;

	/* a4xx (at least patchlevel 0) cannot seem to flat-interpolate
	 * so we need to use ldlv.u32 to load the varying directly:
	 */
	bool flat_bypass;

	unsigned num_internal_temps;
	struct tgsi_src_register internal_temps[8];

	/* for looking up which system value is which */
	unsigned sysval_semantics[8];

	/* idx/slot for last compiler generated immediate */
	unsigned immediate_idx;

	/* stack of branch instructions that mark (potentially nested)
	 * branch if/else/loop/etc
	 */
	struct {
		struct ir3_instruction *instr, *cond;
		bool inv;   /* true iff in else leg of branch */
	} branch[16];
	unsigned int branch_count;

	/* list of kill instructions: */
	struct ir3_instruction *kill[16];
	unsigned int kill_count;

	/* used when dst is same as one of the src, to avoid overwriting a
	 * src element before the remaining scalar instructions that make
	 * up the vector operation
	 */
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;

	/* just for catching incorrect use of get_dst()/put_dst():
	 */
	bool using_tmp_dst;
};


static void vectorize(struct ir3_compile_context *ctx,
		struct ir3_instruction *instr, struct tgsi_dst_register *dst,
		int nsrcs, ...);
static void create_mov(struct ir3_compile_context *ctx,
		struct tgsi_dst_register *dst, struct tgsi_src_register *src);
static type_t get_ftype(struct ir3_compile_context *ctx);
static type_t get_utype(struct ir3_compile_context *ctx);

static unsigned setup_arrays(struct ir3_compile_context *ctx, unsigned file, unsigned i)
{
	/* ArrayID 0 for a given file is the legacy array spanning the entire file: */
	ctx->array[i].first = 0;
	ctx->array[i].last = ctx->info.file_max[file];
	ctx->array_offsets[file] = i;
	i += ctx->info.array_max[file] + 1;
	return i;
}

static unsigned
compile_init(struct ir3_compile_context *ctx, struct ir3_shader_variant *so,
		const struct tgsi_token *tokens)
{
	unsigned ret, i;
	struct tgsi_shader_info *info = &ctx->info;
	struct tgsi_lowering_config lconfig = {
			.color_two_side = so->key.color_two_side,
			.lower_DST  = true,
			.lower_XPD  = true,
			.lower_SCS  = true,
			.lower_LRP  = true,
			.lower_FRC  = true,
			.lower_POW  = true,
			.lower_LIT  = true,
			.lower_EXP  = true,
			.lower_LOG  = true,
			.lower_DP4  = true,
			.lower_DP3  = true,
			.lower_DPH  = true,
			.lower_DP2  = true,
			.lower_DP2A = true,
	};

	switch (so->type) {
	case SHADER_FRAGMENT:
	case SHADER_COMPUTE:
		lconfig.saturate_s = so->key.fsaturate_s;
		lconfig.saturate_t = so->key.fsaturate_t;
		lconfig.saturate_r = so->key.fsaturate_r;
		ctx->integer_s = so->key.finteger_s;
		break;
	case SHADER_VERTEX:
		lconfig.saturate_s = so->key.vsaturate_s;
		lconfig.saturate_t = so->key.vsaturate_t;
		lconfig.saturate_r = so->key.vsaturate_r;
		ctx->integer_s = so->key.vinteger_s;
		break;
	}

	if (!so->shader) {
		/* hack for standalone compiler which does not have
		 * screen/context:
		 */
	} else if (ir3_shader_gpuid(so->shader) >= 400) {
		/* a4xx seems to have *no* sam.p */
		lconfig.lower_TXP = ~0;  /* lower all txp */
		/* need special handling for "flat" */
		ctx->flat_bypass = true;
	} else {
		/* a3xx just needs to avoid sam.p for 3d tex */
		lconfig.lower_TXP = (1 << TGSI_TEXTURE_3D);
		/* no special handling for "flat" */
		ctx->flat_bypass = false;
	}

	ctx->tokens = tgsi_transform_lowering(&lconfig, tokens, &ctx->info);
	ctx->free_tokens = !!ctx->tokens;
	if (!ctx->tokens) {
		/* no lowering */
		ctx->tokens = tokens;
	}
	ctx->ir = so->ir;
	ctx->so = so;
	ctx->array_dirty = 0;
	ctx->next_inloc = 8;
	ctx->num_internal_temps = 0;
	ctx->branch_count = 0;
	ctx->kill_count = 0;
	ctx->block = NULL;
	ctx->current_instr = NULL;
	ctx->num_output_updates = 0;
	ctx->atomic = false;
	ctx->frag_pos = NULL;
	ctx->frag_face = NULL;
	ctx->vertex_id = NULL;
	ctx->instance_id = NULL;
	ctx->tmp_src = NULL;
	ctx->using_tmp_dst = false;

	memset(ctx->frag_coord, 0, sizeof(ctx->frag_coord));
	memset(ctx->array, 0, sizeof(ctx->array));
	memset(ctx->array_offsets, 0, sizeof(ctx->array_offsets));

#define FM(x) (1 << TGSI_FILE_##x)
	/* NOTE: if relative addressing is used, we set constlen in
	 * the compiler (to worst-case value) since we don't know in
	 * the assembler what the max addr reg value can be:
	 */
	if (info->indirect_files & FM(CONSTANT))
		so->constlen = MIN2(255, ctx->info.const_file_max[0] + 1);

	i = 0;
	i += setup_arrays(ctx, TGSI_FILE_INPUT, i);
	i += setup_arrays(ctx, TGSI_FILE_TEMPORARY, i);
	i += setup_arrays(ctx, TGSI_FILE_OUTPUT, i);
	/* any others? we don't track arrays for const..*/

	/* Immediates go after constants: */
	so->first_immediate = so->first_driver_param =
		info->const_file_max[0] + 1;
	/* 1 unit for the vertex id base */
	if (so->type == SHADER_VERTEX)
		so->first_immediate++;
	/* 4 (vec4) units for ubo base addresses */
	so->first_immediate += 4;
	ctx->immediate_idx = 4 * (ctx->info.file_max[TGSI_FILE_IMMEDIATE] + 1);

	ret = tgsi_parse_init(&ctx->parser, ctx->tokens);
	if (ret != TGSI_PARSE_OK)
		return ret;

	ctx->type = ctx->parser.FullHeader.Processor.Processor;

	return ret;
}

static void
compile_error(struct ir3_compile_context *ctx, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	_debug_vprintf(format, ap);
	va_end(ap);
	tgsi_dump(ctx->tokens, 0);
	debug_assert(0);
}

#define compile_assert(ctx, cond) do { \
		if (!(cond)) compile_error((ctx), "failed assert: "#cond"\n"); \
	} while (0)

static void
compile_free(struct ir3_compile_context *ctx)
{
	if (ctx->free_tokens)
		free((void *)ctx->tokens);
	tgsi_parse_free(&ctx->parser);
}

struct instr_translater {
	void (*fxn)(const struct instr_translater *t,
			struct ir3_compile_context *ctx,
			struct tgsi_full_instruction *inst);
	unsigned tgsi_opc;
	opc_t opc;
	opc_t hopc;    /* opc to use for half_precision mode, if different */
	unsigned arg;
};

static void
instr_finish(struct ir3_compile_context *ctx)
{
	unsigned i;

	if (ctx->atomic)
		return;

	for (i = 0; i < ctx->num_output_updates; i++)
		*(ctx->output_updates[i].instrp) = ctx->output_updates[i].instr;

	ctx->num_output_updates = 0;

	while (ctx->array_dirty) {
		unsigned aid = ffs(ctx->array_dirty) - 1;
		ctx->array[aid].fanin = NULL;
		ctx->array_dirty &= ~(1 << aid);
	}
}

/* For "atomic" groups of instructions, for example the four scalar
 * instructions to perform a vec4 operation.  Basically this just
 * blocks out handling of output_updates so the next scalar instruction
 * still sees the result from before the start of the atomic group.
 *
 * NOTE: when used properly, this could probably replace get/put_dst()
 * stuff.
 */
static void
instr_atomic_start(struct ir3_compile_context *ctx)
{
	ctx->atomic = true;
}

static void
instr_atomic_end(struct ir3_compile_context *ctx)
{
	ctx->atomic = false;
	instr_finish(ctx);
}

static struct ir3_instruction *
instr_create(struct ir3_compile_context *ctx, int category, opc_t opc)
{
	instr_finish(ctx);
	return (ctx->current_instr = ir3_instr_create(ctx->block, category, opc));
}

static struct ir3_block *
push_block(struct ir3_compile_context *ctx)
{
	struct ir3_block *block;
	unsigned ntmp, nin, nout;

#define SCALAR_REGS(file) (4 * (ctx->info.file_max[TGSI_FILE_ ## file] + 1))

	/* hmm, give ourselves room to create 8 extra temporaries (vec4):
	 */
	ntmp = SCALAR_REGS(TEMPORARY);
	ntmp += 8 * 4;

	nout = SCALAR_REGS(OUTPUT);
	nin  = SCALAR_REGS(INPUT) + SCALAR_REGS(SYSTEM_VALUE);

	/* for outermost block, 'inputs' are the actual shader INPUT
	 * register file.  Reads from INPUT registers always go back to
	 * top block.  For nested blocks, 'inputs' is used to track any
	 * TEMPORARY file register from one of the enclosing blocks that
	 * is ready in this block.
	 */
	if (!ctx->block) {
		/* NOTE: fragment shaders actually have two inputs (r0.xy, the
		 * position)
		 */
		if (ctx->type == TGSI_PROCESSOR_FRAGMENT) {
			int n = 2;
			if (ctx->info.reads_position)
				n += 4;
			if (ctx->info.uses_frontface)
				n += 4;
			nin = MAX2(n, nin);
			nout += ARRAY_SIZE(ctx->kill);
		}
	} else {
		nin = ntmp;
	}

	block = ir3_block_create(ctx->ir, ntmp, nin, nout);

	if ((ctx->type == TGSI_PROCESSOR_FRAGMENT) && !ctx->block)
		block->noutputs -= ARRAY_SIZE(ctx->kill);

	block->parent = ctx->block;
	ctx->block = block;

	return block;
}

static void
pop_block(struct ir3_compile_context *ctx)
{
	ctx->block = ctx->block->parent;
	compile_assert(ctx, ctx->block);
}

static struct ir3_instruction *
create_output(struct ir3_block *block, struct ir3_instruction *instr,
		unsigned n)
{
	struct ir3_instruction *out;

	out = ir3_instr_create(block, -1, OPC_META_OUTPUT);
	out->inout.block = block;
	ir3_reg_create(out, n, 0);
	if (instr)
		ir3_reg_create(out, 0, IR3_REG_SSA)->instr = instr;

	return out;
}

static struct ir3_instruction *
create_input(struct ir3_block *block, struct ir3_instruction *instr,
		unsigned n)
{
	struct ir3_instruction *in;

	in = ir3_instr_create(block, -1, OPC_META_INPUT);
	in->inout.block = block;
	ir3_reg_create(in, n, 0);
	if (instr)
		ir3_reg_create(in, 0, IR3_REG_SSA)->instr = instr;

	return in;
}

static struct ir3_instruction *
block_input(struct ir3_block *block, unsigned n)
{
	/* references to INPUT register file always go back up to
	 * top level:
	 */
	if (block->parent)
		return block_input(block->parent, n);
	return block->inputs[n];
}

/* return temporary in scope, creating if needed meta-input node
 * to track block inputs
 */
static struct ir3_instruction *
block_temporary(struct ir3_block *block, unsigned n)
{
	/* references to TEMPORARY register file, find the nearest
	 * enclosing block which has already assigned this temporary,
	 * creating meta-input instructions along the way to keep
	 * track of block inputs
	 */
	if (block->parent && !block->temporaries[n]) {
		/* if already have input for this block, reuse: */
		if (!block->inputs[n])
			block->inputs[n] = block_temporary(block->parent, n);

		/* and create new input to return: */
		return create_input(block, block->inputs[n], n);
	}
	return block->temporaries[n];
}

static struct ir3_instruction *
create_immed(struct ir3_compile_context *ctx, float val)
{
	/* NOTE: *don't* use instr_create() here!
	 */
	struct ir3_instruction *instr;
	instr = ir3_instr_create(ctx->block, 1, 0);
	instr->cat1.src_type = get_ftype(ctx);
	instr->cat1.dst_type = get_ftype(ctx);
	ir3_reg_create(instr, 0, 0);
	ir3_reg_create(instr, 0, IR3_REG_IMMED)->fim_val = val;
	return instr;
}

static void
ssa_instr_set(struct ir3_compile_context *ctx, unsigned file, unsigned n,
		struct ir3_instruction *instr)
{
	struct ir3_block *block = ctx->block;
	unsigned idx = ctx->num_output_updates;

	compile_assert(ctx, idx < ARRAY_SIZE(ctx->output_updates));

	/* NOTE: defer update of temporaries[idx] or output[idx]
	 * until instr_finish(), so that if the current instruction
	 * reads the same TEMP/OUT[] it gets the old value:
	 *
	 * bleh.. this might be a bit easier to just figure out
	 * in instr_finish().  But at that point we've already
	 * lost information about OUTPUT vs TEMPORARY register
	 * file..
	 */

	switch (file) {
	case TGSI_FILE_OUTPUT:
		compile_assert(ctx, n < block->noutputs);
		ctx->output_updates[idx].instrp = &block->outputs[n];
		ctx->output_updates[idx].instr = instr;
		ctx->num_output_updates++;
		break;
	case TGSI_FILE_TEMPORARY:
		compile_assert(ctx, n < block->ntemporaries);
		ctx->output_updates[idx].instrp = &block->temporaries[n];
		ctx->output_updates[idx].instr = instr;
		ctx->num_output_updates++;
		break;
	case TGSI_FILE_ADDRESS:
		compile_assert(ctx, n < 1);
		ctx->output_updates[idx].instrp = &block->address;
		ctx->output_updates[idx].instr = instr;
		ctx->num_output_updates++;
		break;
	}
}

static struct ir3_instruction *
ssa_instr_get(struct ir3_compile_context *ctx, unsigned file, unsigned n)
{
	struct ir3_block *block = ctx->block;
	struct ir3_instruction *instr = NULL;

	switch (file) {
	case TGSI_FILE_INPUT:
		instr = block_input(ctx->block, n);
		break;
	case TGSI_FILE_OUTPUT:
		/* really this should just happen in case of 'MOV_SAT OUT[n], ..',
		 * for the following clamp instructions:
		 */
		instr = block->outputs[n];
		/* we don't have to worry about read from an OUTPUT that was
		 * assigned outside of the current block, because the _SAT
		 * clamp instructions will always be in the same block as
		 * the original instruction which wrote the OUTPUT
		 */
		compile_assert(ctx, instr);
		break;
	case TGSI_FILE_TEMPORARY:
		instr = block_temporary(ctx->block, n);
		if (!instr) {
			/* this can happen when registers (or components of a TGSI
			 * register) are used as src before they have been assigned
			 * (undefined contents).  To avoid confusing the rest of the
			 * compiler, and to generally keep things peachy, substitute
			 * an instruction that sets the src to 0.0.  Or to keep
			 * things undefined, I could plug in a random number? :-P
			 *
			 * NOTE: *don't* use instr_create() here!
			 */
			instr = create_immed(ctx, 0.0);
			/* no need to recreate the immed for every access: */
			block->temporaries[n] = instr;
		}
		break;
	case TGSI_FILE_SYSTEM_VALUE:
		switch (ctx->sysval_semantics[n >> 2]) {
		case TGSI_SEMANTIC_VERTEXID_NOBASE:
			instr = ctx->vertex_id;
			break;
		case TGSI_SEMANTIC_BASEVERTEX:
			instr = ctx->basevertex;
			break;
		case TGSI_SEMANTIC_INSTANCEID:
			instr = ctx->instance_id;
			break;
		}
		break;
	}

	return instr;
}

static int dst_array_id(struct ir3_compile_context *ctx,
		const struct tgsi_dst_register *dst)
{
	// XXX complete hack to recover tgsi_full_dst_register...
	// nothing that isn't wrapped in a tgsi_full_dst_register
	// should be indirect
	const struct tgsi_full_dst_register *fdst = (const void *)dst;
	return fdst->Indirect.ArrayID + ctx->array_offsets[dst->File];
}

static int src_array_id(struct ir3_compile_context *ctx,
		const struct tgsi_src_register *src)
{
	// XXX complete hack to recover tgsi_full_src_register...
	// nothing that isn't wrapped in a tgsi_full_src_register
	// should be indirect
	const struct tgsi_full_src_register *fsrc = (const void *)src;
	debug_assert(src->File != TGSI_FILE_CONSTANT);
	return fsrc->Indirect.ArrayID + ctx->array_offsets[src->File];
}

static struct ir3_instruction *
array_fanin(struct ir3_compile_context *ctx, unsigned aid, unsigned file)
{
	struct ir3_instruction *instr;

	if (ctx->array[aid].fanin) {
		instr = ctx->array[aid].fanin;
	} else {
		unsigned first = ctx->array[aid].first;
		unsigned last  = ctx->array[aid].last;
		unsigned i, j;

		instr = ir3_instr_create2(ctx->block, -1, OPC_META_FI,
				1 + (4 * (last + 1 - first)));
		ir3_reg_create(instr, 0, 0);
		for (i = first; i <= last; i++) {
			for (j = 0; j < 4; j++) {
				unsigned n = regid(i, j);
				ir3_reg_create(instr, 0, IR3_REG_SSA)->instr =
						ssa_instr_get(ctx, file, n);
			}
		}
		ctx->array[aid].fanin = instr;
		ctx->array_dirty |= (1 << aid);
	}

	return instr;
}

static void
ssa_dst(struct ir3_compile_context *ctx, struct ir3_instruction *instr,
		const struct tgsi_dst_register *dst, unsigned chan)
{
	if (dst->Indirect) {
		struct ir3_register *reg = instr->regs[0];
		unsigned i, aid = dst_array_id(ctx, dst);
		unsigned first = ctx->array[aid].first;
		unsigned last  = ctx->array[aid].last;
		unsigned off   = dst->Index - first; /* vec4 offset */

		reg->size = 4 * (1 + last - first);
		reg->offset = regid(off, chan);

		instr->fanin = array_fanin(ctx, aid, dst->File);

		/* annotate with the array-id, to help out the register-
		 * assignment stage.  At least for the case of indirect
		 * writes, we should capture enough dependencies to
		 * preserve the order of reads/writes of the array, so
		 * the multiple "names" for the array should end up all
		 * assigned to the same registers.
		 */
		instr->fanin->fi.aid = aid;

		/* Since we are scalarizing vec4 tgsi instructions/regs, we
		 * run into a slight complication here.  To do the naive thing
		 * and setup a fanout for each scalar array element would end
		 * up with the result that the instructions generated for each
		 * component of the vec4 would end up clobbering each other.
		 * So we take advantage here of knowing that the array index
		 * (after the shl.b) will be a multiple of four, and only set
		 * every fourth scalar component in the array.  See also
		 * fixup_ssa_dst_array()
		 */
		for (i = first; i <= last; i++) {
			struct ir3_instruction *split;
			unsigned n = regid(i, chan);
			int off = (4 * (i - first)) + chan;

			if (is_meta(instr) && (instr->opc == OPC_META_FO))
				off -= instr->fo.off;

			split = ir3_instr_create(ctx->block, -1, OPC_META_FO);
			split->fo.off = off;
			ir3_reg_create(split, 0, 0);
			ir3_reg_create(split, 0, IR3_REG_SSA)->instr = instr;

			ssa_instr_set(ctx, dst->File, n, split);
		}
	} else {
		/* normal case (not relative addressed GPR) */
		ssa_instr_set(ctx, dst->File, regid(dst->Index, chan), instr);
	}
}

static void
ssa_src(struct ir3_compile_context *ctx, struct ir3_register *reg,
		const struct tgsi_src_register *src, unsigned chan)
{
	struct ir3_instruction *instr;

	if (src->Indirect && (src->File != TGSI_FILE_CONSTANT)) {
		/* for relative addressing of gpr's (due to register assignment)
		 * we must generate a fanin instruction to collect all possible
		 * array elements that the instruction could address together:
		 */
		unsigned aid   = src_array_id(ctx, src);
		unsigned first = ctx->array[aid].first;
		unsigned last  = ctx->array[aid].last;
		unsigned off   = src->Index - first; /* vec4 offset */

		reg->size = 4 * (1 + last - first);
		reg->offset = regid(off, chan);

		instr = array_fanin(ctx, aid, src->File);
	} else if (src->File == TGSI_FILE_CONSTANT && src->Dimension) {
		const struct tgsi_full_src_register *fsrc = (const void *)src;
		struct ir3_instruction *temp = NULL;
		int ubo_regid = regid(ctx->so->first_driver_param, 0) +
			fsrc->Dimension.Index - 1;
		int offset = 0;

		/* We don't handle indirect UBO array accesses... yet. */
		compile_assert(ctx, !fsrc->Dimension.Indirect);
		/* UBOs start at index 1. */
		compile_assert(ctx, fsrc->Dimension.Index > 0);

		if (src->Indirect) {
			/* In case of an indirect index, it will have been loaded into an
			 * address register. There will be a sequence of
			 *
			 *   shl.b x, val, 2
			 *   mova a0, x
			 *
			 * We rely on this sequence to get the original val out and shift
			 * it by 4, since we're dealing in vec4 units.
			 */
			compile_assert(ctx, ctx->block->address);
			compile_assert(ctx, ctx->block->address->regs[1]->instr->opc ==
						   OPC_SHL_B);

			temp = instr = instr_create(ctx, 2, OPC_SHL_B);
			ir3_reg_create(instr, 0, 0);
			ir3_reg_create(instr, 0, IR3_REG_HALF | IR3_REG_SSA)->instr =
				ctx->block->address->regs[1]->instr->regs[1]->instr;
			ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 4;
		} else if (src->Index >= 64) {
			/* Otherwise it's a plain index (in vec4 units). Move it into a
			 * register.
			 */
			temp = instr = instr_create(ctx, 1, 0);
			instr->cat1.src_type = get_utype(ctx);
			instr->cat1.dst_type = get_utype(ctx);
			ir3_reg_create(instr, 0, 0);
			ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = src->Index * 16;
		} else {
			/* The offset is small enough to fit into the ldg instruction
			 * directly.
			 */
			offset = src->Index * 16;
		}

		if (temp) {
			/* If there was an offset (most common), add it to the buffer
			 * address.
			 */
			instr = instr_create(ctx, 2, OPC_ADD_S);
			ir3_reg_create(instr, 0, 0);
			ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = temp;
			ir3_reg_create(instr, ubo_regid, IR3_REG_CONST);
		} else {
			/* Otherwise just load the buffer address directly */
			instr = instr_create(ctx, 1, 0);
			instr->cat1.src_type = get_utype(ctx);
			instr->cat1.dst_type = get_utype(ctx);
			ir3_reg_create(instr, 0, 0);
			ir3_reg_create(instr, ubo_regid, IR3_REG_CONST);
		}

		temp = instr;

		instr = instr_create(ctx, 6, OPC_LDG);
		instr->cat6.type = TYPE_U32;
		instr->cat6.offset = offset + chan * 4;
		ir3_reg_create(instr, 0, 0);
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = temp;
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 1;

		reg->flags &= ~(IR3_REG_RELATIV | IR3_REG_CONST);
	} else {
		/* normal case (not relative addressed GPR) */
		instr = ssa_instr_get(ctx, src->File, regid(src->Index, chan));
	}

	if (instr) {
		reg->flags |= IR3_REG_SSA;
		reg->instr = instr;
	} else if (reg->flags & IR3_REG_SSA) {
		/* special hack for trans_samp() which calls ssa_src() directly
		 * to build up the collect (fanin) for const src.. (so SSA flag
		 * set but no src instr... it basically gets lucky because we
		 * default to 0.0 for "undefined" src instructions, which is
		 * what it wants.  We probably need to give it a better way to
		 * do this, but for now this hack:
		 */
		reg->instr = create_immed(ctx, 0.0);
	}
}

static struct ir3_register *
add_dst_reg_wrmask(struct ir3_compile_context *ctx,
		struct ir3_instruction *instr, const struct tgsi_dst_register *dst,
		unsigned chan, unsigned wrmask)
{
	unsigned flags = 0, num = 0;
	struct ir3_register *reg;

	switch (dst->File) {
	case TGSI_FILE_OUTPUT:
	case TGSI_FILE_TEMPORARY:
		/* uses SSA */
		break;
	case TGSI_FILE_ADDRESS:
		flags |= IR3_REG_ADDR;
		/* uses SSA */
		break;
	default:
		compile_error(ctx, "unsupported dst register file: %s\n",
			tgsi_file_name(dst->File));
		break;
	}

	if (dst->Indirect) {
		flags |= IR3_REG_RELATIV;

		/* shouldn't happen, and we can't cope with it below: */
		compile_assert(ctx, wrmask == 0x1);

		compile_assert(ctx, ctx->block->address);
		if (instr->address)
			compile_assert(ctx, ctx->block->address == instr->address);

		instr->address = ctx->block->address;
		array_insert(ctx->ir->indirects, instr);
	}

	reg = ir3_reg_create(instr, regid(num, chan), flags);
	reg->wrmask = wrmask;

	if (wrmask == 0x1) {
		/* normal case */
		ssa_dst(ctx, instr, dst, chan);
	} else if ((dst->File == TGSI_FILE_TEMPORARY) ||
			(dst->File == TGSI_FILE_OUTPUT) ||
			(dst->File == TGSI_FILE_ADDRESS)) {
		struct ir3_instruction *prev = NULL;
		unsigned i;

		compile_assert(ctx, !dst->Indirect);

		/* if instruction writes multiple, we need to create
		 * some place-holder collect the registers:
		 */
		for (i = 0; i < 4; i++) {
			/* NOTE: slightly ugly that we setup neighbor ptrs
			 * for FO here, but handle FI in CP pass.. we should
			 * probably just always setup neighbor ptrs in the
			 * frontend?
			 */
			struct ir3_instruction *split =
					ir3_instr_create(ctx->block, -1, OPC_META_FO);
			split->fo.off = i;
			/* unused dst reg: */
			/* NOTE: set SSA flag on dst here, because unused FO's
			 * which don't get scheduled will end up not in the
			 * instruction list when RA sets SSA flag on each dst.
			 * Slight hack.  We really should set SSA flag on
			 * every dst register in the frontend.
			 */
			ir3_reg_create(split, 0, IR3_REG_SSA);
			/* and src reg used to hold original instr */
			ir3_reg_create(split, 0, IR3_REG_SSA)->instr = instr;
			if (prev) {
				split->cp.left = prev;
				split->cp.left_cnt++;
				prev->cp.right = split;
				prev->cp.right_cnt++;
			}
			if ((wrmask & (1 << i)) && !ctx->atomic)
				ssa_dst(ctx, split, dst, chan+i);
			prev = split;
		}
	}

	return reg;
}

static struct ir3_register *
add_dst_reg(struct ir3_compile_context *ctx, struct ir3_instruction *instr,
		const struct tgsi_dst_register *dst, unsigned chan)
{
	return add_dst_reg_wrmask(ctx, instr, dst, chan, 0x1);
}

static struct ir3_register *
add_src_reg_wrmask(struct ir3_compile_context *ctx,
		struct ir3_instruction *instr, const struct tgsi_src_register *src,
		unsigned chan, unsigned wrmask)
{
	unsigned flags = 0, num = 0;
	struct ir3_register *reg;

	switch (src->File) {
	case TGSI_FILE_IMMEDIATE:
		/* TODO if possible, use actual immediate instead of const.. but
		 * TGSI has vec4 immediates, we can only embed scalar (of limited
		 * size, depending on instruction..)
		 */
		flags |= IR3_REG_CONST;
		num = src->Index + ctx->so->first_immediate;
		break;
	case TGSI_FILE_CONSTANT:
		flags |= IR3_REG_CONST;
		num = src->Index;
		break;
	case TGSI_FILE_OUTPUT:
		/* NOTE: we should only end up w/ OUTPUT file for things like
		 * clamp()'ing saturated dst instructions
		 */
	case TGSI_FILE_INPUT:
	case TGSI_FILE_TEMPORARY:
	case TGSI_FILE_SYSTEM_VALUE:
		/* uses SSA */
		break;
	default:
		compile_error(ctx, "unsupported src register file: %s\n",
			tgsi_file_name(src->File));
		break;
	}

	/* We seem to have 8 bits (6.2) for dst register always, so I think
	 * it is safe to assume GPR cannot be >=64
	 *
	 * cat3 instructions only have 8 bits for src2, but cannot take a
	 * const for src2
	 *
	 * cat5 and cat6 in some cases only has 8 bits, but cannot take a
	 * const for any src.
	 *
	 * Other than that we seem to have 12 bits to encode const src,
	 * except for cat1 which may only have 11 bits (but that seems like
	 * a bug)
	 */
	if (flags & IR3_REG_CONST)
		compile_assert(ctx, src->Index < (1 << 9));
	else
		compile_assert(ctx, src->Index < (1 << 6));

	/* NOTE: abs/neg modifiers in tgsi only apply to float */
	if (src->Absolute)
		flags |= IR3_REG_FABS;
	if (src->Negate)
		flags |= IR3_REG_FNEG;

	if (src->Indirect) {
		flags |= IR3_REG_RELATIV;

		/* shouldn't happen, and we can't cope with it below: */
		compile_assert(ctx, wrmask == 0x1);

		compile_assert(ctx, ctx->block->address);
		if (instr->address)
			compile_assert(ctx, ctx->block->address == instr->address);

		instr->address = ctx->block->address;
		array_insert(ctx->ir->indirects, instr);
	}

	reg = ir3_reg_create(instr, regid(num, chan), flags);
	reg->wrmask = wrmask;

	if (wrmask == 0x1) {
		/* normal case */
		ssa_src(ctx, reg, src, chan);
	} else if ((src->File == TGSI_FILE_TEMPORARY) ||
			(src->File == TGSI_FILE_OUTPUT) ||
			(src->File == TGSI_FILE_INPUT)) {
		struct ir3_instruction *collect;
		unsigned i;

		compile_assert(ctx, !src->Indirect);

		/* if instruction reads multiple, we need to create
		 * some place-holder collect the registers:
		 */
		collect = ir3_instr_create(ctx->block, -1, OPC_META_FI);
		ir3_reg_create(collect, 0, 0);   /* unused dst reg */

		for (i = 0; i < 4; i++) {
			if (wrmask & (1 << i)) {
				/* and src reg used point to the original instr */
				ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
						src, chan + i);
			} else if (wrmask & ~((i << i) - 1)) {
				/* if any remaining components, then dummy
				 * placeholder src reg to fill in the blanks:
				 */
				ir3_reg_create(collect, 0, 0);
			}
		}

		reg->flags |= IR3_REG_SSA;
		reg->instr = collect;
	}

	return reg;
}

static struct ir3_register *
add_src_reg(struct ir3_compile_context *ctx, struct ir3_instruction *instr,
		const struct tgsi_src_register *src, unsigned chan)
{
	return add_src_reg_wrmask(ctx, instr, src, chan, 0x1);
}

static void
src_from_dst(struct tgsi_src_register *src, struct tgsi_dst_register *dst)
{
	src->File      = dst->File;
	src->Indirect  = dst->Indirect;
	src->Dimension = dst->Dimension;
	src->Index     = dst->Index;
	src->Absolute  = 0;
	src->Negate    = 0;
	src->SwizzleX  = TGSI_SWIZZLE_X;
	src->SwizzleY  = TGSI_SWIZZLE_Y;
	src->SwizzleZ  = TGSI_SWIZZLE_Z;
	src->SwizzleW  = TGSI_SWIZZLE_W;
}

/* Get internal-temp src/dst to use for a sequence of instructions
 * generated by a single TGSI op.
 */
static struct tgsi_src_register *
get_internal_temp(struct ir3_compile_context *ctx,
		struct tgsi_dst_register *tmp_dst)
{
	struct tgsi_src_register *tmp_src;
	int n;

	tmp_dst->File      = TGSI_FILE_TEMPORARY;
	tmp_dst->WriteMask = TGSI_WRITEMASK_XYZW;
	tmp_dst->Indirect  = 0;
	tmp_dst->Dimension = 0;

	/* assign next temporary: */
	n = ctx->num_internal_temps++;
	compile_assert(ctx, n < ARRAY_SIZE(ctx->internal_temps));
	tmp_src = &ctx->internal_temps[n];

	tmp_dst->Index = ctx->info.file_max[TGSI_FILE_TEMPORARY] + n + 1;

	src_from_dst(tmp_src, tmp_dst);

	return tmp_src;
}

static inline bool
is_const(struct tgsi_src_register *src)
{
	return (src->File == TGSI_FILE_CONSTANT) ||
			(src->File == TGSI_FILE_IMMEDIATE);
}

static inline bool
is_relative(struct tgsi_src_register *src)
{
	return src->Indirect;
}

static inline bool
is_rel_or_const(struct tgsi_src_register *src)
{
	return is_relative(src) || is_const(src);
}

static type_t
get_ftype(struct ir3_compile_context *ctx)
{
	return TYPE_F32;
}

static type_t
get_utype(struct ir3_compile_context *ctx)
{
	return TYPE_U32;
}

static type_t
get_stype(struct ir3_compile_context *ctx)
{
	return TYPE_S32;
}

static unsigned
src_swiz(struct tgsi_src_register *src, int chan)
{
	switch (chan) {
	case 0: return src->SwizzleX;
	case 1: return src->SwizzleY;
	case 2: return src->SwizzleZ;
	case 3: return src->SwizzleW;
	}
	assert(0);
	return 0;
}

/* for instructions that cannot take a const register as src, if needed
 * generate a move to temporary gpr:
 */
static struct tgsi_src_register *
get_unconst(struct ir3_compile_context *ctx, struct tgsi_src_register *src)
{
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;

	compile_assert(ctx, is_rel_or_const(src));

	tmp_src = get_internal_temp(ctx, &tmp_dst);

	create_mov(ctx, &tmp_dst, src);

	return tmp_src;
}

static void
get_immediate(struct ir3_compile_context *ctx,
		struct tgsi_src_register *reg, uint32_t val)
{
	unsigned neg, swiz, idx, i;
	/* actually maps 1:1 currently.. not sure if that is safe to rely on: */
	static const unsigned swiz2tgsi[] = {
			TGSI_SWIZZLE_X, TGSI_SWIZZLE_Y, TGSI_SWIZZLE_Z, TGSI_SWIZZLE_W,
	};

	for (i = 0; i < ctx->immediate_idx; i++) {
		swiz = i % 4;
		idx  = i / 4;

		if (ctx->so->immediates[idx].val[swiz] == val) {
			neg = 0;
			break;
		}

		if (ctx->so->immediates[idx].val[swiz] == -val) {
			neg = 1;
			break;
		}
	}

	if (i == ctx->immediate_idx) {
		/* need to generate a new immediate: */
		swiz = i % 4;
		idx  = i / 4;
		neg  = 0;
		ctx->so->immediates[idx].val[swiz] = val;
		ctx->so->immediates_count = idx + 1;
		ctx->immediate_idx++;
	}

	reg->File      = TGSI_FILE_IMMEDIATE;
	reg->Indirect  = 0;
	reg->Dimension = 0;
	reg->Index     = idx;
	reg->Absolute  = 0;
	reg->Negate    = neg;
	reg->SwizzleX  = swiz2tgsi[swiz];
	reg->SwizzleY  = swiz2tgsi[swiz];
	reg->SwizzleZ  = swiz2tgsi[swiz];
	reg->SwizzleW  = swiz2tgsi[swiz];
}

static void
create_mov(struct ir3_compile_context *ctx, struct tgsi_dst_register *dst,
		struct tgsi_src_register *src)
{
	type_t type_mov = get_ftype(ctx);
	unsigned i;

	for (i = 0; i < 4; i++) {
		/* move to destination: */
		if (dst->WriteMask & (1 << i)) {
			struct ir3_instruction *instr;

			if (src->Absolute || src->Negate) {
				/* can't have abs or neg on a mov instr, so use
				 * absneg.f instead to handle these cases:
				 */
				instr = instr_create(ctx, 2, OPC_ABSNEG_F);
			} else {
				instr = instr_create(ctx, 1, 0);
				instr->cat1.src_type = type_mov;
				instr->cat1.dst_type = type_mov;
			}

			add_dst_reg(ctx, instr, dst, i);
			add_src_reg(ctx, instr, src, src_swiz(src, i));
		}
	}
}

static void
create_clamp(struct ir3_compile_context *ctx,
		struct tgsi_dst_register *dst, struct tgsi_src_register *val,
		struct tgsi_src_register *minval, struct tgsi_src_register *maxval)
{
	struct ir3_instruction *instr;

	instr = instr_create(ctx, 2, OPC_MAX_F);
	vectorize(ctx, instr, dst, 2, val, 0, minval, 0);

	instr = instr_create(ctx, 2, OPC_MIN_F);
	vectorize(ctx, instr, dst, 2, val, 0, maxval, 0);
}

static void
create_clamp_imm(struct ir3_compile_context *ctx,
		struct tgsi_dst_register *dst,
		uint32_t minval, uint32_t maxval)
{
	struct tgsi_src_register minconst, maxconst;
	struct tgsi_src_register src;

	src_from_dst(&src, dst);

	get_immediate(ctx, &minconst, minval);
	get_immediate(ctx, &maxconst, maxval);

	create_clamp(ctx, dst, &src, &minconst, &maxconst);
}

static struct tgsi_dst_register *
get_dst(struct ir3_compile_context *ctx, struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	unsigned i;

	compile_assert(ctx, !ctx->using_tmp_dst);
	ctx->using_tmp_dst = true;

	for (i = 0; i < inst->Instruction.NumSrcRegs; i++) {
		struct tgsi_src_register *src = &inst->Src[i].Register;
		if ((src->File == dst->File) && (src->Index == dst->Index)) {
			if ((dst->WriteMask == TGSI_WRITEMASK_XYZW) &&
					(src->SwizzleX == TGSI_SWIZZLE_X) &&
					(src->SwizzleY == TGSI_SWIZZLE_Y) &&
					(src->SwizzleZ == TGSI_SWIZZLE_Z) &&
					(src->SwizzleW == TGSI_SWIZZLE_W))
				continue;
			ctx->tmp_src = get_internal_temp(ctx, &ctx->tmp_dst);
			ctx->tmp_dst.WriteMask = dst->WriteMask;
			dst = &ctx->tmp_dst;
			break;
		}
	}
	return dst;
}

static void
put_dst(struct ir3_compile_context *ctx, struct tgsi_full_instruction *inst,
		struct tgsi_dst_register *dst)
{
	compile_assert(ctx, ctx->using_tmp_dst);
	ctx->using_tmp_dst = false;

	/* if necessary, add mov back into original dst: */
	if (dst != &inst->Dst[0].Register) {
		create_mov(ctx, &inst->Dst[0].Register, ctx->tmp_src);
	}
}

/* helper to generate the necessary repeat and/or additional instructions
 * to turn a scalar instruction into a vector operation:
 */
static void
vectorize(struct ir3_compile_context *ctx, struct ir3_instruction *instr,
		struct tgsi_dst_register *dst, int nsrcs, ...)
{
	va_list ap;
	int i, j, n = 0;

	instr_atomic_start(ctx);

	for (i = 0; i < 4; i++) {
		if (dst->WriteMask & (1 << i)) {
			struct ir3_instruction *cur;

			if (n++ == 0) {
				cur = instr;
			} else {
				cur = instr_create(ctx, instr->category, instr->opc);
				memcpy(cur->info, instr->info, sizeof(cur->info));
			}

			add_dst_reg(ctx, cur, dst, i);

			va_start(ap, nsrcs);
			for (j = 0; j < nsrcs; j++) {
				struct tgsi_src_register *src =
						va_arg(ap, struct tgsi_src_register *);
				unsigned flags = va_arg(ap, unsigned);
				struct ir3_register *reg;
				if (flags & IR3_REG_IMMED) {
					reg = ir3_reg_create(cur, 0, IR3_REG_IMMED);
					/* this is an ugly cast.. should have put flags first! */
					reg->iim_val = *(int *)&src;
				} else {
					reg = add_src_reg(ctx, cur, src, src_swiz(src, i));
				}
				reg->flags |= flags & ~(IR3_REG_FNEG | IR3_REG_SNEG);
				if (flags & IR3_REG_FNEG)
					reg->flags ^= IR3_REG_FNEG;
				if (flags & IR3_REG_SNEG)
					reg->flags ^= IR3_REG_SNEG;
			}
			va_end(ap);
		}
	}

	instr_atomic_end(ctx);
}

/*
 * Handlers for TGSI instructions which do not have a 1:1 mapping to
 * native instructions:
 */

static void
trans_clamp(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;
	struct tgsi_src_register *src2 = &inst->Src[2].Register;

	create_clamp(ctx, dst, src0, src1, src2);

	put_dst(ctx, inst, dst);
}

/* ARL(x) = x, but mova from hrN.x to a0.. */
static void
trans_arl(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	struct tgsi_src_register *src = &inst->Src[0].Register;
	unsigned chan = src->SwizzleX;

	compile_assert(ctx, dst->File == TGSI_FILE_ADDRESS);

	/* NOTE: we allocate a temporary from a flat register
	 * namespace (ignoring half vs full).  It turns out
	 * not to really matter since registers get reassigned
	 * later in ir3_ra which (hopefully!) can deal a bit
	 * better with mixed half and full precision.
	 */
	tmp_src = get_internal_temp(ctx, &tmp_dst);

	/* cov.{u,f}{32,16}s16 Rtmp, Rsrc */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = (t->tgsi_opc == TGSI_OPCODE_ARL) ?
			get_ftype(ctx) : get_utype(ctx);
	instr->cat1.dst_type = TYPE_S16;
	add_dst_reg(ctx, instr, &tmp_dst, chan)->flags |= IR3_REG_HALF;
	add_src_reg(ctx, instr, src, chan);

	/* shl.b Rtmp, Rtmp, 2 */
	instr = instr_create(ctx, 2, OPC_SHL_B);
	add_dst_reg(ctx, instr, &tmp_dst, chan)->flags |= IR3_REG_HALF;
	add_src_reg(ctx, instr, tmp_src, chan)->flags |= IR3_REG_HALF;
	ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 2;

	/* mova a0, Rtmp */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = TYPE_S16;
	instr->cat1.dst_type = TYPE_S16;
	add_dst_reg(ctx, instr, dst, 0)->flags |= IR3_REG_HALF;
	add_src_reg(ctx, instr, tmp_src, chan)->flags |= IR3_REG_HALF;
}

/*
 * texture fetch/sample instructions:
 */

struct tex_info {
	int8_t order[4];
	int8_t args;
	unsigned src_wrmask, flags;
};

struct target_info {
	uint8_t dims;
	uint8_t cube;
	uint8_t array;
	uint8_t shadow;
};

static const struct target_info tex_targets[] = {
	[TGSI_TEXTURE_1D]               = { 1, 0, 0, 0 },
	[TGSI_TEXTURE_2D]               = { 2, 0, 0, 0 },
	[TGSI_TEXTURE_3D]               = { 3, 0, 0, 0 },
	[TGSI_TEXTURE_CUBE]             = { 3, 1, 0, 0 },
	[TGSI_TEXTURE_RECT]             = { 2, 0, 0, 0 },
	[TGSI_TEXTURE_SHADOW1D]         = { 1, 0, 0, 1 },
	[TGSI_TEXTURE_SHADOW2D]         = { 2, 0, 0, 1 },
	[TGSI_TEXTURE_SHADOWRECT]       = { 2, 0, 0, 1 },
	[TGSI_TEXTURE_1D_ARRAY]         = { 1, 0, 1, 0 },
	[TGSI_TEXTURE_2D_ARRAY]         = { 2, 0, 1, 0 },
	[TGSI_TEXTURE_SHADOW1D_ARRAY]   = { 1, 0, 1, 1 },
	[TGSI_TEXTURE_SHADOW2D_ARRAY]   = { 2, 0, 1, 1 },
	[TGSI_TEXTURE_SHADOWCUBE]       = { 3, 1, 0, 1 },
	[TGSI_TEXTURE_2D_MSAA]          = { 2, 0, 0, 0 },
	[TGSI_TEXTURE_2D_ARRAY_MSAA]    = { 2, 0, 1, 0 },
	[TGSI_TEXTURE_CUBE_ARRAY]       = { 3, 1, 1, 0 },
	[TGSI_TEXTURE_SHADOWCUBE_ARRAY] = { 3, 1, 1, 1 },
};

static void
fill_tex_info(struct ir3_compile_context *ctx,
			  struct tgsi_full_instruction *inst,
			  struct tex_info *info)
{
	const struct target_info *tgt = &tex_targets[inst->Texture.Texture];

	if (tgt->dims == 3)
		info->flags |= IR3_INSTR_3D;
	if (tgt->array)
		info->flags |= IR3_INSTR_A;
	if (tgt->shadow)
		info->flags |= IR3_INSTR_S;

	switch (inst->Instruction.Opcode) {
	case TGSI_OPCODE_TXB:
	case TGSI_OPCODE_TXB2:
	case TGSI_OPCODE_TXL:
	case TGSI_OPCODE_TXF:
		info->args = 2;
		break;
	case TGSI_OPCODE_TXP:
		info->flags |= IR3_INSTR_P;
		/* fallthrough */
	case TGSI_OPCODE_TEX:
	case TGSI_OPCODE_TXD:
		info->args = 1;
		break;
	}

	/*
	 * lay out the first argument in the proper order:
	 *  - actual coordinates first
	 *  - shadow reference
	 *  - array index
	 *  - projection w
	 *
	 * bias/lod go into the second arg
	 */
	int arg, pos = 0;
	for (arg = 0; arg < tgt->dims; arg++)
		info->order[arg] = pos++;
	if (tgt->dims == 1)
		info->order[pos++] = -1;
	if (tgt->shadow)
		info->order[pos++] = MAX2(arg + tgt->array, 2);
	if (tgt->array)
		info->order[pos++] = arg++;
	if (info->flags & IR3_INSTR_P)
		info->order[pos++] = 3;

	info->src_wrmask = (1 << pos) - 1;

	for (; pos < 4; pos++)
		info->order[pos] = -1;

	assert(pos <= 4);
}

static bool check_swiz(struct tgsi_src_register *src, const int8_t order[4])
{
	unsigned i;
	for (i = 1; (i < 4) && order[i] >= 0; i++)
		if (src_swiz(src, i) != (src_swiz(src, 0) + order[i]))
			return false;
	return true;
}

static bool is_1d(unsigned tex)
{
	return tex_targets[tex].dims == 1;
}

static struct tgsi_src_register *
get_tex_coord(struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst,
		const struct tex_info *tinf)
{
	struct tgsi_src_register *coord = &inst->Src[0].Register;
	struct ir3_instruction *instr;
	unsigned tex = inst->Texture.Texture;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	type_t type_mov = get_ftype(ctx);
	unsigned j;

	/* need to move things around: */
	tmp_src = get_internal_temp(ctx, &tmp_dst);

	for (j = 0; j < 4; j++) {
		if (tinf->order[j] < 0)
			continue;
		instr = instr_create(ctx, 1, 0);  /* mov */
		instr->cat1.src_type = type_mov;
		instr->cat1.dst_type = type_mov;
		add_dst_reg(ctx, instr, &tmp_dst, j);
		add_src_reg(ctx, instr, coord,
				src_swiz(coord, tinf->order[j]));
	}

	/* fix up .y coord: */
	if (is_1d(tex)) {
		struct ir3_register *imm;
		instr = instr_create(ctx, 1, 0);  /* mov */
		instr->cat1.src_type = type_mov;
		instr->cat1.dst_type = type_mov;
		add_dst_reg(ctx, instr, &tmp_dst, 1);  /* .y */
		imm = ir3_reg_create(instr, 0, IR3_REG_IMMED);
		if (inst->Instruction.Opcode == TGSI_OPCODE_TXF)
			imm->iim_val = 0;
		else
			imm->fim_val = 0.5;
	}

	return tmp_src;
}

static void
trans_samp(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr, *collect;
	struct ir3_register *reg;
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	struct tgsi_src_register *orig, *coord, *samp, *offset, *dpdx, *dpdy;
	struct tgsi_src_register zero;
	const struct target_info *tgt = &tex_targets[inst->Texture.Texture];
	struct tex_info tinf;
	int i;

	memset(&tinf, 0, sizeof(tinf));
	fill_tex_info(ctx, inst, &tinf);
	coord = get_tex_coord(ctx, inst, &tinf);
	get_immediate(ctx, &zero, 0);

	switch (inst->Instruction.Opcode) {
	case TGSI_OPCODE_TXB2:
		orig = &inst->Src[1].Register;
		samp = &inst->Src[2].Register;
		break;
	case TGSI_OPCODE_TXD:
		orig = &inst->Src[0].Register;
		dpdx = &inst->Src[1].Register;
		dpdy = &inst->Src[2].Register;
		samp = &inst->Src[3].Register;
		if (is_rel_or_const(dpdx))
				dpdx = get_unconst(ctx, dpdx);
		if (is_rel_or_const(dpdy))
				dpdy = get_unconst(ctx, dpdy);
		break;
	default:
		orig = &inst->Src[0].Register;
		samp = &inst->Src[1].Register;
		break;
	}
	if (tinf.args > 1 && is_rel_or_const(orig))
		orig = get_unconst(ctx, orig);

	/* scale up integer coords for TXF based on the LOD */
	if (inst->Instruction.Opcode == TGSI_OPCODE_TXF) {
		struct tgsi_dst_register tmp_dst;
		struct tgsi_src_register *tmp_src;
		type_t type_mov = get_utype(ctx);

		tmp_src = get_internal_temp(ctx, &tmp_dst);
		for (i = 0; i < tgt->dims; i++) {
			instr = instr_create(ctx, 2, OPC_SHL_B);
			add_dst_reg(ctx, instr, &tmp_dst, i);
			add_src_reg(ctx, instr, coord, src_swiz(coord, i));
			add_src_reg(ctx, instr, orig, orig->SwizzleW);
		}
		if (tgt->dims < 2) {
			instr = instr_create(ctx, 1, 0);
			instr->cat1.src_type = type_mov;
			instr->cat1.dst_type = type_mov;
			add_dst_reg(ctx, instr, &tmp_dst, i);
			add_src_reg(ctx, instr, &zero, zero.SwizzleX);
			i++;
		}
		if (tgt->array) {
			instr = instr_create(ctx, 1, 0);
			instr->cat1.src_type = type_mov;
			instr->cat1.dst_type = type_mov;
			add_dst_reg(ctx, instr, &tmp_dst, i);
			add_src_reg(ctx, instr, coord, src_swiz(coord, i));
		}
		coord = tmp_src;
	}

	if (inst->Texture.NumOffsets) {
		struct tgsi_texture_offset *tex_offset = &inst->TexOffsets[0];
		struct tgsi_src_register offset_src = {0};

		offset_src.File = tex_offset->File;
		offset_src.Index = tex_offset->Index;
		offset_src.SwizzleX = tex_offset->SwizzleX;
		offset_src.SwizzleY = tex_offset->SwizzleY;
		offset_src.SwizzleZ = tex_offset->SwizzleZ;
		offset = get_unconst(ctx, &offset_src);
		tinf.flags |= IR3_INSTR_O;
	}

	instr = instr_create(ctx, 5, t->opc);
	if (ctx->integer_s & (1 << samp->Index))
		instr->cat5.type = get_utype(ctx);
	else
		instr->cat5.type = get_ftype(ctx);
	instr->cat5.samp = samp->Index;
	instr->cat5.tex  = samp->Index;
	instr->flags |= tinf.flags;

	add_dst_reg_wrmask(ctx, instr, dst, 0, dst->WriteMask);

	reg = ir3_reg_create(instr, 0, IR3_REG_SSA);

	collect = ir3_instr_create2(ctx->block, -1, OPC_META_FI, 12);
	ir3_reg_create(collect, 0, 0);
	for (i = 0; i < 4; i++) {
		if (tinf.src_wrmask & (1 << i))
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
					coord, src_swiz(coord, i));
		else if (tinf.src_wrmask & ~((1 << i) - 1))
			ir3_reg_create(collect, 0, 0);
	}

	/* Attach derivatives onto the end of the fan-in. Derivatives start after
	 * the 4th argument, so make sure that fi is padded up to 4 first.
	 */
	if (inst->Instruction.Opcode == TGSI_OPCODE_TXD) {
		while (collect->regs_count < 5)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
					&zero, zero.SwizzleX);
		for (i = 0; i < tgt->dims; i++)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA), dpdx, i);
		if (tgt->dims < 2)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
					&zero, zero.SwizzleX);
		for (i = 0; i < tgt->dims; i++)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA), dpdy, i);
		if (tgt->dims < 2)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
					&zero, zero.SwizzleX);
		tinf.src_wrmask |= ((1 << (2 * MAX2(tgt->dims, 2))) - 1) << 4;
	}

	reg->instr = collect;
	reg->wrmask = tinf.src_wrmask;

	/* The second argument contains the offsets, followed by the lod/bias
	 * argument. This is constructed more manually due to the dynamic nature.
	 */
	if (inst->Texture.NumOffsets == 0 && tinf.args == 1)
		return;

	reg = ir3_reg_create(instr, 0, IR3_REG_SSA);

	collect = ir3_instr_create2(ctx->block, -1, OPC_META_FI, 5);
	ir3_reg_create(collect, 0, 0);

	if (inst->Texture.NumOffsets) {
		for (i = 0; i < tgt->dims; i++)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
					offset, i);
		if (tgt->dims < 2)
			ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
					&zero, zero.SwizzleX);
	}
	if (inst->Instruction.Opcode == TGSI_OPCODE_TXB2)
		ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
				orig, orig->SwizzleX);
	else if (tinf.args > 1)
		ssa_src(ctx, ir3_reg_create(collect, 0, IR3_REG_SSA),
				orig, orig->SwizzleW);

	reg->instr = collect;
	reg->wrmask = (1 << (collect->regs_count - 1)) - 1;
}

static void
trans_txq(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	struct tgsi_src_register *level = &inst->Src[0].Register;
	struct tgsi_src_register *samp = &inst->Src[1].Register;
	const struct target_info *tgt = &tex_targets[inst->Texture.Texture];
	struct tex_info tinf;

	memset(&tinf, 0, sizeof(tinf));
	fill_tex_info(ctx, inst, &tinf);
	if (is_rel_or_const(level))
		level = get_unconst(ctx, level);

	instr = instr_create(ctx, 5, OPC_GETSIZE);
	instr->cat5.type = get_utype(ctx);
	instr->cat5.samp = samp->Index;
	instr->cat5.tex  = samp->Index;
	instr->flags |= tinf.flags;

	if (tgt->array && (dst->WriteMask & (1 << tgt->dims))) {
		/* Array size actually ends up in .w rather than .z. This doesn't
		 * matter for miplevel 0, but for higher mips the value in z is
		 * minified whereas w stays. Also, the value in TEX_CONST_3_DEPTH is
		 * returned, which means that we have to add 1 to it for arrays.
		 */
		struct tgsi_dst_register tmp_dst;
		struct tgsi_src_register *tmp_src;
		type_t type_mov = get_utype(ctx);

		tmp_src = get_internal_temp(ctx, &tmp_dst);
		add_dst_reg_wrmask(ctx, instr, &tmp_dst, 0,
						   dst->WriteMask | TGSI_WRITEMASK_W);
		add_src_reg_wrmask(ctx, instr, level, level->SwizzleX, 0x1);

		if (dst->WriteMask & TGSI_WRITEMASK_X) {
			instr = instr_create(ctx, 1, 0);
			instr->cat1.src_type = type_mov;
			instr->cat1.dst_type = type_mov;
			add_dst_reg(ctx, instr, dst, 0);
			add_src_reg(ctx, instr, tmp_src, src_swiz(tmp_src, 0));
		}

		if (tgt->dims == 2) {
			if (dst->WriteMask & TGSI_WRITEMASK_Y) {
				instr = instr_create(ctx, 1, 0);
				instr->cat1.src_type = type_mov;
				instr->cat1.dst_type = type_mov;
				add_dst_reg(ctx, instr, dst, 1);
				add_src_reg(ctx, instr, tmp_src, src_swiz(tmp_src, 1));
			}
		}

		instr = instr_create(ctx, 2, OPC_ADD_U);
		add_dst_reg(ctx, instr, dst, tgt->dims);
		add_src_reg(ctx, instr, tmp_src, src_swiz(tmp_src, 3));
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 1;
	} else {
		add_dst_reg_wrmask(ctx, instr, dst, 0, dst->WriteMask);
		add_src_reg_wrmask(ctx, instr, level, level->SwizzleX, 0x1);
	}

	if (dst->WriteMask & TGSI_WRITEMASK_W) {
		/* The # of levels comes from getinfo.z. We need to add 1 to it, since
		 * the value in TEX_CONST_0 is zero-based.
		 */
		struct tgsi_dst_register tmp_dst;
		struct tgsi_src_register *tmp_src;

		tmp_src = get_internal_temp(ctx, &tmp_dst);
		instr = instr_create(ctx, 5, OPC_GETINFO);
		instr->cat5.type = get_utype(ctx);
		instr->cat5.samp = samp->Index;
		instr->cat5.tex  = samp->Index;
		add_dst_reg_wrmask(ctx, instr, &tmp_dst, 0, TGSI_WRITEMASK_Z);

		instr = instr_create(ctx, 2, OPC_ADD_U);
		add_dst_reg(ctx, instr, dst, 3);
		add_src_reg(ctx, instr, tmp_src, src_swiz(tmp_src, 2));
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 1;
	}
}

/* DDX/DDY */
static void
trans_deriv(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	struct tgsi_src_register *src = &inst->Src[0].Register;
	static const int8_t order[4] = {0, 1, 2, 3};

	if (!check_swiz(src, order)) {
		struct tgsi_dst_register tmp_dst;
		struct tgsi_src_register *tmp_src;

		tmp_src = get_internal_temp(ctx, &tmp_dst);
		create_mov(ctx, &tmp_dst, src);

		src = tmp_src;
	}

	/* This might be a workaround for hw bug?  Blob compiler always
	 * seems to work two components at a time for dsy/dsx.  It does
	 * actually seem to work in some cases (or at least some piglit
	 * tests) for four components at a time.  But seems more reliable
	 * to split this into two instructions like the blob compiler
	 * does:
	 */

	instr = instr_create(ctx, 5, t->opc);
	instr->cat5.type = get_ftype(ctx);
	add_dst_reg_wrmask(ctx, instr, dst, 0, dst->WriteMask & 0x3);
	add_src_reg_wrmask(ctx, instr, src, 0, dst->WriteMask & 0x3);

	instr = instr_create(ctx, 5, t->opc);
	instr->cat5.type = get_ftype(ctx);
	add_dst_reg_wrmask(ctx, instr, dst, 2, (dst->WriteMask >> 2) & 0x3);
	add_src_reg_wrmask(ctx, instr, src, 2, (dst->WriteMask >> 2) & 0x3);
}

/*
 * SEQ(a,b) = (a == b) ? 1.0 : 0.0
 *   cmps.f.eq tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * SNE(a,b) = (a != b) ? 1.0 : 0.0
 *   cmps.f.ne tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * SGE(a,b) = (a >= b) ? 1.0 : 0.0
 *   cmps.f.ge tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * SLE(a,b) = (a <= b) ? 1.0 : 0.0
 *   cmps.f.le tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * SGT(a,b) = (a > b)  ? 1.0 : 0.0
 *   cmps.f.gt tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * SLT(a,b) = (a < b)  ? 1.0 : 0.0
 *   cmps.f.lt tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * CMP(a,b,c) = (a < 0.0) ? b : c
 *   cmps.f.lt tmp0, a, {0.0}
 *   sel.b16 dst, b, tmp0, c
 */
static void
trans_cmp(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_src_register constval0;
	/* final instruction for CMP() uses orig src1 and src2: */
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *a0, *a1, *a2;
	unsigned condition;

	tmp_src = get_internal_temp(ctx, &tmp_dst);

	a0 = &inst->Src[0].Register;  /* a */
	a1 = &inst->Src[1].Register;  /* b */

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_SEQ:
	case TGSI_OPCODE_FSEQ:
		condition = IR3_COND_EQ;
		break;
	case TGSI_OPCODE_SNE:
	case TGSI_OPCODE_FSNE:
		condition = IR3_COND_NE;
		break;
	case TGSI_OPCODE_SGE:
	case TGSI_OPCODE_FSGE:
		condition = IR3_COND_GE;
		break;
	case TGSI_OPCODE_SLT:
	case TGSI_OPCODE_FSLT:
		condition = IR3_COND_LT;
		break;
	case TGSI_OPCODE_SLE:
		condition = IR3_COND_LE;
		break;
	case TGSI_OPCODE_SGT:
		condition = IR3_COND_GT;
		break;
	case TGSI_OPCODE_CMP:
		get_immediate(ctx, &constval0, fui(0.0));
		a0 = &inst->Src[0].Register;  /* a */
		a1 = &constval0;              /* {0.0} */
		condition = IR3_COND_LT;
		break;
	default:
		compile_assert(ctx, 0);
		return;
	}

	if (is_const(a0) && is_const(a1))
		a0 = get_unconst(ctx, a0);

	/* cmps.f.<cond> tmp, a0, a1 */
	instr = instr_create(ctx, 2, OPC_CMPS_F);
	instr->cat2.condition = condition;
	vectorize(ctx, instr, &tmp_dst, 2, a0, 0, a1, 0);

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_SEQ:
	case TGSI_OPCODE_SGE:
	case TGSI_OPCODE_SLE:
	case TGSI_OPCODE_SNE:
	case TGSI_OPCODE_SGT:
	case TGSI_OPCODE_SLT:
		/* cov.u16f16 dst, tmp0 */
		instr = instr_create(ctx, 1, 0);
		instr->cat1.src_type = get_utype(ctx);
		instr->cat1.dst_type = get_ftype(ctx);
		vectorize(ctx, instr, dst, 1, tmp_src, 0);
		break;
	case TGSI_OPCODE_FSEQ:
	case TGSI_OPCODE_FSGE:
	case TGSI_OPCODE_FSNE:
	case TGSI_OPCODE_FSLT:
		/* absneg.s dst, (neg)tmp0 */
		instr = instr_create(ctx, 2, OPC_ABSNEG_S);
		vectorize(ctx, instr, dst, 1, tmp_src, IR3_REG_SNEG);
		break;
	case TGSI_OPCODE_CMP:
		a1 = &inst->Src[1].Register;
		a2 = &inst->Src[2].Register;
		/* sel.{b32,b16} dst, src2, tmp, src1 */
		instr = instr_create(ctx, 3, OPC_SEL_B32);
		vectorize(ctx, instr, dst, 3, a1, 0, tmp_src, 0, a2, 0);

		break;
	}

	put_dst(ctx, inst, dst);
}

/*
 * USNE(a,b) = (a != b) ? ~0 : 0
 *   cmps.u32.ne dst, a, b
 *
 * USEQ(a,b) = (a == b) ? ~0 : 0
 *   cmps.u32.eq dst, a, b
 *
 * ISGE(a,b) = (a > b) ? ~0 : 0
 *   cmps.s32.ge dst, a, b
 *
 * USGE(a,b) = (a > b) ? ~0 : 0
 *   cmps.u32.ge dst, a, b
 *
 * ISLT(a,b) = (a < b) ? ~0 : 0
 *   cmps.s32.lt dst, a, b
 *
 * USLT(a,b) = (a < b) ? ~0 : 0
 *   cmps.u32.lt dst, a, b
 *
 */
static void
trans_icmp(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_src_register *a0, *a1;
	unsigned condition;

	a0 = &inst->Src[0].Register;  /* a */
	a1 = &inst->Src[1].Register;  /* b */

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_USNE:
		condition = IR3_COND_NE;
		break;
	case TGSI_OPCODE_USEQ:
		condition = IR3_COND_EQ;
		break;
	case TGSI_OPCODE_ISGE:
	case TGSI_OPCODE_USGE:
		condition = IR3_COND_GE;
		break;
	case TGSI_OPCODE_ISLT:
	case TGSI_OPCODE_USLT:
		condition = IR3_COND_LT;
		break;

	default:
		compile_assert(ctx, 0);
		return;
	}

	if (is_const(a0) && is_const(a1))
		a0 = get_unconst(ctx, a0);

	tmp_src = get_internal_temp(ctx, &tmp_dst);
	/* cmps.{u32,s32}.<cond> tmp, a0, a1 */
	instr = instr_create(ctx, 2, t->opc);
	instr->cat2.condition = condition;
	vectorize(ctx, instr, &tmp_dst, 2, a0, 0, a1, 0);

	/* absneg.s dst, (neg)tmp */
	instr = instr_create(ctx, 2, OPC_ABSNEG_S);
	vectorize(ctx, instr, dst, 1, tmp_src, IR3_REG_SNEG);

	put_dst(ctx, inst, dst);
}

/*
 * UCMP(a,b,c) = a ? b : c
 *   sel.b16 dst, b, a, c
 */
static void
trans_ucmp(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *a0, *a1, *a2;

	a0 = &inst->Src[0].Register;  /* a */
	a1 = &inst->Src[1].Register;  /* b */
	a2 = &inst->Src[2].Register;  /* c */

	if (is_rel_or_const(a0))
		a0 = get_unconst(ctx, a0);

	/* sel.{b32,b16} dst, b, a, c */
	instr = instr_create(ctx, 3, OPC_SEL_B32);
	vectorize(ctx, instr, dst, 3, a1, 0, a0, 0, a2, 0);
	put_dst(ctx, inst, dst);
}

/*
 * ISSG(a) = a < 0 ? -1 : a > 0 ? 1 : 0
 *   cmps.s.lt tmp_neg, a, 0  # 1 if a is negative
 *   cmps.s.gt tmp_pos, a, 0  # 1 if a is positive
 *   sub.u dst, tmp_pos, tmp_neg
 */
static void
trans_issg(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *a = &inst->Src[0].Register;
	struct tgsi_dst_register neg_dst, pos_dst;
	struct tgsi_src_register *neg_src, *pos_src;

	neg_src = get_internal_temp(ctx, &neg_dst);
	pos_src = get_internal_temp(ctx, &pos_dst);

	/* cmps.s.lt neg, a, 0 */
	instr = instr_create(ctx, 2, OPC_CMPS_S);
	instr->cat2.condition = IR3_COND_LT;
	vectorize(ctx, instr, &neg_dst, 2, a, 0, 0, IR3_REG_IMMED);

	/* cmps.s.gt pos, a, 0 */
	instr = instr_create(ctx, 2, OPC_CMPS_S);
	instr->cat2.condition = IR3_COND_GT;
	vectorize(ctx, instr, &pos_dst, 2, a, 0, 0, IR3_REG_IMMED);

	/* sub.u dst, pos, neg */
	instr = instr_create(ctx, 2, OPC_SUB_U);
	vectorize(ctx, instr, dst, 2, pos_src, 0, neg_src, 0);

	put_dst(ctx, inst, dst);
}



/*
 * Conditional / Flow control
 */

static void
push_branch(struct ir3_compile_context *ctx, bool inv,
		struct ir3_instruction *instr, struct ir3_instruction *cond)
{
	unsigned int idx = ctx->branch_count++;
	compile_assert(ctx, idx < ARRAY_SIZE(ctx->branch));
	ctx->branch[idx].instr = instr;
	ctx->branch[idx].inv = inv;
	/* else side of branch has same condition: */
	if (!inv)
		ctx->branch[idx].cond = cond;
}

static struct ir3_instruction *
pop_branch(struct ir3_compile_context *ctx)
{
	unsigned int idx = --ctx->branch_count;
	return ctx->branch[idx].instr;
}

static void
trans_if(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr, *cond;
	struct tgsi_src_register *src = &inst->Src[0].Register;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_src_register constval;

	get_immediate(ctx, &constval, fui(0.0));
	tmp_src = get_internal_temp(ctx, &tmp_dst);

	if (is_const(src))
		src = get_unconst(ctx, src);

	/* cmps.{f,u}.ne tmp0, b, {0.0} */
	instr = instr_create(ctx, 2, t->opc);
	add_dst_reg(ctx, instr, &tmp_dst, 0);
	add_src_reg(ctx, instr, src, src->SwizzleX);
	add_src_reg(ctx, instr, &constval, constval.SwizzleX);
	instr->cat2.condition = IR3_COND_NE;

	compile_assert(ctx, instr->regs[1]->flags & IR3_REG_SSA); /* because get_unconst() */
	cond = instr->regs[1]->instr;

	/* meta:flow tmp0 */
	instr = instr_create(ctx, -1, OPC_META_FLOW);
	ir3_reg_create(instr, 0, 0);  /* dummy dst */
	add_src_reg(ctx, instr, tmp_src, TGSI_SWIZZLE_X);

	push_branch(ctx, false, instr, cond);
	instr->flow.if_block = push_block(ctx);
}

static void
trans_else(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;

	pop_block(ctx);

	instr = pop_branch(ctx);

	compile_assert(ctx, (instr->category == -1) &&
			(instr->opc == OPC_META_FLOW));

	push_branch(ctx, true, instr, NULL);
	instr->flow.else_block = push_block(ctx);
}

static struct ir3_instruction *
find_temporary(struct ir3_block *block, unsigned n)
{
	if (block->parent && !block->temporaries[n])
		return find_temporary(block->parent, n);
	return block->temporaries[n];
}

static struct ir3_instruction *
find_output(struct ir3_block *block, unsigned n)
{
	if (block->parent && !block->outputs[n])
		return find_output(block->parent, n);
	return block->outputs[n];
}

static struct ir3_instruction *
create_phi(struct ir3_compile_context *ctx, struct ir3_instruction *cond,
		struct ir3_instruction *a, struct ir3_instruction *b)
{
	struct ir3_instruction *phi;

	compile_assert(ctx, cond);

	/* Either side of the condition could be null..  which
	 * indicates a variable written on only one side of the
	 * branch.  Normally this should only be variables not
	 * used outside of that side of the branch.  So we could
	 * just 'return a ? a : b;' in that case.  But for better
	 * defined undefined behavior we just stick in imm{0.0}.
	 * In the common case of a value only used within the
	 * one side of the branch, the PHI instruction will not
	 * get scheduled
	 */
	if (!a)
		a = create_immed(ctx, 0.0);
	if (!b)
		b = create_immed(ctx, 0.0);

	phi = instr_create(ctx, -1, OPC_META_PHI);
	ir3_reg_create(phi, 0, 0);  /* dummy dst */
	ir3_reg_create(phi, 0, IR3_REG_SSA)->instr = cond;
	ir3_reg_create(phi, 0, IR3_REG_SSA)->instr = a;
	ir3_reg_create(phi, 0, IR3_REG_SSA)->instr = b;

	return phi;
}

static void
trans_endif(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct ir3_block *ifb, *elseb;
	struct ir3_instruction **ifout, **elseout;
	unsigned i, ifnout = 0, elsenout = 0;

	pop_block(ctx);

	instr = pop_branch(ctx);

	compile_assert(ctx, (instr->category == -1) &&
			(instr->opc == OPC_META_FLOW));

	ifb = instr->flow.if_block;
	elseb = instr->flow.else_block;
	/* if there is no else block, the parent block is used for the
	 * branch-not-taken src of the PHI instructions:
	 */
	if (!elseb)
		elseb = ifb->parent;

	/* worst case sizes: */
	ifnout = ifb->ntemporaries + ifb->noutputs;
	elsenout = elseb->ntemporaries + elseb->noutputs;

	ifout = ir3_alloc(ctx->ir, sizeof(ifb->outputs[0]) * ifnout);
	if (elseb != ifb->parent)
		elseout = ir3_alloc(ctx->ir, sizeof(ifb->outputs[0]) * elsenout);

	ifnout = 0;
	elsenout = 0;

	/* generate PHI instructions for any temporaries written: */
	for (i = 0; i < ifb->ntemporaries; i++) {
		struct ir3_instruction *a = ifb->temporaries[i];
		struct ir3_instruction *b = elseb->temporaries[i];

		/* if temporary written in if-block, or if else block
		 * is present and temporary written in else-block:
		 */
		if (a || ((elseb != ifb->parent) && b)) {
			struct ir3_instruction *phi;

			/* if only written on one side, find the closest
			 * enclosing update on other side:
			 */
			if (!a)
				a = find_temporary(ifb, i);
			if (!b)
				b = find_temporary(elseb, i);

			ifout[ifnout] = a;
			a = create_output(ifb, a, ifnout++);

			if (elseb != ifb->parent) {
				elseout[elsenout] = b;
				b = create_output(elseb, b, elsenout++);
			}

			phi = create_phi(ctx, instr, a, b);
			ctx->block->temporaries[i] = phi;
		}
	}

	compile_assert(ctx, ifb->noutputs == elseb->noutputs);

	/* .. and any outputs written: */
	for (i = 0; i < ifb->noutputs; i++) {
		struct ir3_instruction *a = ifb->outputs[i];
		struct ir3_instruction *b = elseb->outputs[i];

		/* if output written in if-block, or if else block
		 * is present and output written in else-block:
		 */
		if (a || ((elseb != ifb->parent) && b)) {
			struct ir3_instruction *phi;

			/* if only written on one side, find the closest
			 * enclosing update on other side:
			 */
			if (!a)
				a = find_output(ifb, i);
			if (!b)
				b = find_output(elseb, i);

			ifout[ifnout] = a;
			a = create_output(ifb, a, ifnout++);

			if (elseb != ifb->parent) {
				elseout[elsenout] = b;
				b = create_output(elseb, b, elsenout++);
			}

			phi = create_phi(ctx, instr, a, b);
			ctx->block->outputs[i] = phi;
		}
	}

	ifb->noutputs = ifnout;
	ifb->outputs = ifout;

	if (elseb != ifb->parent) {
		elseb->noutputs = elsenout;
		elseb->outputs = elseout;
	}

	// TODO maybe we want to compact block->inputs?
}

/*
 * Kill
 */

static void
trans_kill(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr, *immed, *cond = NULL;
	bool inv = false;

	/* unconditional kill, use enclosing if condition: */
	if (ctx->branch_count > 0) {
		unsigned int idx = ctx->branch_count - 1;
		cond = ctx->branch[idx].cond;
		inv = ctx->branch[idx].inv;
	} else {
		cond = create_immed(ctx, 1.0);
	}

	compile_assert(ctx, cond);

	immed = create_immed(ctx, 0.0);

	/* cmps.f.ne p0.x, cond, {0.0} */
	instr = instr_create(ctx, 2, OPC_CMPS_F);
	instr->cat2.condition = IR3_COND_NE;
	ir3_reg_create(instr, regid(REG_P0, 0), 0);
	ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = cond;
	ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = immed;
	cond = instr;

	/* kill p0.x */
	instr = instr_create(ctx, 0, OPC_KILL);
	instr->cat0.inv = inv;
	ir3_reg_create(instr, 0, 0);  /* dummy dst */
	ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = cond;

	ctx->kill[ctx->kill_count++] = instr;

	ctx->so->has_kill = true;
}

/*
 * Kill-If
 */

static void
trans_killif(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_src_register *src = &inst->Src[0].Register;
	struct ir3_instruction *instr, *immed, *cond = NULL;
	bool inv = false;

	immed = create_immed(ctx, 0.0);

	/* cmps.f.ne p0.x, cond, {0.0} */
	instr = instr_create(ctx, 2, OPC_CMPS_F);
	instr->cat2.condition = IR3_COND_NE;
	ir3_reg_create(instr, regid(REG_P0, 0), 0);
	ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = immed;
	add_src_reg(ctx, instr, src, src->SwizzleX);

	cond = instr;

	/* kill p0.x */
	instr = instr_create(ctx, 0, OPC_KILL);
	instr->cat0.inv = inv;
	ir3_reg_create(instr, 0, 0);  /* dummy dst */
	ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = cond;

	ctx->kill[ctx->kill_count++] = instr;

	ctx->so->has_kill = true;

}
/*
 * I2F / U2F / F2I / F2U
 */

static void
trans_cov(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src = &inst->Src[0].Register;

	// cov.f32s32 dst, tmp0 /
	instr = instr_create(ctx, 1, 0);
	switch (t->tgsi_opc) {
	case TGSI_OPCODE_U2F:
		instr->cat1.src_type = TYPE_U32;
		instr->cat1.dst_type = TYPE_F32;
		break;
	case TGSI_OPCODE_I2F:
		instr->cat1.src_type = TYPE_S32;
		instr->cat1.dst_type = TYPE_F32;
		break;
	case TGSI_OPCODE_F2U:
		instr->cat1.src_type = TYPE_F32;
		instr->cat1.dst_type = TYPE_U32;
		break;
	case TGSI_OPCODE_F2I:
		instr->cat1.src_type = TYPE_F32;
		instr->cat1.dst_type = TYPE_S32;
		break;

	}
	vectorize(ctx, instr, dst, 1, src, 0);
	put_dst(ctx, inst, dst);
}

/*
 * UMUL / UMAD
 *
 * There is no 32-bit multiply instruction, so splitting a and b into high and
 * low components, we get that
 *
 * dst = al * bl + ah * bl << 16 + al * bh << 16
 *
 *  mull.u tmp0, a, b (mul low, i.e. al * bl)
 *  madsh.m16 tmp1, a, b, tmp0 (mul-add shift high mix, i.e. ah * bl << 16)
 *  madsh.m16 dst, b, a, tmp1 (i.e. al * bh << 16)
 *
 * For UMAD, add in the extra argument after mull.u.
 */
static void
trans_umul(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *a = &inst->Src[0].Register;
	struct tgsi_src_register *b = &inst->Src[1].Register;

	struct tgsi_dst_register tmp0_dst, tmp1_dst;
	struct tgsi_src_register *tmp0_src, *tmp1_src;

	tmp0_src = get_internal_temp(ctx, &tmp0_dst);
	tmp1_src = get_internal_temp(ctx, &tmp1_dst);

	if (is_rel_or_const(a))
		a = get_unconst(ctx, a);
	if (is_rel_or_const(b))
		b = get_unconst(ctx, b);

	/* mull.u tmp0, a, b */
	instr = instr_create(ctx, 2, OPC_MULL_U);
	vectorize(ctx, instr, &tmp0_dst, 2, a, 0, b, 0);

	if (t->tgsi_opc == TGSI_OPCODE_UMAD) {
		struct tgsi_src_register *c = &inst->Src[2].Register;

		/* add.u tmp0, tmp0, c */
		instr = instr_create(ctx, 2, OPC_ADD_U);
		vectorize(ctx, instr, &tmp0_dst, 2, tmp0_src, 0, c, 0);
	}

	/* madsh.m16 tmp1, a, b, tmp0 */
	instr = instr_create(ctx, 3, OPC_MADSH_M16);
	vectorize(ctx, instr, &tmp1_dst, 3, a, 0, b, 0, tmp0_src, 0);

	/* madsh.m16 dst, b, a, tmp1 */
	instr = instr_create(ctx, 3, OPC_MADSH_M16);
	vectorize(ctx, instr, dst, 3, b, 0, a, 0, tmp1_src, 0);
	put_dst(ctx, inst, dst);
}

/*
 * IDIV / UDIV / MOD / UMOD
 *
 * See NV50LegalizeSSA::handleDIV for the origin of this implementation. For
 * MOD/UMOD, it becomes a - [IU]DIV(a, modulus) * modulus.
 */
static void
trans_idiv(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register *dst = get_dst(ctx, inst), *premod_dst = dst;
	struct tgsi_src_register *a = &inst->Src[0].Register;
	struct tgsi_src_register *b = &inst->Src[1].Register;

	struct tgsi_dst_register af_dst, bf_dst, q_dst, r_dst, a_dst, b_dst;
	struct tgsi_src_register *af_src, *bf_src, *q_src, *r_src, *a_src, *b_src;

	struct tgsi_src_register negative_2, thirty_one;
	type_t src_type;

	if (t->tgsi_opc == TGSI_OPCODE_IDIV || t->tgsi_opc == TGSI_OPCODE_MOD)
		src_type = get_stype(ctx);
	else
		src_type = get_utype(ctx);

	af_src = get_internal_temp(ctx, &af_dst);
	bf_src = get_internal_temp(ctx, &bf_dst);
	q_src = get_internal_temp(ctx, &q_dst);
	r_src = get_internal_temp(ctx, &r_dst);
	a_src = get_internal_temp(ctx, &a_dst);
	b_src = get_internal_temp(ctx, &b_dst);

	get_immediate(ctx, &negative_2, -2);
	get_immediate(ctx, &thirty_one, 31);

	if (t->tgsi_opc == TGSI_OPCODE_MOD || t->tgsi_opc == TGSI_OPCODE_UMOD)
		premod_dst = &q_dst;

	/* cov.[us]32f32 af, numerator */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = src_type;
	instr->cat1.dst_type = get_ftype(ctx);
	vectorize(ctx, instr, &af_dst, 1, a, 0);

	/* cov.[us]32f32 bf, denominator */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = src_type;
	instr->cat1.dst_type = get_ftype(ctx);
	vectorize(ctx, instr, &bf_dst, 1, b, 0);

	/* Get the absolute values for IDIV */
	if (type_sint(src_type)) {
		/* absneg.f af, (abs)af */
		instr = instr_create(ctx, 2, OPC_ABSNEG_F);
		vectorize(ctx, instr, &af_dst, 1, af_src, IR3_REG_FABS);

		/* absneg.f bf, (abs)bf */
		instr = instr_create(ctx, 2, OPC_ABSNEG_F);
		vectorize(ctx, instr, &bf_dst, 1, bf_src, IR3_REG_FABS);

		/* absneg.s a, (abs)numerator */
		instr = instr_create(ctx, 2, OPC_ABSNEG_S);
		vectorize(ctx, instr, &a_dst, 1, a, IR3_REG_SABS);

		/* absneg.s b, (abs)denominator */
		instr = instr_create(ctx, 2, OPC_ABSNEG_S);
		vectorize(ctx, instr, &b_dst, 1, b, IR3_REG_SABS);
	} else {
		/* mov.u32u32 a, numerator */
		instr = instr_create(ctx, 1, 0);
		instr->cat1.src_type = src_type;
		instr->cat1.dst_type = src_type;
		vectorize(ctx, instr, &a_dst, 1, a, 0);

		/* mov.u32u32 b, denominator */
		instr = instr_create(ctx, 1, 0);
		instr->cat1.src_type = src_type;
		instr->cat1.dst_type = src_type;
		vectorize(ctx, instr, &b_dst, 1, b, 0);
	}

	/* rcp.f bf, bf */
	instr = instr_create(ctx, 4, OPC_RCP);
	vectorize(ctx, instr, &bf_dst, 1, bf_src, 0);

	/* That's right, subtract 2 as an integer from the float */
	/* add.u bf, bf, -2 */
	instr = instr_create(ctx, 2, OPC_ADD_U);
	vectorize(ctx, instr, &bf_dst, 2, bf_src, 0, &negative_2, 0);

	/* mul.f q, af, bf */
	instr = instr_create(ctx, 2, OPC_MUL_F);
	vectorize(ctx, instr, &q_dst, 2, af_src, 0, bf_src, 0);

	/* cov.f32[us]32 q, q */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = get_ftype(ctx);
	instr->cat1.dst_type = src_type;
	vectorize(ctx, instr, &q_dst, 1, q_src, 0);

	/* integer multiply q by b */
	/* mull.u r, q, b */
	instr = instr_create(ctx, 2, OPC_MULL_U);
	vectorize(ctx, instr, &r_dst, 2, q_src, 0, b_src, 0);

	/* madsh.m16 r, q, b, r */
	instr = instr_create(ctx, 3, OPC_MADSH_M16);
	vectorize(ctx, instr, &r_dst, 3, q_src, 0, b_src, 0, r_src, 0);

	/* madsh.m16, r, b, q, r */
	instr = instr_create(ctx, 3, OPC_MADSH_M16);
	vectorize(ctx, instr, &r_dst, 3, b_src, 0, q_src, 0, r_src, 0);

	/* sub.u r, a, r */
	instr = instr_create(ctx, 2, OPC_SUB_U);
	vectorize(ctx, instr, &r_dst, 2, a_src, 0, r_src, 0);

	/* cov.u32f32, r, r */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = get_utype(ctx);
	instr->cat1.dst_type = get_ftype(ctx);
	vectorize(ctx, instr, &r_dst, 1, r_src, 0);

	/* mul.f r, r, bf */
	instr = instr_create(ctx, 2, OPC_MUL_F);
	vectorize(ctx, instr, &r_dst, 2, r_src, 0, bf_src, 0);

	/* cov.f32u32 r, r */
	instr = instr_create(ctx, 1, 0);
	instr->cat1.src_type = get_ftype(ctx);
	instr->cat1.dst_type = get_utype(ctx);
	vectorize(ctx, instr, &r_dst, 1, r_src, 0);

	/* add.u q, q, r */
	instr = instr_create(ctx, 2, OPC_ADD_U);
	vectorize(ctx, instr, &q_dst, 2, q_src, 0, r_src, 0);

	/* mull.u r, q, b */
	instr = instr_create(ctx, 2, OPC_MULL_U);
	vectorize(ctx, instr, &r_dst, 2, q_src, 0, b_src, 0);

	/* madsh.m16 r, q, b, r */
	instr = instr_create(ctx, 3, OPC_MADSH_M16);
	vectorize(ctx, instr, &r_dst, 3, q_src, 0, b_src, 0, r_src, 0);

	/* madsh.m16 r, b, q, r */
	instr = instr_create(ctx, 3, OPC_MADSH_M16);
	vectorize(ctx, instr, &r_dst, 3, b_src, 0, q_src, 0, r_src, 0);

	/* sub.u r, a, r */
	instr = instr_create(ctx, 2, OPC_SUB_U);
	vectorize(ctx, instr, &r_dst, 2, a_src, 0, r_src, 0);

	/* cmps.u.ge r, r, b */
	instr = instr_create(ctx, 2, OPC_CMPS_U);
	instr->cat2.condition = IR3_COND_GE;
	vectorize(ctx, instr, &r_dst, 2, r_src, 0, b_src, 0);

	if (type_uint(src_type)) {
		/* add.u dst, q, r */
		instr = instr_create(ctx, 2, OPC_ADD_U);
		vectorize(ctx, instr, premod_dst, 2, q_src, 0, r_src, 0);
	} else {
		/* add.u q, q, r */
		instr = instr_create(ctx, 2, OPC_ADD_U);
		vectorize(ctx, instr, &q_dst, 2, q_src, 0, r_src, 0);

		/* negate result based on the original arguments */
		if (is_const(a) && is_const(b))
			a = get_unconst(ctx, a);

		/* xor.b r, numerator, denominator */
		instr = instr_create(ctx, 2, OPC_XOR_B);
		vectorize(ctx, instr, &r_dst, 2, a, 0, b, 0);

		/* shr.b r, r, 31 */
		instr = instr_create(ctx, 2, OPC_SHR_B);
		vectorize(ctx, instr, &r_dst, 2, r_src, 0, &thirty_one, 0);

		/* absneg.s b, (neg)q */
		instr = instr_create(ctx, 2, OPC_ABSNEG_S);
		vectorize(ctx, instr, &b_dst, 1, q_src, IR3_REG_SNEG);

		/* sel.b dst, b, r, q */
		instr = instr_create(ctx, 3, OPC_SEL_B32);
		vectorize(ctx, instr, premod_dst, 3, b_src, 0, r_src, 0, q_src, 0);
	}

	if (t->tgsi_opc == TGSI_OPCODE_MOD || t->tgsi_opc == TGSI_OPCODE_UMOD) {
		/* The division result will have ended up in q. */

		if (is_rel_or_const(b))
			b = get_unconst(ctx, b);

		/* mull.u r, q, b */
		instr = instr_create(ctx, 2, OPC_MULL_U);
		vectorize(ctx, instr, &r_dst, 2, q_src, 0, b, 0);

		/* madsh.m16 r, q, b, r */
		instr = instr_create(ctx, 3, OPC_MADSH_M16);
		vectorize(ctx, instr, &r_dst, 3, q_src, 0, b, 0, r_src, 0);

		/* madsh.m16 r, b, q, r */
		instr = instr_create(ctx, 3, OPC_MADSH_M16);
		vectorize(ctx, instr, &r_dst, 3, b, 0, q_src, 0, r_src, 0);

		/* sub.u dst, a, r */
		instr = instr_create(ctx, 2, OPC_SUB_U);
		vectorize(ctx, instr, dst, 2, a, 0, r_src, 0);
	}

	put_dst(ctx, inst, dst);
}

/*
 * Handlers for TGSI instructions which do have 1:1 mapping to native
 * instructions:
 */

static void
instr_cat0(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	instr_create(ctx, 0, t->opc);
}

static void
instr_cat1(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	struct tgsi_src_register *src = &inst->Src[0].Register;

	/* NOTE: atomic start/end, rather than in create_mov() since
	 * create_mov() is used already w/in atomic sequences (and
	 * we aren't clever enough to deal with the nesting)
	 */
	instr_atomic_start(ctx);
	create_mov(ctx, dst, src);
	instr_atomic_end(ctx);
}

static void
instr_cat2(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;
	struct ir3_instruction *instr;
	unsigned src0_flags = 0, src1_flags = 0;

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_ABS:
		src0_flags = IR3_REG_FABS;
		break;
	case TGSI_OPCODE_IABS:
		src0_flags = IR3_REG_SABS;
		break;
	case TGSI_OPCODE_INEG:
		src0_flags = IR3_REG_SNEG;
		break;
	case TGSI_OPCODE_SUB:
		src1_flags = IR3_REG_FNEG;
		break;
	}

	switch (t->opc) {
	case OPC_ABSNEG_F:
	case OPC_ABSNEG_S:
	case OPC_CLZ_B:
	case OPC_CLZ_S:
	case OPC_SIGN_F:
	case OPC_FLOOR_F:
	case OPC_CEIL_F:
	case OPC_RNDNE_F:
	case OPC_RNDAZ_F:
	case OPC_TRUNC_F:
	case OPC_NOT_B:
	case OPC_BFREV_B:
	case OPC_SETRM:
	case OPC_CBITS_B:
		/* these only have one src reg */
		instr = instr_create(ctx, 2, t->opc);
		vectorize(ctx, instr, dst, 1, src0, src0_flags);
		break;
	default:
		if (is_const(src0) && is_const(src1))
			src0 = get_unconst(ctx, src0);

		instr = instr_create(ctx, 2, t->opc);
		vectorize(ctx, instr, dst, 2, src0, src0_flags,
				src1, src1_flags);
		break;
	}

	put_dst(ctx, inst, dst);
}

static void
instr_cat3(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;
	struct ir3_instruction *instr;

	/* in particular, can't handle const for src1 for cat3..
	 * for mad, we can swap first two src's if needed:
	 */
	if (is_rel_or_const(src1)) {
		if (is_mad(t->opc) && !is_rel_or_const(src0)) {
			struct tgsi_src_register *tmp;
			tmp = src0;
			src0 = src1;
			src1 = tmp;
		} else {
			src1 = get_unconst(ctx, src1);
		}
	}

	instr = instr_create(ctx, 3, t->opc);
	vectorize(ctx, instr, dst, 3, src0, 0, src1, 0,
			&inst->Src[2].Register, 0);
	put_dst(ctx, inst, dst);
}

static void
instr_cat4(const struct instr_translater *t,
		struct ir3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src = &inst->Src[0].Register;
	struct ir3_instruction *instr;
	unsigned i;

	/* seems like blob compiler avoids const as src.. */
	if (is_const(src))
		src = get_unconst(ctx, src);

	/* we need to replicate into each component: */
	for (i = 0; i < 4; i++) {
		if (dst->WriteMask & (1 << i)) {
			instr = instr_create(ctx, 4, t->opc);
			add_dst_reg(ctx, instr, dst, i);
			add_src_reg(ctx, instr, src, src->SwizzleX);
		}
	}

	put_dst(ctx, inst, dst);
}

static const struct instr_translater translaters[TGSI_OPCODE_LAST] = {
#define INSTR(n, f, ...) \
	[TGSI_OPCODE_ ## n] = { .fxn = (f), .tgsi_opc = TGSI_OPCODE_ ## n, ##__VA_ARGS__ }

	INSTR(MOV,          instr_cat1),
	INSTR(RCP,          instr_cat4, .opc = OPC_RCP),
	INSTR(RSQ,          instr_cat4, .opc = OPC_RSQ),
	INSTR(SQRT,         instr_cat4, .opc = OPC_SQRT),
	INSTR(MUL,          instr_cat2, .opc = OPC_MUL_F),
	INSTR(ADD,          instr_cat2, .opc = OPC_ADD_F),
	INSTR(SUB,          instr_cat2, .opc = OPC_ADD_F),
	INSTR(MIN,          instr_cat2, .opc = OPC_MIN_F),
	INSTR(MAX,          instr_cat2, .opc = OPC_MAX_F),
	INSTR(UADD,         instr_cat2, .opc = OPC_ADD_U),
	INSTR(IMIN,         instr_cat2, .opc = OPC_MIN_S),
	INSTR(UMIN,         instr_cat2, .opc = OPC_MIN_U),
	INSTR(IMAX,         instr_cat2, .opc = OPC_MAX_S),
	INSTR(UMAX,         instr_cat2, .opc = OPC_MAX_U),
	INSTR(AND,          instr_cat2, .opc = OPC_AND_B),
	INSTR(OR,           instr_cat2, .opc = OPC_OR_B),
	INSTR(NOT,          instr_cat2, .opc = OPC_NOT_B),
	INSTR(XOR,          instr_cat2, .opc = OPC_XOR_B),
	INSTR(UMUL,         trans_umul),
	INSTR(UMAD,         trans_umul),
	INSTR(UDIV,         trans_idiv),
	INSTR(IDIV,         trans_idiv),
	INSTR(MOD,          trans_idiv),
	INSTR(UMOD,         trans_idiv),
	INSTR(SHL,          instr_cat2, .opc = OPC_SHL_B),
	INSTR(USHR,         instr_cat2, .opc = OPC_SHR_B),
	INSTR(ISHR,         instr_cat2, .opc = OPC_ASHR_B),
	INSTR(IABS,         instr_cat2, .opc = OPC_ABSNEG_S),
	INSTR(INEG,         instr_cat2, .opc = OPC_ABSNEG_S),
	INSTR(AND,          instr_cat2, .opc = OPC_AND_B),
	INSTR(MAD,          instr_cat3, .opc = OPC_MAD_F32, .hopc = OPC_MAD_F16),
	INSTR(TRUNC,        instr_cat2, .opc = OPC_TRUNC_F),
	INSTR(CLAMP,        trans_clamp),
	INSTR(FLR,          instr_cat2, .opc = OPC_FLOOR_F),
	INSTR(ROUND,        instr_cat2, .opc = OPC_RNDNE_F),
	INSTR(SSG,          instr_cat2, .opc = OPC_SIGN_F),
	INSTR(CEIL,         instr_cat2, .opc = OPC_CEIL_F),
	INSTR(ARL,          trans_arl),
	INSTR(UARL,         trans_arl),
	INSTR(EX2,          instr_cat4, .opc = OPC_EXP2),
	INSTR(LG2,          instr_cat4, .opc = OPC_LOG2),
	INSTR(ABS,          instr_cat2, .opc = OPC_ABSNEG_F),
	INSTR(COS,          instr_cat4, .opc = OPC_COS),
	INSTR(SIN,          instr_cat4, .opc = OPC_SIN),
	INSTR(TEX,          trans_samp, .opc = OPC_SAM),
	INSTR(TXP,          trans_samp, .opc = OPC_SAM),
	INSTR(TXB,          trans_samp, .opc = OPC_SAMB),
	INSTR(TXB2,         trans_samp, .opc = OPC_SAMB),
	INSTR(TXL,          trans_samp, .opc = OPC_SAML),
	INSTR(TXD,          trans_samp, .opc = OPC_SAMGQ),
	INSTR(TXF,          trans_samp, .opc = OPC_ISAML),
	INSTR(TXQ,          trans_txq),
	INSTR(DDX,          trans_deriv, .opc = OPC_DSX),
	INSTR(DDY,          trans_deriv, .opc = OPC_DSY),
	INSTR(SGT,          trans_cmp),
	INSTR(SLT,          trans_cmp),
	INSTR(FSLT,         trans_cmp),
	INSTR(SGE,          trans_cmp),
	INSTR(FSGE,         trans_cmp),
	INSTR(SLE,          trans_cmp),
	INSTR(SNE,          trans_cmp),
	INSTR(FSNE,         trans_cmp),
	INSTR(SEQ,          trans_cmp),
	INSTR(FSEQ,         trans_cmp),
	INSTR(CMP,          trans_cmp),
	INSTR(USNE,         trans_icmp, .opc = OPC_CMPS_U),
	INSTR(USEQ,         trans_icmp, .opc = OPC_CMPS_U),
	INSTR(ISGE,         trans_icmp, .opc = OPC_CMPS_S),
	INSTR(USGE,         trans_icmp, .opc = OPC_CMPS_U),
	INSTR(ISLT,         trans_icmp, .opc = OPC_CMPS_S),
	INSTR(USLT,         trans_icmp, .opc = OPC_CMPS_U),
	INSTR(UCMP,         trans_ucmp),
	INSTR(ISSG,         trans_issg),
	INSTR(IF,           trans_if,   .opc = OPC_CMPS_F),
	INSTR(UIF,          trans_if,   .opc = OPC_CMPS_U),
	INSTR(ELSE,         trans_else),
	INSTR(ENDIF,        trans_endif),
	INSTR(END,          instr_cat0, .opc = OPC_END),
	INSTR(KILL,         trans_kill, .opc = OPC_KILL),
	INSTR(KILL_IF,      trans_killif, .opc = OPC_KILL),
	INSTR(I2F,          trans_cov),
	INSTR(U2F,          trans_cov),
	INSTR(F2I,          trans_cov),
	INSTR(F2U,          trans_cov),
};

static ir3_semantic
decl_semantic(const struct tgsi_declaration_semantic *sem)
{
	return ir3_semantic_name(sem->Name, sem->Index);
}

static struct ir3_instruction *
decl_in_frag_bary(struct ir3_compile_context *ctx, unsigned regid,
		unsigned j, unsigned inloc, bool use_ldlv)
{
	struct ir3_instruction *instr;
	struct ir3_register *src;

	if (use_ldlv) {
		/* ldlv.u32 dst, l[#inloc], 1 */
		instr = instr_create(ctx, 6, OPC_LDLV);
		instr->cat6.type = TYPE_U32;
		instr->cat6.iim_val = 1;
		ir3_reg_create(instr, regid, 0);   /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = inloc;
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 1;

		return instr;
	}

	/* bary.f dst, #inloc, r0.x */
	instr = instr_create(ctx, 2, OPC_BARY_F);
	ir3_reg_create(instr, regid, 0);   /* dummy dst */
	ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = inloc;
	src = ir3_reg_create(instr, 0, IR3_REG_SSA);
	src->wrmask = 0x3;
	src->instr = ctx->frag_pos;

	return instr;
}

/* TGSI_SEMANTIC_POSITION
 * """"""""""""""""""""""
 *
 * For fragment shaders, TGSI_SEMANTIC_POSITION is used to indicate that
 * fragment shader input contains the fragment's window position.  The X
 * component starts at zero and always increases from left to right.
 * The Y component starts at zero and always increases but Y=0 may either
 * indicate the top of the window or the bottom depending on the fragment
 * coordinate origin convention (see TGSI_PROPERTY_FS_COORD_ORIGIN).
 * The Z coordinate ranges from 0 to 1 to represent depth from the front
 * to the back of the Z buffer.  The W component contains the reciprocol
 * of the interpolated vertex position W component.
 */
static struct ir3_instruction *
decl_in_frag_coord(struct ir3_compile_context *ctx, unsigned regid,
		unsigned j)
{
	struct ir3_instruction *instr, *src;

	compile_assert(ctx, !ctx->frag_coord[j]);

	ctx->frag_coord[j] = create_input(ctx->block, NULL, 0);


	switch (j) {
	case 0: /* .x */
	case 1: /* .y */
		/* for frag_coord, we get unsigned values.. we need
		 * to subtract (integer) 8 and divide by 16 (right-
		 * shift by 4) then convert to float:
		 */

		/* add.s tmp, src, -8 */
		instr = instr_create(ctx, 2, OPC_ADD_S);
		ir3_reg_create(instr, regid, 0);    /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = ctx->frag_coord[j];
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = -8;
		src = instr;

		/* shr.b tmp, tmp, 4 */
		instr = instr_create(ctx, 2, OPC_SHR_B);
		ir3_reg_create(instr, regid, 0);    /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = src;
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 4;
		src = instr;

		/* mov.u32f32 dst, tmp */
		instr = instr_create(ctx, 1, 0);
		instr->cat1.src_type = TYPE_U32;
		instr->cat1.dst_type = TYPE_F32;
		ir3_reg_create(instr, regid, 0);    /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = src;

		break;
	case 2: /* .z */
	case 3: /* .w */
		/* seems that we can use these as-is: */
		instr = ctx->frag_coord[j];
		break;
	default:
		compile_error(ctx, "invalid channel\n");
		instr = create_immed(ctx, 0.0);
		break;
	}

	return instr;
}

/* TGSI_SEMANTIC_FACE
 * """"""""""""""""""
 *
 * This label applies to fragment shader inputs only and indicates that
 * the register contains front/back-face information of the form (F, 0,
 * 0, 1).  The first component will be positive when the fragment belongs
 * to a front-facing polygon, and negative when the fragment belongs to a
 * back-facing polygon.
 */
static struct ir3_instruction *
decl_in_frag_face(struct ir3_compile_context *ctx, unsigned regid,
		unsigned j)
{
	struct ir3_instruction *instr, *src;

	switch (j) {
	case 0: /* .x */
		compile_assert(ctx, !ctx->frag_face);

		ctx->frag_face = create_input(ctx->block, NULL, 0);

		/* for faceness, we always get -1 or 0 (int).. but TGSI expects
		 * positive vs negative float.. and piglit further seems to
		 * expect -1.0 or 1.0:
		 *
		 *    mul.s tmp, hr0.x, 2
		 *    add.s tmp, tmp, 1
		 *    mov.s16f32, dst, tmp
		 *
		 */

		instr = instr_create(ctx, 2, OPC_MUL_S);
		ir3_reg_create(instr, regid, 0);    /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = ctx->frag_face;
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 2;
		src = instr;

		instr = instr_create(ctx, 2, OPC_ADD_S);
		ir3_reg_create(instr, regid, 0);    /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = src;
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = 1;
		src = instr;

		instr = instr_create(ctx, 1, 0); /* mov */
		instr->cat1.src_type = TYPE_S32;
		instr->cat1.dst_type = TYPE_F32;
		ir3_reg_create(instr, regid, 0);    /* dummy dst */
		ir3_reg_create(instr, 0, IR3_REG_SSA)->instr = src;

		break;
	case 1: /* .y */
	case 2: /* .z */
		instr = create_immed(ctx, 0.0);
		break;
	case 3: /* .w */
		instr = create_immed(ctx, 1.0);
		break;
	default:
		compile_error(ctx, "invalid channel\n");
		instr = create_immed(ctx, 0.0);
		break;
	}

	return instr;
}

static void
decl_in(struct ir3_compile_context *ctx, struct tgsi_full_declaration *decl)
{
	struct ir3_shader_variant *so = ctx->so;
	unsigned name = decl->Semantic.Name;
	unsigned i;

	/* I don't think we should get frag shader input without
	 * semantic info?  Otherwise how do inputs get linked to
	 * vert outputs?
	 */
	compile_assert(ctx, (ctx->type == TGSI_PROCESSOR_VERTEX) ||
			decl->Declaration.Semantic);

	for (i = decl->Range.First; i <= decl->Range.Last; i++) {
		unsigned n = so->inputs_count++;
		unsigned r = regid(i, 0);
		unsigned ncomp, j;

		/* we'll figure out the actual components used after scheduling */
		ncomp = 4;

		DBG("decl in -> r%d", i);

		compile_assert(ctx, n < ARRAY_SIZE(so->inputs));

		so->inputs[n].semantic = decl_semantic(&decl->Semantic);
		so->inputs[n].compmask = (1 << ncomp) - 1;
		so->inputs[n].regid = r;
		so->inputs[n].inloc = ctx->next_inloc;
		so->inputs[n].interpolate = decl->Interp.Interpolate;

		for (j = 0; j < ncomp; j++) {
			struct ir3_instruction *instr = NULL;

			if (ctx->type == TGSI_PROCESSOR_FRAGMENT) {
				/* for fragment shaders, POSITION and FACE are handled
				 * specially, not using normal varying / bary.f
				 */
				if (name == TGSI_SEMANTIC_POSITION) {
					so->inputs[n].bary = false;
					so->frag_coord = true;
					instr = decl_in_frag_coord(ctx, r + j, j);
				} else if (name == TGSI_SEMANTIC_FACE) {
					so->inputs[n].bary = false;
					so->frag_face = true;
					instr = decl_in_frag_face(ctx, r + j, j);
				} else {
					bool use_ldlv = false;

					/* if no interpolation given, pick based on
					 * semantic:
					 */
					if (!decl->Declaration.Interpolate) {
						switch (decl->Semantic.Name) {
						case TGSI_SEMANTIC_COLOR:
							so->inputs[n].interpolate =
									TGSI_INTERPOLATE_COLOR;
							break;
						default:
							so->inputs[n].interpolate =
									TGSI_INTERPOLATE_LINEAR;
						}
					}

					if (ctx->flat_bypass) {
						switch (so->inputs[n].interpolate) {
						case TGSI_INTERPOLATE_COLOR:
							if (!ctx->so->key.rasterflat)
								break;
							/* fallthrough */
						case TGSI_INTERPOLATE_CONSTANT:
							use_ldlv = true;
							break;
						}
					}

					so->inputs[n].bary = true;

					instr = decl_in_frag_bary(ctx, r + j, j,
							so->inputs[n].inloc + j - 8, use_ldlv);
				}
			} else {
				instr = create_input(ctx->block, NULL, (i * 4) + j);
			}

			ctx->block->inputs[(i * 4) + j] = instr;
		}

		if (so->inputs[n].bary || (ctx->type == TGSI_PROCESSOR_VERTEX)) {
			ctx->next_inloc += ncomp;
			so->total_in += ncomp;
		}
	}
}

static void
decl_sv(struct ir3_compile_context *ctx, struct tgsi_full_declaration *decl)
{
	struct ir3_shader_variant *so = ctx->so;
	unsigned r = regid(so->inputs_count, 0);
	unsigned n = so->inputs_count++;

	DBG("decl sv -> r%d", n);

	compile_assert(ctx, n < ARRAY_SIZE(so->inputs));
	compile_assert(ctx, decl->Range.First < ARRAY_SIZE(ctx->sysval_semantics));

	ctx->sysval_semantics[decl->Range.First] = decl->Semantic.Name;
	so->inputs[n].semantic = decl_semantic(&decl->Semantic);
	so->inputs[n].compmask = 1;
	so->inputs[n].regid = r;
	so->inputs[n].inloc = ctx->next_inloc;
	so->inputs[n].interpolate = TGSI_INTERPOLATE_CONSTANT;

	struct ir3_instruction *instr = NULL;

	switch (decl->Semantic.Name) {
	case TGSI_SEMANTIC_VERTEXID_NOBASE:
		ctx->vertex_id = instr = create_input(ctx->block, NULL, r);
		break;
	case TGSI_SEMANTIC_BASEVERTEX:
		ctx->basevertex = instr = instr_create(ctx, 1, 0);
		instr->cat1.src_type = get_stype(ctx);
		instr->cat1.dst_type = get_stype(ctx);
		ir3_reg_create(instr, 0, 0);
		ir3_reg_create(instr, regid(so->first_driver_param + 4, 0),
					   IR3_REG_CONST);
		break;
	case TGSI_SEMANTIC_INSTANCEID:
		ctx->instance_id = instr = create_input(ctx->block, NULL, r);
		break;
	default:
		compile_error(ctx, "Unknown semantic: %s\n",
					  tgsi_semantic_names[decl->Semantic.Name]);
	}

	ctx->block->inputs[r] = instr;
	ctx->next_inloc++;
	so->total_in++;
}

static void
decl_out(struct ir3_compile_context *ctx, struct tgsi_full_declaration *decl)
{
	struct ir3_shader_variant *so = ctx->so;
	unsigned comp = 0;
	unsigned name = decl->Semantic.Name;
	unsigned i;

	compile_assert(ctx, decl->Declaration.Semantic);

	DBG("decl out[%d] -> r%d", name, decl->Range.First);

	if (ctx->type == TGSI_PROCESSOR_VERTEX) {
		switch (name) {
		case TGSI_SEMANTIC_POSITION:
			so->writes_pos = true;
			break;
		case TGSI_SEMANTIC_PSIZE:
			so->writes_psize = true;
			break;
		case TGSI_SEMANTIC_COLOR:
		case TGSI_SEMANTIC_BCOLOR:
		case TGSI_SEMANTIC_GENERIC:
		case TGSI_SEMANTIC_FOG:
		case TGSI_SEMANTIC_TEXCOORD:
			break;
		default:
			compile_error(ctx, "unknown VS semantic name: %s\n",
					tgsi_semantic_names[name]);
		}
	} else {
		switch (name) {
		case TGSI_SEMANTIC_POSITION:
			comp = 2;  /* tgsi will write to .z component */
			so->writes_pos = true;
			break;
		case TGSI_SEMANTIC_COLOR:
			break;
		default:
			compile_error(ctx, "unknown FS semantic name: %s\n",
					tgsi_semantic_names[name]);
		}
	}

	for (i = decl->Range.First; i <= decl->Range.Last; i++) {
		unsigned n = so->outputs_count++;
		unsigned ncomp, j;

		ncomp = 4;

		compile_assert(ctx, n < ARRAY_SIZE(so->outputs));

		so->outputs[n].semantic = decl_semantic(&decl->Semantic);
		so->outputs[n].regid = regid(i, comp);

		/* avoid undefined outputs, stick a dummy mov from imm{0.0},
		 * which if the output is actually assigned will be over-
		 * written
		 */
		for (j = 0; j < ncomp; j++)
			ctx->block->outputs[(i * 4) + j] = create_immed(ctx, 0.0);
	}
}

/* from TGSI perspective, we actually have inputs.  But most of the "inputs"
 * for a fragment shader are just bary.f instructions.  The *actual* inputs
 * from the hw perspective are the frag_pos and optionally frag_coord and
 * frag_face.
 */
static void
fixup_frag_inputs(struct ir3_compile_context *ctx)
{
	struct ir3_shader_variant *so = ctx->so;
	struct ir3_block *block = ctx->block;
	struct ir3_instruction **inputs;
	struct ir3_instruction *instr;
	int n, regid = 0;

	block->ninputs = 0;

	n  = 4;  /* always have frag_pos */
	n += COND(so->frag_face, 4);
	n += COND(so->frag_coord, 4);

	inputs = ir3_alloc(ctx->ir, n * (sizeof(struct ir3_instruction *)));

	if (so->frag_face) {
		/* this ultimately gets assigned to hr0.x so doesn't conflict
		 * with frag_coord/frag_pos..
		 */
		inputs[block->ninputs++] = ctx->frag_face;
		ctx->frag_face->regs[0]->num = 0;

		/* remaining channels not used, but let's avoid confusing
		 * other parts that expect inputs to come in groups of vec4
		 */
		inputs[block->ninputs++] = NULL;
		inputs[block->ninputs++] = NULL;
		inputs[block->ninputs++] = NULL;
	}

	/* since we don't know where to set the regid for frag_coord,
	 * we have to use r0.x for it.  But we don't want to *always*
	 * use r1.x for frag_pos as that could increase the register
	 * footprint on simple shaders:
	 */
	if (so->frag_coord) {
		ctx->frag_coord[0]->regs[0]->num = regid++;
		ctx->frag_coord[1]->regs[0]->num = regid++;
		ctx->frag_coord[2]->regs[0]->num = regid++;
		ctx->frag_coord[3]->regs[0]->num = regid++;

		inputs[block->ninputs++] = ctx->frag_coord[0];
		inputs[block->ninputs++] = ctx->frag_coord[1];
		inputs[block->ninputs++] = ctx->frag_coord[2];
		inputs[block->ninputs++] = ctx->frag_coord[3];
	}

	/* we always have frag_pos: */
	so->pos_regid = regid;

	/* r0.x */
	instr = create_input(block, NULL, block->ninputs);
	instr->regs[0]->num = regid++;
	inputs[block->ninputs++] = instr;
	ctx->frag_pos->regs[1]->instr = instr;

	/* r0.y */
	instr = create_input(block, NULL, block->ninputs);
	instr->regs[0]->num = regid++;
	inputs[block->ninputs++] = instr;
	ctx->frag_pos->regs[2]->instr = instr;

	block->inputs = inputs;
}

static void
compile_instructions(struct ir3_compile_context *ctx)
{
	push_block(ctx);

	/* for fragment shader, we have a single input register (usually
	 * r0.xy) which is used as the base for bary.f varying fetch instrs:
	 */
	if (ctx->type == TGSI_PROCESSOR_FRAGMENT) {
		struct ir3_instruction *instr;
		instr = ir3_instr_create(ctx->block, -1, OPC_META_FI);
		ir3_reg_create(instr, 0, 0);
		ir3_reg_create(instr, 0, IR3_REG_SSA);    /* r0.x */
		ir3_reg_create(instr, 0, IR3_REG_SSA);    /* r0.y */
		ctx->frag_pos = instr;
	}

	while (!tgsi_parse_end_of_tokens(&ctx->parser)) {
		tgsi_parse_token(&ctx->parser);

		switch (ctx->parser.FullToken.Token.Type) {
		case TGSI_TOKEN_TYPE_DECLARATION: {
			struct tgsi_full_declaration *decl =
					&ctx->parser.FullToken.FullDeclaration;
			unsigned file = decl->Declaration.File;
			if (file == TGSI_FILE_OUTPUT) {
				decl_out(ctx, decl);
			} else if (file == TGSI_FILE_INPUT) {
				decl_in(ctx, decl);
			} else if (decl->Declaration.File == TGSI_FILE_SYSTEM_VALUE) {
				decl_sv(ctx, decl);
			}

			if ((file != TGSI_FILE_CONSTANT) && decl->Declaration.Array) {
				int aid = decl->Array.ArrayID + ctx->array_offsets[file];

				compile_assert(ctx, aid < ARRAY_SIZE(ctx->array));

				/* legacy ArrayID==0 stuff probably isn't going to work
				 * well (and is at least untested).. let's just scream:
				 */
				compile_assert(ctx, aid != 0);

				ctx->array[aid].first = decl->Range.First;
				ctx->array[aid].last  = decl->Range.Last;
			}
			break;
		}
		case TGSI_TOKEN_TYPE_IMMEDIATE: {
			/* TODO: if we know the immediate is small enough, and only
			 * used with instructions that can embed an immediate, we
			 * can skip this:
			 */
			struct tgsi_full_immediate *imm =
					&ctx->parser.FullToken.FullImmediate;
			unsigned n = ctx->so->immediates_count++;
			compile_assert(ctx, n < ARRAY_SIZE(ctx->so->immediates));
			memcpy(ctx->so->immediates[n].val, imm->u, 16);
			break;
		}
		case TGSI_TOKEN_TYPE_INSTRUCTION: {
			struct tgsi_full_instruction *inst =
					&ctx->parser.FullToken.FullInstruction;
			unsigned opc = inst->Instruction.Opcode;
			const struct instr_translater *t = &translaters[opc];

			if (t->fxn) {
				t->fxn(t, ctx, inst);
				ctx->num_internal_temps = 0;

				compile_assert(ctx, !ctx->using_tmp_dst);
			} else {
				compile_error(ctx, "unknown TGSI opc: %s\n",
						tgsi_get_opcode_name(opc));
			}

			switch (inst->Instruction.Saturate) {
			case TGSI_SAT_ZERO_ONE:
				create_clamp_imm(ctx, &inst->Dst[0].Register,
						fui(0.0), fui(1.0));
				break;
			case TGSI_SAT_MINUS_PLUS_ONE:
				create_clamp_imm(ctx, &inst->Dst[0].Register,
						fui(-1.0), fui(1.0));
				break;
			}

			instr_finish(ctx);

			break;
		}
		case TGSI_TOKEN_TYPE_PROPERTY: {
			struct tgsi_full_property *prop =
				&ctx->parser.FullToken.FullProperty;
			switch (prop->Property.PropertyName) {
			case TGSI_PROPERTY_FS_COLOR0_WRITES_ALL_CBUFS:
				ctx->so->color0_mrt = !!prop->u[0].Data;
				break;
			}
		}
		default:
			break;
		}
	}
}

static void
compile_dump(struct ir3_compile_context *ctx)
{
	const char *name = (ctx->so->type == SHADER_VERTEX) ? "vert" : "frag";
	static unsigned n = 0;
	char fname[16];
	FILE *f;
	snprintf(fname, sizeof(fname), "%s-%04u.dot", name, n++);
	f = fopen(fname, "w");
	if (!f)
		return;
	ir3_block_depth(ctx->block);
	ir3_dump(ctx->ir, name, ctx->block, f);
	fclose(f);
}

int
ir3_compile_shader(struct ir3_shader_variant *so,
		const struct tgsi_token *tokens, struct ir3_shader_key key,
		bool cp)
{
	struct ir3_compile_context ctx;
	struct ir3_block *block;
	struct ir3_instruction **inputs;
	unsigned i, j, actual_in;
	int ret = 0, max_bary;

	assert(!so->ir);

	so->ir = ir3_create();

	assert(so->ir);

	if (compile_init(&ctx, so, tokens) != TGSI_PARSE_OK) {
		DBG("INIT failed!");
		ret = -1;
		goto out;
	}

	/* for now, until the edge cases are worked out: */
	if (ctx.info.indirect_files_written & (FM(TEMPORARY) | FM(INPUT) | FM(OUTPUT)))
		cp = false;

	compile_instructions(&ctx);

	block = ctx.block;
	so->ir->block = block;

	/* keep track of the inputs from TGSI perspective.. */
	inputs = block->inputs;

	/* but fixup actual inputs for frag shader: */
	if (ctx.type == TGSI_PROCESSOR_FRAGMENT)
		fixup_frag_inputs(&ctx);

	/* at this point, for binning pass, throw away unneeded outputs: */
	if (key.binning_pass) {
		for (i = 0, j = 0; i < so->outputs_count; i++) {
			unsigned name = sem2name(so->outputs[i].semantic);
			unsigned idx = sem2idx(so->outputs[i].semantic);

			/* throw away everything but first position/psize */
			if ((idx == 0) && ((name == TGSI_SEMANTIC_POSITION) ||
					(name == TGSI_SEMANTIC_PSIZE))) {
				if (i != j) {
					so->outputs[j] = so->outputs[i];
					block->outputs[(j*4)+0] = block->outputs[(i*4)+0];
					block->outputs[(j*4)+1] = block->outputs[(i*4)+1];
					block->outputs[(j*4)+2] = block->outputs[(i*4)+2];
					block->outputs[(j*4)+3] = block->outputs[(i*4)+3];
				}
				j++;
			}
		}
		so->outputs_count = j;
		block->noutputs = j * 4;
	}

	/* if we want half-precision outputs, mark the output registers
	 * as half:
	 */
	if (key.half_precision) {
		for (i = 0; i < block->noutputs; i++) {
			if (!block->outputs[i])
				continue;
			block->outputs[i]->regs[0]->flags |= IR3_REG_HALF;
		}
	}

	/* at this point, we want the kill's in the outputs array too,
	 * so that they get scheduled (since they have no dst).. we've
	 * already ensured that the array is big enough in push_block():
	 */
	if (ctx.type == TGSI_PROCESSOR_FRAGMENT) {
		for (i = 0; i < ctx.kill_count; i++)
			block->outputs[block->noutputs++] = ctx.kill[i];
	}

	if (fd_mesa_debug & FD_DBG_OPTDUMP)
		compile_dump(&ctx);

	ret = ir3_block_flatten(block);
	if (ret < 0) {
		DBG("FLATTEN failed!");
		goto out;
	}
	if ((ret > 0) && (fd_mesa_debug & FD_DBG_OPTDUMP))
		compile_dump(&ctx);

	if (fd_mesa_debug & FD_DBG_OPTMSGS) {
		printf("BEFORE CP:\n");
		ir3_dump_instr_list(block->head);
	}

	ir3_block_depth(block);

	/* First remove all the extra mov's (which we could skip if the
	 * front-end was clever enough not to insert them in the first
	 * place).  Then figure out left/right neighbors, re-inserting
	 * extra mov's when needed to avoid conflicts.
	 */
	if (cp && !(fd_mesa_debug & FD_DBG_NOCP))
		ir3_block_cp(block);

	if (fd_mesa_debug & FD_DBG_OPTMSGS) {
		printf("BEFORE GROUPING:\n");
		ir3_dump_instr_list(block->head);
	}

	/* Group left/right neighbors, inserting mov's where needed to
	 * solve conflicts:
	 */
	ir3_block_group(block);

	if (fd_mesa_debug & FD_DBG_OPTDUMP)
		compile_dump(&ctx);

	ir3_block_depth(block);

	if (fd_mesa_debug & FD_DBG_OPTMSGS) {
		printf("AFTER DEPTH:\n");
		ir3_dump_instr_list(block->head);
	}

	ret = ir3_block_sched(block);
	if (ret) {
		DBG("SCHED failed!");
		goto out;
	}

	if (fd_mesa_debug & FD_DBG_OPTMSGS) {
		printf("AFTER SCHED:\n");
		ir3_dump_instr_list(block->head);
	}

	ret = ir3_block_ra(block, so->type, so->frag_coord, so->frag_face);
	if (ret) {
		DBG("RA failed!");
		goto out;
	}

	if (fd_mesa_debug & FD_DBG_OPTMSGS) {
		printf("AFTER RA:\n");
		ir3_dump_instr_list(block->head);
	}

	ir3_block_legalize(block, &so->has_samp, &max_bary);

	/* fixup input/outputs: */
	for (i = 0; i < so->outputs_count; i++) {
		so->outputs[i].regid = block->outputs[i*4]->regs[0]->num;
		/* preserve hack for depth output.. tgsi writes depth to .z,
		 * but what we give the hw is the scalar register:
		 */
		if ((ctx.type == TGSI_PROCESSOR_FRAGMENT) &&
			(sem2name(so->outputs[i].semantic) == TGSI_SEMANTIC_POSITION))
			so->outputs[i].regid += 2;
	}
	/* Note that some or all channels of an input may be unused: */
	actual_in = 0;
	for (i = 0; i < so->inputs_count; i++) {
		unsigned j, regid = ~0, compmask = 0;
		so->inputs[i].ncomp = 0;
		for (j = 0; j < 4; j++) {
			struct ir3_instruction *in = inputs[(i*4) + j];
			if (in) {
				compmask |= (1 << j);
				regid = in->regs[0]->num - j;
				actual_in++;
				so->inputs[i].ncomp++;
			}
		}
		so->inputs[i].regid = regid;
		so->inputs[i].compmask = compmask;
	}

	/* fragment shader always gets full vec4's even if it doesn't
	 * fetch all components, but vertex shader we need to update
	 * with the actual number of components fetch, otherwise thing
	 * will hang due to mismaptch between VFD_DECODE's and
	 * TOTALATTRTOVS
	 */
	if (so->type == SHADER_VERTEX)
		so->total_in = actual_in;
	else
		so->total_in = align(max_bary + 1, 4);

out:
	if (ret) {
		ir3_destroy(so->ir);
		so->ir = NULL;
	}
	compile_free(&ctx);

	return ret;
}
