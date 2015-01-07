
/*
 * Copyright (c) 2009-2011 Intel Corporation. All Rights Reserved.
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
 */

#define _GNU_SOURCE 1
#include "va.h"
#include "va_enc_h264.h"
#include "va_backend.h"
#include "va_trace.h"
#include "va_enc_h264.h"
#include "va_dec_jpeg.h"
#include "va_dec_vp8.h"
#include "va_vpp.h"
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

/*
 * Env. to debug some issue, e.g. the decode/encode issue in a video conference scenerio:
 * .LIBVA_TRACE=log_file: general VA parameters saved into log_file
 * .LIBVA_TRACE_BUFDATA: dump all VA data buffer into log_file
 * .LIBVA_TRACE_CODEDBUF=coded_clip_file: save the coded clip into file coded_clip_file
 * .LIBVA_TRACE_SURFACE=yuv_file: save surface YUV into file yuv_file. Use file name to determine
 *                                decode/encode or jpeg surfaces
 * .LIBVA_TRACE_SURFACE_GEOMETRY=WIDTHxHEIGHT+XOFF+YOFF: only save part of surface context into file
 *                                due to storage bandwidth limitation
 */

/* global settings */

/* LIBVA_TRACE */
int trace_flag = 0;

/* per context settings */
struct trace_context {
    /* LIBVA_TRACE */
    FILE *trace_fp_log; /* save the log into a file */
    char *trace_log_fn; /* file name */

    /* LIBVA_TRACE_CODEDBUF */
    FILE *trace_fp_codedbuf; /* save the encode result into a file */
    char *trace_codedbuf_fn; /* file name */

    /* LIBVA_TRACE_SURFACE */
    FILE *trace_fp_surface; /* save the surface YUV into a file */
    char *trace_surface_fn; /* file name */

    VAContextID  trace_context; /* current context */

    VASurfaceID  trace_rendertarget; /* current render target */
    VAProfile trace_profile; /* current profile for buffers */
    VAEntrypoint trace_entrypoint; /* current entrypoint */

    unsigned int trace_frame_no; /* current frame NO */
    unsigned int trace_slice_no; /* current slice NO */
    unsigned int trace_slice_size; /* current slice buffer size */

    unsigned int trace_surface_width; /* surface dumping geometry */
    unsigned int trace_surface_height;
    unsigned int trace_surface_xoff;
    unsigned int trace_surface_yoff;

    unsigned int trace_frame_width; /* current frame width */
    unsigned int trace_frame_height; /* current frame height */
};

#define TRACE_CTX(dpy) ((struct trace_context *)((VADisplayContextP)dpy)->vatrace)

#define DPY2TRACECTX(dpy)                               \
    struct trace_context *trace_ctx = TRACE_CTX(dpy);   \
                                                        \
    if (trace_ctx == NULL)                              \
        return;                                         \

#define TRACE_FUNCNAME(idx)    va_TraceMsg(trace_ctx, "==========%s\n", __func__);

/* Prototype declarations (functions defined in va.c) */

void va_errorMessage(const char *msg, ...);
void va_infoMessage(const char *msg, ...);

int va_parseConfig(char *env, char *env_value);

VAStatus vaBufferInfo(
    VADisplay dpy,
    VAContextID context,        /* in */
    VABufferID buf_id,          /* in */
    VABufferType *type,         /* out */
    unsigned int *size,         /* out */
    unsigned int *num_elements  /* out */
    );

VAStatus vaLockSurface(VADisplay dpy,
                       VASurfaceID surface,
                       unsigned int *fourcc, /* following are output argument */
                       unsigned int *luma_stride,
                       unsigned int *chroma_u_stride,
                       unsigned int *chroma_v_stride,
                       unsigned int *luma_offset,
                       unsigned int *chroma_u_offset,
                       unsigned int *chroma_v_offset,
                       unsigned int *buffer_name,
                       void **buffer
                       );

VAStatus vaUnlockSurface(VADisplay dpy,
                         VASurfaceID surface
                         );

#define FILE_NAME_SUFFIX(env_value)                      \
do {                                                    \
    int tmp = strnlen(env_value, sizeof(env_value));    \
    int left = sizeof(env_value) - tmp;                 \
                                                        \
    snprintf(env_value+tmp,                             \
             left,                                      \
             ".%04d.%08lx",                             \
             suffix,                                    \
             (unsigned long)trace_ctx);                 \
} while (0)

void va_TraceInit(VADisplay dpy)
{
    char env_value[1024];
    unsigned short suffix = 0xffff & ((unsigned int)time(NULL));
    int trace_index = 0;
    FILE *tmp;
    struct trace_context *trace_ctx = calloc(sizeof(struct trace_context), 1);

    if (trace_ctx == NULL)
        return;

    if (va_parseConfig("LIBVA_TRACE", &env_value[0]) == 0) {
        FILE_NAME_SUFFIX(env_value);
        trace_ctx->trace_log_fn = strdup(env_value);

        tmp = fopen(env_value, "w");
        if (tmp) {
            trace_ctx->trace_fp_log = tmp;
            va_infoMessage("LIBVA_TRACE is on, save log into %s\n", trace_ctx->trace_log_fn);
            trace_flag = VA_TRACE_FLAG_LOG;
        } else
            va_errorMessage("Open file %s failed (%s)\n", env_value, strerror(errno));
    }

    /* may re-get the global settings for multiple context */
    if ((trace_flag & VA_TRACE_FLAG_LOG) && (va_parseConfig("LIBVA_TRACE_BUFDATA", NULL) == 0)) {
        trace_flag |= VA_TRACE_FLAG_BUFDATA;
        va_infoMessage("LIBVA_TRACE_BUFDATA is on, dump buffer into log file\n");
    }

    /* per-context setting */
    if (va_parseConfig("LIBVA_TRACE_CODEDBUF", &env_value[0]) == 0) {
        FILE_NAME_SUFFIX(env_value);
        trace_ctx->trace_codedbuf_fn = strdup(env_value);
        va_infoMessage("LIBVA_TRACE_CODEDBUF is on, save codedbuf into log file %s\n",
                       trace_ctx->trace_codedbuf_fn);
        trace_flag |= VA_TRACE_FLAG_CODEDBUF;
    }

    if (va_parseConfig("LIBVA_TRACE_SURFACE", &env_value[0]) == 0) {
        FILE_NAME_SUFFIX(env_value);
        trace_ctx->trace_surface_fn = strdup(env_value);

        va_infoMessage("LIBVA_TRACE_SURFACE is on, save surface into %s\n",
                       trace_ctx->trace_surface_fn);

        /* for surface data dump, it is time-consume, and may
         * cause some side-effect, so only trace the needed surfaces
         * to trace encode surface, set the trace file name to sth like *enc*
         * to trace decode surface, set the trace file name to sth like *dec*
         * if no dec/enc in file name, set both
         */
        if (strstr(env_value, "dec"))
            trace_flag |= VA_TRACE_FLAG_SURFACE_DECODE;
        if (strstr(env_value, "enc"))
            trace_flag |= VA_TRACE_FLAG_SURFACE_ENCODE;
        if (strstr(env_value, "jpeg") || strstr(env_value, "jpg"))
            trace_flag |= VA_TRACE_FLAG_SURFACE_JPEG;

        if (va_parseConfig("LIBVA_TRACE_SURFACE_GEOMETRY", &env_value[0]) == 0) {
            char *p = env_value, *q;

            trace_ctx->trace_surface_width = strtod(p, &q);
            p = q+1; /* skip "x" */
            trace_ctx->trace_surface_height = strtod(p, &q);
            p = q+1; /* skip "+" */
            trace_ctx->trace_surface_xoff = strtod(p, &q);
            p = q+1; /* skip "+" */
            trace_ctx->trace_surface_yoff = strtod(p, &q);

            va_infoMessage("LIBVA_TRACE_SURFACE_GEOMETRY is on, only dump surface %dx%d+%d+%d content\n",
                           trace_ctx->trace_surface_width,
                           trace_ctx->trace_surface_height,
                           trace_ctx->trace_surface_xoff,
                           trace_ctx->trace_surface_yoff);
        }
    }

    ((VADisplayContextP)dpy)->vatrace = trace_ctx;
}


void va_TraceEnd(VADisplay dpy)
{
    DPY2TRACECTX(dpy);

    if (trace_ctx->trace_fp_log)
        fclose(trace_ctx->trace_fp_log);

    if (trace_ctx->trace_fp_codedbuf)
        fclose(trace_ctx->trace_fp_codedbuf);

    if (trace_ctx->trace_fp_surface)
        fclose(trace_ctx->trace_fp_surface);

    if (trace_ctx->trace_log_fn)
        free(trace_ctx->trace_log_fn);

    if (trace_ctx->trace_codedbuf_fn)
        free(trace_ctx->trace_codedbuf_fn);

    if (trace_ctx->trace_surface_fn)
        free(trace_ctx->trace_surface_fn);

    free(trace_ctx);
    ((VADisplayContextP)dpy)->vatrace = NULL;
}


void va_TraceMsg(struct trace_context *trace_ctx, const char *msg, ...)
{
    va_list args;

    if (!(trace_flag & VA_TRACE_FLAG_LOG))
        return;

    if (msg)  {
        struct timeval tv;

        if (gettimeofday(&tv, NULL) == 0)
            fprintf(trace_ctx->trace_fp_log, "[%04d.%06d] ",
                    (unsigned int)tv.tv_sec & 0xffff, (unsigned int)tv.tv_usec);
        va_start(args, msg);
        vfprintf(trace_ctx->trace_fp_log, msg, args);
        va_end(args);
    } else
        fflush(trace_ctx->trace_fp_log);
}


void va_TraceSurface(VADisplay dpy)
{
    unsigned int i, j;
    unsigned int fourcc; /* following are output argument */
    unsigned int luma_stride;
    unsigned int chroma_u_stride;
    unsigned int chroma_v_stride;
    unsigned int luma_offset;
    unsigned int chroma_u_offset;
    unsigned int chroma_v_offset;
    unsigned int buffer_name;
    void *buffer = NULL;
    unsigned char *Y_data, *UV_data, *tmp;
    VAStatus va_status;
    unsigned char check_sum = 0;
    DPY2TRACECTX(dpy);

    if (!trace_ctx->trace_fp_surface)
        return;

    va_TraceMsg(trace_ctx, "==========dump surface data in file %s\n", trace_ctx->trace_surface_fn);

    va_TraceMsg(trace_ctx, NULL);

    va_status = vaLockSurface(
        dpy,
        trace_ctx->trace_rendertarget,
        &fourcc,
        &luma_stride, &chroma_u_stride, &chroma_v_stride,
        &luma_offset, &chroma_u_offset, &chroma_v_offset,
        &buffer_name, &buffer);

    if (va_status != VA_STATUS_SUCCESS) {
        va_TraceMsg(trace_ctx, "Error:vaLockSurface failed\n");
        return;
    }

    va_TraceMsg(trace_ctx, "\tfourcc = 0x%08x\n", fourcc);
    va_TraceMsg(trace_ctx, "\twidth = %d\n", trace_ctx->trace_frame_width);
    va_TraceMsg(trace_ctx, "\theight = %d\n", trace_ctx->trace_frame_height);
    va_TraceMsg(trace_ctx, "\tluma_stride = %d\n", luma_stride);
    va_TraceMsg(trace_ctx, "\tchroma_u_stride = %d\n", chroma_u_stride);
    va_TraceMsg(trace_ctx, "\tchroma_v_stride = %d\n", chroma_v_stride);
    va_TraceMsg(trace_ctx, "\tluma_offset = %d\n", luma_offset);
    va_TraceMsg(trace_ctx, "\tchroma_u_offset = %d\n", chroma_u_offset);
    va_TraceMsg(trace_ctx, "\tchroma_v_offset = %d\n", chroma_v_offset);

    if (buffer == NULL) {
        va_TraceMsg(trace_ctx, "Error:vaLockSurface return NULL buffer\n");
        va_TraceMsg(trace_ctx, NULL);

        vaUnlockSurface(dpy, trace_ctx->trace_rendertarget);
        return;
    }
    va_TraceMsg(trace_ctx, "\tbuffer location = 0x%08x\n", buffer);
    va_TraceMsg(trace_ctx, NULL);

    Y_data = (unsigned char*)buffer;
    UV_data = (unsigned char*)buffer + chroma_u_offset;

    tmp = Y_data + luma_stride * trace_ctx->trace_surface_yoff;
    for (i=0; i<trace_ctx->trace_surface_height; i++) {
        fwrite(tmp + trace_ctx->trace_surface_xoff,
               trace_ctx->trace_surface_width,
               1, trace_ctx->trace_fp_surface);

        tmp += luma_stride;
    }
    tmp = UV_data + chroma_u_stride * trace_ctx->trace_surface_yoff / 2;
    if (fourcc == VA_FOURCC_NV12) {
        for (i=0; i<trace_ctx->trace_surface_height/2; i++) {
            fwrite(tmp + trace_ctx->trace_surface_xoff,
                   trace_ctx->trace_surface_width,
                   1, trace_ctx->trace_fp_surface);

            tmp += chroma_u_stride;
        }
    }

    vaUnlockSurface(dpy, trace_ctx->trace_rendertarget);

    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceInitialize (
    VADisplay dpy,
    int *major_version,     /* out */
    int *minor_version      /* out */
)
{
    DPY2TRACECTX(dpy);
    TRACE_FUNCNAME(idx);
}

void va_TraceTerminate (
    VADisplay dpy
)
{
    DPY2TRACECTX(dpy);
    TRACE_FUNCNAME(idx);
}


void va_TraceCreateConfig(
    VADisplay dpy,
    VAProfile profile,
    VAEntrypoint entrypoint,
    VAConfigAttrib *attrib_list,
    int num_attribs,
    VAConfigID *config_id /* out */
)
{
    int i;
    int encode, decode, jpeg;
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\tprofile = %d\n", profile);
    va_TraceMsg(trace_ctx, "\tentrypoint = %d\n", entrypoint);
    va_TraceMsg(trace_ctx, "\tnum_attribs = %d\n", num_attribs);
    if (attrib_list) {
        for (i = 0; i < num_attribs; i++) {
            va_TraceMsg(trace_ctx, "\t\tattrib_list[%d].type = 0x%08x\n", i, attrib_list[i].type);
            va_TraceMsg(trace_ctx, "\t\tattrib_list[%d].value = 0x%08x\n", i, attrib_list[i].value);
        }
    }
    va_TraceMsg(trace_ctx, NULL);

    trace_ctx->trace_profile = profile;
    trace_ctx->trace_entrypoint = entrypoint;

    /* avoid to create so many empty files */
    encode = (trace_ctx->trace_entrypoint == VAEntrypointEncSlice);
    decode = (trace_ctx->trace_entrypoint == VAEntrypointVLD);
    jpeg = (trace_ctx->trace_entrypoint == VAEntrypointEncPicture);
    if ((encode && (trace_flag & VA_TRACE_FLAG_SURFACE_ENCODE)) ||
        (decode && (trace_flag & VA_TRACE_FLAG_SURFACE_DECODE)) ||
        (jpeg && (trace_flag & VA_TRACE_FLAG_SURFACE_JPEG))) {
        FILE *tmp = fopen(trace_ctx->trace_surface_fn, "w");

        if (tmp)
            trace_ctx->trace_fp_surface = tmp;
        else {
            va_errorMessage("Open file %s failed (%s)\n",
                            trace_ctx->trace_surface_fn,
                            strerror(errno));
            trace_ctx->trace_fp_surface = NULL;
            trace_flag &= ~(VA_TRACE_FLAG_SURFACE);
        }
    }

    if (encode && (trace_flag & VA_TRACE_FLAG_CODEDBUF)) {
        FILE *tmp = fopen(trace_ctx->trace_codedbuf_fn, "w");

        if (tmp)
            trace_ctx->trace_fp_codedbuf = tmp;
        else {
            va_errorMessage("Open file %s failed (%s)\n",
                            trace_ctx->trace_codedbuf_fn,
                            strerror(errno));
            trace_ctx->trace_fp_codedbuf = NULL;
            trace_flag &= ~VA_TRACE_FLAG_CODEDBUF;
        }
    }
}

static void va_TraceSurfaceAttributes(
    struct trace_context *trace_ctx,
    VASurfaceAttrib    *attrib_list,
    unsigned int       *num_attribs
)
{
    int i, num;
    VASurfaceAttrib *p;

    if (!attrib_list || !num_attribs)
        return;

    p = attrib_list;
    num = *num_attribs;
    if (num > VASurfaceAttribCount)
        num = VASurfaceAttribCount;

    for (i=0; i<num; i++) {
        int type = p->value.type;

        va_TraceMsg(trace_ctx, "\tattrib_list[%i] =\n", i);

        va_TraceMsg(trace_ctx, "\t\ttype = %d\n", p->type);
        va_TraceMsg(trace_ctx, "\t\tflags = %d\n", p->flags);
        va_TraceMsg(trace_ctx, "\t\tvalue.type = %d\n", type);
        switch (type) {
        case VAGenericValueTypeInteger:
            va_TraceMsg(trace_ctx, "\t\tvalue.value.i = 0x%08x\n", p->value.value.i);
            break;
        case VAGenericValueTypeFloat:
            va_TraceMsg(trace_ctx, "\t\tvalue.value.f = %f\n", p->value.value.f);
            break;
        case VAGenericValueTypePointer:
            va_TraceMsg(trace_ctx, "\t\tvalue.value.p = %p\n", p->value.value.p);
            if ((p->type == VASurfaceAttribExternalBufferDescriptor) && p->value.value.p) {
                VASurfaceAttribExternalBuffers *tmp = (VASurfaceAttribExternalBuffers *) p->value.value.p;
                int j;

                va_TraceMsg(trace_ctx, "\t\t--VASurfaceAttribExternalBufferDescriptor\n");
                va_TraceMsg(trace_ctx, "\t\t  pixel_format=0x%08x\n", tmp->pixel_format);
                va_TraceMsg(trace_ctx, "\t\t  width=%d\n", tmp->width);
                va_TraceMsg(trace_ctx, "\t\t  height=%d\n", tmp->height);
                va_TraceMsg(trace_ctx, "\t\t  data_size=%d\n", tmp->data_size);
                va_TraceMsg(trace_ctx, "\t\t  num_planes=%d\n", tmp->num_planes);
                va_TraceMsg(trace_ctx, "\t\t  pitches[4]=%d %d %d %d\n",
                            tmp->pitches[0], tmp->pitches[1], tmp->pitches[2], tmp->pitches[3]);
                va_TraceMsg(trace_ctx, "\t\t  offsets[4]=%d %d %d %d\n",
                            tmp->offsets[0], tmp->offsets[1], tmp->offsets[2], tmp->offsets[3]);
                va_TraceMsg(trace_ctx, "\t\t  flags=0x%08x\n", tmp->flags);
                va_TraceMsg(trace_ctx, "\t\t  num_buffers=0x%08x\n", tmp->num_buffers);
                va_TraceMsg(trace_ctx, "\t\t  buffers=%p\n", tmp->buffers);
                for (j = 0; j < tmp->num_buffers; j++) {
                    va_TraceMsg(trace_ctx, "\t\t\tbuffers[%d]=%p\n", j, tmp->buffers[j]);
                }
            }
            break;
        case VAGenericValueTypeFunc:
            va_TraceMsg(trace_ctx, "\t\tvalue.value.fn = %p\n", p->value.value.fn);
            break;
        default:
            break;
        }

        p++;
    }
}

void va_TraceCreateSurfaces(
    VADisplay dpy,
    int width,
    int height,
    int format,
    int num_surfaces,
    VASurfaceID *surfaces,    /* out */
    VASurfaceAttrib    *attrib_list,
    unsigned int        num_attribs
)
{
    int i;
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\twidth = %d\n", width);
    va_TraceMsg(trace_ctx, "\theight = %d\n", height);
    va_TraceMsg(trace_ctx, "\tformat = %d\n", format);
    va_TraceMsg(trace_ctx, "\tnum_surfaces = %d\n", num_surfaces);

    if (surfaces) {
        for (i = 0; i < num_surfaces; i++)
            va_TraceMsg(trace_ctx, "\t\tsurfaces[%d] = 0x%08x\n", i, surfaces[i]);
    }

    va_TraceSurfaceAttributes(trace_ctx, attrib_list, &num_attribs);

    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceDestroySurfaces(
    VADisplay dpy,
    VASurfaceID *surface_list,
    int num_surfaces
)
{
    int i;
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    if (surface_list) {
        for (i = 0; i < num_surfaces; i++)
            va_TraceMsg(trace_ctx, "\t\tsurfaces[%d] = 0x%08x\n", i, surface_list[i]);
    }

    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceCreateContext(
    VADisplay dpy,
    VAConfigID config_id,
    int picture_width,
    int picture_height,
    int flag,
    VASurfaceID *render_targets,
    int num_render_targets,
    VAContextID *context        /* out */
)
{
    int i;
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\tconfig = 0x%08x\n", config_id);
    va_TraceMsg(trace_ctx, "\twidth = %d\n", picture_width);
    va_TraceMsg(trace_ctx, "\theight = %d\n", picture_height);
    va_TraceMsg(trace_ctx, "\tflag = 0x%08x\n", flag);
    va_TraceMsg(trace_ctx, "\tnum_render_targets = %d\n", num_render_targets);
    if (render_targets) {
        for (i=0; i<num_render_targets; i++)
            va_TraceMsg(trace_ctx, "\t\trender_targets[%d] = 0x%08x\n", i, render_targets[i]);
    }
    if (context) {
        va_TraceMsg(trace_ctx, "\tcontext = 0x%08x\n", *context);
        trace_ctx->trace_context = *context;
    } else
        trace_ctx->trace_context = VA_INVALID_ID;

    trace_ctx->trace_frame_no = 0;
    trace_ctx->trace_slice_no = 0;

    trace_ctx->trace_frame_width = picture_width;
    trace_ctx->trace_frame_height = picture_height;

    if (trace_ctx->trace_surface_width == 0)
        trace_ctx->trace_surface_width = picture_width;
    if (trace_ctx->trace_surface_height == 0)
        trace_ctx->trace_surface_height = picture_height;
}


static char * buffer_type_to_string(int type)
{
    switch (type) {
    case VAPictureParameterBufferType: return "VAPictureParameterBufferType";
    case VAIQMatrixBufferType: return "VAIQMatrixBufferType";
    case VABitPlaneBufferType: return "VABitPlaneBufferType";
    case VASliceGroupMapBufferType: return "VASliceGroupMapBufferType";
    case VASliceParameterBufferType: return "VASliceParameterBufferType";
    case VASliceDataBufferType: return "VASliceDataBufferType";
    case VAProtectedSliceDataBufferType: return "VAProtectedSliceDataBufferType";
    case VAMacroblockParameterBufferType: return "VAMacroblockParameterBufferType";
    case VAResidualDataBufferType: return "VAResidualDataBufferType";
    case VADeblockingParameterBufferType: return "VADeblockingParameterBufferType";
    case VAImageBufferType: return "VAImageBufferType";
    case VAQMatrixBufferType: return "VAQMatrixBufferType";
    case VAHuffmanTableBufferType: return "VAHuffmanTableBufferType";
/* Following are encode buffer types */
    case VAEncCodedBufferType: return "VAEncCodedBufferType";
    case VAEncSequenceParameterBufferType: return "VAEncSequenceParameterBufferType";
    case VAEncPictureParameterBufferType: return "VAEncPictureParameterBufferType";
    case VAEncSliceParameterBufferType: return "VAEncSliceParameterBufferType";
    case VAEncPackedHeaderParameterBufferType: return "VAEncPackedHeaderParameterBufferType";
    case VAEncPackedHeaderDataBufferType: return "VAEncPackedHeaderDataBufferType";
    case VAEncMiscParameterBufferType: return "VAEncMiscParameterBufferType";
    case VAEncMacroblockParameterBufferType: return "VAEncMacroblockParameterBufferType";
    case VAProcPipelineParameterBufferType: return "VAProcPipelineParameterBufferType";
    case VAProcFilterParameterBufferType: return "VAProcFilterParameterBufferType";
    default: return "UnknowBuffer";
    }
}

void va_TraceCreateBuffer (
    VADisplay dpy,
    VAContextID context,	/* in */
    VABufferType type,		/* in */
    unsigned int size,		/* in */
    unsigned int num_elements,	/* in */
    void *data,			/* in */
    VABufferID *buf_id		/* out */
)
{
    DPY2TRACECTX(dpy);

    /* only trace CodedBuffer */
    if (type != VAEncCodedBufferType)
        return;

    TRACE_FUNCNAME(idx);
    va_TraceMsg(trace_ctx, "\tbuf_type=%s\n", buffer_type_to_string(type));
    if (buf_id)
        va_TraceMsg(trace_ctx, "\tbuf_id=0x%x\n", *buf_id);
    va_TraceMsg(trace_ctx, "\tsize=%d\n", size);
    va_TraceMsg(trace_ctx, "\tnum_elements=%d\n", num_elements);

    va_TraceMsg(trace_ctx, NULL);
}

void va_TraceDestroyBuffer (
    VADisplay dpy,
    VABufferID buf_id    /* in */
)
{
    VABufferType type;
    unsigned int size;
    unsigned int num_elements;

    VACodedBufferSegment *buf_list;
    int i = 0;

    DPY2TRACECTX(dpy);

    vaBufferInfo(dpy, trace_ctx->trace_context, buf_id, &type, &size, &num_elements);

    /* only trace CodedBuffer */
    if (type != VAEncCodedBufferType)
        return;

    TRACE_FUNCNAME(idx);
    va_TraceMsg(trace_ctx, "\tbuf_type=%s\n", buffer_type_to_string(type));
    va_TraceMsg(trace_ctx, "\tbuf_id=0x%x\n", buf_id);
    va_TraceMsg(trace_ctx, "\tsize=%d\n", size);
    va_TraceMsg(trace_ctx, "\tnum_elements=%d\n", num_elements);

    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceMapBuffer (
    VADisplay dpy,
    VABufferID buf_id,    /* in */
    void **pbuf           /* out */
)
{
    VABufferType type;
    unsigned int size;
    unsigned int num_elements;
    
    VACodedBufferSegment *buf_list;
    int i = 0;
    
    DPY2TRACECTX(dpy);

    vaBufferInfo(dpy, trace_ctx->trace_context, buf_id, &type, &size, &num_elements);    
    
    /* only trace CodedBuffer */
    if (type != VAEncCodedBufferType)
        return;

    TRACE_FUNCNAME(idx);
    va_TraceMsg(trace_ctx, "\tbuf_id=0x%x\n", buf_id);
    va_TraceMsg(trace_ctx, "\tbuf_type=%s\n", buffer_type_to_string(type));
    if ((pbuf == NULL) || (*pbuf == NULL))
        return;
    
    buf_list = (VACodedBufferSegment *)(*pbuf);
    while (buf_list != NULL) {
        va_TraceMsg(trace_ctx, "\tCodedbuf[%d] =\n", i++);
        
        va_TraceMsg(trace_ctx, "\t   size = %d\n", buf_list->size);
        va_TraceMsg(trace_ctx, "\t   bit_offset = %d\n", buf_list->bit_offset);
        va_TraceMsg(trace_ctx, "\t   status = 0x%08x\n", buf_list->status);
        va_TraceMsg(trace_ctx, "\t   reserved = 0x%08x\n", buf_list->reserved);
        va_TraceMsg(trace_ctx, "\t   buf = 0x%08x\n", buf_list->buf);

        if (trace_ctx->trace_fp_codedbuf) {
            va_TraceMsg(trace_ctx, "\tDump the content to file\n");
            fwrite(buf_list->buf, buf_list->size, 1, trace_ctx->trace_fp_codedbuf);
        }
        
        buf_list = buf_list->next;
    }
    va_TraceMsg(trace_ctx, NULL);
}

static void va_TraceVABuffers(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    unsigned int i;
    unsigned char *p = pbuf;

    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "--%s\n",  buffer_type_to_string(type));

    if ((trace_flag & VA_TRACE_FLAG_BUFDATA) && trace_ctx->trace_fp_log) {
        for (i=0; i<size; i++) {
            unsigned char value =  p[i];

            if (i==0)
                fprintf(trace_ctx->trace_fp_log, "\t\t0x%04x:", i);
            else if ((i%16) == 0)
                fprintf(trace_ctx->trace_fp_log, "\n\t\t0x%04x:", i);

            fprintf(trace_ctx->trace_fp_log, " %02x", value);
        }
        fprintf(trace_ctx->trace_fp_log, "\n");
    }
    
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAPictureParameterBufferMPEG2(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAPictureParameterBufferMPEG2 *p=(VAPictureParameterBufferMPEG2 *)data;
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx,"VAPictureParameterBufferMPEG2\n");

    va_TraceMsg(trace_ctx,"\thorizontal size= %d\n", p->horizontal_size);
    va_TraceMsg(trace_ctx,"\tvertical size= %d\n", p->vertical_size);
    va_TraceMsg(trace_ctx,"\tforward reference picture= %d\n", p->forward_reference_picture);
    va_TraceMsg(trace_ctx,"\tbackward reference picture= %d\n", p->backward_reference_picture);
    va_TraceMsg(trace_ctx,"\tpicture coding type= %d\n", p->picture_coding_type);
    va_TraceMsg(trace_ctx,"\tf mode= %d\n", p->f_code);

    va_TraceMsg(trace_ctx,"\tpicture coding extension = %d\n", p->picture_coding_extension.value);
    va_TraceMsg(trace_ctx,"\tintra_dc_precision= %d\n", p->picture_coding_extension.bits.intra_dc_precision);
    va_TraceMsg(trace_ctx,"\tpicture_structure= %d\n", p->picture_coding_extension.bits.picture_structure);
    va_TraceMsg(trace_ctx,"\ttop_field_first= %d\n", p->picture_coding_extension.bits.top_field_first);
    va_TraceMsg(trace_ctx,"\tframe_pred_frame_dct= %d\n", p->picture_coding_extension.bits.frame_pred_frame_dct);
    va_TraceMsg(trace_ctx,"\tconcealment_motion_vectors= %d\n", p->picture_coding_extension.bits.concealment_motion_vectors);
    va_TraceMsg(trace_ctx,"\tq_scale_type= %d\n", p->picture_coding_extension.bits.q_scale_type);
    va_TraceMsg(trace_ctx,"\tintra_vlc_format= %d\n", p->picture_coding_extension.bits.intra_vlc_format);
    va_TraceMsg(trace_ctx,"\talternate_scan= %d\n", p->picture_coding_extension.bits.alternate_scan);
    va_TraceMsg(trace_ctx,"\trepeat_first_field= %d\n", p->picture_coding_extension.bits.repeat_first_field);
    va_TraceMsg(trace_ctx,"\tprogressive_frame= %d\n", p->picture_coding_extension.bits.progressive_frame);
    va_TraceMsg(trace_ctx,"\tis_first_field= %d\n", p->picture_coding_extension.bits.is_first_field);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAIQMatrixBufferMPEG2(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAIQMatrixBufferMPEG2 *p=(VAIQMatrixBufferMPEG2 *)data;
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx,"VAIQMatrixBufferMPEG2\n");

    va_TraceMsg(trace_ctx,"\tload_intra_quantiser_matrix = %d\n", p->load_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tload_non_intra_quantiser_matrix = %d\n", p->load_non_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tload_chroma_intra_quantiser_matrix = %d\n", p->load_chroma_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tload_chroma_non_intra_quantiser_matrix = %d\n", p->load_chroma_non_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tintra_quantiser_matrix = %d\n", p->intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tnon_intra_quantiser_matrix = %d\n", p->non_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tchroma_intra_quantiser_matrix = %d\n", p->chroma_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx,"\tchroma_non_intra_quantiser_matrix = %d\n", p->chroma_non_intra_quantiser_matrix);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVASliceParameterBufferMPEG2(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VASliceParameterBufferMPEG2 *p=(VASliceParameterBufferMPEG2 *)data;

    DPY2TRACECTX(dpy);

    trace_ctx->trace_slice_no++;
    
    trace_ctx->trace_slice_size = p->slice_data_size;

    va_TraceMsg(trace_ctx,"VASliceParameterBufferMPEG2\n");

    va_TraceMsg(trace_ctx,"\tslice_data_size = %d\n", p->slice_data_size);
    va_TraceMsg(trace_ctx,"\tslice_data_offset = %d\n", p->slice_data_offset);
    va_TraceMsg(trace_ctx,"\tslice_data_flag = %d\n", p->slice_data_flag);
    va_TraceMsg(trace_ctx,"\tmacroblock_offset = %d\n", p->macroblock_offset);
    va_TraceMsg(trace_ctx,"\tslice_horizontal_position = %d\n", p->slice_horizontal_position);
    va_TraceMsg(trace_ctx,"\tslice_vertical_position = %d\n", p->slice_vertical_position);
    va_TraceMsg(trace_ctx,"\tquantiser_scale_code = %d\n", p->quantiser_scale_code);
    va_TraceMsg(trace_ctx,"\tintra_slice_flag = %d\n", p->intra_slice_flag);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAPictureParameterBufferJPEG(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i;
    VAPictureParameterBufferJPEGBaseline *p=(VAPictureParameterBufferJPEGBaseline *)data;
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx,"*VAPictureParameterBufferJPEG\n");
    va_TraceMsg(trace_ctx,"\tpicture_width = %u\n", p->picture_width);
    va_TraceMsg(trace_ctx,"\tpicture_height = %u\n", p->picture_height);
    va_TraceMsg(trace_ctx,"\tcomponents = \n");
    for (i = 0; i < p->num_components && i < 255; ++i) {
        va_TraceMsg(trace_ctx,"\t\t[%d] component_id = %u\n", i, p->components[i].component_id);
        va_TraceMsg(trace_ctx,"\t\t[%d] h_sampling_factor = %u\n", i, p->components[i].h_sampling_factor);
        va_TraceMsg(trace_ctx,"\t\t[%d] v_sampling_factor = %u\n", i, p->components[i].v_sampling_factor);
        va_TraceMsg(trace_ctx,"\t\t[%d] quantiser_table_selector = %u\n", i, p->components[i].quantiser_table_selector);
    }
}

static void va_TraceVAIQMatrixBufferJPEG(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i, j;
    static char tmp[1024];
    VAIQMatrixBufferJPEGBaseline *p=(VAIQMatrixBufferJPEGBaseline *)data;
    DPY2TRACECTX(dpy);
    va_TraceMsg(trace_ctx,"*VAIQMatrixParameterBufferJPEG\n");
    va_TraceMsg(trace_ctx,"\tload_quantiser_table =\n");
    for (i = 0; i < 4; ++i) {
        va_TraceMsg(trace_ctx,"\t\t[%d] = %u\n", i, p->load_quantiser_table[i]);
    }
    va_TraceMsg(trace_ctx,"\tquantiser_table =\n");
    for (i = 0; i < 4; ++i) {
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 64; ++j) {
            sprintf(tmp + strlen(tmp), "%u ", p->quantiser_table[i][j]);
        }
        va_TraceMsg(trace_ctx,"\t\t[%d] = %s\n", i, tmp);
    }
}

static void va_TraceVASliceParameterBufferJPEG(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i;
    VASliceParameterBufferJPEGBaseline *p=(VASliceParameterBufferJPEGBaseline *)data;
    DPY2TRACECTX(dpy);
    va_TraceMsg(trace_ctx,"*VASliceParameterBufferJPEG\n");
    va_TraceMsg(trace_ctx,"\tslice_data_size = %u\n", p->slice_data_size);
    va_TraceMsg(trace_ctx,"\tslice_data_offset = %u\n", p->slice_data_offset);
    va_TraceMsg(trace_ctx,"\tslice_data_flag = %u\n", p->slice_data_flag);
    va_TraceMsg(trace_ctx,"\tslice_horizontal_position = %u\n", p->slice_horizontal_position);
    va_TraceMsg(trace_ctx,"\tslice_vertical_position = %u\n", p->slice_vertical_position);
    va_TraceMsg(trace_ctx,"\tcomponents = \n");
    for (i = 0; i < p->num_components && i < 4; ++i) {
        va_TraceMsg(trace_ctx,"\t\t[%d] component_selector = %u\n", i, p->components[i].component_selector);
        va_TraceMsg(trace_ctx,"\t\t[%d] dc_table_selector = %u\n", i, p->components[i].dc_table_selector);
        va_TraceMsg(trace_ctx,"\t\t[%d] ac_table_selector = %u\n", i, p->components[i].ac_table_selector);
    }
    va_TraceMsg(trace_ctx,"\trestart_interval = %u\n", p->restart_interval);
    va_TraceMsg(trace_ctx,"\tnum_mcus = %u\n", p->num_mcus);
}

static void va_TraceVAHuffmanTableBufferJPEG(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i, j;
    static char tmp[1024];
    VAHuffmanTableBufferJPEGBaseline *p=(VAHuffmanTableBufferJPEGBaseline *)data;
    DPY2TRACECTX(dpy);
    va_TraceMsg(trace_ctx,"*VAHuffmanTableBufferJPEG\n");

    for (i = 0; i < 2; ++i) {
        va_TraceMsg(trace_ctx,"\tload_huffman_table[%d] =%u\n", i, p->load_huffman_table[0]);
        va_TraceMsg(trace_ctx,"\thuffman_table[%d] =\n", i);
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 16; ++j) {
            sprintf(tmp + strlen(tmp), "%u ", p->huffman_table[i].num_dc_codes[j]);
        }
        va_TraceMsg(trace_ctx,"\t\tnum_dc_codes =%s\n", tmp);
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 12; ++j) {
            sprintf(tmp + strlen(tmp), "%u ", p->huffman_table[i].dc_values[j]);
        }
        va_TraceMsg(trace_ctx,"\t\tdc_values =%s\n", tmp);
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 16; ++j) {
            sprintf(tmp + strlen(tmp), "%u ", p->huffman_table[i].num_ac_codes[j]);
        }
        va_TraceMsg(trace_ctx,"\t\tnum_dc_codes =%s\n", tmp);
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 162; ++j) {
            sprintf(tmp + strlen(tmp), "%u ", p->huffman_table[i].ac_values[j]);
        }
        va_TraceMsg(trace_ctx,"\t\tnum_dc_codes =%s\n", tmp);
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 2; ++j) {
            sprintf(tmp + strlen(tmp), "%u ", p->huffman_table[i].pad[j]);
        }
        va_TraceMsg(trace_ctx,"\t\tnum_dc_codes =%s\n", tmp);
    }
}

static void va_TraceVAPictureParameterBufferMPEG4(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i;
    VAPictureParameterBufferMPEG4 *p=(VAPictureParameterBufferMPEG4 *)data;
    
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx,"*VAPictureParameterBufferMPEG4\n");
    va_TraceMsg(trace_ctx,"\tvop_width = %d\n", p->vop_width);
    va_TraceMsg(trace_ctx,"\tvop_height = %d\n", p->vop_height);
    va_TraceMsg(trace_ctx,"\tforward_reference_picture = %d\n", p->forward_reference_picture);
    va_TraceMsg(trace_ctx,"\tbackward_reference_picture = %d\n", p->backward_reference_picture);
    va_TraceMsg(trace_ctx,"\tvol_fields value = %d\n", p->vol_fields.value);
    va_TraceMsg(trace_ctx,"\tshort_video_header= %d\n", p->vol_fields.bits.short_video_header);
    va_TraceMsg(trace_ctx,"\tchroma_format= %d\n", p->vol_fields.bits.chroma_format);
    va_TraceMsg(trace_ctx,"\tinterlaced= %d\n", p->vol_fields.bits.interlaced);
    va_TraceMsg(trace_ctx,"\tobmc_disable= %d\n", p->vol_fields.bits.obmc_disable);
    va_TraceMsg(trace_ctx,"\tsprite_enable= %d\n", p->vol_fields.bits.sprite_enable);
    va_TraceMsg(trace_ctx,"\tsprite_warping_accuracy= %d\n", p->vol_fields.bits.sprite_warping_accuracy);
    va_TraceMsg(trace_ctx,"\tquant_type= %d\n", p->vol_fields.bits.quant_type);
    va_TraceMsg(trace_ctx,"\tquarter_sample= %d\n", p->vol_fields.bits.quarter_sample);
    va_TraceMsg(trace_ctx,"\tdata_partitioned= %d\n", p->vol_fields.bits.data_partitioned);
    va_TraceMsg(trace_ctx,"\treversible_vlc= %d\n", p->vol_fields.bits.reversible_vlc);
    va_TraceMsg(trace_ctx,"\tresync_marker_disable= %d\n", p->vol_fields.bits.resync_marker_disable);
    va_TraceMsg(trace_ctx,"\tno_of_sprite_warping_points = %d\n", p->no_of_sprite_warping_points);
    va_TraceMsg(trace_ctx,"\tsprite_trajectory_du =");
    for(i=0;i<3;i++)
        va_TraceMsg(trace_ctx,"\t%d", p->sprite_trajectory_du[i]);

    va_TraceMsg(trace_ctx,"\n");
    va_TraceMsg(trace_ctx,"\tsprite_trajectory_dv =");
    for(i=0;i<3;i++)
        va_TraceMsg(trace_ctx,"\t%d", p->sprite_trajectory_dv[i]);
    va_TraceMsg(trace_ctx,"\n");
    va_TraceMsg(trace_ctx,"\tvop_fields value = %d\n", p->vop_fields.value);
    va_TraceMsg(trace_ctx,"\tvop_coding_type= %d\n", p->vop_fields.bits.vop_coding_type);
    va_TraceMsg(trace_ctx,"\tbackward_reference_vop_coding_type= %d\n", p->vop_fields.bits.backward_reference_vop_coding_type);
    va_TraceMsg(trace_ctx,"\tvop_rounding_type= %d\n", p->vop_fields.bits.vop_rounding_type);
    va_TraceMsg(trace_ctx,"\tintra_dc_vlc_thr= %d\n", p->vop_fields.bits.intra_dc_vlc_thr);
    va_TraceMsg(trace_ctx,"\ttop_field_first= %d\n", p->vop_fields.bits.top_field_first);
    va_TraceMsg(trace_ctx,"\talternate_vertical_scan_flag= %d\n", p->vop_fields.bits.alternate_vertical_scan_flag);
    va_TraceMsg(trace_ctx,"\tvop_fcode_forward = %d\n", p->vop_fcode_forward);
    va_TraceMsg(trace_ctx,"\tvop_fcode_backward = %d\n", p->vop_fcode_backward);
    va_TraceMsg(trace_ctx,"\tnum_gobs_in_vop = %d\n", p->num_gobs_in_vop);
    va_TraceMsg(trace_ctx,"\tnum_macroblocks_in_gob = %d\n", p->num_macroblocks_in_gob);
    va_TraceMsg(trace_ctx,"\tTRB = %d\n", p->TRB);
    va_TraceMsg(trace_ctx,"\tTRD = %d\n", p->TRD);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAIQMatrixBufferMPEG4(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i;
    VAIQMatrixBufferMPEG4 *p=(VAIQMatrixBufferMPEG4 *)data;
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx,"VAIQMatrixBufferMPEG4\n");

    va_TraceMsg(trace_ctx,"\tload_intra_quant_mat = %d\n", p->load_intra_quant_mat);
    va_TraceMsg(trace_ctx,"\tload_non_intra_quant_mat = %d\n", p->load_non_intra_quant_mat);
    va_TraceMsg(trace_ctx,"\tintra_quant_mat =\n");
    for(i=0;i<64;i++)
        va_TraceMsg(trace_ctx,"\t\t%d\n", p->intra_quant_mat[i]);

    va_TraceMsg(trace_ctx,"\tnon_intra_quant_mat =\n");
    for(i=0;i<64;i++)
        va_TraceMsg(trace_ctx,"\t\t%d\n", p->non_intra_quant_mat[i]);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncSequenceParameterBufferMPEG4(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncSequenceParameterBufferMPEG4 *p = (VAEncSequenceParameterBufferMPEG4 *)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAEncSequenceParameterBufferMPEG4\n");
    
    va_TraceMsg(trace_ctx, "\tprofile_and_level_indication = %d\n", p->profile_and_level_indication);
    va_TraceMsg(trace_ctx, "\tintra_period = %d\n", p->intra_period);
    va_TraceMsg(trace_ctx, "\tvideo_object_layer_width = %d\n", p->video_object_layer_width);
    va_TraceMsg(trace_ctx, "\tvideo_object_layer_height = %d\n", p->video_object_layer_height);
    va_TraceMsg(trace_ctx, "\tvop_time_increment_resolution = %d\n", p->vop_time_increment_resolution);
    va_TraceMsg(trace_ctx, "\tfixed_vop_rate = %d\n", p->fixed_vop_rate);
    va_TraceMsg(trace_ctx, "\tfixed_vop_time_increment = %d\n", p->fixed_vop_time_increment);
    va_TraceMsg(trace_ctx, "\tbits_per_second = %d\n", p->bits_per_second);
    va_TraceMsg(trace_ctx, "\tframe_rate = %d\n", p->frame_rate);
    va_TraceMsg(trace_ctx, "\tinitial_qp = %d\n", p->initial_qp);
    va_TraceMsg(trace_ctx, "\tmin_qp = %d\n", p->min_qp);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncPictureParameterBufferMPEG4(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncPictureParameterBufferMPEG4 *p = (VAEncPictureParameterBufferMPEG4 *)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAEncPictureParameterBufferMPEG4\n");
    va_TraceMsg(trace_ctx, "\treference_picture = 0x%08x\n", p->reference_picture);
    va_TraceMsg(trace_ctx, "\treconstructed_picture = 0x%08x\n", p->reconstructed_picture);
    va_TraceMsg(trace_ctx, "\tcoded_buf = 0x%08x\n", p->coded_buf);
    va_TraceMsg(trace_ctx, "\tpicture_width = %d\n", p->picture_width);
    va_TraceMsg(trace_ctx, "\tpicture_height = %d\n", p->picture_height);
    va_TraceMsg(trace_ctx, "\tmodulo_time_base = %d\n", p->modulo_time_base);
    va_TraceMsg(trace_ctx, "\tvop_time_increment = %d\n", p->vop_time_increment);
    va_TraceMsg(trace_ctx, "\tpicture_type = %d\n", p->picture_type);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVASliceParameterBufferMPEG4(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VASliceParameterBufferMPEG4 *p=(VASliceParameterBufferMPEG4 *)data;
    
    DPY2TRACECTX(dpy);

    trace_ctx->trace_slice_no++;

    trace_ctx->trace_slice_size = p->slice_data_size;

    va_TraceMsg(trace_ctx,"VASliceParameterBufferMPEG4\n");

    va_TraceMsg(trace_ctx,"\tslice_data_size = %d\n", p->slice_data_size);
    va_TraceMsg(trace_ctx,"\tslice_data_offset = %d\n", p->slice_data_offset);
    va_TraceMsg(trace_ctx,"\tslice_data_flag = %d\n", p->slice_data_flag);
    va_TraceMsg(trace_ctx,"\tmacroblock_offset = %d\n", p->macroblock_offset);
    va_TraceMsg(trace_ctx,"\tmacroblock_number = %d\n", p->macroblock_number);
    va_TraceMsg(trace_ctx,"\tquant_scale = %d\n", p->quant_scale);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static inline void va_TraceFlagIfNotZero(
    struct trace_context *trace_ctx,
    const char *name,   /* in */
    unsigned int flag   /* in */
)
{
    if (flag != 0) {
        va_TraceMsg(trace_ctx, "%s = %x\n", name, flag);
    }
}


static void va_TraceVAPictureParameterBufferH264(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i;
    VAPictureParameterBufferH264 *p = (VAPictureParameterBufferH264*)data;
    
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx, "\t--VAPictureParameterBufferH264\n");

    va_TraceMsg(trace_ctx, "\tCurrPic.picture_id = 0x%08x\n", p->CurrPic.picture_id);
    va_TraceMsg(trace_ctx, "\tCurrPic.frame_idx = %d\n", p->CurrPic.frame_idx);
    va_TraceMsg(trace_ctx, "\tCurrPic.flags = %d\n", p->CurrPic.flags);
    va_TraceMsg(trace_ctx, "\tCurrPic.TopFieldOrderCnt = %d\n", p->CurrPic.TopFieldOrderCnt);
    va_TraceMsg(trace_ctx, "\tCurrPic.BottomFieldOrderCnt = %d\n", p->CurrPic.BottomFieldOrderCnt);

    va_TraceMsg(trace_ctx, "\tReferenceFrames (TopFieldOrderCnt-BottomFieldOrderCnt-picture_id-frame_idx-flags:\n");
    for (i = 0; i < 16; i++)
    {
        if ((p->ReferenceFrames[i].picture_id != VA_INVALID_SURFACE) &&
            ((p->ReferenceFrames[i].flags & VA_PICTURE_H264_INVALID) == 0)) {
            va_TraceMsg(trace_ctx, "\t\t%08d-%08d-0x%08x-%08d-0x%08x\n",
                        p->ReferenceFrames[i].TopFieldOrderCnt,
                        p->ReferenceFrames[i].BottomFieldOrderCnt,
                        p->ReferenceFrames[i].picture_id,
                        p->ReferenceFrames[i].frame_idx,
                        p->ReferenceFrames[i].flags);
        } else
            break;
    }
    va_TraceMsg(trace_ctx, "\tpicture_width_in_mbs_minus1 = %d\n", p->picture_width_in_mbs_minus1);
    va_TraceMsg(trace_ctx, "\tpicture_height_in_mbs_minus1 = %d\n", p->picture_height_in_mbs_minus1);
    va_TraceMsg(trace_ctx, "\tbit_depth_luma_minus8 = %d\n", p->bit_depth_luma_minus8);
    va_TraceMsg(trace_ctx, "\tbit_depth_chroma_minus8 = %d\n", p->bit_depth_chroma_minus8);
    va_TraceMsg(trace_ctx, "\tnum_ref_frames = %d\n", p->num_ref_frames);
    va_TraceMsg(trace_ctx, "\tseq fields = %d\n", p->seq_fields.value);
    va_TraceMsg(trace_ctx, "\tchroma_format_idc = %d\n", p->seq_fields.bits.chroma_format_idc);
    va_TraceMsg(trace_ctx, "\tresidual_colour_transform_flag = %d\n", p->seq_fields.bits.residual_colour_transform_flag);
    va_TraceMsg(trace_ctx, "\tframe_mbs_only_flag = %d\n", p->seq_fields.bits.frame_mbs_only_flag);
    va_TraceMsg(trace_ctx, "\tmb_adaptive_frame_field_flag = %d\n", p->seq_fields.bits.mb_adaptive_frame_field_flag);
    va_TraceMsg(trace_ctx, "\tdirect_8x8_inference_flag = %d\n", p->seq_fields.bits.direct_8x8_inference_flag);
    va_TraceMsg(trace_ctx, "\tMinLumaBiPredSize8x8 = %d\n", p->seq_fields.bits.MinLumaBiPredSize8x8);
    va_TraceMsg(trace_ctx, "\tnum_slice_groups_minus1 = %d\n", p->num_slice_groups_minus1);
    va_TraceMsg(trace_ctx, "\tslice_group_map_type = %d\n", p->slice_group_map_type);
    va_TraceMsg(trace_ctx, "\tslice_group_change_rate_minus1 = %d\n", p->slice_group_change_rate_minus1);
    va_TraceMsg(trace_ctx, "\tpic_init_qp_minus26 = %d\n", p->pic_init_qp_minus26);
    va_TraceMsg(trace_ctx, "\tpic_init_qs_minus26 = %d\n", p->pic_init_qs_minus26);
    va_TraceMsg(trace_ctx, "\tchroma_qp_index_offset = %d\n", p->chroma_qp_index_offset);
    va_TraceMsg(trace_ctx, "\tsecond_chroma_qp_index_offset = %d\n", p->second_chroma_qp_index_offset);
    va_TraceMsg(trace_ctx, "\tpic_fields = 0x%03x\n", p->pic_fields.value);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tentropy_coding_mode_flag", p->pic_fields.bits.entropy_coding_mode_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tweighted_pred_flag", p->pic_fields.bits.weighted_pred_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tweighted_bipred_idc", p->pic_fields.bits.weighted_bipred_idc);
    va_TraceFlagIfNotZero(trace_ctx, "\t\ttransform_8x8_mode_flag", p->pic_fields.bits.transform_8x8_mode_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tfield_pic_flag", p->pic_fields.bits.field_pic_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tconstrained_intra_pred_flag", p->pic_fields.bits.constrained_intra_pred_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tpic_order_present_flag", p->pic_fields.bits.pic_order_present_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tdeblocking_filter_control_present_flag", p->pic_fields.bits.deblocking_filter_control_present_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\tredundant_pic_cnt_present_flag", p->pic_fields.bits.redundant_pic_cnt_present_flag);
    va_TraceFlagIfNotZero(trace_ctx, "\t\treference_pic_flag", p->pic_fields.bits.reference_pic_flag);
    va_TraceMsg(trace_ctx, "\tframe_num = %d\n", p->frame_num);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVASliceParameterBufferH264(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    int i;
    VASliceParameterBufferH264* p = (VASliceParameterBufferH264*)data;
    DPY2TRACECTX(dpy);

    trace_ctx->trace_slice_no++;
    trace_ctx->trace_slice_size = p->slice_data_size;

    va_TraceMsg(trace_ctx, "\t--VASliceParameterBufferH264\n");
    va_TraceMsg(trace_ctx, "\tslice_data_size = %d\n", p->slice_data_size);
    va_TraceMsg(trace_ctx, "\tslice_data_offset = %d\n", p->slice_data_offset);
    va_TraceMsg(trace_ctx, "\tslice_data_flag = %d\n", p->slice_data_flag);
    va_TraceMsg(trace_ctx, "\tslice_data_bit_offset = %d\n", p->slice_data_bit_offset);
    va_TraceMsg(trace_ctx, "\tfirst_mb_in_slice = %d\n", p->first_mb_in_slice);
    va_TraceMsg(trace_ctx, "\tslice_type = %d\n", p->slice_type);
    va_TraceMsg(trace_ctx, "\tdirect_spatial_mv_pred_flag = %d\n", p->direct_spatial_mv_pred_flag);
    va_TraceMsg(trace_ctx, "\tnum_ref_idx_l0_active_minus1 = %d\n", p->num_ref_idx_l0_active_minus1);
    va_TraceMsg(trace_ctx, "\tnum_ref_idx_l1_active_minus1 = %d\n", p->num_ref_idx_l1_active_minus1);
    va_TraceMsg(trace_ctx, "\tcabac_init_idc = %d\n", p->cabac_init_idc);
    va_TraceMsg(trace_ctx, "\tslice_qp_delta = %d\n", p->slice_qp_delta);
    va_TraceMsg(trace_ctx, "\tdisable_deblocking_filter_idc = %d\n", p->disable_deblocking_filter_idc);
    va_TraceMsg(trace_ctx, "\tslice_alpha_c0_offset_div2 = %d\n", p->slice_alpha_c0_offset_div2);
    va_TraceMsg(trace_ctx, "\tslice_beta_offset_div2 = %d\n", p->slice_beta_offset_div2);

    va_TraceMsg(trace_ctx, "\tRefPicList0 =\n");
    for (i = 0; i < 32; i++) {
        if ((p->RefPicList0[i].picture_id != VA_INVALID_SURFACE) &&
            ((p->RefPicList0[i].flags & VA_PICTURE_H264_INVALID) == 0))
        va_TraceMsg(trace_ctx, "%08d-%08d-0x%08x-%08d-0x%08x\n", p->RefPicList0[i].TopFieldOrderCnt, p->RefPicList0[i].BottomFieldOrderCnt, p->RefPicList0[i].picture_id, p->RefPicList0[i].frame_idx,  p->RefPicList0[i].flags);
        else
            break;
    }
    va_TraceMsg(trace_ctx, "\tRefPicList1 =\n");
    for (i = 0; i < 32; i++) {
        if ((p->RefPicList1[i].picture_id != VA_INVALID_SURFACE) &&
            ((p->RefPicList1[i].flags & VA_PICTURE_H264_INVALID) == 0))
            va_TraceMsg(trace_ctx, "%08d-%08d-0x%08x-%08d-0x%08x\n", p->RefPicList1[i].TopFieldOrderCnt, p->RefPicList1[i].BottomFieldOrderCnt, p->RefPicList1[i].picture_id, p->RefPicList1[i].frame_idx, p->RefPicList1[i].flags);
        else
            break;
    }

    va_TraceMsg(trace_ctx, "\tluma_log2_weight_denom = %d\n", p->luma_log2_weight_denom);
    va_TraceMsg(trace_ctx, "\tchroma_log2_weight_denom = %d\n", p->chroma_log2_weight_denom);
    va_TraceMsg(trace_ctx, "\tluma_weight_l0_flag = %d\n", p->luma_weight_l0_flag);

    for (i = 0; (i <= p->num_ref_idx_l0_active_minus1) && (i<32); i++) {
        va_TraceMsg(trace_ctx, "\t\t%d\t%d\n",
            p->luma_weight_l0[i],
            p->luma_offset_l0[i]);
    }


    va_TraceMsg(trace_ctx, "\tchroma_weight_l0_flag = %d\n", p->chroma_weight_l0_flag);

    for (i = 0; (i <= p->num_ref_idx_l0_active_minus1) && (i<32); i++) {
        va_TraceMsg(trace_ctx, "\t\t%d\t%d\t%d\t%d\n",
            p->chroma_weight_l0[i][0],
            p->chroma_offset_l0[i][0],
            p->chroma_weight_l0[i][1],
            p->chroma_offset_l0[i][1]);
    }


    va_TraceMsg(trace_ctx, "\tluma_weight_l1_flag = %d\n", p->luma_weight_l1_flag);

    for (i = 0; (i <=  p->num_ref_idx_l1_active_minus1) && (i<32); i++) {
        va_TraceMsg(trace_ctx, "\t\t%d\t%d\n",
            p->luma_weight_l1[i],
            p->luma_offset_l1[i]);
    }


    va_TraceMsg(trace_ctx, "\tchroma_weight_l1_flag = %d\n", p->chroma_weight_l1_flag);

    for (i = 0; (i <= p->num_ref_idx_l1_active_minus1) && (i<32); i++) {
        va_TraceMsg(trace_ctx, "\t\t%d\t%d\t%d\t%d\n",
            p->chroma_weight_l1[i][0],
            p->chroma_offset_l1[i][0],
            p->chroma_weight_l1[i][1],
            p->chroma_offset_l1[i][1]);

    }
    va_TraceMsg(trace_ctx, NULL);
}

static void va_TraceVAIQMatrixBufferH264(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data
)
{
    int i, j;
    VAIQMatrixBufferH264* p = (VAIQMatrixBufferH264* )data;

    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx, "\t--VAIQMatrixBufferH264\n");

    va_TraceMsg(trace_ctx, "\tScalingList4x4[6][16]=\n");
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 16; j++) {
            if (trace_ctx->trace_fp_log) {
                fprintf(trace_ctx->trace_fp_log, "\t%d", p->ScalingList4x4[i][j]);
                if ((j + 1) % 8 == 0)
                    fprintf(trace_ctx->trace_fp_log, "\n");
            }
        }
    }

    va_TraceMsg(trace_ctx, "\tScalingList8x8[2][64]=\n");
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 64; j++) {
            if (trace_ctx->trace_fp_log) {
                fprintf(trace_ctx->trace_fp_log,"\t%d", p->ScalingList8x8[i][j]);
                if ((j + 1) % 8 == 0)
                    fprintf(trace_ctx->trace_fp_log, "\n");
            }
        }
    }

    va_TraceMsg(trace_ctx, NULL);
}



static void va_TraceVAEncSequenceParameterBufferH264(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncSequenceParameterBufferH264 *p = (VAEncSequenceParameterBufferH264 *)data;
    DPY2TRACECTX(dpy);
    unsigned int i;

    va_TraceMsg(trace_ctx, "\t--VAEncSequenceParameterBufferH264\n");

    va_TraceMsg(trace_ctx, "\tseq_parameter_set_id = %d\n", p->seq_parameter_set_id);
    va_TraceMsg(trace_ctx, "\tlevel_idc = %d\n", p->level_idc);
    va_TraceMsg(trace_ctx, "\tintra_period = %d\n", p->intra_period);
    va_TraceMsg(trace_ctx, "\tintra_idr_period = %d\n", p->intra_idr_period);
    va_TraceMsg(trace_ctx, "\tip_period = %d\n", p->ip_period);
    va_TraceMsg(trace_ctx, "\tbits_per_second = %d\n", p->bits_per_second);
    va_TraceMsg(trace_ctx, "\tmax_num_ref_frames = %d\n", p->max_num_ref_frames);
    va_TraceMsg(trace_ctx, "\tpicture_width_in_mbs = %d\n", p->picture_width_in_mbs);
    va_TraceMsg(trace_ctx, "\tpicture_height_in_mbs = %d\n", p->picture_height_in_mbs);
    va_TraceMsg(trace_ctx, "\tchroma_format_idc = %d\n", p->seq_fields.bits.chroma_format_idc);
    va_TraceMsg(trace_ctx, "\tframe_mbs_only_flag = %d\n", p->seq_fields.bits.frame_mbs_only_flag);
    va_TraceMsg(trace_ctx, "\tmb_adaptive_frame_field_flag = %d\n", p->seq_fields.bits.mb_adaptive_frame_field_flag);
    va_TraceMsg(trace_ctx, "\tseq_scaling_matrix_present_flag = %d\n", p->seq_fields.bits.seq_scaling_matrix_present_flag);
    va_TraceMsg(trace_ctx, "\tdirect_8x8_inference_flag = %d\n", p->seq_fields.bits.direct_8x8_inference_flag);
    va_TraceMsg(trace_ctx, "\tlog2_max_frame_num_minus4 = %d\n", p->seq_fields.bits.log2_max_frame_num_minus4);
    va_TraceMsg(trace_ctx, "\tpic_order_cnt_type = %d\n", p->seq_fields.bits.pic_order_cnt_type);
    va_TraceMsg(trace_ctx, "\tlog2_max_pic_order_cnt_lsb_minus4 = %d\n", p->seq_fields.bits.log2_max_pic_order_cnt_lsb_minus4);
    va_TraceMsg(trace_ctx, "\tdelta_pic_order_always_zero_flag = %d\n", p->seq_fields.bits.delta_pic_order_always_zero_flag);
    va_TraceMsg(trace_ctx, "\tbit_depth_luma_minus8 = %d\n", p->bit_depth_luma_minus8);
    va_TraceMsg(trace_ctx, "\tbit_depth_chroma_minus8 = %d\n", p->bit_depth_chroma_minus8);
    va_TraceMsg(trace_ctx, "\tnum_ref_frames_in_pic_order_cnt_cycle = %d\n", p->num_ref_frames_in_pic_order_cnt_cycle);
    va_TraceMsg(trace_ctx, "\toffset_for_non_ref_pic = %d\n", p->offset_for_non_ref_pic);
    va_TraceMsg(trace_ctx, "\toffset_for_top_to_bottom_field = %d\n", p->offset_for_top_to_bottom_field);
    for(i = 0; (i < p->max_num_ref_frames) && (i < 32); ++i)
        va_TraceMsg(trace_ctx, "\toffset_for_ref_frame[%d] = %d\n", i, p->offset_for_ref_frame[i]);
    va_TraceMsg(trace_ctx, "\tframe_cropping_flag = %d\n", p->frame_cropping_flag);
    va_TraceMsg(trace_ctx, "\tframe_crop_left_offset = %d\n", p->frame_crop_left_offset);
    va_TraceMsg(trace_ctx, "\tframe_crop_right_offset = %d\n", p->frame_crop_right_offset);
    va_TraceMsg(trace_ctx, "\tframe_crop_top_offset = %d\n", p->frame_crop_top_offset);
    va_TraceMsg(trace_ctx, "\tframe_crop_bottom_offset = %d\n", p->frame_crop_bottom_offset);
    va_TraceMsg(trace_ctx, "\tvui_parameters_present_flag = %d\n", p->vui_parameters_present_flag);
    va_TraceMsg(trace_ctx, "\taspect_ratio_info_present_flag = %d\n", p->vui_fields.bits.aspect_ratio_info_present_flag);
    va_TraceMsg(trace_ctx, "\ttiming_info_present_flag = %d\n", p->vui_fields.bits.timing_info_present_flag);
    va_TraceMsg(trace_ctx, "\tbitstream_restriction_flag = %d\n", p->vui_fields.bits.bitstream_restriction_flag);
    va_TraceMsg(trace_ctx, "\tlog2_max_mv_length_horizontal = %d\n", p->vui_fields.bits.log2_max_mv_length_horizontal);
    va_TraceMsg(trace_ctx, "\tlog2_max_mv_length_vertical = %d\n", p->vui_fields.bits.log2_max_mv_length_vertical);
    va_TraceMsg(trace_ctx, "\taspect_ratio_idc = %d\n", p->aspect_ratio_idc);
    va_TraceMsg(trace_ctx, "\tsar_width = %d\n", p->sar_width);
    va_TraceMsg(trace_ctx, "\tsar_height = %d\n", p->sar_height);
    va_TraceMsg(trace_ctx, "\tnum_units_in_tick = %d\n", p->num_units_in_tick);
    va_TraceMsg(trace_ctx, "\ttime_scale = %d\n", p->time_scale);

    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAEncPictureParameterBufferH264(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncPictureParameterBufferH264 *p = (VAEncPictureParameterBufferH264 *)data;
    DPY2TRACECTX(dpy);
    int i;

    va_TraceMsg(trace_ctx, "\t--VAEncPictureParameterBufferH264\n");

    va_TraceMsg(trace_ctx, "\tCurrPic.picture_id = 0x%08x\n", p->CurrPic.picture_id);
    va_TraceMsg(trace_ctx, "\tCurrPic.frame_idx = %d\n", p->CurrPic.frame_idx);
    va_TraceMsg(trace_ctx, "\tCurrPic.flags = %d\n", p->CurrPic.flags);
    va_TraceMsg(trace_ctx, "\tCurrPic.TopFieldOrderCnt = %d\n", p->CurrPic.TopFieldOrderCnt);
    va_TraceMsg(trace_ctx, "\tCurrPic.BottomFieldOrderCnt = %d\n", p->CurrPic.BottomFieldOrderCnt);
    va_TraceMsg(trace_ctx, "\tReferenceFrames (TopFieldOrderCnt-BottomFieldOrderCnt-picture_id-frame_idx-flags):\n");
    for (i = 0; i < 16; i++)
    {
        if ((p->ReferenceFrames[i].picture_id != VA_INVALID_SURFACE) &&
            ((p->ReferenceFrames[i].flags & VA_PICTURE_H264_INVALID) == 0)) {
            va_TraceMsg(trace_ctx, "\t\t%08d-%08d-0x%08x-%08d-0x%08x\n",
                        p->ReferenceFrames[i].TopFieldOrderCnt,
                        p->ReferenceFrames[i].BottomFieldOrderCnt,
                        p->ReferenceFrames[i].picture_id,
                        p->ReferenceFrames[i].frame_idx,
                        p->ReferenceFrames[i].flags
                        );
        } else
            break;
    }
    va_TraceMsg(trace_ctx, "\tcoded_buf = %08x\n", p->coded_buf);
    va_TraceMsg(trace_ctx, "\tpic_parameter_set_id = %d\n", p->pic_parameter_set_id);
    va_TraceMsg(trace_ctx, "\tseq_parameter_set_id = %d\n", p->seq_parameter_set_id);
    va_TraceMsg(trace_ctx, "\tlast_picture = 0x%08x\n", p->last_picture);
    va_TraceMsg(trace_ctx, "\tframe_num = %d\n", p->frame_num);
    va_TraceMsg(trace_ctx, "\tpic_init_qp = %d\n", p->pic_init_qp);
    va_TraceMsg(trace_ctx, "\tnum_ref_idx_l0_active_minus1 = %d\n", p->num_ref_idx_l0_active_minus1);
    va_TraceMsg(trace_ctx, "\tnum_ref_idx_l1_active_minus1 = %d\n", p->num_ref_idx_l1_active_minus1);
    va_TraceMsg(trace_ctx, "\tchroma_qp_index_offset = %d\n", p->chroma_qp_index_offset);
    va_TraceMsg(trace_ctx, "\tsecond_chroma_qp_index_offset = %d\n", p->second_chroma_qp_index_offset);
    va_TraceMsg(trace_ctx, "\tpic_fields = 0x%03x\n", p->pic_fields.value);
    va_TraceMsg(trace_ctx, "\tidr_pic_flag = %d\n", p->pic_fields.bits.idr_pic_flag);
    va_TraceMsg(trace_ctx, "\treference_pic_flag = %d\n", p->pic_fields.bits.reference_pic_flag);
    va_TraceMsg(trace_ctx, "\tentropy_coding_mode_flag = %d\n", p->pic_fields.bits.entropy_coding_mode_flag);
    va_TraceMsg(trace_ctx, "\tweighted_pred_flag = %d\n", p->pic_fields.bits.weighted_pred_flag);
    va_TraceMsg(trace_ctx, "\tweighted_bipred_idc = %d\n", p->pic_fields.bits.weighted_bipred_idc);
    va_TraceMsg(trace_ctx, "\tconstrained_intra_pred_flag = %d\n", p->pic_fields.bits.constrained_intra_pred_flag);
    va_TraceMsg(trace_ctx, "\ttransform_8x8_mode_flag = %d\n", p->pic_fields.bits.transform_8x8_mode_flag);
    va_TraceMsg(trace_ctx, "\tdeblocking_filter_control_present_flag = %d\n", p->pic_fields.bits.deblocking_filter_control_present_flag);
    va_TraceMsg(trace_ctx, "\tredundant_pic_cnt_present_flag = %d\n", p->pic_fields.bits.redundant_pic_cnt_present_flag);
    va_TraceMsg(trace_ctx, "\tpic_order_present_flag = %d\n", p->pic_fields.bits.pic_order_present_flag);
    va_TraceMsg(trace_ctx, "\tpic_scaling_matrix_present_flag = %d\n", p->pic_fields.bits.pic_scaling_matrix_present_flag);

    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncSliceParameterBuffer(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncSliceParameterBuffer* p = (VAEncSliceParameterBuffer*)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAEncSliceParameterBuffer\n");
    
    va_TraceMsg(trace_ctx, "\tstart_row_number = %d\n", p->start_row_number);
    va_TraceMsg(trace_ctx, "\tslice_height = %d\n", p->slice_height);
    va_TraceMsg(trace_ctx, "\tslice_flags.is_intra = %d\n", p->slice_flags.bits.is_intra);
    va_TraceMsg(trace_ctx, "\tslice_flags.disable_deblocking_filter_idc = %d\n", p->slice_flags.bits.disable_deblocking_filter_idc);
    va_TraceMsg(trace_ctx, "\tslice_flags.uses_long_term_ref = %d\n", p->slice_flags.bits.uses_long_term_ref);
    va_TraceMsg(trace_ctx, "\tslice_flags.is_long_term_ref = %d\n", p->slice_flags.bits.is_long_term_ref);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncSliceParameterBufferH264(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncSliceParameterBufferH264* p = (VAEncSliceParameterBufferH264*)data;
    DPY2TRACECTX(dpy);
    int i;

    if (!p)
        return;
    
    va_TraceMsg(trace_ctx, "\t--VAEncSliceParameterBufferH264\n");
    va_TraceMsg(trace_ctx, "\tmacroblock_address = %d\n", p->macroblock_address);
    va_TraceMsg(trace_ctx, "\tnum_macroblocks = %d\n", p->num_macroblocks);
    va_TraceMsg(trace_ctx, "\tmacroblock_info = %08x\n", p->macroblock_info);
    va_TraceMsg(trace_ctx, "\tslice_type = %d\n", p->slice_type);
    va_TraceMsg(trace_ctx, "\tpic_parameter_set_id = %d\n", p->pic_parameter_set_id);
    va_TraceMsg(trace_ctx, "\tidr_pic_id = %d\n", p->idr_pic_id);
    va_TraceMsg(trace_ctx, "\tpic_order_cnt_lsb = %d\n", p->pic_order_cnt_lsb);
    va_TraceMsg(trace_ctx, "\tdelta_pic_order_cnt_bottom = %d\n", p->delta_pic_order_cnt_bottom);
    va_TraceMsg(trace_ctx, "\tdelta_pic_order_cnt[0] = %d\n", p->delta_pic_order_cnt[0]);
    va_TraceMsg(trace_ctx, "\tdelta_pic_order_cnt[1] = %d\n", p->delta_pic_order_cnt[1]);
    va_TraceMsg(trace_ctx, "\tdirect_spatial_mv_pred_flag = %d\n", p->direct_spatial_mv_pred_flag);
    va_TraceMsg(trace_ctx, "\tnum_ref_idx_active_override_flag = %d\n", p->num_ref_idx_active_override_flag);
    va_TraceMsg(trace_ctx, "\tnum_ref_idx_l1_active_minus1 = %d\n", p->num_ref_idx_l1_active_minus1);
    va_TraceMsg(trace_ctx, "\tslice_beta_offset_div2 = %d\n", p->slice_beta_offset_div2);

    va_TraceMsg(trace_ctx, "\tRefPicList0 (TopFieldOrderCnt-BottomFieldOrderCnt-picture_id-frame_idx-flags):\n");

    
    
    for (i = 0; i < 32; i++) {
        if ((p->RefPicList0[i].picture_id != VA_INVALID_SURFACE) &&
            ((p->RefPicList0[i].flags & VA_PICTURE_H264_INVALID) == 0))
            va_TraceMsg(trace_ctx, "\t\t%08d-%08d-0x%08x-%08d-0x%08x\n",
                        p->RefPicList0[i].TopFieldOrderCnt,
                        p->RefPicList0[i].BottomFieldOrderCnt,
                        p->RefPicList0[i].picture_id,
                        p->RefPicList0[i].frame_idx,
                        p->RefPicList0[i].flags);
        else
            break;
    }
    
    va_TraceMsg(trace_ctx, "\tRefPicList1 (TopFieldOrderCnt-BottomFieldOrderCnt-picture_id-frame_idx-flags):\n");
    for (i = 0; i < 32; i++) {
        if ((p->RefPicList1[i].picture_id != VA_INVALID_SURFACE) &&
            ((p->RefPicList1[i].flags & VA_PICTURE_H264_INVALID) == 0))
            va_TraceMsg(trace_ctx, "\t\t%08d-%08d-0x%08x-%08d-0x%08d\n",
                        p->RefPicList1[i].TopFieldOrderCnt,
                        p->RefPicList1[i].BottomFieldOrderCnt,
                        p->RefPicList1[i].picture_id,
                        p->RefPicList1[i].frame_idx,
                        p->RefPicList1[i].flags
                        );
        else
            break;
    }
    
    va_TraceMsg(trace_ctx, "\tluma_log2_weight_denom = %d\n", p->luma_log2_weight_denom);
    va_TraceMsg(trace_ctx, "\tchroma_log2_weight_denom = %d\n", p->chroma_log2_weight_denom);
    va_TraceMsg(trace_ctx, "\tluma_weight_l0_flag = %d\n", p->luma_weight_l0_flag);
    if (p->luma_weight_l0_flag) {
        for (i = 0; (i <= p->num_ref_idx_l0_active_minus1) && (i<32); i++) {
            va_TraceMsg(trace_ctx, "\t\t%d\t%d\n",
                        p->luma_weight_l0[i],
                        p->luma_offset_l0[i]);
        }
    }

    va_TraceMsg(trace_ctx, "\tchroma_weight_l0_flag = %d\n", p->chroma_weight_l0_flag);
    if (p->chroma_weight_l0_flag) {
        for (i = 0; (i <= p->num_ref_idx_l0_active_minus1) && (i<32); i++) {
            va_TraceMsg(trace_ctx, "\t\t%d\t%d\t%d\t%d\n",
                        p->chroma_weight_l0[i][0],
                        p->chroma_offset_l0[i][0],
                        p->chroma_weight_l0[i][1],
                        p->chroma_offset_l0[i][1]);
        }
    }

    va_TraceMsg(trace_ctx, "\tluma_weight_l1_flag = %d\n", p->luma_weight_l1_flag);
    if (p->luma_weight_l1_flag) {
        for (i = 0; (i <= p->num_ref_idx_l1_active_minus1) && (i<32); i++) {
            va_TraceMsg(trace_ctx, "\t\t%d\t\t%d\n",
                        p->luma_weight_l1[i],
                        p->luma_offset_l1[i]);
        }
    }

    va_TraceMsg(trace_ctx, "\tchroma_weight_l1_flag = %d\n", p->chroma_weight_l1_flag);
    if (p->chroma_weight_l1_flag && p->num_ref_idx_l1_active_minus1 < 32) {
        for (i = 0; (i <= p->num_ref_idx_l1_active_minus1) && (i<32); i++) {
            va_TraceMsg(trace_ctx, "\t\t%d\t%d\t%d\t%d\n",
                        p->chroma_weight_l1[i][0],
                        p->chroma_offset_l1[i][0],
                        p->chroma_weight_l1[i][1],
                        p->chroma_offset_l1[i][1]);
        }
    }
    va_TraceMsg(trace_ctx, NULL);

    va_TraceMsg(trace_ctx, "\tcabac_init_idc = %d\n", p->cabac_init_idc);
    va_TraceMsg(trace_ctx, "\tslice_qp_delta = %d\n", p->slice_qp_delta);
    va_TraceMsg(trace_ctx, "\tdisable_deblocking_filter_idc = %d\n", p->disable_deblocking_filter_idc);
    va_TraceMsg(trace_ctx, "\tslice_alpha_c0_offset_div2 = %d\n", p->slice_alpha_c0_offset_div2);
    va_TraceMsg(trace_ctx, "\tslice_beta_offset_div2 = %d\n", p->slice_beta_offset_div2);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAEncPackedHeaderParameterBufferType(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncPackedHeaderParameterBuffer* p = (VAEncPackedHeaderParameterBuffer*)data;
    DPY2TRACECTX(dpy);
    int i;

    if (!p)
        return;
    va_TraceMsg(trace_ctx, "\t--VAEncPackedHeaderParameterBuffer\n");
    va_TraceMsg(trace_ctx, "\ttype = 0x%08x\n", p->type);
    va_TraceMsg(trace_ctx, "\tbit_length = %d\n", p->bit_length);
    va_TraceMsg(trace_ctx, "\thas_emulation_bytes = %d\n", p->has_emulation_bytes);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncMiscParameterBuffer(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncMiscParameterBuffer* tmp = (VAEncMiscParameterBuffer*)data;
    DPY2TRACECTX(dpy);
    
    switch (tmp->type) {
    case VAEncMiscParameterTypeFrameRate:
    {
        VAEncMiscParameterFrameRate *p = (VAEncMiscParameterFrameRate *)tmp->data;
        va_TraceMsg(trace_ctx, "\t--VAEncMiscParameterFrameRate\n");
        va_TraceMsg(trace_ctx, "\tframerate = %d\n", p->framerate);
        
        break;
    }
    case VAEncMiscParameterTypeRateControl:
    {
        VAEncMiscParameterRateControl *p = (VAEncMiscParameterRateControl *)tmp->data;

        va_TraceMsg(trace_ctx, "\t--VAEncMiscParameterRateControl\n");
        va_TraceMsg(trace_ctx, "\tbits_per_second = %d\n", p->bits_per_second);
        va_TraceMsg(trace_ctx, "\ttarget_percentage = %d\n", p->target_percentage);
        va_TraceMsg(trace_ctx, "\twindow_size = %d\n", p->window_size);
        va_TraceMsg(trace_ctx, "\tinitial_qp = %d\n", p->initial_qp);
        va_TraceMsg(trace_ctx, "\tmin_qp = %d\n", p->min_qp);
        va_TraceMsg(trace_ctx, "\tbasic_unit_size = %d\n", p->basic_unit_size);
        va_TraceMsg(trace_ctx, "\trc_flags.reset = %d \n", p->rc_flags.bits.reset);
        va_TraceMsg(trace_ctx, "\trc_flags.disable_frame_skip = %d\n", p->rc_flags.bits.disable_frame_skip);
        va_TraceMsg(trace_ctx, "\trc_flags.disable_bit_stuffing = %d\n", p->rc_flags.bits.disable_bit_stuffing);
        break;
    }
    case VAEncMiscParameterTypeMaxSliceSize:
    {
        VAEncMiscParameterMaxSliceSize *p = (VAEncMiscParameterMaxSliceSize *)tmp->data;
        
        va_TraceMsg(trace_ctx, "\t--VAEncMiscParameterTypeMaxSliceSize\n");
        va_TraceMsg(trace_ctx, "\tmax_slice_size = %d\n", p->max_slice_size);
        break;
    }
    case VAEncMiscParameterTypeAIR:
    {
        VAEncMiscParameterAIR *p = (VAEncMiscParameterAIR *)tmp->data;
        
        va_TraceMsg(trace_ctx, "\t--VAEncMiscParameterAIR\n");
        va_TraceMsg(trace_ctx, "\tair_num_mbs = %d\n", p->air_num_mbs);
        va_TraceMsg(trace_ctx, "\tair_threshold = %d\n", p->air_threshold);
        va_TraceMsg(trace_ctx, "\tair_auto = %d\n", p->air_auto);
        break;
    }
    case VAEncMiscParameterTypeHRD:
    {
        VAEncMiscParameterHRD *p = (VAEncMiscParameterHRD *)tmp->data;

        va_TraceMsg(trace_ctx, "\t--VAEncMiscParameterHRD\n");
        va_TraceMsg(trace_ctx, "\tinitial_buffer_fullness = %d\n", p->initial_buffer_fullness);
        va_TraceMsg(trace_ctx, "\tbuffer_size = %d\n", p->buffer_size);
        break;
    }
    case VAEncMiscParameterTypeMaxFrameSize:
    {
        VAEncMiscParameterBufferMaxFrameSize *p = (VAEncMiscParameterBufferMaxFrameSize *)tmp->data;

        va_TraceMsg(trace_ctx, "\t--VAEncMiscParameterTypeMaxFrameSize\n");
        va_TraceMsg(trace_ctx, "\tmax_frame_size = %d\n", p->max_frame_size);
        break;
    }
    default:
        va_TraceMsg(trace_ctx, "Unknown VAEncMiscParameterBuffer(type = %d):\n", tmp->type);
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, data);
        break;
    }
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAPictureParameterBufferVC1(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data
)
{
    VAPictureParameterBufferVC1* p = (VAPictureParameterBufferVC1*)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAPictureParameterBufferVC1\n");
    
    va_TraceMsg(trace_ctx, "\tforward_reference_picture = 0x%08x\n", p->forward_reference_picture);
    va_TraceMsg(trace_ctx, "\tbackward_reference_picture = 0x%08x\n", p->backward_reference_picture);
    va_TraceMsg(trace_ctx, "\tinloop_decoded_picture = 0x%08x\n", p->inloop_decoded_picture);
    
    va_TraceMsg(trace_ctx, "\tpulldown = %d\n", p->sequence_fields.bits.pulldown);
    va_TraceMsg(trace_ctx, "\tinterlace = %d\n", p->sequence_fields.bits.interlace);
    va_TraceMsg(trace_ctx, "\ttfcntrflag = %d\n", p->sequence_fields.bits.tfcntrflag);
    va_TraceMsg(trace_ctx, "\tfinterpflag = %d\n", p->sequence_fields.bits.finterpflag);
    va_TraceMsg(trace_ctx, "\tpsf = %d\n", p->sequence_fields.bits.psf);
    va_TraceMsg(trace_ctx, "\tmultires = %d\n", p->sequence_fields.bits.multires);
    va_TraceMsg(trace_ctx, "\toverlap = %d\n", p->sequence_fields.bits.overlap);
    va_TraceMsg(trace_ctx, "\tsyncmarker = %d\n", p->sequence_fields.bits.syncmarker);
    va_TraceMsg(trace_ctx, "\trangered = %d\n", p->sequence_fields.bits.rangered);
    va_TraceMsg(trace_ctx, "\tmax_b_frames = %d\n", p->sequence_fields.bits.max_b_frames);
    va_TraceMsg(trace_ctx, "\tprofile = %d\n", p->sequence_fields.bits.profile);
    va_TraceMsg(trace_ctx, "\tcoded_width = %d\n", p->coded_width);
    va_TraceMsg(trace_ctx, "\tcoded_height = %d\n", p->coded_height);
    va_TraceMsg(trace_ctx, "\tclosed_entry = %d\n", p->entrypoint_fields.bits.closed_entry);
    va_TraceMsg(trace_ctx, "\tbroken_link = %d\n", p->entrypoint_fields.bits.broken_link);
    va_TraceMsg(trace_ctx, "\tclosed_entry = %d\n", p->entrypoint_fields.bits.closed_entry);
    va_TraceMsg(trace_ctx, "\tpanscan_flag = %d\n", p->entrypoint_fields.bits.panscan_flag);
    va_TraceMsg(trace_ctx, "\tloopfilter = %d\n", p->entrypoint_fields.bits.loopfilter);
    va_TraceMsg(trace_ctx, "\tconditional_overlap_flag = %d\n", p->conditional_overlap_flag);
    va_TraceMsg(trace_ctx, "\tfast_uvmc_flag = %d\n", p->fast_uvmc_flag);
    va_TraceMsg(trace_ctx, "\trange_mapping_luma_flag = %d\n", p->range_mapping_fields.bits.luma_flag);
    va_TraceMsg(trace_ctx, "\trange_mapping_luma = %d\n", p->range_mapping_fields.bits.luma);
    va_TraceMsg(trace_ctx, "\trange_mapping_chroma_flag = %d\n", p->range_mapping_fields.bits.chroma_flag);
    va_TraceMsg(trace_ctx, "\trange_mapping_chroma = %d\n", p->range_mapping_fields.bits.chroma);
    va_TraceMsg(trace_ctx, "\tb_picture_fraction = %d\n", p->b_picture_fraction);
    va_TraceMsg(trace_ctx, "\tcbp_table = %d\n", p->cbp_table);
    va_TraceMsg(trace_ctx, "\tmb_mode_table = %d\n", p->mb_mode_table);
    va_TraceMsg(trace_ctx, "\trange_reduction_frame = %d\n", p->range_reduction_frame);
    va_TraceMsg(trace_ctx, "\trounding_control = %d\n", p->rounding_control);
    va_TraceMsg(trace_ctx, "\tpost_processing = %d\n", p->post_processing);
    va_TraceMsg(trace_ctx, "\tpicture_resolution_index = %d\n", p->picture_resolution_index);
    va_TraceMsg(trace_ctx, "\tluma_scale = %d\n", p->luma_scale);
    va_TraceMsg(trace_ctx, "\tluma_shift = %d\n", p->luma_shift);
    va_TraceMsg(trace_ctx, "\tpicture_type = %d\n", p->picture_fields.bits.picture_type);
    va_TraceMsg(trace_ctx, "\tframe_coding_mode = %d\n", p->picture_fields.bits.frame_coding_mode);
    va_TraceMsg(trace_ctx, "\ttop_field_first = %d\n", p->picture_fields.bits.top_field_first);
    va_TraceMsg(trace_ctx, "\tis_first_field = %d\n", p->picture_fields.bits.is_first_field);
    va_TraceMsg(trace_ctx, "\tintensity_compensation = %d\n", p->picture_fields.bits.intensity_compensation);
    va_TraceMsg(trace_ctx, "\tmv_type_mb = %d\n", p->raw_coding.flags.mv_type_mb);
    va_TraceMsg(trace_ctx, "\tdirect_mb = %d\n", p->raw_coding.flags.direct_mb);
    va_TraceMsg(trace_ctx, "\tskip_mb = %d\n", p->raw_coding.flags.skip_mb);
    va_TraceMsg(trace_ctx, "\tfield_tx = %d\n", p->raw_coding.flags.field_tx);
    va_TraceMsg(trace_ctx, "\tforward_mb = %d\n", p->raw_coding.flags.forward_mb);
    va_TraceMsg(trace_ctx, "\tac_pred = %d\n", p->raw_coding.flags.ac_pred);
    va_TraceMsg(trace_ctx, "\toverflags = %d\n", p->raw_coding.flags.overflags);
    va_TraceMsg(trace_ctx, "\tbp_mv_type_mb = %d\n", p->bitplane_present.flags.bp_mv_type_mb);
    va_TraceMsg(trace_ctx, "\tbp_direct_mb = %d\n", p->bitplane_present.flags.bp_direct_mb);
    va_TraceMsg(trace_ctx, "\tbp_skip_mb = %d\n", p->bitplane_present.flags.bp_skip_mb);
    va_TraceMsg(trace_ctx, "\tbp_field_tx = %d\n", p->bitplane_present.flags.bp_field_tx);
    va_TraceMsg(trace_ctx, "\tbp_forward_mb = %d\n", p->bitplane_present.flags.bp_forward_mb);
    va_TraceMsg(trace_ctx, "\tbp_ac_pred = %d\n", p->bitplane_present.flags.bp_ac_pred);
    va_TraceMsg(trace_ctx, "\tbp_overflags = %d\n", p->bitplane_present.flags.bp_overflags);
    va_TraceMsg(trace_ctx, "\treference_distance_flag = %d\n", p->reference_fields.bits.reference_distance_flag);
    va_TraceMsg(trace_ctx, "\treference_distance = %d\n", p->reference_fields.bits.reference_distance);
    va_TraceMsg(trace_ctx, "\tnum_reference_pictures = %d\n", p->reference_fields.bits.num_reference_pictures);
    va_TraceMsg(trace_ctx, "\treference_field_pic_indicator = %d\n", p->reference_fields.bits.reference_field_pic_indicator);
    va_TraceMsg(trace_ctx, "\tmv_mode = %d\n", p->mv_fields.bits.mv_mode);
    va_TraceMsg(trace_ctx, "\tmv_mode2 = %d\n", p->mv_fields.bits.mv_mode2);
    va_TraceMsg(trace_ctx, "\tmv_table = %d\n", p->mv_fields.bits.mv_table);
    va_TraceMsg(trace_ctx, "\ttwo_mv_block_pattern_table = %d\n", p->mv_fields.bits.two_mv_block_pattern_table);
    va_TraceMsg(trace_ctx, "\tfour_mv_switch = %d\n", p->mv_fields.bits.four_mv_switch);
    va_TraceMsg(trace_ctx, "\tfour_mv_block_pattern_table = %d\n", p->mv_fields.bits.four_mv_block_pattern_table);
    va_TraceMsg(trace_ctx, "\textended_mv_flag = %d\n", p->mv_fields.bits.extended_mv_flag);
    va_TraceMsg(trace_ctx, "\textended_mv_range = %d\n", p->mv_fields.bits.extended_mv_range);
    va_TraceMsg(trace_ctx, "\textended_dmv_flag = %d\n", p->mv_fields.bits.extended_dmv_flag);
    va_TraceMsg(trace_ctx, "\textended_dmv_range = %d\n", p->mv_fields.bits.extended_dmv_range);
    va_TraceMsg(trace_ctx, "\tdquant = %d\n", p->pic_quantizer_fields.bits.dquant);
    va_TraceMsg(trace_ctx, "\tquantizer = %d\n", p->pic_quantizer_fields.bits.quantizer);
    va_TraceMsg(trace_ctx, "\thalf_qp = %d\n", p->pic_quantizer_fields.bits.half_qp);
    va_TraceMsg(trace_ctx, "\tpic_quantizer_scale = %d\n", p->pic_quantizer_fields.bits.pic_quantizer_scale);
    va_TraceMsg(trace_ctx, "\tpic_quantizer_type = %d\n", p->pic_quantizer_fields.bits.pic_quantizer_type);
    va_TraceMsg(trace_ctx, "\tdq_frame = %d\n", p->pic_quantizer_fields.bits.dq_frame);
    va_TraceMsg(trace_ctx, "\tdq_profile = %d\n", p->pic_quantizer_fields.bits.dq_profile);
    va_TraceMsg(trace_ctx, "\tdq_sb_edge = %d\n", p->pic_quantizer_fields.bits.dq_sb_edge);
    va_TraceMsg(trace_ctx, "\tdq_db_edge = %d\n", p->pic_quantizer_fields.bits.dq_db_edge);
    va_TraceMsg(trace_ctx, "\tdq_binary_level = %d\n", p->pic_quantizer_fields.bits.dq_binary_level);
    va_TraceMsg(trace_ctx, "\talt_pic_quantizer = %d\n", p->pic_quantizer_fields.bits.alt_pic_quantizer);
    va_TraceMsg(trace_ctx, "\tvariable_sized_transform_flag = %d\n", p->transform_fields.bits.variable_sized_transform_flag);
    va_TraceMsg(trace_ctx, "\tmb_level_transform_type_flag = %d\n", p->transform_fields.bits.mb_level_transform_type_flag);
    va_TraceMsg(trace_ctx, "\tframe_level_transform_type = %d\n", p->transform_fields.bits.frame_level_transform_type);
    va_TraceMsg(trace_ctx, "\ttransform_ac_codingset_idx1 = %d\n", p->transform_fields.bits.transform_ac_codingset_idx1);
    va_TraceMsg(trace_ctx, "\ttransform_ac_codingset_idx2 = %d\n", p->transform_fields.bits.transform_ac_codingset_idx2);
    va_TraceMsg(trace_ctx, "\tintra_transform_dc_table = %d\n", p->transform_fields.bits.intra_transform_dc_table);
    va_TraceMsg(trace_ctx, NULL);
}

static void va_TraceVASliceParameterBufferVC1(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void* data
)
{
    VASliceParameterBufferVC1 *p = (VASliceParameterBufferVC1*)data;
    DPY2TRACECTX(dpy);

    trace_ctx->trace_slice_no++;
    trace_ctx->trace_slice_size = p->slice_data_size;

    va_TraceMsg(trace_ctx, "\t--VASliceParameterBufferVC1\n");
    va_TraceMsg(trace_ctx, "\tslice_data_size = %d\n", p->slice_data_size);
    va_TraceMsg(trace_ctx, "\tslice_data_offset = %d\n", p->slice_data_offset);
    va_TraceMsg(trace_ctx, "\tslice_data_flag = %d\n", p->slice_data_flag);
    va_TraceMsg(trace_ctx, "\tmacroblock_offset = %d\n", p->macroblock_offset);
    va_TraceMsg(trace_ctx, "\tslice_vertical_position = %d\n", p->slice_vertical_position);
    va_TraceMsg(trace_ctx, NULL);
}

static void va_TraceVAPictureParameterBufferVP8(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    char tmp[1024];
    VAPictureParameterBufferVP8 *p = (VAPictureParameterBufferVP8 *)data;
    DPY2TRACECTX(dpy);
    int i,j;

    va_TraceMsg(trace_ctx, "\t--VAPictureParameterBufferVP8\n");

    va_TraceMsg(trace_ctx, "\tframe_width = %d\n", p->frame_width);
    va_TraceMsg(trace_ctx, "\tframe_height = %d\n", p->frame_height);
    va_TraceMsg(trace_ctx, "\tlast_ref_frame = %x\n", p->last_ref_frame);
    va_TraceMsg(trace_ctx, "\tgolden_ref_frame = %x\n", p->golden_ref_frame);
    va_TraceMsg(trace_ctx, "\talt_ref_frame = %x\n", p->alt_ref_frame);
    va_TraceMsg(trace_ctx, "\tout_of_loop_frame = %x\n", p->out_of_loop_frame);

    va_TraceMsg(trace_ctx, "\tkey_frame = %d\n", p->pic_fields.bits.key_frame);
    va_TraceMsg(trace_ctx, "\tversion = %d\n", p->pic_fields.bits.version);
    va_TraceMsg(trace_ctx, "\tsegmentation_enabled = %d\n", p->pic_fields.bits.segmentation_enabled);
    va_TraceMsg(trace_ctx, "\tupdate_mb_segmentation_map = %d\n", p->pic_fields.bits.update_mb_segmentation_map);
    va_TraceMsg(trace_ctx, "\tupdate_segment_feature_data = %d\n", p->pic_fields.bits.update_segment_feature_data);
    va_TraceMsg(trace_ctx, "\tfilter_type = %d\n", p->pic_fields.bits.filter_type);
    va_TraceMsg(trace_ctx, "\tsharpness_level = %d\n", p->pic_fields.bits.sharpness_level);
    va_TraceMsg(trace_ctx, "\tloop_filter_adj_enable = %d\n", p->pic_fields.bits.loop_filter_adj_enable);
    va_TraceMsg(trace_ctx, "\tmode_ref_lf_delta_update = %d\n", p->pic_fields.bits.mode_ref_lf_delta_update);
    va_TraceMsg(trace_ctx, "\tsign_bias_golden = %d\n", p->pic_fields.bits.sign_bias_golden);
    va_TraceMsg(trace_ctx, "\tsign_bias_alternate = %d\n", p->pic_fields.bits.sign_bias_alternate);
    va_TraceMsg(trace_ctx, "\tmb_no_coeff_skip = %d\n", p->pic_fields.bits.mb_no_coeff_skip);
    va_TraceMsg(trace_ctx, "\tloop_filter_disable = %d\n", p->pic_fields.bits.loop_filter_disable);

    va_TraceMsg(trace_ctx, "\tmb_segment_tree_probs: 0x%2x, 0x%2x, 0x%2x\n",
        p->mb_segment_tree_probs[0], p->mb_segment_tree_probs[1], p->mb_segment_tree_probs[2]);

    va_TraceMsg(trace_ctx, "\tloop_filter_level: %d, %d, %d, %d\n",
        p->loop_filter_level[0], p->loop_filter_level[1], p->loop_filter_level[2], p->loop_filter_level[3]);

    va_TraceMsg(trace_ctx, "\tloop_filter_deltas_ref_frame: %d, %d, %d, %d\n",
        p->loop_filter_deltas_ref_frame[0], p->loop_filter_deltas_ref_frame[1], p->loop_filter_deltas_ref_frame[2], p->loop_filter_deltas_ref_frame[3]);

    va_TraceMsg(trace_ctx, "\tloop_filter_deltas_mode: %d, %d, %d, %d\n",
        p->loop_filter_deltas_mode[0], p->loop_filter_deltas_mode[1], p->loop_filter_deltas_mode[2], p->loop_filter_deltas_mode[3]);

    va_TraceMsg(trace_ctx, "\tprob_skip_false = %2x\n", p->prob_skip_false);
    va_TraceMsg(trace_ctx, "\tprob_intra = %2x\n", p->prob_intra);
    va_TraceMsg(trace_ctx, "\tprob_last = %2x\n", p->prob_last);
    va_TraceMsg(trace_ctx, "\tprob_gf = %2x\n", p->prob_gf);

    va_TraceMsg(trace_ctx, "\ty_mode_probs: 0x%2x, 0x%2x, 0x%2x, 0x%2x\n",
        p->y_mode_probs[0], p->y_mode_probs[1], p->y_mode_probs[2], p->y_mode_probs[3]);

    va_TraceMsg(trace_ctx, "\tuv_mode_probs: 0x%2x, 0x%2x, 0x%2x\n",
        p->uv_mode_probs[0], p->uv_mode_probs[1], p->uv_mode_probs[2]);

    va_TraceMsg(trace_ctx, "\tmv_probs[2][19]:\n");
    for(i = 0; i<2; ++i) {
        memset(tmp, 0, sizeof tmp);
        for (j=0; j<19; j++)
            sprintf(tmp + strlen(tmp), "%2x ", p->mv_probs[i][j]);
        va_TraceMsg(trace_ctx,"\t\t[%d] = %s\n", i, tmp);
    }

    va_TraceMsg(trace_ctx, "\tbool_coder_ctx: range = %02x, value = %02x, count = %d\n",
        p->bool_coder_ctx.range, p->bool_coder_ctx.value, p->bool_coder_ctx.count);

    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVASliceParameterBufferVP8(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VASliceParameterBufferVP8 *p = (VASliceParameterBufferVP8 *)data;
    DPY2TRACECTX(dpy);
    int i;

    va_TraceMsg(trace_ctx, "\t--VASliceParameterBufferVP8\n");

    va_TraceMsg(trace_ctx, "\tslice_data_size = %d\n", p->slice_data_size);
    va_TraceMsg(trace_ctx, "\tslice_data_offset = %d\n", p->slice_data_offset);
    va_TraceMsg(trace_ctx, "\tslice_data_flag = %d\n", p->slice_data_flag);
    va_TraceMsg(trace_ctx, "\tmacroblock_offset = %d\n", p->macroblock_offset);
    va_TraceMsg(trace_ctx, "\tnum_of_partitions = %d\n", p->num_of_partitions);

    for(i = 0; i<9; ++i)
        va_TraceMsg(trace_ctx, "\tpartition_size[%d] = %d\n", i, p->partition_size[i]);

    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAIQMatrixBufferVP8(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    char tmp[1024];
    VAIQMatrixBufferVP8 *p = (VAIQMatrixBufferVP8 *)data;
    DPY2TRACECTX(dpy);
    int i,j;

    va_TraceMsg(trace_ctx, "\t--VAIQMatrixBufferVP8\n");

    va_TraceMsg(trace_ctx, "\tquantization_index[4][6]=\n");
    for (i = 0; i < 4; i++) {
        memset(tmp, 0, sizeof tmp);
        for (j = 0; j < 6; j++)
            sprintf(tmp + strlen(tmp), "%4x, ", p->quantization_index[i][j]);
        va_TraceMsg(trace_ctx,"\t\t[%d] = %s\n", i, tmp);
    }

    va_TraceMsg(trace_ctx, NULL);

    return;
}
static void va_TraceVAProbabilityBufferVP8(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    char tmp[1024];
    VAProbabilityDataBufferVP8 *p = (VAProbabilityDataBufferVP8 *)data;
    DPY2TRACECTX(dpy);
    int i,j,k,l;

    va_TraceMsg(trace_ctx, "\t--VAProbabilityDataBufferVP8\n");

    for (i = 0; i < 4; i++)
        for (j = 0; j < 8; j++) {
            memset(tmp, 0, sizeof tmp);
            for (k=0; k<3; k++)
                for (l=0; l<11; l++)
                    sprintf(tmp + strlen(tmp), "%2x, ", p->dct_coeff_probs[i][j][k][l]);
            va_TraceMsg(trace_ctx,"\t\t[%d, %d] = %s\n", i, j, tmp);
        }

    va_TraceMsg(trace_ctx, NULL);

    return;
}

void va_TraceBeginPicture(
    VADisplay dpy,
    VAContextID context,
    VASurfaceID render_target
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\tcontext = 0x%08x\n", context);
    va_TraceMsg(trace_ctx, "\trender_targets = 0x%08x\n", render_target);
    va_TraceMsg(trace_ctx, "\tframe_count  = #%d\n", trace_ctx->trace_frame_no);
    va_TraceMsg(trace_ctx, NULL);

    trace_ctx->trace_rendertarget = render_target; /* for surface data dump after vaEndPicture */

    trace_ctx->trace_frame_no++;
    trace_ctx->trace_slice_no = 0;
}

static void va_TraceMPEG2Buf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    switch (type) {
    case VAPictureParameterBufferType:
        va_TraceVAPictureParameterBufferMPEG2(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:
        va_TraceVAIQMatrixBufferMPEG2(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VABitPlaneBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceGroupMapBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceParameterBufferType:
        va_TraceVASliceParameterBufferMPEG2(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAMacroblockParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAResidualDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VADeblockingParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAImageBufferType:
        break;
    case VAProtectedSliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncCodedBufferType:
        break;
    case VAEncSequenceParameterBufferType:
        break;
    case VAEncPictureParameterBufferType:
        break;
    case VAEncSliceParameterBufferType:
        break;
    default:
        break;
    }
}

static void va_TraceVAEncSequenceParameterBufferH263(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncSequenceParameterBufferH263 *p = (VAEncSequenceParameterBufferH263 *)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAEncSequenceParameterBufferH263\n");
    
    va_TraceMsg(trace_ctx, "\tintra_period = %d\n", p->intra_period);
    va_TraceMsg(trace_ctx, "\tbits_per_second = %d\n", p->bits_per_second);
    va_TraceMsg(trace_ctx, "\tframe_rate = %d\n", p->frame_rate);
    va_TraceMsg(trace_ctx, "\tinitial_qp = %d\n", p->initial_qp);
    va_TraceMsg(trace_ctx, "\tmin_qp = %d\n", p->min_qp);
    va_TraceMsg(trace_ctx, NULL);

    return;
}


static void va_TraceVAEncPictureParameterBufferH263(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncPictureParameterBufferH263 *p = (VAEncPictureParameterBufferH263 *)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAEncPictureParameterBufferH263\n");
    va_TraceMsg(trace_ctx, "\treference_picture = 0x%08x\n", p->reference_picture);
    va_TraceMsg(trace_ctx, "\treconstructed_picture = 0x%08x\n", p->reconstructed_picture);
    va_TraceMsg(trace_ctx, "\tcoded_buf = %08x\n", p->coded_buf);
    va_TraceMsg(trace_ctx, "\tpicture_width = %d\n", p->picture_width);
    va_TraceMsg(trace_ctx, "\tpicture_height = %d\n", p->picture_height);
    va_TraceMsg(trace_ctx, "\tpicture_type = 0x%08x\n", p->picture_type);
    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncPictureParameterBufferJPEG(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAEncPictureParameterBufferJPEG *p = (VAEncPictureParameterBufferJPEG *)data;
    int i;
    
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAEncPictureParameterBufferJPEG\n");
    va_TraceMsg(trace_ctx, "\treconstructed_picture = 0x%08x\n", p->reconstructed_picture);
    va_TraceMsg(trace_ctx, "\tcoded_buf = %08x\n", p->coded_buf);
    va_TraceMsg(trace_ctx, "\tpicture_width = %d\n", p->picture_width);
    va_TraceMsg(trace_ctx, "\tpicture_height = %d\n", p->picture_height);

    va_TraceMsg(trace_ctx, NULL);

    return;
}

static void va_TraceVAEncQMatrixBufferJPEG(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data)
{
    VAQMatrixBufferJPEG *p = (VAQMatrixBufferJPEG *)data;
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\t--VAQMatrixBufferJPEG\n");
    va_TraceMsg(trace_ctx, "\tload_lum_quantiser_matrix = %d", p->load_lum_quantiser_matrix);
    if (p->load_lum_quantiser_matrix) {
        int i;
        for (i = 0; i < 64; i++) {
            if ((i % 8) == 0)
                va_TraceMsg(trace_ctx, "\n\t");
            va_TraceMsg(trace_ctx, "\t0x%02x", p->lum_quantiser_matrix[i]);
        }
        va_TraceMsg(trace_ctx, "\n");
    }
    va_TraceMsg(trace_ctx, "\tload_chroma_quantiser_matrix = %08x\n", p->load_chroma_quantiser_matrix);
    if (p->load_chroma_quantiser_matrix) {
        int i;
        for (i = 0; i < 64; i++) {
            if ((i % 8) == 0)
                va_TraceMsg(trace_ctx, "\n\t");
            va_TraceMsg(trace_ctx, "\t0x%02x", p->chroma_quantiser_matrix[i]);
        }
        va_TraceMsg(trace_ctx, "\n");
    }
    
    va_TraceMsg(trace_ctx, NULL);
    
    return;
}

static void va_TraceH263Buf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    switch (type) {
    case VAPictureParameterBufferType:/* print MPEG4 buffer */
        va_TraceVAPictureParameterBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:/* print MPEG4 buffer */
        va_TraceVAIQMatrixBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VABitPlaneBufferType:/* print MPEG4 buffer */
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceGroupMapBufferType:
        break;
    case VASliceParameterBufferType:/* print MPEG4 buffer */
        va_TraceVASliceParameterBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAMacroblockParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAResidualDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VADeblockingParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAImageBufferType:
        break;
    case VAProtectedSliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncCodedBufferType:
        break;
    case VAEncSequenceParameterBufferType:
        va_TraceVAEncSequenceParameterBufferH263(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPictureParameterBufferType:
        va_TraceVAEncPictureParameterBufferH263(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncSliceParameterBufferType:
        va_TraceVAEncSliceParameterBuffer(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPackedHeaderParameterBufferType:
        va_TraceVAEncPackedHeaderParameterBufferType(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}


static void va_TraceJPEGBuf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    switch (type) {
    case VABitPlaneBufferType:
    case VASliceGroupMapBufferType:
    case VASliceDataBufferType:
    case VAMacroblockParameterBufferType:
    case VAResidualDataBufferType:
    case VADeblockingParameterBufferType:
    case VAImageBufferType:
    case VAProtectedSliceDataBufferType:
    case VAEncCodedBufferType:
    case VAEncSequenceParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncSliceParameterBufferType:
        va_TraceVAEncPictureParameterBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAPictureParameterBufferType:
        va_TraceVAPictureParameterBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:
        va_TraceVAIQMatrixBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceParameterBufferType:
        va_TraceVASliceParameterBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAHuffmanTableBufferType:
        va_TraceVAHuffmanTableBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPictureParameterBufferType:
        va_TraceVAEncPictureParameterBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAQMatrixBufferType:
        va_TraceVAEncQMatrixBufferJPEG(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}

static void va_TraceMPEG4Buf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    switch (type) {
    case VAPictureParameterBufferType:
        va_TraceVAPictureParameterBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:
        va_TraceVAIQMatrixBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VABitPlaneBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceGroupMapBufferType:
        break;
    case VASliceParameterBufferType:
        va_TraceVASliceParameterBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAMacroblockParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAResidualDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VADeblockingParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAImageBufferType:
        break;
    case VAProtectedSliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncCodedBufferType:
        break;
    case VAEncSequenceParameterBufferType:
        va_TraceVAEncSequenceParameterBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPictureParameterBufferType:
        va_TraceVAEncPictureParameterBufferMPEG4(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncSliceParameterBufferType:
        va_TraceVAEncSliceParameterBuffer(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}


static void va_TraceH264Buf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    DPY2TRACECTX(dpy);
    
    switch (type) {
    case VAPictureParameterBufferType:
        va_TraceVAPictureParameterBufferH264(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:
        va_TraceVAIQMatrixBufferH264(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VABitPlaneBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);        
        break;
    case VASliceGroupMapBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceParameterBufferType:
        va_TraceVASliceParameterBufferH264(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, trace_ctx->trace_slice_size, num_elements, pbuf);
        break;
    case VAMacroblockParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);        
        break;
    case VAResidualDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);        
        break;
    case VADeblockingParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAImageBufferType:
        break;
    case VAProtectedSliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncCodedBufferType:
        break;
    case VAEncSequenceParameterBufferType:
        va_TraceVAEncSequenceParameterBufferH264(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPictureParameterBufferType:
        va_TraceVAEncPictureParameterBufferH264(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncSliceParameterBufferType:
        if (size == sizeof(VAEncSliceParameterBuffer))
            va_TraceVAEncSliceParameterBuffer(dpy, context, buffer, type, size, num_elements, pbuf);
        else
            va_TraceVAEncSliceParameterBufferH264(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPackedHeaderParameterBufferType:
        va_TraceVAEncPackedHeaderParameterBufferType(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
        
    case VAEncMiscParameterBufferType:
        va_TraceVAEncMiscParameterBuffer(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}

static void va_TraceVP8Buf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    DPY2TRACECTX(dpy);

    switch (type) {
    case VAPictureParameterBufferType:
        va_TraceVAPictureParameterBufferVP8(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:
        va_TraceVAIQMatrixBufferVP8(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VABitPlaneBufferType:
        break;
    case VASliceGroupMapBufferType:
        break;
    case VASliceParameterBufferType:
        va_TraceVASliceParameterBufferVP8(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceDataBufferType:
        break;
    case VAProbabilityBufferType:
        va_TraceVAProbabilityBufferVP8(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAMacroblockParameterBufferType:
        break;
    case VAResidualDataBufferType:
        break;
    case VADeblockingParameterBufferType:
        break;
    case VAImageBufferType:
        break;
    case VAProtectedSliceDataBufferType:
        break;
    case VAEncCodedBufferType:
        break;
    case VAEncSequenceParameterBufferType:
        break;
    case VAEncPictureParameterBufferType:
        break;
    case VAEncSliceParameterBufferType:
        break;
    case VAEncPackedHeaderParameterBufferType:
        break;
    case VAEncMiscParameterBufferType:
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}

static void va_TraceVC1Buf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    DPY2TRACECTX(dpy);

    switch (type) {
    case VAPictureParameterBufferType:
        va_TraceVAPictureParameterBufferVC1(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAIQMatrixBufferType:
        break;
    case VABitPlaneBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceGroupMapBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceParameterBufferType:
        va_TraceVASliceParameterBufferVC1(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VASliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, trace_ctx->trace_slice_size, num_elements, pbuf);
        break;
    case VAMacroblockParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAResidualDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VADeblockingParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAImageBufferType:
        break;
    case VAProtectedSliceDataBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncCodedBufferType:
        break;
    case VAEncSequenceParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncPictureParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    case VAEncSliceParameterBufferType:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}

static void
va_TraceProcFilterParameterBufferDeinterlacing(
    VADisplay dpy,
    VAContextID context,
    VAProcFilterParameterBufferBase *base
)
{
    VAProcFilterParameterBufferDeinterlacing *deint = (VAProcFilterParameterBufferDeinterlacing *)base;

    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx, "\t    type = %d\n", deint->type);
    va_TraceMsg(trace_ctx, "\t    algorithm = %d\n", deint->algorithm);
    va_TraceMsg(trace_ctx, "\t    flags = %d\n", deint->flags);
}

static void
va_TraceProcFilterParameterBufferColorBalance(
    VADisplay dpy,
    VAContextID context,
    VAProcFilterParameterBufferBase *base
)
{
    VAProcFilterParameterBufferColorBalance *color_balance = (VAProcFilterParameterBufferColorBalance *)base;

    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx, "\t    type = %d\n", color_balance->type);
    va_TraceMsg(trace_ctx, "\t    attrib = %d\n", color_balance->attrib);
    va_TraceMsg(trace_ctx, "\t    value = %f\n", color_balance->value);
}

static void
va_TraceProcFilterParameterBufferBase(
    VADisplay dpy,
    VAContextID context,
    VAProcFilterParameterBufferBase *base
)
{
    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx, "\t    type = %d\n", base->type);
}

static void
va_TraceProcFilterParameterBuffer(
    VADisplay dpy,
    VAContextID context,
    VABufferID *filters,
    unsigned int num_filters
)
{
    VABufferType type;
    unsigned int size;
    unsigned int num_elements;
    VAProcFilterParameterBufferBase *base_filter = NULL;
    int i;

    DPY2TRACECTX(dpy);

    if (num_filters == 0 || filters == NULL) {
        va_TraceMsg(trace_ctx, "\t  num_filters = %d\n", num_filters);
        va_TraceMsg(trace_ctx, "\t  filters = %p\n", filters);
        return;
    }

    va_TraceMsg(trace_ctx, "\t  num_filters = %d\n", num_filters);

    /* get buffer type information */
    for (i = 0; i < num_filters; i++) {
        vaBufferInfo(dpy, context, filters[i], &type, &size, &num_elements);

        if (type != VAProcFilterParameterBufferType) {
            va_TraceMsg(trace_ctx, "\t  filters[%d] = 0x%08x (INVALID)\n", i, filters[i]);
            return;
        } else {
            va_TraceMsg(trace_ctx, "\t  filters[%d] = 0x%08x\n", i, filters[i]);
        }

        base_filter = NULL;
        vaMapBuffer(dpy, filters[i], (void **)&base_filter);

        if (base_filter == NULL) {
            vaUnmapBuffer(dpy, filters[i]);
            return;
        }

        switch (base_filter->type) {
        case VAProcFilterDeinterlacing:
            va_TraceProcFilterParameterBufferDeinterlacing(dpy,
                                                           context,
                                                           base_filter);
            break;
        case VAProcFilterColorBalance:
            va_TraceProcFilterParameterBufferColorBalance(dpy,
                                                          context,
                                                          base_filter);
            break;
        default:
            va_TraceProcFilterParameterBufferBase(dpy,
                                                  context,
                                                  base_filter);
            break;
        }

        vaUnmapBuffer(dpy, filters[i]);
    }
}

static void
va_TraceVAProcPipelineParameterBuffer(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *data
)
{
    VAProcPipelineParameterBuffer *p = (VAProcPipelineParameterBuffer *)data;
    int i;

    DPY2TRACECTX(dpy);

    va_TraceMsg(trace_ctx, "\t--VAProcPipelineParameterBuffer\n");

    va_TraceMsg(trace_ctx, "\t  surface = 0x%08x\n", p->surface);

    if (p->surface_region) {
        va_TraceMsg(trace_ctx, "\t  surface_region\n");
        va_TraceMsg(trace_ctx, "\t    x = %d\n", p->surface_region->x);
        va_TraceMsg(trace_ctx, "\t    y = %d\n", p->surface_region->y);
        va_TraceMsg(trace_ctx, "\t    width = %d\n", p->surface_region->width);
        va_TraceMsg(trace_ctx, "\t    height = %d\n", p->surface_region->height);
    } else {
        va_TraceMsg(trace_ctx, "\t  surface_region = (NULL)\n");
    }

    va_TraceMsg(trace_ctx, "\t  surface_color_standard = %d\n", p->surface_color_standard);

    if (p->output_region) {
        va_TraceMsg(trace_ctx, "\t  output_region\n");
        va_TraceMsg(trace_ctx, "\t    x = %d\n", p->output_region->x);
        va_TraceMsg(trace_ctx, "\t    y = %d\n", p->output_region->y);
        va_TraceMsg(trace_ctx, "\t    width = %d\n", p->output_region->width);
        va_TraceMsg(trace_ctx, "\t    height = %d\n", p->output_region->height);
    } else {
        va_TraceMsg(trace_ctx, "\t  output_region = (NULL)\n");
    }

    va_TraceMsg(trace_ctx, "\t  output_background_color = 0x%08x\n", p->output_background_color);
    va_TraceMsg(trace_ctx, "\t  output_color_standard = %d\n", p->output_color_standard);
    va_TraceMsg(trace_ctx, "\t  pipeline_flags = 0x%08x\n", p->pipeline_flags);
    va_TraceMsg(trace_ctx, "\t  filter_flags = 0x%08x\n", p->filter_flags);

    va_TraceProcFilterParameterBuffer(dpy, context, p->filters, p->num_filters);

    va_TraceMsg(trace_ctx, "\t  num_forward_references = 0x%08x\n", p->num_forward_references);

    if (p->num_forward_references) {
        va_TraceMsg(trace_ctx, "\t  forward_references\n");

        if (p->forward_references) {
            /* only dump the first 5 forward references */
            for (i = 0; i < p->num_forward_references && i < 5; i++) {
                va_TraceMsg(trace_ctx, "\t    forward_references[%d] = 0x%08x\n", i, p->forward_references[i]);
            }
        } else {
            for (i = 0; i < p->num_forward_references && i < 5; i++) {
                va_TraceMsg(trace_ctx, "\t    forward_references[%d] = (NULL)\n", i);
            }
        }
    }

    va_TraceMsg(trace_ctx, "\t  num_backward_references = 0x%08x\n", p->num_backward_references);

    if (p->num_backward_references) {
        va_TraceMsg(trace_ctx, "\t  backward_references\n");

        if (p->backward_references) {
            /* only dump the first 5 backward references */
            for (i = 0; i < p->num_backward_references && i < 5; i++) {
                va_TraceMsg(trace_ctx, "\t    backward_references[%d] = 0x%08x\n", i, p->backward_references[i]);
            }
        } else {
            for (i = 0; i < p->num_backward_references && i < 5; i++) {
                va_TraceMsg(trace_ctx, "\t    backward_references[%d] = (NULL)\n", i);
            }
        }
    }

    /* FIXME: add other info later */

    va_TraceMsg(trace_ctx, NULL);
}

static void
va_TraceNoneBuf(
    VADisplay dpy,
    VAContextID context,
    VABufferID buffer,
    VABufferType type,
    unsigned int size,
    unsigned int num_elements,
    void *pbuf
)
{
    DPY2TRACECTX(dpy);

    switch (type) {
    case VAProcPipelineParameterBufferType:
        va_TraceVAProcPipelineParameterBuffer(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    default:
        va_TraceVABuffers(dpy, context, buffer, type, size, num_elements, pbuf);
        break;
    }
}

void va_TraceRenderPicture(
    VADisplay dpy,
    VAContextID context,
    VABufferID *buffers,
    int num_buffers
)
{
    VABufferType type;
    unsigned int size;
    unsigned int num_elements;
    int i;
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);
    
    va_TraceMsg(trace_ctx, "\tcontext = 0x%08x\n", context);
    va_TraceMsg(trace_ctx, "\tnum_buffers = %d\n", num_buffers);
    if (buffers == NULL)
        return;
    
    for (i = 0; i < num_buffers; i++) {
        unsigned char *pbuf = NULL;
        unsigned int j;
        
        /* get buffer type information */
        vaBufferInfo(dpy, context, buffers[i], &type, &size, &num_elements);

        va_TraceMsg(trace_ctx, "\t---------------------------\n");
        va_TraceMsg(trace_ctx, "\tbuffers[%d] = 0x%08x\n", i, buffers[i]);
        va_TraceMsg(trace_ctx, "\t  type = %s\n", buffer_type_to_string(type));
        va_TraceMsg(trace_ctx, "\t  size = %d\n", size);
        va_TraceMsg(trace_ctx, "\t  num_elements = %d\n", num_elements);

        vaMapBuffer(dpy, buffers[i], (void **)&pbuf);
        if (pbuf == NULL)
            continue;
        
        switch (trace_ctx->trace_profile) {
        case VAProfileMPEG2Simple:
        case VAProfileMPEG2Main:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);
                va_TraceMPEG2Buf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;
        case VAProfileMPEG4Simple:
        case VAProfileMPEG4AdvancedSimple:
        case VAProfileMPEG4Main:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);
                va_TraceMPEG4Buf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;
        case VAProfileH264Baseline:
        case VAProfileH264Main:
        case VAProfileH264High:
        case VAProfileH264ConstrainedBaseline:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);
                
                va_TraceH264Buf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;
        case VAProfileVC1Simple:
        case VAProfileVC1Main:
        case VAProfileVC1Advanced:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);
                
                va_TraceVC1Buf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;
        case VAProfileH263Baseline:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);
                
                va_TraceH263Buf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;
        case VAProfileJPEGBaseline:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);
                
                va_TraceJPEGBuf (dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;

        case VAProfileNone:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);

                va_TraceNoneBuf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;

        case VAProfileVP8Version0_3:
            for (j=0; j<num_elements; j++) {
                va_TraceMsg(trace_ctx, "\telement[%d] =\n", j);

                va_TraceVP8Buf(dpy, context, buffers[i], type, size, num_elements, pbuf + size*j);
            }
            break;

        default:
            break;
        }

        vaUnmapBuffer(dpy, buffers[i]);
    }

    va_TraceMsg(trace_ctx, NULL);
}

void va_TraceEndPicture(
    VADisplay dpy,
    VAContextID context,
    int endpic_done
)
{
    int encode, decode, jpeg;
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\tcontext = 0x%08x\n", context);
    va_TraceMsg(trace_ctx, "\trender_targets = 0x%08x\n", trace_ctx->trace_rendertarget);

    /* avoid to create so many empty files */
    encode = (trace_ctx->trace_entrypoint == VAEntrypointEncSlice);
    decode = (trace_ctx->trace_entrypoint == VAEntrypointVLD);
    jpeg = (trace_ctx->trace_entrypoint == VAEntrypointEncPicture);

    /* trace encode source surface, can do it before HW completes rendering */
    if ((encode && (trace_flag & VA_TRACE_FLAG_SURFACE_ENCODE))||
        (jpeg && (trace_flag & VA_TRACE_FLAG_SURFACE_JPEG)))
        va_TraceSurface(dpy);
    
    /* trace decoded surface, do it after HW completes rendering */
    if (decode && ((trace_flag & VA_TRACE_FLAG_SURFACE_DECODE))) {
        vaSyncSurface(dpy, trace_ctx->trace_rendertarget);
        va_TraceSurface(dpy);
    }

    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceSyncSurface(
    VADisplay dpy,
    VASurfaceID render_target
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\trender_target = 0x%08x\n", render_target);
    va_TraceMsg(trace_ctx, NULL);
}

void va_TraceQuerySurfaceAttributes(
    VADisplay           dpy,
    VAConfigID          config,
    VASurfaceAttrib    *attrib_list,
    unsigned int       *num_attribs
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);
    va_TraceMsg(trace_ctx, "\tconfig = 0x%08x\n", config);
    va_TraceSurfaceAttributes(trace_ctx, attrib_list, num_attribs);
    
    va_TraceMsg(trace_ctx, NULL);

}


void va_TraceQuerySurfaceStatus(
    VADisplay dpy,
    VASurfaceID render_target,
    VASurfaceStatus *status    /* out */
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceMsg(trace_ctx, "\trender_target = 0x%08x\n", render_target);
    if (status)
        va_TraceMsg(trace_ctx, "\tstatus = 0x%08x\n", *status);
    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceQuerySurfaceError(
    VADisplay dpy,
    VASurfaceID surface,
    VAStatus error_status,
    void **error_info       /*out*/
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);
    va_TraceMsg(trace_ctx, "\tsurface = 0x%08x\n", surface);
    va_TraceMsg(trace_ctx, "\terror_status = 0x%08x\n", error_status);
    if (error_info && (error_status == VA_STATUS_ERROR_DECODING_ERROR)) {
        VASurfaceDecodeMBErrors *p = *error_info;
        while (p && (p->status != -1)) {
            va_TraceMsg(trace_ctx, "\t\tstatus = %d\n", p->status);
            va_TraceMsg(trace_ctx, "\t\tstart_mb = %d\n", p->start_mb);
            va_TraceMsg(trace_ctx, "\t\tend_mb = %d\n", p->end_mb);
            p++; /* next error record */
        }
    }
    va_TraceMsg(trace_ctx, NULL);
}

void va_TraceMaxNumDisplayAttributes (
    VADisplay dpy,
    int number
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);
    
    va_TraceMsg(trace_ctx, "\tmax_display_attributes = %d\n", number);
    va_TraceMsg(trace_ctx, NULL);
}

void va_TraceQueryDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,    /* out */
    int *num_attributes               /* out */
)
{
    int i;
    
    DPY2TRACECTX(dpy);
    
    if (attr_list == NULL || num_attributes == NULL)
        return;

    va_TraceMsg(trace_ctx, "\tnum_attributes = %d\n", *num_attributes);
    
    for (i=0; i<*num_attributes; i++) {
        va_TraceMsg(trace_ctx, "\tattr_list[%d] =\n");
        va_TraceMsg(trace_ctx, "\t  typ = 0x%08x\n", attr_list[i].type);
        va_TraceMsg(trace_ctx, "\t  min_value = %d\n", attr_list[i].min_value);
        va_TraceMsg(trace_ctx, "\t  max_value = %d\n", attr_list[i].max_value);
        va_TraceMsg(trace_ctx, "\t  value = %d\n", attr_list[i].value);
        va_TraceMsg(trace_ctx, "\t  flags = %d\n", attr_list[i].flags);
    }
    va_TraceMsg(trace_ctx, NULL);
}


static void va_TraceDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,
    int num_attributes
)
{
    int i;
    
    DPY2TRACECTX(dpy);
    
    va_TraceMsg(trace_ctx, "\tnum_attributes = %d\n", num_attributes);
    if (attr_list == NULL)
        return;
    
    for (i=0; i<num_attributes; i++) {
        va_TraceMsg(trace_ctx, "\tattr_list[%d] =\n");
        va_TraceMsg(trace_ctx, "\t  typ = 0x%08x\n", attr_list[i].type);
        va_TraceMsg(trace_ctx, "\t  min_value = %d\n", attr_list[i].min_value);
        va_TraceMsg(trace_ctx, "\t  max_value = %d\n", attr_list[i].max_value);
        va_TraceMsg(trace_ctx, "\t  value = %d\n", attr_list[i].value);
        va_TraceMsg(trace_ctx, "\t  flags = %d\n", attr_list[i].flags);
    }
    va_TraceMsg(trace_ctx, NULL);
}


void va_TraceGetDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,
    int num_attributes
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceDisplayAttributes (dpy, attr_list, num_attributes);
}

void va_TraceSetDisplayAttributes (
    VADisplay dpy,
    VADisplayAttribute *attr_list,
    int num_attributes
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);

    va_TraceDisplayAttributes (dpy, attr_list, num_attributes);
}


void va_TracePutSurface (
    VADisplay dpy,
    VASurfaceID surface,
    void *draw, /* the target Drawable */
    short srcx,
    short srcy,
    unsigned short srcw,
    unsigned short srch,
    short destx,
    short desty,
    unsigned short destw,
    unsigned short desth,
    VARectangle *cliprects, /* client supplied clip list */
    unsigned int number_cliprects, /* number of clip rects in the clip list */
    unsigned int flags /* de-interlacing flags */
)
{
    DPY2TRACECTX(dpy);

    TRACE_FUNCNAME(idx);
    
    va_TraceMsg(trace_ctx, "\tsurface = 0x%08x\n", surface);
    va_TraceMsg(trace_ctx, "\tdraw = 0x%08x\n", draw);
    va_TraceMsg(trace_ctx, "\tsrcx = %d\n", srcx);
    va_TraceMsg(trace_ctx, "\tsrcy = %d\n", srcy);
    va_TraceMsg(trace_ctx, "\tsrcw = %d\n", srcw);
    va_TraceMsg(trace_ctx, "\tsrch = %d\n", srch);
    va_TraceMsg(trace_ctx, "\tdestx = %d\n", destx);
    va_TraceMsg(trace_ctx, "\tdesty = %d\n", desty);
    va_TraceMsg(trace_ctx, "\tdestw = %d\n", destw);
    va_TraceMsg(trace_ctx, "\tdesth = %d\n", desth);
    va_TraceMsg(trace_ctx, "\tcliprects = 0x%08x\n", cliprects);
    va_TraceMsg(trace_ctx, "\tnumber_cliprects = %d\n", number_cliprects);
    va_TraceMsg(trace_ctx, "\tflags = 0x%08x\n", flags);
    va_TraceMsg(trace_ctx, NULL);
}
