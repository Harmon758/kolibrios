/*
 * Copyright © 2010 Intel Corporation
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
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Xiang Haihao <haihao.xiang@intel.com>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef HAVE_GEN_AVC_SURFACE
#define HAVE_GEN_AVC_SURFACE 1
#endif

#include "intel_batchbuffer.h"
#include "intel_driver.h"

#include "i965_defines.h"
#include "i965_drv_video.h"
#include "i965_avc_bsd.h"
#include "i965_media_h264.h"
#include "i965_media.h"
#include "i965_decoder_utils.h"

static void
i965_avc_bsd_init_avc_bsd_surface(VADriverContextP ctx, 
                                  struct object_surface *obj_surface,
                                  VAPictureParameterBufferH264 *pic_param,
                                  struct i965_h264_context *i965_h264_context)
{
    struct i965_driver_data *i965 = i965_driver_data(ctx);
    GenAvcSurface *avc_bsd_surface = obj_surface->private_data;

    obj_surface->free_private_data = gen_free_avc_surface;

    if (!avc_bsd_surface) {
        avc_bsd_surface = calloc(sizeof(GenAvcSurface), 1);
        assert((obj_surface->size & 0x3f) == 0);
        obj_surface->private_data = avc_bsd_surface;
    }

    avc_bsd_surface->dmv_bottom_flag = (pic_param->pic_fields.bits.field_pic_flag &&
                                        !pic_param->seq_fields.bits.direct_8x8_inference_flag);

    if (avc_bsd_surface->dmv_top == NULL) {
        avc_bsd_surface->dmv_top = dri_bo_alloc(i965->intel.bufmgr,
                                                "direct mv w/r buffer",
                                                DMV_SIZE,
                                                0x1000);
    }

    if (avc_bsd_surface->dmv_bottom_flag &&
        avc_bsd_surface->dmv_bottom == NULL) {
        avc_bsd_surface->dmv_bottom = dri_bo_alloc(i965->intel.bufmgr,
                                                   "direct mv w/r buffer",
                                                   DMV_SIZE,
                                                   0x1000);
    }
}

static void
i965_bsd_ind_obj_base_address(VADriverContextP ctx,
                              struct decode_state *decode_state,
                              int slice,
                              struct i965_h264_context *i965_h264_context)
                              
{
    struct intel_batchbuffer *batch = i965_h264_context->batch;

    dri_bo *ind_bo = decode_state->slice_datas[slice]->bo;

    BEGIN_BCS_BATCH(batch, 3);
    OUT_BCS_BATCH(batch, CMD_BSD_IND_OBJ_BASE_ADDR | (3 - 2));
    OUT_BCS_RELOC(batch, ind_bo,
                  I915_GEM_DOMAIN_INSTRUCTION, 0,
                  0);
    OUT_BCS_BATCH(batch, 0);
    ADVANCE_BCS_BATCH(batch);
}

static void
i965_avc_bsd_img_state(VADriverContextP ctx,
                       struct decode_state *decode_state,
                       struct i965_h264_context *i965_h264_context)
{
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    int qm_present_flag;
    int img_struct;
    int mbaff_frame_flag;
    unsigned int avc_it_command_header;
    unsigned int width_in_mbs, height_in_mbs;
    VAPictureParameterBufferH264 *pic_param;

    if (decode_state->iq_matrix && decode_state->iq_matrix->buffer)
        qm_present_flag = 1;
    else
        qm_present_flag = 0; /* built-in QM matrices */

    assert(decode_state->pic_param && decode_state->pic_param->buffer);
    pic_param = (VAPictureParameterBufferH264 *)decode_state->pic_param->buffer;

    assert(!(pic_param->CurrPic.flags & VA_PICTURE_H264_INVALID));

    if (pic_param->CurrPic.flags & VA_PICTURE_H264_TOP_FIELD)
        img_struct = 1;
    else if (pic_param->CurrPic.flags & VA_PICTURE_H264_BOTTOM_FIELD)
        img_struct = 3;
    else
        img_struct = 0;

    if ((img_struct & 0x1) == 0x1) {
        assert(pic_param->pic_fields.bits.field_pic_flag == 0x1);
    } else {
        assert(pic_param->pic_fields.bits.field_pic_flag == 0x0);
    }

    if (pic_param->seq_fields.bits.frame_mbs_only_flag) { /* a frame containing only frame macroblocks */
        assert(pic_param->seq_fields.bits.mb_adaptive_frame_field_flag == 0);
        assert(pic_param->pic_fields.bits.field_pic_flag == 0);
    } else {
        assert(pic_param->seq_fields.bits.direct_8x8_inference_flag == 1); /* see H.264 spec */
    }

    mbaff_frame_flag = (pic_param->seq_fields.bits.mb_adaptive_frame_field_flag &&
                        !pic_param->pic_fields.bits.field_pic_flag);

    width_in_mbs = ((pic_param->picture_width_in_mbs_minus1 + 1) & 0xff);
    height_in_mbs = ((pic_param->picture_height_in_mbs_minus1 + 1) & 0xff); /* frame height */
                                                                               
    assert(!((width_in_mbs * height_in_mbs) & 0x8000)); /* hardware requirement */

    /* BSD unit doesn't support 4:2:2 and 4:4:4 picture */
    assert(pic_param->seq_fields.bits.chroma_format_idc == 0 || /* monochrome picture */
           pic_param->seq_fields.bits.chroma_format_idc == 1);  /* 4:2:0 */
    assert(pic_param->seq_fields.bits.residual_colour_transform_flag == 0); /* only available for 4:4:4 */

    avc_it_command_header = (CMD_MEDIA_OBJECT_EX | (12 - 2));

    BEGIN_BCS_BATCH(batch, 6);
    OUT_BCS_BATCH(batch, CMD_AVC_BSD_IMG_STATE | (6 - 2));
    OUT_BCS_BATCH(batch, 
                  ((width_in_mbs * height_in_mbs) & 0x7fff));
    OUT_BCS_BATCH(batch, 
                  (height_in_mbs << 16) | 
                  (width_in_mbs << 0));
    OUT_BCS_BATCH(batch, 
                  ((pic_param->second_chroma_qp_index_offset & 0x1f) << 24) |
                  ((pic_param->chroma_qp_index_offset & 0x1f) << 16) | 
                  (SCAN_RASTER_ORDER << 15) | /* AVC ILDB Data */
                  (SCAN_SPECIAL_ORDER << 14) | /* AVC IT Command */
                  (SCAN_RASTER_ORDER << 13) | /* AVC IT Data */
                  (1 << 12) | /* always 1, hardware requirement */
                  (qm_present_flag << 10) |
                  (img_struct << 8) |
                  (16 << 0)); /* FIXME: always support 16 reference frames ??? */
    OUT_BCS_BATCH(batch,
                  (RESIDUAL_DATA_OFFSET << 24) | /* residual data offset */
                  (0 << 17) | /* don't overwrite SRT */
                  (0 << 16) | /* Un-SRT (Unsynchronized Root Thread) */
                  (0 << 12) | /* FIXME: no 16MV ??? */
                  (pic_param->seq_fields.bits.chroma_format_idc << 10) |
                  (i965_h264_context->enable_avc_ildb << 8)  | /* Enable ILDB writing output */
                  (pic_param->pic_fields.bits.entropy_coding_mode_flag << 7) |
                  ((!pic_param->pic_fields.bits.reference_pic_flag) << 6) |
                  (pic_param->pic_fields.bits.constrained_intra_pred_flag << 5) |
                  (pic_param->seq_fields.bits.direct_8x8_inference_flag << 4) |
                  (pic_param->pic_fields.bits.transform_8x8_mode_flag << 3) |
                  (pic_param->seq_fields.bits.frame_mbs_only_flag << 2) |
                  (mbaff_frame_flag << 1) |
                  (pic_param->pic_fields.bits.field_pic_flag << 0));
    OUT_BCS_BATCH(batch, avc_it_command_header);
    ADVANCE_BCS_BATCH(batch);
}

static void
i965_avc_bsd_qm_state(VADriverContextP ctx,
                      struct decode_state *decode_state,
                      struct i965_h264_context *i965_h264_context)
{
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    int cmd_len;
    VAIQMatrixBufferH264 *iq_matrix;
    VAPictureParameterBufferH264 *pic_param;

    if (!decode_state->iq_matrix || !decode_state->iq_matrix->buffer)
        return;

    iq_matrix = (VAIQMatrixBufferH264 *)decode_state->iq_matrix->buffer;

    assert(decode_state->pic_param && decode_state->pic_param->buffer);
    pic_param = (VAPictureParameterBufferH264 *)decode_state->pic_param->buffer;

    cmd_len = 2 + 6 * 4; /* always load six 4x4 scaling matrices */

    if (pic_param->pic_fields.bits.transform_8x8_mode_flag)
        cmd_len += 2 * 16; /* load two 8x8 scaling matrices */

    BEGIN_BCS_BATCH(batch, cmd_len);
    OUT_BCS_BATCH(batch, CMD_AVC_BSD_QM_STATE | (cmd_len - 2));

    if (pic_param->pic_fields.bits.transform_8x8_mode_flag)
        OUT_BCS_BATCH(batch, 
                      (0x0  << 8) | /* don't use default built-in matrices */
                      (0xff << 0)); /* six 4x4 and two 8x8 scaling matrices */
    else
        OUT_BCS_BATCH(batch, 
                      (0x0  << 8) | /* don't use default built-in matrices */
                      (0x3f << 0)); /* six 4x4 scaling matrices */

    intel_batchbuffer_data(batch, &iq_matrix->ScalingList4x4[0][0], 6 * 4 * 4);

    if (pic_param->pic_fields.bits.transform_8x8_mode_flag)
        intel_batchbuffer_data(batch, &iq_matrix->ScalingList8x8[0][0], 2 * 16 * 4);

    ADVANCE_BCS_BATCH(batch);
}

static void
i965_avc_bsd_slice_state(VADriverContextP ctx, 
                         VAPictureParameterBufferH264 *pic_param, 
                         VASliceParameterBufferH264 *slice_param,
                         struct i965_h264_context *i965_h264_context)
{
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    int present_flag, cmd_len, list, j;
    uint8_t ref_idx_state[32];
    char weightoffsets[32 * 6];

    /* don't issue SLICE_STATE for intra-prediction decoding */
    if (slice_param->slice_type == SLICE_TYPE_I ||
        slice_param->slice_type == SLICE_TYPE_SI)
        return;

    cmd_len = 2;

    if (slice_param->slice_type == SLICE_TYPE_P ||
        slice_param->slice_type == SLICE_TYPE_SP) {
        present_flag = PRESENT_REF_LIST0;
        cmd_len += 8;
    } else { 
        present_flag = PRESENT_REF_LIST0 | PRESENT_REF_LIST1;
        cmd_len += 16;
    }

    if ((slice_param->slice_type == SLICE_TYPE_P ||
         slice_param->slice_type == SLICE_TYPE_SP) && 
        (pic_param->pic_fields.bits.weighted_pred_flag == 1)) {
        present_flag |= PRESENT_WEIGHT_OFFSET_L0;
        cmd_len += 48;
    }

    if ((slice_param->slice_type == SLICE_TYPE_B) &&
        (pic_param->pic_fields.bits.weighted_bipred_idc == 1)) {
        present_flag |= PRESENT_WEIGHT_OFFSET_L0 | PRESENT_WEIGHT_OFFSET_L1;
        cmd_len += 96;
    }

    BEGIN_BCS_BATCH(batch, cmd_len);
    OUT_BCS_BATCH(batch, CMD_AVC_BSD_SLICE_STATE | (cmd_len - 2));
    OUT_BCS_BATCH(batch, present_flag);

    for (list = 0; list < 2; list++) {
        int flag, num_va_pics;
        VAPictureH264 *va_pic;

        if (list == 0) {
            flag        = PRESENT_REF_LIST0;
            va_pic      = slice_param->RefPicList0;
            num_va_pics = slice_param->num_ref_idx_l0_active_minus1 + 1;
        } else {
            flag        = PRESENT_REF_LIST1;
            va_pic      = slice_param->RefPicList1;
            num_va_pics = slice_param->num_ref_idx_l1_active_minus1 + 1;
        }

        if (!(present_flag & flag))
            continue;

        gen5_fill_avc_ref_idx_state(
            ref_idx_state,
            va_pic, num_va_pics,
            i965_h264_context->fsid_list
        );            
        intel_batchbuffer_data(batch, ref_idx_state, sizeof(ref_idx_state));
    }

    i965_h264_context->weight128_luma_l0 = 0;
    i965_h264_context->weight128_luma_l1 = 0;
    i965_h264_context->weight128_chroma_l0 = 0;
    i965_h264_context->weight128_chroma_l1 = 0;

    i965_h264_context->weight128_offset0_flag = 0;
    i965_h264_context->weight128_offset0 = 0;

    if (present_flag & PRESENT_WEIGHT_OFFSET_L0) {
        for (j = 0; j < 32; j++) {
            weightoffsets[j * 6 + 0] = slice_param->luma_offset_l0[j];
            weightoffsets[j * 6 + 1] = slice_param->luma_weight_l0[j];
            weightoffsets[j * 6 + 2] = slice_param->chroma_offset_l0[j][0];
            weightoffsets[j * 6 + 3] = slice_param->chroma_weight_l0[j][0];
            weightoffsets[j * 6 + 4] = slice_param->chroma_offset_l0[j][1];
            weightoffsets[j * 6 + 5] = slice_param->chroma_weight_l0[j][1];

            if (pic_param->pic_fields.bits.weighted_pred_flag == 1 ||
                pic_param->pic_fields.bits.weighted_bipred_idc == 1) {
                if (i965_h264_context->use_hw_w128) {
                    if (slice_param->luma_weight_l0[j] == 128)
                        i965_h264_context->weight128_luma_l0 |= (1 << j);

                    if (slice_param->chroma_weight_l0[j][0] == 128 ||
                        slice_param->chroma_weight_l0[j][1] == 128)
                        i965_h264_context->weight128_chroma_l0 |= (1 << j);
                } else {
                    /* FIXME: workaround for weight 128 */
                    if (slice_param->luma_weight_l0[j] == 128 ||
                        slice_param->chroma_weight_l0[j][0] == 128 ||
                        slice_param->chroma_weight_l0[j][1] == 128)
                        i965_h264_context->weight128_offset0_flag = 1;
                }
            }
        }

        intel_batchbuffer_data(batch, weightoffsets, sizeof(weightoffsets));
    }

    if (present_flag & PRESENT_WEIGHT_OFFSET_L1) {
        for (j = 0; j < 32; j++) {
            weightoffsets[j * 6 + 0] = slice_param->luma_offset_l1[j];
            weightoffsets[j * 6 + 1] = slice_param->luma_weight_l1[j];
            weightoffsets[j * 6 + 2] = slice_param->chroma_offset_l1[j][0];
            weightoffsets[j * 6 + 3] = slice_param->chroma_weight_l1[j][0];
            weightoffsets[j * 6 + 4] = slice_param->chroma_offset_l1[j][1];
            weightoffsets[j * 6 + 5] = slice_param->chroma_weight_l1[j][1];

            if (pic_param->pic_fields.bits.weighted_bipred_idc == 1) {
                if (i965_h264_context->use_hw_w128) {
                    if (slice_param->luma_weight_l1[j] == 128)
                        i965_h264_context->weight128_luma_l1 |= (1 << j);

                    if (slice_param->chroma_weight_l1[j][0] == 128 ||
                        slice_param->chroma_weight_l1[j][1] == 128)
                        i965_h264_context->weight128_chroma_l1 |= (1 << j);
                } else {
                    if (slice_param->luma_weight_l0[j] == 128 ||
                        slice_param->chroma_weight_l0[j][0] == 128 ||
                        slice_param->chroma_weight_l0[j][1] == 128)
                        i965_h264_context->weight128_offset0_flag = 1;
                }
            }
        }

        intel_batchbuffer_data(batch, weightoffsets, sizeof(weightoffsets));
    }

    ADVANCE_BCS_BATCH(batch);
}

static void
i965_avc_bsd_buf_base_state(VADriverContextP ctx,
                            VAPictureParameterBufferH264 *pic_param, 
                            VASliceParameterBufferH264 *slice_param,
                            struct i965_h264_context *i965_h264_context)
{
    struct i965_driver_data *i965 = i965_driver_data(ctx);
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    struct i965_avc_bsd_context *i965_avc_bsd_context;
    int i, j;
    VAPictureH264 *va_pic;
    struct object_surface *obj_surface;
    GenAvcSurface *avc_bsd_surface;

    i965_avc_bsd_context = &i965_h264_context->i965_avc_bsd_context;

    BEGIN_BCS_BATCH(batch, 74);
    OUT_BCS_BATCH(batch, CMD_AVC_BSD_BUF_BASE_STATE | (74 - 2));
    OUT_BCS_RELOC(batch, i965_avc_bsd_context->bsd_raw_store.bo,
                  I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                  0);
    OUT_BCS_RELOC(batch, i965_avc_bsd_context->mpr_row_store.bo,
                  I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                  0);
    OUT_BCS_RELOC(batch, i965_h264_context->avc_it_command_mb_info.bo,
                  I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                  i965_h264_context->avc_it_command_mb_info.mbs * i965_h264_context->use_avc_hw_scoreboard * MB_CMD_IN_BYTES);
    OUT_BCS_RELOC(batch, i965_h264_context->avc_it_data.bo,
                  I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                  (i965_h264_context->avc_it_data.write_offset << 6));

    if (i965_h264_context->enable_avc_ildb)
        OUT_BCS_RELOC(batch, i965_h264_context->avc_ildb_data.bo,
                      I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                      0);
    else
        OUT_BCS_BATCH(batch, 0);

    for (i = 0; i < ARRAY_ELEMS(i965_h264_context->fsid_list); i++) {
        if (i965_h264_context->fsid_list[i].surface_id != VA_INVALID_ID) {
            int found = 0;
            for (j = 0; j < ARRAY_ELEMS(pic_param->ReferenceFrames); j++) {
                va_pic = &pic_param->ReferenceFrames[j];
                
                if (va_pic->flags & VA_PICTURE_H264_INVALID)
                    continue;

                if (va_pic->picture_id == i965_h264_context->fsid_list[i].surface_id) {
                    found = 1;
                    break;
                }
            }

            assert(found == 1);

            if (!(va_pic->flags & VA_PICTURE_H264_INVALID)) {
                obj_surface = SURFACE(va_pic->picture_id);
                assert(obj_surface);
                avc_bsd_surface = obj_surface->private_data;
            
                if (avc_bsd_surface == NULL) {
                    OUT_BCS_BATCH(batch, 0);
                    OUT_BCS_BATCH(batch, 0);
                } else {
                    OUT_BCS_RELOC(batch, avc_bsd_surface->dmv_top,
                                  I915_GEM_DOMAIN_INSTRUCTION, 0,
                                  0);

                    if (avc_bsd_surface->dmv_bottom_flag == 1)
                        OUT_BCS_RELOC(batch, avc_bsd_surface->dmv_bottom,
                                      I915_GEM_DOMAIN_INSTRUCTION, 0,
                                      0);
                    else
                        OUT_BCS_RELOC(batch, avc_bsd_surface->dmv_top,
                                      I915_GEM_DOMAIN_INSTRUCTION, 0,
                                      0);
                }
            } 
        } else {
            OUT_BCS_BATCH(batch, 0);
            OUT_BCS_BATCH(batch, 0);
        }
    }

    va_pic = &pic_param->CurrPic;
    assert(!(va_pic->flags & VA_PICTURE_H264_INVALID));
    obj_surface = SURFACE(va_pic->picture_id);
    assert(obj_surface);
    obj_surface->flags &= ~SURFACE_REF_DIS_MASK;
    obj_surface->flags |= (pic_param->pic_fields.bits.reference_pic_flag ? SURFACE_REFERENCED : 0);
    i965_check_alloc_surface_bo(ctx, obj_surface, 0, VA_FOURCC('N','V','1','2'), SUBSAMPLE_YUV420);

    /* initial uv component for YUV400 case */
    if (pic_param->seq_fields.bits.chroma_format_idc == 0) {
         unsigned int uv_offset = obj_surface->width * obj_surface->height; 
         unsigned int uv_size   = obj_surface->width * obj_surface->height / 2; 

         dri_bo_map(obj_surface->bo, 1);
         memset(obj_surface->bo->virtual + uv_offset, 0x80, uv_size);
         dri_bo_unmap(obj_surface->bo);
    }

    i965_avc_bsd_init_avc_bsd_surface(ctx, obj_surface, pic_param, i965_h264_context);
    avc_bsd_surface = obj_surface->private_data;

    OUT_BCS_RELOC(batch, avc_bsd_surface->dmv_top,
                  I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                  0);

    if (avc_bsd_surface->dmv_bottom_flag == 1)
        OUT_BCS_RELOC(batch, avc_bsd_surface->dmv_bottom,
                      I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                      0);
    else
        OUT_BCS_RELOC(batch, avc_bsd_surface->dmv_top,
                      I915_GEM_DOMAIN_INSTRUCTION, I915_GEM_DOMAIN_INSTRUCTION,
                      0);

    /* POC List */
    for (i = 0; i < ARRAY_ELEMS(i965_h264_context->fsid_list); i++) {
        if (i965_h264_context->fsid_list[i].surface_id != VA_INVALID_ID) {
            int found = 0;
            for (j = 0; j < ARRAY_ELEMS(pic_param->ReferenceFrames); j++) {
                va_pic = &pic_param->ReferenceFrames[j];
                
                if (va_pic->flags & VA_PICTURE_H264_INVALID)
                    continue;

                if (va_pic->picture_id == i965_h264_context->fsid_list[i].surface_id) {
                    found = 1;
                    break;
                }
            }

            assert(found == 1);

            if (!(va_pic->flags & VA_PICTURE_H264_INVALID)) {
                OUT_BCS_BATCH(batch, va_pic->TopFieldOrderCnt);
                OUT_BCS_BATCH(batch, va_pic->BottomFieldOrderCnt);
            } 
        } else {
            OUT_BCS_BATCH(batch, 0);
            OUT_BCS_BATCH(batch, 0);
        }
    }

    va_pic = &pic_param->CurrPic;
    OUT_BCS_BATCH(batch, va_pic->TopFieldOrderCnt);
    OUT_BCS_BATCH(batch, va_pic->BottomFieldOrderCnt);

    ADVANCE_BCS_BATCH(batch);
}

static void
g4x_avc_bsd_object(VADriverContextP ctx, 
                   struct decode_state *decode_state,
                   VAPictureParameterBufferH264 *pic_param,
                   VASliceParameterBufferH264 *slice_param,
                   int slice_index,
                   struct i965_h264_context *i965_h264_context)
{
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    int width_in_mbs = pic_param->picture_width_in_mbs_minus1 + 1;
    int height_in_mbs = pic_param->picture_height_in_mbs_minus1 + 1; /* frame height */

    if (slice_param) {
        int encrypted, counter_value, cmd_len;
        int slice_hor_pos, slice_ver_pos;
        int num_ref_idx_l0, num_ref_idx_l1;
        int mbaff_picture = (!pic_param->pic_fields.bits.field_pic_flag &&
                             pic_param->seq_fields.bits.mb_adaptive_frame_field_flag);
        unsigned int slice_data_bit_offset;
        int weighted_pred_idc = 0;
        int first_mb_in_slice = 0;
        int slice_type;

        encrypted = 0; /* FIXME: which flag in VAAPI is used for encryption? */

        if (encrypted) {
            cmd_len = 9;
            counter_value = 0; /* FIXME: ??? */
        } else 
            cmd_len = 8;


        slice_data_bit_offset = avc_get_first_mb_bit_offset_with_epb(
            decode_state->slice_datas[slice_index]->bo,
            slice_param,
            pic_param->pic_fields.bits.entropy_coding_mode_flag
        );

        if (slice_param->slice_type == SLICE_TYPE_I ||
            slice_param->slice_type == SLICE_TYPE_SI)
            slice_type = SLICE_TYPE_I;
        else if (slice_param->slice_type == SLICE_TYPE_P ||
                 slice_param->slice_type == SLICE_TYPE_SP)
            slice_type = SLICE_TYPE_P;
        else {
            assert(slice_param->slice_type == SLICE_TYPE_B);
            slice_type = SLICE_TYPE_B;
        }

        if (slice_type == SLICE_TYPE_I) {
            assert(slice_param->num_ref_idx_l0_active_minus1 == 0);
            assert(slice_param->num_ref_idx_l1_active_minus1 == 0);
            num_ref_idx_l0 = 0;
            num_ref_idx_l1 = 0;
        } else if (slice_type == SLICE_TYPE_P) {
            assert(slice_param->num_ref_idx_l1_active_minus1 == 0);
            num_ref_idx_l0 = slice_param->num_ref_idx_l0_active_minus1 + 1;
            num_ref_idx_l1 = 0;
        } else {
            num_ref_idx_l0 = slice_param->num_ref_idx_l0_active_minus1 + 1;
            num_ref_idx_l1 = slice_param->num_ref_idx_l1_active_minus1 + 1;
        }

        if (slice_type == SLICE_TYPE_P)
            weighted_pred_idc = pic_param->pic_fields.bits.weighted_pred_flag;
        else if (slice_type == SLICE_TYPE_B)
            weighted_pred_idc = pic_param->pic_fields.bits.weighted_bipred_idc;

        first_mb_in_slice = slice_param->first_mb_in_slice << mbaff_picture;
        slice_hor_pos = first_mb_in_slice % width_in_mbs; 
        slice_ver_pos = first_mb_in_slice / width_in_mbs;

        BEGIN_BCS_BATCH(batch, cmd_len);
        OUT_BCS_BATCH(batch, CMD_AVC_BSD_OBJECT | (cmd_len - 2));
        OUT_BCS_BATCH(batch, 
                      (encrypted << 31) |
                      ((slice_param->slice_data_size - (slice_data_bit_offset >> 3)) << 0));
        OUT_BCS_BATCH(batch, 
                      (slice_param->slice_data_offset +
                       (slice_data_bit_offset >> 3)));
        OUT_BCS_BATCH(batch, 
                      (0 << 31) | /* concealment mode: 0->intra 16x16 prediction, 1->inter P Copy */
                      (0 << 14) | /* ignore BSDPrematureComplete Error handling */
                      (0 << 13) | /* FIXME: ??? */
                      (0 << 12) | /* ignore MPR Error handling */
                      (0 << 10) | /* ignore Entropy Error handling */
                      (0 << 8)  | /* ignore MB Header Error handling */
                      (slice_type << 0));
        OUT_BCS_BATCH(batch, 
                      (num_ref_idx_l1 << 24) |
                      (num_ref_idx_l0 << 16) |
                      (slice_param->chroma_log2_weight_denom << 8) |
                      (slice_param->luma_log2_weight_denom << 0));
        OUT_BCS_BATCH(batch, 
                      (weighted_pred_idc << 30) |
                      (slice_param->direct_spatial_mv_pred_flag << 29) |
                      (slice_param->disable_deblocking_filter_idc << 27) |
                      (slice_param->cabac_init_idc << 24) |
                      ((pic_param->pic_init_qp_minus26 + 26 + slice_param->slice_qp_delta) << 16) |
                      ((slice_param->slice_beta_offset_div2 & 0xf) << 8) |
                      ((slice_param->slice_alpha_c0_offset_div2 & 0xf) << 0));
        OUT_BCS_BATCH(batch, 
                      (slice_ver_pos << 24) |
                      (slice_hor_pos << 16) | 
                      (first_mb_in_slice << 0));
        OUT_BCS_BATCH(batch, 
                      (1 << 7) |
                      ((0x7 - (slice_data_bit_offset & 0x7)) << 0));

        if (encrypted) {
            OUT_BCS_BATCH(batch, counter_value);
        }

        ADVANCE_BCS_BATCH(batch); 
    } else {
        BEGIN_BCS_BATCH(batch, 8); 
        OUT_BCS_BATCH(batch, CMD_AVC_BSD_OBJECT | (8 - 2));
        OUT_BCS_BATCH(batch, 0); /* indirect data length for phantom slice is 0 */
        OUT_BCS_BATCH(batch, 0); /* indirect data start address for phantom slice is 0 */
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, width_in_mbs * height_in_mbs / (1 + !!pic_param->pic_fields.bits.field_pic_flag));
        OUT_BCS_BATCH(batch, 0);
        ADVANCE_BCS_BATCH(batch);
    }
}

static void
ironlake_avc_bsd_object(VADriverContextP ctx, 
                        struct decode_state *decode_state,
                        VAPictureParameterBufferH264 *pic_param,
                        VASliceParameterBufferH264 *slice_param,
                        int slice_index,
                        struct i965_h264_context *i965_h264_context)
{
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    int width_in_mbs = pic_param->picture_width_in_mbs_minus1 + 1;
    int height_in_mbs = pic_param->picture_height_in_mbs_minus1 + 1; /* frame height */

    if (slice_param) {
        int encrypted, counter_value;
        int slice_hor_pos, slice_ver_pos;
        int num_ref_idx_l0, num_ref_idx_l1;
        int mbaff_picture = (!pic_param->pic_fields.bits.field_pic_flag &&
                             pic_param->seq_fields.bits.mb_adaptive_frame_field_flag);
        unsigned int slice_data_bit_offset;
        int weighted_pred_idc = 0;
        int first_mb_in_slice;
        int slice_type;

        encrypted = 0; /* FIXME: which flag in VAAPI is used for encryption? */

        if (encrypted) {
            counter_value = 0; /* FIXME: ??? */
        } else 
            counter_value = 0;

        slice_data_bit_offset = avc_get_first_mb_bit_offset_with_epb(
            decode_state->slice_datas[slice_index]->bo,
            slice_param,
            pic_param->pic_fields.bits.entropy_coding_mode_flag
        );

        if (slice_param->slice_type == SLICE_TYPE_I ||
            slice_param->slice_type == SLICE_TYPE_SI)
            slice_type = SLICE_TYPE_I;
        else if (slice_param->slice_type == SLICE_TYPE_P ||
                 slice_param->slice_type == SLICE_TYPE_SP)
            slice_type = SLICE_TYPE_P;
        else {
            assert(slice_param->slice_type == SLICE_TYPE_B);
            slice_type = SLICE_TYPE_B;
        }

        if (slice_type == SLICE_TYPE_I) {
            assert(slice_param->num_ref_idx_l0_active_minus1 == 0);
            assert(slice_param->num_ref_idx_l1_active_minus1 == 0);
            num_ref_idx_l0 = 0;
            num_ref_idx_l1 = 0;
        } else if (slice_type == SLICE_TYPE_P) {
            assert(slice_param->num_ref_idx_l1_active_minus1 == 0);
            num_ref_idx_l0 = slice_param->num_ref_idx_l0_active_minus1 + 1;
            num_ref_idx_l1 = 0;
        } else {
            num_ref_idx_l0 = slice_param->num_ref_idx_l0_active_minus1 + 1;
            num_ref_idx_l1 = slice_param->num_ref_idx_l1_active_minus1 + 1;
        }

        if (slice_type == SLICE_TYPE_P)
            weighted_pred_idc = pic_param->pic_fields.bits.weighted_pred_flag;
        else if (slice_type == SLICE_TYPE_B)
            weighted_pred_idc = pic_param->pic_fields.bits.weighted_bipred_idc;

        first_mb_in_slice = slice_param->first_mb_in_slice << mbaff_picture;
        slice_hor_pos = first_mb_in_slice % width_in_mbs; 
        slice_ver_pos = first_mb_in_slice / width_in_mbs;

        BEGIN_BCS_BATCH(batch, 16);
        OUT_BCS_BATCH(batch, CMD_AVC_BSD_OBJECT | (16 - 2));
        OUT_BCS_BATCH(batch, 
                      (encrypted << 31) |
                      (0 << 30) | /* FIXME: packet based bit stream */
                      (0 << 29) | /* FIXME: packet format */
                      ((slice_param->slice_data_size - (slice_data_bit_offset >> 3)) << 0));
        OUT_BCS_BATCH(batch, 
                      (slice_param->slice_data_offset +
                       (slice_data_bit_offset >> 3)));
        OUT_BCS_BATCH(batch, 
                      (0 << 31) | /* concealment mode: 0->intra 16x16 prediction, 1->inter P Copy */
                      (0 << 14) | /* ignore BSDPrematureComplete Error handling */
                      (0 << 13) | /* FIXME: ??? */
                      (0 << 12) | /* ignore MPR Error handling */
                      (0 << 10) | /* ignore Entropy Error handling */
                      (0 << 8)  | /* ignore MB Header Error handling */
                      (slice_type << 0));
        OUT_BCS_BATCH(batch, 
                      (num_ref_idx_l1 << 24) |
                      (num_ref_idx_l0 << 16) |
                      (slice_param->chroma_log2_weight_denom << 8) |
                      (slice_param->luma_log2_weight_denom << 0));
        OUT_BCS_BATCH(batch, 
                      (weighted_pred_idc << 30) |
                      (slice_param->direct_spatial_mv_pred_flag << 29) |
                      (slice_param->disable_deblocking_filter_idc << 27) |
                      (slice_param->cabac_init_idc << 24) |
                      ((pic_param->pic_init_qp_minus26 + 26 + slice_param->slice_qp_delta) << 16) |
                      ((slice_param->slice_beta_offset_div2 & 0xf) << 8) |
                      ((slice_param->slice_alpha_c0_offset_div2 & 0xf) << 0));
        OUT_BCS_BATCH(batch, 
                      (slice_ver_pos << 24) |
                      (slice_hor_pos << 16) | 
                      (first_mb_in_slice << 0));
        OUT_BCS_BATCH(batch, 
                      (1 << 7) |
                      ((0x7 - (slice_data_bit_offset & 0x7)) << 0));
        OUT_BCS_BATCH(batch, counter_value);
        
        /* FIXME: dw9-dw11 */
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, i965_h264_context->weight128_luma_l0);
        OUT_BCS_BATCH(batch, i965_h264_context->weight128_luma_l1);
        OUT_BCS_BATCH(batch, i965_h264_context->weight128_chroma_l0);
        OUT_BCS_BATCH(batch, i965_h264_context->weight128_chroma_l1);

        ADVANCE_BCS_BATCH(batch); 
    } else {
        BEGIN_BCS_BATCH(batch, 16);
        OUT_BCS_BATCH(batch, CMD_AVC_BSD_OBJECT | (16 - 2));
        OUT_BCS_BATCH(batch, 0); /* indirect data length for phantom slice is 0 */
        OUT_BCS_BATCH(batch, 0); /* indirect data start address for phantom slice is 0 */
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, width_in_mbs * height_in_mbs / (1 + !!pic_param->pic_fields.bits.field_pic_flag));
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        OUT_BCS_BATCH(batch, 0);
        ADVANCE_BCS_BATCH(batch);
    }
}

static void
i965_avc_bsd_object(VADriverContextP ctx, 
                    struct decode_state *decode_state,
                    VAPictureParameterBufferH264 *pic_param,
                    VASliceParameterBufferH264 *slice_param,
                    int slice_index,
                    struct i965_h264_context *i965_h264_context)
{
    struct i965_driver_data *i965 = i965_driver_data(ctx);

    if (IS_IRONLAKE(i965->intel.device_id))
        ironlake_avc_bsd_object(ctx, decode_state, pic_param, slice_param, slice_index, i965_h264_context);
    else
        g4x_avc_bsd_object(ctx, decode_state, pic_param, slice_param, slice_index, i965_h264_context);
}

static void
i965_avc_bsd_phantom_slice(VADriverContextP ctx, 
                           struct decode_state *decode_state,
                           VAPictureParameterBufferH264 *pic_param,
                           struct i965_h264_context *i965_h264_context)
{
    i965_avc_bsd_object(ctx, decode_state, pic_param, NULL, 0, i965_h264_context);
}

static void
i965_avc_bsd_frame_store_index(VADriverContextP ctx,
                               VAPictureParameterBufferH264 *pic_param,
                               struct i965_h264_context *i965_h264_context)
{
    struct i965_driver_data *i965 = i965_driver_data(ctx);
    int i, j;

    assert(ARRAY_ELEMS(i965_h264_context->fsid_list) == ARRAY_ELEMS(pic_param->ReferenceFrames));

    for (i = 0; i < ARRAY_ELEMS(i965_h264_context->fsid_list); i++) {
        int found = 0;

        if (i965_h264_context->fsid_list[i].surface_id == VA_INVALID_ID)
            continue;

        for (j = 0; j < ARRAY_ELEMS(pic_param->ReferenceFrames); j++) {
            VAPictureH264 *ref_pic = &pic_param->ReferenceFrames[j];
            if (ref_pic->flags & VA_PICTURE_H264_INVALID)
                continue;

            if (i965_h264_context->fsid_list[i].surface_id == ref_pic->picture_id) {
                found = 1;
                break;
            }
        }

        if (!found) {
            struct object_surface *obj_surface = SURFACE(i965_h264_context->fsid_list[i].surface_id);
            obj_surface->flags &= ~SURFACE_REFERENCED;

            if ((obj_surface->flags & SURFACE_ALL_MASK) == SURFACE_DISPLAYED) {
                dri_bo_unreference(obj_surface->bo);
                obj_surface->bo = NULL;
                obj_surface->flags &= ~SURFACE_REF_DIS_MASK;
            }

            if (obj_surface->free_private_data)
                obj_surface->free_private_data(&obj_surface->private_data);

            i965_h264_context->fsid_list[i].surface_id = VA_INVALID_ID;
            i965_h264_context->fsid_list[i].frame_store_id = -1;
        }
    }

    for (i = 0; i < ARRAY_ELEMS(pic_param->ReferenceFrames); i++) {
        VAPictureH264 *ref_pic = &pic_param->ReferenceFrames[i];
        int found = 0;

        if (ref_pic->flags & VA_PICTURE_H264_INVALID)
            continue;

        for (j = 0; j < ARRAY_ELEMS(i965_h264_context->fsid_list); j++) {
            if (i965_h264_context->fsid_list[j].surface_id == VA_INVALID_ID)
                continue;
            
            if (i965_h264_context->fsid_list[j].surface_id == ref_pic->picture_id) {
                found = 1;
                break;
            }
        }

        if (!found) {
            int frame_idx;
            struct object_surface *obj_surface = SURFACE(ref_pic->picture_id);
            assert(obj_surface);
            i965_check_alloc_surface_bo(ctx, obj_surface, 0, VA_FOURCC('N','V','1','2'), SUBSAMPLE_YUV420);
            
            for (frame_idx = 0; frame_idx < ARRAY_ELEMS(i965_h264_context->fsid_list); frame_idx++) {
                for (j = 0; j < ARRAY_ELEMS(i965_h264_context->fsid_list); j++) {
                    if (i965_h264_context->fsid_list[j].surface_id == VA_INVALID_ID)
                        continue;

                    if (i965_h264_context->fsid_list[j].frame_store_id == frame_idx)
                        break;
                }

                if (j == ARRAY_ELEMS(i965_h264_context->fsid_list))
                    break;
            }

            assert(frame_idx < ARRAY_ELEMS(i965_h264_context->fsid_list));

            for (j = 0; j < ARRAY_ELEMS(i965_h264_context->fsid_list); j++) {
                if (i965_h264_context->fsid_list[j].surface_id == VA_INVALID_ID) {
                    i965_h264_context->fsid_list[j].surface_id = ref_pic->picture_id;
                    i965_h264_context->fsid_list[j].frame_store_id = frame_idx;
                    break;
                }
            }
        }
    }

    for (i = 0; i < ARRAY_ELEMS(i965_h264_context->fsid_list) - 1; i++) {
        if (i965_h264_context->fsid_list[i].surface_id != VA_INVALID_ID &&
            i965_h264_context->fsid_list[i].frame_store_id == i)
            continue;

        for (j = i + 1; j < ARRAY_ELEMS(i965_h264_context->fsid_list); j++) {
            if (i965_h264_context->fsid_list[j].surface_id != VA_INVALID_ID &&
                i965_h264_context->fsid_list[j].frame_store_id == i) {
                VASurfaceID id = i965_h264_context->fsid_list[i].surface_id;
                int frame_idx = i965_h264_context->fsid_list[i].frame_store_id;

                i965_h264_context->fsid_list[i].surface_id = i965_h264_context->fsid_list[j].surface_id;
                i965_h264_context->fsid_list[i].frame_store_id = i965_h264_context->fsid_list[j].frame_store_id;
                i965_h264_context->fsid_list[j].surface_id = id;
                i965_h264_context->fsid_list[j].frame_store_id = frame_idx;
                break;
            }
        }
    }
}

void 
i965_avc_bsd_pipeline(VADriverContextP ctx, struct decode_state *decode_state, void *h264_context)
{
    struct i965_h264_context *i965_h264_context = (struct i965_h264_context *)h264_context;
    struct intel_batchbuffer *batch = i965_h264_context->batch;
    VAPictureParameterBufferH264 *pic_param;
    VASliceParameterBufferH264 *slice_param;
    int i, j;

    assert(decode_state->pic_param && decode_state->pic_param->buffer);
    pic_param = (VAPictureParameterBufferH264 *)decode_state->pic_param->buffer;
    i965_avc_bsd_frame_store_index(ctx, pic_param, i965_h264_context);

    i965_h264_context->enable_avc_ildb = 0;
    i965_h264_context->picture.i_flag = 1;

    for (j = 0; j < decode_state->num_slice_params && i965_h264_context->enable_avc_ildb == 0; j++) {
        assert(decode_state->slice_params && decode_state->slice_params[j]->buffer);
        slice_param = (VASliceParameterBufferH264 *)decode_state->slice_params[j]->buffer;

        for (i = 0; i < decode_state->slice_params[j]->num_elements; i++) {
            assert(slice_param->slice_data_flag == VA_SLICE_DATA_FLAG_ALL);
            assert((slice_param->slice_type == SLICE_TYPE_I) ||
                   (slice_param->slice_type == SLICE_TYPE_SI) ||
                   (slice_param->slice_type == SLICE_TYPE_P) ||
                   (slice_param->slice_type == SLICE_TYPE_SP) ||
                   (slice_param->slice_type == SLICE_TYPE_B));

            if (slice_param->disable_deblocking_filter_idc != 1) {
                i965_h264_context->enable_avc_ildb = 1;
                break;
            }

            slice_param++;
        }
    }

    intel_batchbuffer_start_atomic_bcs(batch, 0x1000);

    i965_avc_bsd_img_state(ctx, decode_state, i965_h264_context);
    i965_avc_bsd_qm_state(ctx, decode_state, i965_h264_context);

    for (j = 0; j < decode_state->num_slice_params; j++) {
        assert(decode_state->slice_params && decode_state->slice_params[j]->buffer);
        slice_param = (VASliceParameterBufferH264 *)decode_state->slice_params[j]->buffer;

        i965_bsd_ind_obj_base_address(ctx, decode_state, j, i965_h264_context);

        for (i = 0; i < decode_state->slice_params[j]->num_elements; i++) {
            assert(slice_param->slice_data_flag == VA_SLICE_DATA_FLAG_ALL);
            assert((slice_param->slice_type == SLICE_TYPE_I) ||
                   (slice_param->slice_type == SLICE_TYPE_SI) ||
                   (slice_param->slice_type == SLICE_TYPE_P) ||
                   (slice_param->slice_type == SLICE_TYPE_SP) ||
                   (slice_param->slice_type == SLICE_TYPE_B));

            if (i965_h264_context->picture.i_flag && 
                (slice_param->slice_type != SLICE_TYPE_I ||
                 slice_param->slice_type != SLICE_TYPE_SI))
                i965_h264_context->picture.i_flag = 0;

            i965_avc_bsd_slice_state(ctx, pic_param, slice_param, i965_h264_context);
            i965_avc_bsd_buf_base_state(ctx, pic_param, slice_param, i965_h264_context);
            i965_avc_bsd_object(ctx, decode_state, pic_param, slice_param, j, i965_h264_context);
            slice_param++;
        }
    }

    i965_avc_bsd_phantom_slice(ctx, decode_state, pic_param, i965_h264_context);
    intel_batchbuffer_emit_mi_flush(batch);
    intel_batchbuffer_end_atomic(batch);
    intel_batchbuffer_flush(batch);
}

void 
i965_avc_bsd_decode_init(VADriverContextP ctx, void *h264_context)
{
    struct i965_driver_data *i965 = i965_driver_data(ctx);
    struct i965_h264_context *i965_h264_context = (struct i965_h264_context *)h264_context;
    struct i965_avc_bsd_context *i965_avc_bsd_context;
    dri_bo *bo;

    assert(i965_h264_context);
    i965_avc_bsd_context = &i965_h264_context->i965_avc_bsd_context;

    dri_bo_unreference(i965_avc_bsd_context->bsd_raw_store.bo);
    bo = dri_bo_alloc(i965->intel.bufmgr,
                      "bsd raw store",
                      0x3000, /* at least 11520 bytes to support 120 MBs per row */
                      64);
    assert(bo);
    i965_avc_bsd_context->bsd_raw_store.bo = bo;

    dri_bo_unreference(i965_avc_bsd_context->mpr_row_store.bo);
    bo = dri_bo_alloc(i965->intel.bufmgr,
                      "mpr row store",
                      0x2000, /* at least 7680 bytes to support 120 MBs per row */
                      64);
    assert(bo);
    i965_avc_bsd_context->mpr_row_store.bo = bo;
}

Bool 
i965_avc_bsd_ternimate(struct i965_avc_bsd_context *i965_avc_bsd_context)
{
    dri_bo_unreference(i965_avc_bsd_context->bsd_raw_store.bo);
    dri_bo_unreference(i965_avc_bsd_context->mpr_row_store.bo);

    return True;
}
