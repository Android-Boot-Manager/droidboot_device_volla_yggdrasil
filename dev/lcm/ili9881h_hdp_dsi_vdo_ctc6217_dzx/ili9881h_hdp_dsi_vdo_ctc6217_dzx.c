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
#endif

#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_notice("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)            (lcm_util.set_reset_pin((v)))
#define MDELAY(n)                   (lcm_util.mdelay(n))

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)     lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                       lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                   lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                        lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)                lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE	0
#define FRAME_WIDTH  										 (720)
#define FRAME_HEIGHT 										 (1520)

#define REGFLAG_DELAY             							 0xFFA
#define REGFLAG_UDELAY             							 0xFFB
#define REGFLAG_PORT_SWAP									 0xFFC
#define REGFLAG_END_OF_TABLE      							 0xFFD   // END OF REGISTERS MARKER

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */

struct LCM_setting_table
{
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] =
{
{0x28,1, {0x00}},
{REGFLAG_DELAY,40,{}},   
{0x10,1, {0x00}},
{REGFLAG_DELAY,120,{}},        
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_initialization_setting[] = {
{0xFF,03,{0x98,0x81,0x01}},
{0x00,01,{0x85}}, //STVA RISE 05
{0x01,01,{0x33}}, //STVA 4H
{0x02,01,{0x2A}}, //STVA 0.3H FTI
{0x03,01,{0x05}},
{0x04,01,{0xCA}}, //STVb RISE
{0x05,01,{0x33}}, //STVb 4H
{0x06,01,{0x2A}}, //STVb 0.3H FTI
{0x07,01,{0x05}},
{0x10,01,{0x0A}},
{0x11,01,{0x0A}},
{0x12,01,{0x09}},

{0x08,01,{0x86}}, //CLK RISE
{0x09,01,{0x01}},
{0x0A,01,{0x73}},
{0x0B,01,{0x00}},
{0x0C,01,{0x01}}, //CLK 0.3H CLW
{0x0D,01,{0x01}}, //CLK 0.3H CLW
{0x0E,01,{0x05}},
{0x0F,01,{0x05}},
{0x28,01,{0x00}}, //STCH1
{0x29,01,{0x00}}, //STCH1
{0xEE,01,{0x11}}, //STCH1


{0x31,01,{0x07}},     // CGOUTR[01] 
{0x32,01,{0x07}},     // CGOUTR[02] 
{0x33,01,{0x07}},     // CGOUTR[03] 
{0x34,01,{0x07}},     // CGOUTR[04] 
{0x35,01,{0x00}},     // CGOUTR[05] UP
{0x36,01,{0x07}},     // CGOUTR[06] 
{0x37,01,{0x01}},     // CGOUTR[07] DOWN
{0x38,01,{0x22}},     // CGOUTR[08] TPE
{0x39,01,{0x08}},     // CGOUTR[09] RESET
{0x3A,01,{0x07}},     // CGOUTR[10] 
{0x3B,01,{0x10}},     // CGOUTR[11] VCK<1>
{0x3C,01,{0x12}},     // CGOUTR[12] VCK<3>
{0x3D,01,{0x14}},     // CGOUTR[13] VCK<5>
{0x3E,01,{0x16}},     // CGOUTR[14] VCK<7>
{0x3F,01,{0x07}},     // CGOUTR[15] 
{0x40,01,{0x0C}},     // CGOUTR[16] VST<1>
{0x41,01,{0x0E}},     // CGOUTR[17] VST<3>
{0x42,01,{0x07}},     // CGOUTR[18] 
{0x43,01,{0x07}},     // CGOUTR[19] 
{0x44,01,{0x07}},     // CGOUTR[20] 
{0x45,01,{0x07}},     // CGOUTR[21] 
{0x46,01,{0x07}},     // CGOUTR[22] 

{0x47,01,{0x07}},     // CGOUTL[01] 
{0x48,01,{0x07}},     // CGOUTL[02] 
{0x49,01,{0x07}},     // CGOUTL[03] 
{0x4A,01,{0x07}},     // CGOUTL[04] 
{0x4B,01,{0x00}},     // CGOUTL[05] UP
{0x4C,01,{0x07}},     // CGOUTL[06] 
{0x4D,01,{0x01}},     // CGOUTL[07] DOWN
{0x4E,01,{0x22}},     // CGOUTL[08] TPE
{0x4F,01,{0x08}},     // CGOUTL[09] RESET
{0x50,01,{0x07}},     // CGOUTL[10] 
{0x51,01,{0x11}},     // CGOUTL[11] VCK<2>
{0x52,01,{0x13}},     // CGOUTL[12] VCK<4>
{0x53,01,{0x15}},     // CGOUTL[13] VCK<6>
{0x54,01,{0x17}},     // CGOUTL[14] VCK<8>
{0x55,01,{0x07}},     // CGOUTL[15] 
{0x56,01,{0x0D}},     // CGOUTL[16] VST<2>
{0x57,01,{0x0F}},     // CGOUTL[17] VST<4>
{0x58,01,{0x07}},     // CGOUTL[18] 
{0x59,01,{0x07}},     // CGOUTL[19] 
{0x5A,01,{0x07}},     // CGOUTL[20] 
{0x5B,01,{0x07}},     // CGOUTL[21] 
{0x5C,01,{0x07}},     // CGOUTL[22] 

{0x61,01,{0x07}},     // CGOUTR[01] 
{0x62,01,{0x07}},     // CGOUTR[02] 
{0x63,01,{0x07}},     // CGOUTR[03] 
{0x64,01,{0x07}},     // CGOUTR[04] 
{0x65,01,{0x00}},     // CGOUTR[05] UP
{0x66,01,{0x07}},     // CGOUTR[06] 
{0x67,01,{0x01}},     // CGOUTR[07] DOWN
{0x68,01,{0x22}},     // CGOUTR[08] TPE
{0x69,01,{0x08}},     // CGOUTR[09] RESET
{0x6A,01,{0x07}},     // CGOUTR[10] 
{0x6B,01,{0x17}},     // CGOUTR[11] VCK<1>
{0x6C,01,{0x15}},     // CGOUTR[12] VCK<3>
{0x6D,01,{0x13}},     // CGOUTR[13] VCK<5>
{0x6E,01,{0x11}},     // CGOUTR[14] VCK<7>
{0x6F,01,{0x07}},     // CGOUTR[15] 
{0x70,01,{0x0F}},     // CGOUTR[16] VST<1>
{0x71,01,{0x0D}},     // CGOUTR[17] VST<3>
{0x72,01,{0x07}},     // CGOUTR[18] 
{0x73,01,{0x07}},     // CGOUTR[19] 
{0x74,01,{0x07}},     // CGOUTR[20] 
{0x75,01,{0x07}},     // CGOUTR[21] 
{0x76,01,{0x07}},     // CGOUTR[22] 


{0x77,01,{0x07}},     //CGOUTL[01]  
{0x78,01,{0x07}},     //CGOUTL[02]  
{0x79,01,{0x07}},     //CGOUTL[03]  
{0x7A,01,{0x07}},     //CGOUTL[04]  
{0x7B,01,{0x00}},     //CGOUTL[05]  UP
{0x7C,01,{0x07}},     //CGOUTL[06]  
{0x7D,01,{0x01}},     //CGOUTL[07]  DOWN
{0x7E,01,{0x22}},     //CGOUTL[08]  TPE
{0x7F,01,{0x08}},     //CGOUTL[09]  RESET
{0x80,01,{0x07}},     //CGOUTL[10]  
{0x81,01,{0x16}},     //CGOUTL[11]  VCK<2>
{0x82,01,{0x14}},     //CGOUTL[12]  VCK<4>
{0x83,01,{0x12}},     //CGOUTL[13]  VCK<6>
{0x84,01,{0x10}},     //CGOUTL[14]  VCK<8>
{0x85,01,{0x07}},     //CGOUTL[15]  
{0x86,01,{0x0E}},     //CGOUTL[16]  VST<2>
{0x87,01,{0x0C}},     //CGOUTL[17]  VST<4>
{0x88,01,{0x07}},     //CGOUTL[18]  
{0x89,01,{0x07}},     //CGOUTL[19]  
{0x8A,01,{0x07}},     //CGOUTL[20]  
{0x8B,01,{0x07}},     //CGOUTL[21]  
{0x8C,01,{0x07}},     //CGOUTL[22]  


{0xB0,01,{0x34}},
{0xB2,01,{0x04}},
{0xB9,01,{0x00}},
{0xBA,01,{0x02}},  //STCH1 TP HIGH
{0xC1,01,{0x10}},
{0xD0,01,{0x01}},
{0xD1,01,{0x10}}, //REGION 2
{0xD2,01,{0x40}}, //REGION 2
{0xD5,01,{0x56}}, //REGION 2 
{0xD6,01,{0x11}}, //REGION 2 
{0xDc,01,{0x40}}, //REGION 4

{0xE2,01,{0x42}}, //src vcom power on /off
{0xE6,01,{0x42}}, //
{0xEA,01,{0x0F}}, //
{0xE7,01,{0x54}}, //


{0xFF,03,{0x98,0x81,0x02}},
{0x40,01,{0x48}},
{0x4B,01,{0x5A}},
{0x4D,01,{0x4E}},
{0x4E,01,{0x00}}, 


{0xFF,03,{0x98,0x81,0x03}},
{0x83,01,{0xE0}},
{0x84,01,{0x03}},

{0xFF,03,{0x98,0x81,0x05}},
{0x03,01,{0x01}},  //vcom1[8]
{0x04,01,{0x20}},  //vcom1[7:0]
{0x50,01,{0x2F}},  //x3 VGH
{0x56,01,{0x73}},  //x3
{0x58,01,{0x63}},  //X3 VGL
{0x63,01,{0x88}},  //GVDDP 5.2
{0x64,01,{0x88}},  //GVDDN -5.2
{0x68,01,{0xB5}},  //VGHO  16V
{0x69,01,{0xAE}},  //VGH   16V
{0x6A,01,{0xAB}},  //VGLO -12.5V
{0x6B,01,{0x70}},  //VGL  -12.5V  old 8F add by lipengpeng
{0x5D,01,{0x03}}, 

{0xFF,03,{0x98,0x81,0x06}},
{0xDD,01,{0x18}}, ////3lane:0x10 4lane:0x18
{0x06,01,{0x92}}, //Internal RTB
{0x11,01,{0x03}}, //
{0x13,01,{0x5E}}, //
{0x14,01,{0x41}}, //
{0x15,01,{0x0A}}, //
{0x16,01,{0x41}}, //
{0x17,01,{0x48}}, //
{0x18,01,{0x3B}}, //
{0xD6,01,{0x85}},
{0x27,01,{0xFF}}, //Internal VFP
{0x28,01,{0x20}}, //Internal VBP
{0x2E,01,{0x01}},  //1520
{0xC0,01,{0xF7}},  //1520
{0xC1,01,{0x02}},  //1520
{0xC2,01,{0x04}},
{0x48,01,{0x0F}},
{0x4D,01,{0x80}},
{0x4E,01,{0x40}},
{0xC7,01,{0x05}},
{0x7C,01,{0x40}},
{0x94,01,{0x00}},   // 
{0xD6,01,{0x87}},   // 


//Gamma Register
{0xFF,03,{0x98,0x81,0x08}},
{0xE0,27,{0x00,0x24,0x4F,0x73,0xA5,0x50,0xD3,0xF8,0x26,0x4c,0x95,0x8a,0xbc,0xeb,0x19,0xAA,0x49,0x84,0xa9,0xD6,0xFe,0xfc,0x2d,0x68,0x98,0x03,0xEC}},
{0xE1,27,{0x00,0x24,0x4F,0x73,0xA5,0x50,0xD3,0xF8,0x26,0x4c,0x95,0x8a,0xbc,0xeb,0x19,0xAA,0x49,0x84,0xa9,0xD6,0xFe,0xfc,0x2d,0x68,0x98,0x03,0xEC}},
//{0xE0,27,00,24,58,7F,B7,54,E6,0D,3A,62,95,9E,D0,FD,2A,AA,58,90,B4,E0,FF,05,34,6D,98,03,EC
//{0xE1,27,00,24,58,7F,B7,54,E6,0D,3A,62,95,9E,D0,FD,2A,AA,58,90,B4,E0,FF,05,34,6D,98,03,EC
{0xFF,03,{0x98,0x81,0x0E}},
{0x00,01,{0xA0}}, //long V
{0x01,01,{0x28}},
{0x02,01,{0x10}},
{0x13,01,{0x0C}},
{0x41,01,{0x00}},
{0x43,01,{0x60}},
{0x40,01,{0x07}},
{0x05,01,{0x00}},
{0x06,01,{0xBF}},


{0xFF,03,{0x98,0x81,0x00}},
{0x51,02,{0x00,0x00}},
{0x53,01,{0x2C}},
{0x55,01,{0x00}},		//PWM 设定开关
{0x68,02,{0x02,0x01}},      //DIMMING

{0x11,1,{0x00}},
{REGFLAG_DELAY,120,{}},
{0x29,1,{0x00}},
{REGFLAG_DELAY, 20, {}}, 
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;
    LCM_LOGI("nt35695----tps6132-lcm_init   push_table++++++++++++++===============================devin----\n");
    for (i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd)
        {
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

    params->type                         = LCM_TYPE_DSI;
    params->width                        = FRAME_WIDTH;
    params->height                       = FRAME_HEIGHT;

#ifndef BUILD_LK
	params->physical_width               = 68;     //LCM_PHYSICAL_WIDTH/1000;
	params->physical_height              = 143;    //LCM_PHYSICAL_HEIGHT/1000;
	params->physical_width_um            = 67600;  //LCM_PHYSICAL_WIDTH; = sqrt((size*25.4)^2/(18^2+9^2))*9*1000
	params->physical_height_um           = 142711; //LCM_PHYSICAL_HEIGHT; = sqrt((size*25.4)^2/(18^2+9^2))*18*1000
#endif

    // enable tearing-free
    params->dbi.te_mode                  = LCM_DBI_TE_MODE_DISABLED;
    params->dbi.te_edge_polarity         = LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode                     = CMD_MODE;
    params->dsi.switch_mode              = SYNC_PULSE_VDO_MODE;
#else
    params->dsi.mode                     = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////
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

	params->dsi.vertical_sync_active				= 4;//2;
	params->dsi.vertical_backporch					= 16;//8;
	params->dsi.vertical_frontporch					= 288;  // rom Q driver
	params->dsi.vertical_active_line				= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 8;//10;
	params->dsi.horizontal_backporch				= 32;//20;
	params->dsi.horizontal_frontporch				= 32;//40;
	params->dsi.horizontal_active_pixel             = FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 280;
	
	// params->dsi.ssc_disable = 1;
}

#define AUXADC_LCM_VOLTAGE_CHANNEL (2)
#define MIN_VAL 0xbb1
#define MAX_VAL 0xdb1
extern iio_read_channel_processed(int channel, int *val);
extern int is_9882N_detect;
static unsigned int lcm_compare_id(void)
{
    int array[4];
    char buffer[4]={0,0,0,0};
    int id=0;
	int rawdata = 0;
	int ret = 0;

    display_ldo18_enable(1);
    display_bias_vpos_enable(1);
    display_bias_vneg_enable(1);
    MDELAY(10);
    display_bias_vpos_set(5800);
	display_bias_vneg_set(5800);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);

    array[0]=0x00043902;
    array[1]=0x008198ff;
    dsi_set_cmdq(array, 2, 1);

    MDELAY(10);
    array[0] = 0x00083700;
    dsi_set_cmdq(array, 1, 1);
    //MDELAY(10);
    read_reg_v2(0x4, &buffer[0], 3);//    NC 0x00  0x98 0x16

	if (is_9882N_detect==1){
		return 0;
	}else{
		id = (buffer[0]<<16) | (buffer[1]<<8) |buffer[2];
		ret=iio_read_channel_processed(AUXADC_LCM_VOLTAGE_CHANNEL,&rawdata);
		if(rawdata >=MIN_VAL && rawdata <=MAX_VAL)
		{
			dprintf(CRITICAL, "%s, LPP ili9881h id = %x\n", __func__, rawdata);
			return 1;
		}else{
			dprintf(CRITICAL, "%s, LPP ili9881h id = %x\n", __func__, rawdata);
			return 0;
		}
	}
	
#ifdef BUILD_LK
	dprintf(CRITICAL, "%s, LK debug: ili9881h id = 0x%08x\n", __func__, id);
#else
	printk("%s: ili9881h id = 0x%08x \n", __func__, id);
#endif

	return (0x008000 == id)?1:0;
}


static void lcm_init(void)
{
    
	SET_RESET_PIN(0);
    display_ldo18_enable(1);
    display_bias_vpos_enable(1);
    display_bias_vneg_enable(1);
    MDELAY(10);
    display_bias_vpos_set(5800);
	display_bias_vneg_set(5800);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(60);
    
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(1);
#if !defined(CONFIG_PRIZE_LCM_POWEROFF_AFTER_TP)
	display_ldo18_enable(0);
	display_bias_disable();
#endif
}

#if defined(CONFIG_PRIZE_LCM_POWEROFF_AFTER_TP)
static void lcm_poweroff_ext(void){
	display_ldo18_enable(0);
	display_bias_disable();
}
#endif

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
LCM_DRIVER ili9881h_hdp_dsi_vdo_ctc6217_dzx_lcm_drv = 
{
  .name			= "ili9881h_hdp_dsi_vdo_ctc6217_dzx",
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "ili9881h",
		.vendor	= "hl",
		.id		= "0x988111",
		.more	= "1520*720",
	},
	#endif
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id = lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
#if defined(CONFIG_PRIZE_LCM_POWEROFF_AFTER_TP)
	.poweroff_ext	= lcm_poweroff_ext,
#endif
};

