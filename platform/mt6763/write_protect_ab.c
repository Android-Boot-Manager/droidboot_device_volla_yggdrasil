/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2017. All rights reserved.
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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#include <stdlib.h>
#include <string.h>
#include <platform/partition.h>
#include <partition_wp.h>
#include <printf.h>
#include <platform/boot_mode.h>
#include <platform/mtk_wdt.h>
#include <platform/sec_export.h>
#include <env.h>
#include <pal_log.h>
#include "part_interface.h"
#include "bootctrl.h"
#include <platform/verified_boot.h>

#define WRITE_PROTECT_PARTITION_NAME_SZ (32)

int set_write_protect(void)
{
	int err = 0;
	const char *ab_suffix = NULL;
	char wp_start[WRITE_PROTECT_PARTITION_NAME_SZ] = {0};
	char wp_end[WRITE_PROTECT_PARTITION_NAME_SZ] = {0};

	ab_suffix = get_suffix();
	if (ab_suffix == NULL) {
		dprintf(CRITICAL, "[%s] invalid address.\n", __func__);
		return ERR_INVALID_ADDR;
	}

#ifdef MTK_UFS_OTP

	/*
	 * UFS OTP partition
	 * All booting modes shall consider if OTP partition needs lock
	 *
	 * NOTE: Make sure OTP partition locking is executed prior than
	 *       other partitions. Because we do not allow below case:
	 *
	 *       - If OTP entry is already existed in current Secure Write Protection
	 *         Configuration Block and OTP partition locking request is not the first
	 *         request. For this case, UFS_OTP_ALREADY_LOCKED will be returned
	 *         and all other partition locking behind will be skipped.
	 */

	/* check if we needs to lock OTP partition now */
	if (ufs_lk_otp_lock_req("otp") == 1) {

		pal_log_info("[%s] Lock OTP partition ... \n", __func__);

		err = partition_write_prot_set("otp", "otp", WP_PERMANENT);

		if (err != 0) {
			pal_log_err("[%s] Lock otp failed: %d\n", __func__, err);
			return err;
		}
	} else
		pal_log_info("[%s] Lock OTP is not required\n", __func__);

#endif /* MTK_UFS_OTP */

	if (g_boot_mode == NORMAL_BOOT) {
		pal_log_info("[%s] Lock boot region \n", __func__);
		err = partition_write_prot_set("preloader", "preloader", WP_POWER_ON);

		if (err != 0) {
			pal_log_err("[%s] Lock boot region failed: %d\n", __func__, err);
			return err;
		}

		snprintf(wp_start, WRITE_PROTECT_PARTITION_NAME_SZ, "sec1");
		snprintf(wp_end, WRITE_PROTECT_PARTITION_NAME_SZ, "logo");

		pal_log_info("[%s]: Lock %s->%s \n", __func__, wp_start, wp_end);
		err = partition_write_prot_set(wp_start, wp_end, WP_POWER_ON);
		if (err != 0) {
			pal_log_err("[%s]: Lock %s->%s failed:%d\n",
				    __func__, wp_start, wp_end, err);
			return err;
		}

		memset(wp_start, 0, sizeof(char) * WRITE_PROTECT_PARTITION_NAME_SZ);
		memset(wp_end, 0, sizeof(char) * WRITE_PROTECT_PARTITION_NAME_SZ);
		snprintf(wp_start, WRITE_PROTECT_PARTITION_NAME_SZ, "md1img%s", ab_suffix);
#ifdef MTK_SECURITY_SW_SUPPORT
		if (TRUE == seclib_sec_boot_enabled(TRUE))
			snprintf(wp_end, WRITE_PROTECT_PARTITION_NAME_SZ, "system%s", ab_suffix);
		else
			snprintf(wp_end, WRITE_PROTECT_PARTITION_NAME_SZ, "tee%s", ab_suffix);
#else
		snprintf(wp_end, WRITE_PROTECT_PARTITION_NAME_SZ, "tee%s", ab_suffix);
#endif
		pal_log_info("[%s]: Lock %s->%s\n", __func__, wp_start, wp_end);
		err = partition_write_prot_set(wp_start, wp_end, WP_POWER_ON);
		if (err != 0) {
			pal_log_err("[%s]: Lock %s->%s failed:%d\n",
				    __func__, wp_start, wp_end, err);
			return err;
		}
	}

	pal_log_info("[%s] Lock seccfg\n", __func__);
	err = partition_write_prot_set("seccfg", "seccfg", WP_POWER_ON);
	if (err != 0) {
		pal_log_err("[%s]: Lock seccfg failed:%d\n", __func__, err);
		return err;
	}

	return 0;
}

void write_protect_flow(void)
{
#if defined(MTK_POWER_ON_WRITE_PROTECT) && !defined(MACH_FPGA)
#if MTK_POWER_ON_WRITE_PROTECT == 1
#if defined(MTK_EMMC_SUPPORT) || defined(MTK_UFS_SUPPORT)

	int bypass_wp = 0;
	int ret = 0;

#ifndef USER_BUILD
	bypass_wp = atoi(get_env("bypass_wp"));
	pal_log_err("bypass write protect flag = %d! \n", bypass_wp);
#endif

	if (!bypass_wp) {
		ret = set_write_protect();
		if (ret != 0)
			pal_log_err("write protect fail! \n");
		pal_log_err("write protect Done! \n");
	} else
		pal_log_err("Bypass write protect! \n");
#endif
#endif
#endif
}

