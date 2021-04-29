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

#include <dev/mrdump.h>
#include "mrdump_private.h"
#include <memory_layout.h>
#include <platform/boot_mode.h>

int mrdump_detection(void) __attribute__((weak));
int mrdump_detection(void)
{
	return 0;
}

void mrdump_reserve_memory(void) __attribute__((weak));
void mrdump_reserve_memory(void)
{
#ifdef PL_BOOTARG_BASE
	if (mblock_query_reserved(&g_boot_arg->mblock_info,
				  "pl-bootarg", 0)) {
		if (mblock_create(&g_boot_arg->mblock_info,
				  &g_boot_arg->orig_dram_info,
				  PL_BOOTARG_BASE, PL_BOOTARG_MAX_SIZE)) {
			dprintf(CRITICAL, "%s: free PL_BOOTARG_BASE failed\n", __func__);
			assert(0);
		}
	}
#endif /* end of PL_BOOTARG_BASE */
}

int mrdump_run2(void) __attribute__((weak));
int mrdump_run2(void)
{
	return 0;
}

void mrdump_reboot(void) __attribute__((weak));
void mrdump_reboot(void)
{
}

int sLOG(char *fmt, ...) __attribute__((weak));
int sLOG(char *fmt, ...)
{
	return 0;
}

void mrdump_append_cmdline(void *fdt) __attribute__((weak));
void mrdump_append_cmdline(void *fdt)
{
}
