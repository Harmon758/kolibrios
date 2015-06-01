/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 2014 LunarG, Inc.
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
 */

#ifndef GENHW_H
#define GENHW_H

#include "pipe/p_compiler.h"
#include "util/u_debug.h"

#include "gen_regs.xml.h"
#include "gen_mi.xml.h"
#include "gen_blitter.xml.h"
#include "gen_render.xml.h"
#include "gen_render_surface.xml.h"
#include "gen_render_dynamic.xml.h"
#include "gen_render_3d.xml.h"
#include "gen_render_media.xml.h"
#include "gen_eu_isa.xml.h"
#include "gen_eu_message.xml.h"

#define GEN_MI_CMD(gen, op) (GEN6_MI_TYPE_MI | gen ## _MI_OPCODE_ ## op)
#define GEN6_MI_CMD(op) GEN_MI_CMD(GEN6, op)
#define GEN7_MI_CMD(op) GEN_MI_CMD(GEN7, op)

#define GEN_BLITTER_CMD(gen, op) \
   (GEN6_BLITTER_TYPE_BLITTER | gen ## _BLITTER_OPCODE_ ## op)
#define GEN6_BLITTER_CMD(op) GEN_BLITTER_CMD(GEN6, op)

#define GEN_RENDER_CMD(subtype, gen, op)  \
   (GEN6_RENDER_TYPE_RENDER |             \
    GEN6_RENDER_SUBTYPE_ ## subtype |     \
    gen ## _RENDER_OPCODE_ ## op)
#define GEN6_RENDER_CMD(subtype, op) GEN_RENDER_CMD(subtype, GEN6, op)
#define GEN7_RENDER_CMD(subtype, op) GEN_RENDER_CMD(subtype, GEN7, op)
#define GEN75_RENDER_CMD(subtype, op) GEN_RENDER_CMD(subtype, GEN75, op)
#define GEN8_RENDER_CMD(subtype, op) GEN_RENDER_CMD(subtype, GEN8, op)

#define GEN_EXTRACT(bits, field) (((bits) & field ## __MASK) >> field ## __SHIFT)
#define GEN_SHIFT32(bits, field) gen_shift32(bits, field ## __MASK, field ## __SHIFT)

static inline uint32_t
gen_shift32(uint32_t bits, uint32_t mask, int shift)
{
   bits <<= shift;

   assert((bits & mask) == bits);
   return bits & mask;
}

static inline bool
gen_is_snb(int devid)
{
   return (devid == 0x0102 || /* GT1 desktop */
           devid == 0x0112 || /* GT2 desktop */
           devid == 0x0122 || /* GT2_PLUS desktop */
           devid == 0x0106 || /* GT1 mobile */
           devid == 0x0116 || /* GT2 mobile */
           devid == 0x0126 || /* GT2_PLUS mobile */
           devid == 0x010a);  /* GT1 server */
}

static inline int
gen_get_snb_gt(int devid)
{
   assert(gen_is_snb(devid));
   return (devid & 0x30) ? 2 : 1;
}

static inline bool
gen_is_ivb(int devid)
{
   return (devid == 0x0152 || /* GT1 desktop */
           devid == 0x0162 || /* GT2 desktop */
           devid == 0x0156 || /* GT1 mobile */
           devid == 0x0166 || /* GT2 mobile */
           devid == 0x015a || /* GT1 server */
           devid == 0x016a);  /* GT2 server */
}

static inline int
gen_get_ivb_gt(int devid)
{
   assert(gen_is_ivb(devid));
   return (devid & 0x30) >> 4;
}

static inline bool
gen_is_hsw(int devid)
{
   return (devid == 0x0402 || /* GT1 desktop */
           devid == 0x0412 || /* GT2 desktop */
           devid == 0x0422 || /* GT3 desktop */
           devid == 0x0406 || /* GT1 mobile */
           devid == 0x0416 || /* GT2 mobile */
           devid == 0x0426 || /* GT2 mobile */
           devid == 0x040a || /* GT1 server */
           devid == 0x041a || /* GT2 server */
           devid == 0x042a || /* GT3 server */
           devid == 0x040b || /* GT1 reserved */
           devid == 0x041b || /* GT2 reserved */
           devid == 0x042b || /* GT3 reserved */
           devid == 0x040e || /* GT1 reserved */
           devid == 0x041e || /* GT2 reserved */
           devid == 0x042e || /* GT3 reserved */
           devid == 0x0c02 || /* SDV */
           devid == 0x0c12 ||
           devid == 0x0c22 ||
           devid == 0x0c06 ||
           devid == 0x0c16 ||
           devid == 0x0c26 ||
           devid == 0x0c0a ||
           devid == 0x0c1a ||
           devid == 0x0c2a ||
           devid == 0x0c0b ||
           devid == 0x0c1b ||
           devid == 0x0c2b ||
           devid == 0x0c0e ||
           devid == 0x0c1e ||
           devid == 0x0c2e ||
           devid == 0x0a02 || /* ULT */
           devid == 0x0a12 ||
           devid == 0x0a22 ||
           devid == 0x0a06 ||
           devid == 0x0a16 ||
           devid == 0x0a26 ||
           devid == 0x0a0a ||
           devid == 0x0a1a ||
           devid == 0x0a2a ||
           devid == 0x0a0b ||
           devid == 0x0a1b ||
           devid == 0x0a2b ||
           devid == 0x0a0e ||
           devid == 0x0a1e ||
           devid == 0x0a2e ||
           devid == 0x0d02 || /* CRW */
           devid == 0x0d12 ||
           devid == 0x0d22 ||
           devid == 0x0d06 ||
           devid == 0x0d16 ||
           devid == 0x0d26 ||
           devid == 0x0d0a ||
           devid == 0x0d1a ||
           devid == 0x0d2a ||
           devid == 0x0d0b ||
           devid == 0x0d1b ||
           devid == 0x0d2b ||
           devid == 0x0d0e ||
           devid == 0x0d1e ||
           devid == 0x0d2e);
}

static inline int
gen_get_hsw_gt(int devid)
{
   assert(gen_is_hsw(devid));
   return ((devid & 0x30) >> 4) + 1;
}

static inline bool
gen_is_bdw(int devid)
{
   return (devid == 0x1602 || /* GT1 ULT */
           devid == 0x1606 || /* GT1 ULT */
           devid == 0x160a || /* GT1 server */
           devid == 0x160b || /* GT1 Iris */
           devid == 0x160d || /* GT1 workstation */
           devid == 0x160e || /* GT1 ULX */
           devid == 0x1612 || /* GT2 */
           devid == 0x1616 ||
           devid == 0x161a ||
           devid == 0x161b ||
           devid == 0x161d ||
           devid == 0x161e ||
           devid == 0x1622 || /* GT3 */
           devid == 0x1626 ||
           devid == 0x162a ||
           devid == 0x162b ||
           devid == 0x162d ||
           devid == 0x162e);
}

static inline int
gen_get_bdw_gt(int devid)
{
   assert(gen_is_bdw(devid));
   return ((devid & 0x30) >> 4) + 1;
}

static inline bool
gen_is_vlv(int devid)
{
   return (devid == 0x0f30 ||
           devid == 0x0f31 ||
           devid == 0x0f32 ||
           devid == 0x0f33 ||
           devid == 0x0157 ||
           devid == 0x0155);
}

static inline bool
gen_is_chv(int devid)
{
   return (devid == 0x22b0 ||
           devid == 0x22b1 ||
           devid == 0x22b2 ||
           devid == 0x22b3);
}

static inline bool
gen_is_atom(int devid)
{
   return (gen_is_vlv(devid) ||
           gen_is_chv(devid));
}

static inline bool
gen_is_desktop(int devid)
{
   assert(!gen_is_atom(devid));
   return ((devid & 0xf) == 0x2);
}

static inline bool
gen_is_mobile(int devid)
{
   assert(!gen_is_atom(devid));
   return ((devid & 0xf) == 0x6);
}

static inline bool
gen_is_server(int devid)
{
   assert(!gen_is_atom(devid));
   return ((devid & 0xf) == 0xa);
}

#endif /* GENHW_H */
