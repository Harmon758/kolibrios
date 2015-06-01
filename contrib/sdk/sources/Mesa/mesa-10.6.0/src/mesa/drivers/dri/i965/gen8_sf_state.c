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

#include "brw_context.h"
#include "brw_state.h"
#include "brw_defines.h"
#include "brw_util.h"
#include "main/macros.h"
#include "main/fbobject.h"
#include "intel_batchbuffer.h"

static void
upload_sbe(struct brw_context *brw)
{
   struct gl_context *ctx = &brw->ctx;
   /* BRW_NEW_FS_PROG_DATA */
   uint32_t num_outputs = brw->wm.prog_data->num_varying_inputs;
   uint16_t attr_overrides[VARYING_SLOT_MAX];
   uint32_t urb_entry_read_length;
   uint32_t point_sprite_enables;
   uint32_t flat_enables;
   int sbe_cmd_length;

   uint32_t dw1 =
      GEN7_SBE_SWIZZLE_ENABLE |
      num_outputs << GEN7_SBE_NUM_OUTPUTS_SHIFT;
   uint32_t dw4 = 0;
   uint32_t dw5 = 0;

   /* _NEW_BUFFERS */
   bool render_to_fbo = _mesa_is_user_fbo(ctx->DrawBuffer);

   /* _NEW_POINT
    *
    * Window coordinates in an FBO are inverted, which means point
    * sprite origin must be inverted.
    */
   if ((ctx->Point.SpriteOrigin == GL_LOWER_LEFT) != render_to_fbo)
      dw1 |= GEN6_SF_POINT_SPRITE_LOWERLEFT;
   else
      dw1 |= GEN6_SF_POINT_SPRITE_UPPERLEFT;

   /* BRW_NEW_VUE_MAP_GEOM_OUT | BRW_NEW_FRAGMENT_PROGRAM |
    * _NEW_POINT | _NEW_LIGHT | _NEW_PROGRAM | BRW_NEW_FS_PROG_DATA
    */
   calculate_attr_overrides(brw, attr_overrides,
                            &point_sprite_enables,
                            &flat_enables,
                            &urb_entry_read_length);

   /* Typically, the URB entry read length and offset should be programmed in
    * 3DSTATE_VS and 3DSTATE_GS; SBE inherits it from the last active stage
    * which produces geometry.  However, we don't know the proper value until
    * we call calculate_attr_overrides().
    *
    * To fit with our existing code, we override the inherited values and
    * specify it here directly, as we did on previous generations.
    */
   dw1 |=
      urb_entry_read_length << GEN7_SBE_URB_ENTRY_READ_LENGTH_SHIFT |
      BRW_SF_URB_ENTRY_READ_OFFSET << GEN8_SBE_URB_ENTRY_READ_OFFSET_SHIFT |
      GEN8_SBE_FORCE_URB_ENTRY_READ_LENGTH |
      GEN8_SBE_FORCE_URB_ENTRY_READ_OFFSET;

   if (brw->gen == 8) {
      sbe_cmd_length = 4;
   } else {
      sbe_cmd_length = 6;

      /* prepare the active component dwords */
      int input_index = 0;
      for (int attr = 0; attr < VARYING_SLOT_MAX; attr++) {
         if (!(brw->fragment_program->Base.InputsRead & BITFIELD64_BIT(attr)))
            continue;

         assert(input_index < 32);

         if (input_index < 16)
            dw4 |= (GEN9_SBE_ACTIVE_COMPONENT_XYZW << (input_index << 1));
         else
            dw5 |= (GEN9_SBE_ACTIVE_COMPONENT_XYZW << ((input_index - 16) << 1));

         ++input_index;
      }
   }
   BEGIN_BATCH(sbe_cmd_length);
   OUT_BATCH(_3DSTATE_SBE << 16 | (sbe_cmd_length - 2));
   OUT_BATCH(dw1);
   OUT_BATCH(point_sprite_enables);
   OUT_BATCH(flat_enables);
   if (sbe_cmd_length >= 6) {
      OUT_BATCH(dw4);
      OUT_BATCH(dw5);
   }
   ADVANCE_BATCH();

   BEGIN_BATCH(11);
   OUT_BATCH(_3DSTATE_SBE_SWIZ << 16 | (11 - 2));

   /* Output DWords 1 through 8: */
   for (int i = 0; i < 8; i++) {
      OUT_BATCH(attr_overrides[i * 2] | attr_overrides[i * 2 + 1] << 16);
   }

   OUT_BATCH(0); /* wrapshortest enables 0-7 */
   OUT_BATCH(0); /* wrapshortest enables 8-15 */
   ADVANCE_BATCH();
}

const struct brw_tracked_state gen8_sbe_state = {
   .dirty = {
      .mesa  = _NEW_BUFFERS |
               _NEW_LIGHT |
               _NEW_POINT |
               _NEW_PROGRAM,
      .brw   = BRW_NEW_CONTEXT |
               BRW_NEW_FRAGMENT_PROGRAM |
               BRW_NEW_FS_PROG_DATA |
               BRW_NEW_VUE_MAP_GEOM_OUT,
   },
   .emit = upload_sbe,
};

static void
upload_sf(struct brw_context *brw)
{
   struct gl_context *ctx = &brw->ctx;
   uint32_t dw1 = 0, dw2 = 0, dw3 = 0;
   float point_size;

   dw1 = GEN6_SF_STATISTICS_ENABLE;

   if (brw->sf.viewport_transform_enable)
       dw1 |= GEN6_SF_VIEWPORT_TRANSFORM_ENABLE;

   /* _NEW_LINE */
   /* OpenGL dictates that line width should be rounded to the nearest
    * integer
    */
   float line_width =
      roundf(CLAMP(ctx->Line.Width, 0.0, ctx->Const.MaxLineWidth));
   uint32_t line_width_u3_7 = U_FIXED(line_width, 7);
   if (line_width_u3_7 == 0)
      line_width_u3_7 = 1;
   if (brw->gen >= 9 || brw->is_cherryview) {
      dw1 |= line_width_u3_7 << GEN9_SF_LINE_WIDTH_SHIFT;
   } else {
      dw2 |= line_width_u3_7 << GEN6_SF_LINE_WIDTH_SHIFT;
   }

   if (ctx->Line.SmoothFlag) {
      dw2 |= GEN6_SF_LINE_END_CAP_WIDTH_1_0;
   }

   /* Clamp to ARB_point_parameters user limits */
   point_size = CLAMP(ctx->Point.Size, ctx->Point.MinSize, ctx->Point.MaxSize);

   /* Clamp to the hardware limits and convert to fixed point */
   dw3 |= U_FIXED(CLAMP(point_size, 0.125, 255.875), 3);

   /* _NEW_PROGRAM | _NEW_POINT */
   if (!(ctx->VertexProgram.PointSizeEnabled || ctx->Point._Attenuated))
      dw3 |= GEN6_SF_USE_STATE_POINT_WIDTH;

   /* _NEW_POINT | _NEW_MULTISAMPLE */
   if ((ctx->Point.SmoothFlag || ctx->Multisample._Enabled) &&
       !ctx->Point.PointSprite) {
      dw3 |= GEN8_SF_SMOOTH_POINT_ENABLE;
   }

   dw3 |= GEN6_SF_LINE_AA_MODE_TRUE;

   /* _NEW_LIGHT */
   if (ctx->Light.ProvokingVertex != GL_FIRST_VERTEX_CONVENTION) {
      dw3 |= (2 << GEN6_SF_TRI_PROVOKE_SHIFT) |
             (2 << GEN6_SF_TRIFAN_PROVOKE_SHIFT) |
             (1 << GEN6_SF_LINE_PROVOKE_SHIFT);
   } else {
      dw3 |= (1 << GEN6_SF_TRIFAN_PROVOKE_SHIFT);
   }

   BEGIN_BATCH(4);
   OUT_BATCH(_3DSTATE_SF << 16 | (4 - 2));
   OUT_BATCH(dw1);
   OUT_BATCH(dw2);
   OUT_BATCH(dw3);
   ADVANCE_BATCH();
}

const struct brw_tracked_state gen8_sf_state = {
   .dirty = {
      .mesa  = _NEW_LIGHT |
               _NEW_PROGRAM |
               _NEW_LINE |
               _NEW_MULTISAMPLE |
               _NEW_POINT,
      .brw   = BRW_NEW_CONTEXT,
   },
   .emit = upload_sf,
};

static void
upload_raster(struct brw_context *brw)
{
   struct gl_context *ctx = &brw->ctx;
   uint32_t dw1 = 0;

   /* _NEW_BUFFERS */
   bool render_to_fbo = _mesa_is_user_fbo(brw->ctx.DrawBuffer);

   /* _NEW_POLYGON */
   if (ctx->Polygon._FrontBit == render_to_fbo)
      dw1 |= GEN8_RASTER_FRONT_WINDING_CCW;

   if (ctx->Polygon.CullFlag) {
      switch (ctx->Polygon.CullFaceMode) {
      case GL_FRONT:
         dw1 |= GEN8_RASTER_CULL_FRONT;
         break;
      case GL_BACK:
         dw1 |= GEN8_RASTER_CULL_BACK;
         break;
      case GL_FRONT_AND_BACK:
         dw1 |= GEN8_RASTER_CULL_BOTH;
         break;
      default:
         unreachable("not reached");
      }
   } else {
      dw1 |= GEN8_RASTER_CULL_NONE;
   }

   /* _NEW_POINT */
   if (ctx->Point.SmoothFlag)
      dw1 |= GEN8_RASTER_SMOOTH_POINT_ENABLE;

   if (ctx->Multisample._Enabled)
      dw1 |= GEN8_RASTER_API_MULTISAMPLE_ENABLE;

   if (ctx->Polygon.OffsetFill)
      dw1 |= GEN6_SF_GLOBAL_DEPTH_OFFSET_SOLID;

   if (ctx->Polygon.OffsetLine)
      dw1 |= GEN6_SF_GLOBAL_DEPTH_OFFSET_WIREFRAME;

   if (ctx->Polygon.OffsetPoint)
      dw1 |= GEN6_SF_GLOBAL_DEPTH_OFFSET_POINT;

   switch (ctx->Polygon.FrontMode) {
   case GL_FILL:
      dw1 |= GEN6_SF_FRONT_SOLID;
      break;
   case GL_LINE:
      dw1 |= GEN6_SF_FRONT_WIREFRAME;
      break;
   case GL_POINT:
      dw1 |= GEN6_SF_FRONT_POINT;
      break;

   default:
      unreachable("not reached");
   }

   switch (ctx->Polygon.BackMode) {
   case GL_FILL:
      dw1 |= GEN6_SF_BACK_SOLID;
      break;
   case GL_LINE:
      dw1 |= GEN6_SF_BACK_WIREFRAME;
      break;
   case GL_POINT:
      dw1 |= GEN6_SF_BACK_POINT;
      break;
   default:
      unreachable("not reached");
   }

   /* _NEW_LINE */
   if (ctx->Line.SmoothFlag)
      dw1 |= GEN8_RASTER_LINE_AA_ENABLE;

   /* _NEW_SCISSOR */
   if (ctx->Scissor.EnableFlags)
      dw1 |= GEN8_RASTER_SCISSOR_ENABLE;

   /* _NEW_TRANSFORM */
   if (!ctx->Transform.DepthClamp) {
      if (brw->gen >= 9) {
         dw1 |= GEN9_RASTER_VIEWPORT_Z_NEAR_CLIP_TEST_ENABLE |
                GEN9_RASTER_VIEWPORT_Z_FAR_CLIP_TEST_ENABLE;
      } else {
         dw1 |= GEN8_RASTER_VIEWPORT_Z_CLIP_TEST_ENABLE;
      }
   }

   BEGIN_BATCH(5);
   OUT_BATCH(_3DSTATE_RASTER << 16 | (5 - 2));
   OUT_BATCH(dw1);
   OUT_BATCH_F(ctx->Polygon.OffsetUnits * 2); /* constant.  copied from gen4 */
   OUT_BATCH_F(ctx->Polygon.OffsetFactor); /* scale */
   OUT_BATCH_F(ctx->Polygon.OffsetClamp); /* global depth offset clamp */
   ADVANCE_BATCH();
}

const struct brw_tracked_state gen8_raster_state = {
   .dirty = {
      .mesa  = _NEW_BUFFERS |
               _NEW_LINE |
               _NEW_MULTISAMPLE |
               _NEW_POINT |
               _NEW_POLYGON |
               _NEW_SCISSOR |
               _NEW_TRANSFORM,
      .brw   = BRW_NEW_CONTEXT,
   },
   .emit = upload_raster,
};
