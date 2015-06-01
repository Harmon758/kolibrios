/*
 * Copyright © 2014 Broadcom
 * Copyright (C) 2012 Rob Clark <robclark@freedesktop.org>
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

#include "pipe/p_state.h"
#include "util/u_inlines.h"
#include "util/u_math.h"
#include "util/u_memory.h"
#include "util/u_helpers.h"

#include "vc4_context.h"

static void *
vc4_generic_cso_state_create(const void *src, uint32_t size)
{
        void *dst = calloc(1, size);
        if (!dst)
                return NULL;
        memcpy(dst, src, size);
        return dst;
}

static void
vc4_generic_cso_state_delete(struct pipe_context *pctx, void *hwcso)
{
        free(hwcso);
}

static void
vc4_set_blend_color(struct pipe_context *pctx,
                    const struct pipe_blend_color *blend_color)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->blend_color = *blend_color;
        vc4->dirty |= VC4_DIRTY_BLEND_COLOR;
}

static void
vc4_set_stencil_ref(struct pipe_context *pctx,
                    const struct pipe_stencil_ref *stencil_ref)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->stencil_ref =* stencil_ref;
        vc4->dirty |= VC4_DIRTY_STENCIL_REF;
}

static void
vc4_set_clip_state(struct pipe_context *pctx,
                   const struct pipe_clip_state *clip)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->clip = *clip;
        vc4->dirty |= VC4_DIRTY_CLIP;
}

static void
vc4_set_sample_mask(struct pipe_context *pctx, unsigned sample_mask)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->sample_mask = (uint16_t)sample_mask;
        vc4->dirty |= VC4_DIRTY_SAMPLE_MASK;
}

static uint16_t
float_to_187_half(float f)
{
        return fui(f) >> 16;
}

static void *
vc4_create_rasterizer_state(struct pipe_context *pctx,
                            const struct pipe_rasterizer_state *cso)
{
        struct vc4_rasterizer_state *so;

        so = CALLOC_STRUCT(vc4_rasterizer_state);
        if (!so)
                return NULL;

        so->base = *cso;

        if (!(cso->cull_face & PIPE_FACE_FRONT))
                so->config_bits[0] |= VC4_CONFIG_BITS_ENABLE_PRIM_FRONT;
        if (!(cso->cull_face & PIPE_FACE_BACK))
                so->config_bits[0] |= VC4_CONFIG_BITS_ENABLE_PRIM_BACK;

        /* Workaround: HW-2726 PTB does not handle zero-size points (BCM2835,
         * BCM21553).
         */
        so->point_size = MAX2(cso->point_size, .125);

        if (cso->front_ccw)
                so->config_bits[0] |= VC4_CONFIG_BITS_CW_PRIMITIVES;

        if (cso->offset_tri) {
                so->config_bits[0] |= VC4_CONFIG_BITS_ENABLE_DEPTH_OFFSET;

                so->offset_units = float_to_187_half(cso->offset_units);
                so->offset_factor = float_to_187_half(cso->offset_scale);
        }

        return so;
}

/* Blend state is baked into shaders. */
static void *
vc4_create_blend_state(struct pipe_context *pctx,
                       const struct pipe_blend_state *cso)
{
        return vc4_generic_cso_state_create(cso, sizeof(*cso));
}

/**
 * The TLB_STENCIL_SETUP data has a little bitfield for common writemask
 * values, so you don't have to do a separate writemask setup.
 */
static uint8_t
tlb_stencil_setup_writemask(uint8_t mask)
{
        switch (mask) {
        case 0x1: return 0;
        case 0x3: return 1;
        case 0xf: return 2;
        case 0xff: return 3;
        default: return 0xff;
        }
}

static uint32_t
tlb_stencil_setup_bits(const struct pipe_stencil_state *state,
                       uint8_t writemask_bits)
{
        static const uint8_t op_map[] = {
                [PIPE_STENCIL_OP_ZERO] = 0,
                [PIPE_STENCIL_OP_KEEP] = 1,
                [PIPE_STENCIL_OP_REPLACE] = 2,
                [PIPE_STENCIL_OP_INCR] = 3,
                [PIPE_STENCIL_OP_DECR] = 4,
                [PIPE_STENCIL_OP_INVERT] = 5,
                [PIPE_STENCIL_OP_INCR_WRAP] = 6,
                [PIPE_STENCIL_OP_DECR_WRAP] = 7,
        };
        uint32_t bits = 0;

        if (writemask_bits != 0xff)
                bits |= writemask_bits << 28;
        bits |= op_map[state->zfail_op] << 25;
        bits |= op_map[state->zpass_op] << 22;
        bits |= op_map[state->fail_op] << 19;
        bits |= state->func << 16;
        /* Ref is filled in at uniform upload time */
        bits |= state->valuemask << 0;

        return bits;
}

static void *
vc4_create_depth_stencil_alpha_state(struct pipe_context *pctx,
                                     const struct pipe_depth_stencil_alpha_state *cso)
{
        struct vc4_depth_stencil_alpha_state *so;

        so = CALLOC_STRUCT(vc4_depth_stencil_alpha_state);
        if (!so)
                return NULL;

        so->base = *cso;

        /* We always keep the early Z state correct, since a later state using
         * early Z may want it.
         */
        so->config_bits[2] |= VC4_CONFIG_BITS_EARLY_Z_UPDATE;

        if (cso->depth.enabled) {
                if (cso->depth.writemask) {
                        so->config_bits[1] |= VC4_CONFIG_BITS_Z_UPDATE;
                }
                so->config_bits[1] |= (cso->depth.func <<
                                       VC4_CONFIG_BITS_DEPTH_FUNC_SHIFT);

                /* We only handle early Z in the < direction because otherwise
                 * we'd have to runtime guess which direction to set in the
                 * render config.
                 */
                if ((cso->depth.func == PIPE_FUNC_LESS ||
                     cso->depth.func == PIPE_FUNC_LEQUAL) &&
                    (!cso->stencil[0].enabled ||
                     (cso->stencil[0].zfail_op == PIPE_STENCIL_OP_KEEP &&
                      (!cso->stencil[1].enabled ||
                       cso->stencil[1].zfail_op == PIPE_STENCIL_OP_KEEP)))) {
                        so->config_bits[2] |= VC4_CONFIG_BITS_EARLY_Z;
                }
        } else {
                so->config_bits[1] |= (PIPE_FUNC_ALWAYS <<
                                       VC4_CONFIG_BITS_DEPTH_FUNC_SHIFT);
        }

        if (cso->stencil[0].enabled) {
                const struct pipe_stencil_state *front = &cso->stencil[0];
                const struct pipe_stencil_state *back = &cso->stencil[1];

                uint8_t front_writemask_bits =
                        tlb_stencil_setup_writemask(front->writemask);
                uint8_t back_writemask = front->writemask;
                uint8_t back_writemask_bits = front_writemask_bits;

                so->stencil_uniforms[0] =
                        tlb_stencil_setup_bits(front, front_writemask_bits);
                if (back->enabled) {
                        back_writemask = back->writemask;
                        back_writemask_bits =
                                tlb_stencil_setup_writemask(back->writemask);

                        so->stencil_uniforms[0] |= (1 << 30);
                        so->stencil_uniforms[1] =
                                tlb_stencil_setup_bits(back, back_writemask_bits);
                        so->stencil_uniforms[1] |= (2 << 30);
                } else {
                        so->stencil_uniforms[0] |= (3 << 30);
                }

                if (front_writemask_bits == 0xff ||
                    back_writemask_bits == 0xff) {
                        so->stencil_uniforms[2] = (front->writemask |
                                                   (back_writemask << 8));
                }
        }

        return so;
}

static void
vc4_set_polygon_stipple(struct pipe_context *pctx,
                        const struct pipe_poly_stipple *stipple)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->stipple = *stipple;
        vc4->dirty |= VC4_DIRTY_STIPPLE;
}

static void
vc4_set_scissor_states(struct pipe_context *pctx,
                       unsigned start_slot,
                       unsigned num_scissors,
                       const struct pipe_scissor_state *scissor)
{
        struct vc4_context *vc4 = vc4_context(pctx);

        vc4->scissor = *scissor;
        vc4->dirty |= VC4_DIRTY_SCISSOR;
}

static void
vc4_set_viewport_states(struct pipe_context *pctx,
                        unsigned start_slot,
                        unsigned num_viewports,
                        const struct pipe_viewport_state *viewport)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->viewport = *viewport;
        vc4->dirty |= VC4_DIRTY_VIEWPORT;
}

static void
vc4_set_vertex_buffers(struct pipe_context *pctx,
                       unsigned start_slot, unsigned count,
                       const struct pipe_vertex_buffer *vb)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        struct vc4_vertexbuf_stateobj *so = &vc4->vertexbuf;

        util_set_vertex_buffers_mask(so->vb, &so->enabled_mask, vb,
                                     start_slot, count);
        so->count = util_last_bit(so->enabled_mask);

        vc4->dirty |= VC4_DIRTY_VTXBUF;
}

static void
vc4_set_index_buffer(struct pipe_context *pctx,
                     const struct pipe_index_buffer *ib)
{
        struct vc4_context *vc4 = vc4_context(pctx);

        if (ib) {
                assert(!ib->user_buffer);

                if (ib->index_size == 4) {
                        struct pipe_resource tmpl = *ib->buffer;
                        assert(tmpl.format == PIPE_FORMAT_R8_UNORM);
                        assert(tmpl.height0 == 1);
                        tmpl.width0 = (tmpl.width0 - ib->offset) / 2;
                        struct pipe_resource *pshadow =
                                vc4_resource_create(&vc4->screen->base, &tmpl);
                        struct vc4_resource *shadow = vc4_resource(pshadow);
                        pipe_resource_reference(&shadow->shadow_parent, ib->buffer);

                        pipe_resource_reference(&vc4->indexbuf.buffer, NULL);
                        vc4->indexbuf.buffer = pshadow;
                        vc4->indexbuf.index_size = 2;
                } else {
                        pipe_resource_reference(&vc4->indexbuf.buffer, ib->buffer);
                        vc4->indexbuf.index_size = ib->index_size;
                }
                vc4->indexbuf.offset = ib->offset;
        } else {
                pipe_resource_reference(&vc4->indexbuf.buffer, NULL);
        }

        vc4->dirty |= VC4_DIRTY_INDEXBUF;
}

static void
vc4_blend_state_bind(struct pipe_context *pctx, void *hwcso)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->blend = hwcso;
        vc4->dirty |= VC4_DIRTY_BLEND;
}

static void
vc4_rasterizer_state_bind(struct pipe_context *pctx, void *hwcso)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        struct vc4_rasterizer_state *rast = hwcso;

        if (vc4->rasterizer && rast &&
            vc4->rasterizer->base.flatshade != rast->base.flatshade) {
                vc4->dirty |= VC4_DIRTY_FLAT_SHADE_FLAGS;
        }

        vc4->rasterizer = hwcso;
        vc4->dirty |= VC4_DIRTY_RASTERIZER;
}

static void
vc4_zsa_state_bind(struct pipe_context *pctx, void *hwcso)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->zsa = hwcso;
        vc4->dirty |= VC4_DIRTY_ZSA;
}

static void *
vc4_vertex_state_create(struct pipe_context *pctx, unsigned num_elements,
                        const struct pipe_vertex_element *elements)
{
        struct vc4_vertex_stateobj *so = CALLOC_STRUCT(vc4_vertex_stateobj);

        if (!so)
                return NULL;

        memcpy(so->pipe, elements, sizeof(*elements) * num_elements);
        so->num_elements = num_elements;

        return so;
}

static void
vc4_vertex_state_bind(struct pipe_context *pctx, void *hwcso)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        vc4->vtx = hwcso;
        vc4->dirty |= VC4_DIRTY_VTXSTATE;
}

static void
vc4_set_constant_buffer(struct pipe_context *pctx, uint shader, uint index,
                        struct pipe_constant_buffer *cb)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        struct vc4_constbuf_stateobj *so = &vc4->constbuf[shader];

        assert(index == 0);

        /* Note that the state tracker can unbind constant buffers by
         * passing NULL here.
         */
        if (unlikely(!cb)) {
                so->enabled_mask &= ~(1 << index);
                so->dirty_mask &= ~(1 << index);
                return;
        }

        assert(!cb->buffer);
        so->cb[index].buffer_offset = cb->buffer_offset;
        so->cb[index].buffer_size   = cb->buffer_size;
        so->cb[index].user_buffer   = cb->user_buffer;

        so->enabled_mask |= 1 << index;
        so->dirty_mask |= 1 << index;
        vc4->dirty |= VC4_DIRTY_CONSTBUF;
}

static void
vc4_set_framebuffer_state(struct pipe_context *pctx,
                          const struct pipe_framebuffer_state *framebuffer)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        struct pipe_framebuffer_state *cso = &vc4->framebuffer;
        unsigned i;

        vc4_flush(pctx);

        for (i = 0; i < framebuffer->nr_cbufs; i++)
                pipe_surface_reference(&cso->cbufs[i], framebuffer->cbufs[i]);
        for (; i < vc4->framebuffer.nr_cbufs; i++)
                pipe_surface_reference(&cso->cbufs[i], NULL);

        cso->nr_cbufs = framebuffer->nr_cbufs;

        pipe_surface_reference(&cso->zsbuf, framebuffer->zsbuf);

        cso->width = framebuffer->width;
        cso->height = framebuffer->height;

        /* Nonzero texture mipmap levels are laid out as if they were in
         * power-of-two-sized spaces.  The renderbuffer config infers its
         * stride from the width parameter, so we need to configure our
         * framebuffer.  Note that if the z/color buffers were mismatched
         * sizes, we wouldn't be able to do this.
         */
        if (cso->cbufs[0] && cso->cbufs[0]->u.tex.level) {
                struct vc4_resource *rsc =
                        vc4_resource(cso->cbufs[0]->texture);
                cso->width =
                        (rsc->slices[cso->cbufs[0]->u.tex.level].stride /
                         rsc->cpp);
        } else if (cso->zsbuf && cso->zsbuf->u.tex.level){
                struct vc4_resource *rsc =
                        vc4_resource(cso->zsbuf->texture);
                cso->width =
                        (rsc->slices[cso->zsbuf->u.tex.level].stride /
                         rsc->cpp);
        }

        vc4->dirty |= VC4_DIRTY_FRAMEBUFFER;
}

static struct vc4_texture_stateobj *
vc4_get_stage_tex(struct vc4_context *vc4, unsigned shader)
{
        vc4->dirty |= VC4_DIRTY_TEXSTATE;

        switch (shader) {
        case PIPE_SHADER_FRAGMENT:
                vc4->dirty |= VC4_DIRTY_FRAGTEX;
                return &vc4->fragtex;
                break;
        case PIPE_SHADER_VERTEX:
                vc4->dirty |= VC4_DIRTY_VERTTEX;
                return &vc4->verttex;
                break;
        default:
                fprintf(stderr, "Unknown shader target %d\n", shader);
                abort();
        }
}

static void *
vc4_create_sampler_state(struct pipe_context *pctx,
                         const struct pipe_sampler_state *cso)
{
        return vc4_generic_cso_state_create(cso, sizeof(*cso));
}

static void
vc4_sampler_states_bind(struct pipe_context *pctx,
                        unsigned shader, unsigned start,
                        unsigned nr, void **hwcso)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        struct vc4_texture_stateobj *stage_tex = vc4_get_stage_tex(vc4, shader);

        assert(start == 0);
        unsigned i;
        unsigned new_nr = 0;

        for (i = 0; i < nr; i++) {
                if (hwcso[i])
                        new_nr = i + 1;
                stage_tex->samplers[i] = hwcso[i];
                stage_tex->dirty_samplers |= (1 << i);
        }

        for (; i < stage_tex->num_samplers; i++) {
                stage_tex->samplers[i] = NULL;
                stage_tex->dirty_samplers |= (1 << i);
        }

        stage_tex->num_samplers = new_nr;
}

static struct pipe_sampler_view *
vc4_create_sampler_view(struct pipe_context *pctx, struct pipe_resource *prsc,
                        const struct pipe_sampler_view *cso)
{
        struct pipe_sampler_view *so = malloc(sizeof(*so));
        struct vc4_resource *rsc = vc4_resource(prsc);

        if (!so)
                return NULL;

        *so = *cso;

        pipe_reference(NULL, &prsc->reference);

        /* There is no hardware level clamping, and the start address of a
         * texture may be misaligned, so in that case we have to copy to a
         * temporary.
         *
         * Also, Raspberry Pi doesn't support sampling from raster textures,
         * so we also have to copy to a temporary then.
         */
        if (so->u.tex.first_level ||
            rsc->vc4_format == VC4_TEXTURE_TYPE_RGBA32R) {
                struct vc4_resource *shadow_parent = vc4_resource(prsc);
                struct pipe_resource tmpl = shadow_parent->base.b;
                struct vc4_resource *clone;

                tmpl.width0 = u_minify(tmpl.width0, so->u.tex.first_level);
                tmpl.height0 = u_minify(tmpl.height0, so->u.tex.first_level);
                tmpl.last_level = so->u.tex.last_level - so->u.tex.first_level;

                prsc = vc4_resource_create(pctx->screen, &tmpl);
                clone = vc4_resource(prsc);
                clone->shadow_parent = &shadow_parent->base.b;
                /* Flag it as needing update of the contents from the parent. */
                clone->writes = shadow_parent->writes - 1;
        }
        so->texture = prsc;
        so->reference.count = 1;
        so->context = pctx;

        return so;
}

static void
vc4_sampler_view_destroy(struct pipe_context *pctx,
                         struct pipe_sampler_view *view)
{
        pipe_resource_reference(&view->texture, NULL);
        free(view);
}

static void
vc4_set_sampler_views(struct pipe_context *pctx, unsigned shader,
                      unsigned start, unsigned nr,
                      struct pipe_sampler_view **views)
{
        struct vc4_context *vc4 = vc4_context(pctx);
        struct vc4_texture_stateobj *stage_tex = vc4_get_stage_tex(vc4, shader);
        unsigned i;
        unsigned new_nr = 0;

        assert(start == 0);

        vc4->dirty |= VC4_DIRTY_TEXSTATE;

        for (i = 0; i < nr; i++) {
                if (views[i])
                        new_nr = i + 1;
                pipe_sampler_view_reference(&stage_tex->textures[i], views[i]);
                stage_tex->dirty_samplers |= (1 << i);
        }

        for (; i < stage_tex->num_textures; i++) {
                pipe_sampler_view_reference(&stage_tex->textures[i], NULL);
                stage_tex->dirty_samplers |= (1 << i);
        }

        stage_tex->num_textures = new_nr;
}

void
vc4_state_init(struct pipe_context *pctx)
{
        pctx->set_blend_color = vc4_set_blend_color;
        pctx->set_stencil_ref = vc4_set_stencil_ref;
        pctx->set_clip_state = vc4_set_clip_state;
        pctx->set_sample_mask = vc4_set_sample_mask;
        pctx->set_constant_buffer = vc4_set_constant_buffer;
        pctx->set_framebuffer_state = vc4_set_framebuffer_state;
        pctx->set_polygon_stipple = vc4_set_polygon_stipple;
        pctx->set_scissor_states = vc4_set_scissor_states;
        pctx->set_viewport_states = vc4_set_viewport_states;

        pctx->set_vertex_buffers = vc4_set_vertex_buffers;
        pctx->set_index_buffer = vc4_set_index_buffer;

        pctx->create_blend_state = vc4_create_blend_state;
        pctx->bind_blend_state = vc4_blend_state_bind;
        pctx->delete_blend_state = vc4_generic_cso_state_delete;

        pctx->create_rasterizer_state = vc4_create_rasterizer_state;
        pctx->bind_rasterizer_state = vc4_rasterizer_state_bind;
        pctx->delete_rasterizer_state = vc4_generic_cso_state_delete;

        pctx->create_depth_stencil_alpha_state = vc4_create_depth_stencil_alpha_state;
        pctx->bind_depth_stencil_alpha_state = vc4_zsa_state_bind;
        pctx->delete_depth_stencil_alpha_state = vc4_generic_cso_state_delete;

        pctx->create_vertex_elements_state = vc4_vertex_state_create;
        pctx->delete_vertex_elements_state = vc4_generic_cso_state_delete;
        pctx->bind_vertex_elements_state = vc4_vertex_state_bind;

        pctx->create_sampler_state = vc4_create_sampler_state;
        pctx->delete_sampler_state = vc4_generic_cso_state_delete;
        pctx->bind_sampler_states = vc4_sampler_states_bind;

        pctx->create_sampler_view = vc4_create_sampler_view;
        pctx->sampler_view_destroy = vc4_sampler_view_destroy;
        pctx->set_sampler_views = vc4_set_sampler_views;
}
