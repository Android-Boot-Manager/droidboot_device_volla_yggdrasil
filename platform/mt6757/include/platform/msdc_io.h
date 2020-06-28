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
*/

#ifndef _MSDC_IO_H_
#define _MSDC_IO_H_

#include "msdc.h"
#include "mmc_types.h"

#if defined(MMC_MSDC_DRV_PRELOADER)
#include "platform.h"
#include "pll.h"
#include "upmu_hw.h"
#endif

#if defined(MMC_MSDC_DRV_LK)
#include "mt_reg_base.h"
#include "upmu_hw.h"
#endif

#if defined(MMC_MSDC_DRV_CTP)
#include "reg_base.H"
#include "intrCtrl.h"
#include "pmic.h"
#endif

/*******************************************************************************
 *FPGA power gpio definition
 ******************************************************************************/

#define PWR_GPIO        (0x10001E84) //Select Power On/Off
#define PWR_GPIO_EO     (0x10001E88) //Select Output/Input Direction

#define PWR_MASK_CARD       (0x1 << 8)   //Vcc
#define PWR_MASK_VOL_18     (0x1 << 9)   //CLK/CMD/DAT for eMMC/SD +  Vccq for eMMC
#define PWR_MASK_VOL_33     (0x1 << 10)  //CLK/CMD/DAT for eMMC/SD +  Vccq for eMMC
#define PWR_MASK_L4     (0x1 << 11)
#define PWR_MSDC        (PWR_MASK_CARD | PWR_MASK_VOL_18 | PWR_MASK_VOL_33 | PWR_MASK_L4)
/*******************************************************************************
 *PINMUX and GPIO definition
 ******************************************************************************/
#define MSDC_PIN_PULL_NONE      (0)
#define MSDC_PIN_PULL_DOWN      (1)
#define MSDC_PIN_PULL_UP        (2)
#define MSDC_PIN_KEEP           (3)

/* add pull down/up mode define */
#define MSDC_GPIO_PULL_UP       (0)
#define MSDC_GPIO_PULL_DOWN     (1)

/*--------------------------------------------------------------------------*/
/* MSDC0~3 GPIO and IO Pad Configuration Base                               */
/*--------------------------------------------------------------------------*/
#define GPIO_base               0x10005000
#ifndef GPIO_BASE
#define GPIO_BASE               GPIO_base
#endif
#define MSDC_GPIO_BASE      GPIO_BASE
#define IO_CFG_5_BASE           (0x10002a00)    /* IO Config 5 base */
#define IO_CFG_0_BASE           (0x10002000)    /* IO Config 0 base */
#define MSDC0_IO_PAD_BASE       (IO_CFG_5_BASE)
#define MSDC1_IO_PAD_BASE       (IO_CFG_0_BASE)

/*--------------------------------------------------------------------------*/
/* MSDC GPIO Related Register                                               */
/*--------------------------------------------------------------------------*/
#define MSDC0_GPIO_MODE23       (MSDC_GPIO_BASE   +  0x460)
#define MSDC0_GPIO_MODE24       (MSDC_GPIO_BASE   +  0x470)
#define MSDC0_GPIO_SMT_ADDR	    (MSDC0_IO_PAD_BASE + 0x10)
#define MSDC0_GPIO_IES_ADDR     (MSDC0_IO_PAD_BASE + 0x0)
#define MSDC0_GPIO_TDSEL_ADDR   (MSDC0_IO_PAD_BASE + 0x20)
#define MSDC0_GPIO_RDSEL_ADDR   (MSDC0_IO_PAD_BASE + 0x40)
#define MSDC0_GPIO_DRV_ADDR	    (MSDC0_IO_PAD_BASE + 0xa0)
#define MSDC0_GPIO_PUPD0_ADDR   (MSDC0_IO_PAD_BASE + 0xc0)
#define MSDC0_GPIO_PUPD1_ADDR   (MSDC0_IO_PAD_BASE + 0xd0)

#define MSDC1_GPIO_MODE4        (MSDC_GPIO_BASE   +  0x330)
#define MSDC1_GPIO_MODE5        (MSDC_GPIO_BASE   +  0x340)
#define MSDC1_GPIO_IES_ADDR     (MSDC1_IO_PAD_BASE + 0x0)
#define MSDC1_GPIO_SMT_ADDR	    (MSDC1_IO_PAD_BASE + 0x10)
#define MSDC1_GPIO_TDSEL_ADDR   (MSDC1_IO_PAD_BASE + 0x30)
#define MSDC1_GPIO_RDSEL0_ADDR  (MSDC1_IO_PAD_BASE + 0x40)
#define MSDC1_GPIO_RDSEL1_ADDR  (MSDC1_IO_PAD_BASE + 0x50)
/* SR is in MSDC1_GPIO_DRV1_ADDR */
#define MSDC1_GPIO_DRV1_ADDR    (MSDC1_IO_PAD_BASE + 0xb0)
#define MSDC1_GPIO_PUPD_ADDR    (MSDC1_IO_PAD_BASE + 0xc0)

/*--------------------------------------------------------------------------*/
/* MSDC GPIO Related Register Mask                                               */
/*--------------------------------------------------------------------------*/
/* MSDC0_GPIO_MODE23, 001b is msdc mode*/
#define MSDC0_MODE_DAT6_MASK            (0x7 << 28)
#define MSDC0_MODE_DAT5_MASK            (0x7 << 24)
#define MSDC0_MODE_DAT1_MASK            (0x7 << 20)
#define MSDC0_MODE_CMD_MASK             (0x7 << 16)
#define MSDC0_MODE_CLK_MASK             (0x7 << 12)
#define MSDC0_MODE_DAT0_MASK            (0x7 << 8)

/* MSDC0_GPIO_MODE24, 001b is msdc mode */
#define MSDC0_MODE_DSL_MASK             (0x7 << 20)
#define MSDC0_MODE_DAT7_MASK            (0x7 << 16)
#define MSDC0_MODE_DAT3_MASK            (0x7 << 12)
#define MSDC0_MODE_DAT2_MASK            (0x7 << 8)
#define MSDC0_MODE_RSTB_MASK            (0x7 << 4)
#define MSDC0_MODE_DAT4_MASK            (0x7 << 0)

/* MSDC1_GPIO_MODE4, 0001b is msdc mode */
#define MSDC1_MODE_DAT3_MASK            (0x7 << 28)
#define MSDC1_MODE_CLK_MASK             (0x7 << 24)

/* MSDC1_GPIO_MODE5, 0001b is msdc mode */
#define MSDC1_MODE_DAT1_MASK            (0x7 << 12)
#define MSDC1_MODE_DAT2_MASK            (0x7 << 8)
#define MSDC1_MODE_DAT0_MASK            (0x7 << 4)
#define MSDC1_MODE_CMD_MASK             (0x7 << 0)

/* MSDC0 SMT mask*/
#define MSDC0_SMT_DSL_MASK              (0x1  <<  4)
#define MSDC0_SMT_RSTB_MASK             (0x1  <<  3)
#define MSDC0_SMT_CMD_MASK              (0x1  <<  2)
#define MSDC0_SMT_CLK_MASK              (0x1  <<  1)
#define MSDC0_SMT_DAT_MASK              (0x1  <<  0)
#define MSDC0_SMT_ALL_MASK              (0x1f <<  0)
/* MSDC1 SMT mask*/
#define MSDC1_SMT_DAT_MASK              (0x1 << 10)
#define MSDC1_SMT_CMD_MASK              (0x1 << 9)
#define MSDC1_SMT_CLK_MASK              (0x1 << 8)
#define MSDC1_SMT_ALL_MASK              (0x7 << 8)

/* MSDC0 PUPD0 mask */
/* MSDC0 PUPD1 mask */
/* MSDC1 PUPD mask*/
/* No need add pin define, because it change everytime */
/* MSDC1 BIAS Tune mask */
#define MSDC1_BIAS_MASK                 (0xf << 24)

/* MSDC0 IES mask*/
#define MSDC0_IES_DSL_MASK              (0x1  <<  4)
#define MSDC0_IES_RSTB_MASK             (0x1  <<  3)
#define MSDC0_IES_CMD_MASK              (0x1  <<  2)
#define MSDC0_IES_CLK_MASK              (0x1  <<  1)
#define MSDC0_IES_DAT_MASK              (0x1  <<  0)
#define MSDC0_IES_ALL_MASK              (0x1f <<  0)
/* MSDC1 IES mask*/
#define MSDC1_IES_DAT_MASK              (0x1 << 10)
#define MSDC1_IES_CMD_MASK              (0x1 <<  9)
#define MSDC1_IES_CLK_MASK              (0x1 <<  8)
#define MSDC1_IES_ALL_MASK              (0x7 <<  8)

/* MSDC0 TDSEL mask*/
#define MSDC0_TDSEL_DSL_MASK            (0xf  << 16)
#define MSDC0_TDSEL_RSTB_MASK           (0xf  << 12)
#define MSDC0_TDSEL_CMD_MASK            (0xf  <<  8)
#define MSDC0_TDSEL_CLK_MASK            (0xf  <<  4)
#define MSDC0_TDSEL_DAT_MASK            (0xf  <<  0)
#define MSDC0_TDSEL_ALL_MASK            (0xfffff << 0)
/* MSDC1 TDSEL mask*/
#define MSDC1_TDSEL_DAT_MASK            (0xf <<  8)
#define MSDC1_TDSEL_CMD_MASK            (0xf <<  4)
#define MSDC1_TDSEL_CLK_MASK            (0xf <<  0)
#define MSDC1_TDSEL_ALL_MASK            (0xfff << 0)

/* MSDC0 RDSEL mask*/
#define MSDC0_RDSEL_DSL_MASK            (0x3f << 24)
#define MSDC0_RDSEL_RSTB_MASK           (0x3f << 18)
#define MSDC0_RDSEL_CMD_MASK            (0x3f << 12)
#define MSDC0_RDSEL_CLK_MASK            (0x3f <<  6)
#define MSDC0_RDSEL_DAT_MASK            (0x3F <<  0)
#define MSDC0_RDSEL_ALL_MASK            (0x3fffffff << 0)
/* MSDC1 RDSEL mask*/
#define MSDC1_RDSEL_CMD_MASK            (0x3f << 22)
#define MSDC1_RDSEL_CLK_MASK            (0x3f << 16)
#define MSDC1_RDSEL0_ALL_MASK           (0xfff << 16)
#define MSDC1_RDSEL_DAT_MASK            (0x3f << 0)
#define MSDC1_RDSEL1_ALL_MASK           (0x3f << 0)

/* MSDC0 DRV mask*/
#define MSDC0_DRV_DSL_MASK              (0xf  <<  16)
#define MSDC0_DRV_RSTB_MASK             (0xf  <<  12)
#define MSDC0_DRV_CMD_MASK              (0xf  <<  8)
#define MSDC0_DRV_CLK_MASK              (0xf  <<  4)
#define MSDC0_DRV_DAT_MASK              (0xf  <<  0)
#define MSDC0_DRV_ALL_MASK              (0xfffff << 0)
/* MSDC1 SR mask*//* SR is in MSDC1_GPIO_DRV1_ADDR */
#define MSDC1_SR_CMD_MASK               (0x1 << 11)
#define MSDC1_SR_DAT_MASK               (0x1 << 7)
#define MSDC1_SR_CLK_MASK               (0x1 << 3)
/* MSDC1 DRV mask*/
#define MSDC1_DRV_CMD_MASK              (0x7 << 8)
#define MSDC1_DRV_DAT_MASK              (0x7 << 4)
#define MSDC1_DRV_CLK_MASK              (0x7 << 0)
#define MSDC1_DRV_ALL_MASK              (0xfff << 0)

/*
   MSDC Driving Strength Definition: specify as gear instead of driving
*/
#define MSDC_DRVN_GEAR0                 0x0
#define MSDC_DRVN_GEAR1                 0x1
#define MSDC_DRVN_GEAR2                 0x2
#define MSDC_DRVN_GEAR3                 0x3
#define MSDC_DRVN_GEAR4                 0x4
#define MSDC_DRVN_GEAR5                 0x5
#define MSDC_DRVN_GEAR6                 0x6
#define MSDC_DRVN_GEAR7                 0x7
#define MSDC_DRVN_DONT_CARE             0x0

#ifndef FPGA_PLATFORM
void msdc_set_driving_by_id(u32 id, struct msdc_cust *hw, bool sd_18);
void msdc_get_driving_by_id(u32 id, struct msdc_cust *hw);
void msdc_set_ies_by_id(u32 id, int set_ies);
void msdc_set_sr_by_id(u32 id, int clk, int cmd, int dat);
void msdc_set_smt_by_id(u32 id, int set_smt);
void msdc_set_tdsel_by_id(u32 id, bool sleep, bool sd_18);
void msdc_set_rdsel_by_id(u32 id, bool sleep, bool sd_18);
void msdc_dump_padctl_by_id(u32 id);
void msdc_pin_config_by_id(u32 id, u32 mode);
void msdc_set_pin_mode(struct mmc_host *host);
#endif

#ifdef FPGA_PLATFORM
#define msdc_set_driving_by_id(id, hw, sd_18)
#define msdc_get_driving_by_id(id, hw)
#define msdc_set_ies_by_id(id, set_ies)
#define msdc_set_sr_by_id(id, clk, cmd, dat)
#define msdc_set_smt_by_id(id, set_smt)
#define msdc_set_tdsel_by_id(id, sleep, sd_18)
#define msdc_set_rdsel_by_id(id, sleep, sd_18)
#define msdc_dump_padctl_by_id(id)
#define msdc_pin_config_by_id(id, mode)
#define msdc_set_pin_mode(host)
#endif

void msdc_gpio_and_pad_init(struct mmc_host *host);

#define msdc_get_driving(host, msdc_cap, sd_18) \
    msdc_get_driving_by_id(host->id, msdc_cap, sd_18)

#define msdc_set_driving(host, msdc_cap, sd_18) \
    msdc_set_driving_by_id(host->id, msdc_cap, sd_18)

#define msdc_set_ies(host, set_ies) \
    msdc_set_ies_by_id(host->id, set_ies)

#define msdc_set_sr(host, clk, cmd, dat, rst, ds) \
    msdc_set_sr_by_id(host->id, clk, cmd, dat, rst, ds)

#define msdc_set_smt(host, set_smt) \
    msdc_set_smt_by_id(host->id, set_smt)

#define msdc_set_tdsel(host, sleep, sd_18) \
    msdc_set_tdsel_by_id(host->id, sleep, sd_18)

#define msdc_set_rdsel(host, sleep, sd_18) \
    msdc_set_rdsel_by_id(host->id, sleep, sd_18)

#define msdc_dump_padctl(host) \
    msdc_dump_padctl_by_id(host->id)

#define msdc_pin_config(host, mode) \
    msdc_pin_config_by_id(host->id, mode)


/*******************************************************************************
 * CLOCK definition
 ******************************************************************************/
#if defined(FPGA_PLATFORM)
#define MSDC_OP_SCLK            (12000000)
#define MSDC_SRC_CLK            (12000000)
#define MSDC_MAX_SCLK           (MSDC_SRC_CLK>>1)
#else
#define MSDC_OP_SCLK            (200000000)
#define MSDC_MAX_SCLK           (800000000)
#endif

#define MSDC_MIN_SCLK           (260000)

#define MSDC_350K_SCLK          (350000)
#define MSDC_400K_SCLK          (400000)
#define MSDC_25M_SCLK           (25000000)
#define MSDC_26M_SCLK           (26000000)
#define MSDC_50M_SCLK           (50000000)
#define MSDC_52M_SCLK           (52000000)
#define MSDC_100M_SCLK          (100000000)
#define MSDC_179M_SCLK          (179000000)
#define MSDC_200M_SCLK          (200000000)
#define MSDC_208M_SCLK          (208000000)
#define MSDC_400M_SCLK          (400000000)
#define MSDC_800M_SCLK          (800000000)

/* each PLL have different gears for select
 * software can used mux interface from clock management module to select */
enum {
	MSDC50_CLKSRC_26MHZ  = 0,
	MSDC50_CLKSRC_400MHZ,
	MSDC50_CLKSRC_200MHZ,
	MSDC50_CLKSRC_156MHZ,
	MSDC50_CLKSRC_182MHZ,
	MSDC50_CLKSRC_NONE,
	MSDC50_CLKSRC_100MHZ,
	MSDC50_CLKSRC_624MHZ,
	MSDC50_CLKSRC_312MHZ,
	MSDC50_CLKSRC_MAX
};

enum {
	MSDC30_CLKSRC_26MHZ   = 0,
	MSDC30_CLKSRC_208MHZ,
	MSDC30_CLKSRC_100MHZ,
	MSDC30_CLKSRC_156MHZ,
	MSDC30_CLKSRC_182MHZ,
	MSDC30_CLKSRC_NONE,
	MSDC30_CLKSRC_178MHZ,
	MSDC30_CLKSRC_200MHZ,
	MSDC30_CLKSRC_MAX
};

#define MSDC50_CLKSRC_DEFAULT   MSDC50_CLKSRC_400MHZ
#define MSDC30_CLKSRC_DEFAULT   MSDC30_CLKSRC_200MHZ
#define MSDC_CLKSRC_DEFAULT(ID) (((ID) == 0) ? MSDC50_CLKSRC_DEFAULT : MSDC30_CLKSRC_200MHZ)

void msdc_clock(struct mmc_host *host, int on);
void msdc_config_clksrc(struct mmc_host *host, u8 clksrc);

extern u32 hclks_msdc50_0[];
extern u32 hclks_msdc30_1[];
#if !defined(FPGA_PLATFORM)
extern u32 *msdc_src_clks;
#else
extern u32 msdc_src_clks[];
#endif

/* PLL register */
/* MSDCPLL register offset */
#define MSDCPLL_CON0_OFFSET             (0x250)
#define MSDCPLL_CON1_OFFSET             (0x254)
#define MSDCPLL_CON2_OFFSET             (0x258)
#define MSDCPLL_PWR_CON0_OFFSET         (0x25c)
/* Clock config register offset */
#define MSDC_CLK_CFG_3_OFFSET           (0x070)

#define MSDC_PERI_PDN_SET0_OFFSET       (0x0008)
#define MSDC_PERI_PDN_CLR0_OFFSET       (0x0010)
#define MSDC_PERI_PDN_STA0_OFFSET       (0x0018)


/*******************************************************************************
 * Power Definition
 ******************************************************************************/
typedef enum MSDC_POWER_VOL_TAG {
	VOL_DEFAULT,
	VOL_0900 = 900,
	VOL_1000 = 1000,
	VOL_1100 = 1100,
	VOL_1200 = 1200,
	VOL_1300 = 1300,
	VOL_1350 = 1350,
	VOL_1500 = 1500,
	VOL_1800 = 1800,
	VOL_2000 = 2000,
	VOL_2100 = 2100,
	VOL_2500 = 2500,
	VOL_2800 = 2800,
	VOL_2900 = 2900,
	VOL_3000 = 3000,
	VOL_3300 = 3300,
	VOL_3400 = 3400,
	VOL_3500 = 3500,
	VOL_3600 = 3600
} MSDC_POWER_VOLTAGE;

#if defined(MMC_MSDC_DRV_CTP) && !defined(FPGA_PLATFORM)
typedef enum MSDC_POWER_TAG {
	MSDC_VMC,
	MSDC_VMCH,
	MSDC_VEMC,
} MSDC_POWER;
#endif

#define REG_VEMC_VOSEL_CAL              MT6351_PMIC_RG_VEMC_CAL_ADDR
#define REG_VEMC_VOSEL                  MT6351_PMIC_RG_VEMC_VOSEL_ADDR
#define REG_VEMC_EN                     MT6351_PMIC_RG_VEMC_EN_ADDR
#define REG_VMC_VOSEL                   MT6351_PMIC_RG_VMC_VOSEL_ADDR
#define REG_VMC_EN                      MT6351_PMIC_RG_VMC_EN_ADDR
#define REG_VMCH_VOSEL_CAL              MT6351_PMIC_RG_VMCH_CAL_ADDR
#define REG_VMCH_VOSEL                  MT6351_PMIC_RG_VMCH_VOSEL_ADDR
#define REG_VMCH_EN                     MT6351_PMIC_RG_VMCH_EN_ADDR

#define MASK_VEMC_VOSEL_CAL             MT6351_PMIC_RG_VEMC_CAL_MASK
#define SHIFT_VEMC_VOSEL_CAL            MT6351_PMIC_RG_VEMC_CAL_SHIFT
#define FIELD_VEMC_VOSEL_CAL            (MASK_VEMC_VOSEL_CAL << \
                        SHIFT_VEMC_VOSEL_CAL)
#define MASK_VEMC_VOSEL                 MT6351_PMIC_RG_VEMC_VOSEL_MASK
#define SHIFT_VEMC_VOSEL                MT6351_PMIC_RG_VEMC_VOSEL_SHIFT
#define FIELD_VEMC_VOSEL                (MASK_VEMC_VOSEL << SHIFT_VEMC_VOSEL)
#define MASK_VEMC_EN                    MT6351_PMIC_RG_VEMC_EN_MASK
#define SHIFT_VEMC_EN                   MT6351_PMIC_RG_VEMC_EN_SHIFT
#define FIELD_VEMC_EN                   (MASK_VEMC_EN << SHIFT_VEMC_EN)
#define MASK_VMC_VOSEL                  MT6351_PMIC_RG_VMC_VOSEL_MASK
#define SHIFT_VMC_VOSEL                 MT6351_PMIC_RG_VMC_VOSEL_SHIFT
#define FIELD_VMC_VOSEL                 (MASK_VMC_VOSEL << SHIFT_VMC_VOSEL)
#define MASK_VMC_EN                     MT6351_PMIC_RG_VMC_EN_MASK
#define SHIFT_VMC_EN                    MT6351_PMIC_RG_VMC_EN_SHIFT
#define FIELD_VMC_EN                    (MASK_VMC_EN << SHIFT_VMC_EN)
#define MASK_VMCH_VOSEL_CAL             MT6351_PMIC_RG_VMCH_CAL_MASK
#define SHIFT_VMCH_VOSEL_CAL            MT6351_PMIC_RG_VMCH_CAL_SHIFT
#define FIELD_VMCH_VOSEL_CAL            (MASK_VMCH_VOSEL_CAL << \
                        SHIFT_VMCH_VOSEL_CAL)
#define MASK_VMCH_VOSEL                 MT6351_PMIC_RG_VMCH_VOSEL_MASK
#define SHIFT_VMCH_VOSEL                MT6351_PMIC_RG_VMCH_VOSEL_SHIFT
#define FIELD_VMCH_VOSEL                (MASK_VMCH_VOSEL << SHIFT_VMCH_VOSEL)
#define MASK_VMCH_EN                    MT6351_PMIC_RG_VMCH_EN_MASK
#define SHIFT_VMCH_EN                   MT6351_PMIC_RG_VMCH_EN_SHIFT
#define FIELD_VMCH_EN                   (MASK_VMCH_EN << SHIFT_VMCH_EN)

#define VEMC_VOSEL_CAL_mV(cal)          ((cal <= 0) ? ((0-cal)/20): (32-cal/20))
#define VEMC_VOSEL_3V                   (0)
#define VEMC_VOSEL_3V3                  (1)
#define VMC_VOSEL_1V8                   (3)
#define VMC_VOSEL_2V8                   (5)
#define VMC_VOSEL_3V                    (6)
#define VMCH_VOSEL_CAL_mV(cal)          ((cal <= 0) ? ((0-cal)/20): (32-cal/20))
#define VMCH_VOSEL_3V                   (0)
#define VMCH_VOSEL_3V3                  (1)

#define REG_VCORE_VOSEL_SW      MT6351_PMIC_BUCK_VCORE_VOSEL_ADDR
#define VCORE_VOSEL_SW_MASK     MT6351_PMIC_BUCK_VCORE_VOSEL_MASK
#define VCORE_VOSEL_SW_SHIFT        MT6351_PMIC_BUCK_VCORE_VOSEL_SHIFT
#define REG_VCORE_VOSEL_HW      MT6351_PMIC_BUCK_VCORE_VOSEL_ON_ADDR
#define VCORE_VOSEL_HW_MASK     MT6351_PMIC_BUCK_VCORE_VOSEL_ON_MASK
#define VCORE_VOSEL_HW_SHIFT        MT6351_PMIC_BUCK_VCORE_VOSEL_ON_SHIFT
#define REG_VIO18_CAL           MT6351_PMIC_RG_VIO18_CAL_ADDR
#define VIO18_CAL_MASK          MT6351_PMIC_RG_VIO18_CAL_MASK
#define VIO18_CAL_SHIFT         MT6351_PMIC_RG_VIO18_CAL_SHIFT

#define CARD_VOL_ACTUAL         VOL_2900

#endif /* end of _MSDC_IO_H_ */

