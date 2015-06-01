#ifndef __NVC0_CONTEXT_H__
#define __NVC0_CONTEXT_H__

#include "pipe/p_context.h"
#include "pipe/p_defines.h"
#include "pipe/p_state.h"

#include "util/u_memory.h"
#include "util/u_math.h"
#include "util/u_inlines.h"
#include "util/u_dynarray.h"

#include "nvc0/nvc0_winsys.h"
#include "nvc0/nvc0_stateobj.h"
#include "nvc0/nvc0_screen.h"
#include "nvc0/nvc0_program.h"
#include "nvc0/nvc0_resource.h"

#include "nv50/nv50_transfer.h"

#include "nouveau_context.h"
#include "nouveau_debug.h"

#include "nv50/nv50_3ddefs.xml.h"
#include "nvc0/nvc0_3d.xml.h"
#include "nv50/nv50_2d.xml.h"
#include "nvc0/nvc0_m2mf.xml.h"
#include "nvc0/nve4_p2mf.xml.h"
#include "nvc0/nvc0_macros.h"

/* NOTE: must keep NVC0_NEW_...PROG in consecutive bits in this order */
#define NVC0_NEW_BLEND        (1 << 0)
#define NVC0_NEW_RASTERIZER   (1 << 1)
#define NVC0_NEW_ZSA          (1 << 2)
#define NVC0_NEW_VERTPROG     (1 << 3)
#define NVC0_NEW_TCTLPROG     (1 << 4)
#define NVC0_NEW_TEVLPROG     (1 << 5)
#define NVC0_NEW_GMTYPROG     (1 << 6)
#define NVC0_NEW_FRAGPROG     (1 << 7)
#define NVC0_NEW_BLEND_COLOUR (1 << 8)
#define NVC0_NEW_STENCIL_REF  (1 << 9)
#define NVC0_NEW_CLIP         (1 << 10)
#define NVC0_NEW_SAMPLE_MASK  (1 << 11)
#define NVC0_NEW_FRAMEBUFFER  (1 << 12)
#define NVC0_NEW_STIPPLE      (1 << 13)
#define NVC0_NEW_SCISSOR      (1 << 14)
#define NVC0_NEW_VIEWPORT     (1 << 15)
#define NVC0_NEW_ARRAYS       (1 << 16)
#define NVC0_NEW_VERTEX       (1 << 17)
#define NVC0_NEW_CONSTBUF     (1 << 18)
#define NVC0_NEW_TEXTURES     (1 << 19)
#define NVC0_NEW_SAMPLERS     (1 << 20)
#define NVC0_NEW_TFB_TARGETS  (1 << 21)
#define NVC0_NEW_IDXBUF       (1 << 22)
#define NVC0_NEW_SURFACES     (1 << 23)
#define NVC0_NEW_MIN_SAMPLES  (1 << 24)

#define NVC0_NEW_CP_PROGRAM   (1 << 0)
#define NVC0_NEW_CP_SURFACES  (1 << 1)
#define NVC0_NEW_CP_TEXTURES  (1 << 2)
#define NVC0_NEW_CP_SAMPLERS  (1 << 3)
#define NVC0_NEW_CP_CONSTBUF  (1 << 4)
#define NVC0_NEW_CP_GLOBALS   (1 << 5)

/* 3d bufctx (during draw_vbo, blit_3d) */
#define NVC0_BIND_FB            0
#define NVC0_BIND_VTX           1
#define NVC0_BIND_VTX_TMP       2
#define NVC0_BIND_IDX           3
#define NVC0_BIND_TEX(s, i)  (  4 + 32 * (s) + (i))
#define NVC0_BIND_CB(s, i)   (164 + 16 * (s) + (i))
#define NVC0_BIND_TFB         244
#define NVC0_BIND_SUF         245
#define NVC0_BIND_SCREEN      246
#define NVC0_BIND_TLS         247
#define NVC0_BIND_3D_COUNT    248

/* compute bufctx (during launch_grid) */
#define NVC0_BIND_CP_CB(i)     (  0 + (i))
#define NVC0_BIND_CP_TEX(i)    ( 16 + (i))
#define NVC0_BIND_CP_SUF         48
#define NVC0_BIND_CP_GLOBAL      49
#define NVC0_BIND_CP_DESC        50
#define NVC0_BIND_CP_SCREEN      51
#define NVC0_BIND_CP_QUERY       52
#define NVC0_BIND_CP_COUNT       53

/* bufctx for other operations */
#define NVC0_BIND_2D            0
#define NVC0_BIND_M2MF          0
#define NVC0_BIND_FENCE         1


struct nvc0_blitctx;

boolean nvc0_blitctx_create(struct nvc0_context *);
void nvc0_blitctx_destroy(struct nvc0_context *);

struct nvc0_context {
   struct nouveau_context base;

   struct nouveau_bufctx *bufctx_3d;
   struct nouveau_bufctx *bufctx;
   struct nouveau_bufctx *bufctx_cp;

   struct nvc0_screen *screen;

   void (*m2mf_copy_rect)(struct nvc0_context *,
                          const struct nv50_m2mf_rect *dst,
                          const struct nv50_m2mf_rect *src,
                          uint32_t nblocksx, uint32_t nblocksy);

   uint32_t dirty;
   uint32_t dirty_cp; /* dirty flags for compute state */

   struct nvc0_graph_state state;

   struct nvc0_blend_stateobj *blend;
   struct nvc0_rasterizer_stateobj *rast;
   struct nvc0_zsa_stateobj *zsa;
   struct nvc0_vertex_stateobj *vertex;

   struct nvc0_program *vertprog;
   struct nvc0_program *tctlprog;
   struct nvc0_program *tevlprog;
   struct nvc0_program *gmtyprog;
   struct nvc0_program *fragprog;
   struct nvc0_program *compprog;

   struct nvc0_constbuf constbuf[6][NVC0_MAX_PIPE_CONSTBUFS];
   uint16_t constbuf_dirty[6];
   uint16_t constbuf_valid[6];
   boolean cb_dirty;

   struct pipe_vertex_buffer vtxbuf[PIPE_MAX_ATTRIBS];
   unsigned num_vtxbufs;
   struct pipe_index_buffer idxbuf;
   uint32_t constant_vbos;
   uint32_t vbo_user; /* bitmask of vertex buffers pointing to user memory */
   uint32_t vb_elt_first; /* from pipe_draw_info, for vertex upload */
   uint32_t vb_elt_limit; /* max - min element (count - 1) */
   uint32_t instance_off; /* current base vertex for instanced arrays */
   uint32_t instance_max; /* last instance for current draw call */

   struct pipe_sampler_view *textures[6][PIPE_MAX_SAMPLERS];
   unsigned num_textures[6];
   uint32_t textures_dirty[6];
   struct nv50_tsc_entry *samplers[6][PIPE_MAX_SAMPLERS];
   unsigned num_samplers[6];
   uint16_t samplers_dirty[6];

   uint32_t tex_handles[6][PIPE_MAX_SAMPLERS]; /* for nve4 */

   struct pipe_framebuffer_state framebuffer;
   struct pipe_blend_color blend_colour;
   struct pipe_stencil_ref stencil_ref;
   struct pipe_poly_stipple stipple;
   struct pipe_scissor_state scissors[NVC0_MAX_VIEWPORTS];
   unsigned scissors_dirty;
   struct pipe_viewport_state viewports[NVC0_MAX_VIEWPORTS];
   unsigned viewports_dirty;
   struct pipe_clip_state clip;

   unsigned sample_mask;
   unsigned min_samples;

   boolean vbo_push_hint;

   uint8_t tfbbuf_dirty;
   struct pipe_stream_output_target *tfbbuf[4];
   unsigned num_tfbbufs;

   struct pipe_query *cond_query;
   boolean cond_cond; /* inverted rendering condition */
   uint cond_mode;
   uint32_t cond_condmode; /* the calculated condition */

   struct nvc0_blitctx *blit;

   struct pipe_surface *surfaces[2][NVC0_MAX_SURFACE_SLOTS];
   uint16_t surfaces_dirty[2];
   uint16_t surfaces_valid[2];

   struct util_dynarray global_residents;
};

static INLINE struct nvc0_context *
nvc0_context(struct pipe_context *pipe)
{
   return (struct nvc0_context *)pipe;
}

static INLINE unsigned
nvc0_shader_stage(unsigned pipe)
{
   switch (pipe) {
   case PIPE_SHADER_VERTEX: return 0;
/* case PIPE_SHADER_TESSELLATION_CONTROL: return 1; */
/* case PIPE_SHADER_TESSELLATION_EVALUATION: return 2; */
   case PIPE_SHADER_GEOMETRY: return 3;
   case PIPE_SHADER_FRAGMENT: return 4;
   case PIPE_SHADER_COMPUTE: return 5;
   default:
      assert(!"invalid PIPE_SHADER type");
      return 0;
   }
}


/* nvc0_context.c */
struct pipe_context *nvc0_create(struct pipe_screen *, void *);
void nvc0_bufctx_fence(struct nvc0_context *, struct nouveau_bufctx *,
                       boolean on_flush);
void nvc0_default_kick_notify(struct nouveau_pushbuf *);

/* nvc0_draw.c */
extern struct draw_stage *nvc0_draw_render_stage(struct nvc0_context *);

/* nvc0_program.c */
boolean nvc0_program_translate(struct nvc0_program *, uint16_t chipset);
boolean nvc0_program_upload_code(struct nvc0_context *, struct nvc0_program *);
void nvc0_program_destroy(struct nvc0_context *, struct nvc0_program *);
void nvc0_program_library_upload(struct nvc0_context *);
uint32_t nvc0_program_symbol_offset(const struct nvc0_program *,
                                    uint32_t label);

/* nvc0_query.c */
void nvc0_init_query_functions(struct nvc0_context *);
void nvc0_query_pushbuf_submit(struct nouveau_pushbuf *,
                               struct pipe_query *, unsigned result_offset);
void nvc0_query_fifo_wait(struct nouveau_pushbuf *, struct pipe_query *);
void nvc0_so_target_save_offset(struct pipe_context *,
                                struct pipe_stream_output_target *, unsigned i,
                                boolean *serialize);

#define NVC0_QUERY_TFB_BUFFER_OFFSET (PIPE_QUERY_TYPES + 0)

/* nvc0_shader_state.c */
void nvc0_vertprog_validate(struct nvc0_context *);
void nvc0_tctlprog_validate(struct nvc0_context *);
void nvc0_tevlprog_validate(struct nvc0_context *);
void nvc0_gmtyprog_validate(struct nvc0_context *);
void nvc0_fragprog_validate(struct nvc0_context *);

void nvc0_tfb_validate(struct nvc0_context *);

/* nvc0_state.c */
extern void nvc0_init_state_functions(struct nvc0_context *);

/* nvc0_state_validate.c */
void nvc0_validate_global_residents(struct nvc0_context *,
                                    struct nouveau_bufctx *, int bin);
extern boolean nvc0_state_validate(struct nvc0_context *, uint32_t state_mask,
                                   unsigned space_words);

/* nvc0_surface.c */
extern void nvc0_clear(struct pipe_context *, unsigned buffers,
                       const union pipe_color_union *color,
                       double depth, unsigned stencil);
extern void nvc0_init_surface_functions(struct nvc0_context *);

/* nvc0_tex.c */
boolean nve4_validate_tsc(struct nvc0_context *nvc0, int s);
void nvc0_validate_textures(struct nvc0_context *);
void nvc0_validate_samplers(struct nvc0_context *);
void nve4_set_tex_handles(struct nvc0_context *);
void nvc0_validate_surfaces(struct nvc0_context *);
void nve4_set_surface_info(struct nouveau_pushbuf *, struct pipe_surface *,
                           struct nvc0_screen *);

struct pipe_sampler_view *
nvc0_create_texture_view(struct pipe_context *,
                         struct pipe_resource *,
                         const struct pipe_sampler_view *,
                         uint32_t flags,
                         enum pipe_texture_target);
struct pipe_sampler_view *
nvc0_create_sampler_view(struct pipe_context *,
                         struct pipe_resource *,
                         const struct pipe_sampler_view *);

/* nvc0_transfer.c */
void
nvc0_init_transfer_functions(struct nvc0_context *);

void
nvc0_m2mf_push_linear(struct nouveau_context *nv,
                      struct nouveau_bo *dst, unsigned offset, unsigned domain,
                      unsigned size, const void *data);
void
nve4_p2mf_push_linear(struct nouveau_context *nv,
                      struct nouveau_bo *dst, unsigned offset, unsigned domain,
                      unsigned size, const void *data);
void
nvc0_cb_push(struct nouveau_context *,
             struct nouveau_bo *bo, unsigned domain,
             unsigned base, unsigned size,
             unsigned offset, unsigned words, const uint32_t *data);

/* nvc0_vbo.c */
void nvc0_draw_vbo(struct pipe_context *, const struct pipe_draw_info *);

void *
nvc0_vertex_state_create(struct pipe_context *pipe,
                         unsigned num_elements,
                         const struct pipe_vertex_element *elements);
void
nvc0_vertex_state_delete(struct pipe_context *pipe, void *hwcso);

void nvc0_vertex_arrays_validate(struct nvc0_context *);

void nvc0_idxbuf_validate(struct nvc0_context *);

/* nvc0_video.c */
struct pipe_video_codec *
nvc0_create_decoder(struct pipe_context *context,
                    const struct pipe_video_codec *templ);

struct pipe_video_buffer *
nvc0_video_buffer_create(struct pipe_context *pipe,
                         const struct pipe_video_buffer *templat);

/* nvc0_push.c */
void nvc0_push_vbo(struct nvc0_context *, const struct pipe_draw_info *);

/* nve4_compute.c */
void nve4_launch_grid(struct pipe_context *,
                      const uint *, const uint *, uint32_t, const void *);

/* nvc0_compute.c */
void nvc0_launch_grid(struct pipe_context *,
                      const uint *, const uint *, uint32_t, const void *);

#endif
