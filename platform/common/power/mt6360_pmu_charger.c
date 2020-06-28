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
#include "mt6360_pmu_charger.h"
#include "mtk_charger_intf.h"

#define MT6360_PMU_CHARGER_LK_DRV_VERSION "1.0.0_MTK"

/* ================= */
/* Internal variable */
/* ================= */
struct mt6360_pmu_chg_info {
	struct mtk_charger_info mchr_info;
	struct mt_i2c_t i2c;
	int i2c_log_level;
	u8 vid;
};

enum mt6360_iinlmtsel {
	MT6360_IIMLMTSEL_AICR_3250 = 0,
	MT6360_IIMLMTSEL_CHR_TYPE,
	MT6360_IINLMTSEL_AICR,
	MT6360_IINLMTSEL_LOWER_LEVEL, /* lower of above three */
};

enum mt6360_charging_status {
	MT6360_CHG_STATUS_READY = 0,
	MT6360_CHG_STATUS_PROGRESS,
	MT6360_CHG_STATUS_DONE,
	MT6360_CHG_STATUS_FAULT,
	MT6360_CHG_STATUS_MAX,
};

/* Charging status name */
static const char *mt6360_chg_status_name[MT6360_CHG_STATUS_MAX] = {
	"ready", "progress", "done", "fault",
};

/* ========================= */
/* I2C operations */
/* ========================= */
static int mt6360_i2c_write_byte(struct mt6360_pmu_chg_info *info,
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
	else
		dprintf(info->i2c_log_level, "%s: I2CW[0x%02X] = 0x%02X\n",
			__func__, cmd, data);
	return ret;
}

static int mt6360_i2c_read_byte(struct mt6360_pmu_chg_info *info,
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
		dprintf(info->i2c_log_level, "%s: I2CR[0x%02X] = 0x%02X\n",
			__func__, cmd, ret_data);
		*data = ret_data;
	}
	return ret;
}

static int mt6360_i2c_block_write(struct mt6360_pmu_chg_info *info,
				  u8 cmd, int len, const u8 *data)
{
	struct mt_i2c_t *i2c = &info->i2c;
	unsigned char write_buf[len + 1];

	write_buf[0] = cmd;
	memcpy(&write_buf[1], data, len);
	return i2c_write(i2c, write_buf, len + 1);
}

static int mt6360_i2c_block_read(struct mt6360_pmu_chg_info *info,
				 u8 cmd, int len, u8 *data)
{
	struct mt_i2c_t *i2c = &info->i2c;
	data[0] = cmd;

	return i2c_write_read(i2c, data, len, len);
}

static int mt6360_i2c_update_bits(struct mt6360_pmu_chg_info *info,
				  u8 cmd, u8 mask, u8 data)
{
	int ret = 0;
	u8 reg_data = 0;

	ret = mt6360_i2c_read_byte(info, cmd, &reg_data);
	if (ret != I2C_OK)
		return ret;
	reg_data = reg_data & 0xFF;
	reg_data &= ~mask;
	reg_data |= (data & mask);
	return mt6360_i2c_write_byte(info, cmd, reg_data);
}

static inline int mt6360_set_bit(struct mt6360_pmu_chg_info *info,
				 u8 reg, u8 mask)
{
    return mt6360_i2c_update_bits(info, reg, mask, mask);
}

static inline int mt6360_clr_bit(struct mt6360_pmu_chg_info *info,
				 u8 reg, u8 mask)
{
    return mt6360_i2c_update_bits(info, reg, mask, 0x00);
}

/* ================== */
/* internal functions */
/* ================== */
static bool mt6360_is_hw_exist(struct mt6360_pmu_chg_info *info)
{
	int ret = 0;
	u8 vid = 0, rev_id = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_DEV_INFO, &data);
	if (ret != I2C_OK)
		return false;
	vid = data & 0xF0;
	rev_id = data & 0x0F;
	if (vid != MT6360_VENDOR_ID) {
		dprintf(CRITICAL, "%s: vid is not match(%d)\n", __func__, vid);
		return false;
	}
	dprintf(CRITICAL, "%s: rev_id = %d\n", __func__, rev_id);

	info->mchr_info.device_id = rev_id;
	info->vid = vid;
	return true;
}

static int mt6360_enable_ilim(struct mt6360_pmu_chg_info *info, bool enable)
{
	dprintf(CRITICAL, "%s: enable ilim = %d\n", __func__, enable);
	return (enable ? mt6360_set_bit : mt6360_clr_bit)
		(info, MT6360_PMU_CHG_CTRL3, MT6360_MASK_ILIM_EN);
}

/* Select IINLMTSEL to use AICR */
static int mt6360_select_input_current_limit(
	        struct mt6360_pmu_chg_info *info, enum mt6360_iinlmtsel sel)
{
	dprintf(CRITICAL, "%s: select input current limit = %d\n",
		__func__, sel);
	return mt6360_i2c_update_bits(info,
				      MT6360_PMU_CHG_CTRL2,
				      MT6360_MASK_IINLMTSEL,
				      sel << MT6360_SHFT_IINLMTSEL);
}

static int mt6360_set_cv(struct mt6360_pmu_chg_info *info, u32 uV)
{
	u8 data = 0;

	if (uV >= 3900)
		data = (uV - 3900) / 10;
	if (data > MT6360_VOREG_MAXVAL)
		data = MT6360_VOREG_MAXVAL;
	dprintf(CRITICAL, "%s: cv = %d\n", __func__, uV);
	return mt6360_i2c_update_bits(info,
				      MT6360_PMU_CHG_CTRL4,
				      MT6360_MASK_VOREG,
				      data << MT6360_SHFT_VOREG);
}

static int mt6360_enable_wdt(struct mt6360_pmu_chg_info *info, bool en)
{
	dprintf(CRITICAL, "%s: en = %d\n", __func__, en);
	return (en ? mt6360_set_bit : mt6360_clr_bit)
		(info, MT6360_PMU_CHG_CTRL13, MT6360_MASK_CHG_WDT_EN);
}

static int mt6360_get_charging_status(struct mt6360_pmu_chg_info *info,
				      enum mt6360_charging_status *chg_stat)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_STAT, &data);
	if (ret != I2C_OK)
		return ret;
	*chg_stat = (data & MT6360_MASK_CHG_STAT) >> MT6360_SHFT_CHG_STAT;
	return ret;
}

static int mt6360_get_mivr(struct mt6360_pmu_chg_info *info, u32 *mivr)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL6, &data);
	if (ret != I2C_OK)
		return ret;
	data = (data & MT6360_MASK_MIVR) >> MT6360_SHFT_MIVR;
	*mivr = 3900 + (data * 100);
	return ret;
}

static int mt6360_get_ieoc(struct mt6360_pmu_chg_info *info, u32 *ieoc)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL9, &data);
	if (ret != I2C_OK)
		return ret;
	data = (data & MT6360_MASK_IEOC) >> MT6360_SHFT_IEOC;
	*ieoc = 100 + (data * 50);
	return ret;
}

static int mt6360_is_charging_enable(struct mt6360_pmu_chg_info *info,
				     bool *enable)
{
	int ret = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL2, &data);
	if (ret != I2C_OK)
		return ret;
	*enable = (data & MT6360_MASK_CHG_EN) ? true : false;
	return ret;
}

/* =========================================================== */
/* The following is implementation for interface of mt_charger */
/* =========================================================== */
static int mt_charger_enable_charging(struct mtk_charger_info *mchr_info,
				      bool enable)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;

	return (enable ? mt6360_set_bit : mt6360_clr_bit)
		(info, MT6360_PMU_CHG_CTRL2, MT6360_MASK_CHG_EN);
}

static int mt_charger_get_ichg(struct mtk_charger_info *mchr_info, u32 *ichg)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	int ret = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL7, &data);
	if (ret != I2C_OK)
		return ret;
	data = (data & MT6360_MASK_ICHG) >> MT6360_SHFT_ICHG;
	*ichg = 100 + (data * 100);
	return ret;
}

static int mt_charger_set_ichg(struct mtk_charger_info *mchr_info, u32 ichg)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	u8 data = 0;

	dprintf(CRITICAL, "%s: ichg = %d\n", __func__, ichg);
	if (ichg >= 100)
		data = (ichg - 100) / 100;
	if (data > MT6360_ICHG_MAXVAL)
		data = MT6360_ICHG_MAXVAL;
	return mt6360_i2c_update_bits(info,
				      MT6360_PMU_CHG_CTRL7,
				      MT6360_MASK_ICHG,
				      data << MT6360_SHFT_ICHG);
}

static int mt_charger_get_aicr(struct mtk_charger_info *mchr_info, u32 *aicr)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	int ret = 0;
	u8 data = 0;

	ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL3, &data);
	if (ret != I2C_OK)
		return ret;
	data = (data & MT6360_MASK_AICR) >> MT6360_SHFT_AICR;
	*aicr = 100 + (data * 50);
	return ret;
}

static int mt6360_adc_get_process_val(struct mt6360_pmu_chg_info *info,
				      enum mt6360_adc_channel chan, u32 *val)
{
	int ret = 0;
	u8 data = 0;

	switch (chan) {
	case MT6360_ADC_USBID:
	case MT6360_ADC_VREF_TS:
	case MT6360_ADC_TS:
		*val *= 1250;
		break;
	case MT6360_ADC_TEMP_JC:
		*val -= 40;
		break;
	case MT6360_ADC_VBAT:
	case MT6360_ADC_VSYS:
	case MT6360_ADC_CHG_VDDP:
		*val *= 1250;
		break;
	case MT6360_ADC_VBUSDIV5:
		*val *= 6250;
		break;
	case MT6360_ADC_VBUSDIV2:
	case MT6360_ADC_IBAT:
		*val *= 2500;
		break;
	case MT6360_ADC_IBUS:
		ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_CTRL3, &data);
		if (ret < 0)
			return ret;
		if (((data & 0xfc) >> 2) < 0x6)
			*val *= 1900;
		else
			*val *= 2500;
		break;
	default:
		break;
	}
	return ret;
}

static int mt_charger_get_adc(struct mtk_charger_info *mchr_info,
			      int chan, u32 *val)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	u8 tmp[2], rpt[3];
	u8 data = 0;
	int i, ret = 0, max_retry_cnt = 20;

	dprintf(CRITICAL, "%s: ++\n", __func__);
	if (chan >= MT6360_ADC_MAX) {
		dprintf(CRITICAL, "%s: undeine channel(%d)\n",
			__func__, chan);
		ret = -EINVAL;
		goto err_adc_init;
	}
	/* first reset all channels and adc en */
	memset(tmp, 0, sizeof(tmp));
	ret = mt6360_i2c_block_write(info, MT6360_PMU_ADC_CONFIG, 3, tmp);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: reset all channels fail\n", __func__);
		goto err_adc_init;
	}
	/* enable adc_donei irq */
	ret = mt6360_clr_bit(info, MT6360_PMU_CHG_MASK6, MT6360_MASK_ADC_DONE);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: unmask adc_donei fail\n", __func__);
		goto err_adc_conv;
	}
	/* select preferred channel : vbus */
	ret = mt6360_i2c_update_bits(info,
				     MT6360_PMU_ADC_RPT_1, 0xf0, chan << 4);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: unmask adc_donei fail\n", __func__);
		goto err_adc_conv;
	}
	/* enable vbus adc channel and adc_en */
	tmp[0] |= (1 << 7);
	if ((chan / 8) > 0)
		tmp[0] |= (1 << (chan % 8));
	else
		tmp[1] |= (1 << (chan % 8));
	ret = mt6360_i2c_block_write(info, MT6360_PMU_ADC_CONFIG, 2, tmp);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: reset all channels fail\n", __func__);
		goto err_adc_conv;
	}
	/* wait adc conversion done */
	for (i = 0; i < max_retry_cnt; i++) {
		mdelay(10);
		/* read adc conversion donei event */
		ret = mt6360_i2c_read_byte(info, MT6360_PMU_CHG_IRQ6, &data);
		if (ret < 0)
			goto err_adc_conv;
		if (!(data & MT6360_MASK_ADC_DONE))
			continue;
		memset(rpt, 0, sizeof(rpt));
		ret = mt6360_i2c_block_read(info, MT6360_PMU_ADC_RPT_1, 3, rpt);
		if (ret < 0)
			goto err_adc_conv;
		if ((rpt[0] & 0x0f) != chan) {
			dprintf(CRITICAL, "%s: not want channel report(%d)\n",
				__func__, rpt[0]);
			continue;
		}
		*val = ((rpt[1] << 8) | rpt[2]);
		ret = mt6360_adc_get_process_val(info, chan, val);
		break;
	}
	if (i == max_retry_cnt) {
		dprintf(CRITICAL, "%s: reach adc retry cnt\n", __func__);
		ret = -EBUSY;
		goto err_adc_conv;
	}
err_adc_conv:
	tmp[0] &= ~(0x7);
	tmp[1] = 0;
	mt6360_i2c_block_write(info, MT6360_PMU_ADC_CONFIG, 2, tmp);
	mt6360_set_bit(info, MT6360_PMU_CHG_MASK6, MT6360_MASK_ADC_DONE);
err_adc_init:
	dprintf(CRITICAL, "%s: --\n", __func__);
	return ret;
}

static int mt_charger_get_vbus(struct mtk_charger_info *mchr_info, u32 *vbus)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	int ret = 0;
	ret = mt_charger_get_adc(info, MT6360_ADC_VBUSDIV5, vbus);
	return ret;
}

static int mt_charger_set_aicr(struct mtk_charger_info *mchr_info, u32 aicr)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	u8 data = 0;

	dprintf(CRITICAL, "%s: aicr = %d\n", __func__, aicr);
	if (aicr >= 100)
		data = (aicr - 100) / 50;
	if (data > MT6360_AICR_MAXVAL)
		data = MT6360_AICR_MAXVAL;
	return mt6360_i2c_update_bits(info,
				      MT6360_PMU_CHG_CTRL3,
				      MT6360_MASK_AICR,
				      data << MT6360_SHFT_AICR);
}

static int mt_charger_set_mivr(struct mtk_charger_info *mchr_info, u32 mivr)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	u8 data = 0;

	dprintf(CRITICAL, "%s: mivr = %d\n", __func__, mivr);
	if (mivr >= 3900)
		data = (mivr - 3900) / 100;
	if (data > MT6360_MIVR_MAXVAL)
		data = MT6360_MIVR_MAXVAL;
	return mt6360_i2c_update_bits(info,
				      MT6360_PMU_CHG_CTRL6,
				      MT6360_MASK_MIVR,
				      data << MT6360_SHFT_MIVR);
}

static int mt_charger_enable_power_path(struct mtk_charger_info *mchr_info,
					bool enable)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;

	dprintf(CRITICAL, "%s: enable = %d\n", __func__, enable);
	return (enable ? mt6360_clr_bit : mt6360_set_bit)
		(info, MT6360_PMU_CHG_CTRL1, MT6360_MASK_FORCE_SLEEP);
}

static int mt_charger_reset_pumpx(struct mtk_charger_info *mchr_info,
				  bool reset)
{
	return mt_charger_set_aicr(mchr_info, reset ? 100 : 500);
}

static int mt_charger_enable_wdt(struct mtk_charger_info *mchr_info, bool en)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;

	return mt6360_enable_wdt(info, en);
}

static int mt_charger_dump_register(struct mtk_charger_info *mchr_info)
{
	struct mt6360_pmu_chg_info *info =
		(struct mt6360_pmu_chg_info *)mchr_info;
	enum mt6360_charging_status chg_status = MT6360_CHG_STATUS_READY;
	u32 ichg = 0, aicr = 0, mivr = 0, ieoc = 0;
	bool chg_en = 0;
	int ret = 0;

	ret = mt_charger_get_ichg(mchr_info, &ichg);
	ret = mt_charger_get_aicr(mchr_info, &aicr);
	ret = mt6360_get_mivr(info, &mivr);
	ret = mt6360_get_ieoc(info, &ieoc);
	ret = mt6360_is_charging_enable(info, &chg_en);
	ret = mt6360_get_charging_status(info, &chg_status);
	dprintf(CRITICAL,
		"%s: ICHG = %dmA, AICR = %dmA, MIVR = %dmV, IEOC = %dmA\n",
		__func__, ichg, aicr, mivr, ieoc);

	dprintf(CRITICAL, "%s: CHG_EN = %d, CHG_STATUS = %s\n",
		__func__, chg_en, mt6360_chg_status_name[chg_status]);
	return ret;
}

static struct mtk_charger_ops mt6360_mchr_ops = {
	.dump_register = mt_charger_dump_register,
	.enable_charging = mt_charger_enable_charging,
	.get_ichg = mt_charger_get_ichg,
	.set_ichg = mt_charger_set_ichg,
	.get_aicr = mt_charger_get_aicr,
	.get_adc = mt_charger_get_adc,
	.get_vbus = mt_charger_get_vbus,
	.set_aicr = mt_charger_set_aicr,
	.set_mivr = mt_charger_set_mivr,
	.enable_power_path = mt_charger_enable_power_path,
	.reset_pumpx = mt_charger_reset_pumpx,
	.enable_wdt = mt_charger_enable_wdt,
};

/* Info of primary charger */
static struct mt6360_pmu_chg_info g_mpci = {
	.mchr_info = {
		.name = "primary_charger",
		.device_id = -1,
		.mchr_ops = &mt6360_mchr_ops,
	},
	.i2c = {
		.id = I2C5,
		.addr = MT6360_SLAVE_ADDR,
		.mode = HS_MODE,
		.speed = 3400,
		.pushpull = true,
	},
	.i2c_log_level = INFO,
	.vid = 0x00,
};

static int mt6360_chg_init_setting(struct mt6360_pmu_chg_info *info)
{
	int ret = 0;

	dprintf(CRITICAL, "%s: starts\n", __func__);
	ret = mt6360_enable_wdt(info, true);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: enable wdt failed\n", __func__);
		goto out;
	}
	/* Select input current limit to referenced from AICR */
	ret = mt6360_select_input_current_limit(info,
		MT6360_IINLMTSEL_AICR);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: select input current limit failed\n",
			__func__);
		goto out;
	}
	mdelay(5);
	/* Disable HW iinlimit, use SW */
	ret = mt6360_enable_ilim(info, false);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: disable ilim failed\n", __func__);
		goto out;
	}
	ret = mt_charger_set_ichg(&info->mchr_info, 2000);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: set ichg failed\n", __func__);
		goto out;
	}
	ret = mt_charger_set_aicr(&info->mchr_info, 500);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: set aicr failed\n", __func__);
		goto out;
	}
	ret = mt_charger_set_mivr(&info->mchr_info, 4500);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: set mivr failed\n", __func__);
		goto out;
	}
	ret = mt6360_set_cv(info, 4350);
	if (ret < 0)
		dprintf(CRITICAL, "%s: set cv failed\n", __func__);
	/* Disable sys drop improvement for download mode */
	ret = mt6360_clr_bit(info, MT6360_PMU_CHG_CTRL20, MT6360_MASK_EN_SDI);
	if (ret < 0)
		dprintf(CRITICAL,
			"%s: disable sys drop improve fail\n", __func__);
out:
	return ret;
}

int mt6360_chg_probe(void)
{
	int ret = 0;

	/* Check primary charger */
	if (!mt6360_is_hw_exist(&g_mpci))
		return -ENODEV;
	dprintf(CRITICAL, "%s: %s\n",
		__func__, MT6360_PMU_CHARGER_LK_DRV_VERSION);
	ret = mt6360_chg_init_setting(&g_mpci);
	if (ret < 0) {
		dprintf(CRITICAL, "%s: init setting fail\n", __func__);
		return ret;
	}
	mtk_charger_set_info(&(g_mpci.mchr_info));
	return ret;
}

/*
 * Revision Note
 * 1.0.0
 * (1) Initial release
 */
