/*
 * Copyright © 2010 Intel Corporation
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

#include "brw_fs.h"
#include "brw_cfg.h"
#include "glsl/glsl_types.h"
#include "glsl/ir_optimization.h"

static void
assign_reg(unsigned *reg_hw_locations, fs_reg *reg)
{
   if (reg->file == GRF) {
      reg->reg = reg_hw_locations[reg->reg] + reg->reg_offset;
      reg->reg_offset = 0;
   }
}

void
fs_visitor::assign_regs_trivial()
{
   unsigned hw_reg_mapping[this->alloc.count + 1];
   unsigned i;
   int reg_width = dispatch_width / 8;

   /* Note that compressed instructions require alignment to 2 registers. */
   hw_reg_mapping[0] = ALIGN(this->first_non_payload_grf, reg_width);
   for (i = 1; i <= this->alloc.count; i++) {
      hw_reg_mapping[i] = (hw_reg_mapping[i - 1] +
			   this->alloc.sizes[i - 1]);
   }
   this->grf_used = hw_reg_mapping[this->alloc.count];

   foreach_block_and_inst(block, fs_inst, inst, cfg) {
      assign_reg(hw_reg_mapping, &inst->dst);
      for (i = 0; i < inst->sources; i++) {
         assign_reg(hw_reg_mapping, &inst->src[i]);
      }
   }

   if (this->grf_used >= max_grf) {
      fail("Ran out of regs on trivial allocator (%d/%d)\n",
	   this->grf_used, max_grf);
   } else {
      this->alloc.count = this->grf_used;
   }

}

static void
brw_alloc_reg_set(struct brw_compiler *compiler, int reg_width)
{
   const struct brw_device_info *devinfo = compiler->devinfo;
   int base_reg_count = BRW_MAX_GRF;
   int index = reg_width - 1;

   /* The registers used to make up almost all values handled in the compiler
    * are a scalar value occupying a single register (or 2 registers in the
    * case of SIMD16, which is handled by dividing base_reg_count by 2 and
    * multiplying allocated register numbers by 2).  Things that were
    * aggregates of scalar values at the GLSL level were split to scalar
    * values by split_virtual_grfs().
    *
    * However, texture SEND messages return a series of contiguous registers
    * to write into.  We currently always ask for 4 registers, but we may
    * convert that to use less some day.
    *
    * Additionally, on gen5 we need aligned pairs of registers for the PLN
    * instruction, and on gen4 we need 8 contiguous regs for workaround simd16
    * texturing.
    *
    * So we have a need for classes for 1, 2, 4, and 8 registers currently,
    * and we add in '3' to make indexing the array easier for the common case
    * (since we'll probably want it for texturing later).
    *
    * And, on gen7 and newer, we do texturing SEND messages from GRFs, which
    * means that we may need any size up to the sampler message size limit (11
    * regs).
    */
   int class_count;
   int class_sizes[MAX_VGRF_SIZE];

   if (devinfo->gen >= 7) {
      for (class_count = 0; class_count < MAX_VGRF_SIZE; class_count++)
         class_sizes[class_count] = class_count + 1;
   } else {
      for (class_count = 0; class_count < 4; class_count++)
         class_sizes[class_count] = class_count + 1;
      class_sizes[class_count++] = 8;
   }

   memset(compiler->fs_reg_sets[index].class_to_ra_reg_range, 0,
          sizeof(compiler->fs_reg_sets[index].class_to_ra_reg_range));
   int *class_to_ra_reg_range = compiler->fs_reg_sets[index].class_to_ra_reg_range;

   /* Compute the total number of registers across all classes. */
   int ra_reg_count = 0;
   for (int i = 0; i < class_count; i++) {
      if (devinfo->gen <= 5 && reg_width == 2) {
         /* From the G45 PRM:
          *
          * In order to reduce the hardware complexity, the following
          * rules and restrictions apply to the compressed instruction:
          * ...
          * * Operand Alignment Rule: With the exceptions listed below, a
          *   source/destination operand in general should be aligned to
          *   even 256-bit physical register with a region size equal to
          *   two 256-bit physical register
          */
         ra_reg_count += (base_reg_count - (class_sizes[i] - 1)) / 2;
      } else {
         ra_reg_count += base_reg_count - (class_sizes[i] - 1);
      }
      /* Mark the last register. We'll fill in the beginnings later. */
      class_to_ra_reg_range[class_sizes[i]] = ra_reg_count;
   }

   /* Fill out the rest of the range markers */
   for (int i = 1; i < 17; ++i) {
      if (class_to_ra_reg_range[i] == 0)
         class_to_ra_reg_range[i] = class_to_ra_reg_range[i-1];
   }

   uint8_t *ra_reg_to_grf = ralloc_array(compiler, uint8_t, ra_reg_count);
   struct ra_regs *regs = ra_alloc_reg_set(compiler, ra_reg_count);
   if (devinfo->gen >= 6)
      ra_set_allocate_round_robin(regs);
   int *classes = ralloc_array(compiler, int, class_count);
   int aligned_pairs_class = -1;

   /* Allocate space for q values.  We allocate class_count + 1 because we
    * want to leave room for the aligned pairs class if we have it. */
   unsigned int **q_values = ralloc_array(compiler, unsigned int *,
                                          class_count + 1);
   for (int i = 0; i < class_count + 1; ++i)
      q_values[i] = ralloc_array(q_values, unsigned int, class_count + 1);

   /* Now, add the registers to their classes, and add the conflicts
    * between them and the base GRF registers (and also each other).
    */
   int reg = 0;
   int pairs_base_reg = 0;
   int pairs_reg_count = 0;
   for (int i = 0; i < class_count; i++) {
      int class_reg_count;
      if (devinfo->gen <= 5 && reg_width == 2) {
         class_reg_count = (base_reg_count - (class_sizes[i] - 1)) / 2;

         /* See comment below.  The only difference here is that we are
          * dealing with pairs of registers instead of single registers.
          * Registers of odd sizes simply get rounded up. */
         for (int j = 0; j < class_count; j++)
            q_values[i][j] = (class_sizes[i] + 1) / 2 +
                             (class_sizes[j] + 1) / 2 - 1;
      } else {
         class_reg_count = base_reg_count - (class_sizes[i] - 1);

         /* From register_allocate.c:
          *
          * q(B,C) (indexed by C, B is this register class) in
          * Runeson/Nyström paper.  This is "how many registers of B could
          * the worst choice register from C conflict with".
          *
          * If we just let the register allocation algorithm compute these
          * values, is extremely expensive.  However, since all of our
          * registers are laid out, we can very easily compute them
          * ourselves.  View the register from C as fixed starting at GRF n
          * somwhere in the middle, and the register from B as sliding back
          * and forth.  Then the first register to conflict from B is the
          * one starting at n - class_size[B] + 1 and the last register to
          * conflict will start at n + class_size[B] - 1.  Therefore, the
          * number of conflicts from B is class_size[B] + class_size[C] - 1.
          *
          *   +-+-+-+-+-+-+     +-+-+-+-+-+-+
          * B | | | | | |n| --> | | | | | | |
          *   +-+-+-+-+-+-+     +-+-+-+-+-+-+
          *             +-+-+-+-+-+
          * C           |n| | | | |
          *             +-+-+-+-+-+
          */
         for (int j = 0; j < class_count; j++)
            q_values[i][j] = class_sizes[i] + class_sizes[j] - 1;
      }
      classes[i] = ra_alloc_reg_class(regs);

      /* Save this off for the aligned pair class at the end. */
      if (class_sizes[i] == 2) {
         pairs_base_reg = reg;
         pairs_reg_count = class_reg_count;
      }

      if (devinfo->gen <= 5 && reg_width == 2) {
         for (int j = 0; j < class_reg_count; j++) {
            ra_class_add_reg(regs, classes[i], reg);

            ra_reg_to_grf[reg] = j * 2;

            for (int base_reg = j;
                 base_reg < j + (class_sizes[i] + 1) / 2;
                 base_reg++) {
               ra_add_transitive_reg_conflict(regs, base_reg, reg);
            }

            reg++;
         }
      } else {
         for (int j = 0; j < class_reg_count; j++) {
            ra_class_add_reg(regs, classes[i], reg);

            ra_reg_to_grf[reg] = j;

            for (int base_reg = j;
                 base_reg < j + class_sizes[i];
                 base_reg++) {
               ra_add_transitive_reg_conflict(regs, base_reg, reg);
            }

            reg++;
         }
      }
   }
   assert(reg == ra_reg_count);

   /* Add a special class for aligned pairs, which we'll put delta_xy
    * in on Gen <= 6 so that we can do PLN.
    */
   if (devinfo->has_pln && reg_width == 1 && devinfo->gen <= 6) {
      aligned_pairs_class = ra_alloc_reg_class(regs);

      for (int i = 0; i < pairs_reg_count; i++) {
	 if ((ra_reg_to_grf[pairs_base_reg + i] & 1) == 0) {
	    ra_class_add_reg(regs, aligned_pairs_class, pairs_base_reg + i);
	 }
      }

      for (int i = 0; i < class_count; i++) {
         /* These are a little counter-intuitive because the pair registers
          * are required to be aligned while the register they are
          * potentially interferring with are not.  In the case where the
          * size is even, the worst-case is that the register is
          * odd-aligned.  In the odd-size case, it doesn't matter.
          */
         q_values[class_count][i] = class_sizes[i] / 2 + 1;
         q_values[i][class_count] = class_sizes[i] + 1;
      }
      q_values[class_count][class_count] = 1;
   }

   ra_set_finalize(regs, q_values);

   ralloc_free(q_values);

   compiler->fs_reg_sets[index].regs = regs;
   for (unsigned i = 0; i < ARRAY_SIZE(compiler->fs_reg_sets[index].classes); i++)
      compiler->fs_reg_sets[index].classes[i] = -1;
   for (int i = 0; i < class_count; i++)
      compiler->fs_reg_sets[index].classes[class_sizes[i] - 1] = classes[i];
   compiler->fs_reg_sets[index].ra_reg_to_grf = ra_reg_to_grf;
   compiler->fs_reg_sets[index].aligned_pairs_class = aligned_pairs_class;
}

void
brw_fs_alloc_reg_sets(struct brw_compiler *compiler)
{
   brw_alloc_reg_set(compiler, 1);
   brw_alloc_reg_set(compiler, 2);
}

static int
count_to_loop_end(const bblock_t *block)
{
   if (block->end()->opcode == BRW_OPCODE_WHILE)
      return block->end_ip;

   int depth = 1;
   /* Skip the first block, since we don't want to count the do the calling
    * function found.
    */
   for (block = block->next();
        depth > 0;
        block = block->next()) {
      if (block->start()->opcode == BRW_OPCODE_DO)
         depth++;
      if (block->end()->opcode == BRW_OPCODE_WHILE) {
         depth--;
         if (depth == 0)
            return block->end_ip;
      }
   }
   unreachable("not reached");
}

/**
 * Sets up interference between thread payload registers and the virtual GRFs
 * to be allocated for program temporaries.
 *
 * We want to be able to reallocate the payload for our virtual GRFs, notably
 * because the setup coefficients for a full set of 16 FS inputs takes up 8 of
 * our 128 registers.
 *
 * The layout of the payload registers is:
 *
 * 0..payload.num_regs-1: fixed function setup (including bary coordinates).
 * payload.num_regs..payload.num_regs+curb_read_lengh-1: uniform data
 * payload.num_regs+curb_read_lengh..first_non_payload_grf-1: setup coefficients.
 *
 * And we have payload_node_count nodes covering these registers in order
 * (note that in SIMD16, a node is two registers).
 */
void
fs_visitor::setup_payload_interference(struct ra_graph *g,
                                       int payload_node_count,
                                       int first_payload_node)
{
   int loop_depth = 0;
   int loop_end_ip = 0;

   int payload_last_use_ip[payload_node_count];
   memset(payload_last_use_ip, 0, sizeof(payload_last_use_ip));
   int ip = 0;
   foreach_block_and_inst(block, fs_inst, inst, cfg) {
      switch (inst->opcode) {
      case BRW_OPCODE_DO:
         loop_depth++;

         /* Since payload regs are deffed only at the start of the shader
          * execution, any uses of the payload within a loop mean the live
          * interval extends to the end of the outermost loop.  Find the ip of
          * the end now.
          */
         if (loop_depth == 1)
            loop_end_ip = count_to_loop_end(block);
         break;
      case BRW_OPCODE_WHILE:
         loop_depth--;
         break;
      default:
         break;
      }

      int use_ip;
      if (loop_depth > 0)
         use_ip = loop_end_ip;
      else
         use_ip = ip;

      /* Note that UNIFORM args have been turned into FIXED_HW_REG by
       * assign_curbe_setup(), and interpolation uses fixed hardware regs from
       * the start (see interp_reg()).
       */
      for (int i = 0; i < inst->sources; i++) {
         if (inst->src[i].file == HW_REG &&
             inst->src[i].fixed_hw_reg.file == BRW_GENERAL_REGISTER_FILE) {
            int node_nr = inst->src[i].fixed_hw_reg.nr;
            if (node_nr >= payload_node_count)
               continue;

            payload_last_use_ip[node_nr] = use_ip;
         }
      }

      /* Special case instructions which have extra implied registers used. */
      switch (inst->opcode) {
      case FS_OPCODE_LINTERP:
         /* On gen6+ in SIMD16, there are 4 adjacent registers used by
          * PLN's sourcing of the deltas, while we list only the first one
          * in the arguments.  Pre-gen6, the deltas are computed in normal
          * VGRFs.
          */
         if (devinfo->gen >= 6) {
            int delta_x_arg = 0;
            if (inst->src[delta_x_arg].file == HW_REG &&
                inst->src[delta_x_arg].fixed_hw_reg.file ==
                BRW_GENERAL_REGISTER_FILE) {
               for (int i = 1; i < 4; ++i) {
                  int node = inst->src[delta_x_arg].fixed_hw_reg.nr + i;
                  assert(node < payload_node_count);
                  payload_last_use_ip[node] = use_ip;
               }
            }
         }
         break;

      case CS_OPCODE_CS_TERMINATE:
         payload_last_use_ip[0] = use_ip;
         break;

      default:
         if (inst->eot) {
            /* We could omit this for the !inst->header_present case, except
             * that the simulator apparently incorrectly reads from g0/g1
             * instead of sideband.  It also really freaks out driver
             * developers to see g0 used in unusual places, so just always
             * reserve it.
             */
            payload_last_use_ip[0] = use_ip;
            payload_last_use_ip[1] = use_ip;
         }
         break;
      }

      ip++;
   }

   for (int i = 0; i < payload_node_count; i++) {
      /* Mark the payload node as interfering with any virtual grf that is
       * live between the start of the program and our last use of the payload
       * node.
       */
      for (unsigned j = 0; j < this->alloc.count; j++) {
         /* Note that we use a <= comparison, unlike virtual_grf_interferes(),
          * in order to not have to worry about the uniform issue described in
          * calculate_live_intervals().
          */
         if (this->virtual_grf_start[j] <= payload_last_use_ip[i]) {
            ra_add_node_interference(g, first_payload_node + i, j);
         }
      }
   }

   for (int i = 0; i < payload_node_count; i++) {
      /* Mark each payload node as being allocated to its physical register.
       *
       * The alternative would be to have per-physical-register classes, which
       * would just be silly.
       */
      if (devinfo->gen <= 5 && dispatch_width == 16) {
         /* We have to divide by 2 here because we only have even numbered
          * registers.  Some of the payload registers will be odd, but
          * that's ok because their physical register numbers have already
          * been assigned.  The only thing this is used for is interference.
          */
         ra_set_node_reg(g, first_payload_node + i, i / 2);
      } else {
         ra_set_node_reg(g, first_payload_node + i, i);
      }
   }
}

/**
 * Sets the mrf_used array to indicate which MRFs are used by the shader IR
 *
 * This is used in assign_regs() to decide which of the GRFs that we use as
 * MRFs on gen7 get normally register allocated, and in register spilling to
 * see if we can actually use MRFs to do spills without overwriting normal MRF
 * contents.
 */
void
fs_visitor::get_used_mrfs(bool *mrf_used)
{
   int reg_width = dispatch_width / 8;

   memset(mrf_used, 0, BRW_MAX_MRF * sizeof(bool));

   foreach_block_and_inst(block, fs_inst, inst, cfg) {
      if (inst->dst.file == MRF) {
         int reg = inst->dst.reg & ~BRW_MRF_COMPR4;
         mrf_used[reg] = true;
         if (reg_width == 2) {
            if (inst->dst.reg & BRW_MRF_COMPR4) {
               mrf_used[reg + 4] = true;
            } else {
               mrf_used[reg + 1] = true;
            }
         }
      }

      if (inst->mlen > 0) {
	 for (int i = 0; i < implied_mrf_writes(inst); i++) {
            mrf_used[inst->base_mrf + i] = true;
         }
      }
   }
}

/**
 * Sets interference between virtual GRFs and usage of the high GRFs for SEND
 * messages (treated as MRFs in code generation).
 */
void
fs_visitor::setup_mrf_hack_interference(struct ra_graph *g, int first_mrf_node)
{
   bool mrf_used[BRW_MAX_MRF];
   get_used_mrfs(mrf_used);

   for (int i = 0; i < BRW_MAX_MRF; i++) {
      /* Mark each MRF reg node as being allocated to its physical register.
       *
       * The alternative would be to have per-physical-register classes, which
       * would just be silly.
       */
      ra_set_node_reg(g, first_mrf_node + i, GEN7_MRF_HACK_START + i);

      /* Since we don't have any live/dead analysis on the MRFs, just mark all
       * that are used as conflicting with all virtual GRFs.
       */
      if (mrf_used[i]) {
         for (unsigned j = 0; j < this->alloc.count; j++) {
            ra_add_node_interference(g, first_mrf_node + i, j);
         }
      }
   }
}

bool
fs_visitor::assign_regs(bool allow_spilling)
{
   struct brw_compiler *compiler = brw->intelScreen->compiler;
   /* Most of this allocation was written for a reg_width of 1
    * (dispatch_width == 8).  In extending to SIMD16, the code was
    * left in place and it was converted to have the hardware
    * registers it's allocating be contiguous physical pairs of regs
    * for reg_width == 2.
    */
   int reg_width = dispatch_width / 8;
   unsigned hw_reg_mapping[this->alloc.count];
   int payload_node_count = ALIGN(this->first_non_payload_grf, reg_width);
   int rsi = reg_width - 1; /* Which compiler->fs_reg_sets[] to use */
   calculate_live_intervals();

   int node_count = this->alloc.count;
   int first_payload_node = node_count;
   node_count += payload_node_count;
   int first_mrf_hack_node = node_count;
   if (devinfo->gen >= 7)
      node_count += BRW_MAX_GRF - GEN7_MRF_HACK_START;
   struct ra_graph *g =
      ra_alloc_interference_graph(compiler->fs_reg_sets[rsi].regs, node_count);

   for (unsigned i = 0; i < this->alloc.count; i++) {
      unsigned size = this->alloc.sizes[i];
      int c;

      assert(size <= ARRAY_SIZE(compiler->fs_reg_sets[rsi].classes) &&
             "Register allocation relies on split_virtual_grfs()");
      c = compiler->fs_reg_sets[rsi].classes[size - 1];

      /* Special case: on pre-GEN6 hardware that supports PLN, the
       * second operand of a PLN instruction needs to be an
       * even-numbered register, so we have a special register class
       * wm_aligned_pairs_class to handle this case.  pre-GEN6 always
       * uses this->delta_xy[BRW_WM_PERSPECTIVE_PIXEL_BARYCENTRIC] as the
       * second operand of a PLN instruction (since it doesn't support
       * any other interpolation modes).  So all we need to do is find
       * that register and set it to the appropriate class.
       */
      if (compiler->fs_reg_sets[rsi].aligned_pairs_class >= 0 &&
          this->delta_xy[BRW_WM_PERSPECTIVE_PIXEL_BARYCENTRIC].file == GRF &&
          this->delta_xy[BRW_WM_PERSPECTIVE_PIXEL_BARYCENTRIC].reg == i) {
         c = compiler->fs_reg_sets[rsi].aligned_pairs_class;
      }

      ra_set_node_class(g, i, c);

      for (unsigned j = 0; j < i; j++) {
	 if (virtual_grf_interferes(i, j)) {
	    ra_add_node_interference(g, i, j);
	 }
      }
   }

   setup_payload_interference(g, payload_node_count, first_payload_node);
   if (devinfo->gen >= 7) {
      setup_mrf_hack_interference(g, first_mrf_hack_node);

      foreach_block_and_inst(block, fs_inst, inst, cfg) {
         /* When we do send-from-GRF for FB writes, we need to ensure that
          * the last write instruction sends from a high register.  This is
          * because the vertex fetcher wants to start filling the low
          * payload registers while the pixel data port is still working on
          * writing out the memory.  If we don't do this, we get rendering
          * artifacts.
          *
          * We could just do "something high".  Instead, we just pick the
          * highest register that works.
          */
         if (inst->eot) {
            int size = alloc.sizes[inst->src[0].reg];
            int reg = compiler->fs_reg_sets[rsi].class_to_ra_reg_range[size] - 1;
            ra_set_node_reg(g, inst->src[0].reg, reg);
            break;
         }
      }
   }

   if (dispatch_width > 8) {
      /* In 16-wide dispatch we have an issue where a compressed
       * instruction is actually two instructions executed simultaneiously.
       * It's actually ok to have the source and destination registers be
       * the same.  In this case, each instruction over-writes its own
       * source and there's no problem.  The real problem here is if the
       * source and destination registers are off by one.  Then you can end
       * up in a scenario where the first instruction over-writes the
       * source of the second instruction.  Since the compiler doesn't know
       * about this level of granularity, we simply make the source and
       * destination interfere.
       */
      foreach_block_and_inst(block, fs_inst, inst, cfg) {
         if (inst->dst.file != GRF)
            continue;

         for (int i = 0; i < inst->sources; ++i) {
            if (inst->src[i].file == GRF) {
               ra_add_node_interference(g, inst->dst.reg, inst->src[i].reg);
            }
         }
      }
   }

   /* Debug of register spilling: Go spill everything. */
   if (unlikely(INTEL_DEBUG & DEBUG_SPILL)) {
      int reg = choose_spill_reg(g);

      if (reg != -1) {
         spill_reg(reg);
         ralloc_free(g);
         return false;
      }
   }

   if (!ra_allocate(g)) {
      /* Failed to allocate registers.  Spill a reg, and the caller will
       * loop back into here to try again.
       */
      int reg = choose_spill_reg(g);

      if (reg == -1) {
         fail("no register to spill:\n");
         dump_instructions(NULL);
      } else if (allow_spilling) {
         spill_reg(reg);
      }

      ralloc_free(g);

      return false;
   }

   /* Get the chosen virtual registers for each node, and map virtual
    * regs in the register classes back down to real hardware reg
    * numbers.
    */
   this->grf_used = payload_node_count;
   for (unsigned i = 0; i < this->alloc.count; i++) {
      int reg = ra_get_node_reg(g, i);

      hw_reg_mapping[i] = compiler->fs_reg_sets[rsi].ra_reg_to_grf[reg];
      this->grf_used = MAX2(this->grf_used,
			    hw_reg_mapping[i] + this->alloc.sizes[i]);
   }

   foreach_block_and_inst(block, fs_inst, inst, cfg) {
      assign_reg(hw_reg_mapping, &inst->dst);
      for (int i = 0; i < inst->sources; i++) {
         assign_reg(hw_reg_mapping, &inst->src[i]);
      }
   }

   this->alloc.count = this->grf_used;

   ralloc_free(g);

   return true;
}

void
fs_visitor::emit_unspill(bblock_t *block, fs_inst *inst, fs_reg dst,
                         uint32_t spill_offset, int count)
{
   int reg_size = 1;
   if (dispatch_width == 16 && count % 2 == 0) {
      reg_size = 2;
      dst.width = 16;
   }

   for (int i = 0; i < count / reg_size; i++) {
      /* The gen7 descriptor-based offset is 12 bits of HWORD units. */
      bool gen7_read = devinfo->gen >= 7 && spill_offset < (1 << 12) * REG_SIZE;

      fs_inst *unspill_inst =
         new(mem_ctx) fs_inst(gen7_read ?
                              SHADER_OPCODE_GEN7_SCRATCH_READ :
                              SHADER_OPCODE_GEN4_SCRATCH_READ,
                              dst);
      unspill_inst->offset = spill_offset;
      unspill_inst->ir = inst->ir;
      unspill_inst->annotation = inst->annotation;
      unspill_inst->regs_written = reg_size;

      if (!gen7_read) {
         unspill_inst->base_mrf = 14;
         unspill_inst->mlen = 1; /* header contains offset */
      }
      inst->insert_before(block, unspill_inst);

      dst.reg_offset += reg_size;
      spill_offset += reg_size * REG_SIZE;
   }
}

void
fs_visitor::emit_spill(bblock_t *block, fs_inst *inst, fs_reg src,
                       uint32_t spill_offset, int count)
{
   int reg_size = 1;
   int spill_base_mrf = 14;
   if (dispatch_width == 16 && count % 2 == 0) {
      spill_base_mrf = 13;
      reg_size = 2;
   }

   for (int i = 0; i < count / reg_size; i++) {
      fs_inst *spill_inst =
         new(mem_ctx) fs_inst(SHADER_OPCODE_GEN4_SCRATCH_WRITE,
                              reg_size * 8, reg_null_f, src);
      src.reg_offset += reg_size;
      spill_inst->offset = spill_offset + i * reg_size * REG_SIZE;
      spill_inst->ir = inst->ir;
      spill_inst->annotation = inst->annotation;
      spill_inst->mlen = 1 + reg_size; /* header, value */
      spill_inst->base_mrf = spill_base_mrf;
      inst->insert_after(block, spill_inst);
   }
}

int
fs_visitor::choose_spill_reg(struct ra_graph *g)
{
   float loop_scale = 1.0;
   float spill_costs[this->alloc.count];
   bool no_spill[this->alloc.count];

   for (unsigned i = 0; i < this->alloc.count; i++) {
      spill_costs[i] = 0.0;
      no_spill[i] = false;
   }

   /* Calculate costs for spilling nodes.  Call it a cost of 1 per
    * spill/unspill we'll have to do, and guess that the insides of
    * loops run 10 times.
    */
   foreach_block_and_inst(block, fs_inst, inst, cfg) {
      for (unsigned int i = 0; i < inst->sources; i++) {
	 if (inst->src[i].file == GRF) {
	    spill_costs[inst->src[i].reg] += loop_scale;

            /* Register spilling logic assumes full-width registers; smeared
             * registers have a width of 1 so if we try to spill them we'll
             * generate invalid assembly.  This shouldn't be a problem because
             * smeared registers are only used as short-term temporaries when
             * loading pull constants, so spilling them is unlikely to reduce
             * register pressure anyhow.
             */
            if (!inst->src[i].is_contiguous()) {
               no_spill[inst->src[i].reg] = true;
            }
	 }
      }

      if (inst->dst.file == GRF) {
	 spill_costs[inst->dst.reg] += inst->regs_written * loop_scale;

         if (!inst->dst.is_contiguous()) {
            no_spill[inst->dst.reg] = true;
         }
      }

      switch (inst->opcode) {

      case BRW_OPCODE_DO:
	 loop_scale *= 10;
	 break;

      case BRW_OPCODE_WHILE:
	 loop_scale /= 10;
	 break;

      case SHADER_OPCODE_GEN4_SCRATCH_WRITE:
	 if (inst->src[0].file == GRF)
	    no_spill[inst->src[0].reg] = true;
	 break;

      case SHADER_OPCODE_GEN4_SCRATCH_READ:
      case SHADER_OPCODE_GEN7_SCRATCH_READ:
	 if (inst->dst.file == GRF)
	    no_spill[inst->dst.reg] = true;
	 break;

      default:
	 break;
      }
   }

   for (unsigned i = 0; i < this->alloc.count; i++) {
      if (!no_spill[i])
	 ra_set_node_spill_cost(g, i, spill_costs[i]);
   }

   return ra_get_best_spill_node(g);
}

void
fs_visitor::spill_reg(int spill_reg)
{
   int size = alloc.sizes[spill_reg];
   unsigned int spill_offset = last_scratch;
   assert(ALIGN(spill_offset, 16) == spill_offset); /* oword read/write req. */
   int spill_base_mrf = dispatch_width > 8 ? 13 : 14;

   /* Spills may use MRFs 13-15 in the SIMD16 case.  Our texturing is done
    * using up to 11 MRFs starting from either m1 or m2, and fb writes can use
    * up to m13 (gen6+ simd16: 2 header + 8 color + 2 src0alpha + 2 omask) or
    * m15 (gen4-5 simd16: 2 header + 8 color + 1 aads + 2 src depth + 2 dst
    * depth), starting from m1.  In summary: We may not be able to spill in
    * SIMD16 mode, because we'd stomp the FB writes.
    */
   if (!spilled_any_registers) {
      bool mrf_used[BRW_MAX_MRF];
      get_used_mrfs(mrf_used);

      for (int i = spill_base_mrf; i < BRW_MAX_MRF; i++) {
         if (mrf_used[i]) {
            fail("Register spilling not supported with m%d used", i);
          return;
         }
      }

      spilled_any_registers = true;
   }

   last_scratch += size * REG_SIZE;

   /* Generate spill/unspill instructions for the objects being
    * spilled.  Right now, we spill or unspill the whole thing to a
    * virtual grf of the same size.  For most instructions, though, we
    * could just spill/unspill the GRF being accessed.
    */
   foreach_block_and_inst (block, fs_inst, inst, cfg) {
      for (unsigned int i = 0; i < inst->sources; i++) {
	 if (inst->src[i].file == GRF &&
	     inst->src[i].reg == spill_reg) {
            int regs_read = inst->regs_read(i);
            int subset_spill_offset = (spill_offset +
                                       REG_SIZE * inst->src[i].reg_offset);
            fs_reg unspill_dst(GRF, alloc.allocate(regs_read));

            inst->src[i].reg = unspill_dst.reg;
            inst->src[i].reg_offset = 0;

            emit_unspill(block, inst, unspill_dst, subset_spill_offset,
                         regs_read);
	 }
      }

      if (inst->dst.file == GRF &&
	  inst->dst.reg == spill_reg) {
         int subset_spill_offset = (spill_offset +
                                    REG_SIZE * inst->dst.reg_offset);
         fs_reg spill_src(GRF, alloc.allocate(inst->regs_written));

         inst->dst.reg = spill_src.reg;
         inst->dst.reg_offset = 0;

         /* If we're immediately spilling the register, we should not use
          * destination dependency hints.  Doing so will cause the GPU do
          * try to read and write the register at the same time and may
          * hang the GPU.
          */
         inst->no_dd_clear = false;
         inst->no_dd_check = false;

	 /* If our write is going to affect just part of the
          * inst->regs_written(), then we need to unspill the destination
          * since we write back out all of the regs_written().
	  */
	 if (inst->is_partial_write())
            emit_unspill(block, inst, spill_src, subset_spill_offset,
                         inst->regs_written);

         emit_spill(block, inst, spill_src, subset_spill_offset,
                    inst->regs_written);
      }
   }

   invalidate_live_intervals();
}
