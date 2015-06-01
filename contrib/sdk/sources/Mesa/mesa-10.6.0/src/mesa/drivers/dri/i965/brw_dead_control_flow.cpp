/*
 * Copyright © 2013 Intel Corporation
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
 */

/** @file brw_dead_control_flow.cpp
 *
 * This file implements the dead control flow elimination optimization pass.
 */

#include "brw_shader.h"
#include "brw_cfg.h"

/* Look for and eliminate dead control flow:
 *
 *   - if/endif
 *   . else in else/endif
 *   - if/else/endif
 */
bool
dead_control_flow_eliminate(backend_visitor *v)
{
   bool progress = false;

   foreach_block_safe (block, v->cfg) {
      bblock_t *if_block = NULL, *else_block = NULL, *endif_block = block;
      bool found = false;

      /* ENDIF instructions, by definition, can only be found at the start of
       * basic blocks.
       */
      backend_instruction *endif_inst = endif_block->start();
      if (endif_inst->opcode != BRW_OPCODE_ENDIF)
         continue;

      backend_instruction *if_inst = NULL, *else_inst = NULL;
      backend_instruction *prev_inst = endif_block->prev()->end();
      if (prev_inst->opcode == BRW_OPCODE_ELSE) {
         else_inst = prev_inst;
         else_block = endif_block->prev();
         found = true;

         if (else_block->start_ip == else_block->end_ip)
            prev_inst = else_block->prev()->end();
      }

      if (prev_inst->opcode == BRW_OPCODE_IF) {
         if_inst = prev_inst;
         if_block = else_block != NULL ? else_block->prev()
                                       : endif_block->prev();
         found = true;
      } else {
         /* Don't remove the ENDIF if we didn't find a dead IF. */
         endif_inst = NULL;
      }

      if (found) {
         bblock_t *earlier_block = NULL, *later_block = NULL;

         if (if_inst) {
            if (if_block->start_ip == if_block->end_ip) {
               earlier_block = if_block->prev();
            } else {
               earlier_block = if_block;
            }
            if_inst->remove(if_block);
         }

         if (else_inst) {
            else_inst->remove(else_block);
         }

         if (endif_inst) {
            if (endif_block->start_ip == endif_block->end_ip) {
               later_block = endif_block->next();
            } else {
               later_block = endif_block;
            }
            endif_inst->remove(endif_block);
         }

         assert((earlier_block == NULL) == (later_block == NULL));
         if (earlier_block && earlier_block->can_combine_with(later_block)) {
            earlier_block->combine_with(later_block);

            /* If ENDIF was in its own block, then we've now deleted it and
             * merged the two surrounding blocks, the latter of which the
             * __next block pointer was pointing to.
             */
            if (endif_block != later_block) {
               __next = earlier_block->next();
            }
         }

         progress = true;
      }
   }

   if (progress)
      v->invalidate_live_intervals();

   return progress;
}
