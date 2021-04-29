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
#define LCM_DSI_CMD_MODE	1
#define FRAME_WIDTH  										1080
#define FRAME_HEIGHT 										2340

//#ifndef GPIO_LCD_BIAS_ENP_PIN
#define GPIO_LCD_BIAS_ENP (GPIO127 | 0x80000000)  //prize add 
//#endif
//#ifndef GPIO_LCD_BIAS_ENN_PIN
#define GPIO_LCD_BIAS_ENN (GPIO202  | 0x80000000)//prize add 
//#endif
//#ifndef GPIO_LCD_RESET
#define GPIO_LCD_RST (GPIO44  | 0x80000000)//prize add 
//#endif

#define GPIO_BUS_FD_EN_PIN (GPIO166  | 0x80000000)//prize add 

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
	unsigned char para_list[80];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
#if (!LCM_DSI_CMD_MODE)
//	{0xB0,1,{0x00}},
//	{0xB3,1,{0x01}},
//	{0xB0,1,{0x80}},
#endif
	{0xFE,1,{0x00}},
	{0xC2,1,{0x08}},
	//{0x36,1,{0x02}},
	{0x51,2,{0x00,0x00}},//prize-add wyq 20181226 set backlight on after logo is ready, to fix abnomal display issue when boot up   //{0x51,2,{0x0F,0xFF}}
	{0x35,1,{0x00}},

	//prize-mod wyq 20190611 color tuning-start
	//{0xB0,1,{0x04}},
	//{0xCD,8,{0x1D,0x00,0xEA,0xED,0xFC,0xEA,0x29,0x2B}},
	//{0xB0,1,{0x80}},
	//{0xE6,1,{0x00}},
	//prize-mod wyq 20190611 color tuning-end
	{0x11,1,{0x00}},
	{REGFLAG_DELAY,150,{}},
	{0x29,1,{0x00}},

	//{REGFLAG_DELAY, 60, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}    
              
};


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
	
		params->dsi.vertical_sync_active				=  2;
		params->dsi.vertical_backporch					= 4;//16 25 30 35 12 8
		params->dsi.vertical_frontporch					= 25;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active = 10;
		params->dsi.horizontal_backporch = 60;//32
		params->dsi.horizontal_frontporch = 105;//78
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;
		/* params->dsi.ssc_disable														 = 1; */
	
#ifndef MACH_FPGA
#if (LCM_DSI_CMD_MODE)
		params->dsi.PLL_CLOCK = 550;	/* this value must be in MTK suggested table */
#else
		params->dsi.PLL_CLOCK = 630;	/* this value must be in MTK suggested table */
#endif
#else
		params->dsi.pll_div1 = 0;
		params->dsi.pll_div2 = 0;
		params->dsi.fbk_div = 0x1;
#endif

		params->dsi.ssc_disable = 0;
		params->dsi.ssc_range = 1;
		params->dsi.cont_clock = 1;
		params->dsi.clk_lp_per_line_enable = 1;


	
}

#if 0
#define RT5081_SLAVE_ADDR_WRITE  0x7C

static int RT5081_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer)
{
	kal_uint32 ret = I2C_OK;
	kal_uint16 len;
	struct mt_i2c_t RT5081_i2c;
	*dataBuffer = addr;

	RT5081_i2c.id = I2C_MT6370_PMU_CHANNEL;
	RT5081_i2c.addr = I2C_MT6370_PMU_SLAVE_7_BIT_ADDR;
	RT5081_i2c.mode = ST_MODE;
	RT5081_i2c.speed = 100;
	len = 1;

	ret = i2c_write_read(&RT5081_i2c, dataBuffer, len, len);
	if (I2C_OK != ret)
		LCM_LOGI("%s: i2c_read  failed! ret: %d\n", __func__, ret);

	return ret;
}

static int RT5081_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;
	struct mt_i2c_t RT5081_i2c;

	write_data[0] = addr;
	write_data[1] = value;

	RT5081_i2c.id = I2C_MT6370_PMU_CHANNEL;
	RT5081_i2c.addr = I2C_MT6370_PMU_SLAVE_7_BIT_ADDR;
	RT5081_i2c.mode = ST_MODE;
	RT5081_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&RT5081_i2c, write_data, len);

	return ret_code;
}

static int RT5081_REG_MASK (kal_uint8 addr, kal_uint8 val, kal_uint8 mask)
{
	kal_uint8 RT5081_reg = 0;
	kal_uint32 ret = 0;

	ret = RT5081_read_byte(addr, &RT5081_reg);

	RT5081_reg &= ~mask;
	RT5081_reg |= val;

	ret = RT5081_write_byte(addr, RT5081_reg);

	return ret;
}

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
#endif

#define GPIO_TP_RESET (GPIO56 | 0x80000000)

static void lcm_power_on(void)
{
	kal_uint32 ret1 = I2C_OK;

		SET_RESET_PIN(0);
	LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
#if 0	
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
	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
	ret1 = RT5081_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);
#endif
	mt_set_gpio_mode(GPIO_BUS_FD_EN_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_BUS_FD_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_BUS_FD_EN_PIN, GPIO_OUT_ONE);

	MDELAY(15);
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);//100

	SET_RESET_PIN(1);
	MDELAY(120);//250
}

static unsigned int lcm_compare_id(void)
{
	
    unsigned int id0,id1,id=0;
	unsigned char buffer[2];
	unsigned int array[16];  

	lcm_power_on();
	
   

	 array[0] = 0x00023700; // read id return two byte,version and id
    dsi_set_cmdq(array, 1, 1);
    
    read_reg_v2(0xA1, buffer, 2);
    MDELAY(10);
	
    #ifdef BUILD_LK
	printf("%s, LK TDDI id = 0x%08x\n", __func__, buffer[0]);
   #else
	printk("%s, Kernel TDDI id = 0x%08x\n", __func__, buffer[0]);
   #endif
   return 0;
   return (0x01 == buffer[0])?1:0; 

}

static void lcm_init(void)
{
	kal_uint32 ret1 = I2C_OK;

	//	SET_RESET_PIN(0);
	LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
	#if 0
	/*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
	ret1 = RT5081_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

	/* set AVDD 5.4v, (4v+28*0.05v) */
	/*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
	ret1 = RT5081_REG_MASK(0xB3, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

	/* set AVEE */
	/*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
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
#endif

	mt_set_gpio_mode(GPIO_BUS_FD_EN_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_BUS_FD_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_BUS_FD_EN_PIN, GPIO_OUT_ONE);

	MDELAY(15);
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);//100

	SET_RESET_PIN(1);
	MDELAY(12);//250
	push_table(lcm_initialization_setting,
		   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
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
	     
#if 0
	ret1 = RT5081_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);
#endif
	MDELAY(5);

	MDELAY(2);

	SET_RESET_PIN(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}

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
	/*BEGIN PN:DTS2013013101431 modified by s00179437 , 2013-01-31 */
	/* delete high speed packet */
	/* data_array[0]=0x00290508; */
	/* dsi_set_cmdq(data_array, 1, 1); */
	/*END PN:DTS2013013101431 modified by s00179437 , 2013-01-31 */

	data_array[0] = 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
}

//prize-add wyq 20181226 set backlight on after logo is ready, to fix abnomal display issue when boot up-start
static struct LCM_setting_table bl_level[] = {
    //{0xFE,1,{0x00}},
	{0x51, 2, {0x07,0xFF} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{

	LCM_LOGI("%s,nt35695 backlight: level = %d\n", __func__, level);
	level = level*8;

	bl_level[0].para_list[0] = (level>>8)&0xff;
	bl_level[0].para_list[1] = (level)&0xff;

	push_table(bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}
//prize-add wyq 20181226 set backlight on after logo is ready, to fix abnomal display issue when boot up-start

LCM_DRIVER r692c9_fhdp_dsi_vdo_visionox_lcm_drv = {
    .name		= "r692c9_fhdp_dsi_vdo_visionox_lcm_drv",
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "r692c9",
		.vendor	= "Synaptics",
		.id		= "0x80",
		.more	= "1080*2340",
	},
	#endif
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
/* prize add by lifenfen, modified for set_backlight_cmdq interface, 20190415 begin */
/*
	.set_backlight = lcm_setbacklight_cmdq,
*/
	.set_backlight_cmdq = lcm_setbacklight_cmdq,
/* prize add by lifenfen, modified for set_backlight_cmdq interface, 20190415 end */
#if (LCM_DSI_CMD_MODE)
	.update = lcm_update,
#endif
};
