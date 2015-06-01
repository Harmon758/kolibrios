/* -*- mode: C; c-file-style: "k&r"; tab-width 4; indent-tabs-mode: t; -*- */

/*
 * Copyright (C) 2014 Rob Clark <robclark@freedesktop.org>
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


#include "util/u_math.h"

#include "ir3.h"

enum {
	SCHEDULED = -1,
	DELAYED = -2,
};

/*
 * Instruction Scheduling:
 *
 * Using the depth sorted list from depth pass, attempt to recursively
 * schedule deepest unscheduled path.  The first instruction that cannot
 * be scheduled, returns the required delay slots it needs, at which
 * point we return back up to the top and attempt to schedule by next
 * highest depth.  After a sufficient number of instructions have been
 * scheduled, return back to beginning of list and start again.  If you
 * reach the end of depth sorted list without being able to insert any
 * instruction, insert nop's.  Repeat until no more unscheduled
 * instructions.
 *
 * There are a few special cases that need to be handled, since sched
 * is currently independent of register allocation.  Usages of address
 * register (a0.x) or predicate register (p0.x) must be serialized.  Ie.
 * if you have two pairs of instructions that write the same special
 * register and then read it, then those pairs cannot be interleaved.
 * To solve this, when we are in such a scheduling "critical section",
 * and we encounter a conflicting write to a special register, we try
 * to schedule any remaining instructions that use that value first.
 */

struct ir3_sched_ctx {
	struct ir3_instruction *scheduled; /* last scheduled instr */
	struct ir3_instruction *addr;      /* current a0.x user, if any */
	struct ir3_instruction *pred;      /* current p0.x user, if any */
	unsigned cnt;
	bool error;
};

static struct ir3_instruction *
deepest(struct ir3_instruction **srcs, unsigned nsrcs)
{
	struct ir3_instruction *d = NULL;
	unsigned i = 0, id = 0;

	while ((i < nsrcs) && !(d = srcs[id = i]))
		i++;

	if (!d)
		return NULL;

	for (; i < nsrcs; i++)
		if (srcs[i] && (srcs[i]->depth > d->depth))
			d = srcs[id = i];

	srcs[id] = NULL;

	return d;
}

static unsigned distance(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *instr, unsigned maxd)
{
	struct ir3_instruction *n = ctx->scheduled;
	unsigned d = 0;
	while (n && (n != instr) && (d < maxd)) {
		if (is_alu(n) || is_flow(n))
			d++;
		n = n->next;
	}
	return d;
}

/* TODO maybe we want double linked list? */
static struct ir3_instruction * prev(struct ir3_instruction *instr)
{
	struct ir3_instruction *p = instr->block->head;
	while (p && (p->next != instr))
		p = p->next;
	return p;
}

static bool is_sfu_or_mem(struct ir3_instruction *instr)
{
	return is_sfu(instr) || is_mem(instr);
}

static void schedule(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *instr, bool remove)
{
	struct ir3_block *block = instr->block;

	/* maybe there is a better way to handle this than just stuffing
	 * a nop.. ideally we'd know about this constraint in the
	 * scheduling and depth calculation..
	 */
	if (ctx->scheduled && is_sfu_or_mem(ctx->scheduled) && is_sfu_or_mem(instr))
		schedule(ctx, ir3_instr_create(block, 0, OPC_NOP), false);

	/* remove from depth list:
	 */
	if (remove) {
		struct ir3_instruction *p = prev(instr);

		/* NOTE: this can happen for inputs which are not
		 * read.. in that case there is no need to schedule
		 * the input, so just bail:
		 */
		if (instr != (p ? p->next : block->head))
			return;

		if (p)
			p->next = instr->next;
		else
			block->head = instr->next;
	}

	if (writes_addr(instr)) {
		assert(ctx->addr == NULL);
		ctx->addr = instr;
	}

	if (writes_pred(instr)) {
		assert(ctx->pred == NULL);
		ctx->pred = instr;
	}

	instr->flags |= IR3_INSTR_MARK;

	instr->next = ctx->scheduled;
	ctx->scheduled = instr;

	ctx->cnt++;
}

/*
 * Delay-slot calculation.  Follows fanin/fanout.
 */

/* calculate delay for specified src: */
static unsigned delay_calc_srcn(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *assigner,
		struct ir3_instruction *consumer, unsigned srcn)
{
	unsigned delay = 0;

	if (is_meta(assigner)) {
		struct ir3_instruction *src;
		foreach_ssa_src(src, assigner) {
			unsigned d = delay_calc_srcn(ctx, src, consumer, srcn);
			delay = MAX2(delay, d);
		}
	} else {
		delay = ir3_delayslots(assigner, consumer, srcn);
		delay -= distance(ctx, assigner, delay);
	}

	return delay;
}

/* calculate delay for instruction (maximum of delay for all srcs): */
static unsigned delay_calc(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *instr)
{
	unsigned delay = 0;
	struct ir3_instruction *src;

	foreach_ssa_src_n(src, i, instr) {
		unsigned d = delay_calc_srcn(ctx, src, instr, i);
		delay = MAX2(delay, d);
	}

	return delay;
}

/* A negative return value signals that an instruction has been newly
 * SCHEDULED (or DELAYED due to address or predicate register already
 * in use), return back up to the top of the stack (to block_sched())
 */
static int trysched(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *instr)
{
	struct ir3_instruction *srcs[64];
	struct ir3_instruction *src;
	unsigned delay, nsrcs = 0;

	/* if already scheduled: */
	if (instr->flags & IR3_INSTR_MARK)
		return 0;

	/* figure out our src's, copy 'em out into an array for sorting: */
	foreach_ssa_src(src, instr) {
		debug_assert(nsrcs < ARRAY_SIZE(srcs));
		srcs[nsrcs++] = src;
	}

	/* for each src register in sorted order:
	 */
	delay = 0;
	while ((src = deepest(srcs, nsrcs))) {
		delay = trysched(ctx, src);
		if (delay)
			return delay;
	}

	/* all our dependents are scheduled, figure out if
	 * we have enough delay slots to schedule ourself:
	 */
	delay = delay_calc(ctx, instr);
	if (delay)
		return delay;

	/* if the instruction is a kill, we need to ensure *every*
	 * bary.f is scheduled.  The hw seems unhappy if the thread
	 * gets killed before the end-input (ei) flag is hit.
	 *
	 * We could do this by adding each bary.f instruction as
	 * virtual ssa src for the kill instruction.  But we have
	 * fixed length instr->regs[].
	 *
	 * TODO this wouldn't be quite right if we had multiple
	 * basic blocks, if any block was conditional.  We'd need
	 * to schedule the bary.f's outside of any block which
	 * was conditional that contained a kill.. I think..
	 */
	if (is_kill(instr)) {
		struct ir3 *ir = instr->block->shader;
		unsigned i;

		for (i = 0; i < ir->baryfs_count; i++) {
			struct ir3_instruction *baryf = ir->baryfs[i];
			if (baryf->depth == DEPTH_UNUSED)
				continue;
			delay = trysched(ctx, baryf);
			if (delay)
				return delay;
		}
	}

	/* if this is a write to address/predicate register, and that
	 * register is currently in use, we need to defer until it is
	 * free:
	 */
	if (writes_addr(instr) && ctx->addr) {
		assert(ctx->addr != instr);
		return DELAYED;
	}
	if (writes_pred(instr) && ctx->pred) {
		assert(ctx->pred != instr);
		return DELAYED;
	}

	schedule(ctx, instr, true);
	return SCHEDULED;
}

static struct ir3_instruction * reverse(struct ir3_instruction *instr)
{
	struct ir3_instruction *reversed = NULL;
	while (instr) {
		struct ir3_instruction *next = instr->next;
		instr->next = reversed;
		reversed = instr;
		instr = next;
	}
	return reversed;
}

static bool uses_current_addr(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *instr)
{
	return instr->address && (ctx->addr == instr->address);
}

static bool uses_current_pred(struct ir3_sched_ctx *ctx,
		struct ir3_instruction *instr)
{
	struct ir3_instruction *src;
	foreach_ssa_src(src, instr)
		if (ctx->pred == src)
			return true;
	return false;
}

/* when we encounter an instruction that writes to the address register
 * when it is in use, we delay that instruction and try to schedule all
 * other instructions using the current address register:
 */
static int block_sched_undelayed(struct ir3_sched_ctx *ctx,
		struct ir3_block *block)
{
	struct ir3_instruction *instr = block->head;
	bool addr_in_use = false;
	bool pred_in_use = false;
	bool all_delayed = true;
	unsigned cnt = ~0, attempted = 0;

	while (instr) {
		struct ir3_instruction *next = instr->next;
		bool addr = uses_current_addr(ctx, instr);
		bool pred = uses_current_pred(ctx, instr);

		if (addr || pred) {
			int ret = trysched(ctx, instr);

			if (ret != DELAYED)
				all_delayed = false;

			if (ret == SCHEDULED)
				cnt = 0;
			else if (ret > 0)
				cnt = MIN2(cnt, ret);
			if (addr)
				addr_in_use = true;
			if (pred)
				pred_in_use = true;

			attempted++;
		}

		instr = next;
	}

	if (!addr_in_use)
		ctx->addr = NULL;

	if (!pred_in_use)
		ctx->pred = NULL;

	/* detect if we've gotten ourselves into an impossible situation
	 * and bail if needed
	 */
	if (all_delayed && (attempted > 0)) {
		if (pred_in_use) {
			/* TODO we probably need to keep a list of instructions
			 * that reference predicate, similar to indirects
			 */
			ctx->error = true;
			return DELAYED;
		}
		if (addr_in_use) {
			struct ir3 *ir = ctx->addr->block->shader;
			struct ir3_instruction *new_addr =
					ir3_instr_clone(ctx->addr);
			unsigned i;

			/* original addr is scheduled, but new one isn't: */
			new_addr->flags &= ~IR3_INSTR_MARK;

			for (i = 0; i < ir->indirects_count; i++) {
				struct ir3_instruction *indirect = ir->indirects[i];

				/* skip instructions already scheduled: */
				if (indirect->flags & IR3_INSTR_MARK)
					continue;

				/* remap remaining instructions using current addr
				 * to new addr:
				 */
				if (indirect->address == ctx->addr)
					indirect->address = new_addr;
			}

			/* all remaining indirects remapped to new addr: */
			ctx->addr = NULL;

			/* not really, but this will trigger us to go back to
			 * main trysched() loop now that we've resolved the
			 * conflict by duplicating the instr that writes to
			 * the address register.
			 */
			return SCHEDULED;
		}
	}

	return cnt;
}

static void block_sched(struct ir3_sched_ctx *ctx, struct ir3_block *block)
{
	struct ir3_instruction *instr;

	/* schedule all the shader input's (meta-instr) first so that
	 * the RA step sees that the input registers contain a value
	 * from the start of the shader:
	 */
	if (!block->parent) {
		unsigned i;
		for (i = 0; i < block->ninputs; i++) {
			struct ir3_instruction *in = block->inputs[i];
			if (in)
				schedule(ctx, in, true);
		}
	}

	while ((instr = block->head) && !ctx->error) {
		/* NOTE: always grab next *before* trysched(), in case the
		 * instruction is actually scheduled (and therefore moved
		 * from depth list into scheduled list)
		 */
		struct ir3_instruction *next = instr->next;
		int cnt = trysched(ctx, instr);

		if (cnt == DELAYED)
			cnt = block_sched_undelayed(ctx, block);

		/* -1 is signal to return up stack, but to us means same as 0: */
		cnt = MAX2(0, cnt);
		cnt += ctx->cnt;
		instr = next;

		/* if deepest remaining instruction cannot be scheduled, try
		 * the increasingly more shallow instructions until needed
		 * number of delay slots is filled:
		 */
		while (instr && (cnt > ctx->cnt)) {
			next = instr->next;
			trysched(ctx, instr);
			instr = next;
		}

		/* and if we run out of instructions that can be scheduled,
		 * then it is time for nop's:
		 */
		while (cnt > ctx->cnt)
			schedule(ctx, ir3_instr_create(block, 0, OPC_NOP), false);
	}

	/* at this point, scheduled list is in reverse order, so fix that: */
	block->head = reverse(ctx->scheduled);
}

int ir3_block_sched(struct ir3_block *block)
{
	struct ir3_sched_ctx ctx = {0};
	ir3_clear_mark(block->shader);
	block_sched(&ctx, block);
	if (ctx.error)
		return -1;
	return 0;
}
