/*
 * Copyright © 2011 Intel Corporation
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

#ifndef BRW_VEC4_H
#define BRW_VEC4_H

#include <stdint.h>
#include "brw_shader.h"
#include "main/compiler.h"
#include "program/hash_table.h"

extern "C" {
#include "brw_vs.h"
#include "brw_context.h"
#include "brw_eu.h"
};

#include "glsl/ir.h"

namespace brw {

class dst_reg;

unsigned
swizzle_for_size(int size);

class reg
{
public:
   /** Register file: ARF, GRF, MRF, IMM. */
   enum register_file file;
   /** virtual register number.  0 = fixed hw reg */
   int reg;
   /** Offset within the virtual register. */
   int reg_offset;
   /** Register type.  BRW_REGISTER_TYPE_* */
   int type;
   struct brw_reg fixed_hw_reg;

   /** Value for file == BRW_IMMMEDIATE_FILE */
   union {
      int32_t i;
      uint32_t u;
      float f;
   } imm;
};

class src_reg : public reg
{
public:
   /* Callers of this ralloc-based new need not call delete. It's
    * easier to just ralloc_free 'ctx' (or any of its ancestors). */
   static void* operator new(size_t size, void *ctx)
   {
      void *node;

      node = ralloc_size(ctx, size);
      assert(node != NULL);

      return node;
   }

   void init();

   src_reg(register_file file, int reg, const glsl_type *type);
   src_reg();
   src_reg(float f);
   src_reg(uint32_t u);
   src_reg(int32_t i);

   bool equals(src_reg *r);
   bool is_zero() const;
   bool is_one() const;

   src_reg(class vec4_visitor *v, const struct glsl_type *type);

   explicit src_reg(dst_reg reg);

   GLuint swizzle; /**< SWIZZLE_XYZW swizzles from Mesa. */
   bool negate;
   bool abs;

   src_reg *reladdr;
};

class dst_reg : public reg
{
public:
   /* Callers of this ralloc-based new need not call delete. It's
    * easier to just ralloc_free 'ctx' (or any of its ancestors). */
   static void* operator new(size_t size, void *ctx)
   {
      void *node;

      node = ralloc_size(ctx, size);
      assert(node != NULL);

      return node;
   }

   void init();

   dst_reg();
   dst_reg(register_file file, int reg);
   dst_reg(register_file file, int reg, const glsl_type *type, int writemask);
   dst_reg(struct brw_reg reg);
   dst_reg(class vec4_visitor *v, const struct glsl_type *type);

   explicit dst_reg(src_reg reg);

   int writemask; /**< Bitfield of WRITEMASK_[XYZW] */

   src_reg *reladdr;
};

class vec4_instruction : public backend_instruction {
public:
   /* Callers of this ralloc-based new need not call delete. It's
    * easier to just ralloc_free 'ctx' (or any of its ancestors). */
   static void* operator new(size_t size, void *ctx)
   {
      void *node;

      node = rzalloc_size(ctx, size);
      assert(node != NULL);

      return node;
   }

   vec4_instruction(vec4_visitor *v, enum opcode opcode,
		    dst_reg dst = dst_reg(),
		    src_reg src0 = src_reg(),
		    src_reg src1 = src_reg(),
		    src_reg src2 = src_reg());

   struct brw_reg get_dst(void);
   struct brw_reg get_src(int i);

   dst_reg dst;
   src_reg src[3];

   bool saturate;
   bool force_writemask_all;
   bool no_dd_clear, no_dd_check;

   int conditional_mod; /**< BRW_CONDITIONAL_* */

   int sampler;
   uint32_t texture_offset; /**< Texture Offset bitfield */
   int target; /**< MRT target. */
   bool shadow_compare;

   bool eot;
   bool header_present;
   int mlen; /**< SEND message length */
   int base_mrf; /**< First MRF in the SEND message, if mlen is nonzero. */

   uint32_t offset; /* spill/unspill offset */
   /** @{
    * Annotation for the generated IR.  One of the two can be set.
    */
   const void *ir;
   const char *annotation;

   bool is_send_from_grf();
   bool can_reswizzle_dst(int dst_writemask, int swizzle, int swizzle_mask);
   void reswizzle_dst(int dst_writemask, int swizzle);
};

/**
 * The vertex shader front-end.
 *
 * Translates either GLSL IR or Mesa IR (for ARB_vertex_program and
 * fixed-function) into VS IR.
 */
class vec4_visitor : public backend_visitor
{
public:
   vec4_visitor(struct brw_context *brw,
                struct brw_vec4_compile *c,
                struct gl_program *prog,
                const struct brw_vec4_prog_key *key,
                struct brw_vec4_prog_data *prog_data,
		struct gl_shader_program *shader_prog,
		struct brw_shader *shader,
		void *mem_ctx,
                bool debug_flag);
   ~vec4_visitor();

   dst_reg dst_null_f()
   {
      return dst_reg(brw_null_reg());
   }

   dst_reg dst_null_d()
   {
      return dst_reg(retype(brw_null_reg(), BRW_REGISTER_TYPE_D));
   }

   struct gl_program *prog;
   struct brw_vec4_compile *c;
   const struct brw_vec4_prog_key *key;
   struct brw_vec4_prog_data *prog_data;
   unsigned int sanity_param_count;

   char *fail_msg;
   bool failed;

   /**
    * GLSL IR currently being processed, which is associated with our
    * driver IR instructions for debugging purposes.
    */
   const void *base_ir;
   const char *current_annotation;

   int *virtual_grf_sizes;
   int virtual_grf_count;
   int virtual_grf_array_size;
   int first_non_payload_grf;
   unsigned int max_grf;
   int *virtual_grf_start;
   int *virtual_grf_end;
   dst_reg userplane[MAX_CLIP_PLANES];

   /**
    * This is the size to be used for an array with an element per
    * reg_offset
    */
   int virtual_grf_reg_count;
   /** Per-virtual-grf indices into an array of size virtual_grf_reg_count */
   int *virtual_grf_reg_map;

   bool live_intervals_valid;

   dst_reg *variable_storage(ir_variable *var);

   void reladdr_to_temp(ir_instruction *ir, src_reg *reg, int *num_reladdr);

   bool need_all_constants_in_pull_buffer;

   /**
    * \name Visit methods
    *
    * As typical for the visitor pattern, there must be one \c visit method for
    * each concrete subclass of \c ir_instruction.  Virtual base classes within
    * the hierarchy should not have \c visit methods.
    */
   /*@{*/
   virtual void visit(ir_variable *);
   virtual void visit(ir_loop *);
   virtual void visit(ir_loop_jump *);
   virtual void visit(ir_function_signature *);
   virtual void visit(ir_function *);
   virtual void visit(ir_expression *);
   virtual void visit(ir_swizzle *);
   virtual void visit(ir_dereference_variable  *);
   virtual void visit(ir_dereference_array *);
   virtual void visit(ir_dereference_record *);
   virtual void visit(ir_assignment *);
   virtual void visit(ir_constant *);
   virtual void visit(ir_call *);
   virtual void visit(ir_return *);
   virtual void visit(ir_discard *);
   virtual void visit(ir_texture *);
   virtual void visit(ir_if *);
   /*@}*/

   src_reg result;

   /* Regs for vertex results.  Generated at ir_variable visiting time
    * for the ir->location's used.
    */
   dst_reg output_reg[BRW_VARYING_SLOT_COUNT];
   const char *output_reg_annotation[BRW_VARYING_SLOT_COUNT];
   int uniform_size[MAX_UNIFORMS];
   int uniform_vector_size[MAX_UNIFORMS];
   int uniforms;

   src_reg shader_start_time;

   struct hash_table *variable_ht;

   bool run(void);
   void fail(const char *msg, ...);

   int virtual_grf_alloc(int size);
   void setup_uniform_clipplane_values();
   void setup_uniform_values(ir_variable *ir);
   void setup_builtin_uniform_values(ir_variable *ir);
   int setup_uniforms(int payload_reg);
   void setup_payload();
   bool reg_allocate_trivial();
   bool reg_allocate();
   void evaluate_spill_costs(float *spill_costs, bool *no_spill);
   int choose_spill_reg(struct ra_graph *g);
   void spill_reg(int spill_reg);
   void move_grf_array_access_to_scratch();
   void move_uniform_array_access_to_pull_constants();
   void move_push_constants_to_pull_constants();
   void split_uniform_registers();
   void pack_uniform_registers();
   void calculate_live_intervals();
   void split_virtual_grfs();
   bool dead_code_eliminate();
   bool virtual_grf_interferes(int a, int b);
   bool opt_copy_propagation();
   bool opt_algebraic();
   bool opt_register_coalesce();
   void opt_set_dependency_control();
   void opt_schedule_instructions();

   bool can_do_source_mods(vec4_instruction *inst);

   vec4_instruction *emit(vec4_instruction *inst);

   vec4_instruction *emit(enum opcode opcode);

   vec4_instruction *emit(enum opcode opcode, dst_reg dst, src_reg src0);

   vec4_instruction *emit(enum opcode opcode, dst_reg dst,
			  src_reg src0, src_reg src1);

   vec4_instruction *emit(enum opcode opcode, dst_reg dst,
			  src_reg src0, src_reg src1, src_reg src2);

   vec4_instruction *emit_before(vec4_instruction *inst,
				 vec4_instruction *new_inst);

   vec4_instruction *MOV(dst_reg dst, src_reg src0);
   vec4_instruction *NOT(dst_reg dst, src_reg src0);
   vec4_instruction *RNDD(dst_reg dst, src_reg src0);
   vec4_instruction *RNDE(dst_reg dst, src_reg src0);
   vec4_instruction *RNDZ(dst_reg dst, src_reg src0);
   vec4_instruction *FRC(dst_reg dst, src_reg src0);
   vec4_instruction *F32TO16(dst_reg dst, src_reg src0);
   vec4_instruction *F16TO32(dst_reg dst, src_reg src0);
   vec4_instruction *ADD(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *MUL(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *MACH(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *MAC(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *AND(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *OR(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *XOR(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *DP3(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *DP4(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *DPH(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *SHL(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *SHR(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *ASR(dst_reg dst, src_reg src0, src_reg src1);
   vec4_instruction *CMP(dst_reg dst, src_reg src0, src_reg src1,
			 uint32_t condition);
   vec4_instruction *IF(src_reg src0, src_reg src1, uint32_t condition);
   vec4_instruction *IF(uint32_t predicate);
   vec4_instruction *PULL_CONSTANT_LOAD(dst_reg dst, src_reg index);
   vec4_instruction *SCRATCH_READ(dst_reg dst, src_reg index);
   vec4_instruction *SCRATCH_WRITE(dst_reg dst, src_reg src, src_reg index);
   vec4_instruction *LRP(dst_reg dst, src_reg a, src_reg y, src_reg x);
   vec4_instruction *BFREV(dst_reg dst, src_reg value);
   vec4_instruction *BFE(dst_reg dst, src_reg bits, src_reg offset, src_reg value);
   vec4_instruction *BFI1(dst_reg dst, src_reg bits, src_reg offset);
   vec4_instruction *BFI2(dst_reg dst, src_reg bfi1_dst, src_reg insert, src_reg base);
   vec4_instruction *FBH(dst_reg dst, src_reg value);
   vec4_instruction *FBL(dst_reg dst, src_reg value);
   vec4_instruction *CBIT(dst_reg dst, src_reg value);

   int implied_mrf_writes(vec4_instruction *inst);

   bool try_rewrite_rhs_to_dst(ir_assignment *ir,
			       dst_reg dst,
			       src_reg src,
			       vec4_instruction *pre_rhs_inst,
			       vec4_instruction *last_rhs_inst);

   bool try_copy_propagation(vec4_instruction *inst, int arg,
                             src_reg *values[4]);

   /** Walks an exec_list of ir_instruction and sends it through this visitor. */
   void visit_instructions(const exec_list *list);

   void emit_vp_sop(uint32_t condmod, dst_reg dst,
                    src_reg src0, src_reg src1, src_reg one);

   void emit_bool_to_cond_code(ir_rvalue *ir, uint32_t *predicate);
   void emit_bool_comparison(unsigned int op, dst_reg dst, src_reg src0, src_reg src1);
   void emit_if_gen6(ir_if *ir);

   void emit_minmax(uint32_t condmod, dst_reg dst, src_reg src0, src_reg src1);

   void emit_block_move(dst_reg *dst, src_reg *src,
			const struct glsl_type *type, uint32_t predicate);

   void emit_constant_values(dst_reg *dst, ir_constant *value);

   /**
    * Emit the correct dot-product instruction for the type of arguments
    */
   void emit_dp(dst_reg dst, src_reg src0, src_reg src1, unsigned elements);

   void emit_scalar(ir_instruction *ir, enum prog_opcode op,
		    dst_reg dst, src_reg src0);

   void emit_scalar(ir_instruction *ir, enum prog_opcode op,
		    dst_reg dst, src_reg src0, src_reg src1);

   void emit_scs(ir_instruction *ir, enum prog_opcode op,
		 dst_reg dst, const src_reg &src);

   src_reg fix_3src_operand(src_reg src);

   void emit_math1_gen6(enum opcode opcode, dst_reg dst, src_reg src);
   void emit_math1_gen4(enum opcode opcode, dst_reg dst, src_reg src);
   void emit_math(enum opcode opcode, dst_reg dst, src_reg src);
   void emit_math2_gen6(enum opcode opcode, dst_reg dst, src_reg src0, src_reg src1);
   void emit_math2_gen4(enum opcode opcode, dst_reg dst, src_reg src0, src_reg src1);
   void emit_math(enum opcode opcode, dst_reg dst, src_reg src0, src_reg src1);
   src_reg fix_math_operand(src_reg src);

   void emit_pack_half_2x16(dst_reg dst, src_reg src0);
   void emit_unpack_half_2x16(dst_reg dst, src_reg src0);

   void swizzle_result(ir_texture *ir, src_reg orig_val, int sampler);

   void emit_ndc_computation();
   void emit_psiz_and_flags(struct brw_reg reg);
   void emit_clip_distances(struct brw_reg reg, int offset);
   void emit_generic_urb_slot(dst_reg reg, int varying);
   void emit_urb_slot(int mrf, int varying);

   void emit_shader_time_begin();
   void emit_shader_time_end();
   void emit_shader_time_write(enum shader_time_shader_type type,
                               src_reg value);

   src_reg get_scratch_offset(vec4_instruction *inst,
			      src_reg *reladdr, int reg_offset);
   src_reg get_pull_constant_offset(vec4_instruction *inst,
				    src_reg *reladdr, int reg_offset);
   void emit_scratch_read(vec4_instruction *inst,
			  dst_reg dst,
			  src_reg orig_src,
			  int base_offset);
   void emit_scratch_write(vec4_instruction *inst,
			   int base_offset);
   void emit_pull_constant_load(vec4_instruction *inst,
				dst_reg dst,
				src_reg orig_src,
				int base_offset);

   bool try_emit_sat(ir_expression *ir);
   bool try_emit_mad(ir_expression *ir, int mul_arg);
   void resolve_ud_negate(src_reg *reg);

   src_reg get_timestamp();

   bool process_move_condition(ir_rvalue *ir);

   void dump_instruction(backend_instruction *inst);

protected:
   void emit_vertex();
   void lower_attributes_to_hw_regs(const int *attribute_map);
   virtual dst_reg *make_reg_for_system_value(ir_variable *ir) = 0;
   virtual int setup_attributes(int payload_reg) = 0;
   virtual void emit_prolog() = 0;
   virtual void emit_program_code() = 0;
   virtual void emit_thread_end() = 0;
   virtual void emit_urb_write_header(int mrf) = 0;
   virtual vec4_instruction *emit_urb_write_opcode(bool complete) = 0;
   virtual int compute_array_stride(ir_dereference_array *ir);

   const bool debug_flag;
};

class vec4_vs_visitor : public vec4_visitor
{
public:
   vec4_vs_visitor(struct brw_context *brw,
                   struct brw_vs_compile *vs_compile,
                   struct brw_vs_prog_data *vs_prog_data,
                   struct gl_shader_program *prog,
                   struct brw_shader *shader,
                   void *mem_ctx);

protected:
   virtual dst_reg *make_reg_for_system_value(ir_variable *ir);
   virtual int setup_attributes(int payload_reg);
   virtual void emit_prolog();
   virtual void emit_program_code();
   virtual void emit_thread_end();
   virtual void emit_urb_write_header(int mrf);
   virtual vec4_instruction *emit_urb_write_opcode(bool complete);

private:
   void setup_vp_regs();
   dst_reg get_vp_dst_reg(const prog_dst_register &dst);
   src_reg get_vp_src_reg(const prog_src_register &src);

   struct brw_vs_compile * const vs_compile;
   struct brw_vs_prog_data * const vs_prog_data;
   src_reg *vp_temp_regs;
   src_reg vp_addr_reg;
};

/**
 * The vertex shader code generator.
 *
 * Translates VS IR to actual i965 assembly code.
 */
class vec4_generator
{
public:
   vec4_generator(struct brw_context *brw,
                  struct gl_shader_program *shader_prog,
                  struct gl_program *prog,
                  void *mem_ctx,
                  bool debug_flag);
   ~vec4_generator();

   const unsigned *generate_assembly(exec_list *insts, unsigned *asm_size);

private:
   void generate_code(exec_list *instructions);
   void generate_vec4_instruction(vec4_instruction *inst,
                                  struct brw_reg dst,
                                  struct brw_reg *src);

   void generate_math1_gen4(vec4_instruction *inst,
			    struct brw_reg dst,
			    struct brw_reg src);
   void generate_math1_gen6(vec4_instruction *inst,
			    struct brw_reg dst,
			    struct brw_reg src);
   void generate_math2_gen4(vec4_instruction *inst,
			    struct brw_reg dst,
			    struct brw_reg src0,
			    struct brw_reg src1);
   void generate_math2_gen6(vec4_instruction *inst,
			    struct brw_reg dst,
			    struct brw_reg src0,
			    struct brw_reg src1);
   void generate_math2_gen7(vec4_instruction *inst,
			    struct brw_reg dst,
			    struct brw_reg src0,
			    struct brw_reg src1);

   void generate_tex(vec4_instruction *inst,
		     struct brw_reg dst,
		     struct brw_reg src);

   void generate_urb_write(vec4_instruction *inst);
   void generate_oword_dual_block_offsets(struct brw_reg m1,
					  struct brw_reg index);
   void generate_scratch_write(vec4_instruction *inst,
			       struct brw_reg dst,
			       struct brw_reg src,
			       struct brw_reg index);
   void generate_scratch_read(vec4_instruction *inst,
			      struct brw_reg dst,
			      struct brw_reg index);
   void generate_pull_constant_load(vec4_instruction *inst,
				    struct brw_reg dst,
				    struct brw_reg index,
				    struct brw_reg offset);
   void generate_pull_constant_load_gen7(vec4_instruction *inst,
                                         struct brw_reg dst,
                                         struct brw_reg surf_index,
                                         struct brw_reg offset);

   struct brw_context *brw;
   struct gl_context *ctx;

   struct brw_compile *p;

   struct gl_shader_program *shader_prog;
   struct gl_shader *shader;
   const struct gl_program *prog;

   void *mem_ctx;
   const bool debug_flag;
};

} /* namespace brw */

#endif /* BRW_VEC4_H */
