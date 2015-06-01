/**************************************************************************
 *
 * Copyright 2010 Thomas Balling Sørensen & Orasanu Lucian.
 * Copyright 2014 Advanced Micro Devices, Inc.
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
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#ifndef VA_PRIVATE_H
#define VA_PRIVATE_H

#include <assert.h>

#include <va/va.h>
#include <va/va_backend.h>

#include "pipe/p_video_enums.h"
#include "pipe/p_video_codec.h"
#include "pipe/p_video_state.h"

#include "vl/vl_compositor.h"
#include "vl/vl_csc.h"

#include "util/u_dynarray.h"

#define VL_VA_DRIVER(ctx) ((vlVaDriver *)ctx->pDriverData)
#define VL_VA_PSCREEN(ctx) (VL_VA_DRIVER(ctx)->vscreen->pscreen)

#define VL_VA_MAX_IMAGE_FORMATS 6

static inline enum pipe_video_chroma_format
ChromaToPipe(int format)
{
   switch (format) {
   case VA_RT_FORMAT_YUV420:
      return PIPE_VIDEO_CHROMA_FORMAT_420;
   case VA_RT_FORMAT_YUV422:
      return PIPE_VIDEO_CHROMA_FORMAT_422;
   case VA_RT_FORMAT_YUV444:
      return PIPE_VIDEO_CHROMA_FORMAT_444;
   default:
      assert(0);
      return PIPE_VIDEO_CHROMA_FORMAT_420;
   }
}

static inline enum pipe_format
YCbCrToPipe(unsigned format)
{
   switch(format) {
   case VA_FOURCC('N','V','1','2'):
      return PIPE_FORMAT_NV12;
   case VA_FOURCC('I','4','2','0'):
      return PIPE_FORMAT_IYUV;
   case VA_FOURCC('Y','V','1','2'):
      return PIPE_FORMAT_YV12;
   case VA_FOURCC('Y','U','Y','V'):
      return PIPE_FORMAT_YUYV;
   case VA_FOURCC('U','Y','V','Y'):
      return PIPE_FORMAT_UYVY;
   case VA_FOURCC('B','G','R','A'):
      return PIPE_FORMAT_B8G8R8A8_UNORM;
   default:
      assert(0);
      return PIPE_FORMAT_NONE;
   }
}

static inline VAProfile
PipeToProfile(enum pipe_video_profile profile)
{
   switch (profile) {
   case PIPE_VIDEO_PROFILE_MPEG2_SIMPLE:
      return VAProfileMPEG2Simple;
   case PIPE_VIDEO_PROFILE_MPEG2_MAIN:
      return VAProfileMPEG2Main;
   case PIPE_VIDEO_PROFILE_MPEG4_SIMPLE:
      return VAProfileMPEG4Simple;
   case PIPE_VIDEO_PROFILE_MPEG4_ADVANCED_SIMPLE:
      return VAProfileMPEG4AdvancedSimple;
   case PIPE_VIDEO_PROFILE_VC1_SIMPLE:
      return VAProfileVC1Simple;
   case PIPE_VIDEO_PROFILE_VC1_MAIN:
      return VAProfileVC1Main;
   case PIPE_VIDEO_PROFILE_VC1_ADVANCED:
      return VAProfileVC1Advanced;
   case PIPE_VIDEO_PROFILE_MPEG4_AVC_BASELINE:
      return VAProfileH264Baseline;
   case PIPE_VIDEO_PROFILE_MPEG4_AVC_MAIN:
      return VAProfileH264Main;
   case PIPE_VIDEO_PROFILE_MPEG4_AVC_HIGH:
      return VAProfileH264High;
   case PIPE_VIDEO_PROFILE_MPEG4_AVC_EXTENDED:
       return VAProfileNone;
   default:
      assert(0);
      return -1;
   }
}

static inline enum pipe_video_profile
ProfileToPipe(VAProfile profile)
{
   switch (profile) {
   case VAProfileMPEG2Simple:
      return PIPE_VIDEO_PROFILE_MPEG2_SIMPLE;
   case VAProfileMPEG2Main:
      return PIPE_VIDEO_PROFILE_MPEG2_MAIN;
   case VAProfileMPEG4Simple:
      return PIPE_VIDEO_PROFILE_MPEG4_SIMPLE;
   case VAProfileMPEG4AdvancedSimple:
      return PIPE_VIDEO_PROFILE_MPEG4_ADVANCED_SIMPLE;
   case VAProfileVC1Simple:
      return PIPE_VIDEO_PROFILE_VC1_SIMPLE;
   case VAProfileVC1Main:
      return PIPE_VIDEO_PROFILE_VC1_MAIN;
   case VAProfileVC1Advanced:
      return PIPE_VIDEO_PROFILE_VC1_ADVANCED;
   case VAProfileH264Baseline:
      return PIPE_VIDEO_PROFILE_MPEG4_AVC_BASELINE;
   case VAProfileH264Main:
      return PIPE_VIDEO_PROFILE_MPEG4_AVC_MAIN;
   case VAProfileH264High:
      return PIPE_VIDEO_PROFILE_MPEG4_AVC_HIGH;
   default:
      return PIPE_VIDEO_PROFILE_UNKNOWN;
   }
}

typedef struct {
   struct vl_screen *vscreen;
   struct pipe_context *pipe;
   struct handle_table *htab;
   struct vl_compositor compositor;
   struct vl_compositor_state cstate;
   vl_csc_matrix csc;
} vlVaDriver;

typedef struct {
   VAImage *image;

   struct u_rect src_rect;
   struct u_rect dst_rect;

   struct pipe_sampler_view *sampler;
} vlVaSubpicture;

typedef struct {
   struct pipe_video_codec *decoder;
   struct pipe_video_buffer *target;
   union {
      struct pipe_picture_desc base;
      struct pipe_mpeg12_picture_desc mpeg12;
      struct pipe_mpeg4_picture_desc mpeg4;
      struct pipe_vc1_picture_desc vc1;
      struct pipe_h264_picture_desc h264;
   } desc;

   struct {
      unsigned long long int frame_num;
      unsigned int start_code_size;
      unsigned int vti_bits;
      unsigned int quant_scale;
      VAPictureParameterBufferMPEG4 pps;
      uint8_t start_code[32];
   } mpeg4;
} vlVaContext;

typedef struct {
   VABufferType type;
   unsigned int size;
   unsigned int num_elements;
   void *data;
} vlVaBuffer;

typedef struct {
   struct pipe_video_buffer templat, *buffer;
   struct pipe_fence_handle *fence;
   struct util_dynarray subpics; /* vlVaSubpicture */
} vlVaSurface;

// Public functions:
VAStatus VA_DRIVER_INIT_FUNC(VADriverContextP ctx);

// vtable functions:
VAStatus vlVaTerminate(VADriverContextP ctx);
VAStatus vlVaQueryConfigProfiles(VADriverContextP ctx, VAProfile *profile_list,int *num_profiles);
VAStatus vlVaQueryConfigEntrypoints(VADriverContextP ctx, VAProfile profile,
                                    VAEntrypoint  *entrypoint_list, int *num_entrypoints);
VAStatus vlVaGetConfigAttributes(VADriverContextP ctx, VAProfile profile, VAEntrypoint entrypoint,
                                 VAConfigAttrib *attrib_list, int num_attribs);
VAStatus vlVaCreateConfig(VADriverContextP ctx, VAProfile profile, VAEntrypoint entrypoint,
                          VAConfigAttrib *attrib_list, int num_attribs, VAConfigID *config_id);
VAStatus vlVaDestroyConfig(VADriverContextP ctx, VAConfigID config_id);
VAStatus vlVaQueryConfigAttributes(VADriverContextP ctx, VAConfigID config_id, VAProfile *profile,
                                   VAEntrypoint *entrypoint, VAConfigAttrib *attrib_list, int *num_attribs);
VAStatus vlVaCreateSurfaces(VADriverContextP ctx, int width, int height, int format,
                            int num_surfaces, VASurfaceID *surfaces);
VAStatus vlVaDestroySurfaces(VADriverContextP ctx, VASurfaceID *surface_list, int num_surfaces);
VAStatus vlVaCreateContext(VADriverContextP ctx, VAConfigID config_id, int picture_width, int picture_height,
                           int flag, VASurfaceID *render_targets, int num_render_targets, VAContextID *context);
VAStatus vlVaDestroyContext(VADriverContextP ctx, VAContextID context);
VAStatus vlVaCreateBuffer(VADriverContextP ctx, VAContextID context, VABufferType type, unsigned int size,
                          unsigned int num_elements, void *data, VABufferID *buf_id);
VAStatus vlVaBufferSetNumElements(VADriverContextP ctx, VABufferID buf_id, unsigned int num_elements);
VAStatus vlVaMapBuffer(VADriverContextP ctx, VABufferID buf_id, void **pbuf);
VAStatus vlVaUnmapBuffer(VADriverContextP ctx, VABufferID buf_id);
VAStatus vlVaDestroyBuffer(VADriverContextP ctx, VABufferID buffer_id);
VAStatus vlVaBeginPicture(VADriverContextP ctx, VAContextID context, VASurfaceID render_target);
VAStatus vlVaRenderPicture(VADriverContextP ctx, VAContextID context, VABufferID *buffers, int num_buffers);
VAStatus vlVaEndPicture(VADriverContextP ctx, VAContextID context);
VAStatus vlVaSyncSurface(VADriverContextP ctx, VASurfaceID render_target);
VAStatus vlVaQuerySurfaceStatus(VADriverContextP ctx, VASurfaceID render_target, VASurfaceStatus *status);
VAStatus vlVaQuerySurfaceError(VADriverContextP ctx, VASurfaceID render_target,
                               VAStatus error_status, void **error_info);
VAStatus vlVaPutSurface(VADriverContextP ctx, VASurfaceID surface, void* draw, short srcx, short srcy,
                        unsigned short srcw, unsigned short srch, short destx, short desty, unsigned short destw,
                        unsigned short desth, VARectangle *cliprects, unsigned int number_cliprects,
                        unsigned int flags);
VAStatus vlVaQueryImageFormats(VADriverContextP ctx, VAImageFormat *format_list, int *num_formats);
VAStatus vlVaQuerySubpictureFormats(VADriverContextP ctx, VAImageFormat *format_list,
                                    unsigned int *flags, unsigned int *num_formats);
VAStatus vlVaCreateImage(VADriverContextP ctx, VAImageFormat *format, int width, int height, VAImage *image);
VAStatus vlVaDeriveImage(VADriverContextP ctx, VASurfaceID surface, VAImage *image);
VAStatus vlVaDestroyImage(VADriverContextP ctx, VAImageID image);
VAStatus vlVaSetImagePalette(VADriverContextP ctx, VAImageID image, unsigned char *palette);
VAStatus vlVaGetImage(VADriverContextP ctx, VASurfaceID surface, int x, int y,
                      unsigned int width, unsigned int height, VAImageID image);
VAStatus vlVaPutImage(VADriverContextP ctx, VASurfaceID surface, VAImageID image, int src_x, int src_y,
                      unsigned int src_width, unsigned int src_height, int dest_x, int dest_y,
                      unsigned int dest_width, unsigned int dest_height);
VAStatus vlVaQuerySubpictureFormats(VADriverContextP ctx, VAImageFormat *format_list,
                                    unsigned int *flags, unsigned int *num_formats);
VAStatus vlVaCreateSubpicture(VADriverContextP ctx, VAImageID image, VASubpictureID *subpicture);
VAStatus vlVaDestroySubpicture(VADriverContextP ctx, VASubpictureID subpicture);
VAStatus vlVaSubpictureImage(VADriverContextP ctx, VASubpictureID subpicture, VAImageID image);
VAStatus vlVaSetSubpictureChromakey(VADriverContextP ctx, VASubpictureID subpicture,
                                    unsigned int chromakey_min, unsigned int chromakey_max,
                                    unsigned int chromakey_mask);
VAStatus vlVaSetSubpictureGlobalAlpha(VADriverContextP ctx, VASubpictureID subpicture, float global_alpha);
VAStatus vlVaAssociateSubpicture(VADriverContextP ctx, VASubpictureID subpicture, VASurfaceID *target_surfaces,
                                 int num_surfaces, short src_x, short src_y,
                                 unsigned short src_width, unsigned short src_height,
                                 short dest_x, short dest_y, unsigned short dest_width, unsigned short dest_height,
                                 unsigned int flags);
VAStatus vlVaDeassociateSubpicture(VADriverContextP ctx, VASubpictureID subpicture,
                                   VASurfaceID *target_surfaces, int num_surfaces);
VAStatus vlVaQueryDisplayAttributes(VADriverContextP ctx, VADisplayAttribute *attr_list, int *num_attributes);
VAStatus vlVaGetDisplayAttributes(VADriverContextP ctx, VADisplayAttribute *attr_list, int num_attributes);
VAStatus vlVaSetDisplayAttributes(VADriverContextP ctx, VADisplayAttribute *attr_list, int num_attributes);
VAStatus vlVaBufferInfo(VADriverContextP ctx, VABufferID buf_id, VABufferType *type,
                        unsigned int *size, unsigned int *num_elements);
VAStatus vlVaLockSurface(VADriverContextP ctx, VASurfaceID surface, unsigned int *fourcc,
                         unsigned int *luma_stride, unsigned int *chroma_u_stride, unsigned int *chroma_v_stride,
                         unsigned int *luma_offset, unsigned int *chroma_u_offset, unsigned int *chroma_v_offset,
                         unsigned int *buffer_name, void **buffer);
VAStatus vlVaUnlockSurface(VADriverContextP ctx, VASurfaceID surface);

#endif //VA_PRIVATE_H
