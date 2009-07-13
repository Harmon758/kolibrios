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
#include "radeon_reg.h"
#include "radeon.h"

/* r520,rv530,rv560,rv570,r580 depends on : */
void r100_hdp_reset(struct radeon_device *rdev);
int rv370_pcie_gart_enable(struct radeon_device *rdev);
void rv370_pcie_gart_disable(struct radeon_device *rdev);
void r420_pipes_init(struct radeon_device *rdev);
void rs600_mc_disable_clients(struct radeon_device *rdev);
void rs600_disable_vga(struct radeon_device *rdev);
int rv515_debugfs_pipes_info_init(struct radeon_device *rdev);
int rv515_debugfs_ga_info_init(struct radeon_device *rdev);

/* This files gather functions specifics to:
 * r520,rv530,rv560,rv570,r580
 *
 * Some of these functions might be used by newer ASICs.
 */
void r520_gpu_init(struct radeon_device *rdev);
int r520_mc_wait_for_idle(struct radeon_device *rdev);

/*
 * MC
 */
int r520_mc_init(struct radeon_device *rdev)
{
	uint32_t tmp;
	int r;

    dbgprintf("%s\n",__FUNCTION__);

//   if (r100_debugfs_rbbm_init(rdev)) {
//       DRM_ERROR("Failed to register debugfs file for RBBM !\n");
//   }
//   if (rv515_debugfs_pipes_info_init(rdev)) {
//       DRM_ERROR("Failed to register debugfs file for pipes !\n");
//   }
//   if (rv515_debugfs_ga_info_init(rdev)) {
//       DRM_ERROR("Failed to register debugfs file for pipes !\n");
//   }

	r520_gpu_init(rdev);
	rv370_pcie_gart_disable(rdev);

	/* Setup GPU memory space */
	rdev->mc.vram_location = 0xFFFFFFFFUL;
	rdev->mc.gtt_location = 0xFFFFFFFFUL;
	if (rdev->flags & RADEON_IS_AGP) {
		r = radeon_agp_init(rdev);
		if (r) {
			printk(KERN_WARNING "[drm] Disabling AGP\n");
			rdev->flags &= ~RADEON_IS_AGP;
			rdev->mc.gtt_size = radeon_gart_size * 1024 * 1024;
		} else {
			rdev->mc.gtt_location = rdev->mc.agp_base;
		}
	}
	r = radeon_mc_setup(rdev);
	if (r) {
		return r;
	}

	/* Program GPU memory space */
    rs600_mc_disable_clients(rdev);
    if (r520_mc_wait_for_idle(rdev)) {
       printk(KERN_WARNING "Failed to wait MC idle while "
		       "programming pipes. Bad things might happen.\n");
	}
	/* Write VRAM size in case we are limiting it */
	WREG32(RADEON_CONFIG_MEMSIZE, rdev->mc.vram_size);
	tmp = rdev->mc.vram_location + rdev->mc.vram_size - 1;
	tmp = REG_SET(R520_MC_FB_TOP, tmp >> 16);
	tmp |= REG_SET(R520_MC_FB_START, rdev->mc.vram_location >> 16);
	WREG32_MC(R520_MC_FB_LOCATION, tmp);
	WREG32(RS690_HDP_FB_LOCATION, rdev->mc.vram_location >> 16);
	WREG32(0x310, rdev->mc.vram_location);
	if (rdev->flags & RADEON_IS_AGP) {
		tmp = rdev->mc.gtt_location + rdev->mc.gtt_size - 1;
		tmp = REG_SET(R520_MC_AGP_TOP, tmp >> 16);
		tmp |= REG_SET(R520_MC_AGP_START, rdev->mc.gtt_location >> 16);
		WREG32_MC(R520_MC_AGP_LOCATION, tmp);
		WREG32_MC(R520_MC_AGP_BASE, rdev->mc.agp_base);
		WREG32_MC(R520_MC_AGP_BASE_2, 0);
	} else {
		WREG32_MC(R520_MC_AGP_LOCATION, 0x0FFFFFFF);
		WREG32_MC(R520_MC_AGP_BASE, 0);
		WREG32_MC(R520_MC_AGP_BASE_2, 0);
	}

    dbgprintf("done: %s\n",__FUNCTION__);

	return 0;
}

void r520_mc_fini(struct radeon_device *rdev)
{
	rv370_pcie_gart_disable(rdev);
	radeon_gart_table_vram_free(rdev);
	radeon_gart_fini(rdev);
}


/*
 * Global GPU functions
 */
void r520_errata(struct radeon_device *rdev)
{
	rdev->pll_errata = 0;
}

int r520_mc_wait_for_idle(struct radeon_device *rdev)
{
	unsigned i;
	uint32_t tmp;

	for (i = 0; i < rdev->usec_timeout; i++) {
		/* read MC_STATUS */
		tmp = RREG32_MC(R520_MC_STATUS);
		if (tmp & R520_MC_STATUS_IDLE) {
			return 0;
		}
		DRM_UDELAY(1);
	}
	return -1;
}

void r520_gpu_init(struct radeon_device *rdev)
{
	unsigned pipe_select_current, gb_pipe_select, tmp;
    dbgprintf("%s\n",__FUNCTION__);

	r100_hdp_reset(rdev);
	rs600_disable_vga(rdev);
	/*
	 * DST_PIPE_CONFIG		0x170C
	 * GB_TILE_CONFIG		0x4018
	 * GB_FIFO_SIZE			0x4024
	 * GB_PIPE_SELECT		0x402C
	 * GB_PIPE_SELECT2              0x4124
	 *	Z_PIPE_SHIFT			0
	 *	Z_PIPE_MASK			0x000000003
	 * GB_FIFO_SIZE2                0x4128
	 *	SC_SFIFO_SIZE_SHIFT		0
	 *	SC_SFIFO_SIZE_MASK		0x000000003
	 *	SC_MFIFO_SIZE_SHIFT		2
	 *	SC_MFIFO_SIZE_MASK		0x00000000C
	 *	FG_SFIFO_SIZE_SHIFT		4
	 *	FG_SFIFO_SIZE_MASK		0x000000030
	 *	ZB_MFIFO_SIZE_SHIFT		6
	 *	ZB_MFIFO_SIZE_MASK		0x0000000C0
	 * GA_ENHANCE			0x4274
	 * SU_REG_DEST			0x42C8
	 */
	/* workaround for RV530 */
	if (rdev->family == CHIP_RV530) {
		WREG32(0x4124, 1);
		WREG32(0x4128, 0xFF);
	}
	r420_pipes_init(rdev);
	gb_pipe_select = RREG32(0x402C);
	tmp = RREG32(0x170C);
	pipe_select_current = (tmp >> 2) & 3;
	tmp = (1 << pipe_select_current) |
	      (((gb_pipe_select >> 8) & 0xF) << 4);
	WREG32_PLL(0x000D, tmp);
	if (r520_mc_wait_for_idle(rdev)) {
		printk(KERN_WARNING "Failed to wait MC idle while "
		       "programming pipes. Bad things might happen.\n");
	}
}


/*
 * VRAM info
 */
static void r520_vram_get_type(struct radeon_device *rdev)
{
	uint32_t tmp;
    dbgprintf("%s\n",__FUNCTION__);

	rdev->mc.vram_width = 128;
	rdev->mc.vram_is_ddr = true;
	tmp = RREG32_MC(R520_MC_CNTL0);
	switch ((tmp & R520_MEM_NUM_CHANNELS_MASK) >> R520_MEM_NUM_CHANNELS_SHIFT) {
	case 0:
		rdev->mc.vram_width = 32;
		break;
	case 1:
		rdev->mc.vram_width = 64;
		break;
	case 2:
		rdev->mc.vram_width = 128;
		break;
	case 3:
		rdev->mc.vram_width = 256;
		break;
	default:
		rdev->mc.vram_width = 128;
		break;
	}
	if (tmp & R520_MC_CHANNEL_SIZE)
		rdev->mc.vram_width *= 2;
}

void r520_vram_info(struct radeon_device *rdev)
{
	r520_vram_get_type(rdev);
	rdev->mc.vram_size = RREG32(RADEON_CONFIG_MEMSIZE);

	rdev->mc.aper_base = drm_get_resource_start(rdev->ddev, 0);
	rdev->mc.aper_size = drm_get_resource_len(rdev->ddev, 0);
}


int radeon_agp_init(struct radeon_device *rdev)
{

    dbgprintf("%s\n",__FUNCTION__);

#if __OS_HAS_AGP
    struct radeon_agpmode_quirk *p = radeon_agpmode_quirk_list;
    struct drm_agp_mode mode;
    struct drm_agp_info info;
    uint32_t agp_status;
    int default_mode;
    bool is_v3;
    int ret;

    /* Acquire AGP. */
    if (!rdev->ddev->agp->acquired) {
        ret = drm_agp_acquire(rdev->ddev);
        if (ret) {
            DRM_ERROR("Unable to acquire AGP: %d\n", ret);
            return ret;
        }
    }

    ret = drm_agp_info(rdev->ddev, &info);
    if (ret) {
        DRM_ERROR("Unable to get AGP info: %d\n", ret);
        return ret;
    }
    mode.mode = info.mode;
    agp_status = (RREG32(RADEON_AGP_STATUS) | RADEON_AGPv3_MODE) & mode.mode;
    is_v3 = !!(agp_status & RADEON_AGPv3_MODE);

    if (is_v3) {
        default_mode = (agp_status & RADEON_AGPv3_8X_MODE) ? 8 : 4;
    } else {
        if (agp_status & RADEON_AGP_4X_MODE) {
            default_mode = 4;
        } else if (agp_status & RADEON_AGP_2X_MODE) {
            default_mode = 2;
        } else {
            default_mode = 1;
        }
    }

    /* Apply AGPMode Quirks */
    while (p && p->chip_device != 0) {
        if (info.id_vendor == p->hostbridge_vendor &&
            info.id_device == p->hostbridge_device &&
            rdev->pdev->vendor == p->chip_vendor &&
            rdev->pdev->device == p->chip_device &&
            rdev->pdev->subsystem_vendor == p->subsys_vendor &&
            rdev->pdev->subsystem_device == p->subsys_device) {
            default_mode = p->default_mode;
        }
        ++p;
    }

    if (radeon_agpmode > 0) {
        if ((radeon_agpmode < (is_v3 ? 4 : 1)) ||
            (radeon_agpmode > (is_v3 ? 8 : 4)) ||
            (radeon_agpmode & (radeon_agpmode - 1))) {
            DRM_ERROR("Illegal AGP Mode: %d (valid %s), leaving at %d\n",
                  radeon_agpmode, is_v3 ? "4, 8" : "1, 2, 4",
                  default_mode);
            radeon_agpmode = default_mode;
        } else {
            DRM_INFO("AGP mode requested: %d\n", radeon_agpmode);
        }
    } else {
        radeon_agpmode = default_mode;
    }

    mode.mode &= ~RADEON_AGP_MODE_MASK;
    if (is_v3) {
        switch (radeon_agpmode) {
        case 8:
            mode.mode |= RADEON_AGPv3_8X_MODE;
            break;
        case 4:
        default:
            mode.mode |= RADEON_AGPv3_4X_MODE;
            break;
        }
    } else {
        switch (radeon_agpmode) {
        case 4:
            mode.mode |= RADEON_AGP_4X_MODE;
            break;
        case 2:
            mode.mode |= RADEON_AGP_2X_MODE;
            break;
        case 1:
        default:
            mode.mode |= RADEON_AGP_1X_MODE;
            break;
        }
    }

    mode.mode &= ~RADEON_AGP_FW_MODE; /* disable fw */
    ret = drm_agp_enable(rdev->ddev, mode);
    if (ret) {
        DRM_ERROR("Unable to enable AGP (mode = 0x%lx)\n", mode.mode);
        return ret;
    }

    rdev->mc.agp_base = rdev->ddev->agp->agp_info.aper_base;
    rdev->mc.gtt_size = rdev->ddev->agp->agp_info.aper_size << 20;

    /* workaround some hw issues */
    if (rdev->family < CHIP_R200) {
        WREG32(RADEON_AGP_CNTL, RREG32(RADEON_AGP_CNTL) | 0x000e0000);
    }
    return 0;
#else
    return 0;
#endif
}




void rv370_pcie_gart_tlb_flush(struct radeon_device *rdev);




int radeon_fence_driver_init(struct radeon_device *rdev)
{
    unsigned long irq_flags;
    int r;

//    write_lock_irqsave(&rdev->fence_drv.lock, irq_flags);
    r = radeon_scratch_get(rdev, &rdev->fence_drv.scratch_reg);
    if (r) {
        DRM_ERROR("Fence failed to get a scratch register.");
//        write_unlock_irqrestore(&rdev->fence_drv.lock, irq_flags);
        return r;
    }
    WREG32(rdev->fence_drv.scratch_reg, 0);
//    atomic_set(&rdev->fence_drv.seq, 0);
//    INIT_LIST_HEAD(&rdev->fence_drv.created);
//    INIT_LIST_HEAD(&rdev->fence_drv.emited);
//    INIT_LIST_HEAD(&rdev->fence_drv.signaled);
    rdev->fence_drv.count_timeout = 0;
//    init_waitqueue_head(&rdev->fence_drv.queue);
//    write_unlock_irqrestore(&rdev->fence_drv.lock, irq_flags);
//    if (radeon_debugfs_fence_init(rdev)) {
//        DRM_ERROR("Failed to register debugfs file for fence !\n");
//    }
    return 0;
}





//domodedovo 9-00    16/07/2009
