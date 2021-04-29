#if defined(BUILD_LK)
#include <debug.h>
#else
#include <linux/kernel.h>
#include <linux/string.h>
#endif

#if defined(BUILD_LK)
	#include <platform/upmu_common.h>
	#include <platform/mt_gpio.h>
	#include <platform/mt_i2c.h> 
	#include <platform/mt_pmic.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)

#define LCM_ID_ST7701S           0x8802		// reg:04//nisin?=8802

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define REGFLAG_DELAY                           0xFFE
#define REGFLAG_END_OF_TABLE                    0xFFF   // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)           lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                          lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                      lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                           lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)                   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define LCM_DSI_CMD_MODE                        0


struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
{0x01,  1 ,{0x00}},
{0xFF,  5 ,{0x77,0x01,0x00,0x00,0x13}},//ADD5/11
{0xEF,  1 ,{0x08}},
{0xFF,  5 ,{0x77,0x01,0x00,0x00,0x10}},
{0xC0,  2 ,{0x64,0x00}},//63==>64
{0xC1,  2 ,{0x09,0x02}},
{0xC2,  2 ,{0x07,0x08}},
{0xB0, 16, {0x00, 0x0D, 0x14, 0x0D, 0x11, 0x07, 0x04, 0x08, 0x08, 0x20, 0x05, 0x14, 0x12, 0x25, 0x2D, 0x1C} },
{0xB1, 16, {0x00, 0x0C, 0x14, 0x0D, 0x11, 0x06, 0x03, 0x08, 0x08, 0x1F, 0x05, 0x14, 0x12, 0x25, 0x2E, 0x1C} },
{0xFF,  5, {0x77, 0x01, 0x00, 0x00, 0x11} },
{0xB0,  1, {0x48} },//68
{0xB1,  1, {0x49} },
{0xB2,  1, {0x80} },
{0xB3,  1, {0x80} },
{0xB5,  1, {0x45} },//40
{0xB7,  1, {0x8A} },
{0xB8,  1, {0x21} },
{0xC0,  1, {0x03} },
{0xC1,  1, {0x78} },
{0xC2,  1, {0x78} },
{0xD0,  1, {0x88} },
{REGFLAG_DELAY, 100, {}},
{0xE0,  3 ,{0x00,0x00,0x02}},
{0xE1, 11 ,{0x01,0xA0,0x03,0xA0,0x02,0xA0,0x04,0xA0,0x00,0x44,0x44}},
{0xE2, 12 ,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0xE3,  4 ,{0x00,0x00,0x33,0x33}},
{0xE4,  2 ,{0x44,0x44}},
{0xE5, 16 ,{0x01,0x26,0xA0,0xA0,0x03,0x28,0xA0,0xA0,0x05,0x2A,0xA0,0xA0,0x07,0x2C,0xA0,0xA0}},
{0xE6,  4 ,{0x00,0x00,0x33,0x33}},
{0xE7,  2 ,{0x44,0x44}},
{0xE8, 16 ,{0x02,0x26,0xA0,0xA0,0x04,0x28,0xA0,0xA0,0x06,0x2A,0xA0,0xA0,0x08,0x2C,0xA0,0xA0}},
{0xEB,  7 ,{0x00,0x00,0xE4,0xE4,0x44,0x00,0x40}},
{0xED, 16 ,{0xFF,0xF7,0x65,0x4F,0x0B,0xA1,0xCF,0xFF,0xFF,0xFC,0x1A,0xB0,0xF4,0x56,0x7F,0xFF}},
{0xEF,  6, {0x08, 0x08, 0x08, 0x45, 0x3F, 0x54} },//ADD5/11
{0xFF,  5, {0x77, 0x01, 0x00, 0x00, 0x13} },
{0xE8,  2, {0x00, 0x0E} },
{0x11,  0, {0x00}},
{REGFLAG_DELAY, 120, {0}},
{0xE8,  2, {0x00, 0x0C} },
{REGFLAG_DELAY, 20, {0}},
{0xE8,  2, {0x00, 0x00} },
{0xFF,  5, {0x77, 0x01, 0x00, 0x00, 0x00} },//ADD5/11
{0x29,  0, {0x00}},
{REGFLAG_DELAY, 20, {}},
{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
    // Display off sequence
    {0x28, 0, {0x00}},
    {REGFLAG_DELAY, 20, {}},

    // Sleep Mode On
    {0x10, 0, {0x00}},
    {REGFLAG_DELAY, 180, {}},
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
#ifndef BUILD_LK
	params->physical_width               = 41;     //LCM_PHYSICAL_WIDTH/1000;
	params->physical_height              = 68;    //LCM_PHYSICAL_HEIGHT/1000;
	params->physical_width_um            = 40511;  //LCM_PHYSICAL_WIDTH; = sqrt((size*25.4)^2/(18^2+9^2))*9*1000
	params->physical_height_um           = 67519; //LCM_PHYSICAL_HEIGHT; = sqrt((size*25.4)^2/(18^2+9^2))*18*1000
	params->density = 180;
#endif
	
    // enable tearing-free
    params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode   = CMD_MODE;
#else
    params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
#endif

    // DSI
    /* Command mode setting */
    //1 Three lane or Four lane
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    // Highly depends on LCD driver capability.
    // Not support in MT6573
    params->dsi.packet_size=256;

    // Video mode setting		

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.word_count=FRAME_WIDTH*3;


    params->dsi.vertical_sync_active				=10;// 3;
    params->dsi.vertical_backporch				=20;// 14;
    params->dsi.vertical_frontporch				= 20;//16;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active			= 8;//11;
    params->dsi.horizontal_backporch				= 20;//64;20
    params->dsi.horizontal_frontporch				= 30;//64;20
    params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

//    params->dsi.HS_TRAIL= 10;

    // Bit rate calculation
    params->dsi.PLL_CLOCK=170;//234

    params->dsi.ssc_disable = 1;
	params->dsi.clk_lp_per_line_enable   = 1;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
}

static void lcm_init(void)
{
unsigned int data_array[64];
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(10); 
    SET_RESET_PIN(0);
    MDELAY(20); 
    SET_RESET_PIN(1);
    MDELAY(180);   

    //push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00001000;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000063C0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000209C1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000837C2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00113902;
data_array[1] = 0x191100B0;
data_array[2] = 0x0706100C;
data_array[3] = 0x0422090A;
data_array[4] = 0x30280E10;
data_array[5] = 0x0000001C;
dsi_set_cmdq(&data_array[0], 6, 1);

data_array[0] = 0x00113902;
data_array[1] = 0x191200B1;
data_array[2] = 0x0604100D;
data_array[3] = 0x04230807;
data_array[4] = 0x30281112;
data_array[5] = 0x0000001C;
dsi_set_cmdq(&data_array[0], 6, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00001100;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x4DB01500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x4AB11500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x00B21500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x80B31500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x40B51500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x8AB71500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x21B81500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x78C11500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x78C21500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x88D01500;
dsi_set_cmdq(&data_array[0], 1, 1);

MDELAY(100);

data_array[0] = 0x00043902;
data_array[1] = 0x020000E0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x000C3902;
data_array[1] = 0x03A001E1;
data_array[2] = 0x04A002A0;
data_array[3] = 0x444400A0;
dsi_set_cmdq(&data_array[0], 4, 1);

data_array[0] = 0x000D3902;
data_array[1] = 0x000000E2;
data_array[2] = 0x00000000;
data_array[3] = 0x00000000;
data_array[4] = 0x00000000;
dsi_set_cmdq(&data_array[0], 5, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x330000E3;
data_array[2] = 0x00000033;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x004444E4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00113902;
data_array[1] = 0xA02601E5;
data_array[2] = 0xA02803A0;
data_array[3] = 0xA02A05A0;
data_array[4] = 0xA02C07A0;
data_array[5] = 0x000000A0;
dsi_set_cmdq(&data_array[0], 6, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x330000E6;
data_array[2] = 0x00000033;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x004444E7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00113902;
data_array[1] = 0xA02602E8;
data_array[2] = 0xA02804A0;
data_array[3] = 0xA02A06A0;
data_array[4] = 0xA02C08A0;
data_array[5] = 0x000000A0;
dsi_set_cmdq(&data_array[0], 6, 1);

data_array[0] = 0x00083902;
data_array[1] = 0xE40000EB;
data_array[2] = 0x400044E4;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00113902;
data_array[1] = 0x65F7FFED;
data_array[2] = 0xCFA10B4F;
data_array[3] = 0x1AFCFFFF;
data_array[4] = 0x7F56F4B0;
data_array[5] = 0x000000FF;
dsi_set_cmdq(&data_array[0], 6, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00000000;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00001200;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x81D11500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0xFF0C1500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00000000;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00001200;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00D11500;
dsi_set_cmdq(&data_array[0], 1, 1);

data_array[0] = 0x00351500;
dsi_set_cmdq(&data_array[0], 1, 1);


data_array[0] = 0x00110500;
dsi_set_cmdq(&data_array[0], 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(&data_array[0], 1, 1);
MDELAY(20);


}

static void lcm_suspend(void)
{
    //push_table(lcm_sleep_mode_in_setting, sizeof(lcm_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
unsigned int data_array[16];
	
data_array[0] = 0x00280500;
dsi_set_cmdq(&data_array[0], 1, 1);
MDELAY(120);

data_array[0] = 0x00100500;
dsi_set_cmdq(&data_array[0], 1, 1);
MDELAY(20);


data_array[0] = 0x00073902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00800000;
dsi_set_cmdq(&data_array[0], 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000177FF;
data_array[2] = 0x00008000;
dsi_set_cmdq(&data_array[0], 3, 1);

MDELAY(5);


	
}


#if (LCM_DSI_CMD_MODE)
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
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]= 0x00053902;
    data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
    data_array[2]= (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]= 0x00290508; //HW bug, so need send one HS packet
    dsi_set_cmdq(data_array, 1, 1);

    data_array[0]= 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);
}
#endif

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
    printf("%s, st7701 lcm_id = 0x%x %x %x\n", __func__, buffer[0],buffer[1],buffer[2]);
#endif
	id = buffer[0]|(buffer[1]<<8)|(buffer[2]<<16);
	
    if (id == 0xffffff){	//default = 0xffffff
		return 1;
	}else{
		return 0;
	}
}

static void lcm_resume(void)
{
    lcm_init();
    //push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{

	return 0;
}

LCM_DRIVER st7701s_wvga_dsi_vdo_boe31_lcm_drv = 
{
    .name           = "st7701s_wvga_dsi_vdo_boe31",
    	//prize-zhangshuigang-20170308-start
	#if defined(CONFIG_PRIZE_HARDWARE_INFO) && !defined (BUILD_LK)
	.lcm_info = {
		.chip	= "st7701s",
		.vendor	= "unknow",
		.id		= "0x38",
		.more	= "lcm",
	},
	#endif
	//prize-zhangshuigang-20170308-end		
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id     = lcm_compare_id,
	.ata_check		= lcm_ata_check,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
};
