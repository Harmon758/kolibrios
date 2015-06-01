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
#include "tgsi/tgsi_parse.h"
#include "tgsi/tgsi_ureg.h"
#include "tgsi/tgsi_info.h"
#include "tgsi/tgsi_strings.h"
#include "tgsi/tgsi_dump.h"
#include "tgsi/tgsi_scan.h"

#include "fd3_compiler.h"
#include "fd3_program.h"
#include "fd3_util.h"

#include "instr-a3xx.h"
#include "ir-a3xx.h"

/* ************************************************************************* */
/* split the out or find some helper to use.. like main/bitset.h.. */

#define MAX_REG 256

typedef uint8_t regmask_t[2 * MAX_REG / 8];

static unsigned regmask_idx(struct ir3_register *reg)
{
	unsigned num = reg->num;
	assert(num < MAX_REG);
	if (reg->flags & IR3_REG_HALF)
		num += MAX_REG;
	return num;
}

static void regmask_set(regmask_t regmask, struct ir3_register *reg,
		unsigned wrmask)
{
	unsigned i;
	for (i = 0; i < 4; i++) {
		if (wrmask & (1 << i)) {
			unsigned idx = regmask_idx(reg) + i;
			regmask[idx / 8] |= 1 << (idx % 8);
		}
	}
}

static unsigned regmask_get(regmask_t regmask, struct ir3_register *reg)
{
	unsigned idx = regmask_idx(reg);
	return regmask[idx / 8] & (1 << (idx % 8));
}

/* ************************************************************************* */

struct fd3_compile_context {
	const struct tgsi_token *tokens;
	struct ir3_shader *ir;
	struct fd3_shader_stateobj *so;

	struct tgsi_parse_context parser;
	unsigned type;

	struct tgsi_shader_info info;

	/* last input dst (for setting (ei) flag): */
	struct ir3_register *last_input;

	unsigned next_inloc;
	unsigned num_internal_temps;
	struct tgsi_src_register internal_temps[6];

	/* track registers which need to synchronize w/ "complex alu" cat3
	 * instruction pipeline:
	 */
	regmask_t needs_ss;

	/* track registers which need to synchronize with texture fetch
	 * pipeline:
	 */
	regmask_t needs_sy;

	/* inputs start at r0, temporaries start after last input, and
	 * outputs start after last temporary.
	 *
	 * We could be more clever, because this is not a hw restriction,
	 * but probably best just to implement an optimizing pass to
	 * reduce the # of registers used and get rid of redundant mov's
	 * (to output register).
	 */
	unsigned base_reg[TGSI_FILE_COUNT];

	/* idx/slot for last compiler generated immediate */
	unsigned immediate_idx;

	/* stack of branch instructions that start (potentially nested)
	 * branch instructions, so that we can fix up the branch targets
	 * so that we can fix up the branch target on the corresponding
	 * END instruction
	 */
	struct ir3_instruction *branch[16];
	unsigned int branch_count;

	/* used when dst is same as one of the src, to avoid overwriting a
	 * src element before the remaining scalar instructions that make
	 * up the vector operation
	 */
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
};


static void vectorize(struct fd3_compile_context *ctx,
		struct ir3_instruction *instr, struct tgsi_dst_register *dst,
		int nsrcs, ...);
static void create_mov(struct fd3_compile_context *ctx,
		struct tgsi_dst_register *dst, struct tgsi_src_register *src);

static unsigned
compile_init(struct fd3_compile_context *ctx, struct fd3_shader_stateobj *so,
		const struct tgsi_token *tokens)
{
	unsigned ret;

	ctx->tokens = tokens;
	ctx->ir = so->ir;
	ctx->so = so;
	ctx->last_input = NULL;
	ctx->next_inloc = 8;
	ctx->num_internal_temps = 0;
	ctx->branch_count = 0;

	memset(ctx->needs_ss, 0, sizeof(ctx->needs_ss));
	memset(ctx->needs_sy, 0, sizeof(ctx->needs_sy));
	memset(ctx->base_reg, 0, sizeof(ctx->base_reg));

	tgsi_scan_shader(tokens, &ctx->info);

	/* Immediates go after constants: */
	ctx->base_reg[TGSI_FILE_CONSTANT]  = 0;
	ctx->base_reg[TGSI_FILE_IMMEDIATE] =
			ctx->info.file_max[TGSI_FILE_CONSTANT] + 1;

	/* Temporaries after outputs after inputs: */
	ctx->base_reg[TGSI_FILE_INPUT]     = 0;
	ctx->base_reg[TGSI_FILE_OUTPUT]    =
			ctx->info.file_max[TGSI_FILE_INPUT] + 1;
	ctx->base_reg[TGSI_FILE_TEMPORARY] =
			ctx->info.file_max[TGSI_FILE_INPUT] + 1 +
			ctx->info.file_max[TGSI_FILE_OUTPUT] + 1;

	so->first_immediate = ctx->base_reg[TGSI_FILE_IMMEDIATE];
	ctx->immediate_idx = 4 * (ctx->info.file_max[TGSI_FILE_CONSTANT] + 1 +
			ctx->info.file_max[TGSI_FILE_IMMEDIATE] + 1);

	ret = tgsi_parse_init(&ctx->parser, tokens);
	if (ret != TGSI_PARSE_OK)
		return ret;

	ctx->type = ctx->parser.FullHeader.Processor.Processor;

	return ret;
}

static void
compile_error(struct fd3_compile_context *ctx, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	_debug_vprintf(format, ap);
	va_end(ap);
	tgsi_dump(ctx->tokens, 0);
	assert(0);
}

#define compile_assert(ctx, cond) do { \
		if (!(cond)) compile_error((ctx), "failed assert: "#cond"\n"); \
	} while (0)

static void
compile_free(struct fd3_compile_context *ctx)
{
	tgsi_parse_free(&ctx->parser);
}

struct instr_translater {
	void (*fxn)(const struct instr_translater *t,
			struct fd3_compile_context *ctx,
			struct tgsi_full_instruction *inst);
	unsigned tgsi_opc;
	opc_t opc;
	opc_t hopc;    /* opc to use for half_precision mode, if different */
	unsigned arg;
};

static unsigned
src_flags(struct fd3_compile_context *ctx, struct ir3_register *reg)
{
	unsigned flags = 0;

	if (regmask_get(ctx->needs_ss, reg)) {
		flags |= IR3_INSTR_SS;
		memset(ctx->needs_ss, 0, sizeof(ctx->needs_ss));
	}

	if (regmask_get(ctx->needs_sy, reg)) {
		flags |= IR3_INSTR_SY;
		memset(ctx->needs_sy, 0, sizeof(ctx->needs_sy));
	}

	return flags;
}

static struct ir3_register *
add_dst_reg(struct fd3_compile_context *ctx, struct ir3_instruction *instr,
		const struct tgsi_dst_register *dst, unsigned chan)
{
	unsigned flags = 0, num = 0;

	switch (dst->File) {
	case TGSI_FILE_OUTPUT:
	case TGSI_FILE_TEMPORARY:
		num = dst->Index + ctx->base_reg[dst->File];
		break;
	default:
		compile_error(ctx, "unsupported dst register file: %s\n",
			tgsi_file_name(dst->File));
		break;
	}

	if (ctx->so->half_precision)
		flags |= IR3_REG_HALF;

	return ir3_reg_create(instr, regid(num, chan), flags);
}

static struct ir3_register *
add_src_reg(struct fd3_compile_context *ctx, struct ir3_instruction *instr,
		const struct tgsi_src_register *src, unsigned chan)
{
	unsigned flags = 0, num = 0;
	struct ir3_register *reg;

	switch (src->File) {
	case TGSI_FILE_IMMEDIATE:
		/* TODO if possible, use actual immediate instead of const.. but
		 * TGSI has vec4 immediates, we can only embed scalar (of limited
		 * size, depending on instruction..)
		 */
	case TGSI_FILE_CONSTANT:
		flags |= IR3_REG_CONST;
		num = src->Index + ctx->base_reg[src->File];
		break;
	case TGSI_FILE_OUTPUT:
		/* NOTE: we should only end up w/ OUTPUT file for things like
		 * clamp()'ing saturated dst instructions
		 */
	case TGSI_FILE_INPUT:
	case TGSI_FILE_TEMPORARY:
		num = src->Index + ctx->base_reg[src->File];
		break;
	default:
		compile_error(ctx, "unsupported src register file: %s\n",
			tgsi_file_name(src->File));
		break;
	}

	if (src->Absolute)
		flags |= IR3_REG_ABS;
	if (src->Negate)
		flags |= IR3_REG_NEGATE;
	if (ctx->so->half_precision)
		flags |= IR3_REG_HALF;

	reg = ir3_reg_create(instr, regid(num, chan), flags);

	instr->flags |= src_flags(ctx, reg);

	return reg;
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
get_internal_temp(struct fd3_compile_context *ctx,
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

/* same as get_internal_temp, but w/ src.xxxx (for instructions that
 * replicate their results)
 */
static struct tgsi_src_register *
get_internal_temp_repl(struct fd3_compile_context *ctx,
		struct tgsi_dst_register *tmp_dst)
{
	struct tgsi_src_register *tmp_src =
			get_internal_temp(ctx, tmp_dst);
	tmp_src->SwizzleX = tmp_src->SwizzleY =
		tmp_src->SwizzleZ = tmp_src->SwizzleW = TGSI_SWIZZLE_X;
	return tmp_src;
}

static inline bool
is_const(struct tgsi_src_register *src)
{
	return (src->File == TGSI_FILE_CONSTANT) ||
			(src->File == TGSI_FILE_IMMEDIATE);
}

static type_t
get_ftype(struct fd3_compile_context *ctx)
{
	return ctx->so->half_precision ? TYPE_F16 : TYPE_F32;
}

static type_t
get_utype(struct fd3_compile_context *ctx)
{
	return ctx->so->half_precision ? TYPE_U16 : TYPE_U32;
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
get_unconst(struct fd3_compile_context *ctx, struct tgsi_src_register *src)
{
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;

	compile_assert(ctx, is_const(src));

	tmp_src = get_internal_temp(ctx, &tmp_dst);

	create_mov(ctx, &tmp_dst, src);

	return tmp_src;
}

static void
get_immediate(struct fd3_compile_context *ctx,
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
create_mov(struct fd3_compile_context *ctx, struct tgsi_dst_register *dst,
		struct tgsi_src_register *src)
{
	type_t type_mov = get_ftype(ctx);
	unsigned i;

	for (i = 0; i < 4; i++) {
		/* move to destination: */
		if (dst->WriteMask & (1 << i)) {
			struct ir3_instruction *instr =
					ir3_instr_create(ctx->ir, 1, 0);
			instr->cat1.src_type = type_mov;
			instr->cat1.dst_type = type_mov;
			add_dst_reg(ctx, instr, dst, i);
			add_src_reg(ctx, instr, src, src_swiz(src, i));
		} else {
			ir3_instr_create(ctx->ir, 0, OPC_NOP);
		}
	}
}

static void
create_clamp(struct fd3_compile_context *ctx, struct tgsi_dst_register *dst,
		struct tgsi_src_register *minval, struct tgsi_src_register *maxval)
{
	struct ir3_instruction *instr;
	struct tgsi_src_register src;

	src_from_dst(&src, dst);

	instr = ir3_instr_create(ctx->ir, 2, OPC_MAX_F);
	vectorize(ctx, instr, dst, 2, &src, 0, minval, 0);

	instr = ir3_instr_create(ctx->ir, 2, OPC_MIN_F);
	vectorize(ctx, instr, dst, 2, &src, 0, maxval, 0);
}

static void
create_clamp_imm(struct fd3_compile_context *ctx,
		struct tgsi_dst_register *dst,
		uint32_t minval, uint32_t maxval)
{
	struct tgsi_src_register minconst, maxconst;

	get_immediate(ctx, &minconst, minval);
	get_immediate(ctx, &maxconst, maxval);

	create_clamp(ctx, dst, &minconst, &maxconst);
}

static struct tgsi_dst_register *
get_dst(struct fd3_compile_context *ctx, struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = &inst->Dst[0].Register;
	unsigned i;
	for (i = 0; i < inst->Instruction.NumSrcRegs; i++) {
		struct tgsi_src_register *src = &inst->Src[i].Register;
		if ((src->File == dst->File) && (src->Index == dst->Index)) {
			ctx->tmp_src = get_internal_temp(ctx, &ctx->tmp_dst);
			ctx->tmp_dst.WriteMask = dst->WriteMask;
			dst = &ctx->tmp_dst;
			break;
		}
	}
	return dst;
}

static void
put_dst(struct fd3_compile_context *ctx, struct tgsi_full_instruction *inst,
		struct tgsi_dst_register *dst)
{
	/* if necessary, add mov back into original dst: */
	if (dst != &inst->Dst[0].Register) {
		create_mov(ctx, &inst->Dst[0].Register, ctx->tmp_src);
	}
}

/* helper to generate the necessary repeat and/or additional instructions
 * to turn a scalar instruction into a vector operation:
 */
static void
vectorize(struct fd3_compile_context *ctx, struct ir3_instruction *instr,
		struct tgsi_dst_register *dst, int nsrcs, ...)
{
	va_list ap;
	int i, j, n = 0;

	add_dst_reg(ctx, instr, dst, 0);

	va_start(ap, nsrcs);
	for (j = 0; j < nsrcs; j++) {
		struct tgsi_src_register *src =
				va_arg(ap, struct tgsi_src_register *);
		unsigned flags = va_arg(ap, unsigned);
		add_src_reg(ctx, instr, src, 0)->flags |= flags;
	}
	va_end(ap);

	for (i = 0; i < 4; i++) {
		if (dst->WriteMask & (1 << i)) {
			struct ir3_instruction *cur;

			if (n++ == 0) {
				cur = instr;
			} else {
				cur = ir3_instr_clone(instr);
				cur->flags &= ~(IR3_INSTR_SY | IR3_INSTR_SS | IR3_INSTR_JP);
			}

			/* fix-up dst register component: */
			cur->regs[0]->num = regid(cur->regs[0]->num >> 2, i);

			/* fix-up src register component: */
			va_start(ap, nsrcs);
			for (j = 0; j < nsrcs; j++) {
				struct tgsi_src_register *src =
						va_arg(ap, struct tgsi_src_register *);
				(void)va_arg(ap, unsigned);
				cur->regs[j+1]->num =
					regid(cur->regs[j+1]->num >> 2,
						src_swiz(src, i));
				cur->flags |= src_flags(ctx, cur->regs[j+1]);
			}
			va_end(ap);
		}
	}

	/* pad w/ nop's.. at least until we are clever enough to
	 * figure out if we really need to..
	 */
	for (; n < 4; n++) {
		ir3_instr_create(instr->shader, 0, OPC_NOP);
	}
}

/*
 * Handlers for TGSI instructions which do not have a 1:1 mapping to
 * native instructions:
 */

static inline void
get_swiz(unsigned *swiz, struct tgsi_src_register *src)
{
	swiz[0] = src->SwizzleX;
	swiz[1] = src->SwizzleY;
	swiz[2] = src->SwizzleZ;
	swiz[3] = src->SwizzleW;
}

static void
trans_dotp(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_dst_register *dst  = &inst->Dst[0].Register;
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;
	unsigned swiz0[4];
	unsigned swiz1[4];
	opc_t opc_mad    = ctx->so->half_precision ? OPC_MAD_F16 : OPC_MAD_F32;
	unsigned n = t->arg;     /* number of components */
	unsigned i, swapped = 0;

	tmp_src = get_internal_temp_repl(ctx, &tmp_dst);

	/* in particular, can't handle const for src1 for cat3/mad:
	 */
	if (is_const(src1)) {
		if (!is_const(src0)) {
			struct tgsi_src_register *tmp;
			tmp = src0;
			src0 = src1;
			src1 = tmp;
			swapped = 1;
		} else {
			src0 = get_unconst(ctx, src0);
		}
	}

	get_swiz(swiz0, src0);
	get_swiz(swiz1, src1);

	instr = ir3_instr_create(ctx->ir, 2, OPC_MUL_F);
	add_dst_reg(ctx, instr, &tmp_dst, 0);
	add_src_reg(ctx, instr, src0, swiz0[0]);
	add_src_reg(ctx, instr, src1, swiz1[0]);

	for (i = 1; i < n; i++) {
		ir3_instr_create(ctx->ir, 0, OPC_NOP);

		instr = ir3_instr_create(ctx->ir, 3, opc_mad);
		add_dst_reg(ctx, instr, &tmp_dst, 0);
		add_src_reg(ctx, instr, src0, swiz0[i]);
		add_src_reg(ctx, instr, src1, swiz1[i]);
		add_src_reg(ctx, instr, tmp_src, 0);
	}

	/* DPH(a,b) = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + b.w */
	if (t->tgsi_opc == TGSI_OPCODE_DPH) {
		ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = 1;

		instr = ir3_instr_create(ctx->ir, 2, OPC_ADD_F);
		add_dst_reg(ctx, instr, &tmp_dst, 0);
		if (swapped)
			add_src_reg(ctx, instr, src0, swiz0[i]);
		else
			add_src_reg(ctx, instr, src1, swiz1[i]);
		add_src_reg(ctx, instr, tmp_src, 0);

		n++;
	}

	ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = 2;

	create_mov(ctx, dst, tmp_src);
}

/* LRP(a,b,c) = (a * b) + ((1 - a) * c) */
static void
trans_lrp(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register tmp_dst1, tmp_dst2;
	struct tgsi_src_register *tmp_src1, *tmp_src2;
	struct tgsi_src_register tmp_const;
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;

	if (is_const(src0) && is_const(src1))
		src0 = get_unconst(ctx, src0);

	tmp_src1 = get_internal_temp(ctx, &tmp_dst1);
	tmp_src2 = get_internal_temp(ctx, &tmp_dst2);

	get_immediate(ctx, &tmp_const, fui(1.0));

	/* tmp1 = (a * b) */
	instr = ir3_instr_create(ctx->ir, 2, OPC_MUL_F);
	vectorize(ctx, instr, &tmp_dst1, 2, src0, 0, src1, 0);

	/* tmp2 = (1 - a) */
	instr = ir3_instr_create(ctx->ir, 2, OPC_ADD_F);
	vectorize(ctx, instr, &tmp_dst2, 2, &tmp_const, 0,
			src0, IR3_REG_NEGATE);

	/* tmp2 = tmp2 * c */
	instr = ir3_instr_create(ctx->ir, 2, OPC_MUL_F);
	vectorize(ctx, instr, &tmp_dst2, 2,
			tmp_src2, 0,
			&inst->Src[2].Register, 0);

	/* dst = tmp1 + tmp2 */
	instr = ir3_instr_create(ctx->ir, 2, OPC_ADD_F);
	vectorize(ctx, instr, &inst->Dst[0].Register, 2,
			tmp_src1, 0,
			tmp_src2, 0);
}

/* FRC(x) = x - FLOOR(x) */
static void
trans_frac(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;

	tmp_src = get_internal_temp(ctx, &tmp_dst);

	/* tmp = FLOOR(x) */
	instr = ir3_instr_create(ctx->ir, 2, OPC_FLOOR_F);
	vectorize(ctx, instr, &tmp_dst, 1,
			&inst->Src[0].Register, 0);

	/* dst = x - tmp */
	instr = ir3_instr_create(ctx->ir, 2, OPC_ADD_F);
	vectorize(ctx, instr, &inst->Dst[0].Register, 2,
			&inst->Src[0].Register, 0,
			tmp_src, IR3_REG_NEGATE);
}

/* POW(a,b) = EXP2(b * LOG2(a)) */
static void
trans_pow(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct ir3_register *r;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_dst_register *dst  = &inst->Dst[0].Register;
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;

	tmp_src = get_internal_temp_repl(ctx, &tmp_dst);

	/* log2 Rtmp, Rsrc0 */
	ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = 5;
	instr = ir3_instr_create(ctx->ir, 4, OPC_LOG2);
	r = add_dst_reg(ctx, instr, &tmp_dst, 0);
	add_src_reg(ctx, instr, src0, src0->SwizzleX);
	regmask_set(ctx->needs_ss, r, TGSI_WRITEMASK_X);

	/* mul.f Rtmp, Rtmp, Rsrc1 */
	instr = ir3_instr_create(ctx->ir, 2, OPC_MUL_F);
	add_dst_reg(ctx, instr, &tmp_dst, 0);
	add_src_reg(ctx, instr, tmp_src, 0);
	add_src_reg(ctx, instr, src1, src1->SwizzleX);

	/* blob compiler seems to ensure there are at least 6 instructions
	 * between a "simple" (non-cat4) instruction and a dependent cat4..
	 * probably we need to handle this in some other places too.
	 */
	ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = 5;

	/* exp2 Rdst, Rtmp */
	instr = ir3_instr_create(ctx->ir, 4, OPC_EXP2);
	r = add_dst_reg(ctx, instr, &tmp_dst, 0);
	add_src_reg(ctx, instr, tmp_src, 0);
	regmask_set(ctx->needs_ss, r, TGSI_WRITEMASK_X);

	create_mov(ctx, dst, tmp_src);
}

/* texture fetch/sample instructions: */
static void
trans_samp(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_register *r;
	struct ir3_instruction *instr;
	struct tgsi_src_register *coord = &inst->Src[0].Register;
	struct tgsi_src_register *samp  = &inst->Src[1].Register;
	unsigned tex = inst->Texture.Texture;
	int8_t *order;
	unsigned i, j, flags = 0;

	switch (t->arg) {
	case TGSI_OPCODE_TEX:
		order = (tex == TGSI_TEXTURE_2D) ?
				(int8_t[4]){ 0,  1, -1, -1 } :  /* 2D */
				(int8_t[4]){ 0,  1,  2, -1 };   /* 3D */
		break;
	case TGSI_OPCODE_TXP:
		order = (tex == TGSI_TEXTURE_2D) ?
				(int8_t[4]){ 0,  1,  3, -1 } :  /* 2D */
				(int8_t[4]){ 0,  1,  2,  3 };   /* 3D */
		flags |= IR3_INSTR_P;
		break;
	default:
		compile_assert(ctx, 0);
		break;
	}

	if (tex == TGSI_TEXTURE_3D)
		flags |= IR3_INSTR_3D;

	/* The texture sample instructions need to coord in successive
	 * registers/components (ie. src.xy but not src.yx).  And TXP
	 * needs the .w component in .z for 2D..  so in some cases we
	 * might need to emit some mov instructions to shuffle things
	 * around:
	 */
	for (i = 1; (i < 4) && (order[i] >= 0); i++) {
		if (src_swiz(coord, i) != (src_swiz(coord, 0) + order[i])) {
			struct tgsi_dst_register tmp_dst;
			struct tgsi_src_register *tmp_src;

			type_t type_mov = get_ftype(ctx);

			/* need to move things around: */
			tmp_src = get_internal_temp(ctx, &tmp_dst);

			for (j = 0; (j < 4) && (order[j] >= 0); j++) {
				instr = ir3_instr_create(ctx->ir, 1, 0);
				instr->cat1.src_type = type_mov;
				instr->cat1.dst_type = type_mov;
				add_dst_reg(ctx, instr, &tmp_dst, j);
				add_src_reg(ctx, instr, coord,
						src_swiz(coord, order[j]));
			}

			coord = tmp_src;

			if (j < 4)
				ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = 4 - j - 1;

			break;
		}
	}

	instr = ir3_instr_create(ctx->ir, 5, t->opc);
	instr->cat5.type = get_ftype(ctx);
	instr->cat5.samp = samp->Index;
	instr->cat5.tex  = samp->Index;
	instr->flags |= flags;

	r = add_dst_reg(ctx, instr, &inst->Dst[0].Register, 0);
	r->wrmask = inst->Dst[0].Register.WriteMask;

	add_src_reg(ctx, instr, coord, coord->SwizzleX);

	regmask_set(ctx->needs_sy, r, r->wrmask);
}

/*
 * SEQ(a,b) = (a == b) ? 1.0 : 0.0
 *   cmps.f.eq tmp0, b, a
 *   cov.u16f16 dst, tmp0
 *
 * SNE(a,b) = (a != b) ? 1.0 : 0.0
 *   cmps.f.eq tmp0, b, a
 *   add.s tmp0, tmp0, -1
 *   sel.f16 dst, {0.0}, tmp0, {1.0}
 *
 * SGE(a,b) = (a >= b) ? 1.0 : 0.0
 *   cmps.f.ge tmp0, a, b
 *   cov.u16f16 dst, tmp0
 *
 * SLE(a,b) = (a <= b) ? 1.0 : 0.0
 *   cmps.f.ge tmp0, b, a
 *   cov.u16f16 dst, tmp0
 *
 * SGT(a,b) = (a > b)  ? 1.0 : 0.0
 *   cmps.f.ge tmp0, b, a
 *   add.s tmp0, tmp0, -1
 *   sel.f16 dst, {0.0}, tmp0, {1.0}
 *
 * SLT(a,b) = (a < b)  ? 1.0 : 0.0
 *   cmps.f.ge tmp0, a, b
 *   add.s tmp0, tmp0, -1
 *   sel.f16 dst, {0.0}, tmp0, {1.0}
 *
 * CMP(a,b,c) = (a < 0.0) ? b : c
 *   cmps.f.ge tmp0, a, {0.0}
 *   add.s tmp0, tmp0, -1
 *   sel.f16 dst, c, tmp0, b
 */
static void
trans_cmp(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_dst_register tmp_dst;
	struct tgsi_src_register *tmp_src;
	struct tgsi_src_register constval0, constval1;
	/* final instruction for CMP() uses orig src1 and src2: */
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *a0, *a1;
	unsigned condition;

	tmp_src = get_internal_temp(ctx, &tmp_dst);

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_SEQ:
	case TGSI_OPCODE_SNE:
		a0 = &inst->Src[1].Register;  /* b */
		a1 = &inst->Src[0].Register;  /* a */
		condition = IR3_COND_EQ;
		break;
	case TGSI_OPCODE_SGE:
	case TGSI_OPCODE_SLT:
		a0 = &inst->Src[0].Register;  /* a */
		a1 = &inst->Src[1].Register;  /* b */
		condition = IR3_COND_GE;
		break;
	case TGSI_OPCODE_SLE:
	case TGSI_OPCODE_SGT:
		a0 = &inst->Src[1].Register;  /* b */
		a1 = &inst->Src[0].Register;  /* a */
		condition = IR3_COND_GE;
		break;
	case TGSI_OPCODE_CMP:
		get_immediate(ctx, &constval0, fui(0.0));
		a0 = &inst->Src[0].Register;  /* a */
		a1 = &constval0;              /* {0.0} */
		condition = IR3_COND_GE;
		break;
	default:
		compile_assert(ctx, 0);
		return;
	}

	if (is_const(a0) && is_const(a1))
		a0 = get_unconst(ctx, a0);

	/* cmps.f.ge tmp, a0, a1 */
	instr = ir3_instr_create(ctx->ir, 2, OPC_CMPS_F);
	instr->cat2.condition = condition;
	vectorize(ctx, instr, &tmp_dst, 2, a0, 0, a1, 0);

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_SEQ:
	case TGSI_OPCODE_SGE:
	case TGSI_OPCODE_SLE:
		/* cov.u16f16 dst, tmp0 */
		instr = ir3_instr_create(ctx->ir, 1, 0);
		instr->cat1.src_type = get_utype(ctx);
		instr->cat1.dst_type = get_ftype(ctx);
		vectorize(ctx, instr, dst, 1, tmp_src, 0);
		break;
	case TGSI_OPCODE_SNE:
	case TGSI_OPCODE_SGT:
	case TGSI_OPCODE_SLT:
	case TGSI_OPCODE_CMP:
		/* add.s tmp, tmp, -1 */
		instr = ir3_instr_create(ctx->ir, 2, OPC_ADD_S);
		instr->repeat = 3;
		add_dst_reg(ctx, instr, &tmp_dst, 0);
		add_src_reg(ctx, instr, tmp_src, 0)->flags |= IR3_REG_R;
		ir3_reg_create(instr, 0, IR3_REG_IMMED)->iim_val = -1;

		if (t->tgsi_opc == TGSI_OPCODE_CMP) {
			/* sel.{f32,f16} dst, src2, tmp, src1 */
			instr = ir3_instr_create(ctx->ir, 3,
					ctx->so->half_precision ? OPC_SEL_F16 : OPC_SEL_F32);
			vectorize(ctx, instr, dst, 3,
					&inst->Src[2].Register, 0,
					tmp_src, 0,
					&inst->Src[1].Register, 0);
		} else {
			get_immediate(ctx, &constval0, fui(0.0));
			get_immediate(ctx, &constval1, fui(1.0));
			/* sel.{f32,f16} dst, {0.0}, tmp0, {1.0} */
			instr = ir3_instr_create(ctx->ir, 3,
					ctx->so->half_precision ? OPC_SEL_F16 : OPC_SEL_F32);
			vectorize(ctx, instr, dst, 3,
					&constval0, 0, tmp_src, 0, &constval1, 0);
		}

		break;
	}

	put_dst(ctx, inst, dst);
}

/*
 * Conditional / Flow control
 */

static unsigned
find_instruction(struct fd3_compile_context *ctx, struct ir3_instruction *instr)
{
	unsigned i;
	for (i = 0; i < ctx->ir->instrs_count; i++)
		if (ctx->ir->instrs[i] == instr)
			return i;
	return ~0;
}

static void
push_branch(struct fd3_compile_context *ctx, struct ir3_instruction *instr)
{
	ctx->branch[ctx->branch_count++] = instr;
}

static void
pop_branch(struct fd3_compile_context *ctx)
{
	struct ir3_instruction *instr;

	/* if we were clever enough, we'd patch this up after the fact,
	 * and set (jp) flag on whatever the next instruction was, rather
	 * than inserting an extra nop..
	 */
	instr = ir3_instr_create(ctx->ir, 0, OPC_NOP);
	instr->flags |= IR3_INSTR_JP;

	/* pop the branch instruction from the stack and fix up branch target: */
	instr = ctx->branch[--ctx->branch_count];
	instr->cat0.immed = ctx->ir->instrs_count - find_instruction(ctx, instr) - 1;
}

/* We probably don't really want to translate if/else/endif into branches..
 * the blob driver evaluates both legs of the if and then uses the sel
 * instruction to pick which sides of the branch to "keep".. but figuring
 * that out will take somewhat more compiler smarts.  So hopefully branches
 * don't kill performance too badly.
 */
static void
trans_if(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;
	struct tgsi_src_register *src = &inst->Src[0].Register;
	struct tgsi_src_register constval;

	get_immediate(ctx, &constval, fui(0.0));

	if (is_const(src))
		src = get_unconst(ctx, src);

	instr = ir3_instr_create(ctx->ir, 2, OPC_CMPS_F);
	ir3_reg_create(instr, regid(REG_P0, 0), 0);
	add_src_reg(ctx, instr, src, src->SwizzleX);
	add_src_reg(ctx, instr, &constval, constval.SwizzleX);
	instr->cat2.condition = IR3_COND_EQ;

	instr = ir3_instr_create(ctx->ir, 0, OPC_BR);
	push_branch(ctx, instr);
}

static void
trans_else(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct ir3_instruction *instr;

	/* for first half of if/else/endif, generate a jump past the else: */
	instr = ir3_instr_create(ctx->ir, 0, OPC_JUMP);

	pop_branch(ctx);
	push_branch(ctx, instr);
}

static void
trans_endif(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	pop_branch(ctx);
}

/*
 * Handlers for TGSI instructions which do have 1:1 mapping to native
 * instructions:
 */

static void
instr_cat0(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	ir3_instr_create(ctx->ir, 0, t->opc);
}

static void
instr_cat1(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src = &inst->Src[0].Register;

	/* mov instructions can't handle a negate on src: */
	if (src->Negate) {
		struct tgsi_src_register constval;
		struct ir3_instruction *instr;

		/* since right now, we are using uniformly either TYPE_F16 or
		 * TYPE_F32, and we don't utilize the conversion possibilities
		 * of mov instructions, we can get away with substituting an
		 * add.f which can handle negate.  Might need to revisit this
		 * in the future if we start supporting widening/narrowing or
		 * conversion to/from integer..
		 */
		instr = ir3_instr_create(ctx->ir, 2, OPC_ADD_F);
		get_immediate(ctx, &constval, fui(0.0));
		vectorize(ctx, instr, dst, 2, src, 0, &constval, 0);
	} else {
		create_mov(ctx, dst, src);
		/* create_mov() generates vector sequence, so no vectorize() */
	}
	put_dst(ctx, inst, dst);
}

static void
instr_cat2(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;
	struct ir3_instruction *instr;
	unsigned src0_flags = 0;

	switch (t->tgsi_opc) {
	case TGSI_OPCODE_ABS:
		src0_flags = IR3_REG_ABS;
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
		instr = ir3_instr_create(ctx->ir, 2, t->opc);
		vectorize(ctx, instr, dst, 1, src0, src0_flags);
		break;
	default:
		if (is_const(src0) && is_const(src1))
			src0 = get_unconst(ctx, src0);

		instr = ir3_instr_create(ctx->ir, 2, t->opc);
		vectorize(ctx, instr, dst, 2, src0, src0_flags, src1, 0);
		break;
	}

	put_dst(ctx, inst, dst);
}

static bool is_mad(opc_t opc)
{
	switch (opc) {
	case OPC_MAD_U16:
	case OPC_MADSH_U16:
	case OPC_MAD_S16:
	case OPC_MADSH_M16:
	case OPC_MAD_U24:
	case OPC_MAD_S24:
	case OPC_MAD_F16:
	case OPC_MAD_F32:
		return true;
	default:
		return false;
	}
}

static void
instr_cat3(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src0 = &inst->Src[0].Register;
	struct tgsi_src_register *src1 = &inst->Src[1].Register;
	struct ir3_instruction *instr;

	/* in particular, can't handle const for src1 for cat3..
	 * for mad, we can swap first two src's if needed:
	 */
	if (is_const(src1)) {
		if (is_mad(t->opc) && !is_const(src0)) {
			struct tgsi_src_register *tmp;
			tmp = src0;
			src0 = src1;
			src1 = tmp;
		} else {
			src0 = get_unconst(ctx, src0);
		}
	}

	instr = ir3_instr_create(ctx->ir, 3,
			ctx->so->half_precision ? t->hopc : t->opc);
	vectorize(ctx, instr, dst, 3, src0, 0, src1, 0,
			&inst->Src[2].Register, 0);
	put_dst(ctx, inst, dst);
}

static void
instr_cat4(const struct instr_translater *t,
		struct fd3_compile_context *ctx,
		struct tgsi_full_instruction *inst)
{
	struct tgsi_dst_register *dst = get_dst(ctx, inst);
	struct tgsi_src_register *src = &inst->Src[0].Register;
	struct ir3_instruction *instr;

	/* seems like blob compiler avoids const as src.. */
	if (is_const(src))
		src = get_unconst(ctx, src);

	ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = 5;
	instr = ir3_instr_create(ctx->ir, 4, t->opc);

	vectorize(ctx, instr, dst, 1, src, 0);

	regmask_set(ctx->needs_ss, instr->regs[0],
			inst->Dst[0].Register.WriteMask);

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
	INSTR(DP2,          trans_dotp, .arg = 2),
	INSTR(DP3,          trans_dotp, .arg = 3),
	INSTR(DP4,          trans_dotp, .arg = 4),
	INSTR(DPH,          trans_dotp, .arg = 3),   /* almost like DP3 */
	INSTR(MIN,          instr_cat2, .opc = OPC_MIN_F),
	INSTR(MAX,          instr_cat2, .opc = OPC_MAX_F),
	INSTR(MAD,          instr_cat3, .opc = OPC_MAD_F32, .hopc = OPC_MAD_F16),
	INSTR(LRP,          trans_lrp),
	INSTR(FRC,          trans_frac),
	INSTR(FLR,          instr_cat2, .opc = OPC_FLOOR_F),
	INSTR(ARL,          instr_cat2, .opc = OPC_FLOOR_F),
	INSTR(EX2,          instr_cat4, .opc = OPC_EXP2),
	INSTR(LG2,          instr_cat4, .opc = OPC_LOG2),
	INSTR(POW,          trans_pow),
	INSTR(ABS,          instr_cat2, .opc = OPC_ABSNEG_F),
	INSTR(COS,          instr_cat4, .opc = OPC_SIN),
	INSTR(SIN,          instr_cat4, .opc = OPC_COS),
	INSTR(TEX,          trans_samp, .opc = OPC_SAM, .arg = TGSI_OPCODE_TEX),
	INSTR(TXP,          trans_samp, .opc = OPC_SAM, .arg = TGSI_OPCODE_TXP),
	INSTR(SGT,          trans_cmp),
	INSTR(SLT,          trans_cmp),
	INSTR(SGE,          trans_cmp),
	INSTR(SLE,          trans_cmp),
	INSTR(SNE,          trans_cmp),
	INSTR(SEQ,          trans_cmp),
	INSTR(CMP,          trans_cmp),
	INSTR(IF,           trans_if),
	INSTR(ELSE,         trans_else),
	INSTR(ENDIF,        trans_endif),
	INSTR(END,          instr_cat0, .opc = OPC_END),
};

static int
decl_in(struct fd3_compile_context *ctx, struct tgsi_full_declaration *decl)
{
	struct fd3_shader_stateobj *so = ctx->so;
	unsigned base = ctx->base_reg[TGSI_FILE_INPUT];
	unsigned i, flags = 0;
	int nop = 0;

	if (ctx->so->half_precision)
		flags |= IR3_REG_HALF;

	for (i = decl->Range.First; i <= decl->Range.Last; i++) {
		unsigned n = so->inputs_count++;
		unsigned r = regid(i + base, 0);
		unsigned ncomp;

		/* TODO use ctx->info.input_usage_mask[decl->Range.n] to figure out ncomp: */
		ncomp = 4;

		DBG("decl in -> r%d", i + base);   // XXX

		so->inputs[n].compmask = (1 << ncomp) - 1;
		so->inputs[n].regid = r;
		so->inputs[n].inloc = ctx->next_inloc;
		ctx->next_inloc += ncomp;

		so->total_in += ncomp;

		/* for frag shaders, we need to generate the corresponding bary instr: */
		if (ctx->type == TGSI_PROCESSOR_FRAGMENT) {
			struct ir3_instruction *instr;

			instr = ir3_instr_create(ctx->ir, 2, OPC_BARY_F);
			instr->repeat = ncomp - 1;

			/* dst register: */
			ctx->last_input = ir3_reg_create(instr, r, flags);

			/* input position: */
			ir3_reg_create(instr, 0, IR3_REG_IMMED | IR3_REG_R)->iim_val =
					so->inputs[n].inloc - 8;

			/* input base (always r0.x): */
			ir3_reg_create(instr, regid(0,0), 0);

			nop = 6;
		}
	}

	return nop;
}

static void
decl_out(struct fd3_compile_context *ctx, struct tgsi_full_declaration *decl)
{
	struct fd3_shader_stateobj *so = ctx->so;
	unsigned base = ctx->base_reg[TGSI_FILE_OUTPUT];
	unsigned name = decl->Semantic.Name;
	unsigned i;

	compile_assert(ctx, decl->Declaration.Semantic);  // TODO is this ever not true?

	DBG("decl out[%d] -> r%d", name, decl->Range.First + base);   // XXX

	if (ctx->type == TGSI_PROCESSOR_VERTEX) {
		switch (name) {
		case TGSI_SEMANTIC_POSITION:
			so->pos_regid = regid(decl->Range.First + base, 0);
			break;
		case TGSI_SEMANTIC_PSIZE:
			so->psize_regid = regid(decl->Range.First + base, 0);
			break;
		case TGSI_SEMANTIC_COLOR:
		case TGSI_SEMANTIC_GENERIC:
		case TGSI_SEMANTIC_FOG:
		case TGSI_SEMANTIC_TEXCOORD:
			for (i = decl->Range.First; i <= decl->Range.Last; i++)
				so->outputs[so->outputs_count++].regid = regid(i + base, 0);
			break;
		default:
			compile_error(ctx, "unknown VS semantic name: %s\n",
					tgsi_semantic_names[name]);
		}
	} else {
		switch (name) {
		case TGSI_SEMANTIC_COLOR:
			so->color_regid = regid(decl->Range.First + base, 0);
			break;
		default:
			compile_error(ctx, "unknown VS semantic name: %s\n",
					tgsi_semantic_names[name]);
		}
	}
}

static void
decl_samp(struct fd3_compile_context *ctx, struct tgsi_full_declaration *decl)
{
	ctx->so->samplers_count++;
}

static void
compile_instructions(struct fd3_compile_context *ctx)
{
	struct ir3_shader *ir = ctx->ir;
	int nop = 0;

	while (!tgsi_parse_end_of_tokens(&ctx->parser)) {
		tgsi_parse_token(&ctx->parser);

		switch (ctx->parser.FullToken.Token.Type) {
		case TGSI_TOKEN_TYPE_DECLARATION: {
			struct tgsi_full_declaration *decl =
					&ctx->parser.FullToken.FullDeclaration;
			if (decl->Declaration.File == TGSI_FILE_OUTPUT) {
				decl_out(ctx, decl);
			} else if (decl->Declaration.File == TGSI_FILE_INPUT) {
				nop = decl_in(ctx, decl);
			} else if (decl->Declaration.File == TGSI_FILE_SAMPLER) {
				decl_samp(ctx, decl);
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
			memcpy(ctx->so->immediates[n].val, imm->u, 16);
			break;
		}
		case TGSI_TOKEN_TYPE_INSTRUCTION: {
			struct tgsi_full_instruction *inst =
					&ctx->parser.FullToken.FullInstruction;
			unsigned opc = inst->Instruction.Opcode;
			const struct instr_translater *t = &translaters[opc];

			if (nop) {
				ir3_instr_create(ctx->ir, 0, OPC_NOP)->repeat = nop - 1;
				nop = 0;
			}

			if (t->fxn) {
				t->fxn(t, ctx, inst);
				ctx->num_internal_temps = 0;
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

			break;
		}
		default:
			break;
		}
	}

	if (ir->instrs_count > 0)
		ir->instrs[0]->flags |= IR3_INSTR_SS | IR3_INSTR_SY;

	if (ctx->last_input)
		ctx->last_input->flags |= IR3_REG_EI;
}

int
fd3_compile_shader(struct fd3_shader_stateobj *so,
		const struct tgsi_token *tokens)
{
	struct fd3_compile_context ctx;

	assert(!so->ir);

	so->ir = ir3_shader_create();

	assert(so->ir);

	so->color_regid = regid(63,0);
	so->pos_regid   = regid(63,0);
	so->psize_regid = regid(63,0);

	if (compile_init(&ctx, so, tokens) != TGSI_PARSE_OK)
		return -1;

	compile_instructions(&ctx);

	compile_free(&ctx);

	return 0;
}
