#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
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
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1560)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER
#define HX8394F_HD720_ID  (0x8394)
#define LCM_DSI_CMD_MODE									0
#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif
#define LOG_TAG "LCM"
#define I2C_I2C_LCD_BIAS_CHANNEL 0
//#ifndef GPIO_LCD_BIAS_ENP_PIN
#define GPIO_LCD_BIAS_ENP (GPIO109 | 0x80000000)
//#endif
//#ifndef GPIO_LCD_BIAS_ENN_PIN
#define GPIO_LCD_BIAS_ENN (GPIO23  | 0x80000000)
//#endif
//#ifndef GPIO_LCD_RESET
#define GPIO_LCD_RST (GPIO45  | 0x80000000)
//#endif

#define GPIO_LCD_LDO1V8 (GPIO27  | 0x80000000)

#define GPIO_65132_ENP  GPIO_LCD_BIAS_ENP
#define GPIO_65132_ENN  GPIO_LCD_BIAS_ENN
#define GPIO_LCM_RESET  GPIO_LCD_RST

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

#define I2C_I2C_LCD_BIAS_CHANNEL 0

static LCM_UTIL_FUNCS lcm_util = {0};
#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
 struct LCM_setting_table {
	unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};
static unsigned int lcm_compare_id(void);

static struct LCM_setting_table lcm_initialization_setting[] = {
	
{0xB9,3,{0xFF,0x83,0x94}},
{0xB1,10,{0x48,0x19,0x79,0x09,0x32,0x44,0x71,0x31,0x4D,0x30}},
{0xBA,6,{0x63,0x03,0x68,0x6B,0xB2,0xC0}},
{0xB2,5,{0x00,0x80,0x87,0x0C,0x07}},
{0xB4,21,{0x17,0x60,0x17,0x60,0x17,0x60,0x01,0x0C,0x70,0x55,
	        0x00,0x3F,0x01,0x6B,0x01,0x6B,0x01,0x6B,0x01,0x0C,0x7C}},
{0xD3,33,{0x00,0x00,0x00,0x00,0x3C,0x1C,0x00,0x00,0x32,0x10,
	        0x09,0x00,0x09,0x32,0x16,0x25,0x06,0x25,0x32,0x00,
	        0x00,0x00,0x00,0x37,0x03,0x0B,0x0B,0x37,0x00,0x00,0x00,0x0C,0x40}},

{0xD5,44,{0x19,0x19,0x18,0x18,0x1B,0x1B,0x1A,0x1A,0x00,0x01,
	        0x02,0x03,0x04,0x05,0x06,0x07,0x20,0x21,0x18,0x18,
	        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	        0x24,0x25,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	        0x18,0x18,0x18,0x18}},

{0xD6,44,{0x18,0x18,0x19,0x19,0x1B,0x1B,0x1A,0x1A,0x07,0x06,
	        0x05,0x04,0x03,0x02,0x01,0x00,0x25,0x24,0x18,0x18,
	        0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	        0x21,0x20,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
	        0x18,0x18,0x18,0x18}},

{0xE0,58,{0x00,0x09,0x16,0x20,0x24,0x2A,0x2F,0x2E,0x62,0x75,
					0x85,0x84,0x8A,0x98,0x97,0x97,0xA2,0xA2,0x9D,0xAA,
					0xB9,0x5B,0x59,0x5D,0x60,0x63,0x6A,0x76,0x7F,0x00,
					0x09,0x16,0x1F,0x24,0x2A,0x2F,0x2E,0x62,0x75,0x85,
					0x83,0x89,0x97,0x96,0x97,0xA2,0xA2,0x9D,0xAA,0xB9,
          0x5B,0x59,0x5D,0x60,0x63,0x6A,0x76,0x7F}},

{0xCC,1,{0x0B}},
{0xC0,2,{0x1F,0x31}},
{0xB6,2,{0x75,0x75}},
{0xD4,1,{0x02}},
{0xBD,1,{0x01}},
{0xB1,1,{0x00}},
{0xBD,1,{0x00}},
{0xC6,1,{0xED}},
{0xD2,1, {0xCC}},
 {0x11,0, {0x00}},
 {REGFLAG_DELAY, 120, {}},
 {0x29,0, {0x00}},
 {REGFLAG_DELAY, 20, {}},
 
	
 {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
 {0x11, 1, {0x00}},
 {REGFLAG_DELAY, 120, {}},

 // Display ON
 {0x29, 1, {0x00}},
 {REGFLAG_DELAY, 20, {}},
 {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_in_setting[] = {
 // Display off sequence
 {0x28, 1, {0x00}},
 {REGFLAG_DELAY, 50, {}},

 // Sleep Mode On
 {0x10, 1, {0x00}},
 {REGFLAG_DELAY, 120, {}},
 {0xB1, 1, {0x01}},
 {REGFLAG_DELAY, 50, {}},
 {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;
    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}
// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
		
	params->type   = LCM_TYPE_DSI;
	
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	
    params->physical_width               = 65;     //LCM_PHYSICAL_WIDTH/1000;
    params->physical_height              = 140;
#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
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
	params->dsi.packet_size = 256;
	
	// Video mode setting		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
			
	params->dsi.vertical_sync_active				= 2;
	params->dsi.vertical_backporch					= 16;
	params->dsi.vertical_frontporch 				= 9;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 
	
	params->dsi.horizontal_sync_active				= 30;
	params->dsi.horizontal_backporch				= 30;
	params->dsi.horizontal_frontporch				= 30;
	params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;
	
	// Bit rate calculation
	//params->dsi.PLL_CLOCK = 220;
	params->dsi.PLL_CLOCK = 245;
		
    params->dsi.ssc_disable = 0;
 	params->dsi.clk_lp_per_line_enable = 0;
		
		
		
#if 0
	params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;

                              
    params->dsi.lcm_esd_check_table[0].cmd              = 0xD9;
    params->dsi.lcm_esd_check_table[0].count            = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0]     = 0x80;

                
	params->dsi.lcm_esd_check_table[1].cmd          = 0x09;
	params->dsi.lcm_esd_check_table[1].count        = 4;
	params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
	params->dsi.lcm_esd_check_table[1].para_list[1] = 0x8F;
	params->dsi.lcm_esd_check_table[1].para_list[2] = 0x8B;
    params->dsi.lcm_esd_check_table[1].para_list[3] = 0x00;	
	
		
	params->dsi.lcm_esd_check_table[2].cmd          = 0x45;
	params->dsi.lcm_esd_check_table[2].count        = 2;
	params->dsi.lcm_esd_check_table[2].para_list[0] = 0x05;
	params->dsi.lcm_esd_check_table[2].para_list[1] = 0x2A;
#endif
   
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
	tps65132_i2c.addr = (TPS65132_SLAVE_ADDR_WRITE >> 1);
	tps65132_i2c.mode = ST_MODE;
	tps65132_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&tps65132_i2c, write_data, len);

	return ret_code;
}


static void lcm_set_bias(void)
{
	//unsigned char cmd = 0x00;
	//unsigned char data = 0x14;
	
	//int ret = 0;
	


	mt_set_gpio_mode(GPIO_65132_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENP, GPIO_OUT_ONE);
	MDELAY(50);
	mt_set_gpio_mode(GPIO_65132_ENN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_65132_ENN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_65132_ENN, GPIO_OUT_ONE);
//	dprintf(0,"lcm_set_bias===========================\n");
/*
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
		LCM_LOGI("tps6132----cmd=%0x--i2c write error----\n", cmd);
	else
		LCM_LOGI("pzp nt35695----tps6132----cmd=%0x--i2c write success----\n", cmd);
	*/

}
#endif



static void lcm_power_sequence_on(void)
{
	
	kal_uint32 ret = I2C_OK;
	display_ldo28_enable(1);
	
	mt_set_gpio_mode(GPIO_LCD_LDO1V8, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_LDO1V8, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_LDO1V8, GPIO_OUT_ONE);
	
	mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);	
	MDELAY(20);//100
	
	lcm_set_bias();

	/*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
	ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

	/* set AVDD 5.4v, (4v+28*0.05v) */
	/*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
	ret = PMU_REG_MASK(0xB3, 36, (0x3F << 0));
	if (ret < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

	/* set AVEE */
	/*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
	ret = PMU_REG_MASK(0xB4, 36, (0x3F << 0));
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
	
	MDELAY(20);
	mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(5);
	mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(5);
	mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(50);//250
	
}

static void lcm_power_sequence_off(void)
{
	
	mt_set_gpio_mode(GPIO_LCD_RST, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_RST, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
}


static void lcm_init(void)
{
   printf("lcm_init start \n ");
    lcm_power_sequence_on();
	MDELAY(15);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
   printf("lcm_init end \n ");
		
}
static void lcm_suspend(void)
{
	push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
	lcm_power_sequence_off();
}
static void lcm_resume(void)
{
	lcm_power_sequence_on();
	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


static unsigned int lcm_compare_id(void)
{	
	unsigned char buffer[3];
	unsigned int id = 0;
	unsigned int data_array[2];
	
	lcm_power_sequence_on();
	
		
	data_array[0]= 0x00043902;
	data_array[1]= (0x94<<24)|(0x83<<16)|(0xff<<8)|0xb9;
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0]= 0x00023902;
	data_array[1]= (0x33<<8)|0xba;
	dsi_set_cmdq(data_array, 2, 1);
		
	data_array[0]= 0x00043902;
	data_array[1]= (0x94<<24)|(0x83<<16)|(0xff<<8)|0xb9;
	dsi_set_cmdq(data_array, 2, 1);
	
	data_array[0] = 0x00033700;
	dsi_set_cmdq(data_array, 1, 1);
	
	read_reg_v2(0x04, buffer, 3);
		
	id = (buffer[0] << 16) | (buffer [1] << 8) | buffer [2] ; 
	
#ifdef BUILD_LK
		dprintf(0,"hx8394f id: buf:0x%02x ,0x%02x,0x%02x, id=0x%x\n", buffer[0], buffer[1], buffer[2], id);
#else
		printk("%s, kernel debug: hx8394f id = 0x%x\n", __func__, id);
#endif
	return (id == HX8394F_HD720_ID ? 1 : 0);
}
LCM_DRIVER hx8394f_dsi_vdo_t20_lcm_drv = 
{
	.name			= "hx8394f_hd720_dsi_vdo",
  
	#if defined(CONFIG_PRIZE_HARDWARE_INFO)  && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "hx8394f",
		.vendor	= "unknow",
		.id		= "0x0094",
		.more	= "1440*720",
	},
	#endif
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	
};
