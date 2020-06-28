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

#include <atags.h>              // for target_atag_masp_data()
#include <app.h>
#include <debug.h>
#include <arch.h>
#include <arch/arm.h>
#include <arch/arm/mmu.h>
#include <dev/udc.h>
#include <reg.h>
#include <string.h>
#include <stdlib.h>
#include <kernel/thread.h>
#include <arch/ops.h>
#include <ctype.h>
#include <target.h>
#include <platform.h>
#include <platform/mt_reg_base.h>
#include <platform/boot_mode.h>
#include <load_vfy_boot.h>
#include <boot_opt.h>
#include <bootimg.h>
#ifdef MTK_GPT_SCHEME_SUPPORT
#include <platform/partition.h>
#else
#include <mt_partition.h>
#endif
#include <platform/mt_disp_drv.h>
#ifdef MTK_PARTITION_COMMON
#include <env.h>
#else
#include <platform/env.h>
#endif
#include <target/cust_usb.h>
#include <platform/mt_gpt.h>
#if defined(MTK_SECURITY_SW_SUPPORT)
#include "oemkey.h"
#endif
#include <video.h>              // for video_printf()
#include <platform/mtk_wdt.h>   // for mtk_wdt_disable()
#include <mt_boot.h>
#include <libfdt.h>
#include <ufdt_overlay.h>       // for ufdt_install_blob(), ufdt_apply_overlay()
#include <mt_rtc.h>             // for Check_RTC_Recovery_Mode()
#include <part_interface.h>     // for partition_read()
#include <dev/mrdump.h>         // for mrdump_append_cmdline()
#include <iothread.h>
#include <RoT.h>
#include <odm_mdtbo.h>          // for load_overlay_dtbo()

#include <profiling.h>
#include <rnd.h>
#include <verified_boot_common.h>
#include <ram_console_common.h>

#include <lk_load_md_wrapper.h> // for ccci load modem image relate
#include <pal_assert.h>
#include <pal_typedefs.h>
#include <pal_log.h>
#include <memory_layout.h>
#ifdef MTK_RECOVERY_RAMDISK_SPLIT
#include <load_vfy_ramdisk.h>
#endif
#include <bootargs.h>

#ifndef LK_RAMDISK_MAX_SIZE
#define LK_RAMDISK_MAX_SIZE		(16*1024*1024)
#endif /* LK_RAMDISK_MAX_SIZE */

#define TMPBUF_SIZE        200
//#define MTK_DEBUG_SHELL


#ifdef MBLOCK_LIB_SUPPORT
#include <mblock.h>
#endif

#ifdef MTK_AB_OTA_UPDATER
#include "bootctrl.h"
#endif

#include <dev/uart.h>
#include <sec_hrid.h>
#include <sec_devinfo.h>

extern u32 current_lk_buf_addr_get(void) __attribute__((weak));
extern u32 current_buf_addr_get(void) __attribute__((weak));
extern u32 current_buf_pl_lk_log_size_get(void) __attribute__((weak));
enum {
	RUNTIME_LOG_DEACTIVATED = 0,
	RUNTIME_LOG_ACTIVATED
};
u8 g_lk_final_log = RUNTIME_LOG_ACTIVATED;

// FIXME!!! The following function declaration should not exist, and appropriate
//          header files instead should be included.
void write_protect_flow(void) __attribute__((weak));
int mboot_recovery_load_raw_part(char *part_name, unsigned long *addr,
				 unsigned int size);
#ifdef PRIZE_CAMERA_HALL_CALI
/*prize  add  for hall  calibration by zhuzhengjiang    20190529-begin*/
int mboot_recovery_load_raw_part_offset(char *part_name, uint32_t *addr,
					uint32_t offset, uint32_t size);
/*prize  add  for hall  calibration by zhuzhengjiang    20190529-end*/
#endif
extern int kernel_charging_boot(void) __attribute__((weak));
extern int pmic_detect_powerkey(void);
extern void mt65xx_backlight_off(void);

extern void jumparch64_smc(u32 addr, u32 arg1, u32 arg2, u32 arg3);

extern u32 memory_size(void);
extern unsigned *target_atag_devinfo_data(unsigned *ptr);
extern unsigned *target_atag_videolfb(unsigned *ptr, size_t buf_size);
extern unsigned *target_atag_mdinfo(unsigned *ptr);
extern unsigned *target_atag_ptp(unsigned *ptr);
extern void platform_uninit(void);
extern void custom_port_in_kernel(BOOTMODE boot_mode, char *command);
extern const char *mt_disp_get_lcm_id(void);
extern unsigned int DISP_GetVRamSize(void);
extern int mt_disp_is_lcm_connected(void);
extern int fastboot_init(void *base, unsigned size);
extern int sec_boot_check(int try_lock);
extern int seclib_set_oemkey(u8 *key, u32 key_size);
extern BI_DRAM bi_dram[MAX_NR_BANK];
#ifdef DEVICE_TREE_SUPPORT
#include <libfdt.h>
extern unsigned int *device_tree, device_tree_size;
#endif
extern unsigned int g_boot_state;
extern int platform_skip_hibernation(void) __attribute__((weak));
extern int is_meta_log_disable(void)__attribute__((weak));

extern int g_is_64bit_kernel;
u32 g_64bit_dtb_size = 0;

#if defined(MBLOCK_LIB_SUPPORT) && defined(MTK_3LEVEL_PAGETABLE)
#define ALLOCATE_FROM_MBLOCK
#endif

#ifdef ALLOCATE_FROM_MBLOCK
/* Occupy dtb/kernel/randisk from mblock */
u32 dtb_kernel_addr_mb = 0;
u32 kernel_addr_mb = 0;
u32 ramdisk_addr_mb = 0;
u32 kernel_sz_mb = 0;
u32 ramdisk_sz_mb = 0;
u32 lk_addr_mb = 0;
u32 scratch_addr_mb = 0;
#endif

/* do not use this variable any more, it exists only to make
 * all platforms built successfully.
 */
struct bootimg_hdr *g_boot_hdr = NULL;

char g_boot_reason[][16] = {"power_key", "usb", "rtc", "wdt", "wdt_by_pass_pwk", "tool_by_pass_pwk", "2sec_reboot", "unknown", "kernel_panic", "reboot", "watchdog"};
char g_reboot_reason[][20] = {"reboot", "watchdog", "kernel_panic", "oemerr_2sec_reboot"};
#if defined(MTK_SECURITY_SW_SUPPORT)
u8 g_oemkey[OEM_PUBK_SZ] = {OEM_PUBK};
#endif

/* battery driver related */
signed int fg_swocv_v;
signed int fg_swocv_i;
int shutdown_time;
int boot_voltage;
int two_sec_reboot;

#ifdef MTK_AB_OTA_UPDATER
char *p_AB_suffix;
static uint8_t AB_retry_count;
#endif /* MTK_AB_OTA_UPDATER */

/* Please define SN_BUF_LEN in cust_usb.h */
#ifndef SN_BUF_LEN
#define SN_BUF_LEN  19  /* fastboot use 13 bytes as default, max is 19 */
#endif

/*PRIZE-uique serial-tangan-20170920-end*/
#define PRIZE_SN_BUF_LEN  20
/*PRIZE-uique serial-tangan-20170920-end*/
#define FDT_BUFF_SIZE		(2048) //must align with CMDLINE_LEN
#define FDT_CHECKER_SIZE	(8)
#define FDT_SPARE_SIZE		(FDT_BUFF_SIZE + FDT_CHECKER_SIZE)
#define FDT_BUFF_END	 	"BUFFEND"

#define DEFAULT_SERIAL_NUM "0123456789ABCDEF"

#define KERNEL_64BITS 1
#define KERNEL_32BITS 0

#define VIDEOLFB_PRE_HEADER_LENGTH  (5)

/* define meta init.rc path */
#if defined (MTK_RC_TO_VENDOR)
#define META_INIT_RC        "/vendor/etc/init/hw/meta_init.rc"
#define FACTORY_INIT_RC     "/vendor/etc/init/hw/factory_init.rc"
#else
#define META_INIT_RC        "/meta_init.rc"
#define FACTORY_INIT_RC     "/factory_init.rc"
#endif

/*
 * Support read barcode from /dev/pro_info to be serial number.
 * Then pass the serial number from cmdline to kernel.
 */
/* The following option should be defined in project make file. */
/* #define SERIAL_NUM_FROM_BARCODE */

#if defined(CONFIG_MTK_USB_UNIQUE_SERIAL) || (defined(MTK_SECURITY_SW_SUPPORT) && defined(MTK_SEC_FASTBOOT_UNLOCK_SUPPORT))
#define SERIALNO_LEN    38  /* from preloader */
char sn_buf[SN_BUF_LEN + 1] = ""; /* will read from EFUSE_CTR_BASE */
#else
#define SERIALNO_LEN    38
char sn_buf[SN_BUF_LEN + 1] = FASTBOOT_DEVNAME;
#endif

static struct udc_device surf_udc_device = {
	.vendor_id  = USB_VENDORID,
	.product_id = USB_PRODUCTID,
	.version_id = USB_VERSIONID,
	.manufacturer   = USB_MANUFACTURER,
	.product    = USB_PRODUCT_NAME,
};

typedef enum BUILD_TYPE {
	BUILD_TYPE_USER         = 0,
	BUILD_TYPE_USERDEBUG    = 1,
	BUILD_TYPE_ENG          = 2
} BUILD_TYPE_T;

enum {
	REBOOT_REASON_REBOOT = 0,
	REBOOT_REASON_WATCHDOG,
	REBOOT_REASON_KERNEL_PANIC,
	REBOOT_REASON_2S_REBOOT
};

/****************************************************************************
* Note that userdebug build defines both USERDEBUG_BUILD and USER_BUILD for
* backward compatibility for now.  Therefore, it is important to check
* USERDEBUG_BUILD before checking USER_BUILD.
****************************************************************************/
#ifdef USERDEBUG_BUILD
static BUILD_TYPE_T eBuildType = BUILD_TYPE_USERDEBUG;
#elif defined(USER_BUILD)
static BUILD_TYPE_T eBuildType = BUILD_TYPE_USER;
#elif defined(ENG_BUILD)
static BUILD_TYPE_T eBuildType = BUILD_TYPE_ENG;
#else
static BUILD_TYPE_T eBuildType = BUILD_TYPE_USER;
#endif

void msg_header_error(char *img_name)
{
	pal_log_err("[MBOOT] Load '%s' partition Error\n", img_name);
	pal_log_err(
		"\n*******************************************************\n");
	pal_log_err("ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR\n");
	pal_log_err("*******************************************************\n");
	pal_log_err("> If you use NAND boot\n");
	pal_log_err("> (1) %s is wrong !!!! \n", img_name);
	pal_log_err(
		"> (2) please make sure the image you've downloaded is correct\n");
	pal_log_err("\n> If you use MSDC boot\n");
	pal_log_err("> (1) %s is not founded in SD card !!!! \n", img_name);
	pal_log_err("> (2) please make sure the image is put in SD card\n");
	pal_log_err("*******************************************************\n");
	pal_log_err("ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR\n");
	pal_log_err("*******************************************************\n");
	mtk_wdt_disable();
	mdelay(8000);
	mtk_arch_reset(1);
}

void msg_img_error(char *img_name)
{
	pal_log_err("[MBOOT] Load '%s' partition Error\n", img_name);
	pal_log_err(
		"\n*******************************************************\n");
	pal_log_err("ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR\n");
	pal_log_err("*******************************************************\n");
	pal_log_err("> Please check kernel and rootfs in %s are both correct.\n",
		img_name);
	pal_log_err("*******************************************************\n");
	pal_log_err("ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR.ERROR\n");
	pal_log_err("*******************************************************\n");
	mtk_wdt_disable();
	mdelay(8000);
	mtk_arch_reset(1);
}

//*********
//* Notice : it's kernel start addr (and not include any debug header)
extern unsigned int g_kmem_off;

//*********
//* Notice : it's rootfs start addr (and not include any debug header)
extern int g_nr_bank;
extern unsigned int boot_time;
extern BOOT_ARGUMENT *g_boot_arg;
extern bool g_boot_reason_change __attribute__((weak));
extern int has_set_p2u;
extern unsigned int g_fb_base;
extern unsigned int g_fb_size;
unsigned int logo_lk_t = 0;
bool boot_ftrace = false;

static void mboot_free_lk_scratch_from_mblock()
{
#ifdef ALLOCATE_FROM_MBLOCK
	/* Free before stepping into kernel */
	if (lk_addr_mb != 0) {
		mblock_create(&g_boot_arg->mblock_info,
			&g_boot_arg->orig_dram_info,
			(u64)lk_addr_mb, (u64)MEMSIZE);
	}

	if (scratch_addr_mb != 0) {
		mblock_create(&g_boot_arg->mblock_info,
			&g_boot_arg->orig_dram_info,
			(u64)scratch_addr_mb, (u64)(SCRATCH_SIZE + 16*MB));
	}
#endif
}

void mboot_allocate_lk_scratch_from_mblock()
{
#ifdef ALLOCATE_FROM_MBLOCK
	/* never allocate mb more than once */
	if ((lk_addr_mb == MEMBASE)||(scratch_addr_mb == SCRATCH_ADDR))
		return;

	// Claim the LK MEMBASE from mblock during its life time
	lk_addr_mb = (u32)mblock_reserve_ext(&g_boot_arg->mblock_info,
			MEMSIZE, SECTION_SIZE, (MEMBASE + MEMSIZE),
			0, "lk_addr_mb");

	// Claim the LK SCRATCH from mblock during its life time (including logo)
	scratch_addr_mb = (u32)mblock_reserve_ext(&g_boot_arg->mblock_info,
			(SCRATCH_SIZE + 16*MB), PAGE_SIZE, (SCRATCH_ADDR + SCRATCH_SIZE + 16*MB),
			0, "scratch_addr_mb");

	/* Check if the clarmed mb address the same as predefined one */
	if ((!lk_addr_mb) || (lk_addr_mb != MEMBASE)) {
		pal_log_err("Warning! MEMBASE (0x%x) is not taken from mb (0x%x)\n", MEMBASE, lk_addr_mb);
		assert(0);
	}
	if ((!scratch_addr_mb) || (scratch_addr_mb != SCRATCH_ADDR)) {
		pal_log_err("Warning! SCRATCH (0x%x) is not taken from mb (0x%x)\n", SCRATCH_ADDR, scratch_addr_mb);
		assert(0);
	}
#endif
}

static void mboot_free_bootimg_from_mblock()
{
#ifdef ALLOCATE_FROM_MBLOCK
	/* Return the bootimg mb before stepping into kernel */
	if (dtb_kernel_addr_mb != 0) {
		mblock_create(&g_boot_arg->mblock_info,
			&g_boot_arg->orig_dram_info,
			(u64)dtb_kernel_addr_mb, (u64)DTB_MAX_SIZE);
	}

	if (kernel_addr_mb != 0) {
		mblock_create(&g_boot_arg->mblock_info,
			&g_boot_arg->orig_dram_info,
			(u64)kernel_addr_mb, (u64)kernel_sz_mb);
	}

	if (ramdisk_addr_mb != 0) {
		mblock_create(&g_boot_arg->mblock_info,
			&g_boot_arg->orig_dram_info,
			(u64)ramdisk_addr_mb, (u64)ramdisk_sz_mb);
	}
#endif
}


void mboot_allocate_bootimg_from_mblock(struct bootimg_hdr *p_boot_hdr)
{
#ifdef ALLOCATE_FROM_MBLOCK

	int skip_ramdisk_check = 0;

	/* never allocate mb more than once */
	if ((kernel_addr_mb == p_boot_hdr->kernel_addr)||
	(ramdisk_addr_mb == p_boot_hdr->ramdisk_addr)||
	(dtb_kernel_addr_mb == p_boot_hdr->tags_addr))
		return;

	#ifdef KERNEL_DECOMPRESS_SIZE
		kernel_sz_mb = KERNEL_DECOMPRESS_SIZE;
	#else
		kernel_sz_mb = 0x03200000;
	#endif
	ramdisk_sz_mb = ROUNDUP(LK_RAMDISK_MAX_SIZE, PAGE_SIZE);
	if (ramdisk_sz_mb == 0)
		skip_ramdisk_check = 1;

	/* To avoid dtb being corrupted, use mblock to claim it now */
	dtb_kernel_addr_mb = (u32)mblock_reserve_ext(&g_boot_arg->mblock_info,
		DTB_MAX_SIZE, DTB_MAX_SIZE, (p_boot_hdr->tags_addr+DTB_MAX_SIZE),
		0, "dtb_kernel_addr_mb");

	kernel_addr_mb = (u32)mblock_reserve_ext(&g_boot_arg->mblock_info,
		kernel_sz_mb, PAGE_SIZE,(p_boot_hdr->kernel_addr+kernel_sz_mb),
		0, "kernel_addr_mb");

	if (!skip_ramdisk_check) {
		ramdisk_addr_mb = (u32)mblock_reserve_ext(&g_boot_arg->mblock_info,
			ramdisk_sz_mb, PAGE_SIZE,(p_boot_hdr->ramdisk_addr+ramdisk_sz_mb),
			0, "ramdisk_addr_mb");
	}

	/* Check if the clarmed mb address the same as predefined one */
	if ((!dtb_kernel_addr_mb) || (dtb_kernel_addr_mb!=p_boot_hdr->tags_addr)) {
		pal_log_err("Warning! dtb_kernel_addr (0x%x) is not taken from mb (0x%x)\n", p_boot_hdr->tags_addr, dtb_kernel_addr_mb);
		assert(0);
	}

	if ((!kernel_addr_mb) || (kernel_addr_mb!=p_boot_hdr->kernel_addr)) {
		pal_log_err("Warning! kernel_addr (0x%x) is not taken from mb (0x%x)\n", p_boot_hdr->kernel_addr, kernel_addr_mb);
		assert(0);
	}

	if (!skip_ramdisk_check) {
		if ((!ramdisk_addr_mb) || (ramdisk_addr_mb!=p_boot_hdr->ramdisk_addr)) {
			pal_log_err("Warning! ramdisk_addr (0x%x) is not taken from mb (0x%x)\n", p_boot_hdr->ramdisk_addr, ramdisk_addr_mb);
			assert(0);
		}
	}

#ifndef DUMMY_MEMORY_LAYOUT
	/* Use memory_layout.h to check if overlap */
	if ((dtb_kernel_addr_mb!=LK_DT_BASE)) {
		pal_log_err("Warning! compare memory_layout.h with bootimg about DT\n");
		assert(0);
	}

	if (!skip_ramdisk_check) {
		if ((ramdisk_addr_mb!=LK_RAMDISK_BASE)||(ramdisk_sz_mb > LK_RAMDISK_MAX_SIZE)) {
			pal_log_err("Warning! compare memory_layout.h with bootimg about ramdisk\n");
			assert(0);
		}
	}
#endif

#endif
}


static void check_hibernation()
{
	int hibboot = 0;
	char tmpbuf[TMPBUF_SIZE];

	hibboot = get_env("hibboot") == NULL ? 0 : atoi(get_env("hibboot"));

	switch (g_boot_mode) {
	case RECOVERY_BOOT:
	case FACTORY_BOOT:
	case ALARM_BOOT:
#if defined(MTK_KERNEL_POWER_OFF_CHARGING) || defined(MTK_CHARGER_NEW_ARCH)
	case KERNEL_POWER_OFF_CHARGING_BOOT:
	case LOW_POWER_OFF_CHARGING_BOOT:
#endif
		goto SKIP_HIB_BOOT;

	default:
		break;
	}

	if (platform_skip_hibernation && platform_skip_hibernation())
		goto SKIP_HIB_BOOT;

	if (get_env("resume") != NULL) {
		if (1 == hibboot) {
			snprintf(tmpbuf, TMPBUF_SIZE, "%s%s", " resume=", get_env("resume"));
			cmdline_append(tmpbuf);
#if defined(MTK_MLC_NAND_SUPPORT)
			snprintf(tmpbuf, TMPBUF_SIZE, "%s%s", " ubi.mtd=", get_env("ubi_data_mtd"));
			cmdline_append(tmpbuf);
#endif
			//cmdline_append(" no_console_suspend");
		} else if (0 != hibboot)
			pal_log_err("resume = %s but hibboot = %s\n", get_env("resume"),
				get_env("hibboot"));
	} else
		pal_log_err("resume = NULL \n");

	return;

SKIP_HIB_BOOT:
	if (hibboot != 0)
		if (set_env("hibboot", "0") != 0)
			pal_log_err("lk_env hibboot set failed!!!\n");
	if (get_env("resume") != NULL)
		if (set_env("resume", '\0') != 0)
			pal_log_err("lk_evn resume set resume failed!!!\n");
}





#ifdef DEVICE_TREE_SUPPORT

void lk_jump64(u32 addr, u32 arg1, u32 arg2, u32 arg3)
{
	pal_log_err("\n[LK]jump to K64 0x%x\n", addr);
	pal_log_info("smc jump\n");
	jumparch64_smc(addr, arg1, arg2, arg3);
	pal_log_err("Do nothing now! wait for SMC done\n");
	while (1)
		;
}

void memcpy_u8(unsigned char *dest, unsigned char *src, unsigned int size)
{
	unsigned int i;

	for (i = 0; i < size; i++)
		*(dest + i) = *(src + i);
}

extern bool decompress_kernel(unsigned char *in, void *out, int inlen,
			      int outlen);

unsigned int get_reboot_reason(unsigned int boot_reason)
{
	unsigned int rc_wdt_status;
	unsigned int rc_exp_type;
	unsigned int ret = 0;

	if (boot_reason == 4) {
		if (ram_console_get_wdt_status(&rc_wdt_status) && ram_console_get_exp_type(&rc_exp_type)) {
			pal_log_err("rc_wdt_status = %d, rc_exp_type = %d\n", rc_wdt_status, rc_exp_type);

			switch (rc_exp_type) {
				case 0:
					if (rc_wdt_status == 5)
						ret = REBOOT_REASON_WATCHDOG;
					break;

				case AEE_EXP_TYPE_HWT:
					ret = REBOOT_REASON_WATCHDOG;
					break;

				case AEE_EXP_TYPE_KE:
				case AEE_EXP_TYPE_NESTED_PANIC:
					ret = REBOOT_REASON_KERNEL_PANIC;
					break;

				default:
					break;

			}
		}
	} else if (boot_reason == 6) {
		ret = REBOOT_REASON_2S_REBOOT;
	} else {
		ret = REBOOT_REASON_REBOOT;
	}

	return ret;
}

#ifdef PRIZE_CAMERA_HALL_CALI
/*prize  add  for hall  calibration by zhuzhengjiang    20190221-begin*/
#define CMDLINE_HALL_CALIDATA_SIZE 30
int mboot_common_get_hall_calidata(char *buf)
{	
	int calidata_len =637 ; // hall calidata addr 632 ,length :6 
//	int i =0;
	int ret = 0 ;
	ret = mboot_recovery_load_raw_part("proinfo", buf, calidata_len);
	//for(i=341;i<347;i++){
		//printf("zhuzhengjiang zhuanhuan buf[%d]=%d \n",i,hall_calidata_buf[i]-'0');
	//}
	return ret;
}
/*prize  add  for hall  calibration by zhuzhengjiang    20190221-end*/
#endif

int boot_linux_fdt(void *kernel, unsigned *tags,
		   unsigned machtype,
		   void *ramdisk, unsigned ramdisk_sz)
{
	void *fdt = tags;
	int ret;
	int offset;
	char tmpbuf[TMPBUF_SIZE];
	dt_dram_info mem_reg_property[128];
#ifdef PRIZE_CAMERA_HALL_CALI	
	char cmdline_tmpbuf[CMDLINE_HALL_CALIDATA_SIZE];///*prize  add  for hall  calibration by zhuzhengjiang    20190221-begin*/
#endif
	int i;
	void (*entry)(unsigned, unsigned, unsigned *) = kernel;
	unsigned int lk_t = 0;
	unsigned int pl_t = 0;
	unsigned int boot_reason = 0;
	unsigned int reboot_reason = 0;
	char *ptr;
	char spare[FDT_SPARE_SIZE],	/* SPARE_SIZE = BUFF_SIZE + CHCKER_SIZE */
	     *buf 	= spare,
		*checker 	= (spare + FDT_BUFF_SIZE);

	int decompress_outbuf_size = 0x1C00000;
	unsigned int zimage_addr = 0;
	unsigned int zimage_size = 0;
	unsigned int dtb_addr = 0;
	unsigned int dtb_size;
	u32 seed[2];
	const void *seedp;
	int seed_len;

	if (g_is_64bit_kernel) {
		uint32_t kernel_target_addr = 0;

		zimage_addr = (unsigned int)target_get_scratch_address();
		zimage_size = get_kernel_real_sz();
		kernel_target_addr = get_kernel_target_addr();

		pal_log_info("64 bits kernel\n");
		pal_log_info("kernel_sz=0x%08x\n", zimage_size);

		if (kernel_target_addr & 0x7FFFF) {
			pal_log_err("64 bit kernel can't boot at 0x%08x\n",
					kernel_target_addr);
			while (1);
		}

		dtb_size = (g_64bit_dtb_size + 0x3) & (~0x3);
		dtb_addr = (unsigned int)fdt;
		zimage_size -= dtb_size;

		pal_log_info("zimage_addr=0x%08x, zimage_size=0x%08x\n",
				zimage_addr, zimage_size);
		pal_log_info("decompress kernel image...\n");

		/* for 64bit decompreesed size.
		 * LK start: 0x41E00000, Kernel Start: 0x40080000
		 * Max is 0x41E00000 - 0x40080000 = 0x1D80000.
		 * using 0x1C00000=28MB for decompressed kernel image size */
#ifdef KERNEL_DECOMPRESS_SIZE
		decompress_outbuf_size = KERNEL_DECOMPRESS_SIZE;
#endif
		if (decompress_kernel((unsigned char *)zimage_addr,
				      (void *)kernel_target_addr, (int)zimage_size,
				      (int)decompress_outbuf_size)) {
			pal_log_err("decompress kernel image fail!!!\n");
			while (1);
		}
	} else {
		pal_log_info("32 bits kernel\n");
		zimage_size = *(uint32_t *)((uint32_t)kernel + 0x2c) - *
			      (uint32_t *)((uint32_t)kernel + 0x28);
		dtb_addr = (uint32_t)kernel + zimage_size;
		wake_up_iothread();
		wait_for_iothread();
	}

	if (fdt32_to_cpu(*(unsigned int *)dtb_addr) == FDT_MAGIC) {
#if CFG_DTB_EARLY_LOADER_SUPPORT
		dtb_size = fdt32_to_cpu(*(unsigned int *)(fdt + 0x4));
#else
		dtb_size = fdt32_to_cpu(*(unsigned int *)(dtb_addr + 0x4));
#endif
	} else {
		pal_log_err("Can't find device tree. Please check your kernel image\n");
		while (1);
	}
	pal_log_info("dtb_addr = 0x%08X, dtb_size = 0x%08X\n", dtb_addr, dtb_size);

#if CFG_DTB_EARLY_LOADER_SUPPORT
	//skip dtb copy, we load dtb in preloader
#else
	memcpy(fdt, (void *)dtb_addr, dtb_size);

	ret = setup_fdt(fdt,
			DTB_MAX_SIZE);	//MIN(0x100000, (g_fb_base-(unsigned int)fdt)));
	if (ret == FALSE) {
		PAL_ASSERT(0);
		return FALSE;
	}

	/* Under recovery, get dtbo info from bootimg hdr version 1 */
	uint64_t recovery_dtbo_offset = 0;
	if (g_boot_mode == RECOVERY_BOOT) {
		if (get_header_version() == BOOT_HEADER_VERSION_ONE) {
			recovery_dtbo_offset = get_recovery_dtbo_offset();
		}
	}

	bool rtn = dtb_overlay(fdt, dtb_size, (uint64_t)recovery_dtbo_offset);
	UNUSED(rtn);
	/* command line buffer init */
	bootargs_init(fdt);
#endif

	strncpy(checker, FDT_BUFF_END, FDT_CHECKER_SIZE - 1);
	checker[FDT_CHECKER_SIZE - 1] = '\0';

	dtb_size = fdt32_to_cpu(*(unsigned int *)(fdt + 0x4));

	extern int target_fdt_jtag(void *fdt)__attribute__((weak));
	if (target_fdt_jtag)
		target_fdt_jtag(fdt);

	extern int target_fdt_model(void *fdt)__attribute__((weak));
	if (target_fdt_model)
		target_fdt_model(fdt);

	extern int target_fdt_cpus(void *fdt)__attribute__((weak));
	if (target_fdt_cpus)
		target_fdt_cpus(fdt);

	load_images(g_boot_mode);

#ifdef MTK_SECURITY_ANTI_ROLLBACK
#ifdef MTK_OTP_FRAMEWORK_V2
		if (g_boot_mode == NORMAL_BOOT || g_boot_mode == RECOVERY_BOOT) {
			ret = sec_otp_ver_update(g_boot_mode);
		}
#else
		if (g_boot_mode == NORMAL_BOOT) {
			ret = sec_otp_ver_update(g_boot_mode);
			imgver_not_sync_warning(g_boot_arg->pl_imgver_status, ret);
		}
#endif
#endif

	extern int setup_mem_property_use_mblock_info(dt_dram_info *,
			size_t) __attribute__((weak));
	if (setup_mem_property_use_mblock_info) {
		ret = setup_mem_property_use_mblock_info(
			      &mem_reg_property[0],
			      sizeof(mem_reg_property) / sizeof(dt_dram_info));
		if (ret) {
			PAL_ASSERT(0);
			return FALSE;
		}
	} else {
		for (i = 0; i < g_nr_bank; ++i) {
			unsigned int fb_size = (i == g_nr_bank - 1) ? g_fb_size : 0;

#ifndef MTK_LM_MODE
			mem_reg_property[i].start_hi = cpu_to_fdt32(0);
			mem_reg_property[i].start_lo = cpu_to_fdt32(bi_dram[i].start);
			mem_reg_property[i].size_hi = cpu_to_fdt32(0);
			mem_reg_property[i].size_lo = cpu_to_fdt32(bi_dram[i].size - fb_size);

#else
			mem_reg_property[i].start_hi = cpu_to_fdt32(bi_dram[i].start >> 32);
			mem_reg_property[i].start_lo = cpu_to_fdt32(bi_dram[i].start);
			mem_reg_property[i].size_hi = cpu_to_fdt32((bi_dram[i].size - fb_size) >> 32);
			mem_reg_property[i].size_lo = cpu_to_fdt32(bi_dram[i].size - fb_size);

#endif
			pal_log_info(" mem_reg_property[%d].start_hi = 0x%08X\n", i,
				mem_reg_property[i].start_hi);
			pal_log_info(" mem_reg_property[%d].start_lo = 0x%08X\n", i,
				mem_reg_property[i].start_lo);
			pal_log_info(" mem_reg_property[%d].size_hi = 0x%08X\n", i,
				mem_reg_property[i].size_hi);
			pal_log_info(" mem_reg_property[%d].size_lo = 0x%08X\n", i,
				mem_reg_property[i].size_lo);
		}
	}

	extern int set_fdt_emi_info(void *fdt)__attribute((weak));
	if (set_fdt_emi_info) {
		ret = set_fdt_emi_info(fdt);
		if (ret)
			pal_log_err("ERROR: EMI info incorrect\n");
	}

	extern int target_fdt_dram_dummy_read(void *fdt,
					      unsigned int rank_num)__attribute__((weak));
	if (target_fdt_dram_dummy_read) {
		ret = target_fdt_dram_dummy_read(fdt, g_nr_bank);
		if (ret)
			pal_log_err("ERROR: DRAM dummy read address incorrect\n");
	}

	extern int set_fdt_dbg_info(void *fdt)__attribute__((weak));
	if (set_fdt_dbg_info) {
		ret = set_fdt_dbg_info(fdt);
		if (ret)
			pal_log_err("ERROR: debug info base and size incorrect\n");
	}
	/*
	 * if there is no memory node exist
	 * we will create a new one
	 */
#if defined(MBLOCK_LIB_SUPPORT) || defined(NEW_MEMORY_RESERVED_MODEL)
	{
		int nodeoffset;
		offset = fdt_path_offset(fdt, "/memory");
		if (offset < 0) {
			offset = fdt_path_offset(fdt, "/");
			if (offset < 0) {
				pal_log_err("ERROR: root node search failed , while(1)\n");
				while (1)
					;
			}
			nodeoffset = fdt_add_subnode(fdt, offset, "memory");
			if (nodeoffset < 0) {
				pal_log_err("ERROR: add subnode memory failed, while(1)\n");
				while (1)
					;
			} else {
				ret = fdt_setprop_string(fdt, nodeoffset, "device_type", "memory");
				pal_log_err("DTS:/memory node is not found create new memory node\n");
			}
		}
	}

	offset = fdt_path_offset(fdt, "/memory");
	if (offset < 0) {
		pal_log_err("ERROR: /memory node not exist, while(1)\n");
		while (1);
	}
#endif

	extern int get_mblock_num(void) __attribute__((weak));

#if defined(MBLOCK_LIB_SUPPORT)
#if	defined(MBLOCK_LIB_SUPPORT) && (MBLOCK_EXPAND(MBLOCK_LIB_SUPPORT) == MBLOCK_EXPAND(2))
	pal_log_err("PASS memory DTS node\n");
	ret = fdt_setprop(fdt, offset, "reg", mem_reg_property, sizeof(dt_dram_info));
#else
	pal_log_err("PASS memory DTS node\n");
	ret = fdt_setprop(fdt, offset, "reg", mem_reg_property,
			  ((int)get_mblock_num ? get_mblock_num() : g_nr_bank) * sizeof(dt_dram_info));
#endif
#else
#if defined(NEW_MEMORY_RESERVED_MODEL)
	pal_log_err("PASS memory DTS node\n");
	ret = fdt_setprop(fdt, offset, "reg", mem_reg_property,
			  ((int)get_mblock_num ? get_mblock_num() : g_nr_bank) * sizeof(dt_dram_info));
#endif
#endif
	if (ret) {
		assert(0);
		return FALSE;
	}

	if (platform_atag_append) {
		ret = platform_atag_append(fdt);
		if (ret) {
			assert(0);
			return FALSE;
		}
	}
#ifdef MBLOCK_LIB_SUPPORT
	ret = fdt_memory_append(fdt);
	if (ret) {
		assert(0);
		return FALSE;
	}
#endif

	offset = fdt_path_offset(fdt, "/chosen");
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-start",
			       (unsigned int) ramdisk);
	if (ret) {
		assert(0);
		return FALSE;
	}
	ret = fdt_setprop_cell(fdt, offset, "linux,initrd-end",
			       (unsigned int)ramdisk + ramdisk_sz);
	if (ret) {
		assert(0);
		return FALSE;
	}

	ptr = (char *)target_atag_boot((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,boot", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}

	seedp = fdt_getprop(fdt, offset, "kaslr-seed", &seed_len);
	/* get random kaslr-seed if it is defined in the dtb */
	if (seedp) {
		if (seed_len != sizeof(u64)) {
			pal_log_err("incorrect kaslr-seed length=%d\n",
				seed_len);
			assert(0);
			return FALSE;
		}
		get_rnd(&seed[0]);
		get_rnd(&seed[1]);
		ret = fdt_setprop(fdt, offset, "kaslr-seed", seed, seed_len);
		if (ret) {
			assert(0);
			return FALSE;
		}
	}

#if defined(MTK_DLPT_SUPPORT)
	ptr = (char *)target_atag_imix_r((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,imix_r", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}
#endif
	snprintf(buf, FDT_BUFF_SIZE, "%d", fg_swocv_v);
	ptr = buf + strlen(buf);
	ret = fdt_setprop(fdt, offset, "atag,fg_swocv_v", buf, ptr - buf);
	pal_log_err("fg_swocv_v buf [%s], [0x%x:0x%x:%d]\n", buf, (unsigned)buf,
		(unsigned)ptr, ptr - buf);

	snprintf(buf, FDT_BUFF_SIZE, "%d", fg_swocv_i);
	ptr = buf + strlen(buf);
	ret = fdt_setprop(fdt, offset, "atag,fg_swocv_i", buf, ptr - buf);
	pal_log_err("fg_swocv_i buf [%s], [0x%x:0x%x:%d]\n", buf, (unsigned)buf,
		(unsigned)ptr, ptr - buf);

	snprintf(buf, FDT_BUFF_SIZE, "%d", shutdown_time);
	ptr = buf + strlen(buf);
	ret = fdt_setprop(fdt, offset, "atag,shutdown_time", buf, ptr - buf);
	pal_log_err("shutdown_time buf [%s], [0x%x:0x%x:%d]\n", buf,
		(unsigned)buf, (unsigned)ptr, ptr - buf);

	snprintf(buf, FDT_BUFF_SIZE, "%d", boot_voltage);
	ptr = buf + strlen(buf);
	ret = fdt_setprop(fdt, offset, "atag,boot_voltage", buf, ptr - buf);
	pal_log_err("boot_voltage buf [%s], [0x%x:0x%x:%d]\n", buf, (unsigned)buf,
		(unsigned)ptr, ptr - buf);

	snprintf(buf, FDT_BUFF_SIZE, "%d", two_sec_reboot);
	ptr = buf + strlen(buf);
	ret = fdt_setprop(fdt, offset, "atag,two_sec_reboot", buf, ptr - buf);
	pal_log_err("boot_voltage buf [%s], [0x%x:0x%x:%d]\n", buf, (unsigned)buf,
		(unsigned)ptr, ptr - buf);

	ptr = (char *)target_atag_mem((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}

	if (target_atag_partition_data) {
		ptr = (char *)target_atag_partition_data((unsigned *)buf);
		if (ptr != buf) {
			ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);
			if (ret) {
				assert(0);
				return FALSE;
			}
		}
	}
#if !(defined(MTK_UFS_SUPPORT) || defined(MTK_EMMC_SUPPORT))
	if (target_atag_nand_data) {
		ptr = (char *)target_atag_nand_data((unsigned *)buf);
		if (ptr != buf) {
			ret = fdt_setprop(fdt, offset, "atag,mem", buf, ptr - buf);
			if (ret) {
				assert(0);
				return FALSE;
			}
		}
	}
#endif
	extern unsigned int *target_atag_vcore_dvfs(unsigned * ptr)__attribute__((
				weak));
	if (target_atag_vcore_dvfs) {
		ptr = (char *)target_atag_vcore_dvfs((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "atag,vcore_dvfs", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		}
	} else
		pal_log_err("Not Support VCORE DVFS\n");

	//some platform might not have this function, use weak reference for
	extern unsigned *target_atag_dfo(unsigned * ptr)__attribute__((weak));
	if (target_atag_dfo) {
		ptr = (char *)target_atag_dfo((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "atag,dfo", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		}
	}

	if (g_boot_mode == META_BOOT || g_boot_mode == ADVMETA_BOOT ||
	    g_boot_mode == ATE_FACTORY_BOOT || g_boot_mode == FACTORY_BOOT) {
		ptr = (char *)target_atag_meta((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "atag,meta", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		}
		unsigned int meta_com_id = g_boot_arg->meta_com_id;
		if (g_boot_mode == META_BOOT) {
			int adb = !(meta_com_id & 0x0001);
			int elt = !!(meta_com_id & 0x0004);

			if (!adb && !elt) {
				/*only META*/
				cmdline_append("androidboot.usbconfig=1");
			} else if (adb && !elt) {
				/*META + ADB*/
				cmdline_append("androidboot.usbconfig=0");
			} else if (!adb && elt) {
				/*META + ELT*/
				cmdline_append("androidboot.usbconfig=2");
			} else {
				/*META + ELT + ADB*/
				cmdline_append("androidboot.usbconfig=3");
			}
		} else {
			int adb = !(meta_com_id & 0x0001);
			if (!adb) {
				cmdline_append("androidboot.usbconfig=1");
			} else {
				cmdline_append("androidboot.usbconfig=0");
			}
		}
		if (g_boot_mode == META_BOOT || g_boot_mode == ADVMETA_BOOT) {
			snprintf(tmpbuf, TMPBUF_SIZE, "androidboot.init_rc=%s", META_INIT_RC);
			cmdline_append(tmpbuf);

			if ((meta_com_id & 0x0002) != 0)
				cmdline_append("androidboot.mblogenable=0");

			else
				cmdline_append("androidboot.mblogenable=1");
		} else {
			snprintf(tmpbuf, TMPBUF_SIZE, "androidboot.init_rc=%s", FACTORY_INIT_RC);
			cmdline_append(tmpbuf);
		}
	}

#ifdef PRIZE_CAMERA_HALL_CALI
	/*prize  add  for hall  calibration by zhuzhengjiang    20190221-begin*/
	int len=0;
	char hall_calidata_buf[638] = {0};
	//len =mboot_common_get_hall_calidata(hall_calidata_buf);
//	mboot_recovery_load_raw_part_offset("proinfo", hall_calidata_buf,1536*1024,655);
	mboot_recovery_load_raw_part_offset("proinfo", hall_calidata_buf,0,637); // hall calidata addr 632 ,length :6
	for(len = 632 ;len<638;len++)
	{
	    //pal_log_err("tangan prize proinfo buffer=%d\n",buf[j]);
        dprintf(CRITICAL,"prize proinfo hall_calidata_buf[%d]=%d\n",len,hall_calidata_buf[len] - 48 );
	}
	snprintf(cmdline_tmpbuf, CMDLINE_HALL_CALIDATA_SIZE, "hall_up_cali_data=%d%d%d", hall_calidata_buf[632]-'0',hall_calidata_buf[633]-'0',hall_calidata_buf[634]-'0');
	cmdline_append(cmdline_tmpbuf);
	   
	snprintf(cmdline_tmpbuf, CMDLINE_HALL_CALIDATA_SIZE, "hall_down_cali_data=%d%d%d", hall_calidata_buf[635]-'0',hall_calidata_buf[636]-'0',hall_calidata_buf[637]-'0');
	cmdline_append(cmdline_tmpbuf);
	/*prize  add  for hall  calibration by zhuzhengjiang    20190221-end*/
#endif	
/* prize added by wangmengdong, hardwareinfo, add current flash-information, 20190614-start */
	snprintf(tmpbuf, TMPBUF_SIZE, "lpddr_used_index=%d", g_boot_arg->emmc_lpddr_used_index);
	cmdline_append(tmpbuf);
/* prize added by wangmengdong, hardwareinfo, add current flash-information, 20190614-end */
	ptr = (char *)target_atag_devinfo_data((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,devinfo", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}

#ifndef MACH_FPGA_NO_DISPLAY
	ptr = (char *)target_atag_videolfb((unsigned *)buf, FDT_BUFF_SIZE);
	ret = fdt_setprop(fdt, offset, "atag,videolfb", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}

#if (MTK_DUAL_DISPLAY_SUPPORT == 2)
	ptr = (char *)target_atag_ext_videolfb((unsigned *)buf);
	ret = fdt_setprop(fdt, offset, "atag,ext_videolfb", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}
#endif
#else

	extern int mt_disp_config_frame_buffer(void *fdt)__attribute__((weak));
	if (mt_disp_config_frame_buffer)
		ret = mt_disp_config_frame_buffer(fdt);

#endif

	extern int lastpc_decode(void *fdt)__attribute__((weak));
	if (lastpc_decode) {
		ret = lastpc_decode(fdt);
		if (ret) {
			assert(0);
			return FALSE;
		}
	}

	if (target_atag_mdinfo) {
		ptr = (char *)target_atag_mdinfo((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "atag,mdinfo", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		}
	} else
		pal_log_err("DFO_MODEN_INFO Only support in MT6582/MT6592\n");

	ccci_update_lk_arg_info_to_dt(buf, fdt, offset);

	extern unsigned int *target_atag_ptp(unsigned * ptr)__attribute__((weak));
	if (target_atag_ptp) {
		ptr = (char *)target_atag_ptp((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "atag,ptp", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		} else
			pal_log_err("Create PTP DT OK\n");
	} else
		pal_log_err("PTP_INFO Only support in MT6795\n");

	if (target_atag_masp_data) {
		ptr = (char *)target_atag_masp_data((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "atag,masp", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		} else
			pal_log_err("create masp atag OK\n");
	} else
		pal_log_err("masp atag not support in this platform\n");

	extern unsigned int *target_atag_tee(unsigned * ptr)__attribute__((weak));
	if (target_atag_tee) {
		ptr = (char *)target_atag_tee((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "tee_reserved_mem", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		}
	} else
		pal_log_err("tee_reserved_mem not supported\n");

	extern unsigned int *target_atag_isram(unsigned * ptr)__attribute__((weak));
	if (target_atag_isram) {
		ptr = (char *)target_atag_isram((unsigned *)buf);
		ret = fdt_setprop(fdt, offset, "non_secure_sram", buf, ptr - buf);
		if (ret) {
			assert(0);
			return FALSE;
		}
	} else
		pal_log_err("non_secure_sram not supported\n");

	if (!has_set_p2u) {
		switch (eBuildType) {
		case BUILD_TYPE_USER:
			if (((g_boot_mode == META_BOOT) && is_meta_log_disable &&
#ifdef LOG_STORE_SUPPORT
			    (is_meta_log_disable() == 0)) || g_boot_arg->log_dynamic_switch)
#else
		      (is_meta_log_disable() == 0)))
#endif
				cmdline_append("printk.disable_uart=0");
			else
				cmdline_append("printk.disable_uart=1");
			break;

		case BUILD_TYPE_USERDEBUG:
			if ((g_boot_mode == META_BOOT) && is_meta_log_disable &&
#ifdef LOG_STORE_SUPPORT
			    (is_meta_log_disable() == 1) && (g_boot_arg->log_dynamic_switch == 0))
#else
			    (is_meta_log_disable() == 1))
#endif
				cmdline_append("printk.disable_uart=1 slub_debug=O");
#ifdef LOG_STORE_SUPPORT
			else if (boot_ftrace && g_boot_arg->log_dynamic_switch == 0)
#else
			else if (boot_ftrace)
#endif
				cmdline_append("printk.disable_uart=1 slub_debug=-");
			else
				cmdline_append("printk.disable_uart=0");
			break;

		case BUILD_TYPE_ENG:
			if ((g_boot_mode == META_BOOT) && is_meta_log_disable &&
			    (is_meta_log_disable() == 1))
				cmdline_append("printk.disable_uart=1 slub_debug=O");
			else
				cmdline_append("printk.disable_uart=0 ddebug_query=\"file *mediatek* +p ; file *gpu* =_\"");
			break;

		default:
			assert(0);
			break;
		}

		/*Append pre-loader boot time to kernel command line*/
		pl_t = g_boot_arg->boot_time;
		snprintf(tmpbuf, TMPBUF_SIZE, "%s%d", "bootprof.pl_t=", pl_t);
		cmdline_append(tmpbuf);
		/*Append lk boot time to kernel command line*/
		lk_t = ((unsigned int)get_timer(boot_time));
		snprintf(tmpbuf, TMPBUF_SIZE, "%s%d", "bootprof.lk_t=", lk_t);
		cmdline_append(tmpbuf);

		if (logo_lk_t != 0) {
			snprintf(tmpbuf, TMPBUF_SIZE, "%s%d", "bootprof.logo_t=", logo_lk_t);
			cmdline_append(tmpbuf);
		}

		PROFILING_PRINTF("1st logo takes %d ms", logo_lk_t);
		PROFILING_PRINTF("boot_time takes %d ms", lk_t);
	}
	/*Append pre-loader boot reason to kernel command line*/
#ifdef MTK_KERNEL_POWER_OFF_CHARGING
	if (g_boot_reason_change)
		boot_reason = 4;

	else
#endif
	{
		boot_reason = g_boot_arg->boot_reason;
	}
	snprintf(tmpbuf, TMPBUF_SIZE, "%s%d", "boot_reason=", boot_reason);
	cmdline_append(tmpbuf);

	/* Append androidboot.serialno=xxxxyyyyzzzz in cmdline */
	snprintf(tmpbuf, TMPBUF_SIZE, "%s%s", "androidboot.serialno=", sn_buf);
	cmdline_append(tmpbuf);

	/* snprintf(tmpbuf, TMPBUF_SIZE, "%s%s",  "androidboot.bootreason=",
		 g_boot_reason[boot_reason]); */
	reboot_reason = get_reboot_reason(boot_reason);
	snprintf(tmpbuf, TMPBUF_SIZE, "%s%s",  "androidboot.bootreason=",
		g_reboot_reason[reboot_reason]);
	cmdline_append(tmpbuf);


	extern unsigned int *target_commandline_force_gpt(char *cmd)__attribute__((
				weak));
	if (target_commandline_force_gpt)
		target_commandline_force_gpt((char *)cmdline_get());

	extern void ccci_append_tel_fo_setting(char *cmdline)__attribute__((weak));
	if (ccci_append_tel_fo_setting)
		ccci_append_tel_fo_setting((char *)cmdline_get());

	if (eBuildType == BUILD_TYPE_ENG)
		cmdline_append("initcall_debug=1");

	extern int dfd_set_base_addr(void *fdt)__attribute__((weak));
	if (dfd_set_base_addr) {
		ret = dfd_set_base_addr(fdt);
		if (ret)
			pal_log_err("[DFD] failed to get base address (%d)\n", ret);
	}

	extern unsigned int get_usb2jtag(void) __attribute__((weak));
	extern unsigned int set_usb2jtag(unsigned int en)  __attribute__((weak));
	if (get_usb2jtag) {
		if (get_usb2jtag() == 1)
			cmdline_append("usb2jtag_mode=1");
		else
			cmdline_append("usb2jtag_mode=0");
	}

	check_hibernation();

	/*DTS memory will be modified during lk boot process
	 * so we need to put the cmdline in the last mile*/
	mrdump_append_cmdline();

#if 0
	ptr = (char *)target_atag_commandline((u8 *)buf, FDT_BUFF_SIZE,
					      (const char *)cmdline_get());
	ret = fdt_setprop(fdt, offset, "atag,cmdline", buf, ptr - buf);
	if (ret) {
		assert(0);
		return FALSE;
	}
#else

	dprintf(INFO, "target_atag_commandline skip for not used\n"); //skip copy atag for kernel not used

#endif

	ret = fdt_setprop_string(fdt, offset, "bootargs", (char *)cmdline_get());
	if (ret) {
		assert(0);
		return FALSE;
	}

	extern int *target_fdt_firmware(void *fdt, char *serialno)__attribute__((weak));
	if (target_fdt_firmware)
		target_fdt_firmware(fdt, sn_buf);

	/* This depends on target_fdt_firmware, must after target_fdt_firmware */
	ccci_update_md_opt_to_fdt_firmware(fdt);

	/* Return the mb before stepping into kernel */
	mboot_free_bootimg_from_mblock();
	mboot_free_lk_scratch_from_mblock();

#if defined(MBLOCK_LIB_SUPPORT)
#if	defined(MBLOCK_LIB_SUPPORT) && (MBLOCK_EXPAND(MBLOCK_LIB_SUPPORT) == MBLOCK_EXPAND(2))
	/* this should be proper place for mblock memory santiy check*/
	ret = mblock_sanity_check(fdt, &g_boot_arg->mblock_info,
				  &g_boot_arg->orig_dram_info);
	if (ret) {
		assert(0);
		return FALSE;
	}

	ret = mblock_reserved_append(fdt);
	if (ret) {
		assert(0);
		return FALSE;
	}
#endif
#endif

	ccci_create_MD_attr_dt_node(fdt);

	ret = fdt_pack(fdt);
	if (ret) {
		assert(0);
		return FALSE;
	}

	pal_log_err("booting linux @ %p, ramdisk @ %p (%d)\n",
		kernel, ramdisk, ramdisk_sz);

	if (strncmp(checker, FDT_BUFF_END, FDT_CHECKER_SIZE) != 0) {
		pal_log_err("ERROR: fdt buff overflow\n");
		assert(0);
		return FALSE;
	}

	enter_critical_section();
	/* do any platform specific cleanup before kernel entry */
	platform_uninit();
#ifdef HAVE_CACHE_PL310
	l2_disable();
#endif

	arch_disable_cache(UCACHE);
	arch_disable_mmu();

#ifndef MACH_FPGA
	extern void platform_sec_post_init(void)__attribute__((weak));
	if (platform_sec_post_init)
		platform_sec_post_init();
#endif

	/*Prevent the system jumps to Kernel if we unplugged Charger/USB before*/
	if (kernel_charging_boot) {
		if (kernel_charging_boot() == -1) {
			pal_log_err(
				"[%s] Unplugged Usb/Charger in Kernel Charging Mode Before Jumping to Kernel, Power Off\n",
				__func__);
#ifndef NO_POWER_OFF
#ifdef MTK_PMIC_POWER_OFF
			mt_power_off();
#else
			mt6575_power_off();
#endif
#endif
		}
		if (kernel_charging_boot() == 1) {
			if (pmic_detect_powerkey()) {
				pal_log_err(
					"[%s] PowerKey Pressed in Kernel Charging Mode Before Jumping to Kernel, Reboot Os\n",
					__func__);
				//mt65xx_backlight_off();
				//mt_disp_power(0);
				mtk_arch_reset(1);
			}
		}
	}

	pal_log_err("DRAM Rank :%d\n", g_nr_bank);
	for (i = 0; i < g_nr_bank; i++) {
#ifndef MTK_LM_MODE
		pal_log_err("DRAM Rank[%d] Start = 0x%x, Size = 0x%x\n", i,
			(unsigned int)bi_dram[i].start, (unsigned int)bi_dram[i].size);
#else
		pal_log_err("DRAM Rank[%d] Start = 0x%llx, Size = 0x%llx\n", i,
			bi_dram[i].start, bi_dram[i].size);
#endif
	}

#ifdef MBLOCK_LIB_SUPPORT
	mblock_show_info();
#endif

	cmdline_print();
	pal_log_err("lk boot time = %d ms\n", lk_t);
	pal_log_err("lk boot mode = %d\n", g_boot_mode);
	pal_log_err("lk boot reason = %s\n", g_boot_reason[boot_reason]);
	pal_log_err("lk finished --> jump to linux kernel %s\n\n",
		g_is_64bit_kernel ? "64Bit" : "32Bit");

#ifdef USER_LOAD
	//For not intruding user load, print LK log in a whole here, when triggered.
	if (g_lk_final_log == RUNTIME_LOG_DEACTIVATED) {
		g_lk_final_log = RUNTIME_LOG_ACTIVATED;
		if (current_lk_buf_addr_get && current_buf_addr_get && current_buf_pl_lk_log_size_get) {
			*(u8 *)(current_buf_addr_get() + current_buf_pl_lk_log_size_get() - 1) = '\0';
			uart_puts((char *)current_lk_buf_addr_get());
		}
	}
#endif

	if (Debug_log_EMI_MPU)
		Debug_log_EMI_MPU();

	if (g_is_64bit_kernel) {
		lk_jump64((u32)entry, (u32)tags, 0, KERNEL_64BITS);
	} else {
#ifdef MTK_SMC_K32_SUPPORT
		lk_jump64((u32)entry, (u32)machtype, (u32)tags, KERNEL_32BITS);
#else
		entry(0, machtype, tags);
#endif
	}
	while (1);
	return 0;
}



#endif // DEVICE_TREE_SUPPORT

void boot_linux(void *kernel, unsigned *tags,
		unsigned machtype,
		void *ramdisk, unsigned ramdisk_sz)
{
#ifdef DEVICE_TREE_SUPPORT
	boot_linux_fdt((void *)kernel, (unsigned *)tags,
		       machtype,
		       (void *)ramdisk, ramdisk_sz);

	while (1) ;
#endif
}

#ifdef MTK_AB_OTA_UPDATER
void get_AB_OTA_param(void)
{
	p_AB_suffix = get_suffix();
	AB_retry_count = get_retry_count(p_AB_suffix);
	pal_log_err("[%s:%d] p_AB_suffix: %s, AB_retry_count: %d\n", __func__,
		__LINE__, p_AB_suffix, AB_retry_count);
}

void get_AB_OTA_name(char *part_name, int size)
{
	if (!p_AB_suffix)
		get_AB_OTA_param();
	snprintf(part_name, size, "%s%s", part_name, p_AB_suffix);
}
#endif /* MTK_AB_OTA_UPDATER */

int boot_linux_from_storage(void)
{
	int ret = 0;
	uint32_t kernel_target_addr = 0;
	uint32_t ramdisk_target_addr = 0;
	uint32_t tags_target_addr = 0;
	uint32_t ramdisk_addr = 0;
	uint32_t ramdisk_real_sz = 0;

#define CMDLINE_TMP_CONCAT_SIZE 110
	char cmdline_tmpbuf[CMDLINE_TMP_CONCAT_SIZE];
	switch (g_boot_mode) {
	case NORMAL_BOOT:
	case META_BOOT:
	case ADVMETA_BOOT:
	case SW_REBOOT:
	case ALARM_BOOT:
#ifdef MTK_KERNEL_POWER_OFF_CHARGING
	case KERNEL_POWER_OFF_CHARGING_BOOT:
	case LOW_POWER_OFF_CHARGING_BOOT:
#endif
		PROFILING_START("load boot image");
#if defined(CFG_NAND_BOOT)
		snprintf(cmdline_tmpbuf, CMDLINE_TMP_CONCAT_SIZE, "%s%x%s%x",
			 NAND_MANF_CMDLINE, nand_flash_man_code, NAND_DEV_CMDLINE, nand_flash_dev_id);
		cmdline_append(cmdline_tmpbuf);
#endif
		ret = load_vfy_boot(BOOTIMG_TYPE_BOOT, CFG_BOOTIMG_LOAD_ADDR);
		PAL_ASSERT(ret >= 0);

		PROFILING_END();
		break;

	case RECOVERY_BOOT:
		/* it's boot.img when system as root is enabled, and is *
		 * recovery.img when system as root is disabled. *
		 */
		PROFILING_START("load recovery image");

		ret = load_vfy_boot(BOOTIMG_TYPE_RECOVERY, CFG_BOOTIMG_LOAD_ADDR);
		PAL_ASSERT(ret >= 0);

		PROFILING_END();
		break;

	case FACTORY_BOOT:
	case ATE_FACTORY_BOOT:
		/* it's boot.img, we don't have standalone factory image now */
		PROFILING_START("load factory image");
#if defined(CFG_NAND_BOOT)
		snprintf(cmdline_tmpbuf, CMDLINE_TMP_CONCAT_SIZE, "%s%x%s%x",
			 NAND_MANF_CMDLINE, nand_flash_man_code, NAND_DEV_CMDLINE, nand_flash_dev_id);
		cmdline_append(cmdline_tmpbuf);
#endif
		ret = load_vfy_boot(BOOTIMG_TYPE_BOOT, CFG_BOOTIMG_LOAD_ADDR);
		PAL_ASSERT(ret >= 0);

		PROFILING_END();
		break;

	case FASTBOOT:
	case DOWNLOAD_BOOT:
	case UNKNOWN_BOOT:
		break;

	}

	kernel_target_addr = get_kernel_target_addr();
	ramdisk_target_addr = get_ramdisk_target_addr();
	ramdisk_addr = get_ramdisk_addr();
	ramdisk_real_sz = get_ramdisk_real_sz();
	tags_target_addr = get_tags_addr();

	PAL_ASSERT(kernel_target_addr != 0);
	PAL_ASSERT(ramdisk_target_addr != 0);
	PAL_ASSERT(ramdisk_addr != 0);
#if (!defined(SYSTEM_AS_ROOT) && !defined(MTK_RECOVERY_RAMDISK_SPLIT))
	PAL_ASSERT(ramdisk_real_sz != 0);
#endif

#ifdef MTK_3LEVEL_PAGETABLE
	/* rootfs addr */
	arch_mmu_map((uint64_t)ramdisk_target_addr,
		(uint32_t)ramdisk_target_addr,
		MMU_MEMORY_TYPE_NORMAL_WRITE_BACK | MMU_MEMORY_AP_P_RW_U_NA,
		ROUNDUP(LK_RAMDISK_MAX_SIZE, PAGE_SIZE));
#endif
#ifdef MTK_RECOVERY_RAMDISK_SPLIT
	if (g_boot_mode == RECOVERY_BOOT) {
		uint32_t ramdisk_compressed_sz;
		load_vfy_ramdisk(&ramdisk_compressed_sz);
		ramdisk_real_sz = ramdisk_compressed_sz;
	}
	else
#endif /* MTK_RECOVERY_RAMDISK_SPLIT */
	/* relocate rootfs */
	memcpy((void *)ramdisk_target_addr,
		(void *)ramdisk_addr,
		(size_t)ramdisk_real_sz);

	custom_port_in_kernel(g_boot_mode, cmdline_get());

	/* append cmdline from bootimg hdr */
	cmdline_append(get_cmdline());

#ifdef SELINUX_STATUS
#if SELINUX_STATUS == 1
	cmdline_append("androidboot.selinux=disabled");
#elif SELINUX_STATUS == 2
	cmdline_append("androidboot.selinux=permissive");
#endif
#endif

	/* This is patch for Android Test Mode(ATM). */
	/* 1. Sets kernel cmdline for ATM only in normal mode
	* 2. Bypass write protect in boot mode "normal" when ATM is enabled.
	* Background:
	* "proinfo" partition is write protected in boot mode "normal". When ATM is enabled,
	* we bypass write protection since we needs to write to proinfo. Whether device is in ATM
	* should also be passed to kernel through cmdline, only seen in normal mode
	*/
	if (g_boot_mode == NORMAL_BOOT) {
		if (true == get_atm_enable_status()) {
			cmdline_append("androidboot.atm=enable");
		} else if (false == get_atm_enable_status()) {
			write_protect_flow();
			cmdline_append("androidboot.atm=disabled");
		}
	}

	/* pass the meta_log_disable to user space logger, default is enable */
	if (is_meta_log_disable && (is_meta_log_disable() == 1)) {
		cmdline_append("androidboot.meta_log_disable=1");
	} else {
		cmdline_append("androidboot.meta_log_disable=0");
	}

	/* send kernel the dtbo_idx upon overlay success */
	if (hw_mdtbo_index != 0xff) {
		sprintf(cmdline_tmpbuf, "androidboot.dtbo_idx=%d", hw_mdtbo_index);
		cmdline_append(cmdline_tmpbuf);
	}

	/* pass related root of trust info via SMC call */
	if (send_root_of_trust_info != NULL)
		send_root_of_trust_info();

	boot_linux((void *)kernel_target_addr,
			(unsigned *)tags_target_addr,
		   	board_machtype(),
			(void *)ramdisk_target_addr,
			ramdisk_real_sz);

	while (1);

	return 0;
}

#if defined(CONFIG_MTK_USB_UNIQUE_SERIAL) || (defined(MTK_SECURITY_SW_SUPPORT) && defined(MTK_SEC_FASTBOOT_UNLOCK_SUPPORT))
static char udc_chr[32] = {"ABCDEFGHIJKLMNOPQRSTUVWSYZ456789"};

int get_serial(u64 hwkey, u32 chipid, char ser[SERIALNO_LEN])
{
	u16 hashkey[4];
	u32 idx, ser_idx;
	u32 digit, id;
	u64 tmp = hwkey;

	memset(ser, 0x00, SERIALNO_LEN);

	/* split to 4 key with 16-bit width each */
	tmp = hwkey;
	for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
		hashkey[idx] = (u16)(tmp & 0xffff);
		tmp >>= 16;
	}

	/* hash the key with chip id */
	id = chipid;
	for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
		digit = (id % 10);
		hashkey[idx] = (hashkey[idx] >> digit) | (hashkey[idx] << (16 - digit));
		id = (id / 10);
	}

	/* generate serail using hashkey */
	ser_idx = 0;
	for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
		ser[ser_idx++] = (hashkey[idx] & 0x001f);
		ser[ser_idx++] = (hashkey[idx] & 0x00f8) >> 3;
		ser[ser_idx++] = (hashkey[idx] & 0x1f00) >> 8;
		ser[ser_idx++] = (hashkey[idx] & 0xf800) >> 11;
	}
	for (idx = 0; idx < ser_idx; idx++)
		ser[idx] = udc_chr[(int)ser[idx]];
	ser[ser_idx] = 0x00;
	return 0;
}
#endif /* CONFIG_MTK_USB_UNIQUE_SERIAL */

#ifdef SERIAL_NUM_FROM_BARCODE
static inline int read_product_info(char *buf)
{
	int tmp = 0;

	if (!buf) return 0;

	mboot_recovery_load_raw_part("proinfo", buf, SN_BUF_LEN);

	for (; tmp < SN_BUF_LEN; tmp++) {
		if ((buf[tmp] == 0 || buf[tmp] == 0x20) && tmp > 0)
			break;

		else if (!isalpha(buf[tmp]) && !isdigit(buf[tmp]))
			break;
	}
	return tmp;
}
#endif

#ifdef CONFIG_MTK_USB_UNIQUE_SERIAL
/*PRIZE-uique serial-tangan-20170920-start*/
static inline int read_product_info_prize(char *buf)
{
	int tmp = 0;

	if (!buf) return 0;

	mboot_recovery_load_raw_part("proinfo", buf, PRIZE_SN_BUF_LEN);

	for ( ; tmp < PRIZE_SN_BUF_LEN; tmp++) {
		if ( (buf[tmp] == 0 || buf[tmp] == 0x20) && tmp > 0) {
			break;
		}else if ( !isalpha(buf[tmp]) && !isdigit(buf[tmp]))
			return 0;
	}
	return tmp;
}
/*PRIZE-uique serial-tangan-20170920-end*/
static inline int read_product_usbid(char *serialno)
{
	u64 key;
	u32 hrid_size, ser_len;
	u32 i, chip_code, errcode = 0;
	char *cur_serialp = serialno;
	char serial_num[SERIALNO_LEN];
    /*PRIZE-uique serial-tangan-20170920-start*/
	char tmp_prize[PRIZE_SN_BUF_LEN+1] = {0};
	unsigned ser_len_prize = 0;
	ser_len_prize = read_product_info_prize(tmp_prize);
	if(ser_len_prize == 0){
	/*PRIZE-uique serial-tangan-20170920-end*/
	/* read machine type */
	chip_code = board_machtype();

	/* read hrid */
	hrid_size = get_hrid_size();

	/* check ser_buf len. if need 128bit id, should defined into cust_usb.h */
	if (SN_BUF_LEN  < hrid_size * 8) {
		hrid_size = 2;
		errcode = 1;
	}

	for (i = 0; i < hrid_size / 2; i++) {
		key = get_devinfo_with_index(13 + i * 2); /* 13, 15 */
		key = (key << 32) | (unsigned int)get_devinfo_with_index(
			      12 + i * 2); /* 12, 14 */

		if (key != 0) {
			get_serial(key, chip_code, serial_num);
			ser_len = strlen(serial_num);
		} else {
			ser_len = strlen(DEFAULT_SERIAL_NUM);
			memcpy(serial_num, DEFAULT_SERIAL_NUM, ser_len);
			errcode = 2;
		}
		/* copy serial from serial_num to sn_buf */
		memcpy(cur_serialp, serial_num, ser_len);
		cur_serialp += ser_len;
	}
	cur_serialp = '\0';
	/*PRIZE-uique serial-tangan-20170920-start*/
	}else{
		memset( cur_serialp, 0, sizeof(cur_serialp)+1);
        int temp_i = 0;
        for(;temp_i<ser_len_prize;temp_i++){
            cur_serialp[temp_i] = tmp_prize[temp_i];
        }
		cur_serialp[temp_i] = '\0'; 
	}
	/*PRIZE-uique serial-tangan-20170920-end*/

	return errcode;
}
#endif

/******************************************************************************
******************************************************************************/
static void set_serial_num(void)
{
	unsigned int len;
	char *id_tmp = get_env("MTK_DEVICE_ID");
	if (!id_tmp) {
		pal_log_info("Set serial # to default value.\n");
		len = strlen(DEFAULT_SERIAL_NUM);
		len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
		strncpy(sn_buf, DEFAULT_SERIAL_NUM, len);
		sn_buf[len] = '\0';
	} else {
		pal_log_info("Set serial # from para.\n");
		len = strlen(id_tmp);
		len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
		strncpy(sn_buf, id_tmp, len);
		sn_buf[len] = '\0';
	}
/* prize modified by lifenfen, same usb serial number for automated testing, 20190409 begin */
#ifndef PRIZE_AUTOTEST_SUPPORT
#ifdef CONFIG_MTK_USB_UNIQUE_SERIAL
	int errcode = read_product_usbid(sn_buf);
	if (errcode)
		pal_log_err("Set serial # from efuse. error: %d\n", errcode);
	len = strlen(sn_buf);
	len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
    /*PRIZE-uique serial-tangan-20170920-start*/
    if(len == SN_BUF_LEN){
        len+=1;
    }
    /*PRIZE-uique serial-tangan-20170920-end*/
	sn_buf[len] = '\0';
#endif  // CONFIG_MTK_USB_UNIQUE_SERIAL

#ifdef SERIAL_NUM_FROM_BARCODE
	len = (unsigned int)read_product_info(sn_buf);  // sn_buf[] may be changed.
	if (len == 0) {
		len = strlen(DEFAULT_SERIAL_NUM);
		len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
		strncpy(sn_buf, DEFAULT_SERIAL_NUM, len);
	} else
		len = (len < SN_BUF_LEN) ? len : SN_BUF_LEN;
	sn_buf[len] = '\0';
#endif  // SERIAL_NUM_FROM_BARCODE
#endif
/* prize modified by lifenfen, same usb serial number for automated testing, 20190409 end */
	//pal_log_err("Serial #: \"%s\"\n", sn_buf);
	surf_udc_device.serialno = sn_buf;
}

void mt_boot_init(const struct app_descriptor *app)
{
	unsigned usb_init = 0;
	unsigned sz = 0;
#ifdef MTK_AB_OTA_UPDATER
	int ret;
#endif

	set_serial_num();

#ifdef MTK_DEBUG_SHELL
    if (app != NULL)
	goto lk_debug;
#endif

	if (g_boot_mode == FASTBOOT)
		goto fastboot;

#ifdef MTK_SECURITY_SW_SUPPORT
#if MTK_FORCE_VERIFIED_BOOT_SIG_VFY
	/* verify oem image with android verified boot signature instead of mediatek proprietary signature */
	/* verification is postponed to boot image loading stage */
	/* note in this case, boot/recovery image will be verified even when secure boot is disabled */
	g_boot_state = BOOT_STATE_RED;
#else
	if (0 != sec_boot_check(0))
		g_boot_state = BOOT_STATE_RED;
#endif
#endif

	/* Will not return */
	boot_linux_from_storage();

fastboot:
	target_fastboot_init();
	if (!usb_init)
		udc_init(&surf_udc_device);

	mt_part_dump();
	sz = target_get_max_flash_size();
	fastboot_init(target_get_scratch_address(), sz);
	udc_start();

#ifdef MTK_DEBUG_SHELL
lk_debug:
	   mtk_wdt_disable();
	   dprintf(INFO, "mt_boot_init not go to kernel and disable wdt !!\n");
#endif

}

APP_START(mt_boot)
.init = mt_boot_init,
 APP_END
