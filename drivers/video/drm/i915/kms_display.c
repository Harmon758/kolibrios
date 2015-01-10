
#define iowrite32(v, addr)      writel((v), (addr))

#include <drm/drmP.h>
#include <uapi/drm/drm.h>
#include "i915_drv.h"
#include "intel_drv.h"
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/pci.h>

#include <syscall.h>

#include "bitmap.h"
#include <display.h>


display_t *os_display;
struct drm_i915_gem_object *main_fb_obj;

u32 cmd_buffer;
u32 cmd_offset;

void init_render();
int  sna_init();

static cursor_t*  __stdcall select_cursor_kms(cursor_t *cursor);
static void       __stdcall move_cursor_kms(cursor_t *cursor, int x, int y);

void __stdcall restore_cursor(int x, int y)
{};

void disable_mouse(void)
{};


static char *manufacturer_name(unsigned char *x)
{
    static char name[4];

    name[0] = ((x[0] & 0x7C) >> 2) + '@';
    name[1] = ((x[0] & 0x03) << 3) + ((x[1] & 0xE0) >> 5) + '@';
    name[2] = (x[1] & 0x1F) + '@';
    name[3] = 0;

    return name;
}

static int set_mode(struct drm_device *dev, struct drm_connector *connector,
                    struct drm_crtc *crtc, videomode_t *reqmode, bool strict)
{
    struct drm_i915_private *dev_priv   = dev->dev_private;

    struct drm_mode_config  *config     = &dev->mode_config;
    struct drm_display_mode *mode       = NULL, *tmpmode;
    struct drm_framebuffer  *fb         = NULL;
    struct drm_mode_set     set;
    const char *con_name;
    unsigned hdisplay, vdisplay;
    int stride;
    int ret;

    drm_modeset_lock_all(dev);

    list_for_each_entry(tmpmode, &connector->modes, head)
    {
        if( (tmpmode->hdisplay == reqmode->width)  &&
            (tmpmode->vdisplay == reqmode->height) &&
            (drm_mode_vrefresh(tmpmode) == reqmode->freq) )
        {
            mode = tmpmode;
            goto do_set;
        }
    };

    if( (mode == NULL) && (strict == false) )
    {
        list_for_each_entry(tmpmode, &connector->modes, head)
        {
            if( (tmpmode->hdisplay == reqmode->width)  &&
                (tmpmode->vdisplay == reqmode->height) )
            {
                mode = tmpmode;
                goto do_set;
            }
        };
    };

    DRM_ERROR("%s failed\n", __FUNCTION__);

    return -1;

do_set:

    con_name = connector->name;

    DRM_DEBUG_KMS("set mode %d %d: crtc %d connector %s\n",
              reqmode->width, reqmode->height, crtc->base.id,
              con_name);

    drm_mode_set_crtcinfo(mode, CRTC_INTERLACE_HALVE_V);

    hdisplay = mode->hdisplay;
    vdisplay = mode->vdisplay;

    if (crtc->invert_dimensions)
        swap(hdisplay, vdisplay);

    fb = crtc->primary->fb;

    fb->width  = reqmode->width;
    fb->height = reqmode->height;

        main_fb_obj->tiling_mode = I915_TILING_X;

    if( main_fb_obj->tiling_mode == I915_TILING_X)
    {
        if(IS_GEN3(dev))
            for (stride = 512; stride < reqmode->width * 4; stride <<= 1);
        else
            stride = ALIGN(reqmode->width * 4, 512);
    }
    else
    {
        stride = ALIGN(reqmode->width * 4, 64);
    }

    fb->pitches[0]  =
    fb->pitches[1]  =
    fb->pitches[2]  =
    fb->pitches[3]  = stride;

    main_fb_obj->stride  = stride;

    fb->bits_per_pixel = 32;
    fb->depth = 24;

    crtc->enabled = true;
    os_display->crtc = crtc;

//    i915_gem_object_unpin_fence(main_fb_obj);
    i915_gem_object_put_fence(main_fb_obj);

    set.crtc = crtc;
    set.x = 0;
    set.y = 0;
    set.mode = mode;
    set.connectors = &connector;
    set.num_connectors = 1;
    set.fb = fb;

    ret = drm_mode_set_config_internal(&set);

    drm_modeset_unlock_all(dev);

    if ( !ret )
    {
        os_display->width    = fb->width;
        os_display->height   = fb->height;
        os_display->vrefresh = drm_mode_vrefresh(mode);

        sysSetScreen(fb->width, fb->height, fb->pitches[0]);

        DRM_DEBUG_KMS("new mode %d x %d pitch %d\n",
                       fb->width, fb->height, fb->pitches[0]);
    }
    else
        DRM_ERROR("failed to set mode %d_%d on crtc %p\n",
                   fb->width, fb->height, crtc);

    return ret;
}

static int count_connector_modes(struct drm_connector* connector)
{
    struct drm_display_mode  *mode;
    int count = 0;

    list_for_each_entry(mode, &connector->modes, head)
    {
        count++;
    };
    return count;
};

static struct drm_crtc *get_possible_crtc(struct drm_device *dev, struct drm_encoder *encoder)
{
    struct drm_crtc *tmp_crtc;
    int crtc_mask = 1;

    list_for_each_entry(tmp_crtc, &dev->mode_config.crtc_list, head)
    {
        if (encoder->possible_crtcs & crtc_mask)
    {
            encoder->crtc = tmp_crtc;
            DRM_DEBUG_KMS("use CRTC %p ID %d\n", tmp_crtc, tmp_crtc->base.id);
            return tmp_crtc;
        };
        crtc_mask <<= 1;
    };
    return NULL;
};

static int choose_config(struct drm_device *dev, struct drm_connector **boot_connector,
                  struct drm_crtc **boot_crtc)
{
    struct drm_connector_helper_funcs *connector_funcs;
    struct drm_connector *connector;
    struct drm_encoder   *encoder;
    struct drm_crtc      *crtc;

    list_for_each_entry(connector, &dev->mode_config.connector_list, head)
    {
        if( connector->status != connector_status_connected)
            continue;

        encoder = connector->encoder;

        if(encoder == NULL)
        {
            connector_funcs = connector->helper_private;
            encoder = connector_funcs->best_encoder(connector);

        if( encoder == NULL)
        {
            DRM_DEBUG_KMS("CONNECTOR %x ID: %d no active encoders\n",
                        connector, connector->base.id);
            continue;
        };
        }

        crtc = encoder->crtc;
        if(crtc == NULL)
            crtc = get_possible_crtc(dev, encoder);

        if(crtc != NULL)
        {
            *boot_connector = connector;
            *boot_crtc = crtc;

            DRM_DEBUG_KMS("CONNECTOR %p ID:%d status:%d ENCODER %p ID: %d CRTC %p ID:%d\n",
                           connector, connector->base.id, connector->status,
                           encoder, encoder->base.id, crtc, crtc->base.id );
            return 0;
        }
        else
            DRM_DEBUG_KMS("No CRTC for encoder %d\n", encoder->base.id);

    };

    return -ENOENT;
};

static int get_boot_mode(struct drm_connector *connector, videomode_t *usermode)
{
    struct drm_display_mode *mode;

    list_for_each_entry(mode, &connector->modes, head)
    {
        DRM_DEBUG_KMS("check mode w:%d h:%d %dHz\n",
                mode->hdisplay, mode->vdisplay,
                drm_mode_vrefresh(mode));

        if( os_display->width  == mode->hdisplay &&
            os_display->height == mode->vdisplay &&
            drm_mode_vrefresh(mode) == 60)
        {
            usermode->width  = os_display->width;
            usermode->height = os_display->height;
            usermode->freq   = 60;
            return 1;
        }
    }
    return 0;
}

int init_display_kms(struct drm_device *dev, videomode_t *usermode)
{
    struct drm_connector_helper_funcs *connector_funcs;
    struct drm_connector    *connector = NULL;
    struct drm_crtc         *crtc = NULL;
    struct drm_framebuffer  *fb;

    cursor_t  *cursor;
    u32      ifl;
    int        ret;

    mutex_lock(&dev->struct_mutex);
    mutex_lock(&dev->mode_config.mutex);

    ret = choose_config(dev, &connector, &crtc);
    if(ret)
    {
        DRM_DEBUG_KMS("No active connectors!\n");
        mutex_unlock(&dev->mode_config.mutex);
        return -1;
    };

    mutex_lock(&dev->object_name_lock);
    idr_preload(GFP_KERNEL);

    if (!main_fb_obj->base.name) {
        ret = idr_alloc(&dev->object_name_idr, &main_fb_obj->base, 1, 0, GFP_NOWAIT);

        main_fb_obj->base.name = ret;

        /* Allocate a reference for the name table.  */
        drm_gem_object_reference(&main_fb_obj->base);

        DRM_DEBUG_KMS("%s allocate fb name %d\n", __FUNCTION__, main_fb_obj->base.name );
    }

    idr_preload_end();
    mutex_unlock(&dev->object_name_lock);
    drm_gem_object_unreference(&main_fb_obj->base);

    os_display = GetDisplay();
    os_display->ddev = dev;
    os_display->connector = connector;
    os_display->crtc = crtc;

    os_display->supported_modes = count_connector_modes(connector);

    ifl = safe_cli();
    {
        list_for_each_entry(cursor, &os_display->cursors, list)
        {
            init_cursor(cursor);
        };

        os_display->restore_cursor(0,0);
        os_display->init_cursor    = init_cursor;
        os_display->select_cursor  = select_cursor_kms;
        os_display->show_cursor    = NULL;
        os_display->move_cursor    = move_cursor_kms;
        os_display->restore_cursor = restore_cursor;
        os_display->disable_mouse  = disable_mouse;

        crtc->cursor_x = os_display->width/2;
        crtc->cursor_y = os_display->height/2;

        select_cursor_kms(os_display->cursor);
    };
    safe_sti(ifl);

    if( (usermode->width == 0) ||
        (usermode->height == 0))
    {
        if( !get_boot_mode(connector, usermode))
        {
            struct drm_display_mode *mode;

            mode = list_entry(connector->modes.next, typeof(*mode), head);
            usermode->width  = mode->hdisplay;
            usermode->height = mode->vdisplay;
            usermode->freq   = drm_mode_vrefresh(mode);
        };
    };

    mutex_unlock(&dev->mode_config.mutex);
    mutex_unlock(&dev->struct_mutex);

    set_mode(dev, os_display->connector, os_display->crtc, usermode, false);

#ifdef __HWA__
    err = init_bitmaps();
#endif

    LEAVE();

    return 0;
};


int get_videomodes(videomode_t *mode, int *count)
{
    int err = -1;

//    dbgprintf("mode %x count %d\n", mode, *count);

    if( *count == 0 )
    {
        *count = os_display->supported_modes;
        err = 0;
    }
    else if( mode != NULL )
    {
        struct drm_display_mode  *drmmode;
        int i = 0;

        if( *count > os_display->supported_modes)
            *count = os_display->supported_modes;

        list_for_each_entry(drmmode, &os_display->connector->modes, head)
        {
            if( i < *count)
            {
                mode->width  = drmmode->hdisplay;
                mode->height = drmmode->vdisplay;
                mode->bpp    = 32;
                mode->freq   = drm_mode_vrefresh(drmmode);
                i++;
                mode++;
            }
            else break;
        };
        *count = i;
        err = 0;
    };
    return err;
};

int set_user_mode(videomode_t *mode)
{

//    dbgprintf("width %d height %d vrefresh %d\n",
//               mode->width, mode->height, mode->freq);

    if( (mode->width  != 0)  &&
        (mode->height != 0)  &&
        (mode->freq   != 0 ) &&
        ( (mode->width   != os_display->width)  ||
          (mode->height  != os_display->height) ||
          (mode->freq    != os_display->vrefresh) ) )
    {
        return set_mode(os_display->ddev, os_display->connector, os_display->crtc, mode, true);
    };

    return -1;
};

void i915_dpms(struct drm_device *dev, int mode)
{
    const struct drm_connector_funcs *f = os_display->connector->funcs;

    f->dpms(os_display->connector, mode);
};

void __attribute__((regparm(1))) destroy_cursor(cursor_t *cursor)
{
    struct drm_i915_gem_object *obj = cursor->cobj;
    list_del(&cursor->list);

    i915_gem_object_ggtt_unpin(cursor->cobj);

    mutex_lock(&main_device->struct_mutex);
    drm_gem_object_unreference(&obj->base);
    mutex_unlock(&main_device->struct_mutex);

    __DestroyObject(cursor);
};

int init_cursor(cursor_t *cursor)
{
    struct drm_i915_private *dev_priv = os_display->ddev->dev_private;
    struct drm_i915_gem_object *obj;
    uint32_t *bits;
    uint32_t *src;
    void     *mapped;

    int       i,j;
    int       ret;

    if (dev_priv->info.cursor_needs_physical)
    {
        bits = (uint32_t*)KernelAlloc(KMS_CURSOR_WIDTH*KMS_CURSOR_HEIGHT*4);
        if (unlikely(bits == NULL))
            return ENOMEM;
        cursor->cobj = (struct drm_i915_gem_object *)GetPgAddr(bits);
    }
    else
    {
        obj = i915_gem_alloc_object(os_display->ddev, KMS_CURSOR_WIDTH*KMS_CURSOR_HEIGHT*4);
        if (unlikely(obj == NULL))
            return -ENOMEM;

        ret = i915_gem_obj_ggtt_pin(obj, 0,PIN_MAPPABLE | PIN_NONBLOCK);
        if (ret) {
            drm_gem_object_unreference(&obj->base);
            return ret;
        }

        ret = i915_gem_object_set_to_gtt_domain(obj, true);
        if (ret)
        {
            i915_gem_object_ggtt_unpin(obj);
            drm_gem_object_unreference(&obj->base);
            return ret;
        }
/* You don't need to worry about fragmentation issues.
 * GTT space is continuous. I guarantee it.                           */

        mapped = bits = (u32*)MapIoMem(dev_priv->gtt.mappable_base + i915_gem_obj_ggtt_offset(obj),
                    KMS_CURSOR_WIDTH*KMS_CURSOR_HEIGHT*4, PG_SW);

        if (unlikely(bits == NULL))
        {
            i915_gem_object_ggtt_unpin(obj);
            drm_gem_object_unreference(&obj->base);
            return -ENOMEM;
        };
        cursor->cobj = obj;
    };

    src = cursor->data;

    for(i = 0; i < 32; i++)
    {
        for(j = 0; j < 32; j++)
            *bits++ = *src++;
        for(j = 32; j < KMS_CURSOR_WIDTH; j++)
            *bits++ = 0;
    }
    for(i = 0; i < KMS_CURSOR_WIDTH*(KMS_CURSOR_HEIGHT-32); i++)
        *bits++ = 0;

    FreeKernelSpace(mapped);

// release old cursor

    KernelFree(cursor->data);

    cursor->data = bits;

    cursor->header.destroy = destroy_cursor;

    return 0;
}


void __stdcall move_cursor_kms(cursor_t *cursor, int x, int y)
{
    struct drm_crtc *crtc = os_display->crtc;
    x-= cursor->hot_x;
    y-= cursor->hot_y;

	crtc->cursor_x = x;
	crtc->cursor_y = y;

	intel_crtc_update_cursor(crtc, 1);

//    if (crtc->funcs->cursor_move)
//        crtc->funcs->cursor_move(crtc, x, y);

};


cursor_t* __stdcall select_cursor_kms(cursor_t *cursor)
{
    struct drm_i915_private *dev_priv = os_display->ddev->dev_private;
    struct drm_crtc   *crtc = os_display->crtc;
    struct intel_crtc *intel_crtc = to_intel_crtc(crtc);

    cursor_t *old;

    old = os_display->cursor;
    os_display->cursor = cursor;

    intel_crtc->cursor_bo = cursor->cobj;

    if (!dev_priv->info.cursor_needs_physical)
       intel_crtc->cursor_addr = i915_gem_obj_ggtt_offset(cursor->cobj);
    else
        intel_crtc->cursor_addr = (addr_t)cursor->cobj;

    intel_crtc->cursor_width = 64;
    intel_crtc->cursor_height = 64;

    move_cursor_kms(cursor, crtc->cursor_x, crtc->cursor_y);
    return old;
};

int i915_fbinfo(struct drm_i915_fb_info *fb)
{
    struct drm_i915_private *dev_priv = os_display->ddev->dev_private;
    struct intel_crtc *crtc = to_intel_crtc(os_display->crtc);

    struct drm_i915_gem_object *obj = get_fb_obj();

    fb->name   = obj->base.name;
    fb->width  = os_display->width;
    fb->height = os_display->height;
    fb->pitch  = obj->stride;
    fb->tiling = obj->tiling_mode;
    fb->crtc   = crtc->base.base.id;
    fb->pipe   = crtc->pipe;

    return 0;
}


typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
}rect_t;


#define CURRENT_TASK             (0x80003000)

void  FASTCALL GetWindowRect(rect_t *rc)__asm__("GetWindowRect");

int i915_mask_update(struct drm_device *dev, void *data,
            struct drm_file *file)
{
    struct drm_i915_mask *mask = data;
    struct drm_gem_object *obj;
    static unsigned int mask_seqno[256];
    rect_t winrc;
    u32    slot;
    int    ret=0;

    obj = drm_gem_object_lookup(dev, file, mask->handle);
    if (obj == NULL)
        return -ENOENT;

    if (!obj->filp) {
        drm_gem_object_unreference_unlocked(obj);
        return -EINVAL;
    }

    GetWindowRect(&winrc);
    {
//        static warn_count;

        mask->width    = winrc.right - winrc.left + 1;
        mask->height   = winrc.bottom - winrc.top + 1;
        mask->bo_pitch = (mask->width+15) & ~15;

#if 0
        if(warn_count < 1)
        {
            printf("left %d top %d right %d bottom %d\n",
                    winrc.left, winrc.top, winrc.right, winrc.bottom);
            printf("mask pitch %d data %p\n", mask->bo_pitch, mask->bo_map);
            warn_count++;
        };
#endif

     };


    slot = *((u8*)CURRENT_TASK);

    if( mask_seqno[slot] != os_display->mask_seqno)
    {
        u8* src_offset;
        u8* dst_offset;
        u32 ifl;

        ret = i915_mutex_lock_interruptible(dev);
        if (ret)
            goto err1;

        ret = i915_gem_object_set_to_cpu_domain(to_intel_bo(obj), true);
        if(ret != 0 )
        {
            dbgprintf("%s: i915_gem_object_set_to_cpu_domain failed\n", __FUNCTION__);
            goto err2;
        };

//        printf("width %d height %d\n", winrc.right, winrc.bottom);

//        slot = 0x01;

        src_offset = os_display->win_map;
        src_offset+= winrc.top*os_display->width + winrc.left;

        dst_offset = (u8*)mask->bo_map;

        u32 tmp_h = mask->height;

        ifl = safe_cli();
        {
            mask_seqno[slot] = os_display->mask_seqno;

            slot|= (slot<<8)|(slot<<16)|(slot<<24);

            __asm__ __volatile__ (
            "movd       %[slot],   %%xmm6         \n"
            "punpckldq  %%xmm6, %%xmm6            \n"
            "punpcklqdq %%xmm6, %%xmm6            \n"
            :: [slot]  "m" (slot)
            :"xmm6");

            while( tmp_h--)
            {
                int tmp_w = mask->width;

                u8* tmp_src = src_offset;
                u8* tmp_dst = dst_offset;

                src_offset+= os_display->width;
                dst_offset+= mask->bo_pitch;

                while(tmp_w >= 64)
                {
                    __asm__ __volatile__ (
                    "movdqu     (%0),   %%xmm0            \n"
                    "movdqu   16(%0),   %%xmm1            \n"
                    "movdqu   32(%0),   %%xmm2            \n"
                    "movdqu   48(%0),   %%xmm3            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm1            \n"
                    "pcmpeqb    %%xmm6, %%xmm2            \n"
                    "pcmpeqb    %%xmm6, %%xmm3            \n"
                    "movdqa     %%xmm0,   (%%edi)         \n"
                    "movdqa     %%xmm1, 16(%%edi)         \n"
                    "movdqa     %%xmm2, 32(%%edi)         \n"
                    "movdqa     %%xmm3, 48(%%edi)         \n"

                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0","xmm1","xmm2","xmm3");
                    tmp_w -= 64;
                    tmp_src += 64;
                    tmp_dst += 64;
                }

                if( tmp_w >= 32 )
                {
                    __asm__ __volatile__ (
                    "movdqu     (%0),   %%xmm0            \n"
                    "movdqu   16(%0),   %%xmm1            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm1            \n"
                    "movdqa     %%xmm0,   (%%edi)         \n"
                    "movdqa     %%xmm1, 16(%%edi)         \n"

                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0","xmm1");
                    tmp_w -= 32;
                    tmp_src += 32;
                    tmp_dst += 32;
                }

                if( tmp_w >= 16 )
                {
                    __asm__ __volatile__ (
                    "movdqu     (%0),   %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "movdqa     %%xmm0,   (%%edi)         \n"
                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0");
                    tmp_w -= 16;
                    tmp_src += 16;
                    tmp_dst += 16;
                }

                if( tmp_w >= 8 )
                {
                    __asm__ __volatile__ (
                    "movq       (%0),   %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "movq       %%xmm0,   (%%edi)         \n"
                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0");
                    tmp_w -= 8;
                    tmp_src += 8;
                    tmp_dst += 8;
                }
                if( tmp_w >= 4 )
                {
                    __asm__ __volatile__ (
                    "movd       (%0),   %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "movd       %%xmm0,   (%%edi)         \n"
                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0");
                    tmp_w -= 4;
                    tmp_src += 4;
                    tmp_dst += 4;
                }
                while(tmp_w--)
                    *tmp_dst++ = (*tmp_src++ == (u8)slot) ? 0xFF:0x00;
            };
        };
        safe_sti(ifl);

        ret = i915_gem_object_set_to_gtt_domain(to_intel_bo(obj), false);
    }

err2:
    mutex_unlock(&dev->struct_mutex);
err1:
    drm_gem_object_unreference(obj);

    return ret;
}

int i915_mask_update_ex(struct drm_device *dev, void *data,
            struct drm_file *file)
{
    struct drm_i915_mask_update *mask = data;
    struct drm_gem_object *obj;
    static unsigned int mask_seqno[256];
    static warn_count;

    rect_t win;
    u32    winw,winh;
    u32    ml,mt,mr,mb;
    u32    slot;
    int    ret = 0;
    slot = *((u8*)CURRENT_TASK);

    if( mask_seqno[slot] == os_display->mask_seqno)
        return 0;

    memset(mask->bo_map,0,mask->width * mask->height);

    GetWindowRect(&win);
    win.right+= 1;
    win.bottom+=  1;

    winw = win.right - win.left;
    winh = win.bottom - win.top;

    if(mask->dx >= winw ||
       mask->dy >= winh)
       return 1;

    ml = win.left + mask->dx;
    mt = win.top  + mask->dy;
    mr = ml + mask->width;
    mb = mt + mask->height;

    if( ml >= win.right || mt >= win.bottom ||
        mr < win.left   || mb < win.top )
        return 1;

    if( mr > win.right )
        mr = win.right;

    if( mb > win.bottom )
        mb = win.bottom;

    mask->width  = mr - ml;
    mask->height = mb - mt;

    if( mask->width == 0 ||
        mask->height== 0 )
        return 1;

    obj = drm_gem_object_lookup(dev, file, mask->handle);
    if (obj == NULL)
        return -ENOENT;

    if (!obj->filp) {
        drm_gem_object_unreference_unlocked(obj);
        return -EINVAL;
    }

#if 1
    if(warn_count < 1000)
    {
        printf("left %d top %d right %d bottom %d\n",
                ml, mt, mr, mb);
        warn_count++;
    };
#endif


#if 1

    {
        u8* src_offset;
        u8* dst_offset;
        u32 ifl;

        ret = i915_mutex_lock_interruptible(dev);
        if (ret)
            goto err1;

        i915_gem_object_set_to_cpu_domain(to_intel_bo(obj), true);

        src_offset = os_display->win_map;
        src_offset+= mt*os_display->width + ml;
        dst_offset = (u8*)mask->bo_map;

        u32 tmp_h = mask->height;

        ifl = safe_cli();
        {
            mask_seqno[slot] = os_display->mask_seqno;

            slot|= (slot<<8)|(slot<<16)|(slot<<24);

            __asm__ __volatile__ (
            "movd       %[slot],   %%xmm6         \n"
            "punpckldq  %%xmm6, %%xmm6            \n"
            "punpcklqdq %%xmm6, %%xmm6            \n"
            :: [slot]  "m" (slot)
            :"xmm6");

            while( tmp_h--)
            {
                int tmp_w = mask->width;

                u8* tmp_src = src_offset;
                u8* tmp_dst = dst_offset;

                src_offset+= os_display->width;
                dst_offset+= mask->bo_pitch;

                while(tmp_w >= 64)
                {
                    __asm__ __volatile__ (
                    "movdqu     (%0),   %%xmm0            \n"
                    "movdqu   16(%0),   %%xmm1            \n"
                    "movdqu   32(%0),   %%xmm2            \n"
                    "movdqu   48(%0),   %%xmm3            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm1            \n"
                    "pcmpeqb    %%xmm6, %%xmm2            \n"
                    "pcmpeqb    %%xmm6, %%xmm3            \n"
                    "movdqa     %%xmm0,   (%%edi)         \n"
                    "movdqa     %%xmm1, 16(%%edi)         \n"
                    "movdqa     %%xmm2, 32(%%edi)         \n"
                    "movdqa     %%xmm3, 48(%%edi)         \n"

                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0","xmm1","xmm2","xmm3");
                    tmp_w -= 64;
                    tmp_src += 64;
                    tmp_dst += 64;
                }

                if( tmp_w >= 32 )
                {
                    __asm__ __volatile__ (
                    "movdqu     (%0),   %%xmm0            \n"
                    "movdqu   16(%0),   %%xmm1            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm1            \n"
                    "movdqa     %%xmm0,   (%%edi)         \n"
                    "movdqa     %%xmm1, 16(%%edi)         \n"

                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0","xmm1");
                    tmp_w -= 32;
                    tmp_src += 32;
                    tmp_dst += 32;
                }

                if( tmp_w >= 16 )
                {
                    __asm__ __volatile__ (
                    "movdqu     (%0),   %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "movdqa     %%xmm0,   (%%edi)         \n"
                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0");
                    tmp_w -= 16;
                    tmp_src += 16;
                    tmp_dst += 16;
                }

                if( tmp_w >= 8 )
                {
                    __asm__ __volatile__ (
                    "movq       (%0),   %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "movq       %%xmm0,   (%%edi)         \n"
                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0");
                    tmp_w -= 8;
                    tmp_src += 8;
                    tmp_dst += 8;
                }
                if( tmp_w >= 4 )
                {
                    __asm__ __volatile__ (
                    "movd       (%0),   %%xmm0            \n"
                    "pcmpeqb    %%xmm6, %%xmm0            \n"
                    "movd       %%xmm0,   (%%edi)         \n"
                    :: "r" (tmp_src), "D" (tmp_dst)
                    :"xmm0");
                    tmp_w -= 4;
                    tmp_src += 4;
                    tmp_dst += 4;
                }
                while(tmp_w--)
                    *tmp_dst++ = (*tmp_src++ == (u8)slot) ? 0xFF:0x00;
            };
        };
        safe_sti(ifl);

        i915_gem_object_set_to_gtt_domain(to_intel_bo(obj), false);
    }
#endif

err2:
    mutex_unlock(&dev->struct_mutex);
err1:
    drm_gem_object_unreference(obj);

    return ret;
}











#define NSEC_PER_SEC    1000000000L

void getrawmonotonic(struct timespec *ts)
{
    u32 tmp = GetTimerTicks();

    ts->tv_sec  = tmp/100;
    ts->tv_nsec = (tmp - ts->tv_sec*100)*10000000;
}

void set_normalized_timespec(struct timespec *ts, time_t sec, s64 nsec)
{
    while (nsec >= NSEC_PER_SEC) {
        /*
         * The following asm() prevents the compiler from
         * optimising this loop into a modulo operation. See
         * also __iter_div_u64_rem() in include/linux/time.h
         */
        asm("" : "+rm"(nsec));
        nsec -= NSEC_PER_SEC;
        ++sec;
    }
    while (nsec < 0) {
        asm("" : "+rm"(nsec));
        nsec += NSEC_PER_SEC;
        --sec;
    }
    ts->tv_sec = sec;
    ts->tv_nsec = nsec;
}

void
prepare_to_wait(wait_queue_head_t *q, wait_queue_t *wait, int state)
{
    unsigned long flags;

//    wait->flags &= ~WQ_FLAG_EXCLUSIVE;
    spin_lock_irqsave(&q->lock, flags);
    if (list_empty(&wait->task_list))
            __add_wait_queue(q, wait);
    spin_unlock_irqrestore(&q->lock, flags);
}

/**
 * finish_wait - clean up after waiting in a queue
 * @q: waitqueue waited on
 * @wait: wait descriptor
 *
 * Sets current thread back to running state and removes
 * the wait descriptor from the given waitqueue if still
 * queued.
 */
void finish_wait(wait_queue_head_t *q, wait_queue_t *wait)
{
    unsigned long flags;

//    __set_current_state(TASK_RUNNING);
    /*
     * We can check for list emptiness outside the lock
     * IFF:
     *  - we use the "careful" check that verifies both
     *    the next and prev pointers, so that there cannot
     *    be any half-pending updates in progress on other
     *    CPU's that we haven't seen yet (and that might
     *    still change the stack area.
     * and
     *  - all other users take the lock (ie we can only
     *    have _one_ other CPU that looks at or modifies
     *    the list).
     */
    if (!list_empty_careful(&wait->task_list)) {
            spin_lock_irqsave(&q->lock, flags);
            list_del_init(&wait->task_list);
            spin_unlock_irqrestore(&q->lock, flags);
    }

    DestroyEvent(wait->evnt);
}

int autoremove_wake_function(wait_queue_t *wait, unsigned mode, int sync, void *key)
{
    list_del_init(&wait->task_list);
    return 1;
}



