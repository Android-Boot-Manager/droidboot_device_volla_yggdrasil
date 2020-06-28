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

#ifndef __LOAD_VFY_BOOT_H__
#define __LOAD_VFY_BOOT_H__

#include <pal_typedefs.h>

/* list of boot image types */
#define BOOTIMG_TYPE_BOOT	(0)
#define BOOTIMG_TYPE_RECOVERY	(1)

struct boot_info {
	uint32_t type;
	uint32_t hdr_loaded;
	uint32_t img_loaded;
	uint32_t verified;
	uint32_t vfy_skipped;
	uint32_t load_addr;
	uint32_t recovery_dtbo_loaded;
	uint32_t kernel_with_mkimg_hdr;
};

/* these APIs are used only by legacy verified boot */
int mboot_android_load_bootimg_hdr(char *part_name, unsigned long addr);
int mboot_android_load_bootimg(char *part_name, unsigned long addr);
int mboot_android_load_recoveryimg_hdr(char *part_name, unsigned long addr);
int mboot_android_load_recoveryimg(char *part_name, unsigned long addr);
int mboot_android_load_factoryimg_hdr(char *part_name, unsigned long addr);
int mboot_android_load_factoryimg(char *part_name, unsigned long addr);

char *get_bootimg_partition_name(uint32_t bootimg_type);

uint32_t bootimg_hdr_valid(uint8_t *buf);
uint32_t load_bootimg_hdr(uint32_t bootimg_type);

uint32_t get_page_sz(void);
uint32_t get_kernel_target_addr(void);
uint32_t get_kernel_addr(void);
int32_t get_kernel_sz(void);
uint32_t get_kernel_real_sz(void);
uint32_t get_ramdisk_target_addr(void);
uint32_t get_ramdisk_addr(void);
int32_t get_ramdisk_sz(void);
uint32_t get_ramdisk_real_sz(void);
uint32_t get_bootimg_sz(void);
uint32_t get_tags_addr(void);
char *get_cmdline(void);
uint32_t get_os_version(void);
uint32_t get_header_version(void);
uint32_t get_header_size(void);
uint32_t get_recovery_dtbo_sz(void);
uint64_t get_recovery_dtbo_offset(void);
uint32_t get_recovery_dtbo_loaded(void);
void set_recovery_dtbo_loaded(void);
void set_bootimg_loaded(uint32_t addr);
void set_bootimg_verified(void);
void set_bootimg_verify_skipped(void);

int load_vfy_boot(uint32_t bootimg_type, uint32_t addr);
char *get_dtbo_part_name(void);

extern struct boot_info g_boot_info;
extern void *avb_heap;
extern uint32_t avb_heap_sz;

#endif /* __LOAD_VFY_BOOT_H__ */
