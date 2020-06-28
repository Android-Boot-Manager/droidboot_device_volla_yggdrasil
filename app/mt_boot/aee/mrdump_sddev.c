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

#include <malloc.h>
#include <stddef.h>
#include <stdint.h>
#include <block_generic_interface.h>
#if 0
#include <platform/mmc_common_inter.h>
#endif
#include <part_interface.h>
#include <part_status.h>

#include "aee.h"
#include "kdump.h"
#include "kdump_sdhc.h"

static char *part_device_init(int dev, const char *part_name)
{
	if (partition_exists(part_name) != PART_OK)
		return NULL;

	voprintf_info("%s size: %lu Mb\n", part_name, partition_get_size_by_name(part_name) / 0x100000UL);

	return (char*) part_name;
}

static bool part_device_read(struct mrdump_dev *dev, uint64_t offset, uint8_t *buf, int32_t len)
{
	if (dev == NULL) {
		voprintf_error("%s dev is NULL!\n", __func__);
		return false;
	} else {
		return partition_read(dev->handle, offset, buf, len) == len;
	}
}

static bool part_device_write(struct mrdump_dev *dev, uint64_t offset, uint8_t *buf, int32_t len)
{
	if (dev == NULL) {
		voprintf_error("%s dev is NULL!\n", __func__);
		return false;
	} else {
		return partition_write(dev->handle, offset, buf, len) == len;
	}
}

struct mrdump_dev *mrdump_dev_emmc_vfat(void)
{
	char *fatpart;
	struct mrdump_dev *dev = malloc(sizeof(struct mrdump_dev));
	if (!dev) {
		voprintf_error("%s: malloc() failed!\n", __func__);
		return NULL;
	}

	fatpart = part_device_init(0, "intsd");
	if (fatpart == NULL) {
		voprintf_error("No VFAT partition found!\n");
		free(dev);
		return NULL;
	}
	dev->name = "emmc";
	dev->handle = fatpart;
	dev->read = part_device_read;
	dev->write = part_device_write;
	return dev;
}

static char *mrdump_get_ext4_partition(void)
{
	char *ext4part;

	ext4part = part_device_init(0, "userdata");   //mt6735, mt6752, mt6795
	if (ext4part != NULL)
		return ext4part;

	ext4part = part_device_init(0, "USRDATA");    //mt6582, mt6592, mt8127
	if (ext4part != NULL)
		return ext4part;

	ext4part = part_device_init(0, "PART_USER");  //mt6572
	if (ext4part != NULL)
		return ext4part;

	return NULL;
}

struct mrdump_dev *mrdump_dev_emmc_ext4(void)
{
	char *ext4part;
	struct mrdump_dev *dev = malloc(sizeof(struct mrdump_dev));
	if (!dev) {
		voprintf_error("%s: malloc() failed!\n", __func__);
		return NULL;
	}

	ext4part = mrdump_get_ext4_partition();
	if (ext4part == NULL) {
		voprintf_error("No EXT4 partition found!\n");
		free(dev);
		return NULL;
	}
	dev->name = "emmc";
	dev->handle = ext4part;
	dev->read = part_device_read;
	dev->write = part_device_write;
	return dev;
}


#if 0
static bool mrdump_dev_sdcard_read(struct mrdump_dev *dev, uint32_t sector_addr, uint8_t *pdBuf, int32_t blockLen)
{
	return mmc_wrap_bread(1, sector_addr, blockLen, pdBuf) == 1;
}

static bool mrdump_dev_sdcard_write(struct mrdump_dev *dev, uint32_t sector_addr, uint8_t *pdBuf, int32_t blockLen)
{
	return mmc_wrap_bwrite(1, sector_addr, blockLen, pdBuf) == 1;
}

struct mrdump_dev *mrdump_dev_sdcard(void)
{
	struct mrdump_dev *dev = malloc(sizeof(struct mrdump_dev));
	dev->name = "sdcard";
	dev->handle = NULL;
	dev->read = mrdump_dev_sdcard_read;
	dev->write = mrdump_dev_sdcard_write;

	mmc_legacy_init(2);
	return dev;
}
#endif
