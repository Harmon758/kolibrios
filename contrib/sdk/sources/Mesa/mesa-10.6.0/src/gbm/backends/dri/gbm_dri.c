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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Benjamin Franzke <benjaminfranzke@googlemail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <xf86drm.h>

#include <GL/gl.h> /* dri_interface needs GL types */
#include <GL/internal/dri_interface.h>

#include "gbm_driint.h"

#include "gbmint.h"
#include "loader.h"

/* For importing wl_buffer */
#if HAVE_WAYLAND_PLATFORM
#include "../../../egl/wayland/wayland-drm/wayland-drm.h"
#endif

static __DRIimage *
dri_lookup_egl_image(__DRIscreen *screen, void *image, void *data)
{
   struct gbm_dri_device *dri = data;

   if (dri->lookup_image == NULL)
      return NULL;

   return dri->lookup_image(screen, image, dri->lookup_user_data);
}

static __DRIbuffer *
dri_get_buffers(__DRIdrawable * driDrawable,
		 int *width, int *height,
		 unsigned int *attachments, int count,
		 int *out_count, void *data)
{
   struct gbm_dri_surface *surf = data;
   struct gbm_dri_device *dri = gbm_dri_device(surf->base.gbm);

   if (dri->get_buffers == NULL)
      return NULL;

   return dri->get_buffers(driDrawable, width, height, attachments,
                           count, out_count, surf->dri_private);
}

static void
dri_flush_front_buffer(__DRIdrawable * driDrawable, void *data)
{
   struct gbm_dri_surface *surf = data;
   struct gbm_dri_device *dri = gbm_dri_device(surf->base.gbm);

   if (dri->flush_front_buffer != NULL)
      dri->flush_front_buffer(driDrawable, surf->dri_private);
}

static __DRIbuffer *
dri_get_buffers_with_format(__DRIdrawable * driDrawable,
                            int *width, int *height,
                            unsigned int *attachments, int count,
                            int *out_count, void *data)
{
   struct gbm_dri_surface *surf = data;
   struct gbm_dri_device *dri = gbm_dri_device(surf->base.gbm);

   if (dri->get_buffers_with_format == NULL)
      return NULL;

   return
      dri->get_buffers_with_format(driDrawable, width, height, attachments,
                                   count, out_count, surf->dri_private);
}

static int
image_get_buffers(__DRIdrawable *driDrawable,
                  unsigned int format,
                  uint32_t *stamp,
                  void *loaderPrivate,
                  uint32_t buffer_mask,
                  struct __DRIimageList *buffers)
{
   struct gbm_dri_surface *surf = loaderPrivate;
   struct gbm_dri_device *dri = gbm_dri_device(surf->base.gbm);

   if (dri->image_get_buffers == NULL)
      return 0;

   return dri->image_get_buffers(driDrawable, format, stamp,
                                 surf->dri_private, buffer_mask, buffers);
}

static void
swrast_get_drawable_info(__DRIdrawable *driDrawable,
                         int           *x,
                         int           *y,
                         int           *width,
                         int           *height,
                         void          *loaderPrivate)
{
   struct gbm_dri_surface *surf = loaderPrivate;

   *x = 0;
   *y = 0;
   *width = surf->base.width;
   *height = surf->base.height;
}

static void
swrast_put_image2(__DRIdrawable *driDrawable,
                  int            op,
                  int            x,
                  int            y,
                  int            width,
                  int            height,
                  int            stride,
                  char          *data,
                  void          *loaderPrivate)
{
   struct gbm_dri_surface *surf = loaderPrivate;
   struct gbm_dri_device *dri = gbm_dri_device(surf->base.gbm);

   dri->swrast_put_image2(driDrawable,
                          op, x, y,
                          width, height, stride,
                          data, surf->dri_private);
}

static void
swrast_put_image(__DRIdrawable *driDrawable,
                 int            op,
                 int            x,
                 int            y,
                 int            width,
                 int            height,
                 char          *data,
                 void          *loaderPrivate)
{
   return swrast_put_image2(driDrawable, op, x, y, width, height,
                            width * 4, data, loaderPrivate);
}

static void
swrast_get_image(__DRIdrawable *driDrawable,
                 int            x,
                 int            y,
                 int            width,
                 int            height,
                 char          *data,
                 void          *loaderPrivate)
{
   struct gbm_dri_surface *surf = loaderPrivate;
   struct gbm_dri_device *dri = gbm_dri_device(surf->base.gbm);

   dri->swrast_get_image(driDrawable,
                         x, y,
                         width, height,
                         data, surf->dri_private);
}

static const __DRIuseInvalidateExtension use_invalidate = {
   .base = { __DRI_USE_INVALIDATE, 1 }
};

static const __DRIimageLookupExtension image_lookup_extension = {
   .base = { __DRI_IMAGE_LOOKUP, 1 },

   .lookupEGLImage          = dri_lookup_egl_image
};

static const __DRIdri2LoaderExtension dri2_loader_extension = {
   .base = { __DRI_DRI2_LOADER, 3 },

   .getBuffers              = dri_get_buffers,
   .flushFrontBuffer        = dri_flush_front_buffer,
   .getBuffersWithFormat    = dri_get_buffers_with_format,
};

static const __DRIimageLoaderExtension image_loader_extension = {
   .base = { __DRI_IMAGE_LOADER, 1 },

   .getBuffers          = image_get_buffers,
   .flushFrontBuffer    = dri_flush_front_buffer,
};

static const __DRIswrastLoaderExtension swrast_loader_extension = {
   .base = { __DRI_SWRAST_LOADER, 2 },

   .getDrawableInfo = swrast_get_drawable_info,
   .putImage        = swrast_put_image,
   .getImage        = swrast_get_image,
   .putImage2       = swrast_put_image2
};

static const __DRIextension *gbm_dri_screen_extensions[] = {
   &image_lookup_extension.base,
   &use_invalidate.base,
   &dri2_loader_extension.base,
   &image_loader_extension.base,
   &swrast_loader_extension.base,
   NULL,
};

struct dri_extension_match {
   const char *name;
   int version;
   int offset;
};

static struct dri_extension_match dri_core_extensions[] = {
   { __DRI2_FLUSH, 1, offsetof(struct gbm_dri_device, flush) },
   { __DRI_IMAGE, 1, offsetof(struct gbm_dri_device, image) },
   { NULL, 0, 0 }
};

static struct dri_extension_match gbm_dri_device_extensions[] = {
   { __DRI_CORE, 1, offsetof(struct gbm_dri_device, core) },
   { __DRI_DRI2, 1, offsetof(struct gbm_dri_device, dri2) },
   { NULL, 0, 0 }
};

static struct dri_extension_match gbm_swrast_device_extensions[] = {
   { __DRI_CORE, 1, offsetof(struct gbm_dri_device, core), },
   { __DRI_SWRAST, 1, offsetof(struct gbm_dri_device, swrast) },
   { NULL, 0, 0 }
};

static int
dri_bind_extensions(struct gbm_dri_device *dri,
                    struct dri_extension_match *matches,
                    const __DRIextension **extensions)
{
   int i, j, ret = 0;
   void *field;

   for (i = 0; extensions[i]; i++) {
      for (j = 0; matches[j].name; j++) {
         if (strcmp(extensions[i]->name, matches[j].name) == 0 &&
             extensions[i]->version >= matches[j].version) {
            field = ((char *) dri + matches[j].offset);
            *(const __DRIextension **) field = extensions[i];
         }
      }
   }

   for (j = 0; matches[j].name; j++) {
      field = ((char *) dri + matches[j].offset);
      if (*(const __DRIextension **) field == NULL) {
         ret = -1;
      }
   }

   return ret;
}

static const __DRIextension **
dri_open_driver(struct gbm_dri_device *dri)
{
   const __DRIextension **extensions = NULL;
   char path[PATH_MAX], *search_paths, *p, *next, *end;
   char *get_extensions_name;

   search_paths = NULL;
   /* don't allow setuid apps to use LIBGL_DRIVERS_PATH or GBM_DRIVERS_PATH */
   if (geteuid() == getuid()) {
      /* Read GBM_DRIVERS_PATH first for compatibility, but LIBGL_DRIVERS_PATH
       * is recommended over GBM_DRIVERS_PATH.
       */
      search_paths = getenv("GBM_DRIVERS_PATH");

      /* Read LIBGL_DRIVERS_PATH if GBM_DRIVERS_PATH was not set.
       * LIBGL_DRIVERS_PATH is recommended over GBM_DRIVERS_PATH.
       */
      if (search_paths == NULL) {
         search_paths = getenv("LIBGL_DRIVERS_PATH");
      }
   }
   if (search_paths == NULL)
      search_paths = DEFAULT_DRIVER_DIR;

   dri->driver = NULL;
   end = search_paths + strlen(search_paths);
   for (p = search_paths; p < end && dri->driver == NULL; p = next + 1) {
      int len;
      next = strchr(p, ':');
      if (next == NULL)
         next = end;

      len = next - p;
#if GLX_USE_TLS
      snprintf(path, sizeof path,
               "%.*s/tls/%s_dri.so", len, p, dri->base.driver_name);
      dri->driver = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
#endif
      if (dri->driver == NULL) {
         snprintf(path, sizeof path,
                  "%.*s/%s_dri.so", len, p, dri->base.driver_name);
         dri->driver = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
      }
      /* not need continue to loop all paths once the driver is found */
      if (dri->driver != NULL)
         break;
   }

   if (dri->driver == NULL) {
      fprintf(stderr, "gbm: failed to open any driver (search paths %s)\n",
              search_paths);
      fprintf(stderr, "gbm: Last dlopen error: %s\n", dlerror());
      return NULL;
   }

   if (asprintf(&get_extensions_name, "%s_%s",
                __DRI_DRIVER_GET_EXTENSIONS, dri->base.driver_name) != -1) {
      const __DRIextension **(*get_extensions)(void);

      get_extensions = dlsym(dri->driver, get_extensions_name);
      free(get_extensions_name);

      if (get_extensions)
         extensions = get_extensions();
   }

   if (!extensions)
      extensions = dlsym(dri->driver, __DRI_DRIVER_EXTENSIONS);
   if (extensions == NULL) {
      fprintf(stderr, "gbm: driver exports no extensions (%s)", dlerror());
      dlclose(dri->driver);
   }

   return extensions;
}

static int
dri_load_driver(struct gbm_dri_device *dri)
{
   const __DRIextension **extensions;

   extensions = dri_open_driver(dri);
   if (!extensions)
      return -1;

   if (dri_bind_extensions(dri, gbm_dri_device_extensions, extensions) < 0) {
      dlclose(dri->driver);
      fprintf(stderr, "failed to bind extensions\n");
      return -1;
   }

   dri->driver_extensions = extensions;

   return 0;
}

static int
dri_load_driver_swrast(struct gbm_dri_device *dri)
{
   const __DRIextension **extensions;

   extensions = dri_open_driver(dri);
   if (!extensions)
      return -1;

   if (dri_bind_extensions(dri, gbm_swrast_device_extensions, extensions) < 0) {
      dlclose(dri->driver);
      fprintf(stderr, "failed to bind extensions\n");
      return -1;
   }

   dri->driver_extensions = extensions;

   return 0;
}

static int
dri_screen_create_dri2(struct gbm_dri_device *dri, char *driver_name)
{
   const __DRIextension **extensions;
   int ret = 0;

   dri->base.driver_name = driver_name;
   if (dri->base.driver_name == NULL)
      return -1;

   ret = dri_load_driver(dri);
   if (ret) {
      fprintf(stderr, "failed to load driver: %s\n", dri->base.driver_name);
      return ret;
   };

   dri->extensions = gbm_dri_screen_extensions;

   if (dri->dri2 == NULL)
      return -1;

   if (dri->dri2->base.version >= 4) {
      dri->screen = dri->dri2->createNewScreen2(0, dri->base.base.fd,
                                                dri->extensions,
                                                dri->driver_extensions,
                                                &dri->driver_configs, dri);
   } else {
      dri->screen = dri->dri2->createNewScreen(0, dri->base.base.fd,
                                               dri->extensions,
                                               &dri->driver_configs, dri);
   }
   if (dri->screen == NULL)
      return -1;

   extensions = dri->core->getExtensions(dri->screen);
   if (dri_bind_extensions(dri, dri_core_extensions, extensions) < 0) {
      ret = -1;
      goto free_screen;
   }

   dri->lookup_image = NULL;
   dri->lookup_user_data = NULL;

   return 0;

free_screen:
   dri->core->destroyScreen(dri->screen);

   return ret;
}

static int
dri_screen_create_swrast(struct gbm_dri_device *dri)
{
   int ret;

   dri->base.driver_name = strdup("swrast");
   if (dri->base.driver_name == NULL)
      return -1;

   ret = dri_load_driver_swrast(dri);
   if (ret) {
      fprintf(stderr, "failed to load swrast driver\n");
      return ret;
   }

   dri->extensions = gbm_dri_screen_extensions;

   if (dri->swrast == NULL)
      return -1;

   if (dri->swrast->base.version >= 4) {
      dri->screen = dri->swrast->createNewScreen2(0, dri->extensions,
                                                  dri->driver_extensions,
                                                  &dri->driver_configs, dri);
   } else {
      dri->screen = dri->swrast->createNewScreen(0, dri->extensions,
                                                 &dri->driver_configs, dri);
   }
   if (dri->screen == NULL)
      return -1;

   dri->lookup_image = NULL;
   dri->lookup_user_data = NULL;

   return 0;
}

static int
dri_screen_create(struct gbm_dri_device *dri)
{
   char *driver_name;

   driver_name = loader_get_driver_for_fd(dri->base.base.fd, 0);
   if (!driver_name)
      return -1;

   return dri_screen_create_dri2(dri, driver_name);
}

static int
dri_screen_create_sw(struct gbm_dri_device *dri)
{
   char *driver_name;
   int ret;

   driver_name = strdup("kms_swrast");
   if (!driver_name)
      return -errno;

   ret = dri_screen_create_dri2(dri, driver_name);
   if (ret == 0)
      return ret;

   return dri_screen_create_swrast(dri);
}

static int
gbm_dri_is_format_supported(struct gbm_device *gbm,
                            uint32_t format,
                            uint32_t usage)
{
   switch (format) {
   case GBM_BO_FORMAT_XRGB8888:
   case GBM_FORMAT_XRGB8888:
      break;
   case GBM_BO_FORMAT_ARGB8888:
   case GBM_FORMAT_ARGB8888:
      if (usage & GBM_BO_USE_SCANOUT)
         return 0;
      break;
   default:
      return 0;
   }

   if (usage & GBM_BO_USE_CURSOR &&
       usage & GBM_BO_USE_RENDERING)
      return 0;

   return 1;
}

static int
gbm_dri_bo_write(struct gbm_bo *_bo, const void *buf, size_t count)
{
   struct gbm_dri_bo *bo = gbm_dri_bo(_bo);

   if (bo->image != NULL) {
      errno = EINVAL;
      return -1;
   }

   memcpy(bo->map, buf, count);

   return 0;
}

static int
gbm_dri_bo_get_fd(struct gbm_bo *_bo)
{
   struct gbm_dri_device *dri = gbm_dri_device(_bo->gbm);
   struct gbm_dri_bo *bo = gbm_dri_bo(_bo);
   int fd;

   if (bo->image == NULL)
      return -1;

   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_FD, &fd);

   return fd;
}

static void
gbm_dri_bo_destroy(struct gbm_bo *_bo)
{
   struct gbm_dri_device *dri = gbm_dri_device(_bo->gbm);
   struct gbm_dri_bo *bo = gbm_dri_bo(_bo);
   struct drm_mode_destroy_dumb arg;

   if (bo->image != NULL) {
      dri->image->destroyImage(bo->image);
   } else {
      gbm_dri_bo_unmap(bo);
      memset(&arg, 0, sizeof(arg));
      arg.handle = bo->handle;
      drmIoctl(dri->base.base.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &arg);
   }

   free(bo);
}

static uint32_t
gbm_dri_to_gbm_format(uint32_t dri_format)
{
   uint32_t ret = 0;

   switch (dri_format) {
   case __DRI_IMAGE_FORMAT_RGB565:
      ret = GBM_FORMAT_RGB565;
      break;
   case __DRI_IMAGE_FORMAT_XRGB8888:
      ret = GBM_FORMAT_XRGB8888;
      break;
   case __DRI_IMAGE_FORMAT_ARGB8888:
      ret = GBM_FORMAT_ARGB8888;
      break;
   case __DRI_IMAGE_FORMAT_ABGR8888:
      ret = GBM_FORMAT_ABGR8888;
      break;
   default:
      ret = 0;
      break;
   }

   return ret;
}

static struct gbm_bo *
gbm_dri_bo_import(struct gbm_device *gbm,
                  uint32_t type, void *buffer, uint32_t usage)
{
   struct gbm_dri_device *dri = gbm_dri_device(gbm);
   struct gbm_dri_bo *bo;
   __DRIimage *image;
   unsigned dri_use = 0;
   int gbm_format;

   /* Required for query image WIDTH & HEIGHT */
   if (dri->image == NULL || dri->image->base.version < 4) {
      errno = ENOSYS;
      return NULL;
   }

   switch (type) {
#if HAVE_WAYLAND_PLATFORM
   case GBM_BO_IMPORT_WL_BUFFER:
   {
      struct wl_drm_buffer *wb;

      if (!dri->wl_drm) {
         errno = EINVAL;
         return NULL;
      }

      wb = wayland_drm_buffer_get(dri->wl_drm, (struct wl_resource *) buffer);
      if (!wb) {
         errno = EINVAL;
         return NULL;
      }

      image = dri->image->dupImage(wb->driver_buffer, NULL);

      switch (wb->format) {
      case WL_DRM_FORMAT_XRGB8888:
         gbm_format = GBM_FORMAT_XRGB8888;
         break;
      case WL_DRM_FORMAT_ARGB8888:
         gbm_format = GBM_FORMAT_ARGB8888;
         break;
      case WL_DRM_FORMAT_RGB565:
         gbm_format = GBM_FORMAT_RGB565;
         break;
      case WL_DRM_FORMAT_YUYV:
         gbm_format = GBM_FORMAT_YUYV;
         break;
      default:
         return NULL;
      }
      break;
   }
#endif

   case GBM_BO_IMPORT_EGL_IMAGE:
   {
      int dri_format;
      if (dri->lookup_image == NULL) {
         errno = EINVAL;
         return NULL;
      }

      image = dri->lookup_image(dri->screen, buffer, dri->lookup_user_data);
      image = dri->image->dupImage(image, NULL);
      dri->image->queryImage(image, __DRI_IMAGE_ATTRIB_FORMAT, &dri_format);
      gbm_format = gbm_dri_to_gbm_format(dri_format);
      if (gbm_format == 0) {
         errno = EINVAL;
         return NULL;
      }
      break;
   }

   case GBM_BO_IMPORT_FD:
   {
      struct gbm_import_fd_data *fd_data = buffer;
      int stride = fd_data->stride, offset = 0;

      image = dri->image->createImageFromFds(dri->screen,
                                             fd_data->width,
                                             fd_data->height,
                                             fd_data->format,
                                             &fd_data->fd, 1,
                                             &stride, &offset,
                                             NULL);
      gbm_format = fd_data->format;
      break;
   }

   default:
      errno = ENOSYS;
      return NULL;
   }


   bo = calloc(1, sizeof *bo);
   if (bo == NULL)
      return NULL;

   bo->image = image;

   if (usage & GBM_BO_USE_SCANOUT)
      dri_use |= __DRI_IMAGE_USE_SCANOUT;
   if (usage & GBM_BO_USE_CURSOR)
      dri_use |= __DRI_IMAGE_USE_CURSOR;
   if (dri->image->base.version >= 2 &&
       !dri->image->validateUsage(bo->image, dri_use)) {
      errno = EINVAL;
      free(bo);
      return NULL;
   }

   bo->base.base.gbm = gbm;
   bo->base.base.format = gbm_format;

   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_WIDTH,
                          (int*)&bo->base.base.width);
   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_HEIGHT,
                          (int*)&bo->base.base.height);
   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_STRIDE,
                          (int*)&bo->base.base.stride);
   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_HANDLE,
                          &bo->base.base.handle.s32);

   return &bo->base.base;
}

static struct gbm_bo *
create_dumb(struct gbm_device *gbm,
                  uint32_t width, uint32_t height,
                  uint32_t format, uint32_t usage)
{
   struct gbm_dri_device *dri = gbm_dri_device(gbm);
   struct drm_mode_create_dumb create_arg;
   struct gbm_dri_bo *bo;
   struct drm_mode_destroy_dumb destroy_arg;
   int ret;
   int is_cursor, is_scanout;

   is_cursor = (usage & GBM_BO_USE_CURSOR) != 0 &&
      format == GBM_FORMAT_ARGB8888;
   is_scanout = (usage & GBM_BO_USE_SCANOUT) != 0 &&
      format == GBM_FORMAT_XRGB8888;
   if (!is_cursor && !is_scanout) {
      errno = EINVAL;
      return NULL;
   }

   bo = calloc(1, sizeof *bo);
   if (bo == NULL)
      return NULL;

   memset(&create_arg, 0, sizeof(create_arg));
   create_arg.bpp = 32;
   create_arg.width = width;
   create_arg.height = height;

   ret = drmIoctl(dri->base.base.fd, DRM_IOCTL_MODE_CREATE_DUMB, &create_arg);
   if (ret)
      goto free_bo;

   bo->base.base.gbm = gbm;
   bo->base.base.width = width;
   bo->base.base.height = height;
   bo->base.base.stride = create_arg.pitch;
   bo->base.base.format = format;
   bo->base.base.handle.u32 = create_arg.handle;
   bo->handle = create_arg.handle;
   bo->size = create_arg.size;

   if (gbm_dri_bo_map(bo) == NULL)
      goto destroy_dumb;

   return &bo->base.base;

destroy_dumb:
   memset(&destroy_arg, 0, sizeof destroy_arg);
   destroy_arg.handle = create_arg.handle;
   drmIoctl(dri->base.base.fd, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy_arg);
free_bo:
   free(bo);

   return NULL;
}

static struct gbm_bo *
gbm_dri_bo_create(struct gbm_device *gbm,
                  uint32_t width, uint32_t height,
                  uint32_t format, uint32_t usage)
{
   struct gbm_dri_device *dri = gbm_dri_device(gbm);
   struct gbm_dri_bo *bo;
   int dri_format;
   unsigned dri_use = 0;

   if (usage & GBM_BO_USE_WRITE || dri->image == NULL)
      return create_dumb(gbm, width, height, format, usage);

   bo = calloc(1, sizeof *bo);
   if (bo == NULL)
      return NULL;

   bo->base.base.gbm = gbm;
   bo->base.base.width = width;
   bo->base.base.height = height;
   bo->base.base.format = format;

   switch (format) {
   case GBM_FORMAT_RGB565:
      dri_format =__DRI_IMAGE_FORMAT_RGB565;
      break;
   case GBM_FORMAT_XRGB8888:
   case GBM_BO_FORMAT_XRGB8888:
      dri_format = __DRI_IMAGE_FORMAT_XRGB8888;
      break;
   case GBM_FORMAT_ARGB8888:
   case GBM_BO_FORMAT_ARGB8888:
      dri_format = __DRI_IMAGE_FORMAT_ARGB8888;
      break;
   case GBM_FORMAT_ABGR8888:
      dri_format = __DRI_IMAGE_FORMAT_ABGR8888;
      break;
   case GBM_FORMAT_ARGB2101010:
      dri_format = __DRI_IMAGE_FORMAT_ARGB2101010;
      break;
   case GBM_FORMAT_XRGB2101010:
      dri_format = __DRI_IMAGE_FORMAT_XRGB2101010;
      break;
   default:
      errno = EINVAL;
      goto failed;
   }

   if (usage & GBM_BO_USE_SCANOUT)
      dri_use |= __DRI_IMAGE_USE_SCANOUT;
   if (usage & GBM_BO_USE_CURSOR)
      dri_use |= __DRI_IMAGE_USE_CURSOR;
   if (usage & GBM_BO_USE_LINEAR)
      dri_use |= __DRI_IMAGE_USE_LINEAR;

   /* Gallium drivers requires shared in order to get the handle/stride */
   dri_use |= __DRI_IMAGE_USE_SHARE;

   bo->image =
      dri->image->createImage(dri->screen,
                              width, height,
                              dri_format, dri_use,
                              bo);
   if (bo->image == NULL)
      goto failed;

   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_HANDLE,
                          &bo->base.base.handle.s32);
   dri->image->queryImage(bo->image, __DRI_IMAGE_ATTRIB_STRIDE,
                          (int *) &bo->base.base.stride);

   return &bo->base.base;

failed:
   free(bo);
   return NULL;
}

static struct gbm_surface *
gbm_dri_surface_create(struct gbm_device *gbm,
                       uint32_t width, uint32_t height,
		       uint32_t format, uint32_t flags)
{
   struct gbm_dri_surface *surf;

   surf = calloc(1, sizeof *surf);
   if (surf == NULL)
      return NULL;

   surf->base.gbm = gbm;
   surf->base.width = width;
   surf->base.height = height;
   surf->base.format = format;
   surf->base.flags = flags;

   return &surf->base;
}

static void
gbm_dri_surface_destroy(struct gbm_surface *_surf)
{
   struct gbm_dri_surface *surf = gbm_dri_surface(_surf);

   free(surf);
}

static void
dri_destroy(struct gbm_device *gbm)
{
   struct gbm_dri_device *dri = gbm_dri_device(gbm);
   unsigned i;

   dri->core->destroyScreen(dri->screen);
   for (i = 0; dri->driver_configs[i]; i++)
      free((__DRIconfig *) dri->driver_configs[i]);
   free(dri->driver_configs);
   dlclose(dri->driver);
   free(dri->base.driver_name);

   free(dri);
}

static struct gbm_device *
dri_device_create(int fd)
{
   struct gbm_dri_device *dri;
   int ret, force_sw;

   dri = calloc(1, sizeof *dri);
   if (!dri)
      return NULL;

   dri->base.base.fd = fd;
   dri->base.base.bo_create = gbm_dri_bo_create;
   dri->base.base.bo_import = gbm_dri_bo_import;
   dri->base.base.is_format_supported = gbm_dri_is_format_supported;
   dri->base.base.bo_write = gbm_dri_bo_write;
   dri->base.base.bo_get_fd = gbm_dri_bo_get_fd;
   dri->base.base.bo_destroy = gbm_dri_bo_destroy;
   dri->base.base.destroy = dri_destroy;
   dri->base.base.surface_create = gbm_dri_surface_create;
   dri->base.base.surface_destroy = gbm_dri_surface_destroy;

   dri->base.type = GBM_DRM_DRIVER_TYPE_DRI;
   dri->base.base.name = "drm";

   force_sw = getenv("GBM_ALWAYS_SOFTWARE") != NULL;
   if (!force_sw) {
      ret = dri_screen_create(dri);
      if (ret)
         ret = dri_screen_create_sw(dri);
   } else {
      ret = dri_screen_create_sw(dri);
   }

   if (ret)
      goto err_dri;

   return &dri->base.base;

err_dri:
   free(dri);

   return NULL;
}

struct gbm_backend gbm_dri_backend = {
   .backend_name = "dri",
   .create_device = dri_device_create,
};
