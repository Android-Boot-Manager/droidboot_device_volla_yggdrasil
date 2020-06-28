/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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

#include <string.h>

#include <pal_typedefs.h>
#include <pal_log.h>
#include <pal_assert.h>

#include <load_vfy_boot.h>
#include <bootimg.h>
#include <boot_opt.h>

#include <platform/boot_mode.h>
#include <part_interface.h>
#include <mkimg.h>

struct bootimg_hdr g_bootimg_hdr;
struct boot_info g_boot_info;
union mkimg_hdr g_mkimg_hdr;

char *get_bootimg_partition_name(uint32_t bootimg_type)
{
	char *result = NULL;

	switch (bootimg_type) {
	case BOOTIMG_TYPE_RECOVERY:
#ifndef RECOVERY_AS_BOOT
#ifdef MTK_GPT_SCHEME_SUPPORT
		result = "recovery";
#else
		result = "RECOVERY";
#endif
		break;
#endif
	case BOOTIMG_TYPE_BOOT:
#ifdef MTK_GPT_SCHEME_SUPPORT
		result = "boot";
#else
		result = "BOOTIMG";
#endif
		break;
	default:
		break;
	}

	return result;
}

uint32_t bootimg_hdr_valid(uint8_t *buf)
{
	if (strncmp((char *)buf, BOOTIMG_MAGIC, BOOTIMG_MAGIC_SZ) == 0)
		return 1;
	else
		return 0;
}

uint32_t mkimg_hdr_valid(uint8_t *buf)
{
	union mkimg_hdr *l_mkimg_hdr = (union mkimg_hdr *)buf;

	if (l_mkimg_hdr->info.magic == MKIMG_MAGIC)
		return 1;
	else
		return 0;
}

uint32_t load_bootimg_hdr(uint32_t bootimg_type)
{
	uint32_t ret = 0;
	char *part_name = NULL;
	unsigned long len;

	/* boot image header could be loaded only once */
	if (g_boot_info.hdr_loaded)
		return 0;

	part_name = get_bootimg_partition_name(bootimg_type);
	if (part_name == NULL) {
		pal_log_err("unknown boot image type: 0x%x\n", bootimg_type);
		return -1;
	}

	len = partition_read(part_name,
				(off_t)0,
				(uint8_t *)&g_bootimg_hdr,
				sizeof(g_bootimg_hdr));
	if (len != sizeof(g_bootimg_hdr)) {
		pal_log_err("boot image load fail: 0x%lu\n", len);
		return -1;
	}

	len = partition_read(part_name,
				(off_t)BOOTIMG_HDR_SZ,
				(uint8_t *)&g_mkimg_hdr,
				sizeof(g_mkimg_hdr.info));
	if (len != sizeof(g_mkimg_hdr.info)) {
		pal_log_err("mkimg header load fail: 0x%lu\n", len);
		return -1;
	}

	if (FALSE == bootimg_hdr_valid((uint8_t *)&g_bootimg_hdr)) {
		pal_log_err("invalid boot image header\n");
		return -1;
	}

	if (mkimg_hdr_valid((uint8_t *)&g_mkimg_hdr))
		g_boot_info.kernel_with_mkimg_hdr = 1;

	if (0 == ret) {
		g_boot_info.hdr_loaded = 1;
		g_boot_info.type = bootimg_type;
	}

	return ret;
}

uint32_t get_recovery_dtbo_sz(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	return (uint32_t)g_bootimg_hdr.recovery_dtbo_size;
}

uint64_t get_recovery_dtbo_offset(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	return (uint64_t)g_bootimg_hdr.recovery_dtbo_offset;
}

uint32_t get_page_sz(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	return (uint32_t)g_bootimg_hdr.page_sz;
}

/* get final kernel image location (after relocation) */
uint32_t get_kernel_target_addr(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	return (uint32_t)g_bootimg_hdr.kernel_addr;
}

/* get kernel image address when it's loaded */
uint32_t get_kernel_addr(void)
{
	uint32_t kernel_addr = 0;

	PAL_ASSERT(g_boot_info.hdr_loaded);
	PAL_ASSERT(g_boot_info.img_loaded);

	kernel_addr = g_boot_info.load_addr;

	return (uint32_t)kernel_addr;
}

int32_t get_kernel_sz(void)
{
	uint32_t page_sz = 0;
	uint32_t kernel_sz = 0;
	int32_t out_kernel_sz = 0;

	PAL_ASSERT(g_boot_info.hdr_loaded);

	kernel_sz = g_bootimg_hdr.kernel_sz;
	page_sz = g_bootimg_hdr.page_sz;

	out_kernel_sz = (int32_t)(((kernel_sz + page_sz - 1) / page_sz) *
				page_sz);
	return out_kernel_sz;
}

uint32_t get_kernel_real_sz(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);
	return g_bootimg_hdr.kernel_sz;
}

/* get final ramdisk image location (after relocation) */
uint32_t get_ramdisk_target_addr(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	return (uint32_t)g_bootimg_hdr.ramdisk_addr;
}

/* get ramdisk image address when it's loaded */
uint32_t get_ramdisk_addr(void)
{
	int32_t kernel_sz = 0;
	uint32_t ramdisk_addr = 0;

	PAL_ASSERT(g_boot_info.hdr_loaded);
	PAL_ASSERT(g_boot_info.img_loaded);

	kernel_sz = get_kernel_sz();
	if (kernel_sz == 0)
		return 0;

	ramdisk_addr = g_boot_info.load_addr + (uint32_t)kernel_sz;

	if (g_boot_info.kernel_with_mkimg_hdr)
		ramdisk_addr -= MKIMG_HDR_SZ;

	return (uint32_t)ramdisk_addr;
}

int32_t get_ramdisk_sz(void)
{
	uint32_t page_sz = 0;
	uint32_t ramdisk_sz = 0;
	int32_t out_ramdisk_sz = 0;

	PAL_ASSERT(g_boot_info.hdr_loaded);

	ramdisk_sz = g_bootimg_hdr.ramdisk_sz;
	page_sz = g_bootimg_hdr.page_sz;

	out_ramdisk_sz = (int32_t)(((ramdisk_sz + page_sz - 1) / page_sz) *
				page_sz);

	return out_ramdisk_sz;
}

uint32_t get_ramdisk_real_sz(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);
	return (uint32_t)g_bootimg_hdr.ramdisk_sz;
}

uint32_t get_bootimg_sz(void)
{
	uint32_t page_sz = 0;
	uint32_t kernel_sz = 0;
	uint32_t ramdisk_sz = 0;

	PAL_ASSERT(g_boot_info.hdr_loaded);

	page_sz = (uint32_t)g_bootimg_hdr.page_sz;
	kernel_sz = (uint32_t)g_bootimg_hdr.kernel_sz;
	kernel_sz = (uint32_t)(((kernel_sz + page_sz - 1) / page_sz) *
				page_sz);
	ramdisk_sz = (uint32_t)g_bootimg_hdr.ramdisk_sz;
	ramdisk_sz = (uint32_t)(((ramdisk_sz + page_sz - 1) / page_sz) *
				page_sz);

	return page_sz + kernel_sz + ramdisk_sz;
}

uint32_t get_tags_addr(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);
	return (uint32_t)g_bootimg_hdr.tags_addr;
}

char *get_cmdline(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	/* ensure commandline is terminated */
	g_bootimg_hdr.cmdline[BOOTIMG_ARGS_SZ - 1] = 0;

	return (char *)g_bootimg_hdr.cmdline;
}

uint32_t get_os_version(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);
	return (uint32_t)g_bootimg_hdr.os_version;
}

uint32_t get_header_version(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);
	return (uint32_t)g_bootimg_hdr.header_version;
}

uint32_t get_header_size(void)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);
	return (uint32_t)g_bootimg_hdr.header_size;
}

void set_bootimg_loaded(uint32_t addr)
{
	PAL_ASSERT(g_boot_info.hdr_loaded);

	g_boot_info.img_loaded = 1;
	g_boot_info.load_addr = addr;
	return;
}

void set_recovery_dtbo_loaded(void)
{
	g_boot_info.recovery_dtbo_loaded = 1;
	return;
}

uint32_t get_recovery_dtbo_loaded(void)
{
	return (uint32_t)g_boot_info.recovery_dtbo_loaded;
}

void set_bootimg_verified(void)
{
	g_boot_info.verified = 1;
	return;
}

void set_bootimg_verify_skipped(void)
{
	g_boot_info.vfy_skipped = 1;
	return;
}

