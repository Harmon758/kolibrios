#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <drm/i915_drm.h>
#include <kos32sys.h>
#include "pixlib3.h"
#include "pixdriver.h"
#include "pxgl.h"

#define WIN_STATE_MINIMIZED  0x02
#define WIN_STATE_ROLLED     0x04

static int drm_ioctl(int fd, unsigned long request, void *arg)
{
    ioctl_t  io;

    io.handle   = fd;
    io.io_code  = request;
    io.input    = arg;
    io.inp_size = 64;
    io.output   = NULL;
    io.out_size = 0;

    return call_service(&io);
}

static EGLImageKHR px_create_image(struct render *px,void *name,GLuint tex, EGLint *attribs)
{
    EGLImageKHR image;

    image = eglCreateImageKHR(px->dpy, px->context,
                              EGL_DRM_BUFFER_MESA, name, attribs);
    if(image == NULL)
        goto err_0;

    glBindTexture(GL_TEXTURE_2D, tex);
    if(glGetError() != GL_NO_ERROR)
       goto err_1;

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
    if(glGetError() != GL_NO_ERROR)
       goto err_1;

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    return image;

err_1:
    eglDestroyImageKHR(px->dpy, image);
err_0:
    return NULL;
};

static int update_fb(struct render *px, int name, int pitch)
{
    GLenum      status;
    EGLImageKHR screen;

    EGLint attribs[] = {
        EGL_WIDTH, px->scr_width,
        EGL_HEIGHT, px->scr_height,
        EGL_DRM_BUFFER_STRIDE_MESA, pitch/4,
        EGL_DRM_BUFFER_FORMAT_MESA,
        EGL_DRM_BUFFER_FORMAT_ARGB32_MESA,
        EGL_DRM_BUFFER_USE_MESA,
        EGL_DRM_BUFFER_USE_SHARE_MESA |
        EGL_DRM_BUFFER_USE_SCANOUT_MESA,
        EGL_NONE
    };
    char *errmsg;

    px->screen = px_create_image(px,(void*)name, px->texture[TEX_SCREEN], attribs);
    errmsg = "failed to create new screen image\n";
    if(px->screen == EGL_NO_IMAGE_KHR)
        goto err_0;

    px->screen = screen;

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
                       GL_TEXTURE_2D, px->texture[TEX_SCREEN],0);

    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        const char *str;
        switch (status)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                str = "incomplete attachment";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                str = "incomplete/missing attachment";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                str = "incomplete draw buffer";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                str = "incomplete read buffer";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                str = "unsupported";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                str = "incomplete multiple";
                break;
            default:
                str = "unknown error";
                break;
        }
        DBG("destination framebuffer incomplete: %s [%#x]\n", str, status);

        return -1;
    };

    DBG("framebuffer changed successfull\n");

    glViewport(0, 0, px->scr_width, px->scr_height);

    return 0;

err_0:
    DBG("%s %s\n", __FUNCTION__, errmsg);

    return -1;
};

static GLint create_shader(GLenum type, const char *source)
{
    GLint ok;
    GLint shader;

    shader = glCreateShader(type);
    if(shader == 0)
        goto err;

    glShaderSource(shader, 1, (const GLchar **) &source, NULL);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLchar *info;
        GLint size;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
        info = malloc(size);

        glGetShaderInfoLog(shader, size, NULL, info);
        DBG("Failed to compile %s: %s\n",
                type == GL_FRAGMENT_SHADER ? "FS" : "VS",info);
        DBG("Program source:\n%s", source);
        DBG("GLSL compile failure\n");
        free(info);
        glDeleteShader(shader);
        shader = 0;
    };

    DBG("create shader %d\n", shader);
err:
    return shader;
}


static void *px_create_obj(struct render *px, size_t size,
                           GLuint *handle, GLuint *name)
{
    struct drm_i915_gem_create create;
    struct drm_i915_gem_mmap mmap_arg;
    struct drm_gem_close close;
    struct drm_gem_flink flink;

    create.size   = size;
    create.handle = 0;

    if(drm_ioctl(px->fd, DRM_IOCTL_I915_GEM_CREATE, &create))
        goto err_0;

    *handle      = create.handle;
    flink.handle = create.handle;
    if (drm_ioctl(px->fd, DRM_IOCTL_GEM_FLINK, &flink))
        goto err_1;

    *name = flink.name;

    mmap_arg.handle = *handle;
    mmap_arg.offset = 0;
    mmap_arg.size   = size;
    if (drm_ioctl(px->fd, DRM_IOCTL_I915_GEM_MMAP, &mmap_arg))
    {
        DBG("%s: failed to mmap bitmap handle=%d, %d bytes, into CPU domain\n",
               __FUNCTION__, *handle, size);
        goto err_1;
    };

    return (void*)(uintptr_t)mmap_arg.addr_ptr;

err_1:
    close.handle = *handle;
    drm_ioctl(px->fd, DRM_IOCTL_GEM_CLOSE, &close);
err_0:
    return NULL;
};


static int px_create_bitmap(struct render *px, struct bitmap *bitmap,
                            size_t size, EGLint format)
{
    struct drm_gem_close close;
    EGLint attribs[] = {
        EGL_WIDTH, bitmap->width,
        EGL_HEIGHT, bitmap->height,
        EGL_DRM_BUFFER_STRIDE_MESA, 0,
        EGL_DRM_BUFFER_FORMAT_MESA,
        format,
        EGL_DRM_BUFFER_USE_MESA,
        EGL_DRM_BUFFER_USE_SHARE_MESA,
        EGL_NONE
    };

    bitmap->buffer = px_create_obj(px, size, &bitmap->handle, &bitmap->name);
    if(bitmap->buffer == NULL)
        goto err_0;

    switch(format)
    {
        case EGL_DRM_BUFFER_FORMAT_ARGB32_MESA:
            attribs[5] = bitmap->pitch/4;
            break;
        case EGL_DRM_BUFFER_FORMAT_R8_MESA:
            attribs[5] = bitmap->pitch;
            break;
        default:
            DBG("%s invalid format %x\n",
                    __FUNCTION__, format);
            goto err_1;
    }

    bitmap->image = px_create_image(px,(void*)bitmap->name,
                                    bitmap->tex, attribs);
    if(bitmap->image == NULL)
        goto err_1;

    DBG("create bitmap:%p %dx%d pitch:%d\n"
           "KHRimage:%p gl_tex:%d handle:%d name:%d\n"
           "mapped at %x\n",
            bitmap, bitmap->width, bitmap->height, bitmap->pitch,
            bitmap->image, bitmap->tex, bitmap->handle, bitmap->name,
            bitmap->buffer);

    return 0;

err_1:
    user_free(bitmap->buffer);
    close.handle = bitmap->handle;
    drm_ioctl(px->fd, DRM_IOCTL_GEM_CLOSE, &close);
err_0:
    return -1;
}


static int create_mask(struct render *px)
{
    struct drm_i915_mask_update update;
    int    pitch;

    pitch = (px->width+15) & ~15;

    px->mask.width  = px->width;
    px->mask.height = px->height;
    px->mask.pitch  = pitch;
    px->mask.tex    = px->texture[TEX_MASK];
    px->mask_size   = pitch * px->height;

    if(px_create_bitmap(px, &px->mask, px->mask_size,
                         EGL_DRM_BUFFER_FORMAT_R8_MESA) == 0)
    {
        update.handle = px->mask.handle;
        update.dx     = px->dx;
        update.dy     = px->dy;
        update.width  = px->width;
        update.height = px->height;
        update.bo_pitch = (px->width+15) & ~15;
        update.bo_map = (int)px->mask.buffer;
        update.forced = 1;
        drm_ioctl(px->fd, SRV_MASK_UPDATE_EX, &update);
        return 0;
    };
    return -1;
};


static int update_mask(struct render *px)
{
    struct drm_gem_close close;
    EGLint attribs[] = {
        EGL_WIDTH, 0,
        EGL_HEIGHT, 0,
        EGL_DRM_BUFFER_STRIDE_MESA, 0,
        EGL_DRM_BUFFER_FORMAT_MESA,
        EGL_DRM_BUFFER_FORMAT_R8_MESA,
        EGL_DRM_BUFFER_USE_MESA,
        EGL_DRM_BUFFER_USE_SHARE_MESA,
        EGL_NONE
    };
    unsigned int size, pitch;

    eglDestroyImageKHR(px->dpy, px->mask.image);

    pitch = (px->width+15) & ~15;
    size = pitch*px->height;

    if(size > px->mask_size)
    {
        user_free(px->mask.buffer);
        close.handle = px->mask.handle;
        drm_ioctl(px->fd, DRM_IOCTL_GEM_CLOSE, &close);

        px->mask_size = size;

        px->mask.buffer = px_create_obj(px, px->mask_size,
                                      &px->mask.handle, &px->mask.name);
        if(px->mask.buffer == 0)
            goto err_0;
    };

    attribs[1] = px->width;
    attribs[3] = px->height;
    attribs[5] = pitch;

    px->mask.image = px_create_image(px,(void*)px->mask.name,
                                     px->mask.tex, attribs);
    if(px->mask.image == NULL)
        goto err_1;

    DBG("create mask w:%d h:%d pitch:%d\n"
        "KHRimage:%p gl_tex:%d handle:%d name:%d\n"
        "mapped at %x\n",
         px->width, px->height,
         pitch, px->mask.image, px->texture[TEX_MASK],
         px->mask.handle, px->mask.name, px->mask.buffer);

    return 0;

err_1:
    user_free(px->mask.buffer);
    close.handle = px->mask.handle;
    drm_ioctl(px->fd, DRM_IOCTL_GEM_CLOSE, &close);
err_0:
    return -1;
};

static struct render* create_render(EGLDisplay dpy, EGLContext context, int dx, int dy, int w, int h)
{
    const char *vs_src =
        "attribute vec4 v_position;\n"
        "attribute vec4 v_texcoord0;\n"
        "varying vec2 source_texture;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = v_position;\n"
        "   source_texture = v_texcoord0.xy;\n"
        "}\n";

    const char *fs_i965 =
        "varying vec2 source_texture;\n"
        "uniform sampler2D sampler_src;\n"
        "uniform sampler2D sampler_mask;\n"
        "void main()\n"
        "{\n"
        "   float ca = texture2D(sampler_mask, source_texture).r;\n"
        "   gl_FragColor = vec4(texture2D(sampler_src, source_texture).rgb, ca);\n"
        "}\n";

    struct drm_i915_fb_info fb;
    struct render *px;
    GLint  vs_shader, fs_shader;
    int    ret;

    px = (struct render*)malloc(sizeof(struct render));
    if(px == NULL)
        goto err_0;

    px->fd  = get_service("DISPLAY");

    __builtin_memset(&fb, 0, sizeof(fb));
    if( 0 != drm_ioctl(px->fd, SRV_FBINFO, &fb))
    {   DBG("failed to get framebuffer info\n");
        goto err_1;
    };

    px->dpy     = dpy;
    px->context = context;
    px->dx      = dx;
    px->dy      = dy;
    px->width   = w;
    px->height  = h;
    px->scr_width  = fb.width;
    px->scr_height = fb.height;

    glGenTextures(2, px->texture);
    if(glGetError() != GL_NO_ERROR)
       goto err_1;

    glGenFramebuffers(1, &px->framebuffer);
    if(glGetError() != GL_NO_ERROR)
       goto err_2;

    glBindFramebuffer(GL_FRAMEBUFFER, px->framebuffer);
    if(glGetError() != GL_NO_ERROR)
       goto err_3;

    if(update_fb(px, fb.name, fb.pitch))
       goto err_4;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(create_mask(px))
        goto err_4;

    px->blit_prog = glCreateProgram();
    if(px->blit_prog == 0)
        goto err_4;

    vs_shader = create_shader(GL_VERTEX_SHADER, vs_src);
    if(vs_shader == 0)
        goto err_4;

    fs_shader = create_shader(GL_FRAGMENT_SHADER, fs_i965);
    if(fs_shader == 0)
        goto err_4;

    glAttachShader(px->blit_prog, vs_shader);
    glAttachShader(px->blit_prog, fs_shader);
    glBindAttribLocation(px->blit_prog, 0, "v_position");
    glBindAttribLocation(px->blit_prog, 1, "v_texcoord0");

    glLinkProgram(px->blit_prog);
    glGetProgramiv(px->blit_prog, GL_LINK_STATUS, &ret);
    if (!ret)
    {
        GLchar *info;
        GLint size;

        glGetProgramiv(px->blit_prog, GL_INFO_LOG_LENGTH, &size);
        info = malloc(size);

        glGetProgramInfoLog(px->blit_prog, size, NULL, info);
        DBG("Failed to link: %s\n", info);
        DBG("GLSL link failure\n");
        free(info);
        goto err_4;
    }

    px->sampler = glGetUniformLocation(px->blit_prog,"sampler_src");
    px->sm_mask = glGetUniformLocation(px->blit_prog,"sampler_mask");

    glUseProgram(px->blit_prog);
    glUniform1i(px->sampler, 0);
    glUniform1i(px->sm_mask, 1);

    glEnableClientState(GL_VERTEX_ARRAY);

    glVertexAttribPointer(0, 2, GL_FLOAT,GL_FALSE, 2 * sizeof(float),px->vertices);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 2 * sizeof(float),px->tc_src);
    glEnableVertexAttribArray(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    px->tc_src[0]    = 0.0;
    px->tc_src[1]    = 0.0;
    px->tc_src[1*2]  = 1.0;
    px->tc_src[1*2+1]= 0.0;
    px->tc_src[2*2]  = 1.0;
    px->tc_src[2*2+1]= 1.0;
    px->tc_src[3*2]  = 0.0;
    px->tc_src[3*2+1]= 1.0;

    DBG("create render framebuffer:%p %dx%d pitch:%d name %d\n",
        px->screen,fb.width, fb.height, fb.pitch, fb.name);

    return px;

err_4:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
err_3:
    glDeleteFramebuffers(1, &px->framebuffer);
err_2:
    glDeleteTextures(2, px->texture);
err_1:
    free(px);
err_0:
    return NULL;
};

static struct render *px;

static bitmap_t *hw_create_bitmap(uint32_t width, uint32_t height)
{
    struct bitmap *bitmap;

    bitmap = malloc(sizeof(struct bitmap));
    if(bitmap == NULL)
        return NULL;

    glGenTextures(1, &bitmap->tex);
    if(glGetError() != GL_NO_ERROR)
       goto err_0;

    bitmap->width   = width;
    bitmap->height  = height;
    bitmap->pitch   = (width*4 + 15) & ~15;

    if(px_create_bitmap(px, bitmap, bitmap->pitch * bitmap->height,
                         EGL_DRM_BUFFER_FORMAT_ARGB32_MESA) == 0)
    {
        return bitmap;
    };

    glDeleteTextures(1, &bitmap->tex);
err_0:
    free(bitmap);
    return NULL;
};

static int hw_destroy_bitmap(bitmap_t * bitmap)
{
    struct drm_gem_close close;

    glDeleteTextures(1, &bitmap->tex);
    eglDestroyImageKHR(px->dpy, bitmap->image);
    user_free(bitmap->buffer);
    close.handle = bitmap->handle;
    drm_ioctl(px->fd, DRM_IOCTL_GEM_CLOSE, &close);
    free(bitmap);
};

static void *hw_lock_bitmap(bitmap_t *bitmap, uint32_t *pitch)
{
    *pitch = bitmap->pitch;

    return bitmap->buffer;
};

static int hw_resize_bitmap(bitmap_t * bitmap, uint32_t width, uint32_t height)
{
    return 0;
}

char proc_info[1024];

static int hw_blit(bitmap_t *bitmap, int dst_x, int dst_y,
                   uint32_t w, uint32_t h, int src_x, int src_y)
{
    struct drm_i915_mask_update update;
    struct drm_i915_fb_info fb;

    uint32_t winx, winy, winw, winh;
    uint8_t  state;
    float xscale, yscale;
    int r, b;
    float *vertices  = px->vertices;

    get_proc_info(proc_info);

    state  = *(uint8_t*)(proc_info+70);
    if(state & (WIN_STATE_MINIMIZED|WIN_STATE_ROLLED))
        return;

    winx = *(uint32_t*)(proc_info+34);
    winy = *(uint32_t*)(proc_info+38);
    winw = *(uint32_t*)(proc_info+42)+1;
    winh = *(uint32_t*)(proc_info+46)+1;

    __builtin_memset(&fb, 0, sizeof(fb));
    if( 0 != drm_ioctl(px->fd, SRV_FBINFO, &fb))
    {
        DBG("failed to get framebuffer info\n");
        return;
    };

    if( fb.width  != px->scr_width ||
        fb.height != px->scr_height )
    {
        px->scr_width  = fb.width;
        px->scr_height = fb.height;

        eglDestroyImageKHR(px->dpy, px->screen);

        if(update_fb(px, fb.name, fb.pitch))
            return;
    };

    update.handle = px->mask.handle;
    update.dx     = px->dx;
    update.dy     = px->dy;
    update.width  = px->width;
    update.height = px->height;
    update.bo_pitch = (px->width+15) & ~15;
    update.bo_map = (int)px->mask.buffer;

    if(drm_ioctl(px->fd, SRV_MASK_UPDATE_EX, &update))
    {
        return;
    }

    xscale = 1.0/px->scr_width;
    yscale = 1.0/px->scr_height;

    r = winx + px->dx + px->width;
    b = winy + px->dy + px->height;

    float t0, t1, t2, t5;

//    render->tc_src[1*2]  = 1.0;
//    render->tc_src[2*2]  = 1.0;
//    render->tc_src[2*2+1]= 1.0;
//    render->tc_src[3*2+1]= 1.0;

    vertices[0]     = t0 = 2*(winx+px->dx)*xscale - 1.0;
    vertices[1 * 2] = t2 = 2*r*xscale - 1.0;

    vertices[2 * 2] = t2;
    vertices[3 * 2] = t0;

    vertices[1]     = t1 = 2*(winy+px->dy)*yscale - 1.0;
    vertices[2*2+1] = t5 = 2*b*yscale - 1.0;
    vertices[1*2+1] = t1;
    vertices[3*2+1] = t5;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bitmap->tex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, px->texture[TEX_MASK]);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glFlush();

    return;
};

static int hw_create_client(int x, int y, uint32_t width, uint32_t height)
{
    px = create_render(eglGetCurrentDisplay(), eglGetCurrentContext(), x, y, width, height);

    return !px;
};

static int hw_resize_client(int x, int y, uint32_t width, uint32_t height)
{
    if( x != px->dx ||
        y != px->dy ||
        width != px->width ||
        height != px->height )
    {
        struct drm_i915_mask_update update;

        px->dx     = x;
        px->dy     = y;
        px->width  = width;
        px->height = height;
        update_mask(px);

        update.handle = px->mask.handle;
        update.dx     = px->dx;
        update.dy     = px->dy;
        update.width  = px->width;
        update.height = px->height;
        update.bo_pitch = (px->width+15) & ~15;
        update.bo_map = (int)px->mask.buffer;
        update.forced = 1;

        drm_ioctl(px->fd, SRV_MASK_UPDATE_EX, &update);
    };

    return 0;
};

static void hw_fini(void)
{
    struct drm_gem_close close;

    DBG("%s\n", __FUNCTION__);

    eglDestroyImageKHR(px->dpy, px->mask.image);
    user_free(px->mask.buffer);
    close.handle = px->mask.handle;
    drm_ioctl(px->fd, DRM_IOCTL_GEM_CLOSE, &close);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &px->framebuffer);
    eglDestroyImageKHR(px->dpy, px->screen);
    glDeleteTextures(2, px->texture);
    egl_destroy(px->dpy, px->context);
    free(px);
};

static struct pix_driver gl_driver =
{
    HW_TEX_BLIT,
    hw_create_bitmap,
    hw_destroy_bitmap,
    hw_lock_bitmap,
    hw_resize_bitmap,
    hw_blit,
    hw_create_client,
    hw_resize_client,
    hw_fini
};

struct pix_driver *DrvInit(uint32_t service)
{
    EGLDisplay     dpy;
    EGLConfig      config;
    EGLContext     context;
    int            ret;

    ret = egl_initialize(&dpy, &config, &context);
    if( ret )
        return NULL;

    return &gl_driver;

err_0:
    egl_destroy(dpy, context);
    return NULL;
}


