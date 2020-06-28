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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#ifndef MACH_FPGA
#include <lcm_pmic.h>

#ifdef MTK_RT5081_PMU_CHARGER_SUPPORT
#include <cust_i2c.h>
	#define I2C_PMU_CHANNEL I2C_RT5081_PMU_CHANNEL
	#define I2C_PMU_SLAVE_7_BIT_ADDR I2C_RT5081_PMU_SLAVE_7_BIT_ADDR
#else
#ifdef MTK_MT6370_PMU
#include <platform/mt_pmu.h>
	#define I2C_PMU_CHANNEL I2C_SUBPMIC_PMU_CHANNEL
	#define I2C_PMU_SLAVE_7_BIT_ADDR I2C_SUBPMIC_PMU_SLAVE_7_BIT_ADDR
#endif
#endif

#if !defined(USE_DTB_NO_DWS)
#include <platform/mt_gpt.h>
#include <cust_gpio_usage.h>
#endif

#if  defined(MTK_RT5081_PMU_CHARGER_SUPPORT) || defined(MTK_MT6370_PMU)
static int PMU_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer)
{
	kal_uint32 ret = I2C_OK;
	kal_uint16 len;
	struct mt_i2c_t PMU_i2c;
	*dataBuffer = addr;

	PMU_i2c.id = I2C_PMU_CHANNEL;
	PMU_i2c.addr = I2C_PMU_SLAVE_7_BIT_ADDR;
	PMU_i2c.mode = ST_MODE;
	PMU_i2c.speed = 100;
	len = 1;

	ret = i2c_write_read(&PMU_i2c, dataBuffer, len, len);
	if (I2C_OK != ret)
		dprintf(1, "[LK/LCM] %s: i2c_read  failed! ret: %d\n", __func__, ret);
	return ret;
}

static int PMU_write_byte(kal_uint8 addr, kal_uint8 value)
{
	kal_uint32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;
	struct mt_i2c_t PMU_i2c;

	write_data[0] = addr;
	write_data[1] = value;

	PMU_i2c.id = I2C_PMU_CHANNEL;
	PMU_i2c.addr = I2C_PMU_SLAVE_7_BIT_ADDR;
	PMU_i2c.mode = ST_MODE;
	PMU_i2c.speed = 100;
	len = 2;

	ret_code = i2c_write(&PMU_i2c, write_data, len);

	return ret_code;
}

int PMU_REG_MASK (kal_uint8 addr, kal_uint8 val, kal_uint8 mask)
{
	kal_uint8 PMU_reg = 0;
	kal_uint32 ret = 0;

	ret = PMU_read_byte(addr, &PMU_reg);

	PMU_reg &= ~mask;
	PMU_reg |= val;

	ret = PMU_write_byte(addr, PMU_reg);

	return ret;
}
#else
int PMU_REG_MASK (kal_uint8 addr, kal_uint8 val, kal_uint8 mask)
{
	return 0;
}

#endif

//prize

#if defined(USE_DTB_NO_DWS)
#include <libfdt.h>
extern void *g_fdt;
static char is_init_lcm_ldo_gpio = 0;
static int gpio_lcd_ldo18_pin = -1;
static int gpio_lcd_ldo28_pin = -1;
static int gpio_lcd_bias_enp_pin = -1;
static int gpio_lcd_bias_enn_pin = -1;
static char init_lcm_ldo_gpio_form_dtb(){
	
	static int offset = 0;
	const void *data = NULL;
	static int is_inited = 0;
	static int len = 0;
	
	offset = fdt_path_offset(g_fdt, "/lcm_power_gpio");
	if (offset < 0) {
		dprintf(CRITICAL, "[lcm_pmic] Failed to find /lcm_power_gpio in dtb\n");
		return -1;
	}else{
		data = fdt_getprop(g_fdt, offset, "gpio_lcd_ldo18_pin", &len);
		if (len > 0 && data){
			gpio_lcd_ldo18_pin = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[lcm_pmic] gpio_lcd_ldo18_pin(%d)\n",gpio_lcd_ldo18_pin);
		}
		data = fdt_getprop(g_fdt, offset, "gpio_lcd_ldo28_pin", &len);
		if (len > 0 && data){
			gpio_lcd_ldo28_pin = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[lcm_pmic] gpio_lcd_ldo28_pin(%d)\n",gpio_lcd_ldo28_pin);
		}
		data = fdt_getprop(g_fdt, offset, "gpio_lcd_bias_enp_pin", &len);
		if (len > 0 && data){
			gpio_lcd_bias_enp_pin = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[lcm_pmic] gpio_lcd_bias_enp_pin(%d)\n",gpio_lcd_bias_enp_pin);
		}
		data = fdt_getprop(g_fdt, offset, "gpio_lcd_bias_enn_pin", &len);
		if (len > 0 && data){
			gpio_lcd_bias_enn_pin = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[lcm_pmic] gpio_lcd_bias_enn_pin(%d)\n",gpio_lcd_bias_enn_pin);
		}
	}
	is_init_lcm_ldo_gpio = 1;
	return is_init_lcm_ldo_gpio;
}
#endif

#if defined(MTK_RT5081_PMU_CHARGER_SUPPORT) || defined(MTK_MT6370_PMU)
int display_bias_enable(void)
{
	int ret = 0;

	/*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
	ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));
	/* set AVDD 5.4v, (4v+28*0.05v) */
	/*ret = RT5081_write_byte(0xB3, (1 << 6) | 28);*/
	ret = PMU_REG_MASK(0xB3, 34, (0x3F << 0));//5.7v
	if (ret < 0){
		dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB3);
	}
	/* set AVEE */
	/*ret = RT5081_write_byte(0xB4, (1 << 6) | 28);*/
	ret = PMU_REG_MASK(0xB4, 34, (0x3F << 0));//5.7
	if (ret < 0){
		dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB4);		
	}
	/* enable AVDD & AVEE */
	/* 0x12--default value; bit3--Vneg; bit6--Vpos; */
	/*ret = RT5081_write_byte(0xB1, 0x12 | (1<<3) | (1<<6));*/
	ret = PMU_REG_MASK(0xB1, (1<<3) | (1<<6), (1<<3) | (1<<6));
	if (ret < 0){
		dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB1);
	}

	return ret;
}

int display_bias_disable(void)
{
	int ret = 0;

	ret = PMU_REG_MASK(0xB1, (0<<3) | (0<<6), (1<<3) | (1<<6));
	if (ret < 0){
		dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB1);
	}

	return ret;
}

int display_bias_vpos_enable(int enable)
{
	int ret = 0;
	
	/*config rt5081 register 0xB2[7:6]=0x3, that is set db_delay=4ms.*/
	//ret = PMU_REG_MASK(0xB2, (0x3 << 6), (0x3 << 6));
	//if (ret < 0){
	//	dprintf("lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB2);
	//}
	
	if (enable){
		ret = PMU_REG_MASK(0xB1, (1<<6), (1<<6));
		if (ret < 0){
			dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB1);
		}
	}else{
		ret = PMU_REG_MASK(0xB1, (0<<6), (1<<6));
		if (ret < 0){
			dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB1);
		}
	}

	return ret;
}

int display_bias_vneg_enable(int enable)
{
	int ret = 0;

	if (enable){
		ret = PMU_REG_MASK(0xB1, (1<<3), (1<<3));
		if (ret < 0){
			dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB1);
		}		
	}else{
		ret = PMU_REG_MASK(0xB1, (0<<3), (1<<3));
		if (ret < 0){
			dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB1);
		}
	}

	return ret;
}

int display_bias_vpos_set(int mv)
{
	int ret = 0;

	if (mv > 6000){
		mv = 6000;
		dprintf(1,"rt5081 set vpos %d exceed maximum, set to 6\n",mv);
	}
	if (mv < 4000){
		mv = 4000;
		dprintf(1,"rt5081 set vpos %d lower than minimum, set to 4\n",mv);
	}
	mv = (mv-4000)/50;

	/* set AVDD 5.4v, (4v+28*0.05v) */
	ret = PMU_REG_MASK(0xB3, mv, (0x3F << 0));
	if (ret < 0){
		dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB3);
	}

	return ret;
}

int display_bias_vneg_set(int mv)
{
	int ret = 0;

	if (mv > 6000){
		mv = 6000;
		dprintf(1,"rt5081 set vpos %d exceed maximum, set to 6\n",mv);
	}
	if (mv < 4000){
		mv = 4000;
		dprintf(1,"rt5081 set vpos %d lower than minimum, set to 4\n",mv);
	}
	mv = (mv-4000)/50;

	/* set AVEE 5.4v, (4v+28*0.05v) */
	ret = PMU_REG_MASK(0xB4, mv, (0x3F << 0));
	if (ret < 0){
		dprintf(1,"lcm----rt5081----cmd=%0x--i2c write error----\n", 0xB4);
	}

	return ret;
}
#else	//defined(MTK_MT6370_PMU)

int display_bias_enable(void)
{
	int ret = 0;
#if defined(GPIO_LCD_BIAS_ENN_PIN)&&defined(GPIO_LCD_BIAS_ENP_PIN)
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);//define in dws
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
	mdelay(10);
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);
#elif defined(USE_DTB_NO_DWS)
	if (!is_init_lcm_ldo_gpio){
		init_lcm_ldo_gpio_form_dtb();
	}
	if ((gpio_lcd_bias_enp_pin >= 0)&&(gpio_lcd_bias_enn_pin >= 0)){
		mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_MODE_00);
		mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_DIR_OUT);
		mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_OUT_ONE);
		mdelay(10);
		mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_MODE_00);
		mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_DIR_OUT);
		mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_OUT_ONE);
	}
#endif
	return ret;
}


int display_bias_disable(void)
{
#if defined(GPIO_LCD_BIAS_ENN_PIN)&&defined(GPIO_LCD_BIAS_ENP_PIN)
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_MODE_00);//define in dws
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);
	mdelay(10);
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
#elif defined(USE_DTB_NO_DWS)
	if (!is_init_lcm_ldo_gpio){
		init_lcm_ldo_gpio_form_dtb();
	}
	if ((gpio_lcd_bias_enp_pin >= 0)&&(gpio_lcd_bias_enn_pin >= 0)){
		mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_MODE_00);
		mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_DIR_OUT);
		mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_OUT_ZERO);
		mdelay(10);
		mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_MODE_00);
		mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_DIR_OUT);
		mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_OUT_ZERO);
	}
#endif
	return 0;
}

int display_bias_vpos_enable(int enable)
{
	int ret = 0;
#if defined(GPIO_LCD_BIAS_ENP_PIN)
	if (enable){
		mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);//define in dws
		mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);		
	}else{
		mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
	}
#elif defined(USE_DTB_NO_DWS)
	if (!is_init_lcm_ldo_gpio){
		init_lcm_ldo_gpio_form_dtb();
	}
	if (gpio_lcd_bias_enp_pin >= 0){
		if (enable){
			mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_MODE_00);
			mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_OUT_ONE);
		}else{
			mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_MODE_00);
			mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enp_pin, GPIO_OUT_ZERO);
		}
	}
#endif

	return ret;
}

int display_bias_vneg_enable(int enable)
{
	int ret = 0;
#if defined(GPIO_LCD_BIAS_ENN_PIN)
	if (enable){
		mt_set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_MODE_00);//define in dws
		mt_set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);		
	}else{
		mt_set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);
	}
#elif defined(USE_DTB_NO_DWS)
	if (!is_init_lcm_ldo_gpio){
		init_lcm_ldo_gpio_form_dtb();
	}
	if (gpio_lcd_bias_enn_pin >= 0){
		if (enable){
			mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_MODE_00);
			mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_OUT_ONE);
		}else{
			mt_set_gpio_mode(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_MODE_00);
			mt_set_gpio_dir(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_bias_enn_pin, GPIO_OUT_ZERO);
		}
	}
#endif
	return ret;
}

int display_bias_vpos_set(int mv)
{
	int ret = 0;

	if (mv > 6000){
		mv = 6000;
		dprintf(1,"TPS65132 set vpos %d exceed maximum, set to 6\n",mv);
	}
	if (mv < 4000){
		mv = 4000;
		dprintf(1,"TPS65132 set vpos %d lower than minimum, set to 4\n",mv);
	}
	mv = (mv-4000)/100;
    
	/* set AVDD 5.4v, (4v+28*0.05v) */
	ret = PMU_REG_MASK(0x00, mv, (0x1F << 0));
	if (ret < 0){
		dprintf(1,"lcm----TPS65132----cmd=%0x--i2c write error----\n", 0x00);
	}

	return ret;
}

int display_bias_vneg_set(int mv)
{
	int ret = 0;

	if (mv > 6000){
		mv = 6000;
		dprintf(1,"TPS65132 set vpos %d exceed maximum, set to 6\n",mv);
	}
	if (mv < 4000){
		mv = 4000;
		dprintf(1,"TPS65132 set vpos %d lower than minimum, set to 4\n",mv);
	}
	mv = (mv-4000)/100;
    
	/* set AVEE 5.4v, (4v+28*0.05v) */
	ret = PMU_REG_MASK(0x01, mv, (0x1F << 0));
	if (ret < 0){
		dprintf(1,"lcm----TPS65132----cmd=%0x--i2c write error----\n", 0x01);
	}

	return ret;
}
#endif


int display_ldo18_enable(int enable)
{
	int ret = 0;
#if defined(GPIO_LCD_LDO18_PIN)
	if (enable){
		mt_set_gpio_mode(GPIO_LCD_LDO18_PIN, GPIO_MODE_00);//define in dws
		mt_set_gpio_dir(GPIO_LCD_LDO18_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_LDO18_PIN, GPIO_OUT_ONE);		
	}else{
		mt_set_gpio_mode(GPIO_LCD_LDO18_PIN, GPIO_MODE_00);//define in dws
		mt_set_gpio_dir(GPIO_LCD_LDO18_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_LDO18_PIN, GPIO_OUT_ZERO);
	}
#elif defined(USE_DTB_NO_DWS)
	if (!is_init_lcm_ldo_gpio){
		init_lcm_ldo_gpio_form_dtb();
	}
	if (gpio_lcd_ldo18_pin >= 0){
		if (enable){
			mt_set_gpio_mode(0x80000000 | gpio_lcd_ldo18_pin, GPIO_MODE_00);//define in dws
			mt_set_gpio_dir(0x80000000 | gpio_lcd_ldo18_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_ldo18_pin, GPIO_OUT_ONE);		
		}else{
			mt_set_gpio_mode(0x80000000 | gpio_lcd_ldo18_pin, GPIO_MODE_00);//define in dws
			mt_set_gpio_dir(0x80000000 | gpio_lcd_ldo18_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_ldo18_pin, GPIO_OUT_ZERO);
		}
	}
#endif

	return ret;
}

int display_ldo28_enable(int enable)
{
	int ret = 0;
#if defined(GPIO_LCD_LDO28_PIN)
	if (enable){
		mt_set_gpio_mode(GPIO_LCD_LDO28_PIN, GPIO_MODE_00);//define in dws
		mt_set_gpio_dir(GPIO_LCD_LDO28_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_LDO28_PIN, GPIO_OUT_ONE);		
	}else{
		mt_set_gpio_mode(GPIO_LCD_LDO28_PIN, GPIO_MODE_00);//define in dws
		mt_set_gpio_dir(GPIO_LCD_LDO28_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_LDO28_PIN, GPIO_OUT_ZERO);
	}
#elif defined(USE_DTB_NO_DWS)
	if (!is_init_lcm_ldo_gpio){
		init_lcm_ldo_gpio_form_dtb();
	}
	if (gpio_lcd_ldo28_pin >= 0){
		if (enable){
			mt_set_gpio_mode(0x80000000 | gpio_lcd_ldo28_pin, GPIO_MODE_00);//define in dws
			mt_set_gpio_dir(0x80000000 | gpio_lcd_ldo28_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_ldo28_pin, GPIO_OUT_ONE);		
		}else{
			mt_set_gpio_mode(0x80000000 | gpio_lcd_ldo28_pin, GPIO_MODE_00);//define in dws
			mt_set_gpio_dir(0x80000000 | gpio_lcd_ldo28_pin, GPIO_DIR_OUT);
			mt_set_gpio_out(0x80000000 | gpio_lcd_ldo28_pin, GPIO_OUT_ZERO);
		}
	}
#endif
	return ret;
}

#endif

