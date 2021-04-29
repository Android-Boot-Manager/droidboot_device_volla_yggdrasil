#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (960)

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

static LCM_UTIL_FUNCS lcm_util = { 0 };

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#define REGFLAG_DELAY								0XFFD
#define REGFLAG_END_OF_TABLE							0xFFE	/* END OF REGISTERS MARKER */


/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
/* #define read_reg	 lcm_util.dsi_read_reg() */
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)



//#define LCM_DSI_CMD_MODE

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};
#if 1
static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x6C, 1,{0x60}},
	{REGFLAG_DELAY, 20, {}},
	{0xB1, 1,{0x00}},
	{0xFA, 4,{0x7F, 0x00, 0x00, 0x00}},
	{REGFLAG_DELAY, 20, {}},
	{0x6C, 1,{0x50}},
	{REGFLAG_DELAY, 60, {}},
	{0x28, 0,{0x00}},
	{REGFLAG_DELAY, 50, {}},
	{0x10, 0,{0x00}},
	{REGFLAG_DELAY, 20, {}},

	{0xF0, 5,{0x55, 0xAA, 0x52, 0x08, 0x00}},
	{0xC2, 1,{0xCE}},
	{0xC3, 1,{0xCD}},
	{0xC6, 1,{0xFC}},
	{0xC5, 1,{0x03}},
	{0xCD, 1,{0x64}},
	{0xC4, 1,{0xFF}},
	{0xC9, 1,{0xCD}},
	{0xF6, 2,{0x5A, 0x87}},
	{0xFD, 3,{0xAA, 0xAA, 0x0A}},
	{0xFE, 2,{0x6A, 0x0A}},
	{0x78, 2,{0x2A, 0xAA}},
	{0x92, 2,{0x17, 0x08}},
	{0x77, 2,{0xAA, 0x2A}},
	{0x76, 2,{0xAA, 0xAA}},

	{0x84, 1,{0x00}},
	{0x78, 2,{0x2B, 0xBA}},
	{0x89, 1,{0x73}},
	{0x88, 1,{0x3A}},
	{0x85, 1,{0xB0}},
	{0x76, 2,{0xEB, 0xAA}},
	{0x94, 1,{0x80}},
	{0x87, 3,{0x04, 0x07, 0x30}},
	{0x93, 1,{0x27}},
	{0xAF, 1,{0x02}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}

};
#endif

static struct LCM_setting_table lcm_initialization_setting[] = {

	/*
	   Note :

	   Data ID will depends on the following rule.

	   count of parameters > 1      => Data ID = 0x39
	   count of parameters = 1      => Data ID = 0x15
	   count of parameters = 0      => Data ID = 0x05

	   Structure Format :

	   {DCS command, count of parameters, {parameter list}}
	   {REGFLAG_DELAY, milliseconds of time, {}},

	   ...

	   Setting ending by predefined flag

	   {REGFLAG_END_OF_TABLE, 0x00, {}}
	 */

	{0xF0, 5,{0x55, 0xAA, 0x52, 0x08, 0x00}},

	{0xF6, 2,{0x5A, 0x87}},
	
	{0xC1, 1,{0x3F}},
	
	{0xC2, 1,{0x0E}},
	
	{0xC6, 1,{0xF8}},
	
	{0xC9, 1,{0x10}},
	
	{0xCD, 1,{0x25}},
	
	{0xF8, 1,{0x8A}},
	
	{0xAC, 1,{0x65}},
	
	{0xA7, 1,{0x47}},
	
	{0xA0, 1,{0x88}},
	
	{0xFA, 3,{0x02, 0x02, 0x02}},
	
	{0xA3, 1,{0x2E}},
	
	{0xFD, 3,{0x28, 0x3C, 0x00}},
	
	{0x71, 1,{0x48}},
	
	{0x72, 1,{0x48}},
	
	{0x73, 2,{0x00, 0x44}},
	
	{0x97, 1,{0xEE}},
	
	{0x83, 1,{0x93}},
	
	{0x9A, 1,{0xb5}},
	
	{0x9B, 1,{0x2c}},
	
	{0x82, 2,{0x30, 0x30}},
	
	{0xB1, 1,{0x10}},
	
	{0x7A, 2,{0x13, 0x1A}},
	
	{0x7B, 2,{0x13, 0x1A}},
	
	{0x86,4,{0x99,0xa3,0xa3,0x61}},
	
	{0x6D, 32,{0x00, 0x1d, 0x0A, 0x10, 0x08, 0x1F, 0x1e, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1f, 0x01, 0x0F, 0x09, 0x1d, 0x00}},
	
	{0x64, 16,{0x18, 0x04, 0x03, 0xC7, 0x03, 0x03, 0x18, 0x03, 0x03, 0xC6, 0x03, 0x03,0x0B, 0x7A,0x0B, 0x7A}},
	
	{0x67, 16,{0x18, 0x02, 0x03, 0xC5, 0x03, 0x03, 0x18, 0x01, 0x03, 0xC4, 0x03, 0x03,0x0B, 0x7A,0x0B, 0x7A}},
	
	{0x60, 8,{0x18, 0x06, 0x0B, 0x7A, 0x18, 0x02,0x0B, 0x7A}},
	
	{0x63, 8,{0x18, 0x02, 0x0B, 0x7A, 0x18, 0x05, 0x0B, 0x7A}},
	
	{0x69, 7,{0x14, 0x22, 0x14, 0x22, 0x44, 0x22, 0x08}},
	
	{0xD1, 52,{0x00, 0x00, 0x00, 0x10, 0x00, 0x46, 0x00, 0x5f, 0x00, 0x7B, 0x00, 0xa5, 0x00, 0xc8, 0x00, 0xff, 0x01, 0x29, 0x01, 0x6e, 0x01, 0xa7, 0x01, 0xfa, 0x02, 0x3d, 0x02, 0x3f, 0x02, 0x7d, 0x02, 0xca, 0x03, 0x00, 0x03, 0x4a, 0x03, 0x85, 0x03, 0xA9, 0x03, 0xC8, 0x03, 0xE8, 0x03, 0xF0, 0x03, 0xF5, 0x03, 0xFE, 0x03, 0xFF}},
	
	{0xD2, 52,{0x00, 0x00, 0x00, 0x10, 0x00, 0x46, 0x00, 0x5f, 0x00, 0x7B, 0x00, 0xa5, 0x00, 0xc8, 0x00, 0xff, 0x01, 0x29, 0x01, 0x6e, 0x01, 0xa7, 0x01, 0xfa, 0x02, 0x3d, 0x02, 0x3f, 0x02, 0x7d, 0x02, 0xca, 0x03, 0x00, 0x03, 0x4a, 0x03, 0x85, 0x03, 0xA9, 0x03, 0xC8, 0x03, 0xE8, 0x03, 0xF0, 0x03, 0xF5, 0x03, 0xFE, 0x03, 0xFF}},
	
	{0xD3, 52,{0x00, 0x00, 0x00, 0x10, 0x00, 0x46, 0x00, 0x5f, 0x00, 0x7B, 0x00, 0xa5, 0x00, 0xc8, 0x00, 0xff, 0x01, 0x29, 0x01, 0x6e, 0x01, 0xa7, 0x01, 0xfa, 0x02, 0x3d, 0x02, 0x3f, 0x02, 0x7d, 0x02, 0xca, 0x03, 0x00, 0x03, 0x4a, 0x03, 0x85, 0x03, 0xA9, 0x03, 0xC8, 0x03, 0xE8, 0x03, 0xF0, 0x03, 0xF5, 0x03, 0xFE, 0x03, 0xFF}},
	
	{0xD4, 52,{0x00, 0x00, 0x00, 0x10, 0x00, 0x46, 0x00, 0x5f, 0x00, 0x7B, 0x00, 0xa5, 0x00, 0xc8, 0x00, 0xff, 0x01, 0x29, 0x01, 0x6e, 0x01, 0xa7, 0x01, 0xfa, 0x02, 0x3d, 0x02, 0x3f, 0x02, 0x7d, 0x02, 0xca, 0x03, 0x00, 0x03, 0x4a, 0x03, 0x85, 0x03, 0xA9, 0x03, 0xC8, 0x03, 0xE8, 0x03, 0xF0, 0x03, 0xF5, 0x03, 0xFE, 0x03, 0xFF}},
	
	{0xD5, 52,{0x00, 0x00, 0x00, 0x10, 0x00, 0x46, 0x00, 0x5f, 0x00, 0x7B, 0x00, 0xa5, 0x00, 0xc8, 0x00, 0xff, 0x01, 0x29, 0x01, 0x6e, 0x01, 0xa7, 0x01, 0xfa, 0x02, 0x3d, 0x02, 0x3f, 0x02, 0x7d, 0x02, 0xca, 0x03, 0x00, 0x03, 0x4a, 0x03, 0x85, 0x03, 0xA9, 0x03, 0xC8, 0x03, 0xE8, 0x03, 0xF0, 0x03, 0xF5, 0x03, 0xFE, 0x03, 0xFF}},
	
	{0xD6, 52,{0x00, 0x00, 0x00, 0x10, 0x00, 0x46, 0x00, 0x5f, 0x00, 0x7B, 0x00, 0xa5, 0x00, 0xc8, 0x00, 0xff, 0x01, 0x29, 0x01, 0x6e, 0x01, 0xa7, 0x01, 0xfa, 0x02, 0x3d, 0x02, 0x3f, 0x02, 0x7d, 0x02, 0xca, 0x03, 0x00, 0x03, 0x4a, 0x03, 0x85, 0x03, 0xA9, 0x03, 0xC8, 0x03, 0xE8, 0x03, 0xF0, 0x03, 0xF5, 0x03, 0xFE, 0x03, 0xFF}},
	
	{0x11, 0,{0x00}},
	{REGFLAG_DELAY, 120, {}},
	
	{0x29,1,{0x00}},
	{REGFLAG_DELAY, 10, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	/* Sleep Out */
	{ 0x11, 1, {0x00} },
	{ REGFLAG_DELAY, 20, {} },

	/* Display ON */
	{ 0x29, 1, {0x00} },
	{ REGFLAG_DELAY, 120, {} },
	{ REGFLAG_END_OF_TABLE, 0x00, {} }
};
#endif



static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;

		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
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

	/* enable tearing-free */
//	params->dbi.te_mode = LCM_DBI_TE_MODE_VSYNC_ONLY;
//	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

#if defined(LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode = SYNC_PULSE_VDO_MODE;
#endif

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_TWO_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	/* Highly depends on LCD driver capability. */
	/* Not support in MT6573 */

	//params->dsi.DSI_WMEM_CONTI = 0x3C;
	//params->dsi.DSI_RMEM_CONTI = 0x3E;


	params->dsi.packet_size = 256;

	/* Video mode setting */
	//params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active 				= 2;
    params->dsi.vertical_backporch					= 15; //20;	//12;
    params->dsi.vertical_frontporch 				= 8; //18;	//14;
	params->dsi.vertical_active_line 				= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active 				= 10;
	params->dsi.horizontal_backporch 				= 30;
	params->dsi.horizontal_frontporch 				= 30;
	params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;

	/* Bit rate calculation */
	params->dsi.PLL_CLOCK = 200;

	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
}

#ifndef BUILD_LK
extern atomic_t ESDCheck_byCPU;
#endif
static unsigned int lcm_ata_check(unsigned char *buffer){
#ifndef BUILD_LK 
	unsigned int ret = 0; 

	unsigned char x0_MSB = 0x5;//((x0>>8)&0xFF); 
	unsigned char x0_LSB = 0x2;//(x0&0xFF); 
	unsigned char x1_MSB = 0x1;//((x1>>8)&0xFF); 
	unsigned char x1_LSB = 0x4;//(x1&0xFF); 
		
	unsigned int data_array[6]; 
	unsigned char read_buf[4]; 

	printk("ATA check kernel size = 0x%x,0x%x,0x%x,0x%x\n",x0_MSB,x0_LSB,x1_MSB,x1_LSB); 
	   
	data_array[0]= 0x0002390A;
	data_array[1]= 0x00002451;  
	dsi_set_cmdq(data_array, 2, 1); 
		
	data_array[0] = 0x00013700; 
	dsi_set_cmdq(data_array, 1, 1); 
	atomic_set(&ESDCheck_byCPU, 1);
	read_reg_v2(0X51, read_buf, 1); 

	atomic_set(&ESDCheck_byCPU, 0);
	if((read_buf[0] == 0x9C)) 
	   ret = 1; 
	else 
		ret = 0; 

	printk("ATA read buf kernel size = 0x%x,0x%x,0x%x,0x%x,ret= %d\n",read_buf[0],read_buf[1],read_buf[2],read_buf[4],ret); 
	return ret;
#endif
}

static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[4] = {0};
	
	SET_RESET_PIN(0);
	MDELAY(200);
	SET_RESET_PIN(1);
	MDELAY(200);

	array[0] = 0x00043700;	/* read id return two byte,version and id */
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04, buffer, 3);
    
#ifdef BUILD_LK
	dprintf(0,"%s:gc9503: id %02x %02x %02x\n", __func__, buffer[0],buffer[1],buffer[2]);
#endif
	return (0x009504 == ((buffer[0] << 16)|(buffer[1] << 8)|buffer[2]))? 1: 0;
	//return 1;


}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
    MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(150);

	push_table(lcm_initialization_setting,
		   sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}



static void lcm_suspend(void)
{
	push_table(lcm_suspend_setting,
		   sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
	//unsigned int data_array[2];
    //
	//data_array[0] = 0x00280500;	/* Display Off */
	//dsi_set_cmdq(data_array, 1, 1);
	//MDELAY(10);
	//data_array[0] = 0x00100500;	/* Sleep In */
	//dsi_set_cmdq(data_array, 1, 1);
	//MDELAY(100);

#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif
  	SET_RESET_PIN(1);
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif
/* push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1); */
	lcm_init();
}

#if 0
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

#ifdef BUILD_LK
	printf("uboot %s\n", __func__);
#else
	pr_debug("kernel %s\n", __func__);
#endif

	data_array[0] = 0x00053902;
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	data_array[3] = 0x00053902;
	data_array[4] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[5] = (y1_LSB);
	data_array[6] = 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}

#endif

LCM_DRIVER gc9503v_fwvga_dsi_hsd_lcm_drv = {

	.name = "gc9503v_fwvga_dsi_hsd",
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "gc9503v",
		.vendor	= "unknow",
		.id		= "gc9503v",
		.more	= "960*480",
	},
	#endif
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id = lcm_compare_id,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
#if defined(LCM_DSI_CMD_MODE)
	.update = lcm_update,
#endif
	.ata_check	= lcm_ata_check
};
