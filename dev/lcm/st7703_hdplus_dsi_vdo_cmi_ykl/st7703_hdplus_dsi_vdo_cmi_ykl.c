#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
//#include <mach/mt_gpio.h>
//#include <linux/xlog.h>
//#include <mach/mt_pm_ldo.h>
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1440)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

#define LCM_ID_ILI9881C	0x98811C

#define LCM_DSI_CMD_MODE									0

#ifndef TRUE
    #define   TRUE     1
#endif
 
#ifndef FALSE
    #define   FALSE    0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

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
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
{0xB9,3,{0xF1,0x12,0x83}},  

{0xBA,27,{0x32,0x81,0x05,0xF9,0x0E,0x0E,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x25,0x00,0x91,0x0A,0x00,0x00,0x01,0x4F,0xD1,0x00,0x00,0x37}}, 
     
{0xB8,1,{0x26}}, 
     
{0xB3,10,{0x10,0x10,0x0A,0x50,0x03,0xFF,0x00,0x00,0x00,0x00}}, 
   
{0xC0,9,{0x73,0x73,0x50,0x50,0x00,0x00,0x08,0x70,0x00}},  
    
{0xBC,1,{0x4F}},  
    
{0xCC,1,{0x0B}},  
    
{0xB4,1,{0x00}},  
    
{0xB1,5,{0x85,0x04,0x20,0xDB,0x01}},  
  
{0xB2,3,{0xF0,0x12,0xD0}},  
  
{0xE3,14,{0x07,0x07,0x0B,0x0B,0x03,0x0B,0x00,0x00,0x00,0x00,0xFF,0x80,0xC0,0x10}},                      
 
{0xC1,12,{0x25,0x00,0x32,0x32,0xBB,0xE1,0xFF,0xFF,0xAF,0xAF,0x7F,0x7F}}, 

{0xB5,2,{0x09,0x09}}, 
 
{0xB6,2,{0x1E,0x1E}},   
  
{0xBF,3,{0x02,0x11,0x00}},    
   
{0xC6,06,{0x01,0x00,0xFF,0xFF,0x00,0x7F}}, 

{0xCA,4,{0x00,0x01,0x0F,0x0F}},  

{0xE9,63,{0xC8,0x10,0x0A,0x05,0xA5,0x80,0x81,0x12,0x31,0x23,0x6F,0x88,0x80,0x2D,0x47,0x0A,0x00,0x00,0xC2,0x00,0x00,0x00,0x00,0x00,0xC2,0x00,0x00,0x00,0x85,0x88,0xF8,0x51,0x18,0x87,0x58,0x83,0x88,0x88,0x88,0x84,0x88,0xF8,0x40,0x08,0x86,0x48,0x82,0x88,0x88,0x88,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 

{0xEA,63,{0x00,0x1A,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x84,0x88,0x8F,0x04,0x28,0x84,0x68,0x80,0x88,0x88,0x88,0x85,0x88,0x8F,0x15,0x38,0x85,0x78,0x81,0x88,0x88,0x88,0x23,0x00,0x00,0x00,0xE8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x0F,0x50,0x00,0x40,0x80,0x2D,0xC0,0x00,0x00,0x00,0x30,0x00}}, 
 
{0xE0,34,{0x00,0x22,0x26,0x26,0x38,0x3F,0x54,0x43,0x07,0x0C,0x0E,0x13,0x15,0x13,0x14,0x10,0x16,0x00,0x22,0x26,0x26,0x38,0x3F,0x54,0x43,0x07,0x0C,0x0E,0x13,0x15,0x13,0x14,0x10,0x16}},  
  
{0xEF,3,{0xFF,0xFF,0x01}}, 

{0xC7,1,{0xB8}}, 
  
{0xC8,3,{0x00,0x00,0x04}},

{0x51,2,{0x00,0x00}},

{0x53,1,{0x2C}},

{0x55,1,{0x00}},

{0x5E,1,{0x04}},    
	{0x11,1,{0x00}},       
	{REGFLAG_DELAY, 150, {}},
	// Display ON            
	{0x29, 1, {0x00}},       
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

/*

static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

/*

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/

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
	//	params->density = 320;//LCM_DENSITY;

#if LCM_DSI_CMD_MODE
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = BURST_VDO_MODE;
#endif
		params->dsi.vertical_sync_active = 4;
		params->dsi.vertical_backporch = 7;
		params->dsi.vertical_frontporch = 16;
		params->dsi.vertical_active_line = FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;	
		params->dsi.horizontal_backporch				= 40;//50
		params->dsi.horizontal_frontporch				= 40;  //30 
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.LANE_NUM = LCM_THREE_LANE;
		
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		params->dsi.ssc_disable				=1;
		
		params->dsi.packet_size=256;
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		// Video mode setting		
		params->dsi.intermediat_buffer_num = 2;


		// params->dsi.HS_TRAIL=20; 
		params->dsi.PLL_CLOCK = 295; //320 20200424 //240=4db  230=1.5db
/***********************    esd  check   ***************************/
#ifndef BUILD_LK
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
		params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
		params->dsi.lcm_esd_check_table[0].count        = 1;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
#endif

}

#define AUXADC_COMPARE_ID          0
#define AUXADC_LCM_VOLTAGE_CHANNEL (2)
#define MIN_VOLTAGE (200)
#define MAX_VOLTAGE (800)

//extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static unsigned int lcm_compare_id(void)
{
   unsigned int array[4];
    unsigned char buffer[3] = {0};
	int id = 0;

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);

	array[0] = 0x00043700;
	dsi_set_cmdq(array, 1, 1);
	//MDELAY(1);
	read_reg_v2(0x04, &buffer[0], 3);

#ifdef BUILD_LK
    printf("%s, st7703 lcm_id = 0x%x %x %x\n", __func__, buffer[0],buffer[1],buffer[2]);
#endif
	id = buffer[0]|(buffer[1]<<8)|(buffer[2]<<16);
	printf("lcm_id=0x%x",id);
    if (id == 0x1f2138){	//default = 0xffffff
		return 1;
	}else{
		return 0;
	}
}

static void lcm_init(void)
{
	//display_ldo28_enable(1);
	//display_ldo18_enable(1);
	//display_bias_enable();
	
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{

	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	//display_bias_disable();
	//display_ldo18_enable(0);
	//`nnnndisplay_ldo28_enable(0);
}


static void lcm_resume(void)
{
//	lcm_initialization_setting[141].para_list[0] = lcm_initialization_setting[141].para_list[0] + 1; 
	lcm_init();
	
//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

/*
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}


static void lcm_setbacklight(unsigned int level)
{
	unsigned int default_level = 145;
	unsigned int mapped_level = 0;

	//for LGE backlight IC mapping table
	if(level > 255) 
			level = 255;

	if(level >0) 
			mapped_level = default_level+(level)*(255-default_level)/(255);
	else
			mapped_level=0;

	// Refresh value of backlight level.
	lcm_backlight_level_setting[0].para_list[0] = mapped_level;

	push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
}

*/

//prize-chenxiaowen-20161013-compatibility-(ili9881c_hd720_dsi_vdo_yin vs ili9881c_hd720_dsi_vdo_cmi)-start
#if defined(ILI9881C_HD720_DSI_VDO_YIN) && defined(ILI9881C_HD720_DSI_VDO_CMI)

#define MIN_VOLTAGE (600)
#define MAX_VOLTAGE (1200)
#endif
//prize-chenxiaowen-20161013-compatibility-(ili9881c_hd720_dsi_vdo_yin vs ili9881c_hd720_dsi_vdo_cmi)-end

#ifndef BUILD_LK
extern atomic_t ESDCheck_byCPU;
#endif



//prize-chenxiaowen-20161013-compatibility-(ili9881c_hd720_dsi_vdo_yin vs ili9881c_hd720_dsi_vdo_cmi)-start
#if defined(ILI9881C_HD720_DSI_VDO_YIN) && defined(ILI9881C_HD720_DSI_VDO_CMI)
static unsigned int ili_lcm_compare_id(void)
{

 int data[4] = {0,0,0,0};
 int res = 0;
 int rawdata = 0;
 int lcm_vol = 0;

 #ifdef AUXADC_LCM_VOLTAGE_CHANNEL
 res = IMM_GetOneChannelValue(AUXADC_LCM_VOLTAGE_CHANNEL,data,&rawdata);
 if(res < 0)
 {
	 #ifdef BUILD_LK
	 dprintf(0,"lixf lcd [adc_uboot]: get data error\n");
	 #endif
	 return 0;

 }
 #endif

 lcm_vol = data[0]*1000+data[1]*10;

 #ifdef BUILD_LK
 dprintf(0,"lk: lcm_vol= %d , file : %s, line : %d\n",lcm_vol, __FILE__, __LINE__);
 #else
 printk("kernel: lcm_vol= %d , file : %s, line : %d\n",lcm_vol, __FILE__, __LINE__);
 #endif

 if (lcm_vol>=MIN_VOLTAGE &&lcm_vol <= MAX_VOLTAGE &&lcm_compare_id())
 {
 		return 1;
 }

 return 0;
}
#endif
//prize-chenxiaowen-20161013-compatibility-(ili9881c_hd720_dsi_vdo_yin vs ili9881c_hd720_dsi_vdo_cmi)-end


LCM_DRIVER st7703_hdplus_dsi_vdo_cmi_ykl_lcm_drv = {
	.name		= "st7703_hdplus_dsi_vdo_cmi_ykl",
    	//prize-lixuefeng-20150512-start
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "st7703",
		.vendor	= "yikauilai",
		.id		= "0x1f2138",
		.more	= "lcm_1440*720",
	},
	#endif
	//prize-lixuefeng-20150512-end	
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//prize-chenxiaowen-20161013-compatibility-(ili9881c_hd720_dsi_vdo_yin vs ili9881c_hd720_dsi_vdo_cmi)-start
  #if defined(ILI9881C_HD720_DSI_VDO_YIN) && defined(ILI9881C_HD720_DSI_VDO_CMI)
  .compare_id = ili_lcm_compare_id,
  #else
  .compare_id = lcm_compare_id,
  #endif
	//prize-chenxiaowen-20161013-compatibility-(ili9881c_hd720_dsi_vdo_yin vs ili9881c_hd720_dsi_vdo_cmi)-end
#if (LCM_DSI_CMD_MODE)
	.set_backlight	= lcm_setbacklight,
    .update         = lcm_update,
#endif
};

