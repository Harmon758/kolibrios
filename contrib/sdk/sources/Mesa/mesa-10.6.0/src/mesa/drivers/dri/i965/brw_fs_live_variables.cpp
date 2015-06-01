/*
 * Copyright © 2012 Intel Corporation
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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

#include "brw_cfg.h"
#include "brw_fs_live_variables.h"

using namespace brw;

#define MAX_INSTRUCTION (1 << 30)

/** @file brw_fs_live_variables.cpp
 *
 * Support for calculating liveness information about virtual GRFs.
 *
 * This produces a live interval for each whole virtual GRF.  We could
 * choose to expose per-component live intervals for VGRFs of size > 1,
 * but we currently do not.  It is easier for the consumers of this
 * information to work with whole VGRFs.
 *
 * However, we internally track use/def information at the per-component
 * (reg_offset) level for greater accuracy.  Large VGRFs may be accessed
 * piecemeal over many (possibly non-adjacent) instructions.  In this case,
 * examining a single instruction is insufficient to decide whether a whole
 * VGRF is ultimately used or defined.  Tracking individual components
 * allows us to easily assemble this information.
 *
 * See Muchnick's Advanced Compiler Design and Implementation, section
 * 14.1 (p444).
 */

void
fs_live_variables::setup_one_read(struct block_data *bd, fs_inst *inst,
                                  int ip, const fs_reg &reg)
{
   int var = var_from_reg(reg);
   assert(var < num_vars);

   /* In most cases, a register can be written over safely by the
    * same instruction that is its last use.  For a single
    * instruction, the sources are dereferenced before writing of the
    * destination starts (naturally).  This gets more complicated for
    * simd16, because the instruction:
    *
    * add(16)      g4<1>F      g4<8,8,1>F   g6<8,8,1>F
    *
    * is actually decoded in hardware as:
    *
    * add(8)       g4<1>F      g4<8,8,1>F   g6<8,8,1>F
    * add(8)       g5<1>F      g5<8,8,1>F   g7<8,8,1>F
    *
    * Which is safe.  However, if we have uniform accesses
    * happening, we get into trouble:
    *
    * add(8)       g4<1>F      g4<0,1,0>F   g6<8,8,1>F
    * add(8)       g5<1>F      g4<0,1,0>F   g7<8,8,1>F
    *
    * Now our destination for the first instruction overwrote the
    * second instruction's src0, and we get garbage for those 8
    * pixels.  There's a similar issue for the pre-gen6
    * pixel_x/pixel_y, which are registers of 16-bit values and thus
    * would get stomped by the first decode as well.
    */
   int end_ip = ip;
   if (inst->exec_size == 16 && (reg.stride == 0 ||
                                 reg.type == BRW_REGISTER_TYPE_UW ||
                                 reg.type == BRW_REGISTER_TYPE_W ||
                                 reg.type == BRW_REGISTER_TYPE_UB ||
                                 reg.type == BRW_REGISTER_TYPE_B)) {
      end_ip++;
   }

   start[var] = MIN2(start[var], ip);
   end[var] = MAX2(end[var], end_ip);

   /* The use[] bitset marks when the block makes use of a variable (VGRF
    * channel) without having completely defined that variable within the
    * block.
    */
   if (!BITSET_TEST(bd->def, var))
      BITSET_SET(bd->use, var);
}

void
fs_live_variables::setup_one_write(struct block_data *bd, fs_inst *inst,
                                   int ip, const fs_reg &reg)
{
   int var = var_from_reg(reg);
   assert(var < num_vars);

   start[var] = MIN2(start[var], ip);
   end[var] = MAX2(end[var], ip);

   /* The def[] bitset marks when an initialization in a block completely
    * screens off previous updates of that variable (VGRF channel).
    */
   if (inst->dst.file == GRF && !inst->is_partial_write()) {
      if (!BITSET_TEST(bd->use, var))
         BITSET_SET(bd->def, var);
   }
}

/**
 * Sets up the use[] and def[] bitsets.
 *
 * The basic-block-level live variable analysis needs to know which
 * variables get used before they're completely defined, and which
 * variables are completely defined before they're used.
 *
 * These are tracked at the per-component level, rather than whole VGRFs.
 */
void
fs_live_variables::setup_def_use()
{
   int ip = 0;

   foreach_block (block, cfg) {
      assert(ip == block->start_ip);
      if (block->num > 0)
	 assert(cfg->blocks[block->num - 1]->end_ip == ip - 1);

      struct block_data *bd = &block_data[block->num];

      foreach_inst_in_block(fs_inst, inst, block) {
	 /* Set use[] for this instruction */
	 for (unsigned int i = 0; i < inst->sources; i++) {
            fs_reg reg = inst->src[i];

            if (reg.file != GRF)
               continue;

            for (int j = 0; j < inst->regs_read(i); j++) {
               setup_one_read(bd, inst, ip, reg);
               reg.reg_offset++;
            }
	 }
         if (inst->reads_flag()) {
            /* The vertical combination predicates read f0.0 and f0.1. */
            if (inst->predicate == BRW_PREDICATE_ALIGN1_ANYV ||
                inst->predicate == BRW_PREDICATE_ALIGN1_ALLV) {
               assert(inst->flag_subreg == 0);
               if (!BITSET_TEST(bd->flag_def, 1)) {
                  BITSET_SET(bd->flag_use, 1);
               }
            }
            if (!BITSET_TEST(bd->flag_def, inst->flag_subreg)) {
               BITSET_SET(bd->flag_use, inst->flag_subreg);
            }
         }

         /* Set def[] for this instruction */
         if (inst->dst.file == GRF) {
            fs_reg reg = inst->dst;
            for (int j = 0; j < inst->regs_written; j++) {
               setup_one_write(bd, inst, ip, reg);
               reg.reg_offset++;
            }
	 }
         if (inst->writes_flag()) {
            if (!BITSET_TEST(bd->flag_use, inst->flag_subreg)) {
               BITSET_SET(bd->flag_def, inst->flag_subreg);
            }
         }

	 ip++;
      }
   }
}

/**
 * The algorithm incrementally sets bits in liveout and livein,
 * propagating it through control flow.  It will eventually terminate
 * because it only ever adds bits, and stops when no bits are added in
 * a pass.
 */
void
fs_live_variables::compute_live_variables()
{
   bool cont = true;

   while (cont) {
      cont = false;

      foreach_block (block, cfg) {
         struct block_data *bd = &block_data[block->num];

	 /* Update livein */
	 for (int i = 0; i < bitset_words; i++) {
            BITSET_WORD new_livein = (bd->use[i] |
                                      (bd->liveout[i] &
                                       ~bd->def[i]));
	    if (new_livein & ~bd->livein[i]) {
               bd->livein[i] |= new_livein;
               cont = true;
	    }
	 }
         BITSET_WORD new_livein = (bd->flag_use[0] |
                                   (bd->flag_liveout[0] &
                                    ~bd->flag_def[0]));
         if (new_livein & ~bd->flag_livein[0]) {
            bd->flag_livein[0] |= new_livein;
            cont = true;
         }

	 /* Update liveout */
	 foreach_list_typed(bblock_link, child_link, link, &block->children) {
            struct block_data *child_bd = &block_data[child_link->block->num];

	    for (int i = 0; i < bitset_words; i++) {
               BITSET_WORD new_liveout = (child_bd->livein[i] &
                                          ~bd->liveout[i]);
               if (new_liveout) {
                  bd->liveout[i] |= new_liveout;
                  cont = true;
               }
	    }
            BITSET_WORD new_liveout = (child_bd->flag_livein[0] &
                                       ~bd->flag_liveout[0]);
            if (new_liveout) {
               bd->flag_liveout[0] |= new_liveout;
               cont = true;
            }
	 }
      }
   }
}

/**
 * Extend the start/end ranges for each variable to account for the
 * new information calculated from control flow.
 */
void
fs_live_variables::compute_start_end()
{
   foreach_block (block, cfg) {
      struct block_data *bd = &block_data[block->num];

      for (int i = 0; i < num_vars; i++) {
	 if (BITSET_TEST(bd->livein, i)) {
	    start[i] = MIN2(start[i], block->start_ip);
	    end[i] = MAX2(end[i], block->start_ip);
	 }

	 if (BITSET_TEST(bd->liveout, i)) {
	    start[i] = MIN2(start[i], block->end_ip);
	    end[i] = MAX2(end[i], block->end_ip);
	 }

      }
   }
}

fs_live_variables::fs_live_variables(fs_visitor *v, const cfg_t *cfg)
   : v(v), cfg(cfg)
{
   mem_ctx = ralloc_context(NULL);

   num_vgrfs = v->alloc.count;
   num_vars = 0;
   var_from_vgrf = rzalloc_array(mem_ctx, int, num_vgrfs);
   for (int i = 0; i < num_vgrfs; i++) {
      var_from_vgrf[i] = num_vars;
      num_vars += v->alloc.sizes[i];
   }

   vgrf_from_var = rzalloc_array(mem_ctx, int, num_vars);
   for (int i = 0; i < num_vgrfs; i++) {
      for (unsigned j = 0; j < v->alloc.sizes[i]; j++) {
         vgrf_from_var[var_from_vgrf[i] + j] = i;
      }
   }

   start = ralloc_array(mem_ctx, int, num_vars);
   end = rzalloc_array(mem_ctx, int, num_vars);
   for (int i = 0; i < num_vars; i++) {
      start[i] = MAX_INSTRUCTION;
      end[i] = -1;
   }

   block_data= rzalloc_array(mem_ctx, struct block_data, cfg->num_blocks);

   bitset_words = BITSET_WORDS(num_vars);
   for (int i = 0; i < cfg->num_blocks; i++) {
      block_data[i].def = rzalloc_array(mem_ctx, BITSET_WORD, bitset_words);
      block_data[i].use = rzalloc_array(mem_ctx, BITSET_WORD, bitset_words);
      block_data[i].livein = rzalloc_array(mem_ctx, BITSET_WORD, bitset_words);
      block_data[i].liveout = rzalloc_array(mem_ctx, BITSET_WORD, bitset_words);

      block_data[i].flag_def[0] = 0;
      block_data[i].flag_use[0] = 0;
      block_data[i].flag_livein[0] = 0;
      block_data[i].flag_liveout[0] = 0;
   }

   setup_def_use();
   compute_live_variables();
   compute_start_end();
}

fs_live_variables::~fs_live_variables()
{
   ralloc_free(mem_ctx);
}

void
fs_visitor::invalidate_live_intervals()
{
   ralloc_free(live_intervals);
   live_intervals = NULL;
}

/**
 * Compute the live intervals for each virtual GRF.
 *
 * This uses the per-component use/def data, but combines it to produce
 * information about whole VGRFs.
 */
void
fs_visitor::calculate_live_intervals()
{
   if (this->live_intervals)
      return;

   int num_vgrfs = this->alloc.count;
   ralloc_free(this->virtual_grf_start);
   ralloc_free(this->virtual_grf_end);
   virtual_grf_start = ralloc_array(mem_ctx, int, num_vgrfs);
   virtual_grf_end = ralloc_array(mem_ctx, int, num_vgrfs);

   for (int i = 0; i < num_vgrfs; i++) {
      virtual_grf_start[i] = MAX_INSTRUCTION;
      virtual_grf_end[i] = -1;
   }

   this->live_intervals = new(mem_ctx) fs_live_variables(this, cfg);

   /* Merge the per-component live ranges to whole VGRF live ranges. */
   for (int i = 0; i < live_intervals->num_vars; i++) {
      int vgrf = live_intervals->vgrf_from_var[i];
      virtual_grf_start[vgrf] = MIN2(virtual_grf_start[vgrf],
                                     live_intervals->start[i]);
      virtual_grf_end[vgrf] = MAX2(virtual_grf_end[vgrf],
                                   live_intervals->end[i]);
   }
}

bool
fs_live_variables::vars_interfere(int a, int b)
{
   return !(end[b] <= start[a] ||
            end[a] <= start[b]);
}

bool
fs_visitor::virtual_grf_interferes(int a, int b)
{
   return !(virtual_grf_end[a] <= virtual_grf_start[b] ||
            virtual_grf_end[b] <= virtual_grf_start[a]);
}
