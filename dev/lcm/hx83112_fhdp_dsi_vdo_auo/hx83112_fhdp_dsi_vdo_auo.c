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
#include <mach/mt_pm_ldo.h>
#include <mach/mt_gpio.h>
#endif
/*prize-add-for round corner-houjian-20180918-start*/
#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_rgba4444_roundedpattern.h"
#endif
/*prize-add-for round corner-houjian-20180918-end*/

//#include <cust_gpio_usage.h>

#ifndef MACH_FPGA
//#include <cust_i2c.h>
#endif

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

#define I2C_I2C_LCD_BIAS_CHANNEL 0
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

static const unsigned char LCD_MODULE_ID = 0x01;
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE	0
#define FRAME_WIDTH  										1080
#define FRAME_HEIGHT 										2340

//#ifndef GPIO_LCD_BIAS_ENP_PIN
#define GPIO_LCD_BIAS_ENP (GPIO109 | 0x80000000)
#define GPIO_TP_RST (GPIO15 | 0x80000000)
//#endif
//#ifndef GPIO_LCD_BIAS_ENN_PIN
#define GPIO_LCD_BIAS_ENN (GPIO23  | 0x80000000)
//#define GPIO_LCM_LED_EN (GPIO153  | 0x80000000)
//#endif
//#ifndef GPIO_LCD_RESET
#define GPIO_LCD_RST (GPIO45  | 0x80000000)
//#endif
//prize-penggy modify LCD size-20190328-start
#define LCM_PHYSICAL_WIDTH                  				(67070)
#define LCM_PHYSICAL_HEIGHT                  				(145310)
//prize-penggy modify LCD size-20190328-end
#define GPIO_65132_ENP  GPIO_LCD_BIAS_ENP
#define GPIO_65132_ENN  GPIO_LCD_BIAS_ENN
#define GPIO_LCM_RESET  GPIO_LCD_RST



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
/*prize penggy add-for LCD ESD-20190228-start*/
/*
static struct LCM_setting_table lcm_initialization_setting[] = {

{0xB9,3,{0x83,0x11,0x2A}},
{0xB1,8,{0x08,0x28,0x28,0x83,0x83,0x4C,0x4F,0x33}},
{0xB2,14,{ 0x00, 0x02, 0x00, 0x90, 0x24, 0x00, 0x08, 0x19, 0xEA, 0x11, 0x11, 0x00, 0x11, 0xA3}},
{0xB4,27,{0x58 ,0x68 ,0x58 ,0x68 ,0x0F ,0xEF ,0x0B ,0xC0 ,0x0B ,0xC0 ,0x0B ,0xC0 ,0x00 ,0xFF ,0x00 ,0xFF ,0x00 ,0x00 ,0x14 ,0x15 ,0x00 ,0x29 ,0x11 ,0x07 ,0x12 ,0x00 ,0x29}},
{0xBD,1,{0x02}},
{0xB4,9,{0x00,0x12,0x12, 0x11, 0x88, 0x12, 0x12, 0x00, 0x53}},
{0xBD,1,{0x00}},

{0xB6,3,{0x83 ,0x83 ,0xE3}},
{0xBD,1,{0x03}},

{0xC1,59,{0xFF ,0xFA ,0xF6 ,0xF2 ,0xEE ,0xEA ,0xE6 ,0xDE,0xDA, 0xD5,0xD2,0xCD,0xC9,0xC5,0xC1,0xBD
			,0xB9,0xB4,0xB0,0xA8,0x9F,0x97,0x8F,0x87,0x7F,0x77,0x6F
			,0x67,0x5F,0X57,0x4F,0x48,0x41,0x39,0x31,0x29, 0x21, 0x1A, 0x12, 0x0B, 0x07, 0x06, 0x03, 0x01, 0x00, 0x39, 0x10, 0x33, 0x41, 0x24, 0xA5, 0x69, 0x78, 0x26, 0xDC, 0x46, 0x00}},

{0xBD,1,{0x02}},

{0xC1,59,{0xFF ,0xFA ,0xF6 ,0xF2 ,0xEE ,0xEA ,0xE6 ,0xDE,0xDA, 0xD5,0xD2,0xCD,0xC9,0xC5,0xC1,0xBD
			,0xB9,0xB4,0xB0,0xA8,0x9F,0x97,0x8F,0x87,0x7F,0x77,0x6F
			,0x67,0x5F,0X57,0x4F,0x48,0x41,0x39,0x31,0x29, 0x21, 0x1A, 0x12, 0x0B, 0x07, 0x06, 0x03, 0x01, 0x00, 0x39, 0x10, 0x33, 0x41, 0x24, 0xA5, 0x69, 0x78, 0x26, 0xDC, 0x46, 0x00}},


{0xBD,1,{0x01}},

{0xC1,59,{0xFF ,0xFA ,0xF6 ,0xF2 ,0xEE ,0xEA ,0xE6 ,0xDE,0xDA, 0xD5,0xD2,0xCD,0xC9,0xC5,0xC1,0xBD
			,0xB9,0xB4,0xB0,0xA8,0x9F,0x97,0x8F,0x87,0x7F,0x77,0x6F
			,0x67,0x5F,0X57,0x4F,0x48,0x41,0x39,0x31,0x29, 0x21, 0x1A, 0x12, 0x0B, 0x07, 0x06, 0x03, 0x01, 0x00, 0x39, 0x10, 0x33, 0x41, 0x24, 0xA5, 0x69, 0x78, 0x26, 0xDC, 0x46, 0x00}},


{0xBD,1,{0x00}},


{0xC1,1,{0x01}},

{0xC7,6,{0x70 ,0x00 ,0x04 ,0xE0 ,0x33 ,0x00}},


{0xCC,1,{0x08}},


{0xD2,2,{0x2B, 0x2B}},

{0xD3,49,{0xC0,0x00,0x00,0x00,0x00,0x01,0x00,0x08,0x08,0x03,0x03,0x22,0x18,0x07,0x07,0x07,0x07,0x32,0x10,0x06,0x00,0x06,0x32,0x10,0x07,0x00,0x07,0x32,0x19,0x31,0x09,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x09,0x30,0x00,0x00,0x00,0x06,0x0D,0x00,0x0F}},
{0xBD,1,{0x01}},

{0xD3,8,{0x00,0x00,0x19,0x10,0x00,0x0A,0x00,0x81}},
{0xBD,1,{0x00}},


{0xD5,48,{0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xC0,0xC0,0x18,0x18,0x19,0x19,0x18,0x18,0x40,0x40,0x18,0x18,0x18,0x18,0x3F,0x3F,0x28,0x28,0x24,0x24,0x02,0x03,0x02,0x03,0x00,0x01,0x00,0x01,0x31,0x31,0x31,0x31,0x30,0x30,0x30,0x30,0x2F,0x2F,0x2F,0x2F}},
{0xD6,48,{0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0xC0,0xC0,0x18,0x18,0x19,0x19,0x18,0x18,0x40,0x40,0x18,0x18,0x18,0x18,0x3F,0x3F,0x28,0x28,0x24,0x24,0x02,0x03,0x02,0x03,0x00,0x01,0x00,0x01,0x31,0x31,0x31,0x31,0x30,0x30,0x30,0x30,0x2F,0x2F,0x2F,0x2F}},

{0xD8,24,{0xAA,0xEA,0xAA,0xAA,0xAA,0xAA,0xAA,0xEA,0xAA,0xAA,0xAA,0xAA,0xAA,0xEA,0xAB,0xAA,0xAA,0xAA,0xAA,0xEA,0xAB,0xAA,0xAA,0xAA}},
{0xBD,1,{0x01}},

{0xD8,24,{0xAA,0x2E,0x28,0x00,0x00,0x00,0xAA,0x2E,0x28,0x00,0x00,0x00,0xAA,0xEE,0xAA,0xAA,0xAA,0xAA,0xAA,0xEE,0xAA,0xAA,0xAA,0xAA}},
{0xBD,1,{0x02}},


{0xD8,12,{0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF}},
{0xBD,1,{0x03}},


{0xD8,24,{0xAA,0xAA,0xEA,0xAA,0xAA,0xAA,0xAA,0xAA,0xEA,0xAA,0xAA,0xAA,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF,0xAA,0xFF,0xFF,0xFF,0xFF,0xFF}},
{0xBD,1,{0x00}},

{0xE7,23,{0x0E,0x0E,0x1E,0x65,0x1C,0x65,0x00,0x50,0x20,0x20,0x00,0x00,0x02,0x02,0x02,0x05,0x14,0x14,0x32,0xB9,0x23,0xB9,0x08}},
{0xBD,1,{0x01}},

{0xE7,8,{0x02,0x00,0xA8,0x01,0xA8,0x0D,0xA4,0x0E}},
{0xBD,1,{0x02}},

{0xE7,29,{0x00,0x00,0x08,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x02,0x00}},
{0xBD,1,{0x00}},

{0xE9,1,{0xC3}},

{0xCB,2,{0xD1,0xD6}},

{0xE9,1,{0x3F}},


{0xB9,3,{0x83,0x11,0x2A}},

//prize penggy add-for LCD ESD-20190219-start//
{0xCD,1,{0x01}},
//prize penggy add-for LCD ESD-20190219-end//
	{0x11,1,{0x00}},
	{REGFLAG_DELAY,15,{}},


	{0x29,1,{0x00}},
		{REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}    
              
};
*/
/*prize penggy add-for LCD ESD-20190228-end*/

static void push_table(struct LCM_setting_table *table, unsigned int count,
		       unsigned char force_update)
{
	unsigned int i;
    LCM_LOGI("nt35695----tps6132-lcm_init   push_table++++++++++++++===============================devin----\n");
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
	
			// enable tearing-free
			params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
			params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
        #if (LCM_DSI_CMD_MODE)
			params->dsi.mode   = CMD_MODE;
		params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
        #else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////
        #endif
		
			// DSI
			/* Command mode setting */
			//1 Three lane or Four lane
			params->dsi.LANE_NUM				= LCM_FOUR_LANE;
			//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
		
		
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
#if (LCM_DSI_CMD_MODE)
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
		params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
#else
		params->dsi.intermediat_buffer_num = 0; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
#endif
	
		// Video mode setting
		params->dsi.packet_size=256;
	
	params->dsi.vertical_sync_active				=  5;
	params->dsi.vertical_backporch					= 5;//16 25 30 35 12 8
	params->dsi.vertical_frontporch					= 27;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active = 8;
	params->dsi.horizontal_backporch = 8;//32
	params->dsi.horizontal_frontporch = 28;//78
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;
	/* params->dsi.ssc_disable                                                       = 1; */

	params->dsi.PLL_CLOCK = 480;//244;
		params->dsi.ssc_disable = 0;
		params->dsi.ssc_range = 1;
		params->dsi.cont_clock = 0;
		params->dsi.clk_lp_per_line_enable = 0;
//prize-penggy modify LCD size-20190328-start
	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
    params->physical_height = LCM_PHYSICAL_HEIGHT/1000;
//prize-penggy modify LCD size-20190328-end
//   params->physical_width_um = LCM_PHYSICAL_WIDTH;
//    params->physical_height_um = LCM_PHYSICAL_HEIGHT;
/*prize penggy add-for LCD ESD-20190228-start*/
	#if 1
/*prize penggy add-for LCD ESD-20190219-end*/	
		params->dsi.ssc_disable = 1;
		params->dsi.lcm_ext_te_monitor = FALSE;
		
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
			params->dsi.lcm_esd_check_table[0].cmd					= 0x0a;
			params->dsi.lcm_esd_check_table[0].count			= 1;
			params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9d;
			params->dsi.lcm_esd_check_table[1].cmd					= 0x09;
			params->dsi.lcm_esd_check_table[1].count			= 4;
			params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
			params->dsi.lcm_esd_check_table[1].para_list[1] = 0x73;
			params->dsi.lcm_esd_check_table[1].para_list[2] = 0x04;
			params->dsi.lcm_esd_check_table[1].para_list[3] = 0x00;
		#endif
		/*prize-add-for round corner-houjian-20180918-start*/
		#ifdef MTK_ROUND_CORNER_SUPPORT
			params->round_corner_params.w = ROUND_CORNER_W;
			params->round_corner_params.h = ROUND_CORNER_H;
			params->round_corner_params.lt_addr = left_top;
			params->round_corner_params.rt_addr = right_top;
			params->round_corner_params.lb_addr = left_bottom;
			params->round_corner_params.rb_addr = right_bottom;
		#endif
		/*prize-add-for round corner-houjian-20180918-end*/
	
	}
/*prize penggy add-for LCD ESD-20190228-start*/
void init_lcm_registers(void)
{
unsigned int data_array[16];

data_array[0] = 0x00043902;
data_array[1] = 0x2A1183B9;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);

data_array[0] = 0x00023902;
data_array[1] = 0x000021CD;    //CRC --ESD-20190219-end*/
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(150);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);
}
/*prize penggy add-for LCD ESD-20190228-end*/

#ifdef BUILD_LK
#define TPS65132_SLAVE_ADDR_WRITE  0x7C
static struct mt_i2c_t tps65132_i2c;

static int tps65132_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0] = addr;
	write_data[1] = value;

	tps65132_i2c.id = I2C_I2C_LCD_BIAS_CHANNEL; /* I2C2; */
	/* Since i2c will left shift 1 bit, we need to set FAN5405 I2C address to >>1 */
	tps65132_i2c.addr = (TPS65132_SLAVE_ADDR_WRITE >> 1);
	tps65132_i2c.mode = ST_MODE;
	tps65132_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&tps65132_i2c, write_data, len);
	/* printf("%s: i2c_write: ret_code: %d\n", __func__, ret_code); */

	return ret_code;
}

#endif

static void lcm_set_bias(void)
{
	unsigned char cmd = 0x00;
	unsigned char data = 0x14;
	
	int ret = 0;
	
	mt_set_gpio_mode(GPIO_65132_ENN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENN, GPIO_OUT_ONE);

	MDELAY(30);
	mt_set_gpio_mode(GPIO_65132_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENP, GPIO_OUT_ONE);
	MDELAY(50);

//	dprintf(0,"lcm_set_bias===========================\n");

	cmd = 0x03;
	data = 0x42;
	ret = tps65132_write_byte(cmd, data);

	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", cmd);
	else
		LCM_LOGI("pzp nt35695----tps6132----cmd=%0x--i2c write success----\n", cmd);
	
	
	cmd = 0x00;
	data = 0x14;
	ret = tps65132_write_byte(cmd, data);

	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", cmd);
	else
		LCM_LOGI("pzp nt35695----tps6132----cmd=%0x--i2c write success----\n", cmd);

	cmd = 0x01;
	data = 0x14;//0E

	ret = tps65132_write_byte(cmd, data);

	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", cmd);
	else
		LCM_LOGI("pzp nt35695----tps6132----cmd=%0x--i2c write success----\n", cmd);

}
#define GPIO_TP_RESET (GPIO56 | 0x80000000)

static void lcm_power_on(void)
{

	lcm_set_bias();
}

static unsigned int lcm_compare_id(void)
{
	
    unsigned int id0,id1,id=0;
	unsigned char buffer[4] ={0};
	unsigned int array[16];
	lcm_power_on();
	
   

	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04, buffer, 1);
	
    //#ifdef BUILD_LK
	dprintf("##################LK TDDI id = 0x%08x 0x%08x 0x%08x\n",buffer[0],buffer[1],buffer[2]);
   //#else
	//printk("%s, Kernel TDDI id = 0x%08x\n", __func__, buffer[0]);
   //#endif

   return (0x83 == buffer[0])?1:0; 

}

static void lcm_init(void)
{
	//LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
	kal_uint32 ret1 = I2C_OK;

//		SET_RESET_PIN(0);
	LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
	lcm_power_on();
/*	
	ret1 = RT5081_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));
	ret1 = RT5081_REG_MASK(0xB3, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);
	ret1 = RT5081_REG_MASK(0xB4, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB4);

	
	ret1 = RT5081_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);
*/
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP, GPIO_OUT_ONE);
	
	display_bias_enable();
/*prize penggy add-for LCD ESD-20190228-start*/
	mt_set_gpio_mode(GPIO_TP_RST, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_TP_RST, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_TP_RST, GPIO_OUT_ONE);
	
	MDELAY(15);
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);//100

	SET_RESET_PIN(1);
	MDELAY(120);//250
	

	
    init_lcm_registers();
/*prize penggy add-for LCD ESD-20190228-end*/
	//push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	//mt_set_gpio_mode(GPIO_LCM_LED_EN, GPIO_MODE_00);
	//mt_set_gpio_dir(GPIO_LCM_LED_EN, GPIO_DIR_OUT);
	//mt_set_gpio_out(GPIO_LCM_LED_EN, GPIO_OUT_ZERO);
}

static void lcm_suspend(void)
{

     unsigned int data_array[16];
	kal_uint32 ret1 = I2C_OK;
	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);

	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);
	     
	
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP, GPIO_OUT_ZERO);
	display_bias_disable();
	MDELAY(2);

	SET_RESET_PIN(0);
	MDELAY(10);
	//mt_set_gpio_mode(GPIO_LCM_LED_EN, GPIO_MODE_00);
	//mt_set_gpio_dir(GPIO_LCM_LED_EN, GPIO_DIR_OUT);
	//mt_set_gpio_out(GPIO_LCM_LED_EN, GPIO_OUT_ONE);
}

static void lcm_resume(void)
{
	lcm_init();
}




LCM_DRIVER hx83112_fhdp_dsi_vdo_auo_drv = 
{
    .name			= "hx83112_fhdp_dsi_vdo_auo_auo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,

};
