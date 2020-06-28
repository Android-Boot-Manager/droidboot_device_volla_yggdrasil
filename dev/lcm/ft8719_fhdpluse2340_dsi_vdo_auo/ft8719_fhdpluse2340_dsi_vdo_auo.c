#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"


#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
/*#include <mach/mt_pm_ldo.h>*/
#ifdef CONFIG_MTK_LEGACY
#include <mach/mt_gpio.h>
#endif
#endif

#ifdef CONFIG_MTK_LEGACY
#include <cust_gpio_usage.h>
#endif
#ifndef CONFIG_FPGA_EARLY_PORTING
#if defined(CONFIG_MTK_LEGACY)
#include <cust_i2c.h>
#endif
#endif

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)			(lcm_util.set_reset_pin((v)))
#define MDELAY(n)					(lcm_util.mdelay(n))

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) \
	lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
	lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
	lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE	0
#define FRAME_WIDTH  										(1080)
#define FRAME_HEIGHT 										(2340)

#define REGFLAG_DELAY             							 0xFFFA
#define REGFLAG_UDELAY             							 0xFFFB
#define REGFLAG_PORT_SWAP									 0xFFFC
#define REGFLAG_END_OF_TABLE      							 0xFFFD   // END OF REGISTERS MARKER

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
{0x00,1,{0x00}},
{0xFF,3,{0x87,0x19,0x01}},
{0x00,1,{0x80}},
{0xFF,2,{0x87,0x19}},
{0x00,1,{0xA1}},//A1,A2=1080  A3,A4=2340
{0xB3,5,{0x04,0x38,0x09,0x24,0xC0}},
{0x00,1,{0xA6}},
{0xB3,1,{0xF8}},
{0x00,1,{0xCA}},
{0xC0,1,{0x80}},
{0x00,1,{0xE8}},
{0xC0,1,{0x40}},
{0x00,1,{0x85}},
{0xA7,1,{0x00}},
{0x00,1,{0xCC}},
{0xC0,1,{0x10}},
{0x00,1,{0x86}},
{0xC0,6,{0x01,0x07,0x01,0x01,0x1D,0x06}},
{0x00,1,{0x96}},
{0xC0,6,{0x01,0x07,0x01,0x01,0x1D,0x06}},
{0x00,1,{0xA6}},
{0xC0,6,{0x01,0x07,0x01,0x01,0x1D,0x06}},
{0x00,1,{0xD0}},
{0xC3,8,{0x45,0x00,0x00,0x05,0x45,0x00,0x00,0x05}},
{0x00,1,{0xE0}},
{0xC3,8,{0x45,0x00,0x00,0x05,0x45,0x00,0x00,0x05}},//TP monitor differ low
{0x00,1,{0x82}},
{0xA7,2,{0x33,0x02}},
{0x00,1,{0x80}},
{0xC2,16,{0x84,0x01,0x3A,0x3A,0x81,0x00,0x01,0x8F,0x82,0x00,0x01,0x8E,0x81,0x00,0x01,0x8F}},
{0x00,1,{0x90}},
{0xC2,16,{0x02,0x01,0x05,0x05,0x01,0x00,0x01,0x81,0x02,0x00,0x01,0x81,0x03,0x00,0x01,0x81}},
{0x00,1,{0xA0}},
{0xC2,15,{0x84,0x04,0x00,0x05,0x85,0x83,0x04,0x00,0x05,0x85,0x82,0x04,0x00,0x05,0x85}},
{0x00,1,{0xB0}},
{0xC2,15,{0x81,0x04,0x00,0x05,0x85,0x04,0x04,0x00,0x01,0x8E,0x05,0x04,0x00,0x01,0x90}},
{0x00,1,{0xE0}},
{0xC2,8,{0x33,0x33,0x43,0x77,0x00,0x00,0x00,0x00}},
{0x00,1,{0xE8}},
{0xC2,6,{0x12,0x00,0x05,0x02,0x05,0x05}},
{0x00,1,{0x80}},
{0xCB,16,{0xC1,0xC1,0x00,0xC1,0xC1,0x00,0x00,0xC1,0xFE,0x00,0xC1,0x00,0xFD,0xC1,0x00,0xC0}},
{0x00,1,{0x90}},
{0xCB,16,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xCB,4,{0x00,0x00,0x00,0x00}},
{0x00,1,{0xB0}},
{0xCB,4,{0x55,0x55,0x95,0x55}},
{0x00,1,{0x80}},
{0xCC,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x22,0x24,0x24,0x29,0x29,0x01,0x12,0x02,0x08,0x08}},
{0x00,1,{0x90}},
{0xCC,8,{0x06,0x06,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0x80}},
{0xCD,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x22,0x24,0x24,0x29,0x29,0x01,0x12,0x02,0x09,0x09}},
{0x00,1,{0x90}},
{0xCD,8,{0x07,0x07,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0xA0}},
{0xCC,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x24,0x23,0x24,0x29,0x29,0x01,0x02,0x12,0x07,0x07}},
{0x00,1,{0xB0}},
{0xCC,8,{0x09,0x09,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0xA0}},
{0xCD,16,{0x00,0x00,0x00,0x00,0x25,0x29,0x24,0x23,0x24,0x29,0x29,0x01,0x02,0x12,0x06,0x06}},
{0x00,1,{0xB0}},
{0xCD,8,{0x08,0x08,0x18,0x18,0x17,0x17,0x16,0x16}},
{0x00,1,{0x80}},
{0xC0,6,{0x00,0x7A,0x00,0x6C,0x00,0x10}},
{0x00,1,{0xA0}},
{0xC0,6,{0x01,0x09,0x00,0x3A,0x00,0x10}},
{0x00,1,{0xB0}},
{0xC0,5,{0x00,0x7A,0x02,0x11,0x10}},
{0x00,1,{0xC1}},
{0xC0,8,{0x00,0xB2,0x00,0x8C,0x00,0x77,0x00,0xD2}},
{0x00,1,{0xD7}},
{0xC0,6,{0x00,0x77,0x00,0x6E,0x00,0x10}},
{0x00,1,{0xA5}},
{0xC1,4,{0x00,0x2A,0x00,0x02}},
{0x00,1,{0x82}},
{0xCE,13,{0x01,0x09,0x00,0xD8,0x00,0xD8,0x00,0x90,0x00,0x90,0x0D,0x0E,0x09}},
{0x00,1,{0x90}},
{0xCE,8,{0x00,0x82,0x0D,0x5C,0x00,0x82,0x80,0x09}},
{0x00,1,{0xA0}},// 
{0xCE,3,{0x10,0x00,0x00}},
{0x00,1,{0xB0}},//rescan 2lane（与TCON default有差异）
{0xCE,3,{0x22,0x00,0x00}},
{0x00,1,{0xD1}},
{0xCE,7,{0x00,0x0A,0x01,0x01,0x00,0x5D,0x01}},
{0x00,1,{0xE1}},
{0xCE,11,{0x08,0x02,0x15,0x02,0x15,0x02,0x15,0x00,0x2B,0x00,0x5F}},
{0x00,1,{0xF1}},
{0xCE,9,{0x16,0x0B,0x0F,0x01,0x00,0x00,0xFE,0x01,0x0A}},
{0x00,1,{0xB0}},
{0xCF,4,{0x00,0x00,0x6C,0x70}},
{0x00,1,{0xB5}},
{0xCF,4,{0x04,0x04,0xA4,0xA8}},
{0x00,1,{0xC0}},
{0xCF,4,{0x08,0x08,0xCA,0xCE}},
{0x00,1,{0xC5}},
{0xCF,4,{0x00,0x00,0x08,0x0C}},
{0x00,1,{0x90}},
{0xC0,6,{0x00,0x7A,0x00,0x6C,0x00,0x10}},
{0x00,1,{0x80}},
{0xCE,2,{0x01,0x80}},
{0x00,1,{0x98}},
{0xCE,2,{0x00,0x04}},
{0x00,1,{0xC0}},
{0xCE,3,{0x00,0x00,0x00}},
{0x00,1,{0xD0}},
{0xCE,1,{0x91}},
{0x00,1,{0xE0}},
{0xCE,1,{0x88}},
{0x00,1,{0xF0}},
{0xCE,1,{0x80}},
{0x00,1,{0x82}},
{0xCF,1,{0x06}},
{0x00,1,{0x84}},
{0xCF,1,{0x06}},
{0x00,1,{0x87}},
{0xCF,1,{0x06}},
{0x00,1,{0x89}},
{0xCF,1,{0x06}},
{0x00,1,{0x8A}},
{0xCF,1,{0x07}},
{0x00,1,{0x8B}},
{0xCF,1,{0x00}},
{0x00,1,{0x8C}},
{0xCF,1,{0x06}},
{0x00,1,{0x92}},
{0xCF,1,{0x06}},
{0x00,1,{0x94}},
{0xCF,1,{0x06}},
{0x00,1,{0x97}},
{0xCF,1,{0x06}},
{0x00,1,{0x99}},
{0xCF,1,{0x06}},
{0x00,1,{0x9A}},
{0xCF,1,{0x07}},
{0x00,1,{0x9B}},
{0xCF,1,{0x00}},
{0x00,1,{0x9C}},
{0xCF,1,{0x06}},
{0x00,1,{0xA0}},
{0xCF,1,{0x24}},
{0x00,1,{0xA2}},
{0xCF,1,{0x06}},
{0x00,1,{0xA4}},
{0xCF,1,{0x06}},
{0x00,1,{0xA7}},
{0xCF,1,{0x06}},
{0x00,1,{0xA9}},
{0xCF,1,{0x06}},
{0x00,1,{0xB4}},
{0xCF,1,{0x00}},
{0x00,1,{0xC4}},
{0xCF,1,{0x00}},
{0x00,1,{0x82}},
{0xC5,2,{0x50,0x50}},
{0x00,1,{0x84}},
{0xC5,2,{0x32,0x32}},
{0x00,1,{0x00}},
{0xE1,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE2,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE3,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE4,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE5,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xE6,40,{0x06,0x06,0x09,0x0f,0x39,0x18,0x20,0x26,0x30,0xc8,0x38,0x3f,0x45,0x4a,0x05,0x4f,0x57,0x5f,0x66,0x20,0x6d,0x74,0x7c,0x84,0x63,0x8e,0x94,0x9b,0xa2,0x91,0xab,0xb6,0xc5,0xce,0xb6,0xda,0xec,0xf7,0xff,0xcf}},
{0x00,1,{0x00}},
{0xD8,2,{0x2B,0x2B}},
{0x00,1,{0x00}},
{0xD9,3,{0x00,0x8C,0x8C}},
{0x00,1,{0xA3}},
{0xC5,1,{0x1E}},
{0x00,1,{0xA9}},
{0xC5,1,{0x23}},
{0x00,1,{0x85}},
{0xC4,1,{0x1E}},
{0x00,1,{0x8C}},
{0xC3,3,{0x03,0x00,0x30}},
{0x00,1,{0x86}},
{0xC5,3,{0x00,0x60,0x0C}},
{0x00,1,{0x83}},
{0xA4,1,{0x23}},
{0x00,1,{0xB0}},
{0xF5,1,{0x00}},
{0x00,1,{0xC1}},
{0xB6,1,{0x09,0x89,0x68}},
{0x00,1,{0x89}},
{0xF5,1,{0x5A}},//VGH power off drop
{0x00,1,{0x96}},
{0xF5,1,{0x5A}},//VGH power off drop
{0x00,1,{0x80}},
{0xA7,1,{0x03}},//RGB-BGR
{0x00,1,{0xB0}},
{0xF3,2,{0x01,0xFE}},
{0x00,1,{0x00}},
{0xFF,3,{0xFF,0xFF,0xFF}},
{0x35, 1,{0x00}}, 
{0x11, 1,{0x00}},
{REGFLAG_DELAY, 120, {0}},
{0x29, 1,{0x00}},
{REGFLAG_DELAY, 20, {0}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
              
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
		       unsigned char force_update)
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

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
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

#ifndef BUILD_LK
    //5.2 18:9 59068 118136, 16:9 64754 115118
	params->physical_width               = 81;     //LCM_PHYSICAL_WIDTH/1000;
	params->physical_height              = 162;    //LCM_PHYSICAL_HEIGHT/1000;
	params->physical_width_um            = 80878;  //LCM_PHYSICAL_WIDTH; = sqrt((size*25.4)^2/(18^2+9^2))*9*1000
	params->physical_height_um           = 161755; //LCM_PHYSICAL_HEIGHT; = sqrt((size*25.4)^2/(18^2+9^2))*18*1000
	params->density                      = 480;    //LCM_DENSITY;
#endif

    // enable tearing-free
    params->dbi.te_mode                  = LCM_DBI_TE_MODE_VSYNC_ONLY;
    params->dbi.te_edge_polarity         = LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////
#endif

    // DSI
    /* Command mode setting */
    //1 Three lane or Four lane
    params->dsi.LANE_NUM                 = LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order  = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq    = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding      = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format       = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS                       = LCM_PACKED_PS_24BIT_RGB888;

#if (LCM_DSI_CMD_MODE)
    params->dsi.intermediat_buffer_num   = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
    params->dsi.word_count               = FRAME_WIDTH * 3; //DSI CMD mode need set these two bellow params, different to 6577
#else
    params->dsi.intermediat_buffer_num   = 2;	//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
#endif

    // Video mode setting
    params->dsi.packet_size              = 256;

    params->dsi.vertical_sync_active     = 4;
    params->dsi.vertical_backporch       = 12;
    params->dsi.vertical_frontporch      = 112;
    params->dsi.vertical_active_line     = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active   = 4;
    params->dsi.horizontal_backporch     = 16;
    params->dsi.horizontal_frontporch    = 16;
    params->dsi.horizontal_active_pixel  = FRAME_WIDTH;
    /* params->dsi.ssc_disable = 1; */

    params->dsi.PLL_CLOCK                = 500;

	//params->dsi.cont_clock = 1;
	//params->dsi.clk_lp_per_line_enable = 0;//
	//params->dsi.ssc_disable = 1;	
	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
}

static void lcm_set_bias(int enable)
{
	
	if (enable){
		display_bias_vpos_set(5800);
		display_bias_vneg_set(5800);
		display_bias_enable();
	}else{
		display_bias_disable();
		
	}
}

#ifndef BUILD_LK
extern atomic_t ESDCheck_byCPU;
#endif
static unsigned int lcm_ata_check(unsigned char *buffer)
{
#if 1
#ifndef BUILD_LK 
unsigned int ret = 0 ,ret1=2; 
//unsigned int x0 = FRAME_WIDTH/4; 
//unsigned int x1 = FRAME_WIDTH*3/4; 
//unsigned int y0 = 0;
//unsigned int y1 = y0 + FRAME_HEIGHT - 1;
unsigned char x0_MSB = 0x5;//((x0>>8)&0xFF); 
unsigned char x0_LSB = 0x2;//(x0&0xFF); 
unsigned char x1_MSB = 0x1;//((x1>>8)&0xFF); 
unsigned char x1_LSB = 0x4;//(x1&0xFF); 
//	unsigned char y0_MSB = ((y0>>8)&0xFF);
//	unsigned char y0_LSB = (y0&0xFF);
//	unsigned char y1_MSB = ((y1>>8)&0xFF);
//	unsigned char y1_LSB = (y1&0xFF);
unsigned int data_array[6]; 
unsigned char read_buf[4]; 
unsigned char read_buf1[4]; 
unsigned char read_buf2[4]; 
unsigned char read_buf3[4]; 
#ifdef BUILD_LK 
printf("ATA check size = 0x%x,0x%x,0x%x,0x%x\n",x0_MSB,x0_LSB,x1_MSB,x1_LSB); 
#else 
printk("ATA check size = 0x%x,0x%x,0x%x,0x%x\n",x0_MSB,x0_LSB,x1_MSB,x1_LSB); 
#endif 
data_array[0]= 0x0002390A;//HS packet 
data_array[1]= 0x00002453; 
dsi_set_cmdq(data_array, 2, 1); 
 data_array[0]= 0x0002390A;//HS packet 
data_array[1]= 0x0000F05e; 
dsi_set_cmdq(data_array, 2, 1); 
data_array[0]= 0x0002390A;//HS packet 
data_array[1]= 0x00000355; 
dsi_set_cmdq(data_array, 2, 1); 
//data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x51; 
//data_array[2]= (x1_LSB); 
//dsi_set_cmdq(data_array, 3, 1); 
data_array[0] = 0x00013700; 
dsi_set_cmdq(data_array, 1, 1); 
atomic_set(&ESDCheck_byCPU, 1);
read_reg_v2(0X56, read_buf1, 1); 
read_reg_v2(0X54, read_buf2, 1); 
read_reg_v2(0X5F, read_buf3, 1);
atomic_set(&ESDCheck_byCPU, 0);
if((read_buf1[0] == 0x03)&& (read_buf2[0] == 0x24) && (read_buf3[0] == 0xf0)) 
    ret = 1; 
else 
    ret = 0; 
#ifdef BUILD_LK 
printf("ATA read buf size = 0x%x,0x%x,0x%x,0x%x,ret= %d\n",read_buf[0],read_buf[1],read_buf[2],read_buf[3],ret); 
#else 
printk("ATA read buf  size = 0x%x,0x%x,0x%x,0x%x,ret= %d ret1= %d\n",read_buf[0],read_buf1[0],read_buf2[0],read_buf3[0],ret,ret1); 
printk("ATA read buf new  size = 0x%x,0x%x,0x%x,0x%x,ret= %d ret1= %d\n",read_buf1[0],read_buf1[1],read_buf1[2],read_buf1[3],ret,ret1); 
#endif 
return ret; 
#endif //BUILD_LK
#endif
}

static void lcm_init(void)
{
	display_ldo18_enable(1);
	MDELAY(10);
	lcm_set_bias(1);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(250);
//	lcm_compare_id();
//    init_lcm_registers();
	push_table(lcm_initialization_setting,
		   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	//push_table(lcm_suspend_setting,
		   //sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
    unsigned int data_array[16];
    
	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);

	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);

	SET_RESET_PIN(0);
	MDELAY(10);
	
	lcm_set_bias(0);
	display_ldo18_enable(0);
}

static void lcm_resume(void)
{
	lcm_init();
}

#if (LCM_DSI_CMD_MODE)
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
#endif

#define LCM_ID_NT35532 (0x32)

static unsigned int lcm_compare_id(void)
{
	return 1;

}

LCM_DRIVER ft8719_fhdpluse2340_dsi_vdo_auo_lcm_drv = {
	.name		= "ft8719_fhdpluse2340_dsi_vdo_auo",
    	//prize-lixuefeng-20150512-start
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "ft8719_auo",
		.vendor	= "unknow",
		.id		= "0x8719",
		.more	= "2340x1080",
	},
	#endif
	//prize-lixuefeng-20150512-end	
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id 	= lcm_compare_id,
	//.esd_check = lcm_esd_check,
    #if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
    #endif
    .ata_check	= lcm_ata_check,
};

