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

#ifndef ILO_RENDER_GEN_H
#define ILO_RENDER_GEN_H

#include "core/ilo_builder.h"
#include "core/ilo_builder_3d.h"
#include "core/ilo_builder_render.h"

#include "ilo_common.h"
#include "ilo_state.h"
#include "ilo_render.h"

struct ilo_bo;
struct ilo_blitter;
struct ilo_render;
struct ilo_state_vector;

/**
 * Render Engine.
 */
struct ilo_render {
   const struct ilo_dev *dev;
   struct ilo_builder *builder;

   struct intel_bo *workaround_bo;

   uint32_t sample_pattern_1x;
   uint32_t sample_pattern_2x;
   uint32_t sample_pattern_4x;
   uint32_t sample_pattern_8x[2];
   uint32_t sample_pattern_16x[4];

   bool hw_ctx_changed;

   /*
    * Any state that involves resources needs to be re-emitted when the
    * batch bo changed.  This is because we do not pin the resources and
    * their offsets (or existence) may change between batch buffers.
    */
   bool batch_bo_changed;
   bool state_bo_changed;
   bool instruction_bo_changed;

   /**
    * HW states.
    */
   struct ilo_render_state {
      /*
       * When a WA is needed before some command, we always emit the WA right
       * before the command.  Knowing what have already been done since last
       * 3DPRIMITIVE allows us to skip some WAs.
       */
      uint32_t current_pipe_control_dw1;

      /*
       * When a WA is needed after some command, we may have the WA follow the
       * command immediately or defer it.  If this is non-zero, a PIPE_CONTROL
       * will be emitted before 3DPRIMITIVE.
       */
      uint32_t deferred_pipe_control_dw1;

      bool primitive_restart;
      int reduced_prim;
      int so_max_vertices;

      uint32_t SF_VIEWPORT;
      uint32_t CLIP_VIEWPORT;
      uint32_t SF_CLIP_VIEWPORT; /* GEN7+ */
      uint32_t CC_VIEWPORT;

      uint32_t COLOR_CALC_STATE;
      uint32_t BLEND_STATE;
      uint32_t DEPTH_STENCIL_STATE;

      uint32_t SCISSOR_RECT;

      struct {
         uint32_t BINDING_TABLE_STATE;
         uint32_t SURFACE_STATE[ILO_MAX_SURFACES];
         uint32_t SAMPLER_STATE;
         uint32_t SAMPLER_BORDER_COLOR_STATE[ILO_MAX_SAMPLERS];
         uint32_t PUSH_CONSTANT_BUFFER;
         int PUSH_CONSTANT_BUFFER_size;
      } vs;

      struct {
         uint32_t BINDING_TABLE_STATE;
         uint32_t SURFACE_STATE[ILO_MAX_SURFACES];
         bool active;
      } gs;

      struct {
         uint32_t BINDING_TABLE_STATE;
         uint32_t SURFACE_STATE[ILO_MAX_SURFACES];
         uint32_t SAMPLER_STATE;
         uint32_t SAMPLER_BORDER_COLOR_STATE[ILO_MAX_SAMPLERS];
         uint32_t PUSH_CONSTANT_BUFFER;
         int PUSH_CONSTANT_BUFFER_size;
      } wm;

      struct {
         uint32_t BINDING_TABLE_STATE;
         uint32_t SURFACE_STATE[ILO_MAX_SURFACES];
         uint32_t SAMPLER_STATE;
         uint32_t SAMPLER_BORDER_COLOR_STATE[ILO_MAX_SAMPLERS];
         uint32_t PUSH_CONSTANT_BUFFER;
         int PUSH_CONSTANT_BUFFER_size;
      } cs;
   } state;
};

struct ilo_render_draw_session {
   uint32_t pipe_dirty;

   /* commands */
   int reduced_prim;

   bool prim_changed;
   bool primitive_restart_changed;

   /* dynamic states */
   bool viewport_changed;
   bool scissor_changed;

   bool cc_changed;
   bool dsa_changed;
   bool blend_changed;

   bool sampler_vs_changed;
   bool sampler_gs_changed;
   bool sampler_fs_changed;

   bool pcb_vs_changed;
   bool pcb_gs_changed;
   bool pcb_fs_changed;

   /* surface states */
   bool binding_table_vs_changed;
   bool binding_table_gs_changed;
   bool binding_table_fs_changed;
};

struct ilo_render_rectlist_session {
   uint32_t vb_start;
   uint32_t vb_end;
};

struct ilo_render_launch_grid_session {
   const unsigned *thread_group_offset;
   const unsigned *thread_group_dim;
   unsigned thread_group_size;
   const struct pipe_constant_buffer *input;
   uint32_t pc;

   uint32_t idrt;
   int idrt_size;
};

int
ilo_render_get_draw_commands_len_gen6(const struct ilo_render *render,
                                      const struct ilo_state_vector *vec);

int
ilo_render_get_draw_commands_len_gen7(const struct ilo_render *render,
                                      const struct ilo_state_vector *vec);

int
ilo_render_get_draw_commands_len_gen8(const struct ilo_render *render,
                                      const struct ilo_state_vector *vec);

static inline int
ilo_render_get_draw_commands_len(const struct ilo_render *render,
                                 const struct ilo_state_vector *vec)
{
   if (ilo_dev_gen(render->dev) >= ILO_GEN(8))
      return ilo_render_get_draw_commands_len_gen8(render, vec);
   else if (ilo_dev_gen(render->dev) >= ILO_GEN(7))
      return ilo_render_get_draw_commands_len_gen7(render, vec);
   else
      return ilo_render_get_draw_commands_len_gen6(render, vec);
}

void
ilo_render_emit_draw_commands_gen6(struct ilo_render *render,
                                   const struct ilo_state_vector *vec,
                                   struct ilo_render_draw_session *session);

void
ilo_render_emit_draw_commands_gen7(struct ilo_render *render,
                                   const struct ilo_state_vector *vec,
                                   struct ilo_render_draw_session *session);

void
ilo_render_emit_draw_commands_gen8(struct ilo_render *render,
                                   const struct ilo_state_vector *vec,
                                   struct ilo_render_draw_session *session);

static inline void
ilo_render_emit_draw_commands(struct ilo_render *render,
                              const struct ilo_state_vector *vec,
                              struct ilo_render_draw_session *session)
{
   const unsigned batch_used = ilo_builder_batch_used(render->builder);

   if (ilo_dev_gen(render->dev) >= ILO_GEN(8))
      ilo_render_emit_draw_commands_gen8(render, vec, session);
   else if (ilo_dev_gen(render->dev) >= ILO_GEN(7))
      ilo_render_emit_draw_commands_gen7(render, vec, session);
   else
      ilo_render_emit_draw_commands_gen6(render, vec, session);

   assert(ilo_builder_batch_used(render->builder) <= batch_used +
         ilo_render_get_draw_commands_len(render, vec));
}

int
ilo_render_get_rectlist_commands_len_gen6(const struct ilo_render *render,
                                          const struct ilo_blitter *blitter);

int
ilo_render_get_rectlist_commands_len_gen8(const struct ilo_render *render,
                                          const struct ilo_blitter *blitter);

static inline int
ilo_render_get_rectlist_commands_len(const struct ilo_render *render,
                                     const struct ilo_blitter *blitter)
{
   if (ilo_dev_gen(render->dev) >= ILO_GEN(8))
      return ilo_render_get_rectlist_commands_len_gen8(render, blitter);
   else
      return ilo_render_get_rectlist_commands_len_gen6(render, blitter);
}

void
ilo_render_emit_rectlist_commands_gen6(struct ilo_render *r,
                                       const struct ilo_blitter *blitter,
                                       const struct ilo_render_rectlist_session *session);

void
ilo_render_emit_rectlist_commands_gen7(struct ilo_render *r,
                                       const struct ilo_blitter *blitter,
                                       const struct ilo_render_rectlist_session *session);

void
ilo_render_emit_rectlist_commands_gen8(struct ilo_render *r,
                                       const struct ilo_blitter *blitter,
                                       const struct ilo_render_rectlist_session *session);

static inline void
ilo_render_emit_rectlist_commands(struct ilo_render *render,
                                  const struct ilo_blitter *blitter,
                                  const struct ilo_render_rectlist_session *session)
{
   const unsigned batch_used = ilo_builder_batch_used(render->builder);

   if (ilo_dev_gen(render->dev) >= ILO_GEN(8))
      ilo_render_emit_rectlist_commands_gen8(render, blitter, session);
   else if (ilo_dev_gen(render->dev) >= ILO_GEN(7))
      ilo_render_emit_rectlist_commands_gen7(render, blitter, session);
   else
      ilo_render_emit_rectlist_commands_gen6(render, blitter, session);

   assert(ilo_builder_batch_used(render->builder) <= batch_used +
         ilo_render_get_rectlist_commands_len(render, blitter));
}

int
ilo_render_get_launch_grid_commands_len(const struct ilo_render *render,
                                        const struct ilo_state_vector *vec);

void
ilo_render_emit_launch_grid_commands(struct ilo_render *render,
                                     const struct ilo_state_vector *vec,
                                     const struct ilo_render_launch_grid_session *session);

int
ilo_render_get_draw_dynamic_states_len(const struct ilo_render *render,
                                       const struct ilo_state_vector *vec);

void
ilo_render_emit_draw_dynamic_states(struct ilo_render *render,
                                    const struct ilo_state_vector *vec,
                                    struct ilo_render_draw_session *session);

int
ilo_render_get_rectlist_dynamic_states_len(const struct ilo_render *render,
                                           const struct ilo_blitter *blitter);

void
ilo_render_emit_rectlist_dynamic_states(struct ilo_render *render,
                                        const struct ilo_blitter *blitter,
                                        struct ilo_render_rectlist_session *session);

int
ilo_render_get_launch_grid_dynamic_states_len(const struct ilo_render *render,
                                              const struct ilo_state_vector *vec);

void
ilo_render_emit_launch_grid_dynamic_states(struct ilo_render *render,
                                           const struct ilo_state_vector *vec,
                                           struct ilo_render_launch_grid_session *session);

int
ilo_render_get_draw_surface_states_len(const struct ilo_render *render,
                                       const struct ilo_state_vector *vec);

void
ilo_render_emit_draw_surface_states(struct ilo_render *render,
                                    const struct ilo_state_vector *vec,
                                    struct ilo_render_draw_session *session);

int
ilo_render_get_launch_grid_surface_states_len(const struct ilo_render *render,
                                              const struct ilo_state_vector *vec);

void
ilo_render_emit_launch_grid_surface_states(struct ilo_render *render,
                                           const struct ilo_state_vector *vec,
                                           struct ilo_render_launch_grid_session *session);

/**
 * A convenient wrapper for gen6_PIPE_CONTROL().  This should be enough for
 * our needs everywhere except for queries.
 */
static inline void
ilo_render_pipe_control(struct ilo_render *r, uint32_t dw1)
{
   const uint32_t write_mask = (dw1 & GEN6_PIPE_CONTROL_WRITE__MASK);
   struct intel_bo *bo = (write_mask) ? r->workaround_bo : NULL;

   ILO_DEV_ASSERT(r->dev, 6, 8);

   if (write_mask)
      assert(write_mask == GEN6_PIPE_CONTROL_WRITE_IMM);

   if (dw1 & GEN6_PIPE_CONTROL_CS_STALL) {
      /* CS stall cannot be set alone */
      const uint32_t mask = GEN6_PIPE_CONTROL_RENDER_CACHE_FLUSH |
                            GEN6_PIPE_CONTROL_DEPTH_CACHE_FLUSH |
                            GEN6_PIPE_CONTROL_PIXEL_SCOREBOARD_STALL |
                            GEN6_PIPE_CONTROL_DEPTH_STALL |
                            GEN6_PIPE_CONTROL_WRITE__MASK;
      if (!(dw1 & mask))
         dw1 |= GEN6_PIPE_CONTROL_PIXEL_SCOREBOARD_STALL;
   }

   gen6_PIPE_CONTROL(r->builder, dw1, bo, 0, 0);

   r->state.current_pipe_control_dw1 |= dw1;
   r->state.deferred_pipe_control_dw1 &= ~dw1;
}

/**
 * A convenient wrapper for gen{6,7}_3DPRIMITIVE().
 */
static inline void
ilo_render_3dprimitive(struct ilo_render *r,
                       const struct pipe_draw_info *info,
                       const struct ilo_ib_state *ib)
{
   ILO_DEV_ASSERT(r->dev, 6, 8);

   if (r->state.deferred_pipe_control_dw1)
      ilo_render_pipe_control(r, r->state.deferred_pipe_control_dw1);

   /* 3DPRIMITIVE */
   if (ilo_dev_gen(r->dev) >= ILO_GEN(7))
      gen7_3DPRIMITIVE(r->builder, info, ib);
   else
      gen6_3DPRIMITIVE(r->builder, info, ib);

   r->state.current_pipe_control_dw1 = 0;
   assert(!r->state.deferred_pipe_control_dw1);
}

void
gen6_wa_pre_pipe_control(struct ilo_render *r, uint32_t dw1);

void
gen6_draw_common_select(struct ilo_render *r,
                        const struct ilo_state_vector *ilo,
                        struct ilo_render_draw_session *session);

void
gen6_draw_common_sip(struct ilo_render *r,
                     const struct ilo_state_vector *ilo,
                     struct ilo_render_draw_session *session);

void
gen6_draw_common_base_address(struct ilo_render *r,
                              const struct ilo_state_vector *ilo,
                              struct ilo_render_draw_session *session);

void
gen6_draw_vf(struct ilo_render *r,
             const struct ilo_state_vector *ilo,
             struct ilo_render_draw_session *session);

void
gen6_draw_vf_statistics(struct ilo_render *r,
                        const struct ilo_state_vector *ilo,
                        struct ilo_render_draw_session *session);

void
gen6_draw_vs(struct ilo_render *r,
             const struct ilo_state_vector *ilo,
             struct ilo_render_draw_session *session);

void
gen6_draw_clip(struct ilo_render *r,
               const struct ilo_state_vector *ilo,
               struct ilo_render_draw_session *session);

void
gen6_draw_sf_rect(struct ilo_render *r,
                  const struct ilo_state_vector *ilo,
                  struct ilo_render_draw_session *session);

void
gen6_draw_wm_raster(struct ilo_render *r,
                    const struct ilo_state_vector *ilo,
                    struct ilo_render_draw_session *session);

void
gen7_draw_common_pcb_alloc(struct ilo_render *r,
                           const struct ilo_state_vector *vec,
                           struct ilo_render_draw_session *session);

void
gen7_draw_common_pointers_1(struct ilo_render *r,
                            const struct ilo_state_vector *vec,
                            struct ilo_render_draw_session *session);

void
gen7_draw_common_urb(struct ilo_render *r,
                     const struct ilo_state_vector *vec,
                     struct ilo_render_draw_session *session);

void
gen7_draw_common_pointers_2(struct ilo_render *r,
                            const struct ilo_state_vector *vec,
                            struct ilo_render_draw_session *session);

void
gen7_draw_vs(struct ilo_render *r,
             const struct ilo_state_vector *vec,
             struct ilo_render_draw_session *session);

void
gen7_draw_ds(struct ilo_render *r,
             const struct ilo_state_vector *vec,
             struct ilo_render_draw_session *session);

void
gen7_draw_te(struct ilo_render *r,
             const struct ilo_state_vector *vec,
             struct ilo_render_draw_session *session);

void
gen7_draw_hs(struct ilo_render *r,
             const struct ilo_state_vector *vec,
             struct ilo_render_draw_session *session);

void
gen7_draw_gs(struct ilo_render *r,
             const struct ilo_state_vector *vec,
             struct ilo_render_draw_session *session);

void
gen7_draw_sol(struct ilo_render *r,
              const struct ilo_state_vector *vec,
              struct ilo_render_draw_session *session);

#endif /* ILO_RENDER_GEN_H */
