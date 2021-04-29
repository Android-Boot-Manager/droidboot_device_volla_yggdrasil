/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part,
* shall be strictly prohibited.
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
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
* ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
* THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL
* ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S
* SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
* RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/

#include <platform/mt_typedefs.h>
#include <platform/errno.h>
#include <platform/mt_i2c.h>
#include <printf.h>
#include <debug.h>
#include <platform/timer.h>
#include <libfdt.h>
#include "hl7005all_charger.h"
#include "mtk_charger_intf.h"
#include "mtk_charger.h"
#include <platform/mt_gpio.h>

#define HL7005ALL_CHARGER_LK_DRV_VERSION "0.0.0"

/* ================= */
/* Internal variable */
/* ================= */

enum hl7005all_device_model_enum{
	DEVICE_UNKNOWN,
	DEVICE_PSC5415A,	//111 10
	DEVICE_ETA6937,		//010 10
	DEVICE_PSC5425,	//111 11
	//DEVICE_PSC5415,	//100 xx
};

const unsigned int VBAT_CVTH[] = {
	3500, 3520, 3540, 3560,
	3580, 3600, 3620, 3640,
	3660, 3680, 3700, 3720,
	3740, 3760, 3780, 3800,
	3820, 3840, 3860, 3880,
	3900, 3920, 3940, 3960,
	3980, 4000, 4020, 4040,
	4060, 4080, 4100, 4120,
	4140, 4160, 4180, 4200,
	4220, 4240, 4260, 4280,
	4300, 4320, 4340, 4360,
	4380, 4400, 4420, 4440
};

#if defined(CONFIG_PSC5415A_56MR_SUPPORT)
const unsigned int CSTH_PSC5415A[] = {
	425, 800, 950, 1150,
	1350, 1450
};
#else	//elif defined(CONFIG_PSC5415A_68MR_SUPPORT)
const unsigned int CSTH_PSC5415A[] = {
	350, 500, 800, 950,
	1100, 1200, 1400, 1500
};
#endif

#if defined(CONFIG_PSC5425_40MR_SUPPORT)
const unsigned int CSTH_PSC5425[] = {
	500, 925, 1100, 1300,
	1500, 1800, 2000, 2300
};
#else	//elif defined(CONFIG_PSC5425_33MR_SUPPORT)
const unsigned int CSTH_PSC5425[] = {
	500, 1000, 1300, 1600,
	1900, 2200, 2400
};
#endif

const unsigned int CSTH_ETA6937[] = {
	550, 650, 750, 850,
	950, 1050, 1150, 1250,
	1350, 1450, 1550, 1650,
	1750, 1850, 1950, 2050,
	2150, 2250, 2350, 2450,
	2550, 2650, 2750, 2850,
	2950, 3050
};
const unsigned int CSTH[] = {
	550, 650, 750, 850,
	950, 1050, 1150, 1250
};
const unsigned int INPUT_CSTH_ETA6937[] = {
	300, 500, 800, 1200, 
	1500, 2000, 3000, 5000
};

/*hl7005 REG00 IINLIM[5:0]*/
const unsigned int INPUT_CSTH[] = {
	100, 500, 800, 5000
};


struct hl7005all_chg_info {
	struct mtk_charger_info mchr_info;
	struct mt_i2c_t i2c;
	int i2c_log_level;
	enum hl7005all_device_model_enum dev_model;
	int gpio_sw_en;
};

extern void *g_fdt;

static int mt_charger_dump_register(struct mtk_charger_info *mchr_info);
static int mt_charger_enable_charging(struct mtk_charger_info *mchr_info,bool enable);
static int mt_charger_get_ichg(struct mtk_charger_info *mchr_info, u32 *ichg);
static int mt_charger_set_ichg(struct mtk_charger_info *mchr_info, u32 ichg);
static int mt_charger_get_aicr(struct mtk_charger_info *mchr_info, u32 *aicr);
static int mt_charger_set_aicr(struct mtk_charger_info *mchr_info, u32 aicr);
static int mt_charger_set_mivr(struct mtk_charger_info *mchr_info, u32 mivr);
static int mt_charger_reset_wdt(struct mtk_charger_info *mchr_info);
static struct mtk_charger_ops hl7005all_mchr_ops = {
	.dump_register = mt_charger_dump_register,
	.enable_charging = mt_charger_enable_charging,
	.get_ichg = mt_charger_get_ichg,
	.set_ichg = mt_charger_set_ichg,
	.get_aicr = mt_charger_get_aicr,
	.get_adc = NULL,
	.get_vbus = NULL,
	.set_aicr = mt_charger_set_aicr,
	.set_mivr = mt_charger_set_mivr,
	.enable_power_path = NULL,
	.reset_pumpx = NULL,
	.enable_wdt = NULL,
	.reset_wdt = mt_charger_reset_wdt,
};
/* Info of primary charger */
static struct hl7005all_chg_info g_hl7005all_info = {
	.mchr_info = {
		.name = "primary_charger",
		.device_id = -1,
		.mchr_ops = &hl7005all_mchr_ops,
	},
	.i2c = {
		.id = I2C_APPM,
		.addr = HL7005_SLAVE_ADDR,
		.mode = HS_MODE,
		.speed = 3400,
		.pushpull = true,
	},
	.i2c_log_level = INFO,
	.gpio_sw_en = 0,
};

/* ========================= */
/* I2C operations */
/* ========================= */
static int hl7005all_i2c_write_byte(struct hl7005all_chg_info *info,
				 u8 cmd, u8 data)
{
	struct mt_i2c_t *i2c = &info->i2c;
	int ret = I2C_OK;
	u8 write_buf[2] = {cmd, data};

	ret = i2c_write(i2c, write_buf, 2);
	if (ret != I2C_OK)
		dprintf(CRITICAL,
			"%s: I2CW[0x%02X] = 0x%02X failed, code = %d\n",
			__func__, cmd, data, ret);
	//else
	//	dprintf(info->i2c_log_level, "%s: I2CW[0x%02X] = 0x%02X\n",
	//		__func__, cmd, data);
	return ret;
}

static int hl7005all_i2c_read_byte(struct hl7005all_chg_info *info,
				u8 cmd, u8 *data)
{
	struct mt_i2c_t *i2c = &info->i2c;
	int ret = I2C_OK;
	u8 ret_data = cmd;

	ret = i2c_write_read(i2c, &ret_data, 1, 1);
	if (ret != I2C_OK)
		dprintf(CRITICAL, "%s: I2CR[0x%02X] failed, code = %d\n",
			__func__, cmd, ret);
	else {
		//dprintf(info->i2c_log_level, "%s: I2CR[0x%02X] = 0x%02X\n",
		//	__func__, cmd, ret_data);
		*data = ret_data;
	}
	return ret;
}

static int hl7005all_i2c_block_write(struct hl7005all_chg_info *info,
				  u8 cmd, int len, const u8 *data)
{
	struct mt_i2c_t *i2c = &info->i2c;
	unsigned char write_buf[len + 1];

	write_buf[0] = cmd;
	memcpy(&write_buf[1], data, len);
	return i2c_write(i2c, write_buf, len + 1);
}

static int hl7005all_i2c_block_read(struct hl7005all_chg_info *info,
				 u8 cmd, int len, u8 *data)
{
	struct mt_i2c_t *i2c = &info->i2c;
	data[0] = cmd;

	return i2c_write_read(i2c, data, len, len);
}

static int hl7005all_i2c_update_bits(struct hl7005all_chg_info *info,
				  u8 cmd, u8 mask, u8 data)
{
	int ret = 0;
	u8 reg_data = 0;

	ret = hl7005all_i2c_read_byte(info, cmd, &reg_data);
	if (ret != I2C_OK)
		return ret;
	reg_data = reg_data & 0xFF;
	reg_data &= ~mask;
	reg_data |= (data & mask);
	return hl7005all_i2c_write_byte(info, cmd, reg_data);
}

static inline int hl7005all_set_bit(struct hl7005all_chg_info *info,
				 u8 reg, u8 mask)
{
    return hl7005all_i2c_update_bits(info, reg, mask, mask);
}

static inline int hl7005all_clr_bit(struct hl7005all_chg_info *info,
				 u8 reg, u8 mask)
{
    return hl7005all_i2c_update_bits(info, reg, mask, 0x00);
}

static int hl7005_read_interface(unsigned char reg_num, unsigned char *val, unsigned char MASK,
				unsigned char SHIFT)
{
	unsigned char hl7005_reg = 0;
	int ret = 0;

	ret = hl7005all_i2c_read_byte(&g_hl7005all_info,reg_num, &hl7005_reg);
	hl7005_reg &= (MASK << SHIFT);
	*val = (hl7005_reg >> SHIFT);

	return ret;
}

static int hl7005_config_interface(unsigned char reg_num, unsigned char val, unsigned char MASK,
					unsigned char SHIFT)
{
	u8 hl7005_reg = 0;
	int ret = 0;

	ret = hl7005all_i2c_read_byte(&g_hl7005all_info, reg_num, &hl7005_reg);
	hl7005_reg &= ~(MASK << SHIFT);
	hl7005_reg |= ((val&MASK) << SHIFT);
	if (reg_num == HL7005_CON4)
		hl7005_reg &= ~(1 << CON4_RESET_SHIFT);

	ret = hl7005all_i2c_write_byte(&g_hl7005all_info, reg_num, hl7005_reg);

	return ret;
}

static int hl7005_reg_config_interface(unsigned char reg_num, unsigned char val)
{
	return hl7005all_i2c_write_byte(&g_hl7005all_info, reg_num, val);
}

/***************************************
bmt functions
***************************************/
unsigned int charging_value_to_parameter(const unsigned int *parameter, const unsigned int array_size,
					const unsigned int val)
{
	if (val < array_size)
		return parameter[val];
	return parameter[0];
}

unsigned int charging_parameter_to_value(const unsigned int *parameter, const unsigned int array_size,
					const unsigned int val)
{
	unsigned int i;

	for (i = 0; i < array_size; i++) {
		if (val == *(parameter + i))
			return i;
	}
	/* TODO: ASSERT(0);	// not find the value */
	return 0;
}
static unsigned int bmt_find_closest_level(const unsigned int *pList, unsigned int number,
					 unsigned int level)
{
	unsigned int i;
	unsigned int max_value_in_last_element;

	if (pList[0] < pList[1])
		max_value_in_last_element = 1;
	else
		max_value_in_last_element = 0;

	if (max_value_in_last_element == 1) {
		for (i = (number - 1); i != 0; i--) {	/* max value in the last element */
			if (pList[i] <= level) {
				return pList[i];
			}
		}
		return pList[0];
		/* return 000; */
	} else {
		for (i = 0; i < number; i++) {	/* max value in the first element */
			if (pList[i] <= level)
				return pList[i];
		}
		return pList[number - 1];
		/* return 000; */
	}
}
/***************************************
hl7005 functions
***************************************/

static void hl7005_set_tmr_rst(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON0),
				(unsigned char)(val),
				(unsigned char)(CON0_TMR_RST_MASK),
				(unsigned char)(CON0_TMR_RST_SHIFT)
				);
}

static unsigned int hl7005_get_otg_status(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON0),
				(unsigned char *)(&val),
				(unsigned char)(CON0_OTG_MASK),
				(unsigned char)(CON0_OTG_SHIFT)
				);
	return val;
}

static void hl7005_set_en_stat(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON0),
				(unsigned char)(val),
				(unsigned char)(CON0_EN_STAT_MASK),
				(unsigned char)(CON0_EN_STAT_SHIFT)
				);
}

static unsigned int hl7005_get_chip_status(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON0),
				(unsigned char *)(&val),
				(unsigned char)(CON0_STAT_MASK),
				(unsigned char)(CON0_STAT_SHIFT)
				);
	return val;
}

static unsigned int hl7005_get_boost_status(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON0),
				(unsigned char *)(&val),
				(unsigned char)(CON0_BOOST_MASK),
				(unsigned char)(CON0_BOOST_SHIFT)
				);
	return val;

}

static unsigned int hl7005_get_fault_status(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON0),
				(unsigned char *)(&val),
				(unsigned char)(CON0_FAULT_MASK),
				(unsigned char)(CON0_FAULT_SHIFT)
				);
	return val;
}

static void hl7005_set_input_charging_current(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON1),
				(unsigned char)(val),
				(unsigned char)(CON1_LIN_LIMIT_MASK),
				(unsigned char)(CON1_LIN_LIMIT_SHIFT)
				);
}

static unsigned int hl7005_get_input_charging_current(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON1),
				(unsigned char *)(&val),
				(unsigned char)(CON1_LIN_LIMIT_MASK),
				(unsigned char)(CON1_LIN_LIMIT_SHIFT)
				);

	return val;
}

static void hl7005_set_v_low(unsigned int val)
{

	hl7005_config_interface((unsigned char)(HL7005_CON1),
				(unsigned char)(val),
				(unsigned char)(CON1_LOW_V_MASK),
				(unsigned char)(CON1_LOW_V_SHIFT)
				);
}

static void hl7005_set_te(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON1),
				(unsigned char)(val),
				(unsigned char)(CON1_TE_MASK),
				(unsigned char)(CON1_TE_SHIFT)
				);
}

static void hl7005_set_ce(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON1),
				(unsigned char)(val),
				(unsigned char)(CON1_CE_MASK),
				(unsigned char)(CON1_CE_SHIFT)
				);
}

static void hl7005_set_hz_mode(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON1),
				(unsigned char)(val),
				(unsigned char)(CON1_HZ_MODE_MASK),
				(unsigned char)(CON1_HZ_MODE_SHIFT)
				);
}

static void hl7005_set_opa_mode(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON1),
				(unsigned char)(val),
				(unsigned char)(CON1_OPA_MODE_MASK),
				(unsigned char)(CON1_OPA_MODE_SHIFT)
				);
}

static void hl7005_set_oreg(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON2),
				(unsigned char)(val),
				(unsigned char)(CON2_OREG_MASK),
				(unsigned char)(CON2_OREG_SHIFT)
				);
}
static void hl7005_set_otg_pl(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON2),
				(unsigned char)(val),
				(unsigned char)(CON2_OTG_PL_MASK),
				(unsigned char)(CON2_OTG_PL_SHIFT)
				);
}
static void hl7005_set_otg_en(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON2),
				(unsigned char)(val),
				(unsigned char)(CON2_OTG_EN_MASK),
				(unsigned char)(CON2_OTG_EN_SHIFT)
				);
}

static unsigned int hl7005_get_vender_code(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON3),
				(unsigned char *)(&val),
				(unsigned char)(CON3_VENDER_CODE_MASK),
				(unsigned char)(CON3_VENDER_CODE_SHIFT)
				);
	return val;
}
static unsigned int hl7005_get_pn(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON3),
				(unsigned char *)(&val),
				(unsigned char)(CON3_PIN_MASK),
				(unsigned char)(CON3_PIN_SHIFT)
				);
	return val;
}

static unsigned int hl7005_get_revision(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON3),
				(unsigned char *)(&val),
				(unsigned char)(CON3_REVISION_MASK),
				(unsigned char)(CON3_REVISION_SHIFT)
				);
	return val;
}

static void hl7005_set_reset(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON4),
				(unsigned char)(val),
				(unsigned char)(CON4_RESET_MASK),
				(unsigned char)(CON4_RESET_SHIFT)
				);
}

static void hl7005_set_iocharge(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON4),
				(unsigned char)(val),
				(unsigned char)(CON4_I_CHR_MASK),
				(unsigned char)(CON4_I_CHR_SHIFT)
				);
}

static void hl7005_set_iterm(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON4),
				(unsigned char)(val),
				(unsigned char)(CON4_I_TERM_MASK),
				(unsigned char)(CON4_I_TERM_SHIFT)
				);
}

static void hl7005_set_bit3_iocharge(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON5),
				(unsigned char)(val),
				(unsigned char)(CON5_I_CHR_BIT3_MASK),
				(unsigned char)(CON5_I_CHR_BIT3_SHIFT)
				);
}

static void hl7005_set_bit4_iocharge(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON5),
				(unsigned char)(val),
				(unsigned char)(CON5_I_CHR_BIT4_MASK),
				(unsigned char)(CON5_I_CHR_BIT4_SHIFT)
				);
}

static void hl7005_set_iocharge_offset(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON4),
				(unsigned char)(val),
				(unsigned char)(CON4_I_CHR_OFFSET_MASK),
				(unsigned char)(CON4_I_CHR_OFFSET_SHIFT)
				);
}

static void hl7005_set_iochargeh(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON5),
				(unsigned char)(val),
				(unsigned char)(CON5_I_CHRH_MASK),
				(unsigned char)(CON5_I_CHRH_SHIFT)
				);
}

static void hl7005_set_dis_vreg(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON5),
				(unsigned char)(val),
				(unsigned char)(CON5_DIS_VREG_MASK),
				(unsigned char)(CON5_DIS_VREG_SHIFT)
				);
}

static void hl7005_set_io_level(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON5),
				(unsigned char)(val),
				(unsigned char)(CON5_IO_LEVEL_MASK),
				(unsigned char)(CON5_IO_LEVEL_SHIFT)
				);
}

static unsigned int hl7005_get_sp_status(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON5),
				(unsigned char *)(&val),
				(unsigned char)(CON5_SP_STATUS_MASK),
				(unsigned char)(CON5_SP_STATUS_SHIFT)
				);
	return val;
}

static unsigned int hl7005_get_en_level(void)
{
	unsigned char val = 0;

	hl7005_read_interface((unsigned char)(HL7005_CON5),
				(unsigned char *)(&val),
				(unsigned char)(CON5_EN_LEVEL_MASK),
				(unsigned char)(CON5_EN_LEVEL_SHIFT)
				);
	return val;
}

static void hl7005_set_vsp(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON5),
				(unsigned char)(val),
				(unsigned char)(CON5_VSP_MASK),
				(unsigned char)(CON5_VSP_SHIFT)
				);
}

static void hl7005_set_i_safe(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON6),
				(unsigned char)(val),
				(unsigned char)(CON6_ISAFE_MASK),
				(unsigned char)(CON6_ISAFE_SHIFT)
				);
}

static void hl7005_set_v_safe(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON6),
				(unsigned char)(val),
				(unsigned char)(CON6_VSAFE_MASK),
				(unsigned char)(CON6_VSAFE_SHIFT)
				);
}

static void hl7005_set_iinlimit_selection(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON7),
				(unsigned char)(val),
				(unsigned char)(CON7_IINLIMIT_SELECTION_MASK),
				(unsigned char)(CON7_IINLIMIT_SELECTION_SHIFT)
				);
}

static void hl7005_set_input_charging_current2(unsigned int val)
{
	hl7005_config_interface((unsigned char)(HL7005_CON7),
				(unsigned char)(val),
				(unsigned char)(CON7_IINLIMIT2_MASK),
				(unsigned char)(CON7_IINLIMIT2_SHIFT)
				);
}

/****************************************/

/* ================== */
/* internal functions */
/* ================== */
static bool hl7005all_is_hw_exist(struct hl7005all_chg_info *info)
{
	int ret = 0;
	u8 data = 0;
	unsigned int ven = 0, pn = 0, rev = 0;
	int cnt = 0;
	
	for (cnt = 0;cnt <5; cnt++){
		ret = hl7005all_i2c_read_byte(info, HL7005_CON3, &data);
		if (ret != I2C_OK){
			dprintf(CRITICAL,"%s read vid fail ret(%02x) data(%02x)\n",__func__,ret,data);
			//return false;
			continue;
		}
		if (data != 0x0){
			break;
		}
		mdelay(1);
	}
	if ((ret != I2C_OK)||(data == 0x0)){
		return false;
	}
	
	ven = (data >> CON3_VENDER_CODE_SHIFT)&CON3_VENDER_CODE_MASK;
	pn = (data >> CON3_PIN_SHIFT)&CON3_PIN_MASK ;
	rev = (data >> CON3_REVISION_SHIFT)&CON3_REVISION_MASK ;
	dprintf(CRITICAL,"%s VID(%02X) PN(%02X) REV(%02x)\n",__func__,ven,pn,rev);
	if ((ven & 0b111) == 0b111){
		if ((pn & 0b11) == 0b10){
			info->dev_model = DEVICE_PSC5415A;
		}else if ((pn & 0b11) == 0b11){
			info->dev_model = DEVICE_PSC5425;
		}else{
			info->dev_model = DEVICE_UNKNOWN;
		}
	}else if ((ven & 0b111) == 0b010){
		if ((pn & 0b11) == 0b10){
			info->dev_model = DEVICE_ETA6937;
		}else{
			info->dev_model = DEVICE_UNKNOWN;
		}
	}else{
		info->dev_model = DEVICE_UNKNOWN;
	}
	
	if (info->dev_model == DEVICE_UNKNOWN){
		dprintf(CRITICAL, "%s: vid is not match(%d)\n", __func__, ven);
		return -ENODEV;
	}

	return true;
}


static int hl7005all_set_cv(struct hl7005all_chg_info *info, u32 uv)
{
	unsigned short int array_size;
	unsigned int set_cv_voltage;
	unsigned short int register_value;
	/*static kal_int16 pre_register_value; */
	array_size = ARRAY_SIZE(VBAT_CVTH);
	/*pre_register_value = -1; */
	set_cv_voltage = bmt_find_closest_level(VBAT_CVTH, array_size, uv);

	register_value =
	charging_parameter_to_value(VBAT_CVTH, array_size, set_cv_voltage);
	hl7005_set_oreg(register_value);
	
	return 0;
}

static int hl7005_reset_watch_dog_timer(struct hl7005all_chg_info *info)
{
	switch(info->dev_model){
		case DEVICE_ETA6937:{
			hl7005_set_tmr_rst(1);
			break;
		}
		default:{
			break;
		}
	}
	dprintf(CRITICAL, "%s\n", __func__);
	return 0;
}
/* =========================================================== */
/* The following is implementation for interface of mt_charger */
/* =========================================================== */
static int mt_charger_enable_charging(struct mtk_charger_info *mchr_info,
				      bool enable)
{
	struct hl7005all_chg_info *info =
		(struct hl7005all_chg_info *)mchr_info;

	if (enable) {
	#if defined(USE_DTB_NO_DWS)
			//set charge_disable_pin
		mt_set_gpio_mode(info->gpio_sw_en|0x80000000, GPIO_MODE_00);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_dir(info->gpio_sw_en|0x80000000, GPIO_DIR_OUT);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_out(info->gpio_sw_en|0x80000000, GPIO_OUT_ZERO);	//define GPIO_SWCHARGER_EN_PIN in dws
	#else{
		//set charge_disable_pin
		mt_set_gpio_mode(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_MODE_00);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_dir(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_DIR_OUT);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_out(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_OUT_ZERO);	//define GPIO_SWCHARGER_EN_PIN in dws
	#endif		
		if (info->dev_model == DEVICE_ETA6937){

			hl7005_reg_config_interface(0x06, 0x8A);	//safe set to 4.4v

			hl7005_set_iocharge_offset(0);  //set 0:550mA  1:650mA
			hl7005_set_vsp(3);      // n*80mV+4.2v
			hl7005_set_iterm(1);    // n*50mA
			hl7005_set_te(1);
			hl7005_set_tmr_rst(1);
		}
		hl7005_set_ce(0);
		hl7005_set_hz_mode(0);
		hl7005_set_opa_mode(0);
	} else {
		hl7005_set_ce(1);
	#if defined(USE_DTB_NO_DWS)
		//set charge_disable_pin
		mt_set_gpio_mode(info->gpio_sw_en|0x80000000, GPIO_MODE_00);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_dir(info->gpio_sw_en|0x80000000, GPIO_DIR_OUT);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_out(info->gpio_sw_en|0x80000000, GPIO_OUT_ONE);	//define GPIO_SWCHARGER_EN_PIN in dws
	#else{
		//set charge_disable_pin
		mt_set_gpio_mode(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_MODE_00);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_dir(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_DIR_OUT);	//define GPIO_SWCHARGER_EN_PIN in dws
		mt_set_gpio_out(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_OUT_ONE);	//define GPIO_SWCHARGER_EN_PIN in dws
	#endif
	}
	mdelay(30);// must have after CHARGE_ENABLE or some cmd will not take effect
	return 0;
}

static int mt_charger_get_ichg(struct mtk_charger_info *mchr_info, u32 *ichg)
{
	struct hl7005all_chg_info *info =
		(struct hl7005all_chg_info *)mchr_info;
	unsigned int array_size;
	unsigned char reg_value;
	unsigned char ichg_reg;
	
	switch(info->dev_model){
		case DEVICE_ETA6937:{
			array_size = ARRAY_SIZE(CSTH_ETA6937);
			hl7005_read_interface(HL7005_CON5, &reg_value, CON5_IO_LEVEL_MASK, CON5_IO_LEVEL_SHIFT);
			if (reg_value){
				*ichg = 550;
				break;
			}
			hl7005_read_interface(HL7005_CON4, &reg_value, CON4_I_CHR_MASK, CON4_I_CHR_SHIFT);
			ichg_reg = reg_value;
			hl7005_read_interface(HL7005_CON5, &reg_value, CON5_I_CHRH_MASK, CON5_I_CHRH_SHIFT);
			ichg_reg |= (reg_value << CON5_I_CHRH_SHIFT);
			*ichg = charging_value_to_parameter(CSTH_ETA6937, array_size, ichg_reg);
			hl7005_read_interface(HL7005_CON4, &reg_value, CON4_I_CHR_OFFSET_MASK, CON4_I_CHR_OFFSET_SHIFT);
			if (reg_value){
				*ichg += 100;
			}
			break;
		}
		case DEVICE_PSC5415A:{
			array_size = ARRAY_SIZE(CSTH_PSC5415A);
			hl7005_read_interface(HL7005_CON4, &reg_value, CON4_I_CHR_MASK, CON4_I_CHR_SHIFT);
			*ichg = charging_value_to_parameter(CSTH_PSC5415A, array_size, reg_value);
			break;
		}
		case DEVICE_PSC5425:{
			array_size = ARRAY_SIZE(CSTH_PSC5425);
			hl7005_read_interface(HL7005_CON4, &reg_value, CON4_I_CHR_MASK, CON4_I_CHR_SHIFT);
			*ichg = charging_value_to_parameter(CSTH_PSC5425, array_size, reg_value);
			break;
		}
		default:{
			hl7005_read_interface(HL7005_CON5, &reg_value, CON5_IO_LEVEL_MASK, CON5_IO_LEVEL_SHIFT);
			if (reg_value){
				*ichg = 350;
				break;
			}
			array_size = ARRAY_SIZE(CSTH);
			hl7005_read_interface(HL7005_CON4, &reg_value, CON4_I_CHR_MASK, CON4_I_CHR_SHIFT);
			*ichg = charging_value_to_parameter(CSTH, array_size, reg_value);
			break;
		}
	}
	return 0;
}

static int mt_charger_set_ichg(struct mtk_charger_info *mchr_info, u32 ichg)
{
	struct hl7005all_chg_info *info =
		(struct hl7005all_chg_info *)mchr_info;

	dprintf(CRITICAL, "[hl7005]%s: ichg = %d\n", __func__, ichg);
	unsigned int set_chr_current;
	unsigned int array_size;
	unsigned int register_value;

	switch(info->dev_model){
		case DEVICE_ETA6937:{
			array_size = ARRAY_SIZE(CSTH_ETA6937);
			set_chr_current = bmt_find_closest_level(CSTH_ETA6937, array_size, ichg);
			register_value = charging_parameter_to_value(CSTH_ETA6937, array_size, set_chr_current);
			hl7005_set_io_level(0);	//0:control by ichg, 1:force 550mA
			hl7005_set_iocharge(register_value&0b111);
			hl7005_set_iochargeh((register_value&0b11000)>>3);
			break;
		}
		case DEVICE_PSC5415A:{
			array_size = ARRAY_SIZE(CSTH_PSC5415A);
			set_chr_current = bmt_find_closest_level(CSTH_PSC5415A, array_size, ichg);
			register_value = charging_parameter_to_value(CSTH_PSC5415A, array_size, set_chr_current);
			hl7005_set_iocharge(register_value);
			break;
		}
		case DEVICE_PSC5425:{
			array_size = ARRAY_SIZE(CSTH_PSC5425);
			set_chr_current = bmt_find_closest_level(CSTH_PSC5425, array_size, ichg);
			register_value = charging_parameter_to_value(CSTH_PSC5425, array_size, set_chr_current);
			hl7005_set_iocharge(register_value);
			break;
		}
		default:{
			if (ichg <= 350) {
				hl7005_set_io_level(1);
			} else {
				hl7005_set_io_level(0);
				array_size = ARRAY_SIZE(CSTH);
				set_chr_current = bmt_find_closest_level(CSTH, array_size, ichg);
				register_value = charging_parameter_to_value(CSTH, array_size, set_chr_current);
				hl7005_set_iocharge(register_value);
			}
			break;
		}
	}
	return 0;
}

static int mt_charger_get_aicr(struct mtk_charger_info *mchr_info, u32 *aicr)
{

	unsigned int array_size;
	unsigned int register_value;

	array_size = ARRAY_SIZE(INPUT_CSTH);
	register_value = hl7005_get_input_charging_current();
	*aicr = charging_parameter_to_value(INPUT_CSTH, array_size, register_value);
	return 0;
}

static int mt_charger_set_aicr(struct mtk_charger_info *mchr_info, u32 aicr)
{
	struct hl7005all_chg_info *info =
		(struct hl7005all_chg_info *)mchr_info;
	unsigned int set_chr_current;
	unsigned int array_size;
	unsigned int register_value;

	switch(info->dev_model){
		case DEVICE_ETA6937:{
			array_size = ARRAY_SIZE(INPUT_CSTH_ETA6937);
			set_chr_current = bmt_find_closest_level(INPUT_CSTH_ETA6937, array_size, aicr);//lk new arch aicr in ma
			register_value = charging_parameter_to_value(INPUT_CSTH_ETA6937, array_size, set_chr_current);
			hl7005_set_iinlimit_selection(1);
			hl7005_set_input_charging_current2(register_value);
			
			if (aicr > 800) {
				register_value = 0x3;
			} else {
				array_size = ARRAY_SIZE(INPUT_CSTH);
				set_chr_current = bmt_find_closest_level(INPUT_CSTH, array_size, aicr);
				register_value =
				charging_parameter_to_value(INPUT_CSTH, array_size, set_chr_current);
			}
			hl7005_set_input_charging_current(register_value);
			break;
		}
		default:{
			if (aicr > 800) {
				register_value = 0x3;
			} else {
				array_size = ARRAY_SIZE(INPUT_CSTH);
				set_chr_current = bmt_find_closest_level(INPUT_CSTH, array_size, aicr);
				register_value =
				charging_parameter_to_value(INPUT_CSTH, array_size, set_chr_current);
			}
			hl7005_set_input_charging_current(register_value);
		}
	}
	return 0;
}

static int mt_charger_set_mivr(struct mtk_charger_info *mchr_info, u32 mivr)
{
	u8 data = 0;

	dprintf(CRITICAL, "%s: mivr = %d\n", __func__, mivr);
	if (mivr >= 4200){
		data = (mivr - 4200) / 80;
	}
	if (data > 0x7){
		data = 0x7;
	}
	hl7005_set_vsp(data);
	return 0;
}

static int mt_charger_reset_wdt(struct mtk_charger_info *mchr_info)
{
	struct hl7005all_chg_info *info =
		(struct hl7005all_chg_info *)mchr_info;

	return hl7005_reset_watch_dog_timer(info);
}

static int mt_charger_dump_register(struct mtk_charger_info *mchr_info)
{
	struct hl7005all_chg_info *info =
		(struct hl7005all_chg_info *)mchr_info;
	
	int i = 0;
	u8 hl7005_reg[HL7005_REG_NUM + 1] = {0};
		
	if (info->dev_model == DEVICE_ETA6937){
		for (i = 0; i < HL7005_REG_NUM+1; i++) {
			hl7005all_i2c_read_byte(&g_hl7005all_info, i, &hl7005_reg[i]);
		}
		dprintf(CRITICAL,"[HL7005]dump:");
		for (i = 0; i < HL7005_REG_NUM+1; i++) {
			printf("[0x%x]=0x%02x ", i, hl7005_reg[i]);
		}
	}else{
		for (i = 0; i < HL7005_REG_NUM; i++) {
			hl7005all_i2c_read_byte(&g_hl7005all_info, i, &hl7005_reg[i]);
		}
		dprintf(CRITICAL,"[HL7005]dump:");
		for (i = 0; i < HL7005_REG_NUM; i++) {
			printf("[0x%x]=0x%02x ", i, hl7005_reg[i]);
		}
	}
	printf("\n");

	return 0;
}

static int hl7005all_chg_init_setting(struct hl7005all_chg_info *info)
{
#if defined(USE_DTB_NO_DWS)
		//set charge_disable_pin
	mt_set_gpio_mode(info->gpio_sw_en|0x80000000, GPIO_MODE_00);	//define GPIO_SWCHARGER_EN_PIN in dws
	mt_set_gpio_dir(info->gpio_sw_en|0x80000000, GPIO_DIR_OUT);	//define GPIO_SWCHARGER_EN_PIN in dws
	mt_set_gpio_out(info->gpio_sw_en|0x80000000, GPIO_OUT_ZERO);	//define GPIO_SWCHARGER_EN_PIN in dws
#else{
	//set charge_disable_pin
	mt_set_gpio_mode(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_MODE_00);	//define GPIO_SWCHARGER_EN_PIN in dws
	mt_set_gpio_dir(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_DIR_OUT);	//define GPIO_SWCHARGER_EN_PIN in dws
	mt_set_gpio_out(GPIO_SWCHARGER_EN_PIN|0x80000000, GPIO_OUT_ZERO);	//define GPIO_SWCHARGER_EN_PIN in dws
#endif

	dprintf(CRITICAL, "%s: starts\n", __func__);
	switch(info->dev_model){
		case DEVICE_PSC5415A:{
		#if defined(CONFIG_PSC5415A_56MR_SUPPORT)
			hl7005_reg_config_interface(0x06, 0x5A);
		#else //elif defined(CONFIG_PSC5415A_68MR_SUPPORT)
			hl7005_reg_config_interface(0x06, 0x7A);
		#endif
			break;
		}
		case DEVICE_PSC5425:{
		#if defined(CONFIG_PSC5425_40MR_SUPPORT)
			hl7005_reg_config_interface(0x06, 0x7A);
		#else //elif defined(CONFIG_PSC5425_33MR_SUPPORT)
			hl7005_reg_config_interface(0x06, 0x6A);
		#endif
			break;
		}
		case DEVICE_ETA6937:{
			hl7005_reg_config_interface(0x06, 0x8A);
			break;
		}
		default:{
			hl7005_reg_config_interface(0x06, 0x7A);	/* ISAFE = 1250mA, VSAFE = 4.34V */
			break;
		}
	}

	hl7005_reg_config_interface(0x00, 0xC0);	/* kick chip watch dog */
	hl7005_reg_config_interface(0x01, 0xb8);	/* TE=1, CE=0, HZ_MODE=0, OPA_MODE=0 */
	
	switch(info->dev_model){
		case DEVICE_PSC5415A:
			hl7005_reg_config_interface(0x05, 0x04);	//some version require Bit2 must be set 1
			break;
		case DEVICE_ETA6937:
			hl7005_reg_config_interface(0x05, 0x03);	//vsp4.2+n*80mV
			hl7005_reg_config_interface(0x07, 0x0D);	//vsp+0,iinlim 2A
			break;
		case DEVICE_PSC5425:
		default:
			hl7005_reg_config_interface(0x05, 0x03);
			break;
	}

	hl7005_reg_config_interface(0x04, 0x1A);	/* 146mA */

	if (hl7005_get_chip_status() == 0x00){
		mt_charger_set_aicr((struct mtk_charger_info *)&info,100);	//if not charging, set init input 100mA
	}
	
	//hl7005_dump_register(info);
	
	return 0;
}

static int hl7005all_parse_dt(struct hl7005all_chg_info *info)
{	
	int offset = 0;
	const void *data = NULL;
	int len = 0;
	
	offset = fdt_node_offset_by_compatible(g_fdt,0,"halo,hl7005");
	if (offset < 0){
		dprintf(CRITICAL, "[hl7005] Failed to find halo,hl7005 in dtb\n");
		return -1;
	}else{
		data = fdt_getprop(g_fdt, offset, "gpio_swcharger_en_pin", &len);
		if (len > 0 && data){
			info->gpio_sw_en = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[hl7005] gpio_swcharger_en_pin(%d)\n",info->gpio_sw_en);
		}
		data = fdt_getprop(g_fdt, offset, "id", &len);
		if (len > 0 && data){
			info->i2c.id = fdt32_to_cpu(*(unsigned int *)data);
			dprintf(INFO, "[hl7005] i2c id(%d)\n",info->i2c.id);
		}
	}
	return 0;
}

int hl7005all_chg_probe(void)
{
	int ret = 0;
	
	ret = hl7005all_parse_dt(&g_hl7005all_info);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: parse dt failed\n", __func__);
	}

	/* Check primary charger */
	if (!hl7005all_is_hw_exist(&g_hl7005all_info)){
		dprintf(CRITICAL, "%s:probe fail, device not exist\n",__func__);
		return -ENODEV;
	}
	dprintf(CRITICAL, "%s:version %s\n",__func__, HL7005ALL_CHARGER_LK_DRV_VERSION);
	ret = hl7005all_chg_init_setting(&g_hl7005all_info);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: init setting fail\n", __func__);
		return ret;
	}
	mtk_charger_set_info(&(g_hl7005all_info.mchr_info));
	return ret;
}

/*
 * Revision Note
 * 0.0.0
 * (1) Initial release
 */
