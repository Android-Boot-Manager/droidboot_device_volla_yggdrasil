/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2016. All rights reserved.
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

#include <block_generic_interface.h>
#include <malloc.h>
#include <printf.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <video.h>
#include <dev/mrdump.h>
#ifdef MTK_PARTITION_COMMON
#include <env.h>
#else
#include <platform/env.h>
#endif
#include <platform/mtk_key.h>
#include <platform/mtk_wdt.h>
#include <platform/mt_gpt.h>
#include <target/cust_key.h>
#include <platform/boot_mode.h>
#include <bootimg.h>
#include <platform/ram_console.h>
#include <arch/ops.h>
#include <libfdt.h>
#include <platform.h>
#include <lib/zlib.h>
#include <part_interface.h>
#include "aee.h"
#include "kdump.h"

#ifdef MTK_3LEVEL_PAGETABLE
#include <target.h>
#endif

#define MRDUMP_DELAY_TIME 10

extern BOOT_ARGUMENT *g_boot_arg;
extern BOOTMODE g_boot_mode;

static struct mrdump_control_block *mrdump_cblock = NULL;
static struct mrdump_cblock_result cblock_result;
static unsigned int log_size;
static int output_device;

void voprintf(char type, const char *msg, va_list ap)
{
	char msgbuf[128], *p;

	p = msgbuf;
	if (msg[0] == '\r') {
		*p++ = msg[0];
		msg++;
	}

	*p++ = type;
	*p++ = ':';
	vsnprintf(p, sizeof(msgbuf) - (p - msgbuf), msg, ap);
	switch (type) {
		case 'I':
		case 'W':
		case 'E':
			video_printf("%s", msgbuf);
			break;
	}

	dprintf(CRITICAL,"[%s] %s", MRDUMP_GO_DUMP, msgbuf);

	/* Write log buffer */
	p = msgbuf;
	while ((*p != 0) && (log_size < sizeof(cblock_result.log_buf))) {
		cblock_result.log_buf[log_size] = *p++;
		log_size++;
	}
}

void voprintf_verbose(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	voprintf('V', msg, ap);
	va_end(ap);
}

void voprintf_debug(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	voprintf('D', msg, ap);
	va_end(ap);
}

void voprintf_info(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	voprintf('I', msg, ap);
	va_end(ap);
}

void voprintf_warning(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	voprintf('W', msg, ap);
	va_end(ap);
}

void voprintf_error(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	voprintf('E', msg, ap);
	va_end(ap);
}

void vo_show_progress(int sizeM)
{
	video_set_cursor((video_get_rows() / 4) * 3, (video_get_colums() - 22)/ 2);
	video_printf("=====================\n");
	video_set_cursor((video_get_rows() / 4) * 3 + 1, (video_get_colums() - 22)/ 2);
	video_printf(">>> Written %4dM <<<\n", sizeM);
	video_set_cursor((video_get_rows() / 4) * 3 + 2, (video_get_colums() - 22)/ 2);
	video_printf("=====================\n");
	video_set_cursor(video_get_rows() - 1, 0);

	dprintf(CRITICAL,"... Written %dM\n", sizeM);
}

static void mrdump_status(const char *status, const char *fmt, va_list ap)
{
	char *dest = cblock_result.status;
	dest += strlcpy(dest, status, sizeof(cblock_result.status));
	*dest++ = '\n';

	vsnprintf(dest, sizeof(cblock_result.status) - (dest - cblock_result.status), fmt, ap);
}

void mrdump_status_ok(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mrdump_status("OK", fmt, ap);
	va_end(ap);
}

void mrdump_status_none(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mrdump_status("NONE", fmt, ap);
	va_end(ap);
}

void mrdump_status_error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mrdump_status("FAILED", fmt, ap);
	va_end(ap);
}

uint32_t g_aee_mode = AEE_MODE_MTK_ENG;

const const char *mrdump_mode2string(uint8_t mode)
{
	switch (mode) {
		case AEE_REBOOT_MODE_NORMAL:
			return "NORMAL-BOOT";

		case AEE_REBOOT_MODE_KERNEL_OOPS:
			return "KERNEL-OOPS";

		case AEE_REBOOT_MODE_KERNEL_PANIC:
			return "KERNEL-PANIC";

		case AEE_REBOOT_MODE_NESTED_EXCEPTION:
			return "NESTED-CPU-EXCEPTION";

		case AEE_REBOOT_MODE_WDT:
			return "HWT";

		case AEE_REBOOT_MODE_EXCEPTION_KDUMP:
			return "MANUALDUMP";

		case AEE_REBOOT_MODE_MRDUMP_KEY:
			return "MRDUMP_KEY";

		case AEE_REBOOT_MODE_HANG_DETECT:
			return "KERNEL-HANG-DETECT";

		default:
			return "UNKNOWN-BOOT";
	}
}

#define MRDUMP_EXPDB_OFFSET 3145728

static void mrdump_write_result(void)
{
	dprintf(CRITICAL, "%s: Enter\n", __func__);

	int index = partition_get_index("expdb");
	part_dev_t *dev = mt_part_get_device();
	if (index == -1 || dev == NULL) {
		dprintf(CRITICAL, "%s: no %s partition[%d]\n", __func__, "expdb", index);
		return;
	}
#if defined(MTK_UFS_SUPPORT) || defined(MTK_NEW_COMBO_EMMC_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
	int part_id = partition_get_region(index);
#endif
	u64 part_size = partition_get_size(index);
	if (part_size < MRDUMP_EXPDB_OFFSET) {
		dprintf(CRITICAL, "%s: partition size(%llx) is less then reserved (%x)\n", __func__, part_size, MRDUMP_EXPDB_OFFSET);
		return;
	}
	u64 part_offset = partition_get_offset(index) + part_size - MRDUMP_EXPDB_OFFSET;

	dprintf(CRITICAL, "%s: offset %lld size %lld\n", __func__, part_offset, part_size);

#if (defined(MTK_UFS_SUPPORT) || defined(MTK_EMMC_SUPPORT))
#if (defined(MTK_UFS_SUPPORT) || defined(MTK_NEW_COMBO_EMMC_SUPPORT))
	dev->write(dev, (uchar *)&cblock_result, part_offset, sizeof(cblock_result), part_id);
#else
	dev->write(dev, (uchar *)&cblock_result, part_offset, sizeof(cblock_result));
#endif
#else
	dev->write(dev, (uchar *)&cblock_result, part_offset, sizeof(cblock_result), part_id);
#endif
}

#define SIZE_1MB 1048576ULL
#define SIZE_64MB 67108864ULL

static uint64_t mrdump_mem_size(void)
{
	uint64_t total_dump_size = physical_memory_size();
	const char *mem_size_param = get_env("mrdump_mem_size");
	if (mem_size_param != NULL) {
		uint64_t mem_size = atoi(mem_size_param) * SIZE_1MB;
		voprintf_info("Memory dump size set to %uM\n", (unsigned int) (mem_size / SIZE_1MB));
		if (mem_size >= SIZE_64MB) {
                        /* minimum 64m */
			total_dump_size = MIN(total_dump_size, mem_size);
		}
	}
	return total_dump_size;
}

static int mrdump_output_device(void)
{
	const char *output_device_param = get_env("mrdump_output");
	if (output_device_param != NULL) {
		if (strcmp(output_device_param, "none") == 0)
			return MRDUMP_DEV_NONE;
		if (strcmp(output_device_param, "null") == 0)
			return MRDUMP_DEV_NULL;
		if (strcmp(output_device_param, "internal-storage:ext4") == 0)
			return MRDUMP_DEV_ISTORAGE_EXT4;
		if (strcmp(output_device_param, "internal-storage:vfat") == 0)
			return MRDUMP_DEV_ISTORAGE_VFAT;
		if (strcmp(output_device_param, "usb") == 0)
			return MRDUMP_DEV_USB;
		return MRDUMP_DEV_UNKNOWN;
	}
#ifdef MRDUMP_DEFAULT_USB_DUMP
	return MRDUMP_DEV_USB;
#else
	return MRDUMP_DEV_ISTORAGE_EXT4;
#endif
}

static struct kzip_addlist *mrdump_memlist_fill(void)
{
	struct kzip_addlist *memlist = malloc(sizeof(struct kzip_addlist) * 4);
	if (memlist == NULL) {
		return NULL;
	}
	void *bufp = malloc(KDUMP_CORE_HEADER_SIZE);
	memset(bufp, 0, KDUMP_CORE_HEADER_SIZE);

	memlist[0].address = (uint64_t)(uintptr_t) bufp;
	memlist[0].size = KDUMP_CORE_HEADER_SIZE;
	memlist[0].type = MEM_NO_MAP;
	memlist[1].address = (uint64_t)(uintptr_t) mrdump_cb_addr();
	memlist[1].size = mrdump_cb_size();
	memlist[1].type = MEM_NO_MAP;
	memlist[2].address = DRAM_PHY_ADDR;
	memlist[2].size = mrdump_mem_size();
	memlist[2].type = MEM_DO_MAP;
	memlist[3].address = 0;
	memlist[3].size = 0;
	memlist[3].type = MEM_NO_MAP;
	return memlist;
}

static void mrdump_memlist_free(struct kzip_addlist *memlist)
{
	free((void *)(uintptr_t)memlist[0].address);
	free(memlist);
}

static void kdump_ui(struct mrdump_control_block *mrdump_cblock)
{
	video_clean_screen();
	video_set_cursor(0, 0);

	mrdump_status_error("Unknown error\n");
	voprintf_info("Kdump triggerd by '%s' (address:%x, size:%lluM)\n",
		      mrdump_mode2string(mrdump_cblock->crash_record.reboot_mode),
		      DRAM_PHY_ADDR, mrdump_mem_size() / 0x100000UL);

	/* check machdesc crc */
	uint32_t mcrc = crc32(0xffffffff, (const unsigned char*)&mrdump_cblock->machdesc,
			      sizeof(struct mrdump_machdesc)) ^ 0xffffffff;
	if (mcrc != mrdump_cblock->machdesc_crc) {
		voprintf_error("Control block machdesc field CRC error (%08x, %08x).\n",
			       mcrc, mrdump_cblock->machdesc_crc);
		return;
	}

	struct kzip_addlist *memlist = mrdump_memlist_fill();
	if (memlist == NULL) {
		voprintf_error("Cannot allcate memlist memory.\n");
		return;
	}
	kdump_core_header_init(mrdump_cblock, memlist);

	struct aee_timer elapse_time;
	aee_timer_init(&elapse_time);
	aee_timer_start(&elapse_time);
	int dump_ok;
	switch (output_device) {
		case  MRDUMP_DEV_NONE:
			mrdump_status_none("Output to None (disabled)\n");
			voprintf_info("Output to None (disabled)\n");
			dump_ok = 0;
			break;
		case MRDUMP_DEV_NULL:
			dump_ok = kdump_null_output(mrdump_cblock, memlist);
			break;
		case MRDUMP_DEV_ISTORAGE_EXT4:
			dump_ok = mrdump_ext4_output(mrdump_cblock, memlist, mrdump_dev_emmc_ext4());
			break;
		case MRDUMP_DEV_ISTORAGE_VFAT:
			dump_ok = mrdump_vfat_output(mrdump_cblock, memlist, mrdump_dev_emmc_vfat());
			break;
		case MRDUMP_DEV_USB:
			dump_ok = kdump_usb_output(mrdump_cblock, memlist);
			break;
		default:
			voprintf_error("Unsupport device id %d\n", output_device);
			dump_ok = -1;
	}
	mrdump_memlist_free(memlist);

	aee_mrdump_flush_cblock(mrdump_cblock);
	aee_timer_stop(&elapse_time);
	voprintf_info("Dump finished.(%s, %d sec)\n", dump_ok == 0 ? "ok" : "failed", elapse_time.acc_ms / 1000);

	mtk_wdt_restart();

	video_clean_screen();
	video_set_cursor(0, 0);
}

int mrdump_detection(void)
{
	if (!ram_console_is_abnormal_boot()) {
		dprintf(CRITICAL, "MT-RAMDUMP: No exception detected, skipped\n");
		return 0;
	}

	mrdump_cblock = aee_mrdump_get_params();
	if (mrdump_cblock == NULL) {
		dprintf(CRITICAL, "MT-RAMDUMP control block not found\n");
		return 0;
	}

	memset(&cblock_result, 0, sizeof(struct mrdump_cblock_result));
	log_size = 0;
	strlcpy(cblock_result.sig, MRDUMP_GO_DUMP, sizeof(cblock_result.sig));

	uint8_t reboot_mode = mrdump_cblock->crash_record.reboot_mode;

	if (!g_boot_arg->ddr_reserve_enable) {
		voprintf_debug("DDR reserve mode disabled\n");
		mrdump_status_none("DDR reserve mode disabled\n");
		goto error;
	}

	if (!g_boot_arg->ddr_reserve_success) {
		voprintf_debug("DDR reserve mode failed\n");
		mrdump_status_none("DDR reserve mode failed\n");
		goto error;
	}

	if (mrdump_cblock->enabled != MRDUMP_ENABLE_COOKIE) {
		voprintf_debug("Runtime disabled %x\n", mrdump_cblock->enabled);
		mrdump_status_none("Runtime disabled\n");
		goto error;
	}

	output_device = mrdump_output_device();
	voprintf_debug("sram record with mode %d\n", reboot_mode);
	switch (reboot_mode) {
		case AEE_REBOOT_MODE_GZ_WDT:
		case AEE_REBOOT_MODE_WDT: {
			goto end;
		}
		case AEE_REBOOT_MODE_NORMAL: {
			/* MRDUMP_KEY reboot*/
			if (ram_console_reboot_by_mrdump_key && ram_console_reboot_by_mrdump_key()) {
				mrdump_cblock->crash_record.reboot_mode = AEE_REBOOT_MODE_MRDUMP_KEY;
				goto end;
			} else
				return 0;
		}
		case AEE_REBOOT_MODE_KERNEL_OOPS:
		case AEE_REBOOT_MODE_KERNEL_PANIC:
		case AEE_REBOOT_MODE_NESTED_EXCEPTION:
		case AEE_REBOOT_MODE_EXCEPTION_KDUMP:
		case AEE_REBOOT_MODE_MRDUMP_KEY:
		case AEE_REBOOT_MODE_GZ_KE:
		case AEE_REBOOT_MODE_HANG_DETECT:
			goto end;
	}
	voprintf_debug("Unsupport exception type\n");
	mrdump_status_none("Unsupport exception type\n");

error:
	mrdump_write_result();
	return 0;
end:
	if (output_device == MRDUMP_DEV_USB) {
		g_boot_mode = FASTBOOT;
		set_env("mrdump_output", "usb");
	}

	return 1;
}

void mrdump_reboot(void)
{
#ifdef MTK_PMIC_FULL_RESET
	voprintf_debug("Ready for full pmic reset\n");
	mrdump_write_result();
	mtk_arch_full_reset();
#else
	voprintf_debug("Ready for reset\n");
	mrdump_write_result();
	mtk_arch_reset(1);
#endif
}

int mrdump_run2(void)
{
	if (mrdump_cblock != NULL) {
		kdump_ui(mrdump_cblock);
#ifndef MTK_TC7_FEATURE
		if (output_device != MRDUMP_DEV_USB) {
			mrdump_reboot();
		}
#endif
		mrdump_write_result();
		return 1;
	}
	return 0;
}

void aee_timer_init(struct aee_timer *t)
{
	memset(t, 0, sizeof(struct aee_timer));
}

void aee_timer_start(struct aee_timer *t)
{
	t->start_ms = get_timer_masked();
}

void aee_timer_stop(struct aee_timer *t)
{
	t->acc_ms += (get_timer_masked() - t->start_ms);
	t->start_ms = 0;
}

void kdump_core_header_init(const struct mrdump_control_block *kparams, const struct kzip_addlist *memlist)
{
	if (kparams->machdesc.page_offset <= 0xffffffffULL) {
		voprintf_info("32b kernel detected\n");
		kdump_core32_header_init(kparams, memlist);
	} else {
		voprintf_info("64b kernel detected\n");
		kdump_core64_header_init(kparams, memlist);
	}
}

#ifdef MTK_3LEVEL_PAGETABLE
vaddr_t scratch_addr(void)
{
	return (vaddr_t)target_get_scratch_address();
}
#endif

void mrdump_setup_version(void)
{
        cmdline_append("mrdump.lk=" MRDUMP_GO_DUMP);
}
