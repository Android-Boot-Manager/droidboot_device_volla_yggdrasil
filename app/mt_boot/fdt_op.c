/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2018. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <bootargs.h>
#include <errno.h>
#include <string.h>
#include <pal_assert.h>
#include <pal_typedefs.h>
#include <pal_log.h>
#include <memory_layout.h>
#include <libfdt.h>
#include <profiling.h>
#include <odm_mdtbo.h>
#include <boot_opt.h>
#include <part_interface.h>
#include <ufdt_overlay.h>
#include <fdt_op.h>
#include <bootimg.h>
#include <mt_boot.h>
#include <arch/arm/mmu.h>
#include <load_vfy_boot.h>
#include <mt_rtc.h>
#include <part_interface.h>
#include <part_status.h>
#include <mkimg.h>
#include <platform/mt_gpt.h>

void *g_fdt;
DTBO_SRC g_dtbo_load_src = DTBO_FROM_STANDALONE;
extern u32 g_64bit_dtb_size;
extern int g_is_64bit_kernel;
extern char *p_AB_suffix;


#define DTBO_PART_NEW_A_NAME "dtbo_a"

static char *dtbo_one_part_name = "dtbo";
#ifndef MTK_AB_OTA_UPDATER
static char *dtbo_part_names[] = {"dtbo1", "dtbo2"};
#endif
static char *dtbo_fallback_part_name= "odmdtbo";
static char dtbo_part_name_full[DTBO_PART_NAME_LEN];

extern int mboot_recovery_load_raw_part(char *part_name, unsigned long *addr, unsigned int size);
extern int unshield_recovery_detection(void);

DTBO_SRC get_dtbo_src(void)
{
	return g_dtbo_load_src;
}

/***********************************************************************
* API to retrieve where dtbo partition from, the name is fallback iterated
************************************************************************/
char *get_dtbo_part_name(void)
{
	char *part_name = NULL;
	static uint32_t dtbo_part_name_inited = 0;

	if (!dtbo_part_name_inited) {
#ifdef MTK_AB_OTA_UPDATER
		part_name = dtbo_part_name_full;
		if (partition_exists(DTBO_PART_NEW_A_NAME) == PART_OK)
			strncpy(part_name,dtbo_one_part_name, DTBO_PART_NAME_LEN);
		else
			strncpy(part_name, dtbo_fallback_part_name, DTBO_PART_NAME_LEN);

		int part_name_len = strlen(part_name);
		if (p_AB_suffix) {
			strncpy((void *)&part_name[part_name_len], (void *)p_AB_suffix,
				DTBO_PART_NAME_LEN - part_name_len);
		}
#else
		int i;
		int array_size = (int)(sizeof(dtbo_part_names) / sizeof(dtbo_part_names[0]));

		/* check "dtbo" */
		if (partition_exists(dtbo_one_part_name) == PART_OK) {
			part_name = dtbo_one_part_name;
			goto partname_selected;
		}
		/* check "dtbo1" */
		if (partition_exists(dtbo_part_names[0]) == PART_OK) {
			for (i = 0; i < array_size; i++) {
				if (partition_get_active_bit_by_name(dtbo_part_names[i]))
					part_name = dtbo_part_names[i];
			}
			if(part_name == NULL)
				part_name = dtbo_part_names[0];
		} else
			/* Fallback to old style */
			part_name = dtbo_fallback_part_name;
partname_selected:
		strncpy(dtbo_part_name_full, part_name, DTBO_PART_NAME_LEN);
#endif
		dtbo_part_name_full[DTBO_PART_NAME_LEN - 1] = '\0';
		dtbo_part_name_inited = 1;
	}
	return dtbo_part_name_full;
}


/***********************************************************************
* Done after bldr_load_dtb, overlay from associated dtbo partition
************************************************************************/
bool dtb_overlay(void *fdt, int size, uint64_t recovery_dtbo_offset)
{
	size_t overlay_len = 0;
	char *part_name = NULL;

	g_fdt = fdt;

	if (fdt == NULL) {
		pal_log_err("fdt is NULL\n");
		return FALSE;
	}
	if (size == 0) {
		pal_log_err("fdt size is zero\n");
		return FALSE;
	}

#ifdef MTK_AB_OTA_UPDATER
	part_name = get_dtbo_part_name();
#else
	/* for non-AB supporting bootimg hdr version 1, get dtbo from recovery */
	if (recovery_dtbo_offset == 0) {
		part_name = get_dtbo_part_name();
	}
	else {
		part_name = "recovery";
		g_dtbo_load_src = DTBO_FROM_RECOVERY;
	}
#endif  // MTK_AB_OTA_UPDATER

	// Note: A buffer is allocated in load_overlay_dtbo() to store the loaded
	//       odm dtb.
	char *overlay_buf = load_overlay_dtbo(part_name, &overlay_len, (uint64_t)recovery_dtbo_offset);
	if ((overlay_buf == NULL) || (overlay_len == 0)) {
		pal_log_err("load overlay dtbo failed !\n");
		return FALSE;
	}

	struct fdt_header *fdth = (struct fdt_header *)g_fdt;
	fdth->totalsize = cpu_to_fdt32(size);

	int ret = fdt_open_into(g_fdt, g_fdt, size);
	if (ret) {
		pal_log_err("fdt_open_into failed \n");
		free(overlay_buf);
		return FALSE;
	}
	ret = fdt_check_header(g_fdt);
	if (ret) {
		pal_log_err("fdt_check_header check failed !\n");
		free(overlay_buf);
		return FALSE;
	}

	char *base_buf = fdt;
	size_t blob_len = size;
	struct fdt_header *blob = ufdt_install_blob(base_buf, blob_len);
	if (!blob) {
		pal_log_err("ufdt_install_blob() failed!\n");
		free(overlay_buf);
		return FALSE;
	}
	pal_log_info("blob_len: 0x%x, overlay_len: 0x%x\n", blob_len, overlay_len);

	void *merged_fdt = NULL;
	PROFILING_START("Overlay");
	// Note: A buffer is allocated in ufdt_apply_overlay() to store the merge
	//       device tree.
	merged_fdt = ufdt_apply_overlay(blob, blob_len, overlay_buf, overlay_len);
	if (!merged_fdt) {
		pal_log_err("ufdt_apply_overlay() failed!\n");
		free(overlay_buf);
		assert(0);
		return FALSE;
	}
	PROFILING_END();

	// Compact the merged device tree so that the size of the device tree can
	// be known.
	ret = fdt_pack(merged_fdt);
	if (ret) {
		pal_log_err("fdt_pack(merged_fdt) failed !\n");
		free(merged_fdt);
		free(overlay_buf);
		return FALSE;
	}


	int merged_size = fdt_totalsize(merged_fdt);
	pal_log_info("fdt merged_size: %d\n", merged_size);
	if (merged_size > DTB_MAX_SIZE) {
		pal_log_err("Error: merged size %d > DTB_MAX_SIZE!\n", merged_size);
		free(merged_fdt);
		free(overlay_buf);
		return FALSE;
	}

	// The memory pointed to by "g_fdt" is the location that the Linux kernel
	// expects to find the device tree, and it is at least a few mega-bytes
	// free. The merged device tree is therefore copied to that space.
	memcpy(g_fdt, merged_fdt, merged_size);

	// Make the totalsize of the device tree larger so that properties can
	// be inserted into the device tree.
	((struct fdt_header *)g_fdt)->totalsize = cpu_to_fdt32(DTB_MAX_SIZE);

	free(merged_fdt);
	free(overlay_buf);

	return TRUE;
}


/*******************************************************************
* mmap fdt buffer and sanity check it before doing any operation
********************************************************************/
bool setup_fdt(void *fdt, int size)
{
	int ret;
	g_fdt = fdt;
#ifdef MTK_3LEVEL_PAGETABLE
	u32 addr = (u32)fdt;
	arch_mmu_map((uint64_t)ROUNDDOWN(addr, PAGE_SIZE),
		(uint32_t)ROUNDDOWN(addr, PAGE_SIZE),
		MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		ROUNDUP(size, PAGE_SIZE));
#endif
	ret = fdt_open_into(g_fdt, g_fdt, size); //DTB maximum size is 2MB
	if (ret) return FALSE;
	ret = fdt_check_header(g_fdt);
	if (ret) return FALSE;
	return TRUE;
}


/**************************************************************************
* Please refer to bootimg.h for boot image header structure.
* bootimg header size is 0x800, the kernel img text is next to the header
***************************************************************************/
int bldr_load_dtb(char *boot_load_partition)
{
	int ret = 0;
#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
	char *ptr;
	char *dtb_sz;
	u32 dtb_kernel_addr;
	u32 zimage_addr, zimage_size, dtb_size, addr;
	u32 dtb_addr = 0;
	u32 tmp;
	u32 offset = 0;
	unsigned char *magic;
	struct bootimg_hdr *p_boot_hdr;
	char part_name[16];
	uint64_t recovery_dtbo_offset = 0;

	ptr = malloc(DTB_MAX_SIZE);
	if (ptr == NULL) {
		pal_log_err("dtb malloc failed!\n");
		return -1;
	}

#ifdef MTK_AB_OTA_UPDATER
	/* no more recovery partition in A/B system update, instead choose boot_a or boot_b */
	snprintf(part_name, sizeof(part_name), "boot%s", p_AB_suffix);
#else
	snprintf(part_name, sizeof(part_name), "%s", boot_load_partition);
#endif

	//load boot hdr
	ret = mboot_recovery_load_raw_part(part_name, (unsigned long *)ptr,
					   sizeof(struct bootimg_hdr) + 0x800);
	if (ret < 0) {
		pal_log_err("mboot_recovery_load_raw_part(%s, %d) failed, ret: 0x%x\n",
			part_name, sizeof(struct bootimg_hdr) + 0x800, ret);
		goto _end;
	}

	if (0 == bootimg_hdr_valid((uint8_t *)ptr)) {
		pal_log_err("bootimg_hdr_valid failed\n");
		ret = -1;
		goto _end;
	}

	p_boot_hdr = (void *)ptr;
	dtb_kernel_addr = p_boot_hdr->tags_addr;

	/* Under recovery, get dtbo info from bootimg hdr version 1 */
	if (!strcmp(part_name, "recovery")) {
		if (p_boot_hdr->header_version == BOOT_HEADER_VERSION_ONE) {
			recovery_dtbo_offset = (uint64_t)p_boot_hdr->recovery_dtbo_offset;
			pal_log_err("bldr_load_dtb: recovery_dtbo_offset = 0x%llx\n", (uint64_t)recovery_dtbo_offset);
			/* If offset is 0, implies dtbo in its standalone partition */
			/* In that case, recovery is not self-sufficient */
			if (recovery_dtbo_offset == 0)
				pal_log_err("Warning: check recovery dtbo location!\n");
		}
	}

	/* Claim the DT/kernel/ramdisk addr from mblock */
	mboot_allocate_bootimg_from_mblock(p_boot_hdr);

	platform_parse_bootopt(p_boot_hdr->cmdline);

	if (!g_is_64bit_kernel) {
		//Offset into zImage    Value   Description
		//0x24  0x016F2818  Magic number used to identify this is an ARM Linux zImage
		//0x28  start address   The address the zImage starts at
		//0x2C  end address The address the zImage ends at
		/* bootimg header size is 0x800, the kernel img text is next to the header */
		zimage_addr = (u32)ptr + p_boot_hdr->page_sz;
		if (*(unsigned int *)((unsigned int)zimage_addr) == MKIMG_MAGIC) {
			zimage_addr += MKIMG_HDR_SZ;
			offset += MKIMG_HDR_SZ;
		}
		zimage_size = *(unsigned int *)((unsigned int)zimage_addr + 0x2c) -
			*(unsigned int *)((unsigned int)zimage_addr + 0x28);
		//dtb_addr = (unsigned int)zimage_addr + zimage_size;
		offset += (p_boot_hdr->page_sz + zimage_size);
		tmp = ROUNDDOWN(offset, p_boot_hdr->page_sz);
		ret = partition_read(part_name, (off_t)tmp, (u8 *)ptr, (size_t)DTB_MAX_SIZE);
		if (ret < 0) {
			pal_log_err("partition_read failed, ret: 0x%x\n", ret);
			goto _end;
		}
		dtb_addr = (u32)ptr + offset - tmp;
		dtb_size = fdt32_to_cpu(*(unsigned int *)(ptr + (offset - tmp) + 0x4));
	} else {
		/* bootimg header size is 0x800, the kernel img text is next to the header */
		int i;
		zimage_size = p_boot_hdr->kernel_sz;
		offset = p_boot_hdr->page_sz + p_boot_hdr->kernel_sz - DTB_MAX_SIZE;
		tmp = ROUNDUP(offset, p_boot_hdr->page_sz);
		ret = partition_read(part_name, (off_t)tmp, (u8 *)ptr, (size_t)DTB_MAX_SIZE);
		if (ret < 0) {
			pal_log_err("partition_read failed, ret: 0x%x\n", ret);
			goto _end;
		}
		dtb_addr = 0;
		dtb_size = 0;
		addr = (u32)ptr + DTB_MAX_SIZE - 4;
		for (i = 0; i < (DTB_MAX_SIZE - 4); i++, addr--) {
			//FDT_MAGIC 0xd00dfeed
			//dtb append after image.gz may not 4 byte alignment
			magic = (unsigned char *)addr;
			if (*(magic + 3) == 0xED && *(magic + 2) == 0xFE &&
				*(magic + 1) == 0x0D && *(magic + 0) == 0xD0) {
				dtb_addr = addr;
				break;
			}
		}
		if (dtb_addr == 0) {
			pal_log_err("can't find dtb\n");
			ret = -1;
			goto _end;
		}
		dtb_sz = (char *)(dtb_addr + 4);
		dtb_size = *(dtb_sz) * 0x1000000 + *(dtb_sz + 1) * 0x10000 +
			*(dtb_sz + 2) * 0x100 + *(dtb_sz + 3);
		g_64bit_dtb_size = dtb_size;
		pal_log_err("Kernel(%d) zimage_size:0x%x,dtb_addr:0x%x(dtb_size:0x%x)\n",
			g_is_64bit_kernel, zimage_size, dtb_addr, dtb_size);
	}

	if (dtb_size > DTB_MAX_SIZE) {
		pal_log_err("dtb_size too large: 0x%x\n", dtb_size);
		ret = -1;
		goto _end;
	}

#ifdef MTK_3LEVEL_PAGETABLE
	arch_mmu_map((uint64_t)ROUNDDOWN(dtb_kernel_addr, PAGE_SIZE),
		(uint32_t)ROUNDDOWN(dtb_kernel_addr, PAGE_SIZE),
		MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		ROUNDUP(dtb_size, PAGE_SIZE));
#endif
	pal_log_info("Copy DTB from 0x%x to 0x%x(size: 0x%x)\n", dtb_addr,
		dtb_kernel_addr, dtb_size);
	memcpy((void *)dtb_kernel_addr, (void *)dtb_addr, dtb_size);

	// Place setup_fdt() after bldr_load_dtb() because it sets "fdt_header->totalsize".
	ret = setup_fdt((void *)dtb_kernel_addr, DTB_MAX_SIZE);
	pal_log_err("[LK] fdt setup addr:0x%x status:%d!!!\n", dtb_kernel_addr,
		ret);
	if (ret == FALSE) {
		pal_log_err("setup_fdt fail, ret: 0x%x!\n", ret);
		ret = -1;
	}

	/* load odmdtbo and overlay */
	ret = dtb_overlay(g_fdt, DTB_MAX_SIZE, (uint64_t)recovery_dtbo_offset);
	if (ret == TRUE) {
		if (!strcmp(part_name, "recovery")) {
			set_recovery_dtbo_loaded();
			pal_log_err("dtb_overlay for recovery done !\n");
		}
	}
	/* command line buffer init */
	bootargs_init(g_fdt);
_end:
	free(ptr);
#endif

	return ret;
}


/**************************************************************************************
* Main device tree loading function, considering loading accordingly from different boot mode
***************************************************************************************/
void load_device_tree(void)
{
	PROFILING_START("early load dtb");

	char *part_name = "boot";

	/* If RECOVERY_AS_BOOT is enabled, there is no recovery partition.  */
#if !defined(NO_BOOT_MODE_SEL) && !defined(RECOVERY_AS_BOOT)
	if (Check_RTC_Recovery_Mode() || unshield_recovery_detection())
		part_name = "recovery";
	else
		part_name = "boot";
#endif

#if defined(CFG_DTB_EARLY_LOADER_SUPPORT)
	if (bldr_load_dtb(part_name) < 0)
		dprintf(CRITICAL, "Error: %s failed\n", __func__);
#endif

	PROFILING_END();
}

