/*
 * Copyright © 2011 Marek Olšák <maraeo@gmail.com>
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS, AUTHORS
 * AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 */

#include "radeon_drm_cs.h"

#include "util/u_hash_table.h"
#include "util/u_memory.h"
#include "util/simple_list.h"
#include "util/list.h"
#include "os/os_thread.h"
#include "os/os_mman.h"
#include "os/os_time.h"

#include "state_tracker/drm_driver.h"

#include <sys/ioctl.h>
#include <xf86drm.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

static const struct pb_vtbl radeon_bo_vtbl;

static INLINE struct radeon_bo *radeon_bo(struct pb_buffer *bo)
{
    assert(bo->vtbl == &radeon_bo_vtbl);
    return (struct radeon_bo *)bo;
}

struct radeon_bo_va_hole {
    struct list_head list;
    uint64_t         offset;
    uint64_t         size;
};

struct radeon_bomgr {
    /* Base class. */
    struct pb_manager base;

    /* Winsys. */
    struct radeon_drm_winsys *rws;

    /* List of buffer GEM names. Protected by bo_handles_mutex. */
    struct util_hash_table *bo_names;
    /* List of buffer handles. Protectded by bo_handles_mutex. */
    struct util_hash_table *bo_handles;
    /* List of buffer virtual memory ranges. Protectded by bo_handles_mutex. */
    struct util_hash_table *bo_vas;
    pipe_mutex bo_handles_mutex;
    pipe_mutex bo_va_mutex;

    /* is virtual address supported */
    bool va;
    uint64_t va_offset;
    struct list_head va_holes;
};

static INLINE struct radeon_bomgr *radeon_bomgr(struct pb_manager *mgr)
{
    return (struct radeon_bomgr *)mgr;
}

static struct radeon_bo *get_radeon_bo(struct pb_buffer *_buf)
{
    struct radeon_bo *bo = NULL;

    if (_buf->vtbl == &radeon_bo_vtbl) {
        bo = radeon_bo(_buf);
    } else {
        struct pb_buffer *base_buf;
        pb_size offset;
        pb_get_base_buffer(_buf, &base_buf, &offset);

        if (base_buf->vtbl == &radeon_bo_vtbl)
            bo = radeon_bo(base_buf);
    }

    return bo;
}

static void radeon_bo_wait(struct pb_buffer *_buf, enum radeon_bo_usage usage)
{
    struct radeon_bo *bo = get_radeon_bo(_buf);
    struct drm_radeon_gem_wait_idle args = {0};

    while (p_atomic_read(&bo->num_active_ioctls)) {
        sched_yield();
    }

    args.handle = bo->handle;
    while (drmCommandWrite(bo->rws->fd, DRM_RADEON_GEM_WAIT_IDLE,
                           &args, sizeof(args)) == -EBUSY);
}

static boolean radeon_bo_is_busy(struct pb_buffer *_buf,
                                 enum radeon_bo_usage usage)
{
    struct radeon_bo *bo = get_radeon_bo(_buf);
    struct drm_radeon_gem_busy args = {0};

    if (p_atomic_read(&bo->num_active_ioctls)) {
        return TRUE;
    }

    args.handle = bo->handle;
    return drmCommandWriteRead(bo->rws->fd, DRM_RADEON_GEM_BUSY,
                               &args, sizeof(args)) != 0;
}

static enum radeon_bo_domain get_valid_domain(enum radeon_bo_domain domain)
{
    /* Zero domains the driver doesn't understand. */
    domain &= RADEON_DOMAIN_VRAM_GTT;

    /* If no domain is set, we must set something... */
    if (!domain)
        domain = RADEON_DOMAIN_VRAM_GTT;

    return domain;
}

static enum radeon_bo_domain radeon_bo_get_initial_domain(
		struct radeon_winsys_cs_handle *buf)
{
    struct radeon_bo *bo = (struct radeon_bo*)buf;
    struct drm_radeon_gem_op args;

    if (bo->rws->info.drm_minor < 38)
        return RADEON_DOMAIN_VRAM_GTT;

    memset(&args, 0, sizeof(args));
    args.handle = bo->handle;
    args.op = RADEON_GEM_OP_GET_INITIAL_DOMAIN;

    drmCommandWriteRead(bo->rws->fd, DRM_RADEON_GEM_OP,
                        &args, sizeof(args));

    /* GEM domains and winsys domains are defined the same. */
    return get_valid_domain(args.value);
}

static uint64_t radeon_bomgr_find_va(struct radeon_bomgr *mgr, uint64_t size, uint64_t alignment)
{
    struct radeon_bo_va_hole *hole, *n;
    uint64_t offset = 0, waste = 0;

    alignment = MAX2(alignment, 4096);
    size = align(size, 4096);

    pipe_mutex_lock(mgr->bo_va_mutex);
    /* first look for a hole */
    LIST_FOR_EACH_ENTRY_SAFE(hole, n, &mgr->va_holes, list) {
        offset = hole->offset;
        waste = offset % alignment;
        waste = waste ? alignment - waste : 0;
        offset += waste;
        if (offset >= (hole->offset + hole->size)) {
            continue;
        }
        if (!waste && hole->size == size) {
            offset = hole->offset;
            list_del(&hole->list);
            FREE(hole);
            pipe_mutex_unlock(mgr->bo_va_mutex);
            return offset;
        }
        if ((hole->size - waste) > size) {
            if (waste) {
                n = CALLOC_STRUCT(radeon_bo_va_hole);
                n->size = waste;
                n->offset = hole->offset;
                list_add(&n->list, &hole->list);
            }
            hole->size -= (size + waste);
            hole->offset += size + waste;
            pipe_mutex_unlock(mgr->bo_va_mutex);
            return offset;
        }
        if ((hole->size - waste) == size) {
            hole->size = waste;
            pipe_mutex_unlock(mgr->bo_va_mutex);
            return offset;
        }
    }

    offset = mgr->va_offset;
    waste = offset % alignment;
    waste = waste ? alignment - waste : 0;
    if (waste) {
        n = CALLOC_STRUCT(radeon_bo_va_hole);
        n->size = waste;
        n->offset = offset;
        list_add(&n->list, &mgr->va_holes);
    }
    offset += waste;
    mgr->va_offset += size + waste;
    pipe_mutex_unlock(mgr->bo_va_mutex);
    return offset;
}

static void radeon_bomgr_free_va(struct radeon_bomgr *mgr, uint64_t va, uint64_t size)
{
    struct radeon_bo_va_hole *hole;

    size = align(size, 4096);

    pipe_mutex_lock(mgr->bo_va_mutex);
    if ((va + size) == mgr->va_offset) {
        mgr->va_offset = va;
        /* Delete uppermost hole if it reaches the new top */
        if (!LIST_IS_EMPTY(&mgr->va_holes)) {
            hole = container_of(mgr->va_holes.next, hole, list);
            if ((hole->offset + hole->size) == va) {
                mgr->va_offset = hole->offset;
                list_del(&hole->list);
                FREE(hole);
            }
        }
    } else {
        struct radeon_bo_va_hole *next;

        hole = container_of(&mgr->va_holes, hole, list);
        LIST_FOR_EACH_ENTRY(next, &mgr->va_holes, list) {
	    if (next->offset < va)
	        break;
            hole = next;
        }

        if (&hole->list != &mgr->va_holes) {
            /* Grow upper hole if it's adjacent */
            if (hole->offset == (va + size)) {
                hole->offset = va;
                hole->size += size;
                /* Merge lower hole if it's adjacent */
                if (next != hole && &next->list != &mgr->va_holes &&
                    (next->offset + next->size) == va) {
                    next->size += hole->size;
                    list_del(&hole->list);
                    FREE(hole);
                }
                goto out;
            }
        }

        /* Grow lower hole if it's adjacent */
        if (next != hole && &next->list != &mgr->va_holes &&
            (next->offset + next->size) == va) {
            next->size += size;
            goto out;
        }

        /* FIXME on allocation failure we just lose virtual address space
         * maybe print a warning
         */
        next = CALLOC_STRUCT(radeon_bo_va_hole);
        if (next) {
            next->size = size;
            next->offset = va;
            list_add(&next->list, &hole->list);
        }
    }
out:
    pipe_mutex_unlock(mgr->bo_va_mutex);
}

static void radeon_bo_destroy(struct pb_buffer *_buf)
{
    struct radeon_bo *bo = radeon_bo(_buf);
    struct radeon_bomgr *mgr = bo->mgr;
    struct drm_gem_close args;

    memset(&args, 0, sizeof(args));

    pipe_mutex_lock(bo->mgr->bo_handles_mutex);
    util_hash_table_remove(bo->mgr->bo_handles, (void*)(uintptr_t)bo->handle);
    if (bo->flink_name) {
        util_hash_table_remove(bo->mgr->bo_names,
                               (void*)(uintptr_t)bo->flink_name);
    }
    pipe_mutex_unlock(bo->mgr->bo_handles_mutex);

    if (bo->ptr)
        os_munmap(bo->ptr, bo->base.size);

    /* Close object. */
    args.handle = bo->handle;
    drmIoctl(bo->rws->fd, DRM_IOCTL_GEM_CLOSE, &args);

    if (mgr->va) {
        radeon_bomgr_free_va(mgr, bo->va, bo->base.size);
    }

    pipe_mutex_destroy(bo->map_mutex);

    if (bo->initial_domain & RADEON_DOMAIN_VRAM)
        bo->rws->allocated_vram -= align(bo->base.size, 4096);
    else if (bo->initial_domain & RADEON_DOMAIN_GTT)
        bo->rws->allocated_gtt -= align(bo->base.size, 4096);
    FREE(bo);
}

void *radeon_bo_do_map(struct radeon_bo *bo)
{
    struct drm_radeon_gem_mmap args = {0};
    void *ptr;

    /* If the buffer is created from user memory, return the user pointer. */
    if (bo->user_ptr)
        return bo->user_ptr;

    /* Return the pointer if it's already mapped. */
    if (bo->ptr)
        return bo->ptr;

    /* Map the buffer. */
    pipe_mutex_lock(bo->map_mutex);
    /* Return the pointer if it's already mapped (in case of a race). */
    if (bo->ptr) {
        pipe_mutex_unlock(bo->map_mutex);
        return bo->ptr;
    }
    args.handle = bo->handle;
    args.offset = 0;
    args.size = (uint64_t)bo->base.size;
    if (drmCommandWriteRead(bo->rws->fd,
                            DRM_RADEON_GEM_MMAP,
                            &args,
                            sizeof(args))) {
        pipe_mutex_unlock(bo->map_mutex);
        fprintf(stderr, "radeon: gem_mmap failed: %p 0x%08X\n",
                bo, bo->handle);
        return NULL;
    }

    ptr = os_mmap(0, args.size, PROT_READ|PROT_WRITE, MAP_SHARED,
               bo->rws->fd, args.addr_ptr);
    if (ptr == MAP_FAILED) {
        pipe_mutex_unlock(bo->map_mutex);
        fprintf(stderr, "radeon: mmap failed, errno: %i\n", errno);
        return NULL;
    }
    bo->ptr = ptr;
    pipe_mutex_unlock(bo->map_mutex);

    return bo->ptr;
}

static void *radeon_bo_map(struct radeon_winsys_cs_handle *buf,
                           struct radeon_winsys_cs *rcs,
                           enum pipe_transfer_usage usage)
{
    struct radeon_bo *bo = (struct radeon_bo*)buf;
    struct radeon_drm_cs *cs = (struct radeon_drm_cs*)rcs;

    /* If it's not unsynchronized bo_map, flush CS if needed and then wait. */
    if (!(usage & PIPE_TRANSFER_UNSYNCHRONIZED)) {
        /* DONTBLOCK doesn't make sense with UNSYNCHRONIZED. */
        if (usage & PIPE_TRANSFER_DONTBLOCK) {
            if (!(usage & PIPE_TRANSFER_WRITE)) {
                /* Mapping for read.
                 *
                 * Since we are mapping for read, we don't need to wait
                 * if the GPU is using the buffer for read too
                 * (neither one is changing it).
                 *
                 * Only check whether the buffer is being used for write. */
                if (cs && radeon_bo_is_referenced_by_cs_for_write(cs, bo)) {
                    cs->flush_cs(cs->flush_data, RADEON_FLUSH_ASYNC, NULL);
                    return NULL;
                }

                if (radeon_bo_is_busy((struct pb_buffer*)bo,
                                      RADEON_USAGE_WRITE)) {
                    return NULL;
                }
            } else {
                if (cs && radeon_bo_is_referenced_by_cs(cs, bo)) {
                    cs->flush_cs(cs->flush_data, RADEON_FLUSH_ASYNC, NULL);
                    return NULL;
                }

                if (radeon_bo_is_busy((struct pb_buffer*)bo,
                                      RADEON_USAGE_READWRITE)) {
                    return NULL;
                }
            }
        } else {
            uint64_t time = os_time_get_nano();

            if (!(usage & PIPE_TRANSFER_WRITE)) {
                /* Mapping for read.
                 *
                 * Since we are mapping for read, we don't need to wait
                 * if the GPU is using the buffer for read too
                 * (neither one is changing it).
                 *
                 * Only check whether the buffer is being used for write. */
                if (cs && radeon_bo_is_referenced_by_cs_for_write(cs, bo)) {
                    cs->flush_cs(cs->flush_data, 0, NULL);
                }
                radeon_bo_wait((struct pb_buffer*)bo,
                               RADEON_USAGE_WRITE);
            } else {
                /* Mapping for write. */
                if (cs) {
                    if (radeon_bo_is_referenced_by_cs(cs, bo)) {
                        cs->flush_cs(cs->flush_data, 0, NULL);
                    } else {
                        /* Try to avoid busy-waiting in radeon_bo_wait. */
                        if (p_atomic_read(&bo->num_active_ioctls))
                            radeon_drm_cs_sync_flush(rcs);
                    }
                }

                radeon_bo_wait((struct pb_buffer*)bo, RADEON_USAGE_READWRITE);
            }

            bo->mgr->rws->buffer_wait_time += os_time_get_nano() - time;
        }
    }

    return radeon_bo_do_map(bo);
}

static void radeon_bo_unmap(struct radeon_winsys_cs_handle *_buf)
{
    /* NOP */
}

static void radeon_bo_get_base_buffer(struct pb_buffer *buf,
                                      struct pb_buffer **base_buf,
                                      unsigned *offset)
{
    *base_buf = buf;
    *offset = 0;
}

static enum pipe_error radeon_bo_validate(struct pb_buffer *_buf,
                                          struct pb_validate *vl,
                                          unsigned flags)
{
    /* Always pinned */
    return PIPE_OK;
}

static void radeon_bo_fence(struct pb_buffer *buf,
                            struct pipe_fence_handle *fence)
{
}

static const struct pb_vtbl radeon_bo_vtbl = {
    radeon_bo_destroy,
    NULL, /* never called */
    NULL, /* never called */
    radeon_bo_validate,
    radeon_bo_fence,
    radeon_bo_get_base_buffer,
};

#ifndef RADEON_GEM_GTT_WC
#define RADEON_GEM_GTT_WC		(1 << 2)
#endif
#ifndef RADEON_GEM_CPU_ACCESS
/* BO is expected to be accessed by the CPU */
#define RADEON_GEM_CPU_ACCESS		(1 << 3)
#endif
#ifndef RADEON_GEM_NO_CPU_ACCESS
/* CPU access is not expected to work for this BO */
#define RADEON_GEM_NO_CPU_ACCESS	(1 << 4)
#endif

static struct pb_buffer *radeon_bomgr_create_bo(struct pb_manager *_mgr,
                                                pb_size size,
                                                const struct pb_desc *desc)
{
    struct radeon_bomgr *mgr = radeon_bomgr(_mgr);
    struct radeon_drm_winsys *rws = mgr->rws;
    struct radeon_bo *bo;
    struct drm_radeon_gem_create args;
    struct radeon_bo_desc *rdesc = (struct radeon_bo_desc*)desc;
    int r;

    memset(&args, 0, sizeof(args));

    assert(rdesc->initial_domains);
    assert((rdesc->initial_domains &
            ~(RADEON_GEM_DOMAIN_GTT | RADEON_GEM_DOMAIN_VRAM)) == 0);

    args.size = size;
    args.alignment = desc->alignment;
    args.initial_domain = rdesc->initial_domains;
    args.flags = 0;

    if (rdesc->flags & RADEON_FLAG_GTT_WC)
        args.flags |= RADEON_GEM_GTT_WC;
    if (rdesc->flags & RADEON_FLAG_CPU_ACCESS)
        args.flags |= RADEON_GEM_CPU_ACCESS;
    if (rdesc->flags & RADEON_FLAG_NO_CPU_ACCESS)
        args.flags |= RADEON_GEM_NO_CPU_ACCESS;

    if (drmCommandWriteRead(rws->fd, DRM_RADEON_GEM_CREATE,
                            &args, sizeof(args))) {
        fprintf(stderr, "radeon: Failed to allocate a buffer:\n");
        fprintf(stderr, "radeon:    size      : %d bytes\n", size);
        fprintf(stderr, "radeon:    alignment : %d bytes\n", desc->alignment);
        fprintf(stderr, "radeon:    domains   : %d\n", args.initial_domain);
        fprintf(stderr, "radeon:    flags     : %d\n", args.flags);
        return NULL;
    }

    bo = CALLOC_STRUCT(radeon_bo);
    if (!bo)
        return NULL;

    pipe_reference_init(&bo->base.reference, 1);
    bo->base.alignment = desc->alignment;
    bo->base.usage = desc->usage;
    bo->base.size = size;
    bo->base.vtbl = &radeon_bo_vtbl;
    bo->mgr = mgr;
    bo->rws = mgr->rws;
    bo->handle = args.handle;
    bo->va = 0;
    bo->initial_domain = rdesc->initial_domains;
    pipe_mutex_init(bo->map_mutex);

    if (mgr->va) {
        struct drm_radeon_gem_va va;

        bo->va = radeon_bomgr_find_va(mgr, size, desc->alignment);

        va.handle = bo->handle;
        va.vm_id = 0;
        va.operation = RADEON_VA_MAP;
        va.flags = RADEON_VM_PAGE_READABLE |
                   RADEON_VM_PAGE_WRITEABLE |
                   RADEON_VM_PAGE_SNOOPED;
        va.offset = bo->va;
        r = drmCommandWriteRead(rws->fd, DRM_RADEON_GEM_VA, &va, sizeof(va));
        if (r && va.operation == RADEON_VA_RESULT_ERROR) {
            fprintf(stderr, "radeon: Failed to allocate virtual address for buffer:\n");
            fprintf(stderr, "radeon:    size      : %d bytes\n", size);
            fprintf(stderr, "radeon:    alignment : %d bytes\n", desc->alignment);
            fprintf(stderr, "radeon:    domains   : %d\n", args.initial_domain);
            fprintf(stderr, "radeon:    va        : 0x%016llx\n", (unsigned long long)bo->va);
            radeon_bo_destroy(&bo->base);
            return NULL;
        }
        pipe_mutex_lock(mgr->bo_handles_mutex);
        if (va.operation == RADEON_VA_RESULT_VA_EXIST) {
            struct pb_buffer *b = &bo->base;
            struct radeon_bo *old_bo =
                util_hash_table_get(mgr->bo_vas, (void*)(uintptr_t)va.offset);

            pipe_mutex_unlock(mgr->bo_handles_mutex);
            pb_reference(&b, &old_bo->base);
            return b;
        }

        util_hash_table_set(mgr->bo_vas, (void*)(uintptr_t)bo->va, bo);
        pipe_mutex_unlock(mgr->bo_handles_mutex);
    }

    if (rdesc->initial_domains & RADEON_DOMAIN_VRAM)
        rws->allocated_vram += align(size, 4096);
    else if (rdesc->initial_domains & RADEON_DOMAIN_GTT)
        rws->allocated_gtt += align(size, 4096);

    return &bo->base;
}

static void radeon_bomgr_flush(struct pb_manager *mgr)
{
    /* NOP */
}

/* This is for the cache bufmgr. */
static boolean radeon_bomgr_is_buffer_busy(struct pb_manager *_mgr,
                                           struct pb_buffer *_buf)
{
   struct radeon_bo *bo = radeon_bo(_buf);

   if (radeon_bo_is_referenced_by_any_cs(bo)) {
       return TRUE;
   }

   if (radeon_bo_is_busy((struct pb_buffer*)bo, RADEON_USAGE_READWRITE)) {
       return TRUE;
   }

   return FALSE;
}

static void radeon_bomgr_destroy(struct pb_manager *_mgr)
{
    struct radeon_bomgr *mgr = radeon_bomgr(_mgr);
    util_hash_table_destroy(mgr->bo_names);
    util_hash_table_destroy(mgr->bo_handles);
    util_hash_table_destroy(mgr->bo_vas);
    pipe_mutex_destroy(mgr->bo_handles_mutex);
    pipe_mutex_destroy(mgr->bo_va_mutex);
    FREE(mgr);
}

#define PTR_TO_UINT(x) ((unsigned)((intptr_t)(x)))

static unsigned handle_hash(void *key)
{
    return PTR_TO_UINT(key);
}

static int handle_compare(void *key1, void *key2)
{
    return PTR_TO_UINT(key1) != PTR_TO_UINT(key2);
}

struct pb_manager *radeon_bomgr_create(struct radeon_drm_winsys *rws)
{
    struct radeon_bomgr *mgr;

    mgr = CALLOC_STRUCT(radeon_bomgr);
    if (!mgr)
        return NULL;

    mgr->base.destroy = radeon_bomgr_destroy;
    mgr->base.create_buffer = radeon_bomgr_create_bo;
    mgr->base.flush = radeon_bomgr_flush;
    mgr->base.is_buffer_busy = radeon_bomgr_is_buffer_busy;

    mgr->rws = rws;
    mgr->bo_names = util_hash_table_create(handle_hash, handle_compare);
    mgr->bo_handles = util_hash_table_create(handle_hash, handle_compare);
    mgr->bo_vas = util_hash_table_create(handle_hash, handle_compare);
    pipe_mutex_init(mgr->bo_handles_mutex);
    pipe_mutex_init(mgr->bo_va_mutex);

    mgr->va = rws->info.r600_virtual_address;
    mgr->va_offset = rws->va_start;
    list_inithead(&mgr->va_holes);

    return &mgr->base;
}

static unsigned eg_tile_split(unsigned tile_split)
{
    switch (tile_split) {
    case 0:     tile_split = 64;    break;
    case 1:     tile_split = 128;   break;
    case 2:     tile_split = 256;   break;
    case 3:     tile_split = 512;   break;
    default:
    case 4:     tile_split = 1024;  break;
    case 5:     tile_split = 2048;  break;
    case 6:     tile_split = 4096;  break;
    }
    return tile_split;
}

static unsigned eg_tile_split_rev(unsigned eg_tile_split)
{
    switch (eg_tile_split) {
    case 64:    return 0;
    case 128:   return 1;
    case 256:   return 2;
    case 512:   return 3;
    default:
    case 1024:  return 4;
    case 2048:  return 5;
    case 4096:  return 6;
    }
}

static void radeon_bo_get_tiling(struct pb_buffer *_buf,
                                 enum radeon_bo_layout *microtiled,
                                 enum radeon_bo_layout *macrotiled,
                                 unsigned *bankw, unsigned *bankh,
                                 unsigned *tile_split,
                                 unsigned *stencil_tile_split,
                                 unsigned *mtilea,
                                 bool *scanout)
{
    struct radeon_bo *bo = get_radeon_bo(_buf);
    struct drm_radeon_gem_set_tiling args;

    memset(&args, 0, sizeof(args));

    args.handle = bo->handle;

    drmCommandWriteRead(bo->rws->fd,
                        DRM_RADEON_GEM_GET_TILING,
                        &args,
                        sizeof(args));

    *microtiled = RADEON_LAYOUT_LINEAR;
    *macrotiled = RADEON_LAYOUT_LINEAR;
    if (args.tiling_flags & RADEON_TILING_MICRO)
        *microtiled = RADEON_LAYOUT_TILED;
    else if (args.tiling_flags & RADEON_TILING_MICRO_SQUARE)
        *microtiled = RADEON_LAYOUT_SQUARETILED;

    if (args.tiling_flags & RADEON_TILING_MACRO)
        *macrotiled = RADEON_LAYOUT_TILED;
    if (bankw && tile_split && stencil_tile_split && mtilea && tile_split) {
        *bankw = (args.tiling_flags >> RADEON_TILING_EG_BANKW_SHIFT) & RADEON_TILING_EG_BANKW_MASK;
        *bankh = (args.tiling_flags >> RADEON_TILING_EG_BANKH_SHIFT) & RADEON_TILING_EG_BANKH_MASK;
        *tile_split = (args.tiling_flags >> RADEON_TILING_EG_TILE_SPLIT_SHIFT) & RADEON_TILING_EG_TILE_SPLIT_MASK;
        *stencil_tile_split = (args.tiling_flags >> RADEON_TILING_EG_STENCIL_TILE_SPLIT_SHIFT) & RADEON_TILING_EG_STENCIL_TILE_SPLIT_MASK;
        *mtilea = (args.tiling_flags >> RADEON_TILING_EG_MACRO_TILE_ASPECT_SHIFT) & RADEON_TILING_EG_MACRO_TILE_ASPECT_MASK;
        *tile_split = eg_tile_split(*tile_split);
    }
    if (scanout)
        *scanout = bo->rws->gen >= DRV_SI && !(args.tiling_flags & RADEON_TILING_R600_NO_SCANOUT);
}

static void radeon_bo_set_tiling(struct pb_buffer *_buf,
                                 struct radeon_winsys_cs *rcs,
                                 enum radeon_bo_layout microtiled,
                                 enum radeon_bo_layout macrotiled,
                                 unsigned bankw, unsigned bankh,
                                 unsigned tile_split,
                                 unsigned stencil_tile_split,
                                 unsigned mtilea,
                                 uint32_t pitch,
                                 bool scanout)
{
    struct radeon_bo *bo = get_radeon_bo(_buf);
    struct radeon_drm_cs *cs = radeon_drm_cs(rcs);
    struct drm_radeon_gem_set_tiling args;

    memset(&args, 0, sizeof(args));

    /* Tiling determines how DRM treats the buffer data.
     * We must flush CS when changing it if the buffer is referenced. */
    if (cs && radeon_bo_is_referenced_by_cs(cs, bo)) {
        cs->flush_cs(cs->flush_data, 0, NULL);
    }

    while (p_atomic_read(&bo->num_active_ioctls)) {
        sched_yield();
    }

    if (microtiled == RADEON_LAYOUT_TILED)
        args.tiling_flags |= RADEON_TILING_MICRO;
    else if (microtiled == RADEON_LAYOUT_SQUARETILED)
        args.tiling_flags |= RADEON_TILING_MICRO_SQUARE;

    if (macrotiled == RADEON_LAYOUT_TILED)
        args.tiling_flags |= RADEON_TILING_MACRO;

    args.tiling_flags |= (bankw & RADEON_TILING_EG_BANKW_MASK) <<
        RADEON_TILING_EG_BANKW_SHIFT;
    args.tiling_flags |= (bankh & RADEON_TILING_EG_BANKH_MASK) <<
        RADEON_TILING_EG_BANKH_SHIFT;
    if (tile_split) {
	args.tiling_flags |= (eg_tile_split_rev(tile_split) &
			      RADEON_TILING_EG_TILE_SPLIT_MASK) <<
	    RADEON_TILING_EG_TILE_SPLIT_SHIFT;
    }
    args.tiling_flags |= (stencil_tile_split &
			  RADEON_TILING_EG_STENCIL_TILE_SPLIT_MASK) <<
        RADEON_TILING_EG_STENCIL_TILE_SPLIT_SHIFT;
    args.tiling_flags |= (mtilea & RADEON_TILING_EG_MACRO_TILE_ASPECT_MASK) <<
        RADEON_TILING_EG_MACRO_TILE_ASPECT_SHIFT;

    if (bo->rws->gen >= DRV_SI && !scanout)
        args.tiling_flags |= RADEON_TILING_R600_NO_SCANOUT;

    args.handle = bo->handle;
    args.pitch = pitch;

    drmCommandWriteRead(bo->rws->fd,
                        DRM_RADEON_GEM_SET_TILING,
                        &args,
                        sizeof(args));
}

static struct radeon_winsys_cs_handle *radeon_drm_get_cs_handle(struct pb_buffer *_buf)
{
    /* return radeon_bo. */
    return (struct radeon_winsys_cs_handle*)get_radeon_bo(_buf);
}

static struct pb_buffer *
radeon_winsys_bo_create(struct radeon_winsys *rws,
                        unsigned size,
                        unsigned alignment,
                        boolean use_reusable_pool,
                        enum radeon_bo_domain domain,
                        enum radeon_bo_flag flags)
{
    struct radeon_drm_winsys *ws = radeon_drm_winsys(rws);
    struct radeon_bomgr *mgr = radeon_bomgr(ws->kman);
    struct radeon_bo_desc desc;
    struct pb_manager *provider;
    struct pb_buffer *buffer;

    memset(&desc, 0, sizeof(desc));
    desc.base.alignment = alignment;

    /* Only set one usage bit each for domains and flags, or the cache manager
     * might consider different sets of domains / flags compatible
     */
    if (domain == RADEON_DOMAIN_VRAM_GTT)
        desc.base.usage = 1 << 2;
    else
        desc.base.usage = domain >> 1;
    assert(flags < sizeof(desc.base.usage) * 8 - 3);
    desc.base.usage |= 1 << (flags + 3);

    desc.initial_domains = domain;
    desc.flags = flags;

    /* Assign a buffer manager. */
    if (use_reusable_pool)
        provider = ws->cman;
    else
        provider = ws->kman;

    buffer = provider->create_buffer(provider, size, &desc.base);
    if (!buffer)
        return NULL;

    pipe_mutex_lock(mgr->bo_handles_mutex);
    util_hash_table_set(mgr->bo_handles, (void*)(uintptr_t)get_radeon_bo(buffer)->handle, buffer);
    pipe_mutex_unlock(mgr->bo_handles_mutex);

    return (struct pb_buffer*)buffer;
}

static struct pb_buffer *radeon_winsys_bo_from_ptr(struct radeon_winsys *rws,
                                                   void *pointer, unsigned size)
{
    struct radeon_drm_winsys *ws = radeon_drm_winsys(rws);
    struct radeon_bomgr *mgr = radeon_bomgr(ws->kman);
    struct drm_radeon_gem_userptr args;
    struct radeon_bo *bo;
    int r;

    bo = CALLOC_STRUCT(radeon_bo);
    if (!bo)
        return NULL;

    memset(&args, 0, sizeof(args));
    args.addr = (uintptr_t)pointer;
    args.size = align(size, sysconf(_SC_PAGE_SIZE));
    args.flags = RADEON_GEM_USERPTR_ANONONLY |
        RADEON_GEM_USERPTR_VALIDATE |
        RADEON_GEM_USERPTR_REGISTER;
    if (drmCommandWriteRead(ws->fd, DRM_RADEON_GEM_USERPTR,
                            &args, sizeof(args))) {
        FREE(bo);
        return NULL;
    }

    pipe_mutex_lock(mgr->bo_handles_mutex);

    /* Initialize it. */
    pipe_reference_init(&bo->base.reference, 1);
    bo->handle = args.handle;
    bo->base.alignment = 0;
    bo->base.usage = PB_USAGE_GPU_WRITE | PB_USAGE_GPU_READ;
    bo->base.size = size;
    bo->base.vtbl = &radeon_bo_vtbl;
    bo->mgr = mgr;
    bo->rws = mgr->rws;
    bo->user_ptr = pointer;
    bo->va = 0;
    bo->initial_domain = RADEON_DOMAIN_GTT;
    pipe_mutex_init(bo->map_mutex);

    util_hash_table_set(mgr->bo_handles, (void*)(uintptr_t)bo->handle, bo);

    pipe_mutex_unlock(mgr->bo_handles_mutex);

    if (mgr->va) {
        struct drm_radeon_gem_va va;

        bo->va = radeon_bomgr_find_va(mgr, bo->base.size, 1 << 20);

        va.handle = bo->handle;
        va.operation = RADEON_VA_MAP;
        va.vm_id = 0;
        va.offset = bo->va;
        va.flags = RADEON_VM_PAGE_READABLE |
                   RADEON_VM_PAGE_WRITEABLE |
                   RADEON_VM_PAGE_SNOOPED;
        va.offset = bo->va;
        r = drmCommandWriteRead(ws->fd, DRM_RADEON_GEM_VA, &va, sizeof(va));
        if (r && va.operation == RADEON_VA_RESULT_ERROR) {
            fprintf(stderr, "radeon: Failed to assign virtual address space\n");
            radeon_bo_destroy(&bo->base);
            return NULL;
        }
        pipe_mutex_lock(mgr->bo_handles_mutex);
        if (va.operation == RADEON_VA_RESULT_VA_EXIST) {
            struct pb_buffer *b = &bo->base;
            struct radeon_bo *old_bo =
                util_hash_table_get(mgr->bo_vas, (void*)(uintptr_t)va.offset);

            pipe_mutex_unlock(mgr->bo_handles_mutex);
            pb_reference(&b, &old_bo->base);
            return b;
        }

        util_hash_table_set(mgr->bo_vas, (void*)(uintptr_t)bo->va, bo);
        pipe_mutex_unlock(mgr->bo_handles_mutex);
    }

    ws->allocated_gtt += align(bo->base.size, 4096);

    return (struct pb_buffer*)bo;
}

static struct pb_buffer *radeon_winsys_bo_from_handle(struct radeon_winsys *rws,
                                                      struct winsys_handle *whandle,
                                                      unsigned *stride)
{
    struct radeon_drm_winsys *ws = radeon_drm_winsys(rws);
    struct radeon_bo *bo;
    struct radeon_bomgr *mgr = radeon_bomgr(ws->kman);
    int r;
    unsigned handle;
    uint64_t size = 0;

    /* We must maintain a list of pairs <handle, bo>, so that we always return
     * the same BO for one particular handle. If we didn't do that and created
     * more than one BO for the same handle and then relocated them in a CS,
     * we would hit a deadlock in the kernel.
     *
     * The list of pairs is guarded by a mutex, of course. */
    pipe_mutex_lock(mgr->bo_handles_mutex);

    if (whandle->type == DRM_API_HANDLE_TYPE_SHARED) {
        /* First check if there already is an existing bo for the handle. */
        bo = util_hash_table_get(mgr->bo_names, (void*)(uintptr_t)whandle->handle);
    } else if (whandle->type == DRM_API_HANDLE_TYPE_FD) {
        /* We must first get the GEM handle, as fds are unreliable keys */
        r = drmPrimeFDToHandle(ws->fd, whandle->handle, &handle);
        if (r)
            goto fail;
        bo = util_hash_table_get(mgr->bo_handles, (void*)(uintptr_t)handle);
    } else {
        /* Unknown handle type */
        goto fail;
    }

    if (bo) {
        /* Increase the refcount. */
        struct pb_buffer *b = NULL;
        pb_reference(&b, &bo->base);
        goto done;
    }

    /* There isn't, create a new one. */
    bo = CALLOC_STRUCT(radeon_bo);
    if (!bo) {
        goto fail;
    }

    if (whandle->type == DRM_API_HANDLE_TYPE_SHARED) {
        struct drm_gem_open open_arg = {};
        memset(&open_arg, 0, sizeof(open_arg));
        /* Open the BO. */
        open_arg.name = whandle->handle;
        if (drmIoctl(ws->fd, DRM_IOCTL_GEM_OPEN, &open_arg)) {
            FREE(bo);
            goto fail;
        }
        handle = open_arg.handle;
        size = open_arg.size;
        bo->flink_name = whandle->handle;
    } else if (whandle->type == DRM_API_HANDLE_TYPE_FD) {
        size = lseek(whandle->handle, 0, SEEK_END);
        /* 
         * Could check errno to determine whether the kernel is new enough, but
         * it doesn't really matter why this failed, just that it failed.
         */
        if (size == (off_t)-1) {
            FREE(bo);
            goto fail;
        }
        lseek(whandle->handle, 0, SEEK_SET);
    }

    bo->handle = handle;

    /* Initialize it. */
    pipe_reference_init(&bo->base.reference, 1);
    bo->base.alignment = 0;
    bo->base.usage = PB_USAGE_GPU_WRITE | PB_USAGE_GPU_READ;
    bo->base.size = (unsigned) size;
    bo->base.vtbl = &radeon_bo_vtbl;
    bo->mgr = mgr;
    bo->rws = mgr->rws;
    bo->va = 0;
    pipe_mutex_init(bo->map_mutex);

    if (bo->flink_name)
        util_hash_table_set(mgr->bo_names, (void*)(uintptr_t)bo->flink_name, bo);

    util_hash_table_set(mgr->bo_handles, (void*)(uintptr_t)bo->handle, bo);

done:
    pipe_mutex_unlock(mgr->bo_handles_mutex);

    if (stride)
        *stride = whandle->stride;

    if (mgr->va && !bo->va) {
        struct drm_radeon_gem_va va;

        bo->va = radeon_bomgr_find_va(mgr, bo->base.size, 1 << 20);

        va.handle = bo->handle;
        va.operation = RADEON_VA_MAP;
        va.vm_id = 0;
        va.offset = bo->va;
        va.flags = RADEON_VM_PAGE_READABLE |
                   RADEON_VM_PAGE_WRITEABLE |
                   RADEON_VM_PAGE_SNOOPED;
        va.offset = bo->va;
        r = drmCommandWriteRead(ws->fd, DRM_RADEON_GEM_VA, &va, sizeof(va));
        if (r && va.operation == RADEON_VA_RESULT_ERROR) {
            fprintf(stderr, "radeon: Failed to assign virtual address space\n");
            radeon_bo_destroy(&bo->base);
            return NULL;
        }
        pipe_mutex_lock(mgr->bo_handles_mutex);
        if (va.operation == RADEON_VA_RESULT_VA_EXIST) {
            struct pb_buffer *b = &bo->base;
            struct radeon_bo *old_bo =
                util_hash_table_get(mgr->bo_vas, (void*)(uintptr_t)va.offset);

            pipe_mutex_unlock(mgr->bo_handles_mutex);
            pb_reference(&b, &old_bo->base);
            return b;
        }

        util_hash_table_set(mgr->bo_vas, (void*)(uintptr_t)bo->va, bo);
        pipe_mutex_unlock(mgr->bo_handles_mutex);
    }

    bo->initial_domain = radeon_bo_get_initial_domain((void*)bo);

    if (bo->initial_domain & RADEON_DOMAIN_VRAM)
        ws->allocated_vram += align(bo->base.size, 4096);
    else if (bo->initial_domain & RADEON_DOMAIN_GTT)
        ws->allocated_gtt += align(bo->base.size, 4096);

    return (struct pb_buffer*)bo;

fail:
    pipe_mutex_unlock(mgr->bo_handles_mutex);
    return NULL;
}

static boolean radeon_winsys_bo_get_handle(struct pb_buffer *buffer,
                                           unsigned stride,
                                           struct winsys_handle *whandle)
{
    struct drm_gem_flink flink;
    struct radeon_bo *bo = get_radeon_bo(buffer);

    memset(&flink, 0, sizeof(flink));

    if (whandle->type == DRM_API_HANDLE_TYPE_SHARED) {
        if (!bo->flink_name) {
            flink.handle = bo->handle;

            if (ioctl(bo->rws->fd, DRM_IOCTL_GEM_FLINK, &flink)) {
                return FALSE;
            }

            bo->flink_name = flink.name;

            pipe_mutex_lock(bo->mgr->bo_handles_mutex);
            util_hash_table_set(bo->mgr->bo_names, (void*)(uintptr_t)bo->flink_name, bo);
            pipe_mutex_unlock(bo->mgr->bo_handles_mutex);
        }
        whandle->handle = bo->flink_name;
    } else if (whandle->type == DRM_API_HANDLE_TYPE_KMS) {
        whandle->handle = bo->handle;
    } else if (whandle->type == DRM_API_HANDLE_TYPE_FD) {
        if (drmPrimeHandleToFD(bo->rws->fd, bo->handle, DRM_CLOEXEC, (int*)&whandle->handle))
            return FALSE;
    }

    whandle->stride = stride;
    return TRUE;
}

static uint64_t radeon_winsys_bo_va(struct radeon_winsys_cs_handle *buf)
{
    return ((struct radeon_bo*)buf)->va;
}

void radeon_bomgr_init_functions(struct radeon_drm_winsys *ws)
{
    ws->base.buffer_get_cs_handle = radeon_drm_get_cs_handle;
    ws->base.buffer_set_tiling = radeon_bo_set_tiling;
    ws->base.buffer_get_tiling = radeon_bo_get_tiling;
    ws->base.buffer_map = radeon_bo_map;
    ws->base.buffer_unmap = radeon_bo_unmap;
    ws->base.buffer_wait = radeon_bo_wait;
    ws->base.buffer_is_busy = radeon_bo_is_busy;
    ws->base.buffer_create = radeon_winsys_bo_create;
    ws->base.buffer_from_handle = radeon_winsys_bo_from_handle;
    ws->base.buffer_from_ptr = radeon_winsys_bo_from_ptr;
    ws->base.buffer_get_handle = radeon_winsys_bo_get_handle;
    ws->base.buffer_get_virtual_address = radeon_winsys_bo_va;
    ws->base.buffer_get_initial_domain = radeon_bo_get_initial_domain;
}
