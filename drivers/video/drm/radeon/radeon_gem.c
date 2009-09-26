/*
 * Copyright 2008 Advanced Micro Devices, Inc.
 * Copyright 2008 Red Hat Inc.
 * Copyright 2009 Jerome Glisse.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Dave Airlie
 *          Alex Deucher
 *          Jerome Glisse
 */
#include "drmP.h"
#include "drm.h"
#include "radeon_drm.h"
#include "radeon.h"


#define TTM_PL_SYSTEM           0
#define TTM_PL_TT               1
#define TTM_PL_VRAM             2
#define TTM_PL_PRIV0            3
#define TTM_PL_PRIV1            4
#define TTM_PL_PRIV2            5
#define TTM_PL_PRIV3            6
#define TTM_PL_PRIV4            7
#define TTM_PL_PRIV5            8
#define TTM_PL_SWAPPED          15

#define TTM_PL_FLAG_SYSTEM      (1 << TTM_PL_SYSTEM)
#define TTM_PL_FLAG_TT          (1 << TTM_PL_TT)
#define TTM_PL_FLAG_VRAM        (1 << TTM_PL_VRAM)
#define TTM_PL_FLAG_PRIV0       (1 << TTM_PL_PRIV0)
#define TTM_PL_FLAG_PRIV1       (1 << TTM_PL_PRIV1)
#define TTM_PL_FLAG_PRIV2       (1 << TTM_PL_PRIV2)
#define TTM_PL_FLAG_PRIV3       (1 << TTM_PL_PRIV3)
#define TTM_PL_FLAG_PRIV4       (1 << TTM_PL_PRIV4)
#define TTM_PL_FLAG_PRIV5       (1 << TTM_PL_PRIV5)
#define TTM_PL_FLAG_SWAPPED     (1 << TTM_PL_SWAPPED)
#define TTM_PL_MASK_MEM         0x0000FFFF


int radeon_gem_object_init(struct drm_gem_object *obj)
{
	/* we do nothings here */
	return 0;
}

void radeon_gem_object_free(struct drm_gem_object *gobj)
{
	struct radeon_object *robj = gobj->driver_private;

	gobj->driver_private = NULL;
	if (robj) {
//       radeon_object_unref(&robj);
	}
}

int radeon_gem_object_create(struct radeon_device *rdev, int size,
			     int alignment, int initial_domain,
			     bool discardable, bool kernel,
			     bool interruptible,
			     struct drm_gem_object **obj)
{
	struct drm_gem_object *gobj;
	struct radeon_object *robj;
	int r;

	*obj = NULL;
	gobj = drm_gem_object_alloc(rdev->ddev, size);
	if (!gobj) {
		return -ENOMEM;
	}
	/* At least align on page size */
	if (alignment < PAGE_SIZE) {
		alignment = PAGE_SIZE;
	}
	r = radeon_object_create(rdev, gobj, size, kernel, initial_domain,
				 interruptible, &robj);
	if (r) {
		DRM_ERROR("Failed to allocate GEM object (%d, %d, %u)\n",
			  size, initial_domain, alignment);
//       mutex_lock(&rdev->ddev->struct_mutex);
//       drm_gem_object_unreference(gobj);
//       mutex_unlock(&rdev->ddev->struct_mutex);
		return r;
	}
	gobj->driver_private = robj;
	*obj = gobj;
	return 0;
}

int radeon_gem_object_pin(struct drm_gem_object *obj, uint32_t pin_domain,
			  uint64_t *gpu_addr)
{
	struct radeon_object *robj = obj->driver_private;
	uint32_t flags;

	switch (pin_domain) {
	case RADEON_GEM_DOMAIN_VRAM:
		flags = TTM_PL_FLAG_VRAM;
		break;
	case RADEON_GEM_DOMAIN_GTT:
		flags = TTM_PL_FLAG_TT;
		break;
	default:
		flags = TTM_PL_FLAG_SYSTEM;
		break;
	}
	return radeon_object_pin(robj, flags, gpu_addr);
}

void radeon_gem_object_unpin(struct drm_gem_object *obj)
{
	struct radeon_object *robj = obj->driver_private;
//   radeon_object_unpin(robj);
}

int radeon_gem_set_domain(struct drm_gem_object *gobj,
			  uint32_t rdomain, uint32_t wdomain)
{
	struct radeon_object *robj;
	uint32_t domain;
	int r;

	/* FIXME: reeimplement */
	robj = gobj->driver_private;
	/* work out where to validate the buffer to */
	domain = wdomain;
	if (!domain) {
		domain = rdomain;
	}
	if (!domain) {
		/* Do nothings */
		printk(KERN_WARNING "Set domain withou domain !\n");
		return 0;
	}
	if (domain == RADEON_GEM_DOMAIN_CPU) {
		/* Asking for cpu access wait for object idle */
//       r = radeon_object_wait(robj);
		if (r) {
			printk(KERN_ERR "Failed to wait for object !\n");
			return r;
		}
	}
	return 0;
}

int radeon_gem_init(struct radeon_device *rdev)
{
	INIT_LIST_HEAD(&rdev->gem.objects);
	return 0;
}

void radeon_gem_fini(struct radeon_device *rdev)
{
 //  radeon_object_force_delete(rdev);
}

#if 0
/*
 * GEM ioctls.
 */
int radeon_gem_info_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *filp)
{
	struct radeon_device *rdev = dev->dev_private;
	struct drm_radeon_gem_info *args = data;

	args->vram_size = rdev->mc.real_vram_size;
	/* FIXME: report somethings that makes sense */
	args->vram_visible = rdev->mc.real_vram_size - (4 * 1024 * 1024);
	args->gart_size = rdev->mc.gtt_size;
	return 0;
}

int radeon_gem_pread_ioctl(struct drm_device *dev, void *data,
			   struct drm_file *filp)
{
	/* TODO: implement */
	DRM_ERROR("unimplemented %s\n", __func__);
	return -ENOSYS;
}

int radeon_gem_pwrite_ioctl(struct drm_device *dev, void *data,
			    struct drm_file *filp)
{
	/* TODO: implement */
	DRM_ERROR("unimplemented %s\n", __func__);
	return -ENOSYS;
}

int radeon_gem_create_ioctl(struct drm_device *dev, void *data,
			    struct drm_file *filp)
{
	struct radeon_device *rdev = dev->dev_private;
	struct drm_radeon_gem_create *args = data;
	struct drm_gem_object *gobj;
	uint32_t handle;
	int r;

	/* create a gem object to contain this object in */
	args->size = roundup(args->size, PAGE_SIZE);
	r = radeon_gem_object_create(rdev, args->size, args->alignment,
				     args->initial_domain, false,
				     false, true, &gobj);
	if (r) {
		return r;
	}
	r = drm_gem_handle_create(filp, gobj, &handle);
	if (r) {
		mutex_lock(&dev->struct_mutex);
		drm_gem_object_unreference(gobj);
		mutex_unlock(&dev->struct_mutex);
		return r;
	}
	mutex_lock(&dev->struct_mutex);
	drm_gem_object_handle_unreference(gobj);
	mutex_unlock(&dev->struct_mutex);
	args->handle = handle;
	return 0;
}

int radeon_gem_set_domain_ioctl(struct drm_device *dev, void *data,
				struct drm_file *filp)
{
	/* transition the BO to a domain -
	 * just validate the BO into a certain domain */
	struct drm_radeon_gem_set_domain *args = data;
	struct drm_gem_object *gobj;
	struct radeon_object *robj;
	int r;

	/* for now if someone requests domain CPU -
	 * just make sure the buffer is finished with */

	/* just do a BO wait for now */
	gobj = drm_gem_object_lookup(dev, filp, args->handle);
	if (gobj == NULL) {
		return -EINVAL;
	}
	robj = gobj->driver_private;

	r = radeon_gem_set_domain(gobj, args->read_domains, args->write_domain);

	mutex_lock(&dev->struct_mutex);
	drm_gem_object_unreference(gobj);
	mutex_unlock(&dev->struct_mutex);
	return r;
}

int radeon_gem_mmap_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *filp)
{
	struct drm_radeon_gem_mmap *args = data;
	struct drm_gem_object *gobj;
	struct radeon_object *robj;
	int r;

	gobj = drm_gem_object_lookup(dev, filp, args->handle);
	if (gobj == NULL) {
		return -EINVAL;
	}
	robj = gobj->driver_private;
	r = radeon_object_mmap(robj, &args->addr_ptr);
	mutex_lock(&dev->struct_mutex);
	drm_gem_object_unreference(gobj);
	mutex_unlock(&dev->struct_mutex);
	return r;
}

int radeon_gem_busy_ioctl(struct drm_device *dev, void *data,
			  struct drm_file *filp)
{
	/* FIXME: implement */
	return 0;
}

int radeon_gem_wait_idle_ioctl(struct drm_device *dev, void *data,
			      struct drm_file *filp)
{
	struct drm_radeon_gem_wait_idle *args = data;
	struct drm_gem_object *gobj;
	struct radeon_object *robj;
	int r;

	gobj = drm_gem_object_lookup(dev, filp, args->handle);
	if (gobj == NULL) {
		return -EINVAL;
	}
	robj = gobj->driver_private;
	r = radeon_object_wait(robj);
	mutex_lock(&dev->struct_mutex);
	drm_gem_object_unreference(gobj);
	mutex_unlock(&dev->struct_mutex);
	return r;
}

#endif
