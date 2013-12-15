/**************************************************************************
 *
 * Copyright 2009 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL VMWARE AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

/**
 * @file
 * Texture sampling -- common code.
 *
 * @author Jose Fonseca <jfonseca@vmware.com>
 */

#include "pipe/p_defines.h"
#include "pipe/p_state.h"
#include "util/u_format.h"
#include "util/u_math.h"
#include "lp_bld_arit.h"
#include "lp_bld_const.h"
#include "lp_bld_debug.h"
#include "lp_bld_printf.h"
#include "lp_bld_flow.h"
#include "lp_bld_sample.h"
#include "lp_bld_swizzle.h"
#include "lp_bld_type.h"
#include "lp_bld_logic.h"
#include "lp_bld_pack.h"
#include "lp_bld_quad.h"


/*
 * Bri-linear factor. Should be greater than one.
 */
#define BRILINEAR_FACTOR 2

/**
 * Does the given texture wrap mode allow sampling the texture border color?
 * XXX maybe move this into gallium util code.
 */
boolean
lp_sampler_wrap_mode_uses_border_color(unsigned mode,
                                       unsigned min_img_filter,
                                       unsigned mag_img_filter)
{
   switch (mode) {
   case PIPE_TEX_WRAP_REPEAT:
   case PIPE_TEX_WRAP_CLAMP_TO_EDGE:
   case PIPE_TEX_WRAP_MIRROR_REPEAT:
   case PIPE_TEX_WRAP_MIRROR_CLAMP_TO_EDGE:
      return FALSE;
   case PIPE_TEX_WRAP_CLAMP:
   case PIPE_TEX_WRAP_MIRROR_CLAMP:
      if (min_img_filter == PIPE_TEX_FILTER_NEAREST &&
          mag_img_filter == PIPE_TEX_FILTER_NEAREST) {
         return FALSE;
      } else {
         return TRUE;
      }
   case PIPE_TEX_WRAP_CLAMP_TO_BORDER:
   case PIPE_TEX_WRAP_MIRROR_CLAMP_TO_BORDER:
      return TRUE;
   default:
      assert(0 && "unexpected wrap mode");
      return FALSE;
   }
}


/**
 * Initialize lp_sampler_static_texture_state object with the gallium
 * texture/sampler_view state (this contains the parts which are
 * considered static).
 */
void
lp_sampler_static_texture_state(struct lp_static_texture_state *state,
                                const struct pipe_sampler_view *view)
{
   const struct pipe_resource *texture;

   memset(state, 0, sizeof *state);

   if (!view || !view->texture)
      return;

   texture = view->texture;

   state->format            = view->format;
   state->swizzle_r         = view->swizzle_r;
   state->swizzle_g         = view->swizzle_g;
   state->swizzle_b         = view->swizzle_b;
   state->swizzle_a         = view->swizzle_a;

   state->target            = texture->target;
   state->pot_width         = util_is_power_of_two(texture->width0);
   state->pot_height        = util_is_power_of_two(texture->height0);
   state->pot_depth         = util_is_power_of_two(texture->depth0);
   state->level_zero_only   = !view->u.tex.last_level;

   /*
    * the layer / element / level parameters are all either dynamic
    * state or handled transparently wrt execution.
    */
}


/**
 * Initialize lp_sampler_static_sampler_state object with the gallium sampler
 * state (this contains the parts which are considered static).
 */
void
lp_sampler_static_sampler_state(struct lp_static_sampler_state *state,
                                const struct pipe_sampler_state *sampler)
{
   memset(state, 0, sizeof *state);

   if (!sampler)
      return;

   /*
    * We don't copy sampler state over unless it is actually enabled, to avoid
    * spurious recompiles, as the sampler static state is part of the shader
    * key.
    *
    * Ideally the state tracker or cso_cache module would make all state
    * canonical, but until that happens it's better to be safe than sorry here.
    *
    * XXX: Actually there's much more than can be done here, especially
    * regarding 1D/2D/3D/CUBE textures, wrap modes, etc.
    */

   state->wrap_s            = sampler->wrap_s;
   state->wrap_t            = sampler->wrap_t;
   state->wrap_r            = sampler->wrap_r;
   state->min_img_filter    = sampler->min_img_filter;
   state->mag_img_filter    = sampler->mag_img_filter;

   if (sampler->max_lod > 0.0f) {
      state->min_mip_filter = sampler->min_mip_filter;
   } else {
      state->min_mip_filter = PIPE_TEX_MIPFILTER_NONE;
   }

   if (state->min_mip_filter != PIPE_TEX_MIPFILTER_NONE) {
      if (sampler->lod_bias != 0.0f) {
         state->lod_bias_non_zero = 1;
      }

      /* If min_lod == max_lod we can greatly simplify mipmap selection.
       * This is a case that occurs during automatic mipmap generation.
       */
      if (sampler->min_lod == sampler->max_lod) {
         state->min_max_lod_equal = 1;
      } else {
         if (sampler->min_lod > 0.0f) {
            state->apply_min_lod = 1;
         }

         /*
          * XXX this won't do anything with the mesa state tracker which always
          * sets max_lod to not more than actually present mip maps...
          */
         if (sampler->max_lod < (PIPE_MAX_TEXTURE_LEVELS - 1)) {
            state->apply_max_lod = 1;
         }
      }
   }

   state->compare_mode      = sampler->compare_mode;
   if (sampler->compare_mode != PIPE_TEX_COMPARE_NONE) {
      state->compare_func   = sampler->compare_func;
   }

   state->normalized_coords = sampler->normalized_coords;
}


/**
 * Generate code to compute coordinate gradient (rho).
 * \param derivs  partial derivatives of (s, t, r, q) with respect to X and Y
 *
 * The resulting rho is scalar per quad.
 */
static LLVMValueRef
lp_build_rho(struct lp_build_sample_context *bld,
             unsigned texture_unit,
             LLVMValueRef s,
             LLVMValueRef t,
             LLVMValueRef r,
             LLVMValueRef cube_rho,
             const struct lp_derivatives *derivs)
{
   struct gallivm_state *gallivm = bld->gallivm;
   struct lp_build_context *int_size_bld = &bld->int_size_in_bld;
   struct lp_build_context *float_size_bld = &bld->float_size_in_bld;
   struct lp_build_context *float_bld = &bld->float_bld;
   struct lp_build_context *coord_bld = &bld->coord_bld;
   struct lp_build_context *levelf_bld = &bld->levelf_bld;
   const unsigned dims = bld->dims;
   LLVMValueRef ddx_ddy[2];
   LLVMBuilderRef builder = bld->gallivm->builder;
   LLVMTypeRef i32t = LLVMInt32TypeInContext(bld->gallivm->context);
   LLVMValueRef index0 = LLVMConstInt(i32t, 0, 0);
   LLVMValueRef index1 = LLVMConstInt(i32t, 1, 0);
   LLVMValueRef index2 = LLVMConstInt(i32t, 2, 0);
   LLVMValueRef rho_vec;
   LLVMValueRef int_size, float_size;
   LLVMValueRef rho;
   LLVMValueRef first_level, first_level_vec;
   unsigned length = coord_bld->type.length;
   unsigned num_quads = length / 4;
   unsigned i;
   LLVMValueRef i32undef = LLVMGetUndef(LLVMInt32TypeInContext(gallivm->context));
   LLVMValueRef rho_xvec, rho_yvec;

   /* Note that all simplified calculations will only work for isotropic filtering */

   assert(bld->num_lods != length);

   first_level = bld->dynamic_state->first_level(bld->dynamic_state,
                                                 bld->gallivm, texture_unit);
   first_level_vec = lp_build_broadcast_scalar(int_size_bld, first_level);
   int_size = lp_build_minify(int_size_bld, bld->int_size, first_level_vec);
   float_size = lp_build_int_to_float(float_size_bld, int_size);

   if (cube_rho) {
      LLVMValueRef cubesize;
      LLVMValueRef index0 = lp_build_const_int32(gallivm, 0);
      /*
       * Cube map code did already everything except size mul and per-quad extraction.
       */
      rho = lp_build_pack_aos_scalars(bld->gallivm, coord_bld->type,
                                      levelf_bld->type, cube_rho, 0);
      if (gallivm_debug & GALLIVM_DEBUG_NO_RHO_APPROX) {
         rho = lp_build_sqrt(levelf_bld, rho);
      }
      /* Could optimize this for single quad just skip the broadcast */
      cubesize = lp_build_extract_broadcast(gallivm, bld->float_size_in_type,
                                            levelf_bld->type, float_size, index0);
      rho = lp_build_mul(levelf_bld, cubesize, rho);
   }
   else if (derivs && !(bld->static_texture_state->target == PIPE_TEXTURE_CUBE)) {
      LLVMValueRef ddmax[3], ddx[3], ddy[3];
      for (i = 0; i < dims; i++) {
         LLVMValueRef floatdim;
         LLVMValueRef indexi = lp_build_const_int32(gallivm, i);

         floatdim = lp_build_extract_broadcast(gallivm, bld->float_size_in_type,
                                               coord_bld->type, float_size, indexi);

         if ((gallivm_debug & GALLIVM_DEBUG_NO_RHO_APPROX) && (dims > 1)) {
            ddx[i] = lp_build_mul(coord_bld, floatdim, derivs->ddx[i]);
            ddy[i] = lp_build_mul(coord_bld, floatdim, derivs->ddy[i]);
            ddx[i] = lp_build_mul(coord_bld, ddx[i], ddx[i]);
            ddy[i] = lp_build_mul(coord_bld, ddy[i], ddy[i]);
         }
         else {
            LLVMValueRef tmpx, tmpy;
            tmpx = lp_build_abs(coord_bld, derivs->ddx[i]);
            tmpy = lp_build_abs(coord_bld, derivs->ddy[i]);
            ddmax[i] = lp_build_max(coord_bld, tmpx, tmpy);
            ddmax[i] = lp_build_mul(coord_bld, floatdim, ddmax[i]);
         }
      }
      if ((gallivm_debug & GALLIVM_DEBUG_NO_RHO_APPROX) && (dims > 1)) {
         rho_xvec = lp_build_add(coord_bld, ddx[0], ddx[1]);
         rho_yvec = lp_build_add(coord_bld, ddy[0], ddy[1]);
         if (dims > 2) {
            rho_xvec = lp_build_add(coord_bld, rho_xvec, ddx[2]);
            rho_yvec = lp_build_add(coord_bld, rho_yvec, ddy[2]);
         }
         rho_vec = lp_build_max(coord_bld, rho_xvec, rho_yvec);
         rho = lp_build_pack_aos_scalars(bld->gallivm, coord_bld->type,
                                         levelf_bld->type, rho_vec, 0);
         /*
          * note that as long as we don't care about per-pixel lod could reduce math
          * more (at some shuffle cost), but for now only do sqrt after packing.
          */
         rho = lp_build_sqrt(levelf_bld, rho);
      }
      else {
         rho_vec = ddmax[0];
         if (dims > 1) {
            rho_vec = lp_build_max(coord_bld, rho_vec, ddmax[1]);
            if (dims > 2) {
               rho_vec = lp_build_max(coord_bld, rho_vec, ddmax[2]);
            }
         }
         /*
          * rho_vec now still contains per-pixel rho, convert to scalar per quad
          * since we can't handle per-pixel rho/lod from now on (TODO).
          */
         rho = lp_build_pack_aos_scalars(bld->gallivm, coord_bld->type,
                                         levelf_bld->type, rho_vec, 0);
      }
   }
   else {
      /*
       * This looks all a bit complex, but it's not that bad
       * (the shuffle code makes it look worse than it is).
       * Still, might not be ideal for all cases.
       */
      static const unsigned char swizzle0[] = { /* no-op swizzle */
         0, LP_BLD_SWIZZLE_DONTCARE,
         LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
      };
      static const unsigned char swizzle1[] = {
         1, LP_BLD_SWIZZLE_DONTCARE,
         LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
      };
      static const unsigned char swizzle2[] = {
         2, LP_BLD_SWIZZLE_DONTCARE,
         LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
      };

      if (dims < 2) {
         ddx_ddy[0] = lp_build_packed_ddx_ddy_onecoord(coord_bld, s);
      }
      else if (dims >= 2) {
         ddx_ddy[0] = lp_build_packed_ddx_ddy_twocoord(coord_bld, s, t);
         if (dims > 2) {
            ddx_ddy[1] = lp_build_packed_ddx_ddy_onecoord(coord_bld, r);
         }
      }

      if ((gallivm_debug & GALLIVM_DEBUG_NO_RHO_APPROX) && (dims > 1)) {
         static const unsigned char swizzle01[] = { /* no-op swizzle */
            0, 1,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };
         static const unsigned char swizzle23[] = {
            2, 3,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };
         LLVMValueRef ddx_ddys, ddx_ddyt, floatdim, shuffles[LP_MAX_VECTOR_LENGTH / 4];

         for (i = 0; i < num_quads; i++) {
            shuffles[i*4+0] = shuffles[i*4+1] = index0;
            shuffles[i*4+2] = shuffles[i*4+3] = index1;
         }
         floatdim = LLVMBuildShuffleVector(builder, float_size, float_size,
                                           LLVMConstVector(shuffles, length), "");
         ddx_ddy[0] = lp_build_mul(coord_bld, ddx_ddy[0], floatdim);
         ddx_ddy[0] = lp_build_mul(coord_bld, ddx_ddy[0], ddx_ddy[0]);
         ddx_ddys = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle01);
         ddx_ddyt = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle23);
         rho_vec = lp_build_add(coord_bld, ddx_ddys, ddx_ddyt);

         if (dims > 2) {
            static const unsigned char swizzle02[] = {
               0, 2,
               LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
            };
            floatdim = lp_build_extract_broadcast(gallivm, bld->float_size_in_type,
                                                  coord_bld->type, float_size, index2);
            ddx_ddy[1] = lp_build_mul(coord_bld, ddx_ddy[1], floatdim);
            ddx_ddy[1] = lp_build_mul(coord_bld, ddx_ddy[1], ddx_ddy[1]);
            ddx_ddy[1] = lp_build_swizzle_aos(coord_bld, ddx_ddy[1], swizzle02);
            rho_vec = lp_build_add(coord_bld, rho_vec, ddx_ddy[1]);
         }
         rho_xvec = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle0);
         rho_yvec = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle1);
         rho_vec = lp_build_max(coord_bld, rho_xvec, rho_yvec);

         rho = lp_build_pack_aos_scalars(bld->gallivm, coord_bld->type,
                                         levelf_bld->type, rho_vec, 0);
         rho = lp_build_sqrt(levelf_bld, rho);
      }
      else {
         ddx_ddy[0] = lp_build_abs(coord_bld, ddx_ddy[0]);
         if (dims > 2) {
            ddx_ddy[1] = lp_build_abs(coord_bld, ddx_ddy[1]);
         }

         if (dims < 2) {
            rho_xvec = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle0);
            rho_yvec = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle1);
         }
         else if (dims == 2) {
            static const unsigned char swizzle02[] = {
               0, 2,
               LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
            };
            static const unsigned char swizzle13[] = {
               1, 3,
               LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
            };
            rho_xvec = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle02);
            rho_yvec = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle13);
         }
         else {
            LLVMValueRef shuffles1[LP_MAX_VECTOR_LENGTH];
            LLVMValueRef shuffles2[LP_MAX_VECTOR_LENGTH];
            assert(dims == 3);
            for (i = 0; i < num_quads; i++) {
               shuffles1[4*i + 0] = lp_build_const_int32(gallivm, 4*i);
               shuffles1[4*i + 1] = lp_build_const_int32(gallivm, 4*i + 2);
               shuffles1[4*i + 2] = lp_build_const_int32(gallivm, length + 4*i);
               shuffles1[4*i + 3] = i32undef;
               shuffles2[4*i + 0] = lp_build_const_int32(gallivm, 4*i + 1);
               shuffles2[4*i + 1] = lp_build_const_int32(gallivm, 4*i + 3);
               shuffles2[4*i + 2] = lp_build_const_int32(gallivm, length + 4*i + 2);
               shuffles2[4*i + 3] = i32undef;
            }
            rho_xvec = LLVMBuildShuffleVector(builder, ddx_ddy[0], ddx_ddy[1],
                                              LLVMConstVector(shuffles1, length), "");
            rho_yvec = LLVMBuildShuffleVector(builder, ddx_ddy[0], ddx_ddy[1],
                                              LLVMConstVector(shuffles2, length), "");
         }

         rho_vec = lp_build_max(coord_bld, rho_xvec, rho_yvec);

         if (bld->coord_type.length > 4) {
            /* expand size to each quad */
            if (dims > 1) {
               /* could use some broadcast_vector helper for this? */
               LLVMValueRef src[LP_MAX_VECTOR_LENGTH/4];
               for (i = 0; i < num_quads; i++) {
                  src[i] = float_size;
               }
               float_size = lp_build_concat(bld->gallivm, src, float_size_bld->type, num_quads);
            }
            else {
               float_size = lp_build_broadcast_scalar(coord_bld, float_size);
            }
            rho_vec = lp_build_mul(coord_bld, rho_vec, float_size);

            if (dims <= 1) {
               rho = rho_vec;
            }
            else {
               if (dims >= 2) {
                  LLVMValueRef rho_s, rho_t, rho_r;

                  rho_s = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle0);
                  rho_t = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle1);

                  rho = lp_build_max(coord_bld, rho_s, rho_t);

                  if (dims >= 3) {
                     rho_r = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle2);
                     rho = lp_build_max(coord_bld, rho, rho_r);
                  }
               }
            }
            rho = lp_build_pack_aos_scalars(bld->gallivm, coord_bld->type,
                                            levelf_bld->type, rho, 0);
         }
         else {
            if (dims <= 1) {
               rho_vec = LLVMBuildExtractElement(builder, rho_vec, index0, "");
            }
            rho_vec = lp_build_mul(float_size_bld, rho_vec, float_size);

            if (dims <= 1) {
               rho = rho_vec;
            }
            else {
               if (dims >= 2) {
                  LLVMValueRef rho_s, rho_t, rho_r;

                  rho_s = LLVMBuildExtractElement(builder, rho_vec, index0, "");
                  rho_t = LLVMBuildExtractElement(builder, rho_vec, index1, "");

                  rho = lp_build_max(float_bld, rho_s, rho_t);

                  if (dims >= 3) {
                     rho_r = LLVMBuildExtractElement(builder, rho_vec, index2, "");
                     rho = lp_build_max(float_bld, rho, rho_r);
                  }
               }
            }
         }
      }
   }

   return rho;
}


/*
 * Bri-linear lod computation
 *
 * Use a piece-wise linear approximation of log2 such that:
 * - round to nearest, for values in the neighborhood of -1, 0, 1, 2, etc.
 * - linear approximation for values in the neighborhood of 0.5, 1.5., etc,
 *   with the steepness specified in 'factor'
 * - exact result for 0.5, 1.5, etc.
 *
 *
 *   1.0 -              /----*
 *                     /
 *                    /
 *                   /
 *   0.5 -          *
 *                 /
 *                /
 *               /
 *   0.0 - *----/
 *
 *         |                 |
 *        2^0               2^1
 *
 * This is a technique also commonly used in hardware:
 * - http://ixbtlabs.com/articles2/gffx/nv40-rx800-3.html
 *
 * TODO: For correctness, this should only be applied when texture is known to
 * have regular mipmaps, i.e., mipmaps derived from the base level.
 *
 * TODO: This could be done in fixed point, where applicable.
 */
static void
lp_build_brilinear_lod(struct lp_build_context *bld,
                       LLVMValueRef lod,
                       double factor,
                       LLVMValueRef *out_lod_ipart,
                       LLVMValueRef *out_lod_fpart)
{
   LLVMValueRef lod_fpart;
   double pre_offset = (factor - 0.5)/factor - 0.5;
   double post_offset = 1 - factor;

   if (0) {
      lp_build_printf(bld->gallivm, "lod = %f\n", lod);
   }

   lod = lp_build_add(bld, lod,
                      lp_build_const_vec(bld->gallivm, bld->type, pre_offset));

   lp_build_ifloor_fract(bld, lod, out_lod_ipart, &lod_fpart);

   lod_fpart = lp_build_mul(bld, lod_fpart,
                            lp_build_const_vec(bld->gallivm, bld->type, factor));

   lod_fpart = lp_build_add(bld, lod_fpart,
                            lp_build_const_vec(bld->gallivm, bld->type, post_offset));

   /*
    * It's not necessary to clamp lod_fpart since:
    * - the above expression will never produce numbers greater than one.
    * - the mip filtering branch is only taken if lod_fpart is positive
    */

   *out_lod_fpart = lod_fpart;

   if (0) {
      lp_build_printf(bld->gallivm, "lod_ipart = %i\n", *out_lod_ipart);
      lp_build_printf(bld->gallivm, "lod_fpart = %f\n\n", *out_lod_fpart);
   }
}


/*
 * Combined log2 and brilinear lod computation.
 *
 * It's in all identical to calling lp_build_fast_log2() and
 * lp_build_brilinear_lod() above, but by combining we can compute the integer
 * and fractional part independently.
 */
static void
lp_build_brilinear_rho(struct lp_build_context *bld,
                       LLVMValueRef rho,
                       double factor,
                       LLVMValueRef *out_lod_ipart,
                       LLVMValueRef *out_lod_fpart)
{
   LLVMValueRef lod_ipart;
   LLVMValueRef lod_fpart;

   const double pre_factor = (2*factor - 0.5)/(M_SQRT2*factor);
   const double post_offset = 1 - 2*factor;

   assert(bld->type.floating);

   assert(lp_check_value(bld->type, rho));

   /*
    * The pre factor will make the intersections with the exact powers of two
    * happen precisely where we want then to be, which means that the integer
    * part will not need any post adjustments.
    */
   rho = lp_build_mul(bld, rho,
                      lp_build_const_vec(bld->gallivm, bld->type, pre_factor));

   /* ipart = ifloor(log2(rho)) */
   lod_ipart = lp_build_extract_exponent(bld, rho, 0);

   /* fpart = rho / 2**ipart */
   lod_fpart = lp_build_extract_mantissa(bld, rho);

   lod_fpart = lp_build_mul(bld, lod_fpart,
                            lp_build_const_vec(bld->gallivm, bld->type, factor));

   lod_fpart = lp_build_add(bld, lod_fpart,
                            lp_build_const_vec(bld->gallivm, bld->type, post_offset));

   /*
    * Like lp_build_brilinear_lod, it's not necessary to clamp lod_fpart since:
    * - the above expression will never produce numbers greater than one.
    * - the mip filtering branch is only taken if lod_fpart is positive
    */

   *out_lod_ipart = lod_ipart;
   *out_lod_fpart = lod_fpart;
}


/**
 * Generate code to compute texture level of detail (lambda).
 * \param derivs  partial derivatives of (s, t, r, q) with respect to X and Y
 * \param lod_bias  optional float vector with the shader lod bias
 * \param explicit_lod  optional float vector with the explicit lod
 * \param width  scalar int texture width
 * \param height  scalar int texture height
 * \param depth  scalar int texture depth
 *
 * The resulting lod is scalar per quad, so only the first value per quad
 * passed in from lod_bias, explicit_lod is used.
 */
void
lp_build_lod_selector(struct lp_build_sample_context *bld,
                      unsigned texture_unit,
                      unsigned sampler_unit,
                      LLVMValueRef s,
                      LLVMValueRef t,
                      LLVMValueRef r,
                      LLVMValueRef cube_rho,
                      const struct lp_derivatives *derivs,
                      LLVMValueRef lod_bias, /* optional */
                      LLVMValueRef explicit_lod, /* optional */
                      unsigned mip_filter,
                      LLVMValueRef *out_lod_ipart,
                      LLVMValueRef *out_lod_fpart)

{
   LLVMBuilderRef builder = bld->gallivm->builder;
   struct lp_build_context *levelf_bld = &bld->levelf_bld;
   LLVMValueRef lod;

   *out_lod_ipart = bld->leveli_bld.zero;
   *out_lod_fpart = levelf_bld->zero;

   if (bld->static_sampler_state->min_max_lod_equal) {
      /* User is forcing sampling from a particular mipmap level.
       * This is hit during mipmap generation.
       */
      LLVMValueRef min_lod =
         bld->dynamic_state->min_lod(bld->dynamic_state,
                                     bld->gallivm, sampler_unit);

      lod = lp_build_broadcast_scalar(levelf_bld, min_lod);
   }
   else {
      if (explicit_lod) {
         if (bld->num_lods != bld->coord_type.length)
            lod = lp_build_pack_aos_scalars(bld->gallivm, bld->coord_bld.type,
                                            levelf_bld->type, explicit_lod, 0);
         else
            lod = explicit_lod;
      }
      else {
         LLVMValueRef rho;

         rho = lp_build_rho(bld, texture_unit, s, t, r, cube_rho, derivs);

         /*
          * Compute lod = log2(rho)
          */

         if (!lod_bias &&
             !bld->static_sampler_state->lod_bias_non_zero &&
             !bld->static_sampler_state->apply_max_lod &&
             !bld->static_sampler_state->apply_min_lod) {
            /*
             * Special case when there are no post-log2 adjustments, which
             * saves instructions but keeping the integer and fractional lod
             * computations separate from the start.
             */

            if (mip_filter == PIPE_TEX_MIPFILTER_NONE ||
                mip_filter == PIPE_TEX_MIPFILTER_NEAREST) {
               *out_lod_ipart = lp_build_ilog2(levelf_bld, rho);
               *out_lod_fpart = levelf_bld->zero;
               return;
            }
            if (mip_filter == PIPE_TEX_MIPFILTER_LINEAR &&
                !(gallivm_debug & GALLIVM_DEBUG_NO_BRILINEAR)) {
               lp_build_brilinear_rho(levelf_bld, rho, BRILINEAR_FACTOR,
                                      out_lod_ipart, out_lod_fpart);
               return;
            }
         }

         if (0) {
            lod = lp_build_log2(levelf_bld, rho);
         }
         else {
            lod = lp_build_fast_log2(levelf_bld, rho);
         }

         /* add shader lod bias */
         if (lod_bias) {
            lod_bias = lp_build_pack_aos_scalars(bld->gallivm, bld->coord_bld.type,
                  levelf_bld->type, lod_bias, 0);
            lod = LLVMBuildFAdd(builder, lod, lod_bias, "shader_lod_bias");
         }
      }

      /* add sampler lod bias */
      if (bld->static_sampler_state->lod_bias_non_zero) {
         LLVMValueRef sampler_lod_bias =
            bld->dynamic_state->lod_bias(bld->dynamic_state,
                                         bld->gallivm, sampler_unit);
         sampler_lod_bias = lp_build_broadcast_scalar(levelf_bld,
                                                      sampler_lod_bias);
         lod = LLVMBuildFAdd(builder, lod, sampler_lod_bias, "sampler_lod_bias");
      }

      /* clamp lod */
      if (bld->static_sampler_state->apply_max_lod) {
         LLVMValueRef max_lod =
            bld->dynamic_state->max_lod(bld->dynamic_state,
                                        bld->gallivm, sampler_unit);
         max_lod = lp_build_broadcast_scalar(levelf_bld, max_lod);

         lod = lp_build_min(levelf_bld, lod, max_lod);
      }
      if (bld->static_sampler_state->apply_min_lod) {
         LLVMValueRef min_lod =
            bld->dynamic_state->min_lod(bld->dynamic_state,
                                        bld->gallivm, sampler_unit);
         min_lod = lp_build_broadcast_scalar(levelf_bld, min_lod);

         lod = lp_build_max(levelf_bld, lod, min_lod);
      }
   }

   if (mip_filter == PIPE_TEX_MIPFILTER_LINEAR) {
      if (!(gallivm_debug & GALLIVM_DEBUG_NO_BRILINEAR)) {
         lp_build_brilinear_lod(levelf_bld, lod, BRILINEAR_FACTOR,
                                out_lod_ipart, out_lod_fpart);
      }
      else {
         lp_build_ifloor_fract(levelf_bld, lod, out_lod_ipart, out_lod_fpart);
      }

      lp_build_name(*out_lod_fpart, "lod_fpart");
   }
   else {
      *out_lod_ipart = lp_build_iround(levelf_bld, lod);
   }

   lp_build_name(*out_lod_ipart, "lod_ipart");

   return;
}


/**
 * For PIPE_TEX_MIPFILTER_NEAREST, convert float LOD to integer
 * mipmap level index.
 * Note: this is all scalar per quad code.
 * \param lod_ipart  int texture level of detail
 * \param level_out  returns integer 
 */
void
lp_build_nearest_mip_level(struct lp_build_sample_context *bld,
                           unsigned texture_unit,
                           LLVMValueRef lod_ipart,
                           LLVMValueRef *level_out)
{
   struct lp_build_context *leveli_bld = &bld->leveli_bld;
   LLVMValueRef first_level, last_level, level;

   first_level = bld->dynamic_state->first_level(bld->dynamic_state,
                                                 bld->gallivm, texture_unit);
   last_level = bld->dynamic_state->last_level(bld->dynamic_state,
                                               bld->gallivm, texture_unit);
   first_level = lp_build_broadcast_scalar(leveli_bld, first_level);
   last_level = lp_build_broadcast_scalar(leveli_bld, last_level);

   level = lp_build_add(leveli_bld, lod_ipart, first_level);

   /* clamp level to legal range of levels */
   *level_out = lp_build_clamp(leveli_bld, level, first_level, last_level);
}


/**
 * For PIPE_TEX_MIPFILTER_LINEAR, convert per-quad int LOD(s) to two (per-quad)
 * (adjacent) mipmap level indexes, and fix up float lod part accordingly.
 * Later, we'll sample from those two mipmap levels and interpolate between them.
 */
void
lp_build_linear_mip_levels(struct lp_build_sample_context *bld,
                           unsigned texture_unit,
                           LLVMValueRef lod_ipart,
                           LLVMValueRef *lod_fpart_inout,
                           LLVMValueRef *level0_out,
                           LLVMValueRef *level1_out)
{
   LLVMBuilderRef builder = bld->gallivm->builder;
   struct lp_build_context *leveli_bld = &bld->leveli_bld;
   struct lp_build_context *levelf_bld = &bld->levelf_bld;
   LLVMValueRef first_level, last_level;
   LLVMValueRef clamp_min;
   LLVMValueRef clamp_max;

   first_level = bld->dynamic_state->first_level(bld->dynamic_state,
                                                 bld->gallivm, texture_unit);
   last_level = bld->dynamic_state->last_level(bld->dynamic_state,
                                               bld->gallivm, texture_unit);
   first_level = lp_build_broadcast_scalar(leveli_bld, first_level);
   last_level = lp_build_broadcast_scalar(leveli_bld, last_level);

   *level0_out = lp_build_add(leveli_bld, lod_ipart, first_level);
   *level1_out = lp_build_add(leveli_bld, *level0_out, leveli_bld->one);

   /*
    * Clamp both *level0_out and *level1_out to [first_level, last_level], with
    * the minimum number of comparisons, and zeroing lod_fpart in the extreme
    * ends in the process.
    */

   /*
    * This code (vector select in particular) only works with llvm 3.1
    * (if there's more than one quad, with x86 backend). Might consider
    * converting to our lp_bld_logic helpers.
    */
#if HAVE_LLVM < 0x0301
   assert(leveli_bld->type.length == 1);
#endif

   /* *level0_out < first_level */
   clamp_min = LLVMBuildICmp(builder, LLVMIntSLT,
                             *level0_out, first_level,
                             "clamp_lod_to_first");

   *level0_out = LLVMBuildSelect(builder, clamp_min,
                                 first_level, *level0_out, "");

   *level1_out = LLVMBuildSelect(builder, clamp_min,
                                 first_level, *level1_out, "");

   *lod_fpart_inout = LLVMBuildSelect(builder, clamp_min,
                                      levelf_bld->zero, *lod_fpart_inout, "");

   /* *level0_out >= last_level */
   clamp_max = LLVMBuildICmp(builder, LLVMIntSGE,
                             *level0_out, last_level,
                             "clamp_lod_to_last");

   *level0_out = LLVMBuildSelect(builder, clamp_max,
                                 last_level, *level0_out, "");

   *level1_out = LLVMBuildSelect(builder, clamp_max,
                                 last_level, *level1_out, "");

   *lod_fpart_inout = LLVMBuildSelect(builder, clamp_max,
                                      levelf_bld->zero, *lod_fpart_inout, "");

   lp_build_name(*level0_out, "texture%u_miplevel0", texture_unit);
   lp_build_name(*level1_out, "texture%u_miplevel1", texture_unit);
   lp_build_name(*lod_fpart_inout, "texture%u_mipweight", texture_unit);
}


/**
 * Return pointer to a single mipmap level.
 * \param level  integer mipmap level
 */
LLVMValueRef
lp_build_get_mipmap_level(struct lp_build_sample_context *bld,
                          LLVMValueRef level)
{
   LLVMBuilderRef builder = bld->gallivm->builder;
   LLVMValueRef indexes[2], data_ptr, mip_offset;

   indexes[0] = lp_build_const_int32(bld->gallivm, 0);
   indexes[1] = level;
   mip_offset = LLVMBuildGEP(builder, bld->mip_offsets, indexes, 2, "");
   mip_offset = LLVMBuildLoad(builder, mip_offset, "");
   data_ptr = LLVMBuildGEP(builder, bld->base_ptr, &mip_offset, 1, "");
   return data_ptr;
}

/**
 * Return (per-pixel) offsets to mip levels.
 * \param level  integer mipmap level
 */
LLVMValueRef
lp_build_get_mip_offsets(struct lp_build_sample_context *bld,
                         LLVMValueRef level)
{
   LLVMBuilderRef builder = bld->gallivm->builder;
   LLVMValueRef indexes[2], offsets, offset1;

   indexes[0] = lp_build_const_int32(bld->gallivm, 0);
   if (bld->num_lods == 1) {
      indexes[1] = level;
      offset1 = LLVMBuildGEP(builder, bld->mip_offsets, indexes, 2, "");
      offset1 = LLVMBuildLoad(builder, offset1, "");
      offsets = lp_build_broadcast_scalar(&bld->int_coord_bld, offset1);
   }
   else if (bld->num_lods == bld->coord_bld.type.length / 4) {
      unsigned i;

      offsets = bld->int_coord_bld.undef;
      for (i = 0; i < bld->num_lods; i++) {
         LLVMValueRef indexi = lp_build_const_int32(bld->gallivm, i);
         LLVMValueRef indexo = lp_build_const_int32(bld->gallivm, 4 * i);
         indexes[1] = LLVMBuildExtractElement(builder, level, indexi, "");
         offset1 = LLVMBuildGEP(builder, bld->mip_offsets, indexes, 2, "");
         offset1 = LLVMBuildLoad(builder, offset1, "");
         offsets = LLVMBuildInsertElement(builder, offsets, offset1, indexo, "");
      }
      offsets = lp_build_swizzle_scalar_aos(&bld->int_coord_bld, offsets, 0, 4);
   }
   else {
      unsigned i;

      assert (bld->num_lods == bld->coord_bld.type.length);

      offsets = bld->int_coord_bld.undef;
      for (i = 0; i < bld->num_lods; i++) {
         LLVMValueRef indexi = lp_build_const_int32(bld->gallivm, i);
         indexes[1] = LLVMBuildExtractElement(builder, level, indexi, "");
         offset1 = LLVMBuildGEP(builder, bld->mip_offsets, indexes, 2, "");
         offset1 = LLVMBuildLoad(builder, offset1, "");
         offsets = LLVMBuildInsertElement(builder, offsets, offset1, indexi, "");
      }
   }
   return offsets;
}


/**
 * Codegen equivalent for u_minify().
 * Return max(1, base_size >> level);
 */
LLVMValueRef
lp_build_minify(struct lp_build_context *bld,
                LLVMValueRef base_size,
                LLVMValueRef level)
{
   LLVMBuilderRef builder = bld->gallivm->builder;
   assert(lp_check_value(bld->type, base_size));
   assert(lp_check_value(bld->type, level));

   if (level == bld->zero) {
      /* if we're using mipmap level zero, no minification is needed */
      return base_size;
   }
   else {
      LLVMValueRef size =
         LLVMBuildLShr(builder, base_size, level, "minify");
      assert(bld->type.sign);
      size = lp_build_max(bld, size, bld->one);
      return size;
   }
}


/**
 * Dereference stride_array[mipmap_level] array to get a stride.
 * Return stride as a vector.
 */
static LLVMValueRef
lp_build_get_level_stride_vec(struct lp_build_sample_context *bld,
                              LLVMValueRef stride_array, LLVMValueRef level)
{
   LLVMBuilderRef builder = bld->gallivm->builder;
   LLVMValueRef indexes[2], stride, stride1;
   indexes[0] = lp_build_const_int32(bld->gallivm, 0);
   if (bld->num_lods == 1) {
      indexes[1] = level;
      stride1 = LLVMBuildGEP(builder, stride_array, indexes, 2, "");
      stride1 = LLVMBuildLoad(builder, stride1, "");
      stride = lp_build_broadcast_scalar(&bld->int_coord_bld, stride1);
   }
   else if (bld->num_lods == bld->coord_bld.type.length / 4) {
      LLVMValueRef stride1;
      unsigned i;

      stride = bld->int_coord_bld.undef;
      for (i = 0; i < bld->num_lods; i++) {
         LLVMValueRef indexi = lp_build_const_int32(bld->gallivm, i);
         LLVMValueRef indexo = lp_build_const_int32(bld->gallivm, 4 * i);
         indexes[1] = LLVMBuildExtractElement(builder, level, indexi, "");
         stride1 = LLVMBuildGEP(builder, stride_array, indexes, 2, "");
         stride1 = LLVMBuildLoad(builder, stride1, "");
         stride = LLVMBuildInsertElement(builder, stride, stride1, indexo, "");
      }
      stride = lp_build_swizzle_scalar_aos(&bld->int_coord_bld, stride, 0, 4);
   }
   else {
      LLVMValueRef stride1;
      unsigned i;

      assert (bld->num_lods == bld->coord_bld.type.length);

      stride = bld->int_coord_bld.undef;
      for (i = 0; i < bld->coord_bld.type.length; i++) {
         LLVMValueRef indexi = lp_build_const_int32(bld->gallivm, i);
         indexes[1] = LLVMBuildExtractElement(builder, level, indexi, "");
         stride1 = LLVMBuildGEP(builder, stride_array, indexes, 2, "");
         stride1 = LLVMBuildLoad(builder, stride1, "");
         stride = LLVMBuildInsertElement(builder, stride, stride1, indexi, "");
      }
   }
   return stride;
}


/**
 * When sampling a mipmap, we need to compute the width, height, depth
 * of the source levels from the level indexes.  This helper function
 * does that.
 */
void
lp_build_mipmap_level_sizes(struct lp_build_sample_context *bld,
                            LLVMValueRef ilevel,
                            LLVMValueRef *out_size,
                            LLVMValueRef *row_stride_vec,
                            LLVMValueRef *img_stride_vec)
{
   const unsigned dims = bld->dims;
   LLVMValueRef ilevel_vec;

   /*
    * Compute width, height, depth at mipmap level 'ilevel'
    */
   if (bld->num_lods == 1) {
      ilevel_vec = lp_build_broadcast_scalar(&bld->int_size_bld, ilevel);
      *out_size = lp_build_minify(&bld->int_size_bld, bld->int_size, ilevel_vec);
   }
   else {
      LLVMValueRef int_size_vec;
      LLVMValueRef tmp[LP_MAX_VECTOR_LENGTH];
      unsigned num_quads = bld->coord_bld.type.length / 4;
      unsigned i;

      if (bld->num_lods == num_quads) {
         /*
          * XXX: this should be #ifndef SANE_INSTRUCTION_SET.
          * intel "forgot" the variable shift count instruction until avx2.
          * A harmless 8x32 shift gets translated into 32 instructions
          * (16 extracts, 8 scalar shifts, 8 inserts), llvm is apparently
          * unable to recognize if there are really just 2 different shift
          * count values. So do the shift 4-wide before expansion.
          */
         struct lp_build_context bld4;
         struct lp_type type4;

         type4 = bld->int_coord_bld.type;
         type4.length = 4;

         lp_build_context_init(&bld4, bld->gallivm, type4);

         if (bld->dims == 1) {
            assert(bld->int_size_in_bld.type.length == 1);
            int_size_vec = lp_build_broadcast_scalar(&bld4,
                                                     bld->int_size);
         }
         else {
            assert(bld->int_size_in_bld.type.length == 4);
            int_size_vec = bld->int_size;
         }

         for (i = 0; i < num_quads; i++) {
            LLVMValueRef ileveli;
            LLVMValueRef indexi = lp_build_const_int32(bld->gallivm, i);

            ileveli = lp_build_extract_broadcast(bld->gallivm,
                                                 bld->leveli_bld.type,
                                                 bld4.type,
                                                 ilevel,
                                                 indexi);
            tmp[i] = lp_build_minify(&bld4, int_size_vec, ileveli);
         }
         /*
          * out_size is [w0, h0, d0, _, w1, h1, d1, _, ...] vector for dims > 1,
          * [w0, w0, w0, w0, w1, w1, w1, w1, ...] otherwise.
          */
         *out_size = lp_build_concat(bld->gallivm,
                                     tmp,
                                     bld4.type,
                                     num_quads);
      }
      else {
        /* FIXME: this is terrible and results in _huge_ vector
         * (for the dims > 1 case).
         * Should refactor this (together with extract_image_sizes) and do
         * something more useful. Could for instance if we have width,height
         * with 4-wide vector pack all elements into a 8xi16 vector
         * (on which we can still do useful math) instead of using a 16xi32
         * vector.
         * FIXME: some callers can't handle this yet.
         * For dims == 1 this will create [w0, w1, w2, w3, ...] vector.
         * For dims > 1 this will create [w0, h0, d0, _, w1, h1, d1, _, ...] vector.
         */
         assert(bld->num_lods == bld->coord_bld.type.length);
         if (bld->dims == 1) {
            assert(bld->int_size_in_bld.type.length == 1);
            int_size_vec = lp_build_broadcast_scalar(&bld->int_coord_bld,
                                                     bld->int_size);
            /* vector shift with variable shift count alert... */
            *out_size = lp_build_minify(&bld->int_coord_bld, int_size_vec, ilevel);
         }
         else {
            LLVMValueRef ilevel1;
            for (i = 0; i < bld->num_lods; i++) {
               LLVMValueRef indexi = lp_build_const_int32(bld->gallivm, i);
               ilevel1 = lp_build_extract_broadcast(bld->gallivm, bld->int_coord_type,
                                                    bld->int_size_in_bld.type, ilevel, indexi);
               tmp[i] = bld->int_size;
               tmp[i] = lp_build_minify(&bld->int_size_in_bld, tmp[i], ilevel1);
            }
            *out_size = lp_build_concat(bld->gallivm, tmp,
                                        bld->int_size_in_bld.type,
                                        bld->num_lods);
         }
      }
   }

   if (dims >= 2) {
      *row_stride_vec = lp_build_get_level_stride_vec(bld,
                                                      bld->row_stride_array,
                                                      ilevel);
   }
   if (dims == 3 ||
       bld->static_texture_state->target == PIPE_TEXTURE_CUBE ||
       bld->static_texture_state->target == PIPE_TEXTURE_1D_ARRAY ||
       bld->static_texture_state->target == PIPE_TEXTURE_2D_ARRAY) {
      *img_stride_vec = lp_build_get_level_stride_vec(bld,
                                                      bld->img_stride_array,
                                                      ilevel);
   }
}


/**
 * Extract and broadcast texture size.
 *
 * @param size_type   type of the texture size vector (either
 *                    bld->int_size_type or bld->float_size_type)
 * @param coord_type  type of the texture size vector (either
 *                    bld->int_coord_type or bld->coord_type)
 * @param size        vector with the texture size (width, height, depth)
 */
void
lp_build_extract_image_sizes(struct lp_build_sample_context *bld,
                             struct lp_build_context *size_bld,
                             struct lp_type coord_type,
                             LLVMValueRef size,
                             LLVMValueRef *out_width,
                             LLVMValueRef *out_height,
                             LLVMValueRef *out_depth)
{
   const unsigned dims = bld->dims;
   LLVMTypeRef i32t = LLVMInt32TypeInContext(bld->gallivm->context);
   struct lp_type size_type = size_bld->type;

   if (bld->num_lods == 1) {
      *out_width = lp_build_extract_broadcast(bld->gallivm,
                                              size_type,
                                              coord_type,
                                              size,
                                              LLVMConstInt(i32t, 0, 0));
      if (dims >= 2) {
         *out_height = lp_build_extract_broadcast(bld->gallivm,
                                                  size_type,
                                                  coord_type,
                                                  size,
                                                  LLVMConstInt(i32t, 1, 0));
         if (dims == 3) {
            *out_depth = lp_build_extract_broadcast(bld->gallivm,
                                                    size_type,
                                                    coord_type,
                                                    size,
                                                    LLVMConstInt(i32t, 2, 0));
         }
      }
   }
   else {
      unsigned num_quads = bld->coord_bld.type.length / 4;

      if (dims == 1) {
         *out_width = size;
      }
      else if (bld->num_lods == num_quads) {
         *out_width = lp_build_swizzle_scalar_aos(size_bld, size, 0, 4);
         if (dims >= 2) {
            *out_height = lp_build_swizzle_scalar_aos(size_bld, size, 1, 4);
            if (dims == 3) {
               *out_depth = lp_build_swizzle_scalar_aos(size_bld, size, 2, 4);
            }
         }
      }
      else {
         assert(bld->num_lods == bld->coord_type.length);
         *out_width = lp_build_pack_aos_scalars(bld->gallivm, size_type,
                                                coord_type, size, 0);
         if (dims >= 2) {
            *out_height = lp_build_pack_aos_scalars(bld->gallivm, size_type,
                                                    coord_type, size, 1);
            if (dims == 3) {
               *out_depth = lp_build_pack_aos_scalars(bld->gallivm, size_type,
                                                      coord_type, size, 2);
            }
         }
      }
   }
}


/**
 * Unnormalize coords.
 *
 * @param flt_size  vector with the integer texture size (width, height, depth)
 */
void
lp_build_unnormalized_coords(struct lp_build_sample_context *bld,
                             LLVMValueRef flt_size,
                             LLVMValueRef *s,
                             LLVMValueRef *t,
                             LLVMValueRef *r)
{
   const unsigned dims = bld->dims;
   LLVMValueRef width;
   LLVMValueRef height;
   LLVMValueRef depth;

   lp_build_extract_image_sizes(bld,
                                &bld->float_size_bld,
                                bld->coord_type,
                                flt_size,
                                &width,
                                &height,
                                &depth);

   /* s = s * width, t = t * height */
   *s = lp_build_mul(&bld->coord_bld, *s, width);
   if (dims >= 2) {
      *t = lp_build_mul(&bld->coord_bld, *t, height);
      if (dims >= 3) {
         *r = lp_build_mul(&bld->coord_bld, *r, depth);
      }
   }
}


/** Helper used by lp_build_cube_lookup() */
static LLVMValueRef
lp_build_cube_imapos(struct lp_build_context *coord_bld, LLVMValueRef coord)
{
   /* ima = +0.5 / abs(coord); */
   LLVMValueRef posHalf = lp_build_const_vec(coord_bld->gallivm, coord_bld->type, 0.5);
   LLVMValueRef absCoord = lp_build_abs(coord_bld, coord);
   LLVMValueRef ima = lp_build_div(coord_bld, posHalf, absCoord);
   return ima;
}

/** Helper used by lp_build_cube_lookup() */
static LLVMValueRef
lp_build_cube_imaneg(struct lp_build_context *coord_bld, LLVMValueRef coord)
{
   /* ima = -0.5 / abs(coord); */
   LLVMValueRef negHalf = lp_build_const_vec(coord_bld->gallivm, coord_bld->type, -0.5);
   LLVMValueRef absCoord = lp_build_abs(coord_bld, coord);
   LLVMValueRef ima = lp_build_div(coord_bld, negHalf, absCoord);
   return ima;
}

/**
 * Helper used by lp_build_cube_lookup()
 * FIXME: the sign here can also be 0.
 * Arithmetically this could definitely make a difference. Either
 * fix the comment or use other (simpler) sign function, not sure
 * which one it should be.
 * \param sign  scalar +1 or -1
 * \param coord  float vector
 * \param ima  float vector
 */
static LLVMValueRef
lp_build_cube_coord(struct lp_build_context *coord_bld,
                    LLVMValueRef sign, int negate_coord,
                    LLVMValueRef coord, LLVMValueRef ima)
{
   /* return negate(coord) * ima * sign + 0.5; */
   LLVMValueRef half = lp_build_const_vec(coord_bld->gallivm, coord_bld->type, 0.5);
   LLVMValueRef res;

   assert(negate_coord == +1 || negate_coord == -1);

   if (negate_coord == -1) {
      coord = lp_build_negate(coord_bld, coord);
   }

   res = lp_build_mul(coord_bld, coord, ima);
   if (sign) {
      sign = lp_build_broadcast_scalar(coord_bld, sign);
      res = lp_build_mul(coord_bld, res, sign);
   }
   res = lp_build_add(coord_bld, res, half);

   return res;
}


/** Helper used by lp_build_cube_lookup()
 * Return (major_coord >= 0) ? pos_face : neg_face;
 */
static LLVMValueRef
lp_build_cube_face(struct lp_build_sample_context *bld,
                   LLVMValueRef major_coord,
                   unsigned pos_face, unsigned neg_face)
{
   struct gallivm_state *gallivm = bld->gallivm;
   LLVMBuilderRef builder = gallivm->builder;
   LLVMValueRef cmp = LLVMBuildFCmp(builder, LLVMRealUGE,
                                    major_coord,
                                    bld->float_bld.zero, "");
   LLVMValueRef pos = lp_build_const_int32(gallivm, pos_face);
   LLVMValueRef neg = lp_build_const_int32(gallivm, neg_face);
   LLVMValueRef res = LLVMBuildSelect(builder, cmp, pos, neg, "");
   return res;
}



/**
 * Generate code to do cube face selection and compute per-face texcoords.
 */
void
lp_build_cube_lookup(struct lp_build_sample_context *bld,
                     LLVMValueRef s,
                     LLVMValueRef t,
                     LLVMValueRef r,
                     const struct lp_derivatives *derivs, /* optional */
                     LLVMValueRef *face,
                     LLVMValueRef *face_s,
                     LLVMValueRef *face_t,
                     LLVMValueRef *rho,
                     boolean need_derivs)
{
   struct lp_build_context *coord_bld = &bld->coord_bld;
   LLVMBuilderRef builder = bld->gallivm->builder;
   struct gallivm_state *gallivm = bld->gallivm;
   LLVMValueRef si, ti, ri;

   if (1 || coord_bld->type.length > 4) {
      /*
       * Do per-pixel face selection. We cannot however (as we used to do)
       * simply calculate the derivs afterwards (which is very bogus for
       * explicit derivs btw) because the values would be "random" when
       * not all pixels lie on the same face. So what we do here is just
       * calculate the derivatives after scaling the coords by the absolute
       * value of the inverse major axis, and essentially do rho calculation
       * steps as if it were a 3d texture. This is perfect if all pixels hit
       * the same face, but not so great at edges, I believe the max error
       * should be sqrt(2) with no_rho_approx or 2 otherwise (essentially measuring
       * the 3d distance between 2 points on the cube instead of measuring up/down
       * the edge). Still this is possibly a win over just selecting the same face
       * for all pixels. Unfortunately, something like that doesn't work for
       * explicit derivatives.
       * TODO: handle explicit derivatives by transforming them alongside coords
       * somehow.
       */
      struct lp_build_context *cint_bld = &bld->int_coord_bld;
      struct lp_type intctype = cint_bld->type;
      LLVMValueRef signs, signt, signr, signma;
      LLVMValueRef as, at, ar;
      LLVMValueRef as_ge_at, maxasat, ar_ge_as_at;
      LLVMValueRef snewx, tnewx, snewy, tnewy, snewz, tnewz;
      LLVMValueRef tnegi, rnegi;
      LLVMValueRef ma, mai, ima;
      LLVMValueRef posHalf = lp_build_const_vec(gallivm, coord_bld->type, 0.5);
      LLVMValueRef signmask = lp_build_const_int_vec(gallivm, intctype,
                                                     1 << (intctype.width - 1));
      LLVMValueRef signshift = lp_build_const_int_vec(gallivm, intctype,
                                                      intctype.width -1);
      LLVMValueRef facex = lp_build_const_int_vec(gallivm, intctype, PIPE_TEX_FACE_POS_X);
      LLVMValueRef facey = lp_build_const_int_vec(gallivm, intctype, PIPE_TEX_FACE_POS_Y);
      LLVMValueRef facez = lp_build_const_int_vec(gallivm, intctype, PIPE_TEX_FACE_POS_Z);

      assert(PIPE_TEX_FACE_NEG_X == PIPE_TEX_FACE_POS_X + 1);
      assert(PIPE_TEX_FACE_NEG_Y == PIPE_TEX_FACE_POS_Y + 1);
      assert(PIPE_TEX_FACE_NEG_Z == PIPE_TEX_FACE_POS_Z + 1);

      /*
       * get absolute value (for x/y/z face selection) and sign bit
       * (for mirroring minor coords and pos/neg face selection)
       * of the original coords.
       */
      as = lp_build_abs(&bld->coord_bld, s);
      at = lp_build_abs(&bld->coord_bld, t);
      ar = lp_build_abs(&bld->coord_bld, r);

      /*
       * major face determination: select x if x > y else select y
       * select z if z >= max(x,y) else select previous result
       * if some axis are the same we chose z over y, y over x - the
       * dx10 spec seems to ask for it while OpenGL doesn't care (if we
       * wouldn't care could save a select or two if using different
       * compares and doing at_g_as_ar last since tnewx and tnewz are the
       * same).
       */
      as_ge_at = lp_build_cmp(coord_bld, PIPE_FUNC_GREATER, as, at);
      maxasat = lp_build_max(coord_bld, as, at);
      ar_ge_as_at = lp_build_cmp(coord_bld, PIPE_FUNC_GEQUAL, ar, maxasat);

      if (need_derivs) {
         LLVMValueRef ddx_ddy[2], tmp[3], rho_vec;
         static const unsigned char swizzle0[] = { /* no-op swizzle */
            0, LP_BLD_SWIZZLE_DONTCARE,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };
         static const unsigned char swizzle1[] = {
            1, LP_BLD_SWIZZLE_DONTCARE,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };
         static const unsigned char swizzle01[] = { /* no-op swizzle */
            0, 1,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };
         static const unsigned char swizzle23[] = {
            2, 3,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };
         static const unsigned char swizzle02[] = {
            0, 2,
            LP_BLD_SWIZZLE_DONTCARE, LP_BLD_SWIZZLE_DONTCARE
         };

         /*
          * scale the s/t/r coords pre-select/mirror so we can calculate
          * "reasonable" derivs.
          */
         ma = lp_build_select(coord_bld, as_ge_at, s, t);
         ma = lp_build_select(coord_bld, ar_ge_as_at, r, ma);
         ima = lp_build_cube_imapos(coord_bld, ma);
         s = lp_build_mul(coord_bld, s, ima);
         t = lp_build_mul(coord_bld, t, ima);
         r = lp_build_mul(coord_bld, r, ima);

         /*
          * This isn't quite the same as the "ordinary" (3d deriv) path since we
          * know the texture is square which simplifies things (we can omit the
          * size mul which happens very early completely here and do it at the
          * very end).
          */
         ddx_ddy[0] = lp_build_packed_ddx_ddy_twocoord(coord_bld, s, t);
         ddx_ddy[1] = lp_build_packed_ddx_ddy_onecoord(coord_bld, r);

         if (gallivm_debug & GALLIVM_DEBUG_NO_RHO_APPROX) {
            ddx_ddy[0] = lp_build_mul(coord_bld, ddx_ddy[0], ddx_ddy[0]);
            ddx_ddy[1] = lp_build_mul(coord_bld, ddx_ddy[1], ddx_ddy[1]);
         }
         else {
            ddx_ddy[0] = lp_build_abs(coord_bld, ddx_ddy[0]);
            ddx_ddy[1] = lp_build_abs(coord_bld, ddx_ddy[1]);
         }

         tmp[0] = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle01);
         tmp[1] = lp_build_swizzle_aos(coord_bld, ddx_ddy[0], swizzle23);
         tmp[2] = lp_build_swizzle_aos(coord_bld, ddx_ddy[1], swizzle02);

         if (gallivm_debug & GALLIVM_DEBUG_NO_RHO_APPROX) {
            rho_vec = lp_build_add(coord_bld, tmp[0], tmp[1]);
            rho_vec = lp_build_add(coord_bld, rho_vec, tmp[2]);
         }
         else {
            rho_vec = lp_build_max(coord_bld, tmp[0], tmp[1]);
            rho_vec = lp_build_max(coord_bld, rho_vec, tmp[2]);
         }

         tmp[0] = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle0);
         tmp[1] = lp_build_swizzle_aos(coord_bld, rho_vec, swizzle1);
         *rho = lp_build_max(coord_bld, tmp[0], tmp[1]);
      }

      si = LLVMBuildBitCast(builder, s, lp_build_vec_type(gallivm, intctype), "");
      ti = LLVMBuildBitCast(builder, t, lp_build_vec_type(gallivm, intctype), "");
      ri = LLVMBuildBitCast(builder, r, lp_build_vec_type(gallivm, intctype), "");
      signs = LLVMBuildAnd(builder, si, signmask, "");
      signt = LLVMBuildAnd(builder, ti, signmask, "");
      signr = LLVMBuildAnd(builder, ri, signmask, "");

      /*
       * compute all possible new s/t coords
       * snewx = signs * -r;
       * tnewx = -t;
       * snewy = s;
       * tnewy = signt * r;
       * snewz = signr * s;
       * tnewz = -t;
       */
      tnegi = LLVMBuildXor(builder, ti, signmask, "");
      rnegi = LLVMBuildXor(builder, ri, signmask, "");

      snewx = LLVMBuildXor(builder, signs, rnegi, "");
      tnewx = tnegi;

      snewy = si;
      tnewy = LLVMBuildXor(builder, signt, ri, "");

      snewz = LLVMBuildXor(builder, signr, si, "");
      tnewz = tnegi;

      /* XXX on x86 unclear if we should cast the values back to float
       * or not - on some cpus (nehalem) pblendvb has twice the throughput
       * of blendvps though on others there just might be domain
       * transition penalties when using it (this depends on what llvm
       * will chose for the bit ops above so there appears no "right way",
       * but given the boatload of selects let's just use the int type).
       */

      /* select/mirror */
      if (!need_derivs) {
         ma = lp_build_select(coord_bld, as_ge_at, s, t);
      }
      *face_s = lp_build_select(cint_bld, as_ge_at, snewx, snewy);
      *face_t = lp_build_select(cint_bld, as_ge_at, tnewx, tnewy);
      *face = lp_build_select(cint_bld, as_ge_at, facex, facey);

      if (!need_derivs) {
         ma = lp_build_select(coord_bld, ar_ge_as_at, r, ma);
      }
      *face_s = lp_build_select(cint_bld, ar_ge_as_at, snewz, *face_s);
      *face_t = lp_build_select(cint_bld, ar_ge_as_at, tnewz, *face_t);
      *face = lp_build_select(cint_bld, ar_ge_as_at, facez, *face);

      *face_s = LLVMBuildBitCast(builder, *face_s,
                               lp_build_vec_type(gallivm, coord_bld->type), "");
      *face_t = LLVMBuildBitCast(builder, *face_t,
                               lp_build_vec_type(gallivm, coord_bld->type), "");

      /* add +1 for neg face */
      /* XXX with AVX probably want to use another select here -
       * as long as we ensure vblendvps gets used we can actually
       * skip the comparison and just use sign as a "mask" directly.
       */
      mai = LLVMBuildBitCast(builder, ma, lp_build_vec_type(gallivm, intctype), "");
      signma = LLVMBuildLShr(builder, mai, signshift, "");
      *face = LLVMBuildOr(builder, *face, signma, "face");

      /* project coords */
      if (!need_derivs) {
         ima = lp_build_cube_imapos(coord_bld, ma);
         *face_s = lp_build_mul(coord_bld, *face_s, ima);
         *face_t = lp_build_mul(coord_bld, *face_t, ima);
      }

      *face_s = lp_build_add(coord_bld, *face_s, posHalf);
      *face_t = lp_build_add(coord_bld, *face_t, posHalf);
   }

   else {
      struct lp_build_if_state if_ctx;
      LLVMValueRef face_s_var;
      LLVMValueRef face_t_var;
      LLVMValueRef face_var;
      LLVMValueRef arx_ge_ary_arz, ary_ge_arx_arz;
      LLVMValueRef shuffles[4];
      LLVMValueRef arxy_ge_aryx, arxy_ge_arzz, arxy_ge_arxy_arzz;
      LLVMValueRef arxyxy, aryxzz, arxyxy_ge_aryxzz;
      LLVMValueRef tmp[4], rxyz, arxyz;
      struct lp_build_context *float_bld = &bld->float_bld;

      assert(bld->coord_bld.type.length == 4);

      tmp[0] = s;
      tmp[1] = t;
      tmp[2] = r;
      rxyz = lp_build_hadd_partial4(&bld->coord_bld, tmp, 3);
      arxyz = lp_build_abs(&bld->coord_bld, rxyz);

      shuffles[0] = lp_build_const_int32(gallivm, 0);
      shuffles[1] = lp_build_const_int32(gallivm, 1);
      shuffles[2] = lp_build_const_int32(gallivm, 0);
      shuffles[3] = lp_build_const_int32(gallivm, 1);
      arxyxy = LLVMBuildShuffleVector(builder, arxyz, arxyz, LLVMConstVector(shuffles, 4), "");
      shuffles[0] = lp_build_const_int32(gallivm, 1);
      shuffles[1] = lp_build_const_int32(gallivm, 0);
      shuffles[2] = lp_build_const_int32(gallivm, 2);
      shuffles[3] = lp_build_const_int32(gallivm, 2);
      aryxzz = LLVMBuildShuffleVector(builder, arxyz, arxyz, LLVMConstVector(shuffles, 4), "");
      arxyxy_ge_aryxzz = lp_build_cmp(&bld->coord_bld, PIPE_FUNC_GEQUAL, arxyxy, aryxzz);

      shuffles[0] = lp_build_const_int32(gallivm, 0);
      shuffles[1] = lp_build_const_int32(gallivm, 1);
      arxy_ge_aryx = LLVMBuildShuffleVector(builder, arxyxy_ge_aryxzz, arxyxy_ge_aryxzz,
                                            LLVMConstVector(shuffles, 2), "");
      shuffles[0] = lp_build_const_int32(gallivm, 2);
      shuffles[1] = lp_build_const_int32(gallivm, 3);
      arxy_ge_arzz = LLVMBuildShuffleVector(builder, arxyxy_ge_aryxzz, arxyxy_ge_aryxzz,
                                            LLVMConstVector(shuffles, 2), "");
      arxy_ge_arxy_arzz = LLVMBuildAnd(builder, arxy_ge_aryx, arxy_ge_arzz, "");

      arx_ge_ary_arz = LLVMBuildExtractElement(builder, arxy_ge_arxy_arzz,
                                               lp_build_const_int32(gallivm, 0), "");
      arx_ge_ary_arz = LLVMBuildICmp(builder, LLVMIntNE, arx_ge_ary_arz,
                                               lp_build_const_int32(gallivm, 0), "");
      ary_ge_arx_arz = LLVMBuildExtractElement(builder, arxy_ge_arxy_arzz,
                                               lp_build_const_int32(gallivm, 1), "");
      ary_ge_arx_arz = LLVMBuildICmp(builder, LLVMIntNE, ary_ge_arx_arz,
                                               lp_build_const_int32(gallivm, 0), "");
      face_s_var = lp_build_alloca(gallivm, bld->coord_bld.vec_type, "face_s_var");
      face_t_var = lp_build_alloca(gallivm, bld->coord_bld.vec_type, "face_t_var");
      face_var = lp_build_alloca(gallivm, bld->int_bld.vec_type, "face_var");

      lp_build_if(&if_ctx, gallivm, arx_ge_ary_arz);
      {
         /* +/- X face */
         LLVMValueRef sign, ima;
         si = LLVMBuildExtractElement(builder, rxyz,
                                      lp_build_const_int32(gallivm, 0), "");
         /* +/- X face */
         sign = lp_build_sgn(float_bld, si);
         ima = lp_build_cube_imaneg(coord_bld, s);
         *face_s = lp_build_cube_coord(coord_bld, sign, +1, r, ima);
         *face_t = lp_build_cube_coord(coord_bld, NULL, +1, t, ima);
         *face = lp_build_cube_face(bld, si,
                                    PIPE_TEX_FACE_POS_X,
                                    PIPE_TEX_FACE_NEG_X);
         LLVMBuildStore(builder, *face_s, face_s_var);
         LLVMBuildStore(builder, *face_t, face_t_var);
         LLVMBuildStore(builder, *face, face_var);
      }
      lp_build_else(&if_ctx);
      {
         struct lp_build_if_state if_ctx2;

         lp_build_if(&if_ctx2, gallivm, ary_ge_arx_arz);
         {
            LLVMValueRef sign, ima;
            /* +/- Y face */
            ti = LLVMBuildExtractElement(builder, rxyz,
                                         lp_build_const_int32(gallivm, 1), "");
            sign = lp_build_sgn(float_bld, ti);
            ima = lp_build_cube_imaneg(coord_bld, t);
            *face_s = lp_build_cube_coord(coord_bld, NULL, -1, s, ima);
            *face_t = lp_build_cube_coord(coord_bld, sign, -1, r, ima);
            *face = lp_build_cube_face(bld, ti,
                                       PIPE_TEX_FACE_POS_Y,
                                       PIPE_TEX_FACE_NEG_Y);
            LLVMBuildStore(builder, *face_s, face_s_var);
            LLVMBuildStore(builder, *face_t, face_t_var);
            LLVMBuildStore(builder, *face, face_var);
         }
         lp_build_else(&if_ctx2);
         {
            /* +/- Z face */
            LLVMValueRef sign, ima;
            ri = LLVMBuildExtractElement(builder, rxyz,
                                         lp_build_const_int32(gallivm, 2), "");
            sign = lp_build_sgn(float_bld, ri);
            ima = lp_build_cube_imaneg(coord_bld, r);
            *face_s = lp_build_cube_coord(coord_bld, sign, -1, s, ima);
            *face_t = lp_build_cube_coord(coord_bld, NULL, +1, t, ima);
            *face = lp_build_cube_face(bld, ri,
                                       PIPE_TEX_FACE_POS_Z,
                                       PIPE_TEX_FACE_NEG_Z);
            LLVMBuildStore(builder, *face_s, face_s_var);
            LLVMBuildStore(builder, *face_t, face_t_var);
            LLVMBuildStore(builder, *face, face_var);
         }
         lp_build_endif(&if_ctx2);
      }

      lp_build_endif(&if_ctx);

      *face_s = LLVMBuildLoad(builder, face_s_var, "face_s");
      *face_t = LLVMBuildLoad(builder, face_t_var, "face_t");
      *face   = LLVMBuildLoad(builder, face_var, "face");
      *face   = lp_build_broadcast_scalar(&bld->int_coord_bld, *face);
   }
}


/**
 * Compute the partial offset of a pixel block along an arbitrary axis.
 *
 * @param coord   coordinate in pixels
 * @param stride  number of bytes between rows of successive pixel blocks
 * @param block_length  number of pixels in a pixels block along the coordinate
 *                      axis
 * @param out_offset    resulting relative offset of the pixel block in bytes
 * @param out_subcoord  resulting sub-block pixel coordinate
 */
void
lp_build_sample_partial_offset(struct lp_build_context *bld,
                               unsigned block_length,
                               LLVMValueRef coord,
                               LLVMValueRef stride,
                               LLVMValueRef *out_offset,
                               LLVMValueRef *out_subcoord)
{
   LLVMBuilderRef builder = bld->gallivm->builder;
   LLVMValueRef offset;
   LLVMValueRef subcoord;

   if (block_length == 1) {
      subcoord = bld->zero;
   }
   else {
      /*
       * Pixel blocks have power of two dimensions. LLVM should convert the
       * rem/div to bit arithmetic.
       * TODO: Verify this.
       * It does indeed BUT it does transform it to scalar (and back) when doing so
       * (using roughly extract, shift/and, mov, unpack) (llvm 2.7).
       * The generated code looks seriously unfunny and is quite expensive.
       */
#if 0
      LLVMValueRef block_width = lp_build_const_int_vec(bld->type, block_length);
      subcoord = LLVMBuildURem(builder, coord, block_width, "");
      coord    = LLVMBuildUDiv(builder, coord, block_width, "");
#else
      unsigned logbase2 = util_logbase2(block_length);
      LLVMValueRef block_shift = lp_build_const_int_vec(bld->gallivm, bld->type, logbase2);
      LLVMValueRef block_mask = lp_build_const_int_vec(bld->gallivm, bld->type, block_length - 1);
      subcoord = LLVMBuildAnd(builder, coord, block_mask, "");
      coord = LLVMBuildLShr(builder, coord, block_shift, "");
#endif
   }

   offset = lp_build_mul(bld, coord, stride);

   assert(out_offset);
   assert(out_subcoord);

   *out_offset = offset;
   *out_subcoord = subcoord;
}


/**
 * Compute the offset of a pixel block.
 *
 * x, y, z, y_stride, z_stride are vectors, and they refer to pixels.
 *
 * Returns the relative offset and i,j sub-block coordinates
 */
void
lp_build_sample_offset(struct lp_build_context *bld,
                       const struct util_format_description *format_desc,
                       LLVMValueRef x,
                       LLVMValueRef y,
                       LLVMValueRef z,
                       LLVMValueRef y_stride,
                       LLVMValueRef z_stride,
                       LLVMValueRef *out_offset,
                       LLVMValueRef *out_i,
                       LLVMValueRef *out_j)
{
   LLVMValueRef x_stride;
   LLVMValueRef offset;

   x_stride = lp_build_const_vec(bld->gallivm, bld->type,
                                 format_desc->block.bits/8);

   lp_build_sample_partial_offset(bld,
                                  format_desc->block.width,
                                  x, x_stride,
                                  &offset, out_i);

   if (y && y_stride) {
      LLVMValueRef y_offset;
      lp_build_sample_partial_offset(bld,
                                     format_desc->block.height,
                                     y, y_stride,
                                     &y_offset, out_j);
      offset = lp_build_add(bld, offset, y_offset);
   }
   else {
      *out_j = bld->zero;
   }

   if (z && z_stride) {
      LLVMValueRef z_offset;
      LLVMValueRef k;
      lp_build_sample_partial_offset(bld,
                                     1, /* pixel blocks are always 2D */
                                     z, z_stride,
                                     &z_offset, &k);
      offset = lp_build_add(bld, offset, z_offset);
   }

   *out_offset = offset;
}
