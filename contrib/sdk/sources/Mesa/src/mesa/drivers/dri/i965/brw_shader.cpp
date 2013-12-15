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
 */

extern "C" {
#include "main/macros.h"
#include "brw_context.h"
#include "brw_vs.h"
}
#include "brw_fs.h"
#include "glsl/ir_optimization.h"
#include "glsl/glsl_parser_extras.h"

struct gl_shader *
brw_new_shader(struct gl_context *ctx, GLuint name, GLuint type)
{
   struct brw_shader *shader;

   shader = rzalloc(NULL, struct brw_shader);
   if (shader) {
      shader->base.Type = type;
      shader->base.Name = name;
      _mesa_init_shader(ctx, &shader->base);
   }

   return &shader->base;
}

struct gl_shader_program *
brw_new_shader_program(struct gl_context *ctx, GLuint name)
{
   struct gl_shader_program *prog = rzalloc(NULL, struct gl_shader_program);
   if (prog) {
      prog->Name = name;
      _mesa_init_shader_program(ctx, prog);
   }
   return prog;
}

/**
 * Performs a compile of the shader stages even when we don't know
 * what non-orthogonal state will be set, in the hope that it reflects
 * the eventual NOS used, and thus allows us to produce link failures.
 */
static bool
brw_shader_precompile(struct gl_context *ctx, struct gl_shader_program *prog)
{
   struct brw_context *brw = brw_context(ctx);

   if (brw->precompile && !brw_fs_precompile(ctx, prog))
      return false;

   if (brw->precompile && !brw_vs_precompile(ctx, prog))
      return false;

   return true;
}

static void
brw_lower_packing_builtins(struct brw_context *brw,
                           gl_shader_type shader_type,
                           exec_list *ir)
{
   int ops = LOWER_PACK_SNORM_2x16
           | LOWER_UNPACK_SNORM_2x16
           | LOWER_PACK_UNORM_2x16
           | LOWER_UNPACK_UNORM_2x16
           | LOWER_PACK_SNORM_4x8
           | LOWER_UNPACK_SNORM_4x8
           | LOWER_PACK_UNORM_4x8
           | LOWER_UNPACK_UNORM_4x8;

   if (brw->gen >= 7) {
      /* Gen7 introduced the f32to16 and f16to32 instructions, which can be
       * used to execute packHalf2x16 and unpackHalf2x16. For AOS code, no
       * lowering is needed. For SOA code, the Half2x16 ops must be
       * scalarized.
       */
      if (shader_type == MESA_SHADER_FRAGMENT) {
         ops |= LOWER_PACK_HALF_2x16_TO_SPLIT
             |  LOWER_UNPACK_HALF_2x16_TO_SPLIT;
      }
   } else {
      ops |= LOWER_PACK_HALF_2x16
          |  LOWER_UNPACK_HALF_2x16;
   }

   lower_packing_builtins(ir, ops);
}

GLboolean
brw_link_shader(struct gl_context *ctx, struct gl_shader_program *shProg)
{
   struct brw_context *brw = brw_context(ctx);
   unsigned int stage;

   for (stage = 0; stage < ARRAY_SIZE(shProg->_LinkedShaders); stage++) {
      struct brw_shader *shader =
	 (struct brw_shader *)shProg->_LinkedShaders[stage];

      if (!shader)
	 continue;

      struct gl_program *prog =
	 ctx->Driver.NewProgram(ctx, _mesa_program_index_to_target(stage),
                                shader->base.Name);
      if (!prog)
	return false;
      prog->Parameters = _mesa_new_parameter_list();

      if (stage == 0) {
	 struct gl_vertex_program *vp = (struct gl_vertex_program *) prog;
	 vp->UsesClipDistance = shProg->Vert.UsesClipDistance;
      }

      void *mem_ctx = ralloc_context(NULL);
      bool progress;

      if (shader->ir)
	 ralloc_free(shader->ir);
      shader->ir = new(shader) exec_list;
      clone_ir_list(mem_ctx, shader->ir, shader->base.ir);

      /* lower_packing_builtins() inserts arithmetic instructions, so it
       * must precede lower_instructions().
       */
      brw_lower_packing_builtins(brw, (gl_shader_type) stage, shader->ir);
      do_mat_op_to_vec(shader->ir);
      const int bitfield_insert = brw->gen >= 7
                                  ? BITFIELD_INSERT_TO_BFM_BFI
                                  : 0;
      const int lrp_to_arith = brw->gen < 6 ? LRP_TO_ARITH : 0;
      lower_instructions(shader->ir,
			 MOD_TO_FRACT |
			 DIV_TO_MUL_RCP |
			 SUB_TO_ADD_NEG |
			 EXP_TO_EXP2 |
			 LOG_TO_LOG2 |
                         bitfield_insert |
                         lrp_to_arith);

      /* Pre-gen6 HW can only nest if-statements 16 deep.  Beyond this,
       * if-statements need to be flattened.
       */
      if (brw->gen < 6)
	 lower_if_to_cond_assign(shader->ir, 16);

      do_lower_texture_projection(shader->ir);
      brw_lower_texture_gradients(brw, shader->ir);
      do_vec_index_to_cond_assign(shader->ir);
      lower_vector_insert(shader->ir, true);
      brw_do_cubemap_normalize(shader->ir);
      lower_noise(shader->ir);
      lower_quadop_vector(shader->ir, false);

      bool input = true;
      bool output = stage == MESA_SHADER_FRAGMENT;
      bool temp = stage == MESA_SHADER_FRAGMENT;
      bool uniform = false;

      bool lowered_variable_indexing =
         lower_variable_index_to_cond_assign(shader->ir,
                                             input, output, temp, uniform);

      if (unlikely(brw->perf_debug && lowered_variable_indexing)) {
         perf_debug("Unsupported form of variable indexing in FS; falling "
                    "back to very inefficient code generation\n");
      }

      /* FINISHME: Do this before the variable index lowering. */
      lower_ubo_reference(&shader->base, shader->ir);

      do {
	 progress = false;

	 if (stage == MESA_SHADER_FRAGMENT) {
	    brw_do_channel_expressions(shader->ir);
	    brw_do_vector_splitting(shader->ir);
	 }

	 progress = do_lower_jumps(shader->ir, true, true,
				   true, /* main return */
				   false, /* continue */
				   false /* loops */
				   ) || progress;

	 progress = do_common_optimization(shader->ir, true, true, 32,
                                           &ctx->ShaderCompilerOptions[stage])
	   || progress;
      } while (progress);

      /* Make a pass over the IR to add state references for any built-in
       * uniforms that are used.  This has to be done now (during linking).
       * Code generation doesn't happen until the first time this shader is
       * used for rendering.  Waiting until then to generate the parameters is
       * too late.  At that point, the values for the built-in uniforms won't
       * get sent to the shader.
       */
      foreach_list(node, shader->ir) {
	 ir_variable *var = ((ir_instruction *) node)->as_variable();

	 if ((var == NULL) || (var->mode != ir_var_uniform)
	     || (strncmp(var->name, "gl_", 3) != 0))
	    continue;

	 const ir_state_slot *const slots = var->state_slots;
	 assert(var->state_slots != NULL);

	 for (unsigned int i = 0; i < var->num_state_slots; i++) {
	    _mesa_add_state_reference(prog->Parameters,
				      (gl_state_index *) slots[i].tokens);
	 }
      }

      validate_ir_tree(shader->ir);

      reparent_ir(shader->ir, shader->ir);
      ralloc_free(mem_ctx);

      do_set_program_inouts(shader->ir, prog,
			    shader->base.Type == GL_FRAGMENT_SHADER);

      prog->SamplersUsed = shader->base.active_samplers;
      _mesa_update_shader_textures_used(shProg, prog);

      _mesa_reference_program(ctx, &shader->base.Program, prog);

      brw_add_texrect_params(prog);

      /* This has to be done last.  Any operation that can cause
       * prog->ParameterValues to get reallocated (e.g., anything that adds a
       * program constant) has to happen before creating this linkage.
       */
      _mesa_associate_uniform_storage(ctx, shProg, prog->Parameters);

      _mesa_reference_program(ctx, &prog, NULL);

      if (ctx->Shader.Flags & GLSL_DUMP) {
         printf("\n");
         printf("GLSL IR for linked %s program %d:\n",
                _mesa_glsl_shader_target_name(shader->base.Type), shProg->Name);
         _mesa_print_ir(shader->base.ir, NULL);
         printf("\n");
      }
   }

   if (ctx->Shader.Flags & GLSL_DUMP) {
      for (unsigned i = 0; i < shProg->NumShaders; i++) {
         const struct gl_shader *sh = shProg->Shaders[i];
         if (!sh)
            continue;

         printf("GLSL %s shader %d source for linked program %d:\n",
                _mesa_glsl_shader_target_name(sh->Type),
                i,
                shProg->Name);
         printf("%s", sh->Source);
         printf("\n");
      }
   }

   if (!brw_shader_precompile(ctx, shProg))
      return false;

   return true;
}


int
brw_type_for_base_type(const struct glsl_type *type)
{
   switch (type->base_type) {
   case GLSL_TYPE_FLOAT:
      return BRW_REGISTER_TYPE_F;
   case GLSL_TYPE_INT:
   case GLSL_TYPE_BOOL:
      return BRW_REGISTER_TYPE_D;
   case GLSL_TYPE_UINT:
      return BRW_REGISTER_TYPE_UD;
   case GLSL_TYPE_ARRAY:
      return brw_type_for_base_type(type->fields.array);
   case GLSL_TYPE_STRUCT:
   case GLSL_TYPE_SAMPLER:
      /* These should be overridden with the type of the member when
       * dereferenced into.  BRW_REGISTER_TYPE_UD seems like a likely
       * way to trip up if we don't.
       */
      return BRW_REGISTER_TYPE_UD;
   case GLSL_TYPE_VOID:
   case GLSL_TYPE_ERROR:
   case GLSL_TYPE_INTERFACE:
      assert(!"not reached");
      break;
   }

   return BRW_REGISTER_TYPE_F;
}

uint32_t
brw_conditional_for_comparison(unsigned int op)
{
   switch (op) {
   case ir_binop_less:
      return BRW_CONDITIONAL_L;
   case ir_binop_greater:
      return BRW_CONDITIONAL_G;
   case ir_binop_lequal:
      return BRW_CONDITIONAL_LE;
   case ir_binop_gequal:
      return BRW_CONDITIONAL_GE;
   case ir_binop_equal:
   case ir_binop_all_equal: /* same as equal for scalars */
      return BRW_CONDITIONAL_Z;
   case ir_binop_nequal:
   case ir_binop_any_nequal: /* same as nequal for scalars */
      return BRW_CONDITIONAL_NZ;
   default:
      assert(!"not reached: bad operation for comparison");
      return BRW_CONDITIONAL_NZ;
   }
}

uint32_t
brw_math_function(enum opcode op)
{
   switch (op) {
   case SHADER_OPCODE_RCP:
      return BRW_MATH_FUNCTION_INV;
   case SHADER_OPCODE_RSQ:
      return BRW_MATH_FUNCTION_RSQ;
   case SHADER_OPCODE_SQRT:
      return BRW_MATH_FUNCTION_SQRT;
   case SHADER_OPCODE_EXP2:
      return BRW_MATH_FUNCTION_EXP;
   case SHADER_OPCODE_LOG2:
      return BRW_MATH_FUNCTION_LOG;
   case SHADER_OPCODE_POW:
      return BRW_MATH_FUNCTION_POW;
   case SHADER_OPCODE_SIN:
      return BRW_MATH_FUNCTION_SIN;
   case SHADER_OPCODE_COS:
      return BRW_MATH_FUNCTION_COS;
   case SHADER_OPCODE_INT_QUOTIENT:
      return BRW_MATH_FUNCTION_INT_DIV_QUOTIENT;
   case SHADER_OPCODE_INT_REMAINDER:
      return BRW_MATH_FUNCTION_INT_DIV_REMAINDER;
   default:
      assert(!"not reached: unknown math function");
      return 0;
   }
}

uint32_t
brw_texture_offset(ir_constant *offset)
{
   assert(offset != NULL);

   signed char offsets[3];
   for (unsigned i = 0; i < offset->type->vector_elements; i++)
      offsets[i] = (signed char) offset->value.i[i];

   /* Combine all three offsets into a single unsigned dword:
    *
    *    bits 11:8 - U Offset (X component)
    *    bits  7:4 - V Offset (Y component)
    *    bits  3:0 - R Offset (Z component)
    */
   unsigned offset_bits = 0;
   for (unsigned i = 0; i < offset->type->vector_elements; i++) {
      const unsigned shift = 4 * (2 - i);
      offset_bits |= (offsets[i] << shift) & (0xF << shift);
   }
   return offset_bits;
}

const char *
brw_instruction_name(enum opcode op)
{
   char *fallback;

   if (op < ARRAY_SIZE(opcode_descs) && opcode_descs[op].name)
      return opcode_descs[op].name;

   switch (op) {
   case FS_OPCODE_FB_WRITE:
      return "fb_write";

   case SHADER_OPCODE_RCP:
      return "rcp";
   case SHADER_OPCODE_RSQ:
      return "rsq";
   case SHADER_OPCODE_SQRT:
      return "sqrt";
   case SHADER_OPCODE_EXP2:
      return "exp2";
   case SHADER_OPCODE_LOG2:
      return "log2";
   case SHADER_OPCODE_POW:
      return "pow";
   case SHADER_OPCODE_INT_QUOTIENT:
      return "int_quot";
   case SHADER_OPCODE_INT_REMAINDER:
      return "int_rem";
   case SHADER_OPCODE_SIN:
      return "sin";
   case SHADER_OPCODE_COS:
      return "cos";

   case SHADER_OPCODE_TEX:
      return "tex";
   case SHADER_OPCODE_TXD:
      return "txd";
   case SHADER_OPCODE_TXF:
      return "txf";
   case SHADER_OPCODE_TXL:
      return "txl";
   case SHADER_OPCODE_TXS:
      return "txs";
   case FS_OPCODE_TXB:
      return "txb";
   case SHADER_OPCODE_TXF_MS:
      return "txf_ms";

   case FS_OPCODE_DDX:
      return "ddx";
   case FS_OPCODE_DDY:
      return "ddy";

   case FS_OPCODE_PIXEL_X:
      return "pixel_x";
   case FS_OPCODE_PIXEL_Y:
      return "pixel_y";

   case FS_OPCODE_CINTERP:
      return "cinterp";
   case FS_OPCODE_LINTERP:
      return "linterp";

   case FS_OPCODE_SPILL:
      return "spill";
   case FS_OPCODE_UNSPILL:
      return "unspill";

   case FS_OPCODE_UNIFORM_PULL_CONSTANT_LOAD:
      return "uniform_pull_const";
   case FS_OPCODE_UNIFORM_PULL_CONSTANT_LOAD_GEN7:
      return "uniform_pull_const_gen7";
   case FS_OPCODE_VARYING_PULL_CONSTANT_LOAD:
      return "varying_pull_const";
   case FS_OPCODE_VARYING_PULL_CONSTANT_LOAD_GEN7:
      return "varying_pull_const_gen7";

   case FS_OPCODE_MOV_DISPATCH_TO_FLAGS:
      return "mov_dispatch_to_flags";
   case FS_OPCODE_DISCARD_JUMP:
      return "discard_jump";

   case FS_OPCODE_SET_SIMD4X2_OFFSET:
      return "set_simd4x2_offset";

   case FS_OPCODE_PACK_HALF_2x16_SPLIT:
      return "pack_half_2x16_split";
   case FS_OPCODE_UNPACK_HALF_2x16_SPLIT_X:
      return "unpack_half_2x16_split_x";
   case FS_OPCODE_UNPACK_HALF_2x16_SPLIT_Y:
      return "unpack_half_2x16_split_y";

   case FS_OPCODE_PLACEHOLDER_HALT:
      return "placeholder_halt";

   case VS_OPCODE_URB_WRITE:
      return "urb_write";
   case VS_OPCODE_SCRATCH_READ:
      return "scratch_read";
   case VS_OPCODE_SCRATCH_WRITE:
      return "scratch_write";
   case VS_OPCODE_PULL_CONSTANT_LOAD:
      return "pull_constant_load";
   case VS_OPCODE_PULL_CONSTANT_LOAD_GEN7:
      return "pull_constant_load_gen7";

   default:
      /* Yes, this leaks.  It's in debug code, it should never occur, and if
       * it does, you should just add the case to the list above.
       */
//      asprintf(&fallback, "op%d", op);
      return fallback;
   }
}

bool
backend_instruction::is_tex()
{
   return (opcode == SHADER_OPCODE_TEX ||
           opcode == FS_OPCODE_TXB ||
           opcode == SHADER_OPCODE_TXD ||
           opcode == SHADER_OPCODE_TXF ||
           opcode == SHADER_OPCODE_TXF_MS ||
           opcode == SHADER_OPCODE_TXL ||
           opcode == SHADER_OPCODE_TXS ||
           opcode == SHADER_OPCODE_LOD);
}

bool
backend_instruction::is_math()
{
   return (opcode == SHADER_OPCODE_RCP ||
           opcode == SHADER_OPCODE_RSQ ||
           opcode == SHADER_OPCODE_SQRT ||
           opcode == SHADER_OPCODE_EXP2 ||
           opcode == SHADER_OPCODE_LOG2 ||
           opcode == SHADER_OPCODE_SIN ||
           opcode == SHADER_OPCODE_COS ||
           opcode == SHADER_OPCODE_INT_QUOTIENT ||
           opcode == SHADER_OPCODE_INT_REMAINDER ||
           opcode == SHADER_OPCODE_POW);
}

bool
backend_instruction::is_control_flow()
{
   switch (opcode) {
   case BRW_OPCODE_DO:
   case BRW_OPCODE_WHILE:
   case BRW_OPCODE_IF:
   case BRW_OPCODE_ELSE:
   case BRW_OPCODE_ENDIF:
   case BRW_OPCODE_BREAK:
   case BRW_OPCODE_CONTINUE:
      return true;
   default:
      return false;
   }
}

void
backend_visitor::dump_instructions()
{
   int ip = 0;
   foreach_list(node, &this->instructions) {
      backend_instruction *inst = (backend_instruction *)node;
      printf("%d: ", ip++);
      dump_instruction(inst);
   }
}
