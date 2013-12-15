/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 2012-2013 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Chia-I Wu <olv@lunarg.com>
 */

#include "toy_compiler.h"

/**
 * Dump an operand.
 */
static void
tc_dump_operand(struct toy_compiler *tc,
                enum toy_file file, enum toy_type type, enum toy_rect rect,
                bool indirect, unsigned indirect_subreg, uint32_t val32,
                bool is_dst)
{
   static const char *toy_file_names[TOY_FILE_COUNT] = {
      [TOY_FILE_VRF]        = "v",
      [TOY_FILE_ARF]        = "NOT USED",
      [TOY_FILE_GRF]        = "r",
      [TOY_FILE_MRF]        = "m",
      [TOY_FILE_IMM]        = "NOT USED",
   };
   const char *name = toy_file_names[file];
   int reg, subreg;

   if (file != TOY_FILE_IMM) {
      reg = val32 / TOY_REG_WIDTH;
      subreg = (val32 % TOY_REG_WIDTH) / toy_type_size(type);
   }

   switch (file) {
   case TOY_FILE_GRF:
      if (indirect) {
         const int addr_subreg = indirect_subreg / toy_type_size(TOY_TYPE_UW);

         ilo_printf("%s[a0.%d", name, addr_subreg);
         if (val32)
            ilo_printf("%+d", (int) val32);
         ilo_printf("]");
         break;
      }
      /* fall through */
   case TOY_FILE_VRF:
   case TOY_FILE_MRF:
      ilo_printf("%s%d", name, reg);
      if (subreg)
         ilo_printf(".%d", subreg);
      break;
   case TOY_FILE_ARF:
      switch (reg) {
      case BRW_ARF_NULL:
         ilo_printf("null");
         break;
      case BRW_ARF_ADDRESS:
         ilo_printf("a0.%d", subreg);
         break;
      case BRW_ARF_ACCUMULATOR:
      case BRW_ARF_ACCUMULATOR + 1:
         ilo_printf("acc%d.%d", (reg & 1), subreg);
         break;
      case BRW_ARF_FLAG:
         ilo_printf("f0.%d", subreg);
         break;
      case BRW_ARF_STATE:
         ilo_printf("sr0.%d", subreg);
         break;
      case BRW_ARF_CONTROL:
         ilo_printf("cr0.%d", subreg);
         break;
      case BRW_ARF_NOTIFICATION_COUNT:
      case BRW_ARF_NOTIFICATION_COUNT + 1:
         ilo_printf("n%d.%d", (reg & 1), subreg);
         break;
      case BRW_ARF_IP:
         ilo_printf("ip");
         break;
      }
      break;
   case TOY_FILE_IMM:
      switch (type) {
      case TOY_TYPE_F:
         {
            union fi fi = { .ui = val32 };
            ilo_printf("%f", fi.f);
         }
         break;
      case TOY_TYPE_D:
         ilo_printf("%d", (int32_t) val32);
         break;
      case TOY_TYPE_UD:
         ilo_printf("%u", val32);
         break;
      case TOY_TYPE_W:
         ilo_printf("%d", (int16_t) (val32 & 0xffff));
         break;
      case TOY_TYPE_UW:
         ilo_printf("%u", val32 & 0xffff);
         break;
      case TOY_TYPE_V:
         ilo_printf("0x%08x", val32);
         break;
      default:
         assert(!"unknown imm type");
         break;
      }
      break;
   default:
      assert(!"unexpected file");
      break;
   }

   /* dump the region parameter */
   if (file != TOY_FILE_IMM) {
      int vert_stride, width, horz_stride;

      switch (rect) {
      case TOY_RECT_LINEAR:
         vert_stride = tc->rect_linear_width;
         width = tc->rect_linear_width;
         horz_stride = 1;
         break;
      case TOY_RECT_041:
         vert_stride = 0;
         width = 4;
         horz_stride = 1;
         break;
      case TOY_RECT_010:
         vert_stride = 0;
         width = 1;
         horz_stride = 0;
         break;
      case TOY_RECT_220:
         vert_stride = 2;
         width = 2;
         horz_stride = 0;
         break;
      case TOY_RECT_440:
         vert_stride = 4;
         width = 4;
         horz_stride = 0;
         break;
      case TOY_RECT_240:
         vert_stride = 2;
         width = 4;
         horz_stride = 0;
         break;
      default:
         assert(!"unknown rect parameter");
         vert_stride = 0;
         width = 0;
         horz_stride = 0;
         break;
      }

      if (is_dst)
         ilo_printf("<%d>", horz_stride);
      else
         ilo_printf("<%d;%d,%d>", vert_stride, width, horz_stride);
   }

   switch (type) {
   case TOY_TYPE_F:
      ilo_printf(":f");
      break;
   case TOY_TYPE_D:
      ilo_printf(":d");
      break;
   case TOY_TYPE_UD:
      ilo_printf(":ud");
      break;
   case TOY_TYPE_W:
      ilo_printf(":w");
      break;
   case TOY_TYPE_UW:
      ilo_printf(":uw");
      break;
   case TOY_TYPE_V:
      ilo_printf(":v");
      break;
   default:
      assert(!"unexpected type");
      break;
   }
}

/**
 * Dump a source operand.
 */
static void
tc_dump_src(struct toy_compiler *tc, struct toy_src src)
{
   if (src.negate)
      ilo_printf("-");
   if (src.absolute)
      ilo_printf("|");

   tc_dump_operand(tc, src.file, src.type, src.rect,
         src.indirect, src.indirect_subreg, src.val32, false);

   if (tsrc_is_swizzled(src)) {
      const char xyzw[] = "xyzw";
      ilo_printf(".%c%c%c%c",
            xyzw[src.swizzle_x],
            xyzw[src.swizzle_y],
            xyzw[src.swizzle_z],
            xyzw[src.swizzle_w]);
   }

   if (src.absolute)
      ilo_printf("|");
}

/**
 * Dump a destination operand.
 */
static void
tc_dump_dst(struct toy_compiler *tc, struct toy_dst dst)
{
   tc_dump_operand(tc, dst.file, dst.type, dst.rect,
         dst.indirect, dst.indirect_subreg, dst.val32, true);

   if (dst.writemask != TOY_WRITEMASK_XYZW) {
      ilo_printf(".");
      if (dst.writemask & TOY_WRITEMASK_X)
         ilo_printf("x");
      if (dst.writemask & TOY_WRITEMASK_Y)
         ilo_printf("y");
      if (dst.writemask & TOY_WRITEMASK_Z)
         ilo_printf("z");
      if (dst.writemask & TOY_WRITEMASK_W)
         ilo_printf("w");
   }
}

static const char *
get_opcode_name(unsigned opcode)
{
   switch (opcode) {
   case BRW_OPCODE_MOV:                   return "mov";
   case BRW_OPCODE_SEL:                   return "sel";
   case BRW_OPCODE_NOT:                   return "not";
   case BRW_OPCODE_AND:                   return "and";
   case BRW_OPCODE_OR:                    return "or";
   case BRW_OPCODE_XOR:                   return "xor";
   case BRW_OPCODE_SHR:                   return "shr";
   case BRW_OPCODE_SHL:                   return "shl";
   case BRW_OPCODE_RSR:                   return "rsr";
   case BRW_OPCODE_RSL:                   return "rsl";
   case BRW_OPCODE_ASR:                   return "asr";
   case BRW_OPCODE_CMP:                   return "cmp";
   case BRW_OPCODE_CMPN:                  return "cmpn";
   case BRW_OPCODE_JMPI:                  return "jmpi";
   case BRW_OPCODE_IF:                    return "if";
   case BRW_OPCODE_IFF:                   return "iff";
   case BRW_OPCODE_ELSE:                  return "else";
   case BRW_OPCODE_ENDIF:                 return "endif";
   case BRW_OPCODE_DO:                    return "do";
   case BRW_OPCODE_WHILE:                 return "while";
   case BRW_OPCODE_BREAK:                 return "break";
   case BRW_OPCODE_CONTINUE:              return "continue";
   case BRW_OPCODE_HALT:                  return "halt";
   case BRW_OPCODE_MSAVE:                 return "msave";
   case BRW_OPCODE_MRESTORE:              return "mrestore";
   case BRW_OPCODE_PUSH:                  return "push";
   case BRW_OPCODE_POP:                   return "pop";
   case BRW_OPCODE_WAIT:                  return "wait";
   case BRW_OPCODE_SEND:                  return "send";
   case BRW_OPCODE_SENDC:                 return "sendc";
   case BRW_OPCODE_MATH:                  return "math";
   case BRW_OPCODE_ADD:                   return "add";
   case BRW_OPCODE_MUL:                   return "mul";
   case BRW_OPCODE_AVG:                   return "avg";
   case BRW_OPCODE_FRC:                   return "frc";
   case BRW_OPCODE_RNDU:                  return "rndu";
   case BRW_OPCODE_RNDD:                  return "rndd";
   case BRW_OPCODE_RNDE:                  return "rnde";
   case BRW_OPCODE_RNDZ:                  return "rndz";
   case BRW_OPCODE_MAC:                   return "mac";
   case BRW_OPCODE_MACH:                  return "mach";
   case BRW_OPCODE_LZD:                   return "lzd";
   case BRW_OPCODE_SAD2:                  return "sad2";
   case BRW_OPCODE_SADA2:                 return "sada2";
   case BRW_OPCODE_DP4:                   return "dp4";
   case BRW_OPCODE_DPH:                   return "dph";
   case BRW_OPCODE_DP3:                   return "dp3";
   case BRW_OPCODE_DP2:                   return "dp2";
   case BRW_OPCODE_DPA2:                  return "dpa2";
   case BRW_OPCODE_LINE:                  return "line";
   case BRW_OPCODE_PLN:                   return "pln";
   case BRW_OPCODE_MAD:                   return "mad";
   case BRW_OPCODE_NOP:                   return "nop";
   /* TGSI */
   case TOY_OPCODE_TGSI_IN:               return "tgsi.in";
   case TOY_OPCODE_TGSI_CONST:            return "tgsi.const";
   case TOY_OPCODE_TGSI_SV:               return "tgsi.sv";
   case TOY_OPCODE_TGSI_IMM:              return "tgsi.imm";
   case TOY_OPCODE_TGSI_INDIRECT_FETCH:   return "tgsi.indirect_fetch";
   case TOY_OPCODE_TGSI_INDIRECT_STORE:   return "tgsi.indirect_store";
   case TOY_OPCODE_TGSI_TEX:              return "tgsi.tex";
   case TOY_OPCODE_TGSI_TXB:              return "tgsi.txb";
   case TOY_OPCODE_TGSI_TXD:              return "tgsi.txd";
   case TOY_OPCODE_TGSI_TXL:              return "tgsi.txl";
   case TOY_OPCODE_TGSI_TXP:              return "tgsi.txp";
   case TOY_OPCODE_TGSI_TXF:              return "tgsi.txf";
   case TOY_OPCODE_TGSI_TXQ:              return "tgsi.txq";
   case TOY_OPCODE_TGSI_TXQ_LZ:           return "tgsi.txq_lz";
   case TOY_OPCODE_TGSI_TEX2:             return "tgsi.tex2";
   case TOY_OPCODE_TGSI_TXB2:             return "tgsi.txb2";
   case TOY_OPCODE_TGSI_TXL2:             return "tgsi.txl2";
   case TOY_OPCODE_TGSI_SAMPLE:           return "tgsi.sample";
   case TOY_OPCODE_TGSI_SAMPLE_I:         return "tgsi.sample_i";
   case TOY_OPCODE_TGSI_SAMPLE_I_MS:      return "tgsi.sample_i_ms";
   case TOY_OPCODE_TGSI_SAMPLE_B:         return "tgsi.sample_b";
   case TOY_OPCODE_TGSI_SAMPLE_C:         return "tgsi.sample_c";
   case TOY_OPCODE_TGSI_SAMPLE_C_LZ:      return "tgsi.sample_c_lz";
   case TOY_OPCODE_TGSI_SAMPLE_D:         return "tgsi.sample_d";
   case TOY_OPCODE_TGSI_SAMPLE_L:         return "tgsi.sample_l";
   case TOY_OPCODE_TGSI_GATHER4:          return "tgsi.gather4";
   case TOY_OPCODE_TGSI_SVIEWINFO:        return "tgsi.sviewinfo";
   case TOY_OPCODE_TGSI_SAMPLE_POS:       return "tgsi.sample_pos";
   case TOY_OPCODE_TGSI_SAMPLE_INFO:      return "tgsi.sample_info";
   /* math */
   case TOY_OPCODE_INV:                   return "math.inv";
   case TOY_OPCODE_LOG:                   return "math.log";
   case TOY_OPCODE_EXP:                   return "math.exp";
   case TOY_OPCODE_SQRT:                  return "math.sqrt";
   case TOY_OPCODE_RSQ:                   return "math.rsq";
   case TOY_OPCODE_SIN:                   return "math.sin";
   case TOY_OPCODE_COS:                   return "math.cos";
   case TOY_OPCODE_FDIV:                  return "math.fdiv";
   case TOY_OPCODE_POW:                   return "math.pow";
   case TOY_OPCODE_INT_DIV_QUOTIENT:      return "math.int_div_quotient";
   case TOY_OPCODE_INT_DIV_REMAINDER:     return "math.int_div_remainer";
   /* urb */
   case TOY_OPCODE_URB_WRITE:             return "urb.urb_write";
   /* gs */
   case TOY_OPCODE_EMIT:                  return "gs.emit";
   case TOY_OPCODE_ENDPRIM:               return "gs.endprim";
   /* fs */
   case TOY_OPCODE_DDX:                   return "fs.ddx";
   case TOY_OPCODE_DDY:                   return "fs.ddy";
   case TOY_OPCODE_FB_WRITE:              return "fs.fb_write";
   case TOY_OPCODE_KIL:                   return "fs.kil";
   default:                               return "unk";
   }
}

static const char *
get_cond_modifier_name(unsigned opcode, unsigned cond_modifier)
{
   switch (opcode) {
   case BRW_OPCODE_SEND:
   case BRW_OPCODE_SENDC:
      /* SFID */
      switch (cond_modifier) {
      case BRW_SFID_NULL:                       return "Null";
      case BRW_SFID_SAMPLER:                    return "Sampling Engine";
      case BRW_SFID_MESSAGE_GATEWAY:            return "Message Gateway";
      case GEN6_SFID_DATAPORT_SAMPLER_CACHE:    return "Data Port Sampler Cache";
      case GEN6_SFID_DATAPORT_RENDER_CACHE:     return "Data Port Render Cache";
      case BRW_SFID_URB:                        return "URB";
      case BRW_SFID_THREAD_SPAWNER:             return "Thread Spawner";
      case GEN6_SFID_DATAPORT_CONSTANT_CACHE:   return "Constant Cache";
      default:                                  return "Unknown";
      }
      break;
   case BRW_OPCODE_MATH:
      /* FC */
      switch (cond_modifier) {
      case BRW_MATH_FUNCTION_INV:               return "INV";
      case BRW_MATH_FUNCTION_LOG:               return "LOG";
      case BRW_MATH_FUNCTION_EXP:               return "EXP";
      case BRW_MATH_FUNCTION_SQRT:              return "SQRT";
      case BRW_MATH_FUNCTION_RSQ:               return "RSQ";
      case BRW_MATH_FUNCTION_SIN:               return "SIN";
      case BRW_MATH_FUNCTION_COS:               return "COS";
      case BRW_MATH_FUNCTION_FDIV:              return "FDIV";
      case BRW_MATH_FUNCTION_POW:               return "POW";
      case BRW_MATH_FUNCTION_INT_DIV_QUOTIENT:  return "INT DIV (quotient)";
      case BRW_MATH_FUNCTION_INT_DIV_REMAINDER: return "INT DIV (remainder)";
      default:                                  return "UNK";
      }
      break;
   default:
      switch (cond_modifier) {
      case BRW_CONDITIONAL_NONE:                return NULL;
      case BRW_CONDITIONAL_Z:                   return "z";
      case BRW_CONDITIONAL_NZ:                  return "nz";
      case BRW_CONDITIONAL_G:                   return "g";
      case BRW_CONDITIONAL_GE:                  return "ge";
      case BRW_CONDITIONAL_L:                   return "l";
      case BRW_CONDITIONAL_LE:                  return "le";
      default:                                  return "unk";
      }
      break;
   }
}

/**
 * Dump an instruction.
 */
static void
tc_dump_inst(struct toy_compiler *tc, const struct toy_inst *inst)
{
   const char *name;
   int i;

   name = get_opcode_name(inst->opcode);

   ilo_printf("  %s", name);

   if (inst->opcode == BRW_OPCODE_NOP) {
      ilo_printf("\n");
      return;
   }

   if (inst->saturate)
      ilo_printf(".sat");

   name = get_cond_modifier_name(inst->opcode, inst->cond_modifier);
   if (name)
      ilo_printf(".%s", name);

   ilo_printf(" ");

   tc_dump_dst(tc, inst->dst);

   for (i = 0; i < Elements(inst->src); i++) {
      if (tsrc_is_null(inst->src[i]))
         break;

      ilo_printf(", ");
      tc_dump_src(tc, inst->src[i]);
   }

   ilo_printf("\n");
}

/**
 * Dump the instructions added to the compiler.
 */
void
toy_compiler_dump(struct toy_compiler *tc)
{
   struct toy_inst *inst;
   int pc;

   pc = 0;
   tc_head(tc);
   while ((inst = tc_next_no_skip(tc)) != NULL) {
      /* we do not generate code for markers */
      if (inst->marker)
         ilo_printf("marker:");
      else
         ilo_printf("%6d:", pc++);

      tc_dump_inst(tc, inst);
   }
}

/**
 * Clean up the toy compiler.
 */
void
toy_compiler_cleanup(struct toy_compiler *tc)
{
   struct toy_inst *inst, *next;

   LIST_FOR_EACH_ENTRY_SAFE(inst, next, &tc->instructions, list)
      util_slab_free(&tc->mempool, inst);

   util_slab_destroy(&tc->mempool);
}

/**
 * Initialize the instruction template, from which tc_add() initializes the
 * newly added instructions.
 */
static void
tc_init_inst_templ(struct toy_compiler *tc)
{
   struct toy_inst *templ = &tc->templ;
   int i;

   templ->opcode = BRW_OPCODE_NOP;
   templ->access_mode = BRW_ALIGN_1;
   templ->mask_ctrl = BRW_MASK_ENABLE;
   templ->dep_ctrl = BRW_DEPENDENCY_NORMAL;
   templ->qtr_ctrl = GEN6_COMPRESSION_1Q;
   templ->thread_ctrl = BRW_THREAD_NORMAL;
   templ->pred_ctrl = BRW_PREDICATE_NONE;
   templ->pred_inv = false;
   templ->exec_size = BRW_EXECUTE_1;
   templ->cond_modifier = BRW_CONDITIONAL_NONE;
   templ->acc_wr_ctrl = false;
   templ->saturate = false;

   templ->marker = false;

   templ->dst = tdst_null();
   for (i = 0; i < Elements(templ->src); i++)
      templ->src[i] = tsrc_null();

   for (i = 0; i < Elements(templ->tex.offsets); i++)
      templ->tex.offsets[i] = tsrc_null();

   list_inithead(&templ->list);
}

/**
 * Initialize the toy compiler.
 */
void
toy_compiler_init(struct toy_compiler *tc, const struct ilo_dev_info *dev)
{
   memset(tc, 0, sizeof(*tc));

   tc->dev = dev;

   tc_init_inst_templ(tc);

   util_slab_create(&tc->mempool, sizeof(struct toy_inst),
         64, UTIL_SLAB_SINGLETHREADED);

   list_inithead(&tc->instructions);
   /* instructions are added to the tail */
   tc_tail(tc);

   tc->rect_linear_width = 1;

   /* skip 0 so that util_hash_table_get() never returns NULL */
   tc->next_vrf = 1;
}
