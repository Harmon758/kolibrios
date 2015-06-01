/*
 * Copyright © 2014 Broadcom
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

#ifndef VC4_QPU_H
#define VC4_QPU_H

#include <stdint.h>

#include "util/u_math.h"

#include "vc4_qpu_defines.h"

struct vc4_compile;

struct qpu_reg {
        enum qpu_mux mux;
        uint8_t addr;
};

static inline struct qpu_reg
qpu_rn(int n)
{
        struct qpu_reg r = {
                QPU_MUX_R0 + n,
                0,
        };

        return r;
}

static inline struct qpu_reg
qpu_ra(int addr)
{
        struct qpu_reg r = {
                QPU_MUX_A,
                addr,
        };

        return r;
}

static inline struct qpu_reg
qpu_rb(int addr)
{
        struct qpu_reg r = {
                QPU_MUX_B,
                addr,
        };

        return r;
}

static inline struct qpu_reg
qpu_vary()
{
        struct qpu_reg r = {
                QPU_MUX_A,
                QPU_R_VARY,
        };

        return r;
}

static inline struct qpu_reg
qpu_unif()
{
        struct qpu_reg r = {
                QPU_MUX_A,
                QPU_R_UNIF,
        };

        return r;
}

static inline struct qpu_reg
qpu_vrsetup()
{
        return qpu_ra(QPU_W_VPMVCD_SETUP);
}

static inline struct qpu_reg
qpu_vwsetup()
{
        return qpu_rb(QPU_W_VPMVCD_SETUP);
}

static inline struct qpu_reg
qpu_tlbc()
{
        struct qpu_reg r = {
                QPU_MUX_A,
                QPU_W_TLB_COLOR_ALL,
        };

        return r;
}

static inline struct qpu_reg qpu_r0(void) { return qpu_rn(0); }
static inline struct qpu_reg qpu_r1(void) { return qpu_rn(1); }
static inline struct qpu_reg qpu_r2(void) { return qpu_rn(2); }
static inline struct qpu_reg qpu_r3(void) { return qpu_rn(3); }
static inline struct qpu_reg qpu_r4(void) { return qpu_rn(4); }
static inline struct qpu_reg qpu_r5(void) { return qpu_rn(5); }

uint64_t qpu_NOP(void);
uint64_t qpu_a_MOV(struct qpu_reg dst, struct qpu_reg src);
uint64_t qpu_m_MOV(struct qpu_reg dst, struct qpu_reg src);
uint64_t qpu_a_alu2(enum qpu_op_add op, struct qpu_reg dst,
                    struct qpu_reg src0, struct qpu_reg src1);
uint64_t qpu_m_alu2(enum qpu_op_mul op, struct qpu_reg dst,
                    struct qpu_reg src0, struct qpu_reg src1);
uint64_t qpu_merge_inst(uint64_t a, uint64_t b);
uint64_t qpu_load_imm_ui(struct qpu_reg dst, uint32_t val);
uint64_t qpu_set_sig(uint64_t inst, uint32_t sig);
uint64_t qpu_set_cond_add(uint64_t inst, uint32_t cond);
uint64_t qpu_set_cond_mul(uint64_t inst, uint32_t cond);
uint32_t qpu_encode_small_immediate(uint32_t i);

bool qpu_waddr_is_tlb(uint32_t waddr);
bool qpu_inst_is_tlb(uint64_t inst);
int qpu_num_sf_accesses(uint64_t inst);
void qpu_serialize_one_inst(struct vc4_compile *c, uint64_t inst);

static inline uint64_t
qpu_load_imm_f(struct qpu_reg dst, float val)
{
        return qpu_load_imm_ui(dst, fui(val));
}

#define A_ALU2(op)                                                       \
static inline uint64_t                                                   \
qpu_a_##op(struct qpu_reg dst, struct qpu_reg src0, struct qpu_reg src1) \
{                                                                        \
        return qpu_a_alu2(QPU_A_##op, dst, src0, src1);                  \
}

#define M_ALU2(op)                                                       \
static inline uint64_t                                                   \
qpu_m_##op(struct qpu_reg dst, struct qpu_reg src0, struct qpu_reg src1) \
{                                                                        \
        return qpu_m_alu2(QPU_M_##op, dst, src0, src1);                  \
}

#define A_ALU1(op)                                                       \
static inline uint64_t                                                   \
qpu_a_##op(struct qpu_reg dst, struct qpu_reg src0)                      \
{                                                                        \
        return qpu_a_alu2(QPU_A_##op, dst, src0, src0);                  \
}

/*A_ALU2(NOP) */
A_ALU2(FADD)
A_ALU2(FSUB)
A_ALU2(FMIN)
A_ALU2(FMAX)
A_ALU2(FMINABS)
A_ALU2(FMAXABS)
A_ALU1(FTOI)
A_ALU1(ITOF)
A_ALU2(ADD)
A_ALU2(SUB)
A_ALU2(SHR)
A_ALU2(ASR)
A_ALU2(ROR)
A_ALU2(SHL)
A_ALU2(MIN)
A_ALU2(MAX)
A_ALU2(AND)
A_ALU2(OR)
A_ALU2(XOR)
A_ALU1(NOT)
A_ALU1(CLZ)
A_ALU2(V8ADDS)
A_ALU2(V8SUBS)

/* M_ALU2(NOP) */
M_ALU2(FMUL)
M_ALU2(MUL24)
M_ALU2(V8MULD)
M_ALU2(V8MIN)
M_ALU2(V8MAX)
M_ALU2(V8ADDS)
M_ALU2(V8SUBS)

void
vc4_qpu_disasm(const uint64_t *instructions, int num_instructions);

void
vc4_qpu_validate(uint64_t *insts, uint32_t num_inst);

#endif /* VC4_QPU_H */
