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

#ifndef __MT_DCM_H__
#define __MT_DCM_H__

extern unsigned long dcm_topckgen_base;
extern unsigned long dcm_mcucfg_base;
extern unsigned long dcm_mcucfg_phys_base;
extern unsigned long dcm_dramc_ch0_top0_base;
extern unsigned long dcm_dramc_ch0_top1_base;
extern unsigned long dcm_dramc_ch0_top3_base;
extern unsigned long dcm_dramc_ch1_top0_base;
extern unsigned long dcm_dramc_ch1_top1_base;
extern unsigned long dcm_dramc_ch1_top3_base;
extern unsigned long dcm_emi_base;
extern unsigned long dcm_infracfg_ao_base;
extern unsigned long dcm_cci_base;
extern unsigned long dcm_cci_phys_base;

#define INFRACFG_AO_BASE	(dcm_infracfg_ao_base)
#define MCUCFG_BASE			(dcm_mcucfg_base)
#define MP0_CPUCFG_BASE		(MCUCFG_BASE + 0x0)
#define MP1_CPUCFG_BASE		(MCUCFG_BASE + 0x200)
#define MCU_MISCCFG_BASE	(MCUCFG_BASE + 0x400)

#define TOPCKGEN_BASE		(dcm_topckgen_base)
#define DRAMC_CH0_TOP0_BASE	(dcm_dramc_ch0_top0_base)
#define DRAMC_CH0_TOP1_BASE	(dcm_dramc_ch0_top1_base)
#define CHN0_EMI_BASE		(dcm_dramc_ch0_top3_base)
#define DRAMC_CH1_TOP0_BASE	(dcm_dramc_ch1_top0_base)
#define DRAMC_CH1_TOP1_BASE	(dcm_dramc_ch1_top1_base)
#define CHN1_EMI_BASE		(dcm_dramc_ch1_top3_base)
#define EMI_BASE			(dcm_emi_base)
#define CCI_BASE			(dcm_cci_base)
#define CCI_PHYS_BASE		(dcm_cci_phys_base)

void mt_dcm_disable(void);
void mt_dcm_restore(void);

int sync_dcm_set_cci_freq(unsigned int cci);
int sync_dcm_set_mp0_freq(unsigned int mp0);
int sync_dcm_set_mp1_freq(unsigned int mp1);

#define DCM_OFF (0)
#define DCM_ON (1)

/* #define ENABLE_DCM_IN_LK */

typedef enum {
	DCM_NOT_INIT = 0,
	DCM_INIT_SUCCESS = 1,
	DCM_INIT_FAIL = 2,
} ENUM_INIT_STATE_DCM;

typedef enum {
	ARMCORE_DCM_OFF = DCM_OFF,
	ARMCORE_DCM_MODE1 = DCM_ON,
	ARMCORE_DCM_MODE2 = DCM_ON+1,
} ENUM_ARMCORE_DCM;

typedef enum {
	INFRA_DCM_OFF = DCM_OFF,
	INFRA_DCM_ON = DCM_ON,
} ENUM_INFRA_DCM;

typedef enum {
	PERI_DCM_OFF = DCM_OFF,
	PERI_DCM_ON = DCM_ON,
} ENUM_PERI_DCM;

typedef enum {
	MCUSYS_DCM_OFF = DCM_OFF,
	MCUSYS_DCM_ON = DCM_ON,
} ENUM_MCUSYS_DCM;

typedef enum {
	DRAMC_AO_DCM_OFF = DCM_OFF,
	DRAMC_AO_DCM_ON = DCM_ON,
} ENUM_DRAMC_AO_DCM;

typedef enum {
	DDRPHY_DCM_OFF = DCM_OFF,
	DDRPHY_DCM_ON = DCM_ON,
} ENUM_DDRPHY_DCM;

typedef enum {
	EMI_DCM_OFF = DCM_OFF,
	EMI_DCM_ON = DCM_ON,
} ENUM_EMI_DCM;

typedef enum {
	STALL_DCM_OFF = DCM_OFF,
	STALL_DCM_ON = DCM_ON,
} ENUM_STALL_DCM;

typedef enum {
	GIC_SYNC_DCM_OFF = DCM_OFF,
	GIC_SYNC_DCM_ON = DCM_ON,
} ENUM_GIC_SYNC_DCM;

typedef enum {
	LAST_CORE_DCM_OFF = DCM_OFF,
	LAST_CORE_DCM_ON = DCM_ON,
} ENUM_LAST_CORE_DCM;

typedef enum {
	RGU_DCM_OFF = DCM_OFF,
	RGU_DCM_ON = DCM_ON,
} ENUM_RGU_DCM;

/*****************************************************/
int dcm_armcore(ENUM_ARMCORE_DCM mode);
int dcm_infra(ENUM_INFRA_DCM on);
int dcm_peri(ENUM_PERI_DCM on);
int dcm_mcusys(ENUM_MCUSYS_DCM on);
int dcm_dramc_ao(ENUM_DRAMC_AO_DCM on);
int dcm_emi(ENUM_EMI_DCM on);
int dcm_ddrphy(ENUM_DDRPHY_DCM on);
int dcm_stall_preset(void);
int dcm_stall(ENUM_STALL_DCM on);
int dcm_gic_sync(ENUM_GIC_SYNC_DCM on);
int dcm_last_core(ENUM_LAST_CORE_DCM on);
int dcm_rgu(ENUM_RGU_DCM on);

/*****************************************************/
enum {
	ARMCORE_DCM = 0,
	MCUSYS_DCM,
	INFRA_DCM,
	PERI_DCM,
	EMI_DCM,
	DRAMC_DCM,
	DDRPHY_DCM,
	STALL_DCM,
	GIC_SYNC_DCM,
	LAST_CORE_DCM,
	RGU_DCM,
	NR_DCM = 11,
};

enum {
	ARMCORE_DCM_TYPE	= (1U << 0),
	MCUSYS_DCM_TYPE		= (1U << 1),
	INFRA_DCM_TYPE		= (1U << 2),
	PERI_DCM_TYPE		= (1U << 3),
	EMI_DCM_TYPE		= (1U << 4),
	DRAMC_DCM_TYPE		= (1U << 5),
	DDRPHY_DCM_TYPE		= (1U << 6),
	STALL_DCM_TYPE		= (1U << 7),
	GIC_SYNC_DCM_TYPE	= (1U << 8),
	LAST_CORE_DCM_TYPE	= (1U << 9),
	RGU_DCM_TYPE		= (1U << 10),
	NR_DCM_TYPE = 11,
};

/* #define ALL_DCM_TYPE  (ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE | INFRA_DCM_TYPE | \ */
/*		       PERI_DCM_TYPE | EMI_DCM_TYPE | DRAMC_DCM_TYPE | DDRPHY_DCM_TYPE | \ */
/*		       STALL_DCM_TYPE | GIC_SYNC_DCM_TYPE | LAST_CORE_DCM_TYPE | RGU_DCM_TYPE) */

/* #define INIT_DCM_TYPE  (ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE | INFRA_DCM_TYPE | \ */
/*		       PERI_DCM_TYPE | STALL_DCM_TYPE) */

#ifdef ENABLE_DCM_IN_LK
#define INIT_DCM_TYPE_BY_K	0
#endif


#endif /* #ifndef __MT_DCM_H__ */

