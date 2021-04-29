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

#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_rgba4444_roundedpattern.h"
#endif

#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#include <debug.h>
#ifndef MACH_FPGA
#include <lcm_pmic.h>
#endif
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <mach/mt_pm_ldo.h>
#include <mach/mt_gpio.h>
#endif

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(CRITICAL, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(INFO, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

#define LCM_ID_NT35695 (0xf5)

static const unsigned int BL_MIN_LEVEL = 20;
static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define MDELAY(n)       (lcm_util.mdelay(n))
#define UDELAY(n)       (lcm_util.udelay(n))


/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
      lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifndef BUILD_LK
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
/* #include <linux/jiffies.h> */
/* #include <linux/delay.h> */
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif

/* static unsigned char lcd_id_pins_value = 0xFF; */
static const unsigned char LCD_MODULE_ID = 0x01;
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                     (1080)
#define FRAME_HEIGHT                                    (2160)

#ifndef MACH_FPGA
#define GPIO_65132_EN GPIO_LCD_BIAS_ENP_PIN
#endif

#define REGFLAG_DELAY       0xFFFC
#define REGFLAG_UDELAY  0xFFFB
#define REGFLAG_END_OF_TABLE    0xFFFD
#define REGFLAG_RESET_LOW   0xFFFE
#define REGFLAG_RESET_HIGH  0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {} },
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{0x4F, 1, {0x01} },
	{REGFLAG_DELAY, 120, {} }
};

static struct LCM_setting_table init_setting[] = {
	{0xFF, 1, {0x20} },     /* page 0 */
	{0xFB, 1, {0x01} },
	{0x07, 1, {0x9e} },
	{0x0f, 1, {0xa4} },
	{0x1b, 1, {0x06} },
	{0x62, 1, {0xac} },
	{0x69, 1, {0xd1} },
	{0x6d, 1, {0x44} },
	{0x78, 1, {0x01} },
	{0x89, 1, {0x16} },
	{0x8a, 1, {0x16} },
	{0x8b, 1, {0x16} },  /* NVT 0X0F VCOM 0X34 -->gamma GVDDP 0X6B GVDDN 0XE0 */
	{0x8c, 1, {0x16} },  /* 6B NVT 0XCD */
	
	
	{0x95, 1, {0xe1} },  /* E0 NVT 0XCD */
	{0x96, 1, {0xe1} },
	{0xB0, 16, {0x00,0x05,0x00,0x1f,0x00,0x51,0x00,0x6f,0x00,0x8d,0x00,0xa2,0x00,0xb7,0x00,0xc8} },
	{0xB1, 16, {0x00,0xd9,0x01,0x0f,0x01,0x34,0x01,0x6f,0x01,0x9b,0x01,0xdd,0x02,0x11,0x02,0x13} },
	{0xB2, 16, {0x02,0x46,0x02,0x81,0x02,0xa9,0x02,0xde,0x03,0x02,0x03,0x2f,0x03,0x3e,0x03,0x4d} },
	{0xB3, 12, {0x03,0x60,0x03,0x75,0x03,0x8e,0x03,0xaa,0x03,0xd3,0x03,0xd7} },
	{0xB4, 16, {0x00,0x05,0x00,0x1f,0x00,0x51,0x00,0x6f,0x00,0x8d,0x00,0xa2,0x00,0xb7,0x00,0xc8} },
	{0xB5, 16, {0x00,0xd9,0x01,0x0f,0x01,0x34,0x01,0x6f,0x01,0x9b,0x01,0xdd,0x02,0x11,0x02,0x13} },
	{0xB6, 16, {0x02,0x46,0x02,0x81,0x02,0xa9,0x02,0xde,0x03,0x02,0x03,0x2f,0x03,0x3e,0x03,0x4d} },
	{0xB7, 16, {0x03,0x60,0x03,0x75,0x03,0x8e,0x03,0xaa,0x03,0xd3,0x03,0xd7} },
	{0xB8, 16, {0x00,0x05,0x00,0x1f,0x00,0x51,0x00,0x6f,0x00,0x8d,0x00,0xa2,0x00,0xb7,0x00,0xc8} },
	{0xB9, 16, {0x00,0xd9,0x01,0x0f,0x01,0x34,0x01,0x6f,0x01,0x9b,0x01,0xdd,0x02,0x11,0x02,0x13} },
	{0xBA, 16, {0x02,0x46,0x02,0x81,0x02,0xa9,0x02,0xde,0x03,0x02,0x03,0x2f,0x03,0x3e,0x03,0x4d} },
	{0xBB, 12, {0x03,0x60,0x03,0x75,0x03,0x8e,0x03,0xaa,0x03,0xd3,0x03,0xd7} },

	{0xFF, 1, {0x21} },
	{0xFB, 1, {0x01} },
	{0xB0, 16, {0x00,0x05,0x00,0x1f,0x00,0x51,0x00,0x6f,0x00,0x8d,0x00,0xa2,0x00,0xb7,0x00,0xc8} },
	{0xB1, 16, {0x00,0xd9,0x01,0x0f,0x01,0x34,0x01,0x6f,0x01,0x9b,0x01,0xdd,0x02,0x11,0x02,0x13} },
	{0xB2, 16, {0x02,0x46,0x02,0x81,0x02,0xa9,0x02,0xde,0x03,0x02,0x03,0x2f,0x03,0x3e,0x03,0x4d} },
	{0xB3, 12, {0x03,0x60,0x03,0x75,0x03,0x8e,0x03,0xaa,0x03,0xd3,0x03,0xd7} },
	{0xB4, 16, {0x00,0x05,0x00,0x1f,0x00,0x51,0x00,0x6f,0x00,0x8d,0x00,0xa2,0x00,0xb7,0x00,0xc8} },
	{0xB5, 16, {0x00,0xd9,0x01,0x0f,0x01,0x34,0x01,0x6f,0x01,0x9b,0x01,0xdd,0x02,0x11,0x02,0x13} },
	{0xB6, 16, {0x02,0x46,0x02,0x81,0x02,0xa9,0x02,0xde,0x03,0x02,0x03,0x2f,0x03,0x3e,0x03,0x4d} },
	{0xB7, 16, {0x03,0x60,0x03,0x75,0x03,0x8e,0x03,0xaa,0x03,0xd3,0x03,0xd7} },
	{0xB8, 16, {0x00,0x05,0x00,0x1f,0x00,0x51,0x00,0x6f,0x00,0x8d,0x00,0xa2,0x00,0xb7,0x00,0xc8} },
	{0xB9, 16, {0x00,0xd9,0x01,0x0f,0x01,0x34,0x01,0x6f,0x01,0x9b,0x01,0xdd,0x02,0x11,0x02,0x13} },
	{0xBA, 16, {0x02,0x46,0x02,0x81,0x02,0xa9,0x02,0xde,0x03,0x02,0x03,0x2f,0x03,0x3e,0x03,0x4d} },
	{0xBB, 12, {0x03,0x60,0x03,0x75,0x03,0x8e,0x03,0xaa,0x03,0xd3,0x03,0xd7} },


	{0xFF, 1, {0x24} },     /* page 4 */
	{0xFB, 1, {0x01} },
	{0x00, 1, {0x1c} },
	{0x01, 1, {0x1c} },
	{0x02, 1, {0x1c} },
	{0x03, 1, {0x1c} },
	{0x04, 1, {0x1c} },
	{0x05, 1, {0x1c} },
	{0x06, 1, {0x1c} },
	{0x07, 1, {0x1c} },
	{0x08, 1, {0x1c} },
	{0x09, 1, {0x24} },
	{0x0a, 1, {0x01} },
	{0x0b, 1, {0x00} },
	{0x0c, 1, {0x10} },
	{0x0d, 1, {0x14} },
	{0x0e, 1, {0x12} },
	{0x0f, 1, {0x03} },
	{0x10, 1, {0x05} },
	{0x11, 1, {0x04} },
	{0x12, 1, {0x06} },
	{0x13, 1, {0x0d} },
	{0x14, 1, {0x0d} },
	{0x15, 1, {0x13} },
	{0x16, 1, {0x11} },
	{0x17, 1, {0x1c} },
	{0x18, 1, {0x1c} },
	{0x19, 1, {0x1c} },
	{0x1a, 1, {0x1c} },
	{0x1b, 1, {0x1c} },
	{0x1c, 1, {0x1c} },
	{0x1d, 1, {0x1c} },
	{0x1e, 1, {0x1c} },
	{0x1f, 1, {0x1c} },
	{0x20, 1, {0x00} },
	{0x21, 1, {0x12} },
	{0x22, 1, {0x0d} },
	{0x23, 1, {0x0d} },
	{0x24, 1, {0x03} },
	{0x25, 1, {0x05} },
	{0x26, 1, {0x04} },
	{0x27, 1, {0x06} },
	{0x28, 1, {0x01} },
	{0x29, 1, {0x11} },
	{0x2a, 1, {0x15} },
	{0x2b, 1, {0x13} },
	{0x2c, 1, {0x00} },
	{0x2d, 1, {0x24} },
	{0x2e, 1, {0x00} },
	{0x2f, 1, {0x10} },
	{0x30, 1, {0x2b} },
	{0x31, 1, {0x04} },
	{0x32, 1, {0x08} },
	{0x33, 1, {0x00} },
	{0x34, 1, {0x04} },
	{0x35, 1, {0x00} },
	{0x36, 1, {0x01} }, /* Modified VBP, VFP Dummy Lines */
	{0x37, 1, {0x03} },
	{0x38, 1, {0x65} },
	{0x39, 1, {0x65} }, /* Modified MUX Pulse Width */
	{0x3f, 1, {0x65} },
	{0x40, 1, {0x0b} },
	{0x41, 1, {0x04} },
	{0x42, 1, {0x08} },
	{0x4c, 1, {0x15} },
	{0x4d, 1, {0x14} }, /* COLUMN INVERSION */
	{0x60, 1, {0x90} },
	{0x61, 1, {0x08} },
	{0x62, 1, {0x01} },
	{0x63, 1, {0x20} },
	{0x6a, 1, {0xc0} },
	{0x6b, 1, {0x00} },
	{0x6c, 1, {0x30} },
	{0x72, 1, {0x00} },
	{0x73, 1, {0x00} },
	{0x74, 1, {0x00} },
	{0x75, 1, {0x00} },
	{0x79, 1, {0x11} },
	{0x7a, 1, {0x03} },
	{0x7b, 1, {0x9b} },
	{0x7c, 1, {0x80} },
	{0x7d, 1, {0x07} },
	{0x7e, 1, {0x00} },
	{0x7f, 1, {0x00} },   /* page 5 */
	{0x80, 1, {0x42} },
	{0x81, 1, {0x03} },
	{0x82, 1, {0x11} },
	{0x83, 1, {0x22} },
	{0x84, 1, {0x33} },
	{0x85, 1, {0x00} },
	{0x86, 1, {0x00} },
	{0x87, 1, {0x00} },
	{0x88, 1, {0x11} },
	{0x89, 1, {0x22} },
	{0x8a, 1, {0x33} },
	{0x8b, 1, {0x00} },
	{0x8c, 1, {0x00} },
	{0x8d, 1, {0x00} },
	{0x92, 1, {0x76} },
	{0x93, 1, {0x0a} },
	{0x94, 1, {0x40} },
	{0x9d, 1, {0xb1} },
	{0xb5, 1, {0x54} },
	{0xdc, 1, {0x04} },
	{0xdd, 1, {0x00} },
	{0xde, 1, {0x00} },
	{0xdf, 1, {0x0e} },
	{0xe0, 1, {0x73} },
	{0xe1, 1, {0x04} }, /* 0x3E , NVT 0x3F */
	{0xe2, 1, {0x02} },
	{0xe3, 1, {0x0e} },
	{0xe4, 1, {0x0e} },
	{0xe5, 1, {0x80} },
	{0xeb, 1, {0x03} },

	{0xFF, 1, {0x25} },
	{0xFB, 1, {0x01} },     /* page6 */
	{0x21, 1, {0x1b} },
	{0x22, 1, {0x1b} },
	{0x23, 1, {0x00} },
	{0x24, 1, {0x76} },
	{0x25, 1, {0x76} },
	{0x66, 1, {0x58} },
	{0x67, 1, {0x15} },
	{0x68, 1, {0x58} },
	{0x69, 1, {0x10} },
	{0x6a, 1, {0x0a} },     /* page7 */
	{0x6b, 1, {0x00} },
	{0x6c, 1, {0x55} },
	{0x6d, 1, {0x00} },
	{0x6e, 1, {0x00} },
	{0x6f, 1, {0xd0} },
	{0x70, 1, {0x05} },
	{0x71, 1, {0x5d} },
	{0x72, 1, {0xf2} },
	{0x73, 1, {0x40} },
	{0x74, 1, {0x41} },
	{0x75, 1, {0x45} },
	{0x76, 1, {0x03} },
	{0x77, 1, {0x62} },
	{0x78, 1, {0x15} },
	{0x79, 1, {0x00} },
	{0x7a, 1, {0x00} },
	{0x7b, 1, {0x00} },
	{0x7c, 1, {0x55} },
	{0x7d, 1, {0x00} },
	{0x7e, 1, {0x1d} },
	{0x7f, 1, {0x08} },
	{0x80, 1, {0x00} },
	{0x81, 1, {0x05} },
	{0x82, 1, {0x00} },
	{0x83, 1, {0x00} },
	{0x84, 1, {0x0d} },
	{0xbf, 1, {0x00} },

	{0xFF, 1, {0x26} },
	{0xFB, 1, {0x01} },
	{0x06, 1, {0xff} },
	{0x0c, 1, {0x08} },
	{0x0d, 1, {0x00} },
	{0x0e, 1, {0x01} },
	{0x0f, 1, {0x05} },
	{0x10, 1, {0x06} },
	{0x11, 1, {0x00} },
	{0x12, 1, {0x00} },
	{0x13, 1, {0x00} },
	{0x14, 1, {0x00} },
	{0x15, 1, {0xa4} },
	{0x19, 1, {0x14} }, 
	{0x1a, 1, {0x80} },
	{0x1b, 1, {0x01} },
	{0x1c, 1, {0xaf} },
	{0x1d, 1, {0x14} },
	{0x1e, 1, {0x03} },
	{0x98, 1, {0xf1} },
	{0xa9, 1, {0x11} },
	{0xaa, 1, {0x11} },
	{0xae, 1, {0x8a} },

	{0xFF, 1, {0x27} },
	{0xFB, 1, {0x01} },
	{0x13, 1, {0x00} },
	{0x1e, 1, {0x14} },
	{0xba, 1, {0x01} },
	{0xbb, 1, {0x00} },
	{0xd9, 1, {0x18} },	
	//{0x3B, 3, {0x03, 0x0a, 0x0a} },
	//{0x44, 2, {0x07, 0x78} }, /* set TE event @ line 0x778(1912) */
	//{0x51, 1, {0xFF} },
	//{0x53, 1, {0x24} },
	//{0x55, 1, {0x00} },
	/*{REGFLAG_DELAY, 200, {} },*/
	{0x11, 0, {} },

#ifndef LCM_SET_DISPLAY_ON_DELAY
	{REGFLAG_DELAY, 120, {} },
	{0x29, 0, {} },
#endif
};

#ifdef LCM_SET_DISPLAY_ON_DELAY
/* to reduce init time, we move 120ms delay to lcm_set_display_on() !! */
static struct LCM_setting_table set_display_on[] = {
		e{0x29, 0, {} },
} 
#endif 
#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A, 4, {0x00, 0x00, (FRAME_WIDTH >> 8), (FRAME_WIDTH & 0xFF)} },
	{0x2B, 4, {0x00, 0x00, (FRAME_HEIGHT >> 8), (FRAME_HEIGHT & 0xFF)} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif
#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	/* Sleep Out */
	{0x11, 1, {0x00} },
	{REGFLAG_DELAY, 120, {} },

	/* Display ON */
	{0x29, 1, {0x00} },
	{REGFLAG_DELAY, 20, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	/* Display off sequence */
	{0x28, 1, {0x00} },
	{REGFLAG_DELAY, 20, {} },

	/* Sleep Mode On */
	{0x10, 1, {0x00} },
	{REGFLAG_DELAY, 120, {} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif
static struct LCM_setting_table bl_level[] = {
	{0x51, 1, {0xFF} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {
		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

			case REGFLAG_DELAY:
				if (table[i].count <= 10)
					MDELAY(table[i].count);
				else
					MDELAY(table[i].count);
				break;

			case REGFLAG_UDELAY:
				UDELAY(table[i].count);
				break;

			case REGFLAG_END_OF_TABLE:
				break;

			default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
				break;
		}
	}
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
	params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
	params->dsi.mode = SYNC_PULSE_VDO_MODE;
	params->dsi.switch_mode = CMD_MODE;
#endif
	params->dsi.switch_mode_enable = 0;

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 2;
	params->dsi.vertical_backporch = 8;
	params->dsi.vertical_frontporch = 20;
	params->dsi.vertical_frontporch_for_low_power = 750;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 10;
	params->dsi.horizontal_backporch = 20;
	params->dsi.horizontal_frontporch = 40;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;
	params->dsi.ssc_disable                                                   = 1;
#ifndef MACH_FPGA
#if (LCM_DSI_CMD_MODE)
	params->dsi.PLL_CLOCK = 420;	/* this value must be in MTK suggested table */
#else
	params->dsi.PLL_CLOCK = 480;	/* this value must be in MTK suggested table */
#endif
#else
	params->dsi.pll_div1 = 0;
	params->dsi.pll_div2 = 0;
	params->dsi.fbk_div = 0x1;
#endif
	params->dsi.CLK_HS_POST = 36;
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x53;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;

	params->dsi.lane_swap_en = 1;

	params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_0] = MIPITX_PHY_LANE_3;
	params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_1] = MIPITX_PHY_LANE_0;
	params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_2] = MIPITX_PHY_LANE_1;
	params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_3] = MIPITX_PHY_LANE_2;
	params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_CK] = MIPITX_PHY_LANE_CK;
	params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_RX] = MIPITX_PHY_LANE_1;

#ifdef MTK_ROUND_CORNER_SUPPORT
	params->round_corner_params.round_corner_en = 1;
	params->round_corner_params.full_content = 0;
	params->round_corner_params.w = ROUND_CORNER_W;
	params->round_corner_params.h = ROUND_CORNER_H;
	params->round_corner_params.lt_addr = left_top;
	params->round_corner_params.rt_addr = right_top;
	params->round_corner_params.lb_addr = left_bottom;
	params->round_corner_params.rb_addr = right_bottom;
#endif
}

static void lcm_init_power(void)
{

}

static void lcm_suspend_power(void)
{

}

static void lcm_resume_power(void)
{

}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static U32 lcm_init_tick = 0;
static int is_display_on = 0;
#endif

static void lcm_init(void)
{
	int ret = 0;

	SET_RESET_PIN(0);

#ifndef MACH_FPGA
	/*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
	ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

	/* set AVDD 5.4v, (4v+28*0.05v) */
	/*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
	ret = PMU_REG_MASK(0xB3, 28, (0x3F << 0));
	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

	/* set AVEE */
	/*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
	ret = PMU_REG_MASK(0xB4, 28, (0x3F << 0));
	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB4);

	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
	ret = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

	MDELAY(15);

#endif
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);

	SET_RESET_PIN(1);
	MDELAY(10);

	push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);

#ifdef LCM_SET_DISPLAY_ON_DELAY
	lcm_init_tick = gpt4_get_current_tick();
	is_display_on = 0;
#endif
}

#ifdef LCM_SET_DISPLAY_ON_DELAY
static int lcm_set_display_on(void)
{
	U32 timeout_tick, i = 0;

	if (is_display_on)
		return 0;

	/* we need to wait 120ms after lcm init to set display on */
	timeout_tick = gpt4_time2tick_ms(120);

	while(!gpt4_timeout_tick(lcm_init_tick, timeout_tick)) {
		i++;
		if (i % 1000 == 0) {
			LCM_LOGI("nt35695B %s error: i=%u,lcm_init_tick=%u,cur_tick=%u\n", __func__,
				i, lcm_init_tick, gpt4_get_current_tick());
		}
	}

	push_table(set_display_on, sizeof(set_display_on) / sizeof(struct LCM_setting_table), 1);

	is_display_on = 1;
	return 0;
}
#endif

static void lcm_suspend(void)
{
	int ret;

	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
#ifndef MACH_FPGA
	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12);*/
	ret = PMU_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

	MDELAY(5);

#endif
	SET_RESET_PIN(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}

#if LCM_DSI_CMD_MODE == 1
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);
	unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
	unsigned char y0_LSB = (y0 & 0xFF);
	unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
	unsigned char y1_LSB = (y1 & 0xFF);

	unsigned int data_array[16];

#ifdef LCM_SET_DISPLAY_ON_DELAY
	lcm_set_display_on();
#endif

	data_array[0] = 0x00053902;
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[2] = (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
}
#else /* not LCM_DSI_CMD_MODE */

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
#ifdef LCM_SET_DISPLAY_ON_DELAY
	lcm_set_display_on();
#endif
}
#endif

static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0, version_id = 0;
	unsigned char buffer[2];
	unsigned int array[16];

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);

	SET_RESET_PIN(1);
	MDELAY(20);

	array[0] = 0x00023700;	/* read id return two byte,version and id */
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xF4, buffer, 2);
	id = buffer[0];     /* we only need ID */

	read_reg_v2(0xDB, buffer, 1);
	version_id = buffer[0];

	LCM_LOGI("%s,nt35695_id=0x%08x,version_id=0x%x\n", __func__, id, version_id);

	if (id == LCM_ID_NT35695 && version_id == 0x81)
		return 1;
	else
		return 0;

}


/* return TRUE: need recovery */
/* return FALSE: No need recovery */
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
	char buffer[3];
	int array[4];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x53, buffer, 1);

	if (buffer[0] != 0x24) {
		LCM_LOGI("[LCM ERROR] [0x53]=0x%02x\n", buffer[0]);
		return TRUE;
	}
	LCM_LOGI("[LCM NORMAL] [0x53]=0x%02x\n", buffer[0]);
	return FALSE;
#else
	return FALSE;
#endif

}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
	unsigned int ret = 0;
	unsigned int x0 = FRAME_WIDTH / 4;
	unsigned int x1 = FRAME_WIDTH * 3 / 4;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);

	unsigned int data_array[3];
	unsigned char read_buf[4];
	LCM_LOGI("ATA check size = 0x%x,0x%x,0x%x,0x%x\n", x0_MSB, x0_LSB, x1_MSB, x1_LSB);
	data_array[0] = 0x0005390A; /* HS packet */
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00043700; /* read id return two byte,version and id */
	dsi_set_cmdq(data_array, 1, 1);

	read_reg_v2(0x2A, read_buf, 4);

	if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB)
	        && (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
		ret = 1;
	else
		ret = 0;

	x0 = 0;
	x1 = FRAME_WIDTH - 1;

	x0_MSB = ((x0 >> 8) & 0xFF);
	x0_LSB = (x0 & 0xFF);
	x1_MSB = ((x1 >> 8) & 0xFF);
	x1_LSB = (x1 & 0xFF);

	data_array[0] = 0x0005390A; /* HS packet */
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	return ret;
#else
	return 0;
#endif
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{

	LCM_LOGI("%s,nt35695 backlight: level = %d\n", __func__, level);

	bl_level[0].para_list[0] = level;

	push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_setbacklight(unsigned int level)
{
	LCM_LOGI("%s,nt35695 backlight: level = %d\n", __func__, level);

	bl_level[0].para_list[0] = level;

	push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}

static void *lcm_switch_mode(int mode)
{
#ifndef BUILD_LK
	/* customization: 1. V2C config 2 values, C2V config 1 value; 2. config mode control register */
	if (mode == 0) {    /* V2C */
		lcm_switch_mode_cmd.mode = CMD_MODE;
		lcm_switch_mode_cmd.addr = 0xBB;    /* mode control addr */
		lcm_switch_mode_cmd.val[0] = 0x13;  /* enabel GRAM firstly, ensure writing one frame to GRAM */
		lcm_switch_mode_cmd.val[1] = 0x10;  /* disable video mode secondly */
	} else {        /* C2V */
		lcm_switch_mode_cmd.mode = SYNC_PULSE_VDO_MODE;
		lcm_switch_mode_cmd.addr = 0xBB;
		lcm_switch_mode_cmd.val[0] = 0x03;  /* disable GRAM and enable video mode */
	}
	return (void *)(&lcm_switch_mode_cmd);
#else
	return NULL;
#endif
}


LCM_DRIVER nt36672a_fhd_dsi_vdo_Innolux_lcm_drv = {
	.name = "nt36672a_fhd_dsi_vdo_Innolux_lcm_drv",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
	.init_power = lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,
	.esd_check = lcm_esd_check,
	.set_backlight = lcm_setbacklight,
	.ata_check = lcm_ata_check,
	.update = lcm_update,
	.switch_mode = lcm_switch_mode,
};
