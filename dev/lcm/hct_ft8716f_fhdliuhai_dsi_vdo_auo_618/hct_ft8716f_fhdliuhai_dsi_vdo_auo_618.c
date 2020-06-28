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
#define LCM_DSI_CMD_MODE	0
#define FRAME_WIDTH  										1080
#define FRAME_HEIGHT 										2246

//#ifndef GPIO_LCD_BIAS_ENP_PIN
#define GPIO_LCD_BIAS_ENP (GPIO109 | 0x80000000)
//#endif
//#ifndef GPIO_LCD_BIAS_ENN_PIN
#define GPIO_LCD_BIAS_ENN (GPIO11  | 0x80000000)
//#endif
//#ifndef GPIO_LCD_RESET
#define GPIO_LCD_RST (GPIO45  | 0x80000000)
//#endif



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


static struct LCM_setting_table lcm_initialization_setting[] = {
{0x00,1,{0x00}},                                                                      
{0xff,3,{0x87,0x16,0x01}},                                                            
                                                                                        
{0x00,1,{0x80}},                                                                      
{0xff,2,{0x87,0x16}},                                                                
                                                                                        
//========================================                                              
//Resolution 1080x2246	                                                                
{0x00,1,{0x00}},                                                                      
{0x2A,4,{0x00,0x00,0x04,0x37}}, // 1080x2246                                          
                                                                                        
{0x00,1,{0x00}},                                                                      
{0x2B,4,{0x00,0x00,0x08,0xC5}}, // 1080x2246                                          
                                                                                        
                                                                                        
//========================================                                              
//TCON Initial Code	                                                                    
{0x00,1,{0x80}}, //		                                                                
{0xC0,15,{0x00,0x74,0x00,0x10,0x10,0x00,0x74,0x10,0x10,0x00,0x74,0x00,0x10,0x10,0x00}},

{0x00,1,{0xA0}}, //	CKH cmd mode					            
{0xC0,7,{0x00,0x02,0x02,0x09,0x01,0x16,0x07}}, 	    
		                       
{0x00,1,{0xD0}}, //	CKH vdo mode					            
{0xC0,7,{0x00,0x02,0x02,0x09,0x01,0x16,0x07}},      
                       
{0x00,1,{0x82}}, //	CKH Vblank                    
{0xA5,3,{0x33,0x02,0x0C}},                     
                       
{0x00,1,{0x87}}, //	CKH tp term                    
{0xA5,4,{0x00,0x07,0x77,0x77}}, 

//========================================				
//LTPS Initial Code	
{0x00,1,{0x80}}, // VST1	
{0xC2,4,{0x84,0x02,0x35,0xBF}}, 	
{0x00,1,{0xE0}}, // VEND1						
{0xC2,4,{0x80,0x01,0x08,0x08}},
{0x00,1,{0xF4}}, // RST 
{0xC2,4,{0x88,0x05,0xCB,0x0A,0xEA}}, // = ~AUO RST  (5+2246+3=2254) 8CE	
{0x00,1,{0xB0}}, // CKV 123
{0xC2,15,{0x84,0x01,0x00,0x06,0x84, 0x83,0x02,0x00,0x06,0x84, 0x82,0x03,0x00,0x06,0x84}}, 	
{0x00,1,{0xC0}}, // CKV 4
{0xC2,5,{0x81,0x04,0x00,0x06,0x84}}, 
{0x00,1,{0xDA}}, // CKV Period
{0xC2,2,{0x33,0x33	}}, 
{0x00,1,{0xAA}}, //	CKV TP term					
{0xC3,2,{0x39,0x9C	}},		
{0x00,1,{0xD0}}, //	GOFF 1  // AUO GOFF O E					
{0xC3,13,{0x00,0x0A,0x0A,0x00,0x00,0x00,0x00, 0x01,0x04,0x00,0x5A,0x00,0x00}},
{0x00,1,{0xE0}}, //	GOFF 2	// AUO TPSW				
{0xC3,13,{0x00,0x0A,0x0A,0x00,0x00,0x00,0x00, 0x01,0x04,0x00,0x5A,0x00,0x00}},

//CE80=0x25 touch function enable
//TP control setting(term1/2/3/4)
{0x00,1,{0x80}},
{0xCE,9,{0x25,0x01,0x08,0x01,0x0C,0xFF,0x00,0x20,0x05}}, //8坑
// TP term1/2/3/4 widths control
{0x00,1,{0x90}},
{0xCE,8,{0x00,0x5C,0x0B,0x75,0x00,0x5C,0x00,0x12}},

//===============================================
//DummyTerm_20171120
{0x00,1,{0xB0}},                
{0xCE,3,{0x04,0x00,0x72}},       
{0x00,1,{0x9B}},                   
{0xCE,3,{0x01,0x16,0x07}},         

//========================================
//PanelIF Initial Code	
{0x00,1,{0x80}},	// U 2 D	CC80	
{0xCC,12,{0x02,	0x24,	0x10,	0x01,	0x06,	0x07,	0x08,	0x09,	0x20,	0x21,	0x24,	0x24}},
{0x00,1,{0x90}},	// D 2 U	CC90	
{0xCC,12,{0x10,	0x24,	0x02,	0x01,	0x09,	0x08,	0x07,	0x06,	0x20,	0x21,	0x24,	0x24}},
{0x00,1,{0xA0}},	// no dir 1	CCA0	
{0xCC,15,{0x1A,	0x1B,	0x1C,	0x1D,	0x1E,	0x1F,	0x18,	0x19,	0x14,	0x15,	0x24,	0x24,	0x24,	0x24,	0x24}},
{0x00,1,{0xB0}},	// no dir 2	CCB0	
{0xCC,5,{0x24,	0x24,	0x24,	0x24,	0x24}},
{0x00,1,{0x80}},	// slp in	CB80	
{0xCB,8,{0x00,	0x00,	0x00,	0xC0,	0x30,	0x0C,	0x03,	0x00}},
{0x00,1,{0x90}},	// power on 1	CB90	
{0xCB,15,{0x00,	0x00,	0x00,	0xC0,	0x00,	0x00,	0x00,	0x00,	0x00,	0xFF,	0x00,	0x00,	0x00,	0x00,	0x00}},
{0x00,1,{0xA0}},	// power on 2	CBA0	
{0xCB,15,{0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00}},
{0x00,1,{0xB0}},	// power on 3	CBB0	
{0xCB,2,{0x00,	0x00}},
{0x00,1,{0xC0}},	// power off 1	CBC0	
{0xCB,15,{0x15,	0x00,	0x15,	0x2A,	0x15,	0x15,	0x15,	0x15,	0x2A,	0xD5,	0x00,	0x00,	0x15,	0x15,	0x15}},
{0x00,1,{0xD0}},	// power off 2	CBD0	
{0xCB,15,{0x15,	0x15,	0x15,	0x55,	0x55,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00}},
{0x00,1,{0xE0}},	// power off 3	CBE0	
{0xCB,2,{0x00,	0x00}},
{0x00,1,{0xF0}},	// L V D	CBF0	
{0xCB,8,{0xF3,  0xFF,	0x0F,	0x3F,	0xF0,	0x0C,	0x00,	0x00}},
{0x00,1,{0x80}},	// CGOUT R 1	CD80	
{0xCD,15,{0x01,0x03,	0x13,	0x14,	0x04,	0x16,	0x16,	0x06,	0x08,	0x09,	0x0A,	0x22,	0x0D,	0x0D,	0x0E}},
{0x00,1,{0x90}},	// CGOUT R 2	CD90	
{0xCD,3,{0x0E,0x0F,0x0F}},

{0x00,1,{0xA0}},	// CGOUT L 1	CDA0	
{0xCD,15,{0x01,	0x03,	0x13,	0x14,	0x04,	0x16,	0x16,	0x05,	0x07,	0x09,	0x16,	0x0A,	0x0D,	0x0D,	0x0E}},

{0x00,1,{0xB0}},	// CGOUT L 2	CDB0	
{0xCD,3,{0x0E,	0x0F,	0x0F}},

{0x00,1,{0x80}},	// 	
{0xCF,6,{ 0x00,	0x05,	0x02, 0x00,	0x05,	0x02}},

{0x00,1,{0x81}},	// All gate on off	F381	
{0xF3,12,{	0xFF,	0xF3,	0xC0,	0xFF,	0xF7,	0xC0,	0x0,	0x2,	0x0,	0x0,	0x2,	0x0}},


// DC Power setting
{0x00,1,{0x80}},     
{0xc5,10,{0x00,0xc1,0xdd,0xc4,0x14,0x1e,0x00,0x55,0x50,0x03}}, //同0x06 = Sx = 4.5V / 0x03 = 3.0

{0x00,1,{0x90}},     
{0xc5,10,{0x77,0x19,0x1E,0x00,0x88,0x01,0x32,0x52,0x55,0x50}}, //VGHO = +8.5V / VGLO = -8V / VGH = +11V / VGL = -13.2V 


{0x00,1,{0x00}},
{0xD9,5,{0x00,0xBC,0xBC,0xBC,0xBC}},	//VCOM = TBD

{0x00,1,{0x00}},
{0xD8,2,{0x29,0x29}},		//GVDD/NGVDD = TBD
	
//========================================
{0x00,1,{0x86}},// mipi tx clk div
{0xb0,1,{0x0b}},//

{0x00,1,{0x8C}},// vcom_en pwr on off 
{0xf5,2,{0x15,0x22}},//

{0x00,1,{0x88}},// CKH EQ gnd 
{0xc3,2,{0x11,0x11}},	

{0x00,1,{0x98}},// CKH EQ vsp vsn  
{0xc3,2,{0x11,0x11}},	


//========================================
{0x00,1,{0xa0}}, // panel size = 1080x2246
{0xb3,7,{0x03,0x04,0x38,0x08,0xC6, 0x00,0x50}},  

// Gamma
{0x00,1,{0x00}}, 
{0xe1,24,{0x0d,0x1b,0x34,0x48,0x52, 0x5f,0x6f,0x7c,0x80,0x87,0x8c, 0x91,0x6b,0x68,0x67,0x60,0x53, 0x48,0x39,0x30,0x28,0x1a,0x09, 0x08}},  
{0x00,1,{0xa0}}, 
{0xe2,24,{0x0d,0x1b,0x34,0x48,0x52, 0x5f,0x6f,0x7c,0x80,0x87,0x8c, 0x91,0x6b,0x68,0x67,0x60,0x53, 0x48,0x39,0x30,0x28,0x1a,0x11, 0x10}},  

	
// TP start&&count
{0x00,1,{0x94}},
{0xCF,4,{0x00,0x00,0x10,0x20}},
{0x00,1,{0xA4}},
{0xCF,4,{0x00,0x07,0x01,0x80}},
{0x00,1,{0xd0}},
{0xCF,1,{0x08}},

	{0x29,1,{0x00}},
	{REGFLAG_DELAY,15,{}},
	{0x11,1,{0x00}},


	{REGFLAG_DELAY, 120, {}},

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
		params->dsi.vertical_backporch					= 16;//16 25 30 35 12 8
	params->dsi.vertical_frontporch					= 16;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active = 4;
		params->dsi.horizontal_backporch = 32;//32
		params->dsi.horizontal_frontporch = 32;//78
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;
		/* params->dsi.ssc_disable														 = 1; */
	
		params->dsi.PLL_CLOCK = 580;//244;
		params->dsi.ssc_disable = 0;
		params->dsi.ssc_range = 1;
		params->dsi.cont_clock = 1;
		params->dsi.clk_lp_per_line_enable = 1;

	#if 0
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
	
	}


#define RT5081_SLAVE_ADDR_WRITE  0x7C
/*
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

static int PMU_REG_MASK (kal_uint8 addr, kal_uint8 val, kal_uint8 mask)
{
	kal_uint8 RT5081_reg = 0;
	kal_uint32 ret = 0;

	ret = RT5081_read_byte(addr, &RT5081_reg);

	RT5081_reg &= ~mask;
	RT5081_reg |= val;

	ret = RT5081_write_byte(addr, RT5081_reg);

	return ret;
}
*/
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
	kal_uint32 ret1 = I2C_OK;

		SET_RESET_PIN(0);
	LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
	
	ret1 = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));
	ret1 = PMU_REG_MASK(0xB3, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);
	ret1 = PMU_REG_MASK(0xB4, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB4);
	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
	ret1 = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

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

   return (0x01 == buffer[0])?1:0; 

}

static void lcm_init(void)
{
	//LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
	kal_uint32 ret1 = I2C_OK;

		SET_RESET_PIN(0);
	LCM_LOGI("nt35695----tps6132-lcm_init3333333333333333333333333333===============================devin----\n");
	
	/*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
	ret1 = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));

	/* set AVDD 5.4v, (4v+28*0.05v) */
	/*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
	ret1 = PMU_REG_MASK(0xB3, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB3);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB3);

	/* set AVEE */
	/*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
	ret1 = PMU_REG_MASK(0xB4, 28, (0x3F << 0));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB4);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB4);

	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
	ret1 = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);

	MDELAY(15);
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);//100

	SET_RESET_PIN(1);
	MDELAY(120);//250
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
	     
	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12);*/
	ret1 = PMU_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
	if (ret1 < 0)
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write error----\n", 0xB1);
	else
		LCM_LOGI("nt35695----tps6132----cmd=%0x--i2c write success----\n", 0xB1);
	MDELAY(5);
/* 	mt_set_gpio_mode(VLCM_LDO18_EN, GPIO_MODE_00);
	mt_set_gpio_dir(VLCM_LDO18_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(VLCM_LDO18_EN, GPIO_OUT_ZERO); */
	MDELAY(2);

	SET_RESET_PIN(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}




LCM_DRIVER hct_ft8716f_fhdliuhai_dsi_vdo_auo_618_drv = 
{
    .name			= "hct_ft8716f_fhdliuhai_dsi_vdo_auo_618",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,

};
