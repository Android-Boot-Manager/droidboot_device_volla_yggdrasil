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

#include "msdc.h"
#include "msdc_io.h"

#if defined(MMC_MSDC_DRV_CTP)
#include <common.h>
#include "api.h"    //For invocation cache_clean_invalidate()
#include "cache_api.h"  //For invocation cache_clean_invalidate()
#endif

#if defined(MMC_MSDC_DRV_LK)
#include <kernel/event.h>
#include <platform/mt_irq.h>
#endif

#if defined(MMC_MSDC_DRV_CTP)
#include "gpio.h"

#if defined(MSDC_USE_DCM)
#include "dcm.h"
#endif

#if !defined(FPGA_PLATFORM)
#include "pmic.h"
#include "clock_manager.h"
#endif
#endif


////////////////////////////////////////////////////////////////////////////////
//
// Section 1. Power Control -- Common for ASIC and FPGA
//
////////////////////////////////////////////////////////////////////////////////
//Note: MTKDRV_XXX are used by CTP
#if defined(MTKDRV_PMIC) && defined(MTKDRV_PMIC_WRAP) && defined(MTKDRV_I2C)
#include <upmu_hw.h>
#else
#define pmic_config_interface(RegNum, val, MASK, SHIFT)
#define pmic_read_interface(RegNum, val, MASK, SHIFT)
#endif

#if defined(FPGA_PLATFORM)
#define PWR_GPIO        (0x10001E84) //Select Power On/Off
#define PWR_GPIO_EO     (0x10001E88) //Select Output/Input Direction

#define PWR_MASK_CARD   (0x1 << 8)   //Vcc
#define PWR_MASK_VOL_18 (0x1 << 9)   //CLK/CMD/DAT for eMMC/SD +  Vccq for eMMC
#define PWR_MASK_VOL_33 (0x1 << 10)  //CLK/CMD/DAT for eMMC/SD +  Vccq for eMMC
#define PWR_MASK_L4     (0x1 << 11)
#define PWR_MSDC        (PWR_MASK_CARD | PWR_MASK_VOL_18 | PWR_MASK_VOL_33 | PWR_MASK_L4)

//#define FPGA_GPIO_DEBUG
static void msdc_clr_gpio(u32 bits)
{
	u32 l_val = 0;
	char *name;

	switch (bits) {
		case PWR_MASK_CARD:
			name = "card pwr";
			break;
		case PWR_MASK_VOL_18:
			name = "SD/eMMC bus pwer 1.8V";
			break;
		case PWR_MASK_VOL_33:
			name = "SD/eMMC bus pwer 3.3V";
			break;
		case PWR_MASK_L4:
			name = "L4";
			break;
		default:
			msdc_pr_err("[%s:%s]invalid value: 0x%x\n", __FILE__, __func__, bits);
			return;
	}

	MSDC_GET_FIELD(PWR_GPIO_EO, bits, l_val);
	//msdc_pr_info("====%s====%d\n", name, l_val);
	if (0 == l_val) {
		msdc_pr_info("check me! gpio for %s is input\n", name);
		MSDC_SET_FIELD(PWR_GPIO_EO, bits, 1);
	}
	/* check for set before */
	if (bits &  MSDC_READ32(PWR_GPIO)) {
		msdc_pr_info("clear %s\n", name);
		MSDC_SET_FIELD(PWR_GPIO, bits, 0);
		//l_val = MSDC_READ32(PWR_GPIO);
	}

#ifdef FPGA_GPIO_DEBUG
	{
		u32 val = 0;
		val = MSDC_READ32(PWR_GPIO);
		msdc_pr_err("[clr]PWR_GPIO[8-11]:0x%x\n", val);
		val = MSDC_READ32(PWR_GPIO_EO);
		msdc_pr_err("[clr]GPIO_DIR[8-11]	   :0x%x\n", val);
	}
#endif
}

static void msdc_set_gpio(u32 bits)
{
	u32 l_val = 0;
	char *name;

	switch (bits) {
		case PWR_MASK_CARD:
			name = "card pwr";
			break;
		case PWR_MASK_VOL_18:
			name = "SD/eMMC bus pwer 1.8V";
			break;
		case PWR_MASK_VOL_33:
			name = "SD/eMMC bus pwer 3.3V";
			break;
		case PWR_MASK_L4:
			name = "L4";
			break;
		default:
			msdc_pr_err("[%s:%s]invalid value: 0x%x\n", __FILE__, __func__, bits);
			return;
	}

	MSDC_GET_FIELD(PWR_GPIO_EO, bits, l_val);
	//msdc_pr_info("====%s====%d\n", name, l_val);
	if (0 == l_val) {
		msdc_pr_info("check me! gpio for %s is input\n", name);
		MSDC_SET_FIELD(PWR_GPIO_EO, bits, 1);
	}
	/* check for set before */
	if ((bits &  MSDC_READ32(PWR_GPIO))==0) {
		msdc_pr_info("Set %s\n", name);
		MSDC_SET_FIELD(PWR_GPIO, bits, 1);
		//l_val = MSDC_READ32(PWR_GPIO);
	}

#ifdef FPGA_GPIO_DEBUG
	{
		u32 val = 0;
		val = MSDC_READ32(PWR_GPIO);
		msdc_pr_err("[set]PWR_GPIO[8-11]:0x%x\n", val);
		val = MSDC_READ32(PWR_GPIO_EO);
		msdc_pr_err("[set]GPIO_DIR[8-11]	   :0x%x\n", val);
	}
#endif
}
#endif

#if defined(MMC_MSDC_DRV_CTP) && !defined(FPGA_PLATFORM)
u32 hwPowerOn(MSDC_POWER powerId, MSDC_POWER_VOLTAGE powerVolt)
{
	u32 ret;

	switch (powerId) {
		case MSDC_VEMC:
			if (powerVolt == VOL_3000) {
				pmic_config_interface(REG_VEMC_VOSEL, VEMC_VOSEL_3V, MASK_VEMC_VOSEL, SHIFT_VEMC_VOSEL);
				if (EMMC_VOL_ACTUAL != VOL_3000) {
					pmic_config_interface(REG_VEMC_VOSEL_CAL,
						VEMC_VOSEL_CAL_mV(EMMC_VOL_ACTUAL - VOL_3000),
						MASK_VEMC_VOSEL_CAL, SHIFT_VEMC_VOSEL_CAL);
				}
			} else if (powerVolt == VOL_3300) {
				pmic_config_interface(REG_VEMC_VOSEL, VEMC_VOSEL_3V3, MASK_VEMC_VOSEL, SHIFT_VEMC_VOSEL);
			} else {
				msdc_pr_err("Not support to Set VEMC_3V3 power to %d\n", powerVolt);
			}
			pmic_config_interface(REG_VEMC_EN, 1, MASK_VEMC_EN, SHIFT_VEMC_EN);
			break;

		case MSDC_VMC:
			if (powerVolt == VOL_3300 || powerVolt == VOL_3000)
				pmic_config_interface(REG_VMC_VOSEL, VMC_VOSEL_3V, MASK_VMC_VOSEL, SHIFT_VMC_VOSEL);
			else if (powerVolt == VOL_1800)
				pmic_config_interface(REG_VMC_VOSEL, VMC_VOSEL_1V8, MASK_VMC_VOSEL, SHIFT_VMC_VOSEL);
			else
				msdc_pr_err("Not support to Set VMC power to %d\n", powerVolt);

			pmic_config_interface(REG_VMC_EN, 1, MASK_VMC_EN, SHIFT_VMC_EN);
			break;

		case MSDC_VMCH:
			if (powerVolt == VOL_3000) {
				pmic_config_interface(REG_VMCH_VOSEL, VMCH_VOSEL_3V, MASK_VMCH_VOSEL, SHIFT_VMCH_VOSEL);
				if (SD_VOL_ACTUAL != VOL_3000) {
					pmic_config_interface(REG_VMCH_VOSEL_CAL,
						VMCH_VOSEL_CAL_mV(SD_VOL_ACTUAL - VOL_3000),
						MASK_VMCH_VOSEL_CAL, SHIFT_VMCH_VOSEL_CAL);
				}
			} else if (powerVolt == VOL_3300) {
				pmic_config_interface(REG_VMCH_VOSEL, VMCH_VOSEL_3V3, MASK_VMCH_VOSEL, SHIFT_VMCH_VOSEL);
			} else {
				msdc_pr_err("Not support to Set VMCH power to %d\n", powerVolt);
			}
			pmic_config_interface(REG_VMCH_EN, 1, MASK_VMCH_EN, SHIFT_VMCH_EN);
			break;

		default:
			msdc_pr_err("Not support to Set %d power on\n", powerId);
			break;
	}

	return 0;
}

u32 hwPowerDown(MSDC_POWER powerId)
{
	switch (powerId) {
		case MSDC_VEMC:
			pmic_config_interface(REG_VEMC_EN, 0, MASK_VEMC_EN, SHIFT_VEMC_EN);
			break;
		case MSDC_VMC:
			pmic_config_interface(REG_VMC_EN, 0, MASK_VMC_EN, SHIFT_VMC_EN);
			break;
		case MSDC_VMCH:
			pmic_config_interface(REG_VMCH_EN, 0, MASK_VMCH_EN, SHIFT_VMCH_EN);
			break;
		default:
			msdc_pr_err("Not support to Set %d power down\n", powerId);
			break;
	}
	return 0;
}

static u32 msdc_ldo_power(u32 on, MSDC_POWER powerId, MSDC_POWER_VOLTAGE powerVolt, u32 *status)
{
	if (on) { // want to power on
		if (*status == 0) {  // can power on
			msdc_pr_info("msdc LDO<%d> power on<%d>\n", powerId, powerVolt);
			hwPowerOn(powerId, powerVolt);
			*status = powerVolt;
		} else if (*status == powerVolt) {
			msdc_pr_info("msdc LDO<%d><%d> power on again!\n", powerId, powerVolt);
		} else { // for sd3.0 later
			msdc_pr_info("msdc LDO<%d> change<%d>	to <%d>\n", powerId, *status, powerVolt);
			hwPowerDown(powerId);
			hwPowerOn(powerId, powerVolt);
			*status = powerVolt;
		}
	} else {  // want to power off
		if (*status != 0) {  // has been powerred on
			msdc_pr_info("msdc LDO<%d> power off\n", powerId);
			hwPowerDown(powerId);
			*status = 0;
		} else {
			msdc_pr_info("LDO<%d>	not power on\n", powerId);
		}
	}

	return 0;
}
#endif /* defined(MMC_MSDC_DRV_CTP) && !defined(FPGA_PLATFORM) */

#if defined(FPGA_PLATFORM)
void msdc_card_power(struct mmc_host *host, u32 on)
{
	MSG(CFG, "[SD%d] Turn %s card power \n", host->id, on ? "on" : "off");
	switch (host->id) {
		case 0:
			if (on) {
				//On V6/V7 FPGA, card power need to be on/off
				//On HAPS FPGA, card power maybe always on.
				//Left this code for safety prevent from card power not always not.
				msdc_set_gpio(PWR_MASK_CARD);
			} else {
				msdc_clr_gpio(PWR_MASK_CARD);
			}
			mdelay(10);
			break;
		default:
			//No MSDC1 in FPGA
			break;
	}

	//add for fpga debug
	msdc_set_gpio(PWR_MASK_L4);
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
	MSG(CFG, "[SD%d] Turn %s host power \n", host->id, on ? "on" : "off");

	// GPO[3:2] = {LVL_PWR33, LVL_PWR18};
	msdc_clr_gpio(PWR_MASK_VOL_18);
	msdc_clr_gpio(PWR_MASK_VOL_33);

	switch (host->id) {
		case 0:
			if (on) {
				if (level == VOL_1800)
					msdc_set_gpio(PWR_MASK_VOL_18);
				else
					msdc_set_gpio(PWR_MASK_VOL_33);
			}
			mdelay(10);
			break;
		default:
			//No MSDC1 in FPGA
			break;
	}

	//add for fpga debug
	msdc_set_gpio(PWR_MASK_L4);
}
#else
void msdc_card_power(struct mmc_host *host, u32 on)
{
	//Only CTP support power on/off for verification purose.
	//Preload and LK need not touch power since it is default on
	MSG(CFG, "[SD%d] Turn %s card power \n", host->id, on ? "on" : "off");

#if defined(MMC_MSDC_DRV_CTP)
	switch (host->id) {
		case 0:
			if (msdc_cap[0].power_flash == 1) {
				msdc_cap[0].power_flash = VOL_3000;
			} else {
				msdc_ldo_power(on, MSDC_VEMC, VOL_3000, &msdc_cap[0].power_flash);
				mdelay(10);
			}
			break;
		case 1:
			if (msdc_cap[1].power_flash == 1) {
				msdc_cap[1].power_flash = VOL_3000;
			} else {
				msdc_ldo_power(on, MSDC_VMCH, VOL_3000, &msdc_cap[1].power_flash);
				mdelay(10);
			}
			break;
		case 2:
			if (on && (host->cur_pwr < VOL_2000) ) {
				//For MTK SDIO device initialized at 1.8V, use external source of 1.8V
				//So we just turn VMCH off
				on = 0;
			}
			msdc_ldo_power(on, MSDC_VMCH, VOL_3000, &msdc_cap[2].power_flash);
			mdelay(10);
			break;
		default:
			break;
	}
#endif
}

void msdc_host_power(struct mmc_host *host, u32 on, u32 level)
{
	//Only CTP support power on/off for verification purose.
	//Preload and LK need not touch power since it is default on
#if defined(MMC_MSDC_DRV_CTP)
	u32 card_on = on;
	if (host->id != 0) {
		host->cur_pwr = level;
	}
	msdc_set_tdsel_wrap(host);
	msdc_set_rdsel_wrap(host);
	msdc_set_driving(host, &msdc_cap[host->id], (level == VOL_1800));

	MSG(CFG, "[SD%d] Turn %s host power \n", host->id, on ? "on" : "off");

	switch (host->id) {
		case 0:
			//no need change;
			break;
		case 1:
			if (msdc_cap[1].power_io == 1) {
				msdc_cap[1].power_io = level;
			} else {
				msdc_ldo_power(on, MSDC_VMC, level, &msdc_cap[1].power_io);
				mdelay(10);
			}
			break;
		case 3:
			// no need change;
			break;
		default:
			break;
	}
#endif
}

void msdc_dump_ldo_sts(struct mmc_host *host)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
	u32 ldo_en = 0, ldo_vol = 0;

	pmic_read_interface(REG_VEMC_EN, &ldo_en, MASK_VEMC_EN,
	                    SHIFT_VEMC_EN);
	pmic_read_interface(REG_VEMC_VOSEL, &ldo_vol, MASK_VEMC_VOSEL,
	                    SHIFT_VEMC_VOSEL);
	msdc_pr_err(" VEMC_EN=0x%x, should:1b'1, "
	       "VEMC_VOL=0x%x, should:1b'0(3.0V),1b'1(3.3V)\n",
	       ldo_en, ldo_vol);
#endif
}
#endif

void msdc_power(struct mmc_host *host, u8 mode)
{
	if (mode == MMC_POWER_ON || mode == MMC_POWER_UP) {
		msdc_pin_config(host, MSDC_PIN_PULL_UP);
		msdc_host_power(host, 1, host->cur_pwr);
		msdc_card_power(host, 1);
		msdc_clock(host, 1);
	} else {
		msdc_clock(host, 0);
		msdc_host_power(host, 0, host->cur_pwr);
		msdc_card_power(host, 0);
		msdc_pin_config(host, MSDC_PIN_PULL_DOWN);
	}
}


/**************************************************************/
/* Section 2: Clock                    */
/**************************************************************/
#if !defined(FPGA_PLATFORM)
u32 hclks_msdc0[] = { MSDC0_SRC_0, MSDC0_SRC_1, MSDC0_SRC_2, MSDC0_SRC_3,
		      MSDC0_SRC_4, MSDC0_SRC_5, MSDC0_SRC_6, MSDC0_SRC_7};

/* msdc1/2 clock source reference value is 200M */
u32 hclks_msdc1[] = { MSDC1_SRC_0, MSDC1_SRC_1, MSDC1_SRC_2, MSDC1_SRC_3,
		      MSDC1_SRC_4, MSDC1_SRC_5, MSDC1_SRC_6, MSDC1_SRC_7};

u32 hclks_msdc3[] = { MSDC3_SRC_0, MSDC3_SRC_1, MSDC3_SRC_2, MSDC3_SRC_3,
		      MSDC3_SRC_4, MSDC3_SRC_5, MSDC3_SRC_6, MSDC3_SRC_7};

u32 *hclks_msdc_all[] = {
	hclks_msdc0,
	hclks_msdc1,
	hclks_msdc3,
};

void msdc_dump_clock_sts(struct mmc_host *host)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
	msdc_pr_info(" msdcpll en[0x10212250][bit0 should be 0x1]=0x%x\n", MSDC_READ32(0x10212250));
	msdc_pr_info(" msdcpll on[0x1021225C][bit0~1 should be 0x1]=0x%x\n", MSDC_READ32(0x1021225C));

	msdc_pr_info("MSDC0 CLK_MUX[0x10210140]= 0x%x (bit 16~18), CLK_CG[0x11010290] = %d, %d (bit 0,24)\n",
		/* mux at bits16~18 */
		(MSDC_READ32(0x10210140) >> 16) & 7,
		/* cg at bit 0, 24 */
		(MSDC_READ32(0x11010290) >> 0) & 1,
		(MSDC_READ32(0x11010290) >> 24) & 1);
	msdc_pr_info("MSDC1 CLK_MUX[0x10210140]= 0x%x (bit 24~26), CLK_CG[0x11010290] = %d\n",
		/* mux at bits24~26 */
		(MSDC_READ32(0x10210140) >> 24) & 7,
		/* mux at bit 1 */
		(MSDC_READ32(0x11010290) >> 1) & 1);
	msdc_pr_info("MSDC3 CLK_MUX[0x10210150]= 0x%x (bit 8~10), CLK_CG[0x11010290] = %d\n",
		/* mux at bits8~10 */
		(MSDC_READ32(0x10210150) >> 8) & 7,
		/* mux at bit 3 */
		(MSDC_READ32(0x11010290) >> 3) & 1);
#endif
}
#endif

void msdc_clock(struct mmc_host *host, int on)
{
	//Only CTP need enable/disable clock
	//Preloader will turn on clock with predefined clock source and module need not to touch clock setting.
	//LK will use preloader's setting and need not to touch clock setting.
#if !defined(FPGA_PLATFORM)
#if defined(MMC_MSDC_DRV_CTP)
	//int clk_id[MSDC_MAX_NUM] = {MT_CG_INFRA_MSDC_0, MT_CG_INFRA_MSDC_1, MT_CG_INFRA_MSDC_2, MT_CG_INFRA_MSDC_3};
	MSG(CFG, "[SD%d] Turn %s %s clock \n", host->id, on ? "on" : "off", "host");

	if (on) {
		//INFRA_enable_clock(clk_id[host->id]);
		if (host->id == 0) {
			MSDC_SET_BIT32(MSDC0_CLOCK_UNGATE_REG, MSDC0_CLOCK_CG_SECURE);
			MSDC_SET_BIT32(MSDC0_CLOCK_UNGATE_REG, MSDC0_CLOCK_CG_NORMAL);
			MSDC_SET_BIT32(MSDC0_CLOCK_UNGATE_REG, MSDC0_CLOCK_CG_MD);
			if (IF_CHIP_VER2())
				MSDC_SET_BIT32(MSDC0_CLOCK_UNGATE_REG, MSDC0_CLOCK_SRC_CG);
		} else if (host->id == 1) {
			MSDC_SET_BIT32(MSDC1_CLOCK_UNGATE_REG, MSDC1_CLOCK_CG);
			if (IF_CHIP_VER2())
				MSDC_SET_BIT32(MSDC1_CLOCK_UNGATE_REG, MSDC1_CLOCK_SRC_CG);
		} else if (host->id == 3) {
			MSDC_SET_BIT32(MSDC3_CLOCK_UNGATE_REG, MSDC3_CLOCK_CG);
			if (IF_CHIP_VER2())
				MSDC_SET_BIT32(MSDC3_CLOCK_UNGATE_REG, MSDC3_CLOCK_SRC_CG);
		}
	} else {
		//INFRA_disable_clock(clk_id[host->id]);
		if (host->id == 0) {
			MSDC_SET_BIT32(MSDC0_CLOCK_GATE_REG, MSDC0_CLOCK_CG_SECURE);
			MSDC_SET_BIT32(MSDC0_CLOCK_GATE_REG, MSDC0_CLOCK_CG_NORMAL);
			MSDC_SET_BIT32(MSDC0_CLOCK_GATE_REG, MSDC0_CLOCK_CG_MD);
			if (IF_CHIP_VER2())
				MSDC_SET_BIT32(MSDC0_CLOCK_GATE_REG, MSDC0_CLOCK_SRC_CG);
		} else if (host->id == 1) {
			MSDC_SET_BIT32(MSDC1_CLOCK_GATE_REG, MSDC1_CLOCK_CG);
			if (IF_CHIP_VER2())
				MSDC_SET_BIT32(MSDC1_CLOCK_GATE_REG, MSDC1_CLOCK_SRC_CG);
		} else if (host->id == 3) {
			MSDC_SET_BIT32(MSDC3_CLOCK_GATE_REG, MSDC3_CLOCK_CG);
			if (IF_CHIP_VER2())
				MSDC_SET_BIT32(MSDC3_CLOCK_GATE_REG, MSDC3_CLOCK_SRC_CG);
		}
	}
#endif

	msdc_dump_clock_sts(host);

#endif
}

/* perloader will pre-set msdc pll and the mux channel of msdc pll */
/* note: pll will not changed */
void msdc_config_clksrc(struct mmc_host *host, int clksrc)
{
#if !defined(FPGA_PLATFORM)
	#if defined(MMC_MSDC_DRV_PRELOADER) || defined(MMC_MSDC_DRV_LK)
	host->pll_mux_clk = MSDC0_CLKSRC_DEFAULT;
	host->src_clk = msdc_get_hclk(host->id, MSDC0_CLKSRC_DEFAULT);

	if (host->id == 0)
		host->src_clk = MSDC0_E1_CLKSRC_FREQ;
	else if (host->id == 1)
		host->src_clk = MSDC1_E1_CLKSRC_FREQ;
	#endif

	#if defined(MMC_MSDC_DRV_CTP)
	if (host->card && mmc_card_hs400(host->card)) {
		/* if card is in hs400 mode, force clk source to be 400MHz */
		clksrc = MSDC0_CLKSRC_400MHZ;
	}
	host->pll_mux_clk = clksrc;
	host->src_clk = msdc_get_hclk(host->id, clksrc);

	/* The following section shall be confirmed by clock owner during porting */
	if (host->id == 0) {
		MSDC_SET_FIELD((TOPCKGEN_BASE + 0x0140), 0x7 << 16, host->pll_mux_clk);
	} else if (host->id == 1) {
		MSDC_SET_FIELD((TOPCKGEN_BASE + 0x0140), 0x7 << 24, host->pll_mux_clk);
	} else if (host->id == 3) {
		MSDC_SET_FIELD((TOPCKGEN_BASE + 0x0150), 0x7 << 8, host->pll_mux_clk);
	}
	MSDC_WRITE32(TOPCKGEN_BASE+0x04, 0x07FFFFFF);
	#endif
#else
	host->src_clk = msdc_get_hclk(host->id, clksrc);
#endif

}



/**************************************************************/
/* Section 3: GPIO and Pad                    */
/**************************************************************/
#if !defined(FPGA_PLATFORM)
void msdc_dump_padctl_by_id(u32 id)
{
#if defined(MTK_MSDC_BRINGUP_DEBUG)
	if (id == 0) {
		msdc_pr_err("MSDC0 MODE16[%x] =0x%x\tshould: 0x222121?? or 0x111111??n",
			MSDC0_GPIO_MODE16, MSDC_READ32(MSDC0_GPIO_MODE16));
		msdc_pr_err("MSDC0 MODE17[%x] =0x%x\tshould: 0x??121122 or 0x??111111\n",
			MSDC0_GPIO_MODE17, MSDC_READ32(MSDC0_GPIO_MODE17));
		msdc_pr_err("MSDC0 IES   [%x] =0x%x\tshould: 0x??????1f\n",
			MSDC0_GPIO_IES_ADDR, MSDC_READ32(MSDC0_GPIO_IES_ADDR));
		msdc_pr_err("MSDC0 SMT   [%x] =0x%x\tshould: 0x??????1f\n",
			MSDC0_GPIO_SMT_ADDR, MSDC_READ32(MSDC0_GPIO_SMT_ADDR));
		msdc_pr_err("MSDC0 TDSEL0[%x] =0x%x\n",
			MSDC0_GPIO_TDSEL0_ADDR,
			MSDC_READ32(MSDC0_GPIO_TDSEL0_ADDR));
		msdc_pr_err("MSDC0 RDSEL0[%x] =0x%x\n",
			MSDC0_GPIO_RDSEL0_ADDR,
			MSDC_READ32(MSDC0_GPIO_RDSEL0_ADDR));
		msdc_pr_err("MSDC0 DRV0  [%x] =0x%x\n",
			MSDC0_GPIO_DRV0_ADDR,
			MSDC_READ32(MSDC0_GPIO_DRV0_ADDR));
		msdc_pr_err("PUPD/R1/R0: dat/cmd:0/0/1, clk/dst: 1/1/0\n");
		msdc_pr_err("MSDC0 PUPD0 [%x] =0x%x\tshould: 0x11111611\n",
			MSDC0_GPIO_PUPD0_ADDR,
			MSDC_READ32(MSDC0_GPIO_PUPD0_ADDR));
		msdc_pr_err("MSDC0 PUPD1 [%x] =0x%x\tshould: 0x????1161\n",
			MSDC0_GPIO_PUPD1_ADDR,
			MSDC_READ32(MSDC0_GPIO_PUPD1_ADDR));

	} else if (id == 1) {
		msdc_pr_err("MSDC1 MODE4 [%x] =0x%x\tshould: 0x111?????\n",
			MSDC1_GPIO_MODE4, MSDC_READ32(MSDC1_GPIO_MODE4));
		msdc_pr_err("MSDC1 MODE5 [%x] =0x%x\tshould: 0x?????111\n",
			MSDC1_GPIO_MODE5, MSDC_READ32(MSDC1_GPIO_MODE5));
		msdc_pr_err("MSDC1 IES    [%x] =0x%x\tshould: 0x??????7?\n",
			MSDC1_GPIO_IES_ADDR, MSDC_READ32(MSDC1_GPIO_IES_ADDR));
		msdc_pr_err("MSDC1 SMT    [%x] =0x%x\tshould: 0x??????7?\n",
			MSDC1_GPIO_SMT_ADDR, MSDC_READ32(MSDC1_GPIO_SMT_ADDR));
		msdc_pr_err("MSDC1 TDSEL0 [%x] =0x%x\n",
			MSDC1_GPIO_TDSEL0_ADDR,
			MSDC_READ32(MSDC1_GPIO_TDSEL0_ADDR));
		msdc_pr_err("should 1.8v: sleep: TBD, awake: TBD\n");
		msdc_pr_err("MSDC1 RDSEL0 [%x] =0x%x\n",
			MSDC1_GPIO_RDSEL0_ADDR,
			MSDC_READ32(MSDC1_GPIO_RDSEL0_ADDR));
		msdc_pr_err("1.8V: TBD, 2.9v: TBD\n");
		msdc_pr_err("MSDC1 DRV0   [%x] =0x%x\n",
			MSDC1_GPIO_DRV0_ADDR,
			MSDC_READ32(MSDC1_GPIO_DRV0_ADDR));
		msdc_pr_err("MSDC1 PUPD0  [%x] =0x%x\tshould: 0x??222226\n",
			MSDC1_GPIO_PUPD0_ADDR,
			MSDC_READ32(MSDC1_GPIO_PUPD0_ADDR));

	} else if (id == 3) {
		msdc_pr_err("MSDC3 MODE7  [%x] =0x%x\tshould: 0x11??????\n",
			MSDC3_GPIO_MODE7, MSDC_READ32(MSDC3_GPIO_MODE7));
		msdc_pr_err("MSDC3 MODE8  [%x] =0x%x\tshould: 0x???11111\n",
			MSDC3_GPIO_MODE8, MSDC_READ32(MSDC3_GPIO_MODE8));
		msdc_pr_err("MSDC3 IES    [%x] =0x%x\tshould: bit3\n",
			MSDC3_GPIO_IES_ADDR, MSDC_READ32(MSDC3_GPIO_IES_ADDR));
		msdc_pr_err("MSDC3 SMT    [%x] =0x%x\tshould: bit3\n",
			MSDC3_GPIO_SMT_ADDR, MSDC_READ32(MSDC3_GPIO_SMT_ADDR));
		msdc_pr_err("MSDC3 TDSEL  [%x] =0x%x\tshould: bit12~15\n",
			MSDC3_GPIO_TDSEL_ADDR,
			MSDC_READ32(MSDC3_GPIO_TDSEL_ADDR));
		msdc_pr_err("MSDC3 RDSEL  [%x] =0x%x\tshould: bit6~11\n",
			MSDC3_GPIO_RDSEL_ADDR,
			MSDC_READ32(MSDC3_GPIO_RDSEL_ADDR));
		msdc_pr_err("MSDC3 DRV    [%x] =0x%x\tshould: bit12~15\n",
			MSDC3_GPIO_DRV_ADDR, MSDC_READ32(MSDC3_GPIO_DRV_ADDR));
		msdc_pr_err("MSDC3 PUPD0  [%x] =0x%x\n",
			MSDC3_GPIO_PUPD_ADDR,
			MSDC_READ32(MSDC3_GPIO_PUPD_ADDR));
	}
#endif
}

void msdc_set_pin_mode(struct mmc_host *host)
{
	if (host->id == 0) {
#if (defined(MMC_MSDC_DRV_PRELOADER) && (CFG_BOOT_DEV == BOOTDEV_UFS)) || \
		(defined(MMC_MSDC_DRV_LK) && defined(MTK_UFS_SUPPORT))
		msdc_pr_err("bypass msdc_set_pin_mode for UFS booting\n");
#else
		int pin_state; /* 0: Aux2, other:Aux1 */

		MSDC_GET_FIELD(MSDC_HW_TRAPPING_ADDR, MSDC_HW_TRAPPING_MASK, pin_state);
		if (pin_state == 0x0) {
			MSDC_SET_FIELD(MSDC0_GPIO_MODE16, 0xFFFFFF00, 0x222121);
			MSDC_SET_FIELD(MSDC0_GPIO_MODE17, 0x00FFFFFF, 0x121122);
		} else {
			MSDC_SET_FIELD(MSDC0_GPIO_MODE16, 0xFFFFFF00, 0x111111);
			MSDC_SET_FIELD(MSDC0_GPIO_MODE17, 0x00FFFFFF, 0x111111);
		}
#endif
	} else if (host->id == 1) {
		MSDC_SET_FIELD(MSDC1_GPIO_MODE4, 0xFFF00000, 0x111);
		MSDC_SET_FIELD(MSDC1_GPIO_MODE5, 0x00000FFF, 0x111);
	} else if (host->id == 3) {
		MSDC_SET_FIELD(MSDC3_GPIO_MODE7, 0xFF000000, 0x11);
		MSDC_SET_FIELD(MSDC3_GPIO_MODE8, 0x000FFFFF, 0x11111);
	}
}

void msdc_set_ies_by_id(u32 id, int set_ies)
{
	if (id == 0) {
		MSDC_SET_FIELD(MSDC0_GPIO_IES_ADDR, MSDC0_IES_ALL_MASK,
			(set_ies ? 0x1F : 0));
	} else if (id == 1) {
		MSDC_SET_FIELD(MSDC1_GPIO_IES_ADDR, MSDC1_IES_ALL_MASK,
			(set_ies ? 0x7 : 0));
	} else if (id == 3) {
		MSDC_SET_FIELD(MSDC3_GPIO_IES_ADDR, MSDC3_IES_ALL_MASK,
			(set_ies ? 0x1 : 0));
	}
}

void msdc_set_smt_by_id(u32 id, int set_smt)
{
	if (id == 0) {
		MSDC_SET_FIELD(MSDC0_GPIO_SMT_ADDR, MSDC0_SMT_ALL_MASK,
			(set_smt ? 0x1F : 0));
	} else if (id == 1) {
		MSDC_SET_FIELD(MSDC1_GPIO_SMT_ADDR, MSDC1_SMT_ALL_MASK,
			(set_smt ? 0x7 : 0));
	} else if (id == 3) {
		MSDC_SET_FIELD(MSDC3_GPIO_SMT_ADDR, MSDC3_SMT_ALL_MASK,
			(set_smt ? 0x1 : 0));
	}
}

void msdc_set_tdsel_by_id(u32 id, u32 flag, u32 value)
{
	u32 cust_val;

	if (id == 0) {
		if (flag == MSDC_TDRDSEL_CUST)
			cust_val = value;
		else
			cust_val = 0;
		MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_CMD_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DAT_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_CLK_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_RSTB_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_DSL_MASK,
			cust_val);
	} else if (id == 1) {
		if (flag == MSDC_TDRDSEL_CUST)
			cust_val = value;
		else
			cust_val = 0;
		MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_CMD_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_DAT_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_CLK_MASK,
			cust_val);
	} else if (id == 3) {
		if (flag == MSDC_TDRDSEL_CUST)
			cust_val = value;
		else
			cust_val = 0;
		MSDC_SET_FIELD(MSDC3_GPIO_TDSEL_ADDR, MSDC3_TDSEL_ALL_MASK,
			cust_val);
	}
}

void msdc_set_rdsel_by_id(u32 id, u32 flag, u32 value)
{
	u32 cust_val;

	if (id == 0) {
		if (flag == MSDC_TDRDSEL_CUST)
			cust_val = value;
		else
			cust_val = 0;
		MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_CMD_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_DAT_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_CLK_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_RSTB_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_DSL_MASK,
			cust_val);
	} else if (id == 1) {
		if (flag == MSDC_TDRDSEL_CUST)
			cust_val = value;
		else
			cust_val = 0;
		MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_CMD_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_DAT_MASK,
			cust_val);
		MSDC_SET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_CLK_MASK,
			cust_val);
	} else if (id == 3) {
		if (flag == MSDC_TDRDSEL_CUST)
			cust_val = value;
		else
			cust_val = 0;
		MSDC_SET_FIELD(MSDC3_GPIO_RDSEL_ADDR, MSDC3_RDSEL_ALL_MASK,
			cust_val);
	}
}

void msdc_get_tdsel_by_id(u32 id, u32 *value)
{
	if (id == 0) {
		MSDC_GET_FIELD(MSDC0_GPIO_TDSEL0_ADDR, MSDC0_TDSEL0_CMD_MASK,
			*value);
	} else if (id == 1) {
		MSDC_GET_FIELD(MSDC1_GPIO_TDSEL0_ADDR, MSDC1_TDSEL0_CMD_MASK,
			*value);
	} else if (id == 3) {
		MSDC_GET_FIELD(MSDC3_GPIO_TDSEL_ADDR, MSDC3_TDSEL_ALL_MASK,
			*value);
	}
}

void msdc_get_rdsel_by_id(u32 id, u32 *value)
{
	if (id == 0) {
		MSDC_GET_FIELD(MSDC0_GPIO_RDSEL0_ADDR, MSDC0_RDSEL0_CMD_MASK,
			*value);
	} else if (id == 1) {
		MSDC_GET_FIELD(MSDC1_GPIO_RDSEL0_ADDR, MSDC1_RDSEL0_CMD_MASK,
			*value);
	} else if (id == 3) {
		MSDC_GET_FIELD(MSDC3_GPIO_RDSEL_ADDR, MSDC3_TDSEL_ALL_MASK,
			*value);
	}
}

void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat, int rst, int ds)
{
	if (id == 0) {
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_SR_CMD_MASK,
			(cmd != 0));
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_SR_DSL_MASK,
			(ds != 0));
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_SR_CLK_MASK,
			(clk != 0));
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_SR_DAT_MASK,
			(dat != 0));
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_SR_RSTB_MASK,
			(rst != 0));
	} else if (id == 1) {
		MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_SR_CMD_MASK,
			(cmd != 0));
		MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_SR_CLK_MASK,
			(clk != 0));
		MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_SR_DAT_MASK,
			(dat != 0));
	} else if (id == 3) {
		MSDC_SET_FIELD(MSDC3_GPIO_DRV_ADDR, MSDC3_SR_ALL_MASK,
			(dat != 0));
	}
}

void msdc_set_driving_by_id(u32 id, struct msdc_cust *msdc_cap, bool sd_18)
{
	if (id == 0) {
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DSL_MASK,
			msdc_cap->ds_drv);
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_RSTB_MASK,
			msdc_cap->rst_drv);
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_CMD_MASK,
			msdc_cap->cmd_drv);
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_CLK_MASK,
			msdc_cap->clk_drv);
		MSDC_SET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT_MASK,
			msdc_cap->dat_drv);
	} else if (id == 1) {
		MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_CMD_MASK,
			msdc_cap->cmd_drv);
		MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_CLK_MASK,
			msdc_cap->clk_drv);
		MSDC_SET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_DAT_MASK,
			msdc_cap->dat_drv);
	} else if (id == 3) {
		MSDC_SET_FIELD(MSDC3_GPIO_DRV_ADDR, MSDC3_DRV_ALL_MASK,
			msdc_cap->dat_drv);
	}
}

#if defined(MMC_MSDC_DRV_CTP)
void msdc_get_driving_by_id(u32 id, struct msdc_cust *msdc_cap, bool sd_18)
{
	if (id == 0) {
		MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DSL_MASK,
			msdc_cap->ds_drv);
		MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_RSTB_MASK,
			msdc_cap->rst_drv);
		MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_CMD_MASK,
			msdc_cap->cmd_drv);
		MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_CLK_MASK,
			msdc_cap->clk_drv);
		MSDC_GET_FIELD(MSDC0_GPIO_DRV0_ADDR, MSDC0_DRV0_DAT_MASK,
			msdc_cap->dat_drv);
	} else if (id == 1) {
		MSDC_GET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_CMD_MASK,
			msdc_cap->cmd_drv);
		MSDC_GET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_CLK_MASK,
			msdc_cap->clk_drv);
		MSDC_GET_FIELD(MSDC1_GPIO_DRV0_ADDR, MSDC1_DRV0_DAT_MASK,
			msdc_cap->dat_drv);
	} else if (id == 3) {
		MSDC_GET_FIELD(MSDC3_GPIO_DRV_ADDR, MSDC3_DRV_ALL_MASK,
			msdc_cap->dat_drv);
	}
}
#endif


/* msdc pin config
 * MSDC0
 * PUPD/RO/R1
 * 0/0/0: High-Z
 * 0/0/1: Pull-up with 50Kohm
 * 0/1/0: Pull-up with 10Kohm
 * 0/1/1: Pull-up with 10Kohm//50Kohm
 * 1/0/0: High-Z
 * 1/0/1: Pull-down with 50Kohm
 * 1/1/0: Pull-down with 10Kohm
 * 1/1/1: Pull-down with 10Kohm//50Kohm
 */
void msdc_pin_config_by_id(u32 id, u32 mode)
{
	if (id == 0) {
		/* 1. don't pull CLK high;
		 * 2. Don't toggle RST to prevent from entering boot mode
		 */
		if (mode == MSDC_PIN_PULL_NONE) {
			/* Switch MSDC0_* to no ohm PU */
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD0_MASK, 0x0);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD1_MASK, 0x0);
		} else if (mode == MSDC_PIN_PULL_DOWN) {
			/* Switch MSDC0_* to 50K ohm PD */
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD0_MASK, 0x66666666);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD1_MASK, 0x6666);
		} else if (mode == MSDC_PIN_PULL_UP) {
			/* Switch MSDC0_CLK to 50K ohm PD,
			 * MSDC0_CMD/MSDC0_DAT* to 10K ohm PU,
			 * MSDC0_DSL to 50K ohm PD
			 */
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD0_ADDR, MSDC0_PUPD0_MASK, 0x11111611);
			MSDC_SET_FIELD(MSDC0_GPIO_PUPD1_ADDR, MSDC0_PUPD1_MASK, 0x1161);
		}
	} else if (id == 1) {
		if (mode == MSDC_PIN_PULL_NONE) {
			/* Switch MSDC1_* to no ohm PU */
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD0_MASK, 0x0);
		} else if (mode == MSDC_PIN_PULL_DOWN) {
			/* Switch MSDC1_* to 50K ohm PD */
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD0_MASK, 0x666666);
		} else if (mode == MSDC_PIN_PULL_UP) {
			/* Switch MSDC1_CLK to 50K ohm PD,
			* MSDC1_CMD/MSDC1_DAT* to 50K ohm PU
			*/
			MSDC_SET_FIELD(MSDC1_GPIO_PUPD0_ADDR, MSDC1_PUPD0_MASK, 0x222226);
		}
	} else if (id == 3) {
		if (mode == MSDC_PIN_PULL_NONE) {
			/* Switch MSDC3_* to 0 ohm PU
			 */
			MSDC_SET_FIELD(MSDC3_GPIO_PUPD_ADDR, MSDC3_PUPD_MASK, 0);
		} else if (mode == MSDC_PIN_PULL_DOWN) {
			/* Switch MSDC3_* to 50K ohm PD
			 */
			MSDC_SET_FIELD(MSDC3_GPIO_PUPD_ADDR, MSDC3_PUPD_MASK, 0x6666666);
		} else if (mode == MSDC_PIN_PULL_UP) {
			/* Switch MSDC3_CLK/DSL to 50K ohm PD,
			 * MSDC3_CMD/MSDC3_DAT* to 10K ohm PU
			 */
			MSDC_SET_FIELD(MSDC3_GPIO_PUPD_ADDR, MSDC3_PUPD_MASK, 0x1111616);
		}
	}
}

/**************************************************************/
/* Section 4: MISC                        */
/**************************************************************/
/* make sure the pad is msdc mode */
void msdc_set_tdsel_wrap(struct mmc_host *host)
{
	if (host->cur_pwr == VOL_1800)
		msdc_set_tdsel_by_id(host->id, MSDC_TDRDSEL_1V8, 0);
	else
		msdc_set_tdsel_by_id(host->id, MSDC_TDRDSEL_3V, 0);
}

void msdc_set_rdsel_wrap(struct mmc_host *host)
{
	if (host->cur_pwr == VOL_1800)
		msdc_set_rdsel_by_id(host->id, MSDC_TDRDSEL_1V8, 0);
	else
		msdc_set_rdsel_by_id(host->id, MSDC_TDRDSEL_3V, 0);
}

void msdc_gpio_and_pad_init(struct mmc_host *host)
{
	/* set smt enable */
	msdc_set_smt(host, 1);

	/* set pull enable */
	msdc_pin_config(host, MSDC_PIN_PULL_UP);

	/* set gpio to msdc mode */
	msdc_set_pin_mode(host);

	/* set driving */
	msdc_set_driving(host, &msdc_cap[host->id], (host->cur_pwr == VOL_1800));

	/* set tdsel and rdsel */
	msdc_set_tdsel_wrap(host);
	msdc_set_rdsel_wrap(host);

	msdc_dump_padctl_by_id(host->id);
}

void msdc_gpio_and_pad_init_by_id(int id)
{
	struct mmc_host *host = mmc_get_host(id);
	msdc_set_smt(host, 1);
	msdc_pin_config(host, MSDC_PIN_PULL_UP);
	msdc_set_pin_mode(host);
	msdc_set_driving(host, &msdc_cap[host->id], (host->cur_pwr == VOL_1800));
	msdc_set_tdsel_wrap(host);
	msdc_set_rdsel_wrap(host);
}
#endif
