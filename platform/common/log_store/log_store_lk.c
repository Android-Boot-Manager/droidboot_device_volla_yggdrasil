/* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein is
* confidential and proprietary to MediaTek Inc. and/or its licensors. Without
* the prior written permission of MediaTek inc. and/or its licensors, any
* reproduction, modification, use or disclosure of MediaTek Software, and
* information contained herein, in whole or in part, shall be strictly
* prohibited.
*
* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
* ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
* NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
* RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
* INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
* TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
* RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
* OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
* SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
* RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
* ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
* RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
* MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
* CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek
* Software") have been modified by MediaTek Inc. All revisions are subject to
* any receiver's applicable license agreements with MediaTek Inc.
*/
#include <stdlib.h>
#include <arch/arm/mmu.h>
#include "log_store_lk.h"
#include <part_interface.h>
#ifdef MTK_GPT_SCHEME_SUPPORT
#include <platform/partition.h>
#else
#include <mt_partition.h>
#endif
extern part_dev_t* mt_part_get_device(void);

#define MOD "LK_LOG_STORE"

#define DEBUG_LOG

#ifdef DEBUG_LOG
#define LOG_DEBUG(fmt, ...) \
    log_store_enable = false; \
    _dprintf(fmt, ##__VA_ARGS__); \
    log_store_enable = true
#else
#define LOG_DEBUG(fmt, ...)
#endif


/* !!!!!!! Because log store be called by print, so these function don't use print log to debug.*/

enum {
	LOG_WRITE = 0x1,        /* Log is write to buff */
	LOG_READ_KERNEL = 0x2,  /* Log have readed by kernel */
	LOG_WRITE_EMMC = 0x4,   /* log need save to emmc */
	LOG_EMPTY = 0x8,        /* log is empty */
	LOG_FULL = 0x10,        /* log is full */
	LOG_PL_FINISH = 0X20,   /* pl boot up finish */
	LOG_LK_FINISH = 0X40,   /* lk boot up finish */
	LOG_DEFAULT = LOG_WRITE_EMMC|LOG_EMPTY,
} BLOG_FLAG;

static int log_store_status = BUFF_NOT_READY;
static struct pl_lk_log *dram_curlog_header;
static char *pbuff;
static struct dram_buf_header *sram_dram_buff;
static bool log_store_enable = true;
static bool lk_is_full = false;
static u32 lk_renew;

void log_store_init(void)
{
	struct sram_log_header *sram_header = NULL;

	LOG_DEBUG("%s:lk log_store_init start.\n", MOD);

	if (log_store_status != BUFF_NOT_READY) {
		LOG_DEBUG("%s:log_sotore_status is ready!\n", MOD);
		return;
	}

	/* SRAM buff header init */
	sram_header = (struct sram_log_header *)SRAM_LOG_ADDR;
	if (sram_header->sig != SRAM_HEADER_SIG) {
		LOG_DEBUG("%s:sram header 0x%x is not match: %d!\n", MOD, (unsigned int)sram_header, sram_header->sig);
		memset(sram_header, 0, sizeof(struct sram_log_header));
		sram_header->sig = SRAM_HEADER_SIG;
		log_store_status = BUFF_ALLOC_ERROR;
		return;
	}

	sram_dram_buff = &(sram_header->dram_buf);
	if (sram_dram_buff->sig != DRAM_HEADER_SIG || sram_dram_buff->flag == BUFF_ALLOC_ERROR) {
		log_store_status = BUFF_ALLOC_ERROR;
		LOG_DEBUG("%s:sram_dram_buff 0x%x, sig 0x%x, flag 0x%x.\n", MOD,
		          (unsigned int)sram_dram_buff, (unsigned int)sram_dram_buff->sig, (unsigned int)sram_dram_buff->flag);
		return;
	}

	pbuff = (char *)sram_dram_buff->buf_addr;
	dram_curlog_header = (struct pl_lk_log*)&(sram_header->dram_curlog_header);

#ifdef MTK_3LEVEL_PAGETABLE
	uint32_t start = ROUNDDOWN((uint32_t)pbuff, PAGE_SIZE);
	uint32_t logsize = ROUNDUP(((uint32_t)pbuff - start + LOG_STORE_SIZE), PAGE_SIZE);
	LOG_DEBUG("%s:dram pl/lk log buff mapping start addr = 0x%x, size = 0x%x\n", MOD, start, logsize);
	if (start >= DRAM_PHY_ADDR) {
		/*need to use header in DRAZM, we must allocate it first */
		log_store_enable = false;
		arch_mmu_map((uint64_t) start, start,
		             MMU_MEMORY_TYPE_DEVICE | MMU_MEMORY_AP_P_RW_U_NA, logsize);
		log_store_enable = true;
	}
#endif

	LOG_DEBUG("%s:sram buff header 0x%x,current log header 0x%x, sig 0x%x, buff_size 0x%x, pl log size 0x%x@0x%x, lk log size 0x%x@0x%x!\n",
	          MOD, (unsigned int)sram_header, (unsigned int)dram_curlog_header, (unsigned int)dram_curlog_header->sig, (unsigned int)dram_curlog_header->buff_size,
	          (unsigned int)dram_curlog_header->sz_pl, (unsigned int)dram_curlog_header->off_pl, (unsigned int)dram_curlog_header->sz_lk, (unsigned int)dram_curlog_header->off_lk);

	if (dram_curlog_header->sig != LOG_STORE_SIG || dram_curlog_header->buff_size != LOG_STORE_SIZE
	        || dram_curlog_header->off_pl != sizeof(struct pl_lk_log)) {
		log_store_status = BUFF_ERROR;
		LOG_DEBUG("%s: BUFF_ERROR, sig 0x%x, buff_size 0x%x, off_pl 0x%x.\n", MOD,
		          dram_curlog_header->sig, dram_curlog_header->buff_size, dram_curlog_header->off_pl);
		return;
	}

	if (dram_curlog_header->sz_pl + sizeof(struct pl_lk_log) >= LOG_STORE_SIZE) {
		LOG_DEBUG("%s: buff full pl size 0x%x.\n", MOD, dram_curlog_header->sz_pl);
		log_store_status = BUFF_FULL;
		return;
	}

	dram_curlog_header->off_lk = sizeof(struct pl_lk_log) + dram_curlog_header->sz_pl;
	dram_curlog_header->sz_lk = 0;
	dram_curlog_header->lk_flag = LOG_DEFAULT;


	log_store_status = BUFF_READY;
	LOG_DEBUG("%s: buff ready.\n", MOD);
}

void lk_log_store(char c)
{
	if (log_store_enable == false)
		return;

	if (log_store_status == BUFF_NOT_READY)
		log_store_init();

	if ((log_store_status != BUFF_READY) || (log_store_status == BUFF_FULL))
		return;

	if (lk_is_full) {
		if (lk_renew >= dram_curlog_header->sz_lk) lk_renew = 0;
		*(pbuff + dram_curlog_header->off_lk + lk_renew) = c;
		lk_renew++;
	} else {
		*(pbuff + dram_curlog_header->off_lk + dram_curlog_header->sz_lk) = c;
		dram_curlog_header->sz_lk++;
		if ((dram_curlog_header->off_lk + dram_curlog_header->sz_lk) >= LOG_STORE_SIZE) {
			lk_is_full = true;
			lk_renew = 0;
			LOG_DEBUG("%s: dram lk buff full", MOD);
		}
	}

	sram_dram_buff->buf_point = dram_curlog_header->sz_lk + dram_curlog_header->sz_pl;
}

u32 current_buf_addr_get(void)
{
	return (u32)(sram_dram_buff->buf_addr);
}

u32 current_lk_buf_addr_get(void)
{
	return (u32)(sram_dram_buff->buf_addr + dram_curlog_header->off_lk);
}

u32 current_buf_pl_lk_log_size_get(void)
{
	return LOG_STORE_SIZE;
}

void set_uart_log_flag(bool enable){
#ifdef UART_SWITCH_SUPPORT
	int index;
	part_dev_t *dev;
	u32 blk_sz;
	u64 part_size;
	u64 part_offset;
	struct log_emmc_header *log_header;

	index = partition_get_index("expdb");
	dev = mt_part_get_device();

	if (index == -1 || dev == NULL) {
		LOG_DEBUG("%s: no %s partition[%d]\n", MOD, "expdb", index);
		return;
	}
#if defined(MTK_NEW_COMBO_EMMC_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT) || defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_UFS_SUPPORT)
	int part_id = partition_get_region(index);
#endif
	part_size = partition_get_size(index);
	blk_sz = dev->blkdev->blksz;
	part_offset = partition_get_offset(index) + part_size - blk_sz;

	char prepostfix[blk_sz];


#if defined(MTK_EMMC_SUPPORT) || defined(MTK_UFS_SUPPORT)
#if defined(MTK_NEW_COMBO_EMMC_SUPPORT) || defined(MTK_UFS_SUPPORT)
	dev->read(dev, part_offset, (uchar *)prepostfix, blk_sz, part_id);
#else
	dev->read(dev, part_offset, (uchar *)prepostfix, blk_sz);
#endif
#else
	dev->read(dev, part_offset, (uchar *)prepostfix, blk_sz, part_id);
#endif
	log_header = (struct log_emmc_header *)prepostfix;

	if (enable) {
		log_header->uart_flag = 1;
	} else {
		log_header->uart_flag = 2;
	}

#if defined(MTK_EMMC_SUPPORT) || defined(MTK_UFS_SUPPORT)
#if defined(MTK_NEW_COMBO_EMMC_SUPPORT) || defined(MTK_UFS_SUPPORT)
	dev->write(dev, (uchar *)prepostfix, part_offset, blk_sz, part_id);
#else
	dev->write(dev, (uchar *)prepostfix, part_offset, blk_sz);
#endif
#else
	dev->write(dev, (uchar *)prepostfix, part_offset, blk_sz, part_id);
#endif
#endif
}
