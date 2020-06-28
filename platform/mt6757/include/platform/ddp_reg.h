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

#ifndef _DDP_REG_H_
#define _DDP_REG_H_

#include <platform/mt_typedefs.h>
#include <platform/sync_write.h>

#include <platform/mt_reg_base.h>

typedef void * cmdqRecHandle;

// MIPITX and DSI

typedef struct {
	unsigned RG_DSI_LDOCORE_EN:1;
	unsigned RG_DSI_CKG_LDOOUT_EN:1;
	unsigned RG_DSI_BCLK_SEL:2;
	unsigned RG_DSI_LD_IDX_SEL:3;
	unsigned rsv_7:1;
	unsigned RG_DSI_PHYCLK_SEL:2;
	unsigned RG_DSI_DSICLK_FREQ_SEL:1;
	unsigned RG_DSI_LPTX_CLMP_EN:1;
	unsigned rsv_12:20;
} MIPITX_DSI_CON_REG, *PMIPITX_DSI_CON_REG;

typedef struct {
	unsigned RG_DSI_LNTC_LDOOUT_EN:1;
	unsigned RG_DSI_LNTC_CKLANE_EN:1;
	unsigned RG_DSI_LNTC_LPTX_IPLUS1:1;
	unsigned RG_DSI_LNTC_LPTX_IPLUS2:1;
	unsigned RG_DSI_LNTC_LPTX_IMINUS:1;
	unsigned RG_DSI_LNTC_LPCD_IPLUS:1;
	unsigned RG_DSI_LNTC_LPCD_IMINUS:1;
	unsigned rsv_7:1;
	unsigned RG_DSI_LNTC_RT_CODE:4;
	unsigned rsv_12:20;
} MIPITX_DSI_CLOCK_LANE_REG, *PMIPITX_DSI_CLOCK_LANE_REG;

typedef struct {
	unsigned RG_DSI_LNT0_LDOOUT_EN:1;
	unsigned RG_DSI_LNT0_CKLANE_EN:1;
	unsigned RG_DSI_LNT0_LPTX_IPLUS1:1;
	unsigned RG_DSI_LNT0_LPTX_IPLUS2:1;
	unsigned RG_DSI_LNT0_LPTX_IMINUS:1;
	unsigned RG_DSI_LNT0_LPCD_IPLUS:1;
	unsigned RG_DSI_LNT0_LPCD_IMINUS:1;
	unsigned rsv_7:1;
	unsigned RG_DSI_LNT0_RT_CODE:4;
	unsigned rsv_11:20;
} MIPITX_DSI_DATA_LANE0_REG, *PMIPITX_DSI_DATA_LANE0_REG;


typedef struct {
	unsigned RG_DSI_LNT1_LDOOUT_EN:1;
	unsigned RG_DSI_LNT1_CKLANE_EN:1;
	unsigned RG_DSI_LNT1_LPTX_IPLUS1:1;
	unsigned RG_DSI_LNT1_LPTX_IPLUS2:1;
	unsigned RG_DSI_LNT1_LPTX_IMINUS:1;
	unsigned RG_DSI_LNT1_LPCD_IPLUS:1;
	unsigned RG_DSI_LNT1_LPCD_IMINUS:1;
	unsigned rsv_7:1;
	unsigned RG_DSI_LNT1_RT_CODE:4;
	unsigned rsv_11:20;
} MIPITX_DSI_DATA_LANE1_REG, *PMIPITX_DSI_DATA_LANE1_REG;


typedef struct {
	unsigned RG_DSI_LNT2_LDOOUT_EN:1;
	unsigned RG_DSI_LNT2_CKLANE_EN:1;
	unsigned RG_DSI_LNT2_LPTX_IPLUS1:1;
	unsigned RG_DSI_LNT2_LPTX_IPLUS2:1;
	unsigned RG_DSI_LNT2_LPTX_IMINUS:1;
	unsigned RG_DSI_LNT2_LPCD_IPLUS:1;
	unsigned RG_DSI_LNT2_LPCD_IMINUS:1;
	unsigned rsv_7:1;
	unsigned RG_DSI_LNT2_RT_CODE:4;
	unsigned rsv_11:20;
} MIPITX_DSI_DATA_LANE2_REG, *PMIPITX_DSI_DATA_LANE2_REG;

typedef struct {
	unsigned RG_DSI_LNT3_LDOOUT_EN:1;
	unsigned RG_DSI_LNT3_CKLANE_EN:1;
	unsigned RG_DSI_LNT3_LPTX_IPLUS1:1;
	unsigned RG_DSI_LNT3_LPTX_IPLUS2:1;
	unsigned RG_DSI_LNT3_LPTX_IMINUS:1;
	unsigned RG_DSI_LNT3_LPCD_IPLUS:1;
	unsigned RG_DSI_LNT3_LPCD_IMINUS:1;
	unsigned rsv_7:1;
	unsigned RG_DSI_LNT3_RT_CODE:4;
	unsigned rsv_11:20;
} MIPITX_DSI_DATA_LANE3_REG, *PMIPITX_DSI_DATA_LANE3_REG;

typedef struct {
	unsigned RG_DSI_LNT_INTR_EN:1;
	unsigned RG_DSI_LNT_HS_BIAS_EN:1;
	unsigned RG_DSI_LNT_IMP_CAL_EN:1;
	unsigned RG_DSI_LNT_TESTMODE_EN:1;
	unsigned RG_DSI_LNT_IMP_CAL_CODE:4;
	unsigned RG_DSI_LNT_AIO_SEL:3;
	unsigned RG_DSI_PAD_TIE_LOW_EN:1;
	unsigned rsv_12:1;
	unsigned RG_DSI_PRESERVE:3;
	unsigned rsv_16:16;
} MIPITX_DSI_TOP_CON_REG, *PMIPITX_DSI_TOP_CON_REG;


typedef struct {
	unsigned RG_DSI_BG_CORE_EN:1;
	unsigned RG_DSI_BG_CKEN:1;
	unsigned RG_DSI_BG_DIV:2;
	unsigned RG_DSI_BG_FAST_CHARGE:1;
	unsigned RG_DSI_V12_SEL:3;
	unsigned RG_DSI_V10_SEL:3;
	unsigned RG_DSI_V072_SEL:3;
	unsigned RG_DSI_V04_SEL:3;
	unsigned RG_DSI_V032_SEL:3;
	unsigned RG_DSI_V02_SEL:3;
	unsigned rsv_23:1;
	unsigned RG_DSI_BG_R1_TRIM:4;
	unsigned RG_DSI_BG_R2_TRIM:4;
} MIPITX_DSI_BG_CON_REG, *PMIPITX_DSI_BG_CON_REG;


typedef struct {
	unsigned RG_DSI0_MPPLL_PLL_EN:1;
	unsigned RG_DSI0_MPPLL_PREDIV:2;
	unsigned RG_DSI0_MPPLL_TXDIV0:2;
	unsigned RG_DSI0_MPPLL_TXDIV1:2;
	unsigned RG_DSI0_MPPLL_POSDIV:3;
	unsigned RG_DSI0_MPPLL_MONVC_EN:1;
	unsigned RG_DSI0_MPPLL_MONREF_EN:1;
	unsigned RG_DSI0_MPPLL_VDO_EN:1;
	unsigned rsv_13:19;
} MIPITX_DSI_PLL_CON0_REG, *PMIPITX_DSI_PLL_CON0_REG;


typedef struct {
	unsigned RG_DSI0_MPPLL_SDM_FRA_EN:1;
	unsigned RG_DSI0_MPPLL_SDM_SSC_PH_INIT:1;
	unsigned RG_DSI0_MPPLL_SDM_SSC_EN:1;
	unsigned rsv_3:13;
	unsigned RG_DSI0_MPPLL_SDM_SSC_PRD:16;
} MIPITX_DSI_PLL_CON1_REG, *PMIPITX_DSI_PLL_CON1_REG;

typedef struct {
	unsigned RG_DSI_MPPLL_SDM_PCW:31;
	unsigned rsv_31:1;
} MIPITX_DSI_PLL_CON2_REG, *PMIPITX_DSI_PLL_CON2_REG;


typedef struct {
	unsigned RG_DSI0_MPPLL_SDM_SSC_DELTA1:16;
	unsigned RG_DSI0_MPPLL_SDM_SSC_DELTA:16;
} MIPITX_DSI_PLL_CON3_REG, *PMIPITX_DSI_PLL_CON3_REG;


typedef struct {
	unsigned RG_DSI0_MPPLL_SDM_PCW_CHG:1;
	unsigned rsv_1:31;
} MIPITX_DSI_PLL_CHG_REG, *PMIPITX_DSI_PLL_CHG_REG;


typedef struct {
	unsigned RG_MPPLL_TST_EN:1;
	unsigned RG_MPPLL_TSTCK_EN:1;
	unsigned RG_MPPLL_TSTSEL:2;
	unsigned RG_MPPLL_S2QDIV:2;
	unsigned RG_MPPLL_PLLOUT_EN:1;
	unsigned RG_MPPLL_PRESERVE:5;
	unsigned rsv_12:20;
} MIPITX_DSI_PLL_TOP_REG, *PMIPITX_DSI_PLL_TOP_REG;


typedef struct {
	unsigned DA_DSI_MPPLL_SDM_PWR_ON:1;
	unsigned DA_DSI_MPPLL_SDM_ISO_EN:1;
	unsigned rsv_2:6;
	unsigned AD_DSI0_MPPLL_SDM_PWR_ACK:1;
	unsigned rsv_9:23;
} MIPITX_DSI_PLL_PWR_REG, *PMIPITX_DSI_PLL_PWR_REG;


typedef struct {
	unsigned RGS_DSI_LNT_IMP_CAL_OUTPUT:1;
	unsigned rsv_1:31;
} MIPITX_DSI_RGS_REG, *PMIPITX_DSI_RGS_REG;


typedef struct {
	unsigned RG_DSI0_GPI0_EN:1;
	unsigned RG_DSI0_GPI1_EN:1;
	unsigned RG_DSI0_GPI2_EN:1;
	unsigned RG_DSI0_GPI3_EN:1;
	unsigned RG_DSI0_GPI4_EN:1;
	unsigned RG_DSI0_GPI5_EN:1;
	unsigned RG_DSI0_GPI6_EN:1;
	unsigned RG_DSI0_GPI7_EN:1;
	unsigned RG_DSI0_GPI8_EN:1;
	unsigned RG_DSI0_GPI9_EN:1;
	unsigned RG_DSI0_GPI_SMT_EN:1;
	unsigned RG_DSI0_GPI_DRIVE_EN:1;
	unsigned rsv_12:20;
} MIPITX_DSI_GPI_EN_REG, *PMIPITX_DSI_GPI_EN_REG;

typedef struct {
	unsigned RG_DSI0_GPI0_PD:1;
	unsigned RG_DSI0_GPI1_PD:1;
	unsigned RG_DSI0_GPI2_PD:1;
	unsigned RG_DSI0_GPI3_PD:1;
	unsigned RG_DSI0_GPI4_PD:1;
	unsigned RG_DSI0_GPI5_PD:1;
	unsigned RG_DSI0_GPI6_PD:1;
	unsigned RG_DSI0_GPI7_PD:1;
	unsigned RG_DSI0_GPI8_PD:1;
	unsigned RG_DSI0_GPI9_PD:1;
	unsigned rsv_10:6;
	unsigned RG_DSI0_GPI0_PU:1;
	unsigned RG_DSI0_GPI1_PU:1;
	unsigned RG_DSI0_GPI2_PU:1;
	unsigned RG_DSI0_GPI3_PU:1;
	unsigned RG_DSI0_GPI4_PU:1;
	unsigned RG_DSI0_GPI5_PU:1;
	unsigned RG_DSI0_GPI6_PU:1;
	unsigned RG_DSI0_GPI7_PU:1;
	unsigned RG_DSI0_GPI8_PU:1;
	unsigned RG_DSI0_GPI9_PU:1;
	unsigned rsv_26:6;
} MIPITX_DSI_GPI_PULL_REG, *PMIPITX_DSI_GPI_PULL_REG;


typedef struct {
	unsigned MIPI_TX_PHY0_SEL:3;
	unsigned rsv_3:1;
	unsigned MIPI_TX_PHY1_SEL:3;
	unsigned rsv_7:1;
	unsigned MIPI_TX_PHY2_SEL:3;
	unsigned rsv_11:1;
	unsigned MIPI_TX_PHY3_SEL:3;
	unsigned rsv_15:1;
	unsigned MIPI_TX_PHYC_SEL:3;
	unsigned rsv_19:1;
	unsigned MIPI_TX_LPRX_SEL:3;
	unsigned rsv_23:9;
} MIPITX_DSI_PHY_SEL_REG, *PMIPITX_DSI_PHY_SEL_REG;


typedef struct {
	unsigned SW_CTRL_EN:1;
	unsigned rsv_1:31;
} MIPITX_DSI_SW_CTRL_REG, *PMIPITX_DSI_SW_CTRL_REG;


typedef struct {
	unsigned SW_LNTC_LPTX_PRE_OE:1;
	unsigned SW_LNTC_LPTX_OE:1;
	unsigned SW_LNTC_LPTX_DP:1;
	unsigned SW_LNTC_LPTX_DN:1;
	unsigned SW_LNTC_HSTX_PRE_OE:1;
	unsigned SW_LNTC_HSTX_OE:1;
	unsigned SW_LNTC_HSTX_RDY:1;
	unsigned SW_LNTC_LPRX_EN:1;
	unsigned SW_LNTC_HSTX_DATA:8;
	unsigned rsv_16:16;
} MIPITX_DSI_SW_CTRL_CON0_REG, *PMIPITX_DSI_SW_CTRL_CON0_REG;


typedef struct {
	unsigned SW_LNT0_LPTX_PRE_OE:1;
	unsigned SW_LNT0_LPTX_OE:1;
	unsigned SW_LNT0_LPTX_DP:1;
	unsigned SW_LNT0_LPTX_DN:1;
	unsigned SW_LNT0_HSTX_PRE_OE:1;
	unsigned SW_LNT0_HSTX_OE:1;
	unsigned SW_LNT0_HSTX_RDY:1;
	unsigned SW_LNT0_LPRX_EN:1;
	unsigned SW_LNT1_LPTX_PRE_OE:1;
	unsigned SW_LNT1_LPTX_OE:1;
	unsigned SW_LNT1_LPTX_DP:1;
	unsigned SW_LNT1_LPTX_DN:1;
	unsigned SW_LNT1_HSTX_PRE_OE:1;
	unsigned SW_LNT1_HSTX_OE:1;
	unsigned SW_LNT1_HSTX_RDY:1;
	unsigned SW_LNT1_LPRX_EN:1;
	unsigned SW_LNT2_LPTX_PRE_OE:1;
	unsigned SW_LNT2_LPTX_OE:1;
	unsigned SW_LNT2_LPTX_DP:1;
	unsigned SW_LNT2_LPTX_DN:1;
	unsigned SW_LNT2_HSTX_PRE_OE:1;
	unsigned SW_LNT2_HSTX_OE:1;
	unsigned SW_LNT2_HSTX_RDY:1;
	unsigned SW_LNT2_LPRX_EN:1;
	unsigned SW_LNT3_LPTX_PRE_OE:1;
	unsigned SW_LNT3_LPTX_OE:1;
	unsigned SW_LNT3_LPTX_DP:1;
	unsigned SW_LNT3_LPTX_DN:1;
	unsigned SW_LNT3_HSTX_PRE_OE:1;
	unsigned SW_LNT3_HSTX_OE:1;
	unsigned SW_LNT3_HSTX_RDY:1;
	unsigned SW_LNT3_LPRX_EN:1;
} MIPITX_DSI_SW_CTRL_CON1_REG, *PMIPITX_DSI_SW_CTRL_CON1_REG;

typedef struct {
	unsigned SW_LNT_HSTX_DATA:8;
	unsigned rsv_8:24;
} MIPITX_DSI_SW_CTRL_CON2_REG, *PMIPITX_DSI_SW_CTRL_CON2_REG;

typedef struct {
	unsigned MIPI_TX_DBG_SEL:4;
	unsigned MIPI_TX_DBG_OUT_EN:1;
	unsigned MIPI_TX_GPIO_MODE_EN:1;
	unsigned MIPI_TX_APB_ASYNC_CNT_EN:1;
        unsigned MIPI_TX_TST_CK_OUT_EN     : 1;
        unsigned MIPI_TX_TST_CK_OUT_SEL    : 1;
        unsigned rsv_9                     : 23;
} MIPITX_DSI_DBG_CON_REG, *PMIPITX_DSI_DBG_CON_REG;

typedef struct
{
	unsigned MIPI_TX_DBG_OUT           : 32;
}MIPI_DSI_DBG_OUT, *PMIPI_DSI_DBG_OUT;

typedef struct {
	unsigned MIPI_TX_APB_ASYNC_ERR:1;
	unsigned rsv_1:31;
} MIPITX_DSI_APB_ASYNC_STA_REG, *PMIPITX_DSI_APB_ASYNC_STA_REG;


/* field definition */
/* ------------------------------------------------------------- */
/* DSI */

typedef struct {
	unsigned DSI_START:1;
	unsigned rsv_1:1;
	unsigned SLEEPOUT_START:1;
	unsigned rsv_3:1;
	unsigned SKEWCAL_START:1;
	unsigned rsv_5:11;
	unsigned VM_CMD_START:1;
	unsigned rsv_17:15;
} DSI_START_REG, *PDSI_START_REG;


typedef struct {
	unsigned rsv_0:1;
	unsigned BUF_UNDERRUN:1;
	unsigned rsv_2:2;
	unsigned ESC_ENTRY_ERR:1;
	unsigned ESC_SYNC_ERR:1;
	unsigned CTRL_ERR:1;
	unsigned CONTENT_ERR:1;
	unsigned rsv_8:24;
} DSI_STATUS_REG, *PDSI_STATUS_REG;


typedef struct {
	unsigned RD_RDY:1;
	unsigned CMD_DONE:1;
	unsigned TE_RDY:1;
	unsigned VM_DONE:1;
	unsigned FRAME_DONE:1;
	unsigned VM_CMD_DONE:1;
	unsigned SLEEPOUT_DONE:1;
	unsigned TE_TIMEOUT:1;
	unsigned VM_VBP_STR:1;
	unsigned VM_VACT_STR:1;
	unsigned VM_VFP_STR:1;
	unsigned SKEWCAL_DONE:1;
	unsigned SLEEPIN_ULPS:1;
	unsigned rsv_13:19;
} DSI_INT_ENABLE_REG, *PDSI_INT_ENABLE_REG;


typedef struct {
	unsigned RD_RDY:1;
	unsigned CMD_DONE:1;
	unsigned TE_RDY:1;
	unsigned VM_DONE:1;
	unsigned FRAME_DONE_INT_EN:1;
	unsigned VM_CMD_DONE:1;
	unsigned SLEEPOUT_DONE:1;
	unsigned TE_TIMEOUT:1;
	unsigned VM_VBP_STR:1;
	unsigned VM_VACT_STR:1;
	unsigned VM_VFP_STR:1;
	unsigned SKEWCAL_DONE:1;
	unsigned SLEEPIN_ULPS:1;
	unsigned rsv_13:18;
	unsigned BUSY:1;
} DSI_INT_STATUS_REG, *PDSI_INT_STATUS_REG;


typedef struct {
	unsigned DSI_RESET:1;
	unsigned rsv_1:1;
	unsigned DPHY_RESET:1;
	unsigned rsv_3:1;
	unsigned DSI_DUAL_EN:1;
	unsigned rsv_5:27;
} DSI_COM_CTRL_REG, *PDSI_COM_CTRL_REG;


typedef enum {
	DSI_CMD_MODE = 0,
	DSI_SYNC_PULSE_VDO_MODE = 1,
	DSI_SYNC_EVENT_VDO_MODE = 2,
	DSI_BURST_VDO_MODE = 3
} DSI_MODE_CTRL;


typedef struct {
	unsigned MODE:2;
	unsigned rsv_2:14;
	unsigned FRM_MODE:1;
	unsigned MIX_MODE:1;
	unsigned V2C_SWITCH_ON:1;
	unsigned C2V_SWITCH_ON:1;
	unsigned SLEEP_MODE:1;
	unsigned rsv_21:11;
} DSI_MODE_CTRL_REG, *PDSI_MODE_CTRL_REG;


typedef enum {
	ONE_LANE = 1,
	TWO_LANE = 2,
	THREE_LANE = 3,
	FOUR_LANE = 4
} DSI_LANE_NUM;


typedef struct {
	unsigned VC_NUM:2;
	unsigned LANE_NUM:4;
	unsigned DIS_EOT:1;
	unsigned BLLP_EN:1;
	unsigned TE_FREERUN:1;
	unsigned EXT_TE_EN:1;
	unsigned EXT_TE_EDGE:1;
	unsigned TE_AUTO_SYNC:1;
	unsigned MAX_RTN_SIZE:4;
	unsigned HSTX_CKLP_EN:1;
	unsigned TYPE1_BTA_SEL:1;
	unsigned TE_WITH_CMD_EN:1;
	unsigned TE_TIMEOUT_CHK_EN:1;
	unsigned EXT_TE_TIME_VM:4;
	unsigned RGB_PKT_CNT:4;
	unsigned LP_ONLY_VBLK:1;
	unsigned rsv_29:3;
} DSI_TXRX_CTRL_REG, *PDSI_TXRX_CTRL_REG;


typedef enum {
	PACKED_PS_16BIT_RGB565 = 0,
	LOOSELY_PS_18BIT_RGB666 = 1,
	PACKED_PS_24BIT_RGB888 = 2,
	PACKED_PS_18BIT_RGB666 = 3
} DSI_PS_TYPE;


typedef struct {
	unsigned DSI_PS_WC:15;
	unsigned rsv_15:1;
	unsigned DSI_PS_SEL:3;
	unsigned rsv_19:5;
	unsigned RGB_SWAP:1;
	unsigned BYTE_SWAP:1;
	unsigned CUSTOM_HEADER:6;
} DSI_PSCTRL_REG, *PDSI_PSCTRL_REG;


typedef struct {
	unsigned VSA_NL:12;
	unsigned rsv_12:20;
} DSI_VSA_NL_REG, *PDSI_VSA_NL_REG;


typedef struct {
	unsigned VBP_NL:12;
	unsigned rsv_12:20;
} DSI_VBP_NL_REG, *PDSI_VBP_NL_REG;


typedef struct {
	unsigned VFP_NL:12;
	unsigned rsv_12:20;
} DSI_VFP_NL_REG, *PDSI_VFP_NL_REG;


typedef struct {
	unsigned VACT_NL:12;
	unsigned rsv_12:20;
} DSI_VACT_NL_REG, *PDSI_VACT_NL_REG;

typedef struct {
	unsigned LFR_MODE:2;
	unsigned LFR_TYPE:2;
	unsigned LFR_EN:1;
	unsigned LFR_UPDATE:1;
	unsigned LFR_VSE_DIS:1;
	unsigned rsv_7:1;
	unsigned LFR_SKIP_NUM:6;
	unsigned rsv_14:18;
} DSI_LFR_CON_REG, *PDSI_LFR_CON_REG;


typedef struct {
	unsigned LFR_SKIP_CNT:6;
	unsigned rsv_6:2;
	unsigned LFR_SKIP_STA:1;
	unsigned rsv_9:23;
} DSI_LFR_STA_REG, *PDSI_LFR_STA_REG;


typedef struct {
	unsigned DSI_WIDTH:15;
	unsigned rsv_15:1;
	unsigned DSI_HEIGHT:15;
	unsigned rsv_31:1;
} DSI_SIZE_CON_REG, *PDSI_SIZE_CON_REG;


typedef struct {
	unsigned HSA_WC:12;
	unsigned rsv_12:20;
} DSI_HSA_WC_REG, *PDSI_HSA_WC_REG;


typedef struct {
	unsigned HBP_WC:12;
	unsigned rsv_12:20;
} DSI_HBP_WC_REG, *PDSI_HBP_WC_REG;


typedef struct {
	unsigned HFP_WC:12;
	unsigned rsv_12:20;
} DSI_HFP_WC_REG, *PDSI_HFP_WC_REG;

typedef struct {
	unsigned BLLP_WC:12;
	unsigned rsv_12:20;
} DSI_BLLP_WC_REG, *PDSI_BLLP_WC_REG;

typedef struct {
	unsigned CMDQ_SIZE:8;
	unsigned rsv_8:24;
} DSI_CMDQ_CTRL_REG, *PDSI_CMDQ_CTRL_REG;

typedef struct {
	unsigned rsv_0:2;
	unsigned HSTX_CKLP_WC:14;
	unsigned HSTX_CKLP_WC_AUTO:1;
	unsigned rsv_17:15;
} DSI_HSTX_CKLP_REG, *PDSI_HSTX_CKLP_REG;

typedef struct {
	unsigned HSTX_CKLP_WC_AUTO_RESULT:17;
	unsigned rsv_17:15;
} DSI_HSTX_CKLP_WC_AUTO_RESULT_REG, *PDSI_HSTX_CKLP_WC_AUTO_RESULT_REG;

typedef struct {
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
} DSI_RX_DATA_REG, *PDSI_RX_DATA_REG;


typedef struct {
	unsigned DSI_RACK:1;
	unsigned DSI_RACK_BYPASS:1;
	unsigned rsv2:30;
} DSI_RACK_REG, *PDSI_RACK_REG;


typedef struct {
	unsigned TRIG0:1;	/* remote rst */
	unsigned TRIG1:1;	/* TE */
	unsigned TRIG2:1;	/* ack */
	unsigned TRIG3:1;	/* rsv */
	unsigned RX_ULPS:1;
	unsigned DIRECTION:1;
	unsigned RX_LPDT:1;
	unsigned rsv7:1;
	unsigned RX_POINTER:4;
	unsigned rsv12:20;
} DSI_TRIG_STA_REG, *PDSI_TRIG_STA_REG;


typedef struct {
	unsigned RWMEM_CONTI:16;
	unsigned rsv16:16;
} DSI_MEM_CONTI_REG, *PDSI_MEM_CONTI_REG;


typedef struct {
	unsigned FRM_BC:21;
	unsigned rsv21:11;
} DSI_FRM_BC_REG, *PDSI_FRM_BC_REG;

typedef struct {
	unsigned _3D_MODE:2;
	unsigned _3D_FMT:2;
	unsigned _3D_VSYNC:1;
	unsigned _3D_LR:1;
	unsigned rsv6:2;
	unsigned _3D_EN:1;
	unsigned rsv9:23;
} DSI_3D_CON_REG, *PDSI_3D_CON_REG;

typedef struct {
	unsigned UPLS_WAKEUP_PRD:16;
	unsigned SKEWCALL_PRD:16;
} DSI_TIME_CON0_REG, *PDSI_TIME_CON0_REG;

typedef struct {
	unsigned TE_WAKEUP_PRD:16;
	unsigned PREFETCH_TIME:15;
	unsigned PREFETCH_EN:1;
} DSI_TIME_CON1_REG, *PDSI_TIME_CON1_REG;


typedef struct {
	unsigned LC_HSTX_CK_PAT:32;
} DSI_PHY_LCPAT_REG, *PDSI_PHY_LCPAT_REG;

typedef struct {
	unsigned LC_HS_TX_EN:1;
	unsigned LC_ULPM_EN:1;
	unsigned LC_WAKEUP_EN:1;
	unsigned TRAIL_FIX:1;
	unsigned rsv4:4;
	unsigned EARLY_DRDY:5;
	unsigned rsv13:3;
	unsigned EARLY_HS_POE:5;
	unsigned rsv21:11;
} DSI_PHY_LCCON_REG, *PDSI_PHY_LCCON_REG;


typedef struct {
	unsigned L0_RM_TRIG_EN:1;
	unsigned L0_ULPM_EN:1;
	unsigned L0_WAKEUP_EN:1;
	unsigned Lx_ULPM_AS_L0:1;
	unsigned L0_RX_FILTER_EN:1;
	unsigned rsv3:27;
} DSI_PHY_LD0CON_REG, *PDSI_PHY_LD0CON_REG;


typedef struct {
	unsigned HS_SYNC_CODE:8;
	unsigned HS_SYNC_CODE2:8;
	unsigned HS_SKEWCAL_PAT:8;
	unsigned HS_DB_SYNC_EN:1;
	unsigned rsv25:7;
} DSI_PHY_SYNCON_REG, *PDSI_PHY_SYNCON_REG;


typedef struct {
	unsigned char LPX;
	unsigned char HS_PRPR;
	unsigned char HS_ZERO;
	unsigned char HS_TRAIL;
} DSI_PHY_TIMCON0_REG, *PDSI_PHY_TIMCON0_REG;


typedef struct {
	unsigned char TA_GO;
	unsigned char TA_SURE;
	unsigned char TA_GET;
	unsigned char DA_HS_EXIT;
} DSI_PHY_TIMCON1_REG, *PDSI_PHY_TIMCON1_REG;


typedef struct {
	unsigned char CONT_DET;
	unsigned char DA_HS_SYNC;
	unsigned char CLK_ZERO;
	unsigned char CLK_TRAIL;
} DSI_PHY_TIMCON2_REG, *PDSI_PHY_TIMCON2_REG;


typedef struct {
	unsigned char CLK_HS_PRPR;
	unsigned char CLK_HS_POST;
	unsigned char CLK_HS_EXIT;
	unsigned rsv24:8;
} DSI_PHY_TIMCON3_REG, *PDSI_PHY_TIMCON3_REG;


typedef struct {
	unsigned VM_CMD_EN:1;
	unsigned LONG_PKT:1;
	unsigned TIME_SEL:1;
	unsigned TS_VSA_EN:1;
	unsigned TS_VBP_EN:1;
	unsigned TS_VFP_EN:1;
	unsigned rsv6:2;
	unsigned CM_DATA_ID:8;
	unsigned CM_DATA_0:8;
	unsigned CM_DATA_1:8;
} DSI_VM_CMD_CON_REG, *PDSI_VM_CMD_CON_REG;


typedef struct {
	DSI_PHY_TIMCON0_REG CTRL0;
	DSI_PHY_TIMCON1_REG CTRL1;
	DSI_PHY_TIMCON2_REG CTRL2;
	DSI_PHY_TIMCON3_REG CTRL3;
} DSI_PHY_TIMCON_REG, *PDSI_PHY_TIMCON_REG;


typedef struct {
	unsigned PKT_CHECK_SUM:16;
	unsigned ACC_CHECK_SUM:16;
} DSI_CKSM_OUT_REG, *PDSI_CKSM_OUT_REG;


typedef struct {
	unsigned DPHY_CTL_STATE_C:9;
	unsigned rsv9:7;
	unsigned DPHY_HS_TX_STATE_C:5;
	unsigned rsv21:11;
} DSI_STATE_DBG0_REG, *PDSI_STATE_DBG0_REG;


typedef struct {
	unsigned CTL_STATE_C:15;
	unsigned rsv15:1;
	unsigned HS_TX_STATE_0:5;
	unsigned rsv21:3;
	unsigned ESC_STATE_0:8;
} DSI_STATE_DBG1_REG, *PDSI_STATE_DBG1_REG;


typedef struct {
	unsigned RX_ESC_STATE:10;
	unsigned rsv10:6;
	unsigned TA_T2R_STATE:5;
	unsigned rsv21:3;
	unsigned TA_R2T_STATE:5;
	unsigned rsv29:3;
} DSI_STATE_DBG2_REG, *PDSI_STATE_DBG2_REG;


typedef struct {
	unsigned CTL_STATE_1:5;
	unsigned rsv5:3;
	unsigned HS_TX_STATE_1:5;
	unsigned rsv13:3;
	unsigned CTL_STATE_2:5;
	unsigned rsv21:3;
	unsigned HS_TX_STATE_2:5;
	unsigned rsv29:3;
} DSI_STATE_DBG3_REG, *PDSI_STATE_DBG3_REG;


typedef struct {
	unsigned CTL_STATE_3:5;
	unsigned rsv5:3;
	unsigned HS_TX_STATE_3:5;
	unsigned rsv13:19;
} DSI_STATE_DBG4_REG, *PDSI_STATE_DBG4_REG;


typedef struct {
	unsigned TIMER_COUNTER:16;
	unsigned TIMER_BUSY:1;
	unsigned rsv17:11;
	unsigned WAKEUP_STATE:4;
} DSI_STATE_DBG5_REG, *PDSI_STATE_DBG5_REG;


typedef struct {
	unsigned CMTRL_STATE:15;
	unsigned rsv15:1;
	unsigned CMDQ_STATE:7;
	unsigned rsv23:9;
} DSI_STATE_DBG6_REG, *PDSI_STATE_DBG6_REG;


typedef struct {
	unsigned VMCTL_STATE:11;
	unsigned rsv11:1;
	unsigned VFP_PERIOD:1;
	unsigned VACT_PERIOD:1;
	unsigned VBP_PERIOD:1;
	unsigned VSA_PERIOD:1;
	unsigned rsv16:16;
} DSI_STATE_DBG7_REG, *PDSI_STATE_DBG7_REG;


typedef struct {
	unsigned WORD_COUNTER:15;
	unsigned rsv15:1;
	unsigned PREFETCH_CNT:15;
	unsigned DSI_PREFETCH_MUTEX:1;
} DSI_STATE_DBG8_REG, *PDSI_STATE_DBG8_REG;


typedef struct {
	unsigned LINE_COUNTER:22;
	unsigned rsv22:10;
} DSI_STATE_DBG9_REG, *PDSI_STATE_DBG9_REG;


typedef struct {
	unsigned DEBUG_OUT_SEL:5;
	unsigned rsv5:3;
	unsigned CHKSUM_REC_EN:1;
	unsigned C2V_START_CON:1;
	unsigned rsv10:4;
	unsigned DYNAMIC_CG_EN:18;
} DSI_DEBUG_SEL_REG, *PDSI_DEBUG_SEL_REG;

typedef struct {
	unsigned LIMIT_WIDTH:15;
	unsigned rsv_15:1;
	unsigned LIMIT_HEIGHT:15;
	unsigned rsv_31:1;
} DSI_STATE_DBG10_REG, *PDSI_STATE_DBG10_REG;


typedef struct {
	unsigned BIST_MODE:1;
	unsigned BIST_ENABLE:1;
	unsigned BIST_FIX_PATTERN:1;
	unsigned BIST_SPC_PATTERN:1;
	unsigned BIST_HS_FREE:1;
	unsigned rsv_05:1;
	unsigned SELF_PAT_MODE:1;
	unsigned rsv_07:1;
	unsigned BIST_LANE_NUM:4; /* To be confirmed */
	unsigned rsv12:4;
	unsigned BIST_TIMING:8;
	unsigned rsv24:8;
} DSI_BIST_CON_REG, *PDSI_BIST_CON_REG;

typedef struct {
	unsigned LIMIT_PS_WC:15;
	unsigned rsv_15:17;
} DSI_STATE_DBG11_REG, *PDSI_STATE_DBG11_REG;

typedef struct {
	unsigned FORCE_COMMIT:1;
	unsigned BYPASS_SHADOW:1;
	unsigned READ_WRK_REG:1;
	unsigned rsv_3:29;
} DSI_SHADOW_DBG_REG, *PDSI_SHADOW_DBG_REG;

typedef struct {
	unsigned UPD_VACT_ERR:1;
	unsigned UPD_VFP_ERR:1;
	unsigned rsv_2:30;
} DSI_SHADOW_STA_REG, *PDSI_SHADOW_STA_REG;

typedef struct {
	DSI_START_REG DSI_START;	/* 0000 */
	DSI_STATUS_REG DSI_STA;	/* 0004 */
	DSI_INT_ENABLE_REG DSI_INTEN;	/* 0008 */
	DSI_INT_STATUS_REG DSI_INTSTA;	/* 000C */
	DSI_COM_CTRL_REG DSI_COM_CTRL;	/* 0010 */
	DSI_MODE_CTRL_REG DSI_MODE_CTRL;	/* 0014 */
	DSI_TXRX_CTRL_REG DSI_TXRX_CTRL;	/* 0018 */
	DSI_PSCTRL_REG DSI_PSCTRL;	/* 001C */
	DSI_VSA_NL_REG DSI_VSA_NL;	/* 0020 */
	DSI_VBP_NL_REG DSI_VBP_NL;	/* 0024 */
	DSI_VFP_NL_REG DSI_VFP_NL;	/* 0028 */
	DSI_VACT_NL_REG DSI_VACT_NL;	/* 002C */
	DSI_LFR_CON_REG DSI_LFR_CON;	/* 0030 */
	DSI_LFR_STA_REG DSI_LFR_STA;	/* 0034 */
	DSI_SIZE_CON_REG DSI_SIZE_CON;	/* 0038 */
	UINT32 rsv_3c[5];	/* 003C..004C */
	DSI_HSA_WC_REG DSI_HSA_WC;	/* 0050 */
	DSI_HBP_WC_REG DSI_HBP_WC;	/* 0054 */
	DSI_HFP_WC_REG DSI_HFP_WC;	/* 0058 */
	DSI_BLLP_WC_REG DSI_BLLP_WC;	/* 005C */
	DSI_CMDQ_CTRL_REG DSI_CMDQ_SIZE;	/* 0060 */
	DSI_HSTX_CKLP_REG DSI_HSTX_CKL_WC;	/* 0064 */
	DSI_HSTX_CKLP_WC_AUTO_RESULT_REG DSI_HSTX_CKL_WC_AUTO_RESULT;	/* 0068 */
	UINT32 rsv_006C[2];	/* 006c..0070 */
	DSI_RX_DATA_REG DSI_RX_DATA0;	/* 0074 */
	DSI_RX_DATA_REG DSI_RX_DATA1;	/* 0078 */
	DSI_RX_DATA_REG DSI_RX_DATA2;	/* 007c */
	DSI_RX_DATA_REG DSI_RX_DATA3;	/* 0080 */
	DSI_RACK_REG DSI_RACK;	/* 0084 */
	DSI_TRIG_STA_REG DSI_TRIG_STA;	/* 0088 */
	UINT32 rsv_008C;	/* 008C */
	DSI_MEM_CONTI_REG DSI_MEM_CONTI;	/* 0090 */
	DSI_FRM_BC_REG DSI_FRM_BC;	/* 0094 */
	DSI_3D_CON_REG DSI_3D_CON;	/* 0098 */
	UINT32 rsv_009C;	/* 009c */
	DSI_TIME_CON0_REG DSI_TIME_CON0;	/* 00A0 */
	DSI_TIME_CON1_REG DSI_TIME_CON1;	/* 00A4 */

	UINT32 rsv_00A8[22];	/* 0A8..0FC */
	UINT32 DSI_PHY_PCPAT;	/* 00100 */

	DSI_PHY_LCCON_REG DSI_PHY_LCCON;	/* 0104 */
	DSI_PHY_LD0CON_REG DSI_PHY_LD0CON;	/* 0108 */
	DSI_PHY_SYNCON_REG DSI_PHY_SYNCON;	/* 010C */
	DSI_PHY_TIMCON0_REG DSI_PHY_TIMECON0;	/* 0110 */
	DSI_PHY_TIMCON1_REG DSI_PHY_TIMECON1;	/* 0114 */
	DSI_PHY_TIMCON2_REG DSI_PHY_TIMECON2;	/* 0118 */
	DSI_PHY_TIMCON3_REG DSI_PHY_TIMECON3;	/* 011C */
	UINT32 rsv_0120[4];	/* 0120..012c */
	DSI_VM_CMD_CON_REG DSI_VM_CMD_CON;	/* 0130 */
	UINT32 DSI_VM_CMD_DATA0;	/* 0134 */
	UINT32 DSI_VM_CMD_DATA4;	/* 0138 */
	UINT32 DSI_VM_CMD_DATA8;	/* 013C */
	UINT32 DSI_VM_CMD_DATAC;	/* 0140 */
	DSI_CKSM_OUT_REG DSI_CKSM_OUT;	/* 0144 */
	DSI_STATE_DBG0_REG DSI_STATE_DBG0;	/* 0148 */
	DSI_STATE_DBG1_REG DSI_STATE_DBG1;	/* 014C */
	DSI_STATE_DBG2_REG DSI_STATE_DBG2;	/* 0150 */
	DSI_STATE_DBG3_REG DSI_STATE_DBG3;	/* 0154 */
	DSI_STATE_DBG4_REG DSI_STATE_DBG4;	/* 0158 */
	DSI_STATE_DBG5_REG DSI_STATE_DBG5;	/* 015C */
	DSI_STATE_DBG6_REG DSI_STATE_DBG6;	/* 0160 */
	DSI_STATE_DBG7_REG DSI_STATE_DBG7;	/* 0164 */
	DSI_STATE_DBG8_REG DSI_STATE_DBG8;	/* 0168 */
	DSI_STATE_DBG9_REG DSI_STATE_DBG9;	/* 016C */
	DSI_DEBUG_SEL_REG DSI_DEBUG_SEL;	/* 0170 */
	DSI_STATE_DBG10_REG DSI_STATE_DBG10;	/* 0174 */
	UINT32 DSI_BIST_PATTERN;	/* 0178 */
	DSI_BIST_CON_REG DSI_BIST_CON;	/* 017C */
	UINT32 DSI_VM_CMD_DATA10;	/* 00180 */
	UINT32 DSI_VM_CMD_DATA14;	/* 00184 */
	UINT32 DSI_VM_CMD_DATA18;	/* 00188 */
	UINT32 DSI_VM_CMD_DATA1C;	/* 0018C */
	DSI_STATE_DBG11_REG DSI_STATE_DBG11;	/* 0190 */
	UINT32 rsv_0194;			/* 0194 */
	DSI_SHADOW_DBG_REG DSI_SHADOW_DBG;	/* 0198 */
	DSI_SHADOW_STA_REG DSI_SHADOW_STA;	/* 019C */
} volatile DSI_REGS, *PDSI_REGS;

/* 0~1 TYPE ,2 BTA,3 HS, 4 CL,5 TE,6~7 RESV, 8~15 DATA_ID,16~23 DATA_0,24~31 DATA_1 */
typedef struct {
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
} DSI_CMDQ, *PDSI_CMDQ;

typedef struct {
	DSI_CMDQ data[128]; /* only support 128 cmdq */
} DSI_CMDQ_REGS, *PDSI_CMDQ_REGS;

typedef struct {
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
} DSI_VM_CMDQ, *PDSI_VM_CMDQ;

typedef struct {
	DSI_VM_CMDQ data[4];
} DSI_VM_CMDQ_REGS, *PDSI_VM_CMDQ_REGS;

typedef struct {
	MIPITX_DSI_CON_REG MIPITX_DSI_CON;	/* 0000 */
	MIPITX_DSI_CLOCK_LANE_REG MIPITX_DSI_CLOCK_LANE;	/* 0004 */
	MIPITX_DSI_DATA_LANE0_REG MIPITX_DSI_DATA_LANE0;	/* 0008 */
	MIPITX_DSI_DATA_LANE1_REG MIPITX_DSI_DATA_LANE1;	/* 000C */
	MIPITX_DSI_DATA_LANE2_REG MIPITX_DSI_DATA_LANE2;	/* 0010 */
	MIPITX_DSI_DATA_LANE3_REG MIPITX_DSI_DATA_LANE3;	/* 0014 */
	UINT32 rsv_18[10];	/* 0018..003C */

	MIPITX_DSI_TOP_CON_REG MIPITX_DSI_TOP_CON;	/* 0040 */
	MIPITX_DSI_BG_CON_REG MIPITX_DSI_BG_CON;	/* 0044 */
	UINT32 rsv_48[2];	/* 0048..004C */
	MIPITX_DSI_PLL_CON0_REG MIPITX_DSI_PLL_CON0;	/* 0050 */
	MIPITX_DSI_PLL_CON1_REG MIPITX_DSI_PLL_CON1;	/* 0054 */
	MIPITX_DSI_PLL_CON2_REG MIPITX_DSI_PLL_CON2;	/* 0058 */
	MIPITX_DSI_PLL_CON3_REG MIPITX_DSI_PLL_CON3;	/* 005C */
	MIPITX_DSI_PLL_CHG_REG MIPITX_DSI_PLL_CHG;	/* 0060 */
	MIPITX_DSI_PLL_TOP_REG MIPITX_DSI_PLL_TOP;	/* 0064 */
	MIPITX_DSI_PLL_PWR_REG MIPITX_DSI_PLL_PWR;	/* 0068 */
	UINT32 rsv_6C;		/* 006C */
	MIPITX_DSI_RGS_REG MIPITX_DSI_RGS;	/* 0070 */
	MIPITX_DSI_GPI_EN_REG MIPITX_DSI_GPI_EN;	/* 0074 */
	MIPITX_DSI_GPI_PULL_REG MIPITX_DSI_GPI_PULL;	/* 0078 */
	MIPITX_DSI_PHY_SEL_REG MIPITX_DSI_PHY_SEL;	/* 007C */

	MIPITX_DSI_SW_CTRL_REG MIPITX_DSI_SW_CTRL_EN;	/* 0080 */
	MIPITX_DSI_SW_CTRL_CON0_REG MIPITX_DSI_SW_CTRL_CON0;	/* 0084 */
	MIPITX_DSI_SW_CTRL_CON1_REG MIPITX_DSI_SW_CTRL_CON1;	/* 0088 */
	MIPITX_DSI_SW_CTRL_CON2_REG MIPITX_DSI_SW_CTRL_CON2;	/* 008C */
	MIPITX_DSI_DBG_CON_REG MIPITX_DSI_DBG_CON;	/* 0090 */
	MIPI_DSI_DBG_OUT MIPITX_DSI_DBG_OUT;		/* 0094 */
	MIPITX_DSI_APB_ASYNC_STA_REG MIPITX_DSI_APB_ASYNC_STA;	/* 0098 */

} volatile DSI_PHY_REGS, *PDSI_PHY_REGS;

#ifndef BUILD_LK
/*
STATIC_ASSERT(0x0050 == offsetof(DSI_PHY_REGS, MIPITX_DSI_PLL_CON0));
STATIC_ASSERT(0x0070 == offsetof(DSI_PHY_REGS, MIPITX_DSI_RGS));
STATIC_ASSERT(0x0080 == offsetof(DSI_PHY_REGS, MIPITX_DSI_SW_CTRL_EN));
STATIC_ASSERT(0x0090 == offsetof(DSI_PHY_REGS, MIPITX_DSI_DBG_CON));

STATIC_ASSERT(0x002C == offsetof(DSI_REGS, DSI_VACT_NL));
STATIC_ASSERT(0x0104 == offsetof(DSI_REGS, DSI_PHY_LCCON));
STATIC_ASSERT(0x011C == offsetof(DSI_REGS, DSI_PHY_TIMECON3));
STATIC_ASSERT(0x017C == offsetof(DSI_REGS, DSI_BIST_CON));
STATIC_ASSERT(0x0100 == offsetof(DSI_REGS, DSI_PHY_PCPAT));


STATIC_ASSERT(0x0098 == offsetof(DSI_REGS, DSI_3D_CON));
*/
#endif


#define DISP_INDEX_OFFSET 0x1000
#define DISP_RDMA_INDEX_OFFSET 0x1000

//TDODO: get base reg addr from system header
#define DDP_REG_BASE_MMSYS_CONFIG    MMSYS_CONFIG_BASE //0xf4000000
#define DDP_REG_BASE_DISP_OVL0       OVL0_BASE         //0xf4008000
#define DDP_REG_BASE_DISP_OVL1       OVL1_BASE         //0xf4009000
#define DDP_REG_BASE_DISP_OVL0_2L    DISP_OVL0_2L_BASE //0xf4018000
#define DDP_REG_BASE_DISP_OVL1_2L    DISP_OVL1_2L_BASE //0xf4019000
#define DDP_REG_BASE_DISP_RDMA0      DISP_RDMA0_BASE   //0xf400A000
#define DDP_REG_BASE_DISP_RDMA1      DISP_RDMA1_BASE   //0xf400B000
#define DDP_REG_BASE_DISP_WDMA0      DISP_WDMA0_BASE   //0xf400C000
#define DDP_REG_BASE_DISP_WDMA1      DISP_WDMA1_BASE   //0xf4017000
#define DDP_REG_BASE_DISP_UFOE       DISP_UFOE_BASE    //0xf401A000
#define DDP_REG_BASE_DISP_SPLIT0     DISP_SPLIT0_BASE  //0xf4018000
#define DDP_REG_BASE_DSI0            DSI0_BASE         //0xf4012000
#define DDP_REG_BASE_DSI1            DSI1_BASE         //0xf4012000
#define DDP_REG_BASE_MM_MUTEX        MM_MUTEX_BASE     //0xf4014000

#define DDP_REG_BASE_DISP_COLOR0     COLOR0_BASE       //0xf400D000
#define DDP_REG_BASE_DISP_CCORR      CCORR_BASE        //0xf400E000
#define DDP_REG_BASE_DISP_AAL        DISP_AAL_BASE     //0xf400F000
#define DDP_REG_BASE_DISP_GAMMA      DISP_GAMMA_BASE   //0xf4010000
#define DDP_REG_BASE_DITHER          DITHER_BASE       //0xf4011000
#define DDP_REG_BASE_DPI             DPI_BASE          //0xf4013000
#define DDP_REG_BASE_DISP_PWM0       DISP_PWM0_BASE    //0xf100E000
#define DDP_REG_BASE_SMI_LARB0       SMI_LARB0_BASE    //0xf4015000
#define DDP_REG_BASE_SMI_LARB4       SMI_LARB4_BASE    //0xf4015000
#define DDP_REG_BASE_SMI_COMMON      SMI_COMMON_BASE   //0xf4016000

#define DDP_REG_BASE_DISP_COLOR1     COLOR1_BASE       //0xf4014000
#define DDP_REG_BASE_DISP_MERGE      0 //DISP_MERGE_BASE   //0xf4017000
#define DDP_REG_BASE_DISP_SPLIT1     0 //DISP_SPLIT1_BASE  //0xf4019000
#define DDP_REG_BASE_DISP_PWM1       0 //DISP_PWM1_BASE    //0xf401F000
#define DDP_REG_BASE_DISP_OD         DISP_OD_BASE      //0xf4023000
#define DDP_REG_BASE_DISP_RDMA2      0 //DISP_RDMA2_BASE   //0xf4010000


#define DISPSYS_REG_ADDR_MIN DDP_REG_BASE_MMSYS_CONFIG
#define DISPSYS_REG_ADDR_MAX DDP_REG_BASE_DISP_OVL1_2L

#define DISPSYS_CONFIG_BASE             DDP_REG_BASE_MMSYS_CONFIG
#define DISPSYS_OVL0_BASE               DDP_REG_BASE_DISP_OVL0
#define DISPSYS_OVL1_BASE               DDP_REG_BASE_DISP_OVL1
#define DISPSYS_RDMA0_BASE              DDP_REG_BASE_DISP_RDMA0
#define DISPSYS_RDMA1_BASE              DDP_REG_BASE_DISP_RDMA1
#define DISPSYS_WDMA0_BASE              DDP_REG_BASE_DISP_WDMA0
#define DISPSYS_WDMA1_BASE              DDP_REG_BASE_DISP_WDMA1
#define DISPSYS_COLOR0_BASE             DDP_REG_BASE_DISP_COLOR0
#define DISPSYS_COLOR1_BASE             DDP_REG_BASE_DISP_COLOR1
#define DISPSYS_AAL_BASE                DDP_REG_BASE_DISP_AAL
#define DISPSYS_GAMMA_BASE              DDP_REG_BASE_DISP_GAMMA
#define DISPSYS_MERGE_BASE              DDP_REG_BASE_DISP_MERGE
#define DISPSYS_SPLIT0_BASE             DDP_REG_BASE_DISP_SPLIT0
#define DISPSYS_SPLIT1_BASE             DDP_REG_BASE_DISP_SPLIT1
#define DISPSYS_UFOE_BASE               DDP_REG_BASE_DISP_UFOE
#define DISPSYS_DSI0_BASE               DDP_REG_BASE_DSI0
#define DISPSYS_DITHER_BASE             DDP_REG_BASE_DITHER
#define DISPSYS_DSI1_BASE               DDP_REG_BASE_DSI1
#define DISPSYS_DPI_BASE                DDP_REG_BASE_DPI
#define DISPSYS_PWM0_BASE               DDP_REG_BASE_DISP_PWM0
#define DISPSYS_PWM1_BASE               DDP_REG_BASE_DISP_PWM1
#define DISPSYS_MUTEX_BASE              DDP_REG_BASE_MM_MUTEX
#define DISPSYS_SMI_LARB0_BASE          DDP_REG_BASE_SMI_LARB0
#define DISPSYS_SMI_LARB4_BASE          DDP_REG_BASE_SMI_LARB4
#define DISPSYS_SMI_COMMON_BASE         DDP_REG_BASE_SMI_COMMON
#define DISPSYS_OD_BASE                 DDP_REG_BASE_DISP_OD
#define DISPSYS_CCORR_BASE              DDP_REG_BASE_DISP_CCORR
// ---------------------------------------------------------------------------
//  Type Casting
// ---------------------------------------------------------------------------

#define AS_INT32(x)     (*(INT32 *)(x))
#define AS_INT16(x)     (*(INT16 *)(x))
#define AS_INT8(x)      (*(INT8  *)(x))

#define AS_UINT32(x)    (*(UINT32 *)(x))
#define AS_UINT16(x)    (*(UINT16 *)(x))
#define AS_UINT8(x)     (*(UINT8  *)(x))


// ---------------------------------------------------------------------------
//  Register Manipulations
// ---------------------------------------------------------------------------

#define READ_REGISTER_UINT32(reg) \
    (*(volatile UINT32 * const)(reg))

#define WRITE_REGISTER_UINT32(reg, val) \
    (*(volatile UINT32 * const)(reg)) = (val)

#define READ_REGISTER_UINT16(reg) \
    (*(volatile UINT16 * const)(reg))

#define WRITE_REGISTER_UINT16(reg, val) \
    (*(volatile UINT16 * const)(reg)) = (val)

#define READ_REGISTER_UINT8(reg) \
    (*(volatile UINT8 * const)(reg))

#define WRITE_REGISTER_UINT8(reg, val) \
    (*(volatile UINT8 * const)(reg)) = (val)

// ---------------------------------------------------------------------------
//  Register Field Access
// ---------------------------------------------------------------------------

#define REG_FLD(width, shift) \
	((unsigned int)((((width) & 0xFF) << 16) | ((shift) & 0xFF)))

#define REG_FLD_MSB_LSB(msb, lsb) REG_FLD((msb) - (lsb) + 1, (lsb))

#define REG_FLD_WIDTH(field) \
	((unsigned int)(((field) >> 16) & 0xFF))

#define REG_FLD_SHIFT(field) \
	((unsigned int)((field) & 0xFF))

#define REG_FLD_MASK(field) \
	((unsigned int)((1ULL << REG_FLD_WIDTH(field)) - 1) << REG_FLD_SHIFT(field))

#define REG_FLD_VAL(field, val) \
	(((val) << REG_FLD_SHIFT(field)) & REG_FLD_MASK(field))

#define REG_FLD_VAL_GET(field, regval) \
	(((regval) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))


#define DISP_REG_GET(reg32) (*(volatile unsigned int*)(reg32))
#define DISP_REG_GET_FIELD(field, reg32) ((*(volatile unsigned int*)(reg32) & REG_FLD_MASK(field)) >> REG_FLD_SHIFT(field))

/* polling register until masked bit is 1 */
#define DDP_REG_POLLING(reg32, mask) \
	do { \
		while (!((DISP_REG_GET(reg32))&mask))\
			; \
	} while (0)

/* Polling register until masked bit is 0 */
#define DDP_REG_POLLING_NEG(reg32, mask) \
	do { \
		while ((DISP_REG_GET(reg32))&mask)\
			; \
	} while (0)

#define DISP_CPU_REG_SET(reg32, val) mt_reg_sync_writel(val, (volatile unsigned int*)(reg32))
#define DISP_CPU_REG_SET_FIELD(field, reg32, val)  \
    do {                                \
           mt_reg_sync_writel( (*(volatile unsigned int*)(reg32) & ~REG_FLD_MASK(field))|REG_FLD_VAL((field), (val)), reg32);  \
    } while (0)

#define DISP_REG_MASK(handle, reg32, val, mask)     \
        do { \
            mt_reg_sync_writel((unsigned int)(INREG32(reg32)&~(mask))|(val),(volatile unsigned long*)(reg32) );\
        } while (0)

#define DISP_REG_SET(handle,reg32, val) \
        do { \
            mt_reg_sync_writel(val, (volatile unsigned int*)(reg32));\
        } while (0)


#define DISP_REG_SET_FIELD(handle,field, reg32, val)  \
        do {  \
             mt_reg_sync_writel( (*(volatile unsigned int*)(reg32) & ~REG_FLD_MASK(field))|REG_FLD_VAL((field), (val)), reg32);  \
        } while (0)

#define DISP_REG_CMDQ_POLLING(handle, reg32, val, mask) \
	do { \
		while ((DISP_REG_GET(reg32) & (mask)) != ((val) & (mask)))\
			; \
	} while (0)

////////////////////////////////////////////  Register Field Access //////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// field definition
// -------------------------------------------------------------
// AAL
#define DISP_AAL_EN                             (DISPSYS_AAL_BASE + 0x000)
#define DISP_AAL_RESET                          (DISPSYS_AAL_BASE + 0x004)
#define DISP_AAL_INTEN                          (DISPSYS_AAL_BASE + 0x008)
#define DISP_AAL_INTSTA                         (DISPSYS_AAL_BASE + 0x00c)
#define DISP_AAL_STATUS                         (DISPSYS_AAL_BASE + 0x010)
#define DISP_AAL_CFG                            (DISPSYS_AAL_BASE + 0x020)
#define DISP_AAL_IN_CNT                         (DISPSYS_AAL_BASE + 0x024)
#define DISP_AAL_OUT_CNT                        (DISPSYS_AAL_BASE + 0x028)
#define DISP_AAL_CHKSUM                         (DISPSYS_AAL_BASE + 0x02c)
#define DISP_AAL_SIZE                           (DISPSYS_AAL_BASE + 0x030)
#define DISP_AAL_CABC_00                        (DISPSYS_AAL_BASE + 0x20c)
#define DISP_AAL_CABC_02                        (DISPSYS_AAL_BASE + 0x214)
#define DISP_AAL_CABC_04                        (DISPSYS_AAL_BASE + 0x21c)
#define DISP_AAL_STATUS_00                      (DISPSYS_AAL_BASE + 0x224)
/* 00 ~ 32: max histogram */
#define DISP_AAL_STATUS_32                      (DISPSYS_AAL_BASE + 0x2a4)
/* bit 8: dre_gain_force_en */
#define DISP_AAL_DRE_GAIN_FILTER_00             (DISPSYS_AAL_BASE + 0x354)
#define DISP_AAL_DRE_FLT_FORCE(idx)             (DISPSYS_AAL_BASE + 0x358 + (idx) * 4)
#define DISP_AAL_DRE_CRV_CAL_00                 (DISPSYS_AAL_BASE + 0x344)
#define DISP_AAL_DRE_MAPPING_00                 (DISPSYS_AAL_BASE + 0x3b0)
#define DISP_AAL_CABC_GAINLMT_TBL(idx)          (DISPSYS_AAL_BASE + 0x40c + (idx) * 4)


#define DISP_PWM_EN_OFF                         (0x00)
#define DISP_PWM_COMMIT_OFF                     (0x08)
#define DISP_PWM_CON_0_OFF                      (0x10)
#define DISP_PWM_CON_1_OFF                      (0x14)


// field definition
// -------------------------------------------------------------
// DISP OD
#define DISP_OD_EN                              (DISPSYS_OD_BASE + 0x000)
#define DISP_OD_RESET                           (DISPSYS_OD_BASE + 0x004)
#define DISP_OD_INTEN                           (DISPSYS_OD_BASE + 0x008)
#define DISP_OD_INTS                            (DISPSYS_OD_BASE + 0x00C)
#define DISP_OD_STATUS                          (DISPSYS_OD_BASE + 0x010)
#define DISP_OD_CFG                             (DISPSYS_OD_BASE + 0x020)
#define DISP_OD_INPUT_COUNT                     (DISPSYS_OD_BASE + 0x024)
#define DISP_OD_OUTPUT_COUNT                    (DISPSYS_OD_BASE + 0x028)
#define DISP_OD_CHKS_UM                         (DISPSYS_OD_BASE + 0x02c)
#define DISP_OD_SIZE                            (DISPSYS_OD_BASE + 0x030)
#define DISP_OD_HSYNC_WIDTH                     (DISPSYS_OD_BASE + 0x040)
#define DISP_OD_VSYNC_WIDTH                     (DISPSYS_OD_BASE + 0x044)
#define DISP_OD_MISC                            (DISPSYS_OD_BASE + 0x048)
// field definition
// -------------------------------------------------------------
// COLOR
#define CFG_MAIN_FLD_M_REG_RESET                 REG_FLD(1, 31)
#define CFG_MAIN_FLD_M_DISP_RESET                REG_FLD(1, 30)
#define CFG_MAIN_FLD_COLOR_DBUF_EN               REG_FLD(1, 29)
#define CFG_MAIN_FLD_C_PP_CM_DBG_SEL             REG_FLD(4, 16)
#define CFG_MAIN_FLD_SEQ_SEL                     REG_FLD(1, 13)
#define CFG_MAIN_FLD_ALLBP                       REG_FLD(1, 7)
#define CFG_MAIN_FLD_HEBP                        REG_FLD(1, 4)
#define CFG_MAIN_FLD_SEBP                        REG_FLD(1, 3)
#define CFG_MAIN_FLD_YEBP                        REG_FLD(1, 2)
#define CFG_MAIN_FLD_P2CBP                       REG_FLD(1, 1)
#define CFG_MAIN_FLD_C2PBP                       REG_FLD(1, 0)
#define START_FLD_DISP_COLOR_START               REG_FLD(1, 0)

#define DISP_COLOR_CFG_MAIN             (DISPSYS_COLOR0_BASE+0x400)
#define DISP_COLOR_PXL_CNT_MAIN         (DISPSYS_COLOR0_BASE+0x404)
#define DISP_COLOR_LINE_CNT_MAIN        (DISPSYS_COLOR0_BASE+0x408)
#define DISP_COLOR_WIN_X_MAIN           (DISPSYS_COLOR0_BASE+0x40C)
#define DISP_COLOR_WIN_Y_MAIN           (DISPSYS_COLOR0_BASE+0x410)
#define DISP_COLOR_TIMING_DETECTION_0   (DISPSYS_COLOR0_BASE+0x418)
#define DISP_COLOR_TIMING_DETECTION_1   (DISPSYS_COLOR0_BASE+0x41c)
#define DISP_COLOR_DBG_CFG_MAIN         (DISPSYS_COLOR0_BASE+0x420)
#define DISP_COLOR_C_BOOST_MAIN         (DISPSYS_COLOR0_BASE+0x428)
#define DISP_COLOR_C_BOOST_MAIN_2       (DISPSYS_COLOR0_BASE+0x42C)
#define DISP_COLOR_LUMA_ADJ             (DISPSYS_COLOR0_BASE+0x430)
#define DISP_COLOR_G_PIC_ADJ_MAIN_1     (DISPSYS_COLOR0_BASE+0x434)
#define DISP_COLOR_G_PIC_ADJ_MAIN_2     (DISPSYS_COLOR0_BASE+0x438)
#define DISP_COLOR_POS_MAIN             (DISPSYS_COLOR0_BASE+0x484)
#define DISP_COLOR_INK_DATA_MAIN        (DISPSYS_COLOR0_BASE+0x488)
#define DISP_COLOR_INK_DATA_MAIN_CR     (DISPSYS_COLOR0_BASE+0x48c)
#define DISP_COLOR_CAP_IN_DATA_MAIN     (DISPSYS_COLOR0_BASE+0x490)
#define DISP_COLOR_CAP_IN_DATA_MAIN_CR  (DISPSYS_COLOR0_BASE+0x494)
#define DISP_COLOR_CAP_OUT_DATA_MAIN    (DISPSYS_COLOR0_BASE+0x498)
#define DISP_COLOR_CAP_OUT_DATA_MAIN_CR (DISPSYS_COLOR0_BASE+0x49c)
#define DISP_COLOR_Y_SLOPE_1_0_MAIN     (DISPSYS_COLOR0_BASE+0x4A0)
#define DISP_COLOR_LOCAL_HUE_CD_0       (DISPSYS_COLOR0_BASE+0x620)
#define DISP_COLOR_TWO_D_WINDOW_1       (DISPSYS_COLOR0_BASE+0x740)
#define DISP_COLOR_TWO_D_W1_RESULT      (DISPSYS_COLOR0_BASE+0x74C)
#define DISP_COLOR_SAT_HIST_X_CFG_MAIN  (DISPSYS_COLOR0_BASE+0x768)
#define DISP_COLOR_SAT_HIST_Y_CFG_MAIN  (DISPSYS_COLOR0_BASE+0x76c)
#define DISP_COLOR_BWS_2                (DISPSYS_COLOR0_BASE+0x79c)
#define DISP_COLOR_CRC_0                (DISPSYS_COLOR0_BASE+0x7e0)
#define DISP_COLOR_PART_SAT_GAIN1_0     (DISPSYS_COLOR0_BASE+0x7FC)
#define DISP_COLOR_PART_SAT_GAIN1_1     (DISPSYS_COLOR0_BASE+0x800)
#define DISP_COLOR_PART_SAT_GAIN1_2     (DISPSYS_COLOR0_BASE+0x804)
#define DISP_COLOR_PART_SAT_GAIN1_3     (DISPSYS_COLOR0_BASE+0x808)
#define DISP_COLOR_PART_SAT_GAIN1_4     (DISPSYS_COLOR0_BASE+0x80C)
#define DISP_COLOR_PART_SAT_GAIN2_0     (DISPSYS_COLOR0_BASE+0x810)
#define DISP_COLOR_PART_SAT_GAIN2_1     (DISPSYS_COLOR0_BASE+0x814)
#define DISP_COLOR_PART_SAT_GAIN2_2     (DISPSYS_COLOR0_BASE+0x818)
#define DISP_COLOR_PART_SAT_GAIN2_3     (DISPSYS_COLOR0_BASE+0x81C)
#define DISP_COLOR_PART_SAT_GAIN2_4     (DISPSYS_COLOR0_BASE+0x820)
#define DISP_COLOR_PART_SAT_GAIN3_0     (DISPSYS_COLOR0_BASE+0x824)
#define DISP_COLOR_PART_SAT_GAIN3_1     (DISPSYS_COLOR0_BASE+0x828)
#define DISP_COLOR_PART_SAT_GAIN3_2     (DISPSYS_COLOR0_BASE+0x82C)
#define DISP_COLOR_PART_SAT_GAIN3_3     (DISPSYS_COLOR0_BASE+0x830)
#define DISP_COLOR_PART_SAT_GAIN3_4     (DISPSYS_COLOR0_BASE+0x834)
#define DISP_COLOR_PART_SAT_POINT1_0    (DISPSYS_COLOR0_BASE+0x838)
#define DISP_COLOR_PART_SAT_POINT1_1    (DISPSYS_COLOR0_BASE+0x83C)
#define DISP_COLOR_PART_SAT_POINT1_2    (DISPSYS_COLOR0_BASE+0x840)
#define DISP_COLOR_PART_SAT_POINT1_3    (DISPSYS_COLOR0_BASE+0x844)
#define DISP_COLOR_PART_SAT_POINT1_4    (DISPSYS_COLOR0_BASE+0x848)
#define DISP_COLOR_PART_SAT_POINT2_0    (DISPSYS_COLOR0_BASE+0x84C)
#define DISP_COLOR_PART_SAT_POINT2_1    (DISPSYS_COLOR0_BASE+0x850)
#define DISP_COLOR_PART_SAT_POINT2_2    (DISPSYS_COLOR0_BASE+0x854)
#define DISP_COLOR_PART_SAT_POINT2_3    (DISPSYS_COLOR0_BASE+0x858)
#define DISP_COLOR_PART_SAT_POINT2_4    (DISPSYS_COLOR0_BASE+0x85C)

#define DISP_COLOR_START                (DISPSYS_COLOR0_BASE+0xC00)
#define DISP_COLOR_INTEN                (DISPSYS_COLOR0_BASE+0xC04)
#define DISP_COLOR_OUT_SEL              (DISPSYS_COLOR0_BASE+0xC08)
#define DISP_COLOR_FRAME_DONE_DEL       (DISPSYS_COLOR0_BASE+0xC10)
#define DISP_COLOR_CRC                  (DISPSYS_COLOR0_BASE+0xC14)
#define DISP_COLOR_SW_SCRATCH           (DISPSYS_COLOR0_BASE+0xC18)
#define DISP_COLOR_CK_ON                (DISPSYS_COLOR0_BASE+0xC28)
#define DISP_COLOR_INTERNAL_IP_WIDTH    (DISPSYS_COLOR0_BASE+0xC50)
#define DISP_COLOR_INTERNAL_IP_HEIGHT   (DISPSYS_COLOR0_BASE+0xC54)
#define DISP_COLOR_CM1_EN               (DISPSYS_COLOR0_BASE+0xC60)
#define DISP_COLOR_CM2_EN               (DISPSYS_COLOR0_BASE+0xCA0)

// -------------------------------------------------------------
// DSI
#define DISP_REG_DSI_INSTA                              (DISPSYS_DSI0_BASE + 0x00C)

// -------------------------------------------------------------
// DPI
#define DISP_REG_DPI_EN                             (DISPSYS_DPI_BASE + 0x000)
#define DISP_REG_DPI_RST                            (DISPSYS_DPI_BASE + 0x004)
#define DISP_REG_DPI_INTEN                          (DISPSYS_DPI_BASE + 0x008)
#define DISP_REG_DPI_INSTA                          (DISPSYS_DPI_BASE + 0x00C)
#define DISP_REG_DPI_CON                            (DISPSYS_DPI_BASE + 0x010)
#define DISP_REG_DPI_OUTPUT_SETTING                 (DISPSYS_DPI_BASE + 0x014)
#define DISP_REG_DPI_SIZE                           (DISPSYS_DPI_BASE + 0x018)
#define DISP_REG_DPI_DDR_SETTING                    (DISPSYS_DPI_BASE + 0x01c)
#define DISP_REG_DPI_TGEN_HWIDTH                    (DISPSYS_DPI_BASE + 0x020)
#define DISP_REG_DPI_TGEN_HPORCH                    (DISPSYS_DPI_BASE + 0x024)
#define DISP_REG_DPI_TGEN_VWIDTH                    (DISPSYS_DPI_BASE + 0x028)
#define DISP_REG_DPI_TGEN_VPORCH                    (DISPSYS_DPI_BASE + 0x02C)
#define DISP_REG_DPI_BG_HCNTL                       (DISPSYS_DPI_BASE + 0x030)
#define DISP_REG_DPI_BG_VCNTL                       (DISPSYS_DPI_BASE + 0x034)
#define DISP_REG_DPI_BG_COLOR                       (DISPSYS_DPI_BASE + 0x038)
#define DISP_REG_DPI_FIFO_CTL                       (DISPSYS_DPI_BASE + 0x03C)
#define DISP_REG_DPI_STATUS                         (DISPSYS_DPI_BASE + 0x040)
#define DISP_REG_DPI_TMODE                          (DISPSYS_DPI_BASE + 0x044)
#define DISP_REG_DPI_CHKSUM                         (DISPSYS_DPI_BASE + 0x048)
#define DISP_REG_DPI_PATTERN                            (DISPSYS_DPI_BASE + 0x04C)
#define DISP_REG_DPI_DUMMY                          (DISPSYS_DPI_BASE + 0x050)
#define DISP_REG_DPI_TGEN_VWIDTH_LEVEN              (DISPSYS_DPI_BASE + 0x068)
#define DISP_REG_DPI_TGEN_VPORCH_LEVEN              (DISPSYS_DPI_BASE + 0x06c)
#define DISP_REG_DPI_TGEN_VWIDTH_RODD               (DISPSYS_DPI_BASE + 0x070)
#define DISP_REG_DPI_TGEN_VPORCH_RODD               (DISPSYS_DPI_BASE + 0x074)
#define DISP_REG_DPI_TGEN_VWIDTH_REVEN              (DISPSYS_DPI_BASE + 0x078)
#define DISP_REG_DPI_TGEN_VPORCH_REVEN              (DISPSYS_DPI_BASE + 0x07c)
#define DISP_REG_DPI_ESAV_VTIM_L                    (DISPSYS_DPI_BASE + 0x080)
#define DISP_REG_DPI_ESAV_VTIM_R                    (DISPSYS_DPI_BASE + 0x084)
#define DISP_REG_DPI_ESAV_VTIM_R                    (DISPSYS_DPI_BASE + 0x088)
#define DISP_REG_DPI_CLPF_SETTING                   (DISPSYS_DPI_BASE + 0x08c)
#define DISP_REG_DPI_Y_LIMIT                        (DISPSYS_DPI_BASE + 0x090)
#define DISP_REG_DPI_C_LIMIT                        (DISPSYS_DPI_BASE + 0x094)
#define DISP_REG_DPI_YUV422_SETTING                 (DISPSYS_DPI_BASE + 0x098)
#define DISP_REG_DPI_EMBSYNC_SETTING                (DISPSYS_DPI_BASE + 0x09c)
#define DISP_REG_DPI_ESAV_CODE_SET0                 (DISPSYS_DPI_BASE + 0x0a0)
#define DISP_REG_DPI_ESAV_CODE_SET1                 (DISPSYS_DPI_BASE + 0x0a4)
#define DISP_REG_DPI_ESAV_CODE_SET0                 (DISPSYS_DPI_BASE + 0x0a8)
#define DISP_REG_DPI_ESAV_CODE_SET1                 (DISPSYS_DPI_BASE + 0x0ac)
#define DISP_REG_DPI_BLANK_CODE_SET                 (DISPSYS_DPI_BASE + 0x0b0)
#define DISP_REG_DPI_MATRIX_SET                     (DISPSYS_DPI_BASE + 0x0b4)

#define EN_FLD_EN                                           REG_FLD(1, 0)
#define RST_FLD_RST                                         REG_FLD(1, 0)
#define INTEN_FLD_INT_UNDERFLOW_EN                          REG_FLD(1, 2)
#define INTEN_FLD_INT_VDE_EN                                REG_FLD(1, 1)
#define INTEN_FLD_INT_VSYNC_EN                              REG_FLD(1, 0)
#define INSTA_FLD_INTSTA_UNDERFLOW_EN                       REG_FLD(1, 2)
#define INSTA_FLD_INTSTA_VDE_EN                             REG_FLD(1, 1)
#define INSTA_FLD_INTSTA_VSYNC_EN                           REG_FLD(1, 0)
#define CON_FLD_IN_RB_SWAP                                  REG_FLD(1, 1)
#define CON_FLD_BG_ENABLE                                   REG_FLD(1, 0)
#define OUTPUT_SETTING_FLD_EDGE_SEL                         REG_FLD(1, 17)
#define OUTPUT_SETTING_FLD_OEN_OFF                          REG_FLD(1, 16)
#define OUTPUT_SETTING_FLD_CK_POL                           REG_FLD(1, 15)
#define OUTPUT_SETTING_FLD_VSYNC_POL                        REG_FLD(1, 14)
#define OUTPUT_SETTING_FLD_HSYNC_POL                        REG_FLD(1, 13)
#define OUTPUT_SETTING_FLD_DE_POL                           REG_FLD(1, 12)
#define OUTPUT_SETTING_FLD_VS_MASK                          REG_FLD(1, 10)
#define OUTPUT_SETTING_FLD_HS_MASK                          REG_FLD(1, 9)
#define OUTPUT_SETTING_FLD_DE_MASK                          REG_FLD(1, 8)
#define OUTPUT_SETTING_FLD_R_MASK                           REG_FLD(1, 6)
#define OUTPUT_SETTING_FLD_G_MASK                           REG_FLD(1, 5)
#define OUTPUT_SETTING_FLD_B_MASK                           REG_FLD(1, 4)
#define OUTPUT_SETTING_FLD_BIT_SWAP                         REG_FLD(1, 3)
#define OUTPUT_SETTING_FLD_CH_SWAP                          REG_FLD(3, 0)
#define DPI_SIZE_FLD_HSIZE                                  REG_FLD(11, 16)
#define DPI_SIZE_FLD_VSIZE                                  REG_FLD(11, 0)
#define TGEN_HWIDTH_FLD_HPW                                 REG_FLD(12, 0)
#define TGEN_HPORCH_FLD_HFP                                 REG_FLD(12, 16)
#define TGEN_HPORCH_FLD_HBP                                 REG_FLD(12, 0)
#define TGEN_VWIDTH_FLD_VPW                                 REG_FLD(8, 0)
#define TGEN_VPORCH_FLD_VFP                                 REG_FLD(8, 16)
#define TGEN_VPORCH_FLD_VBP                                 REG_FLD(8, 0)
#define BG_HCNTL_FLD_BG_LEFT                                REG_FLD(11, 16)
#define BG_HCNTL_FLD_BG_RIGHT                               REG_FLD(11, 0)
#define BG_VCNTL_FLD_BG_TOP                                 REG_FLD(11, 16)
#define BG_VCNTL_FLD_BG_BOT                                 REG_FLD(11, 0)
#define BG_COLOR_FLD_BG_R                                   REG_FLD(8, 16)
#define BG_COLOR_FLD_BG_G                                   REG_FLD(8, 8)
#define BG_COLOR_FLD_BG_B                                   REG_FLD(8, 0)
#define FIFO_CTL_FLD_FIFO_RST_SEL                           REG_FLD(1, 8)
#define FIFO_CTL_FLD_FIFO_VALID_SET                         REG_FLD(5, 0)
#define STATUS_FLD_OUTEN                                    REG_FLD(1, 17)
#define STATUS_FLD_DPI_BUSY                                 REG_FLD(1, 16)
#define STATUS_FLD_V_COUNTER                                REG_FLD(13, 0)
#define TMODE_FLD_DPI_OEN_ON                                REG_FLD(1, 0)
#define CHKSUM_FLD_DPI_CHKSUM_EN                            REG_FLD(1, 31)
#define CHKSUM_FLD_DPI_CHKSUM_READY                         REG_FLD(1, 30)
#define CHKSUM_FLD_DPI_CHKSUM                               REG_FLD(24, 0)
#define PATTERN_FLD_PAT_R_MAN                               REG_FLD(8, 24)
#define PATTERN_FLD_PAT_G_MAN                               REG_FLD(8, 16)
#define PATTERN_FLD_PAT_B_MAN                               REG_FLD(8, 8)
#define PATTERN_FLD_PAT_SEL                                 REG_FLD(3, 4)
#define PATTERN_FLD_PAT_EN                                  REG_FLD(1, 0)
// field definition
// -------------------------------------------------------------
// CCORR
#define DISP_REG_CCORR_EN                                    (DISPSYS_CCORR_BASE + 0x000)
#define DISP_REG_CCORR_RESET                                 (DISPSYS_CCORR_BASE + 0x004)
#define DISP_REG_CCORR_INTEN                                 (DISPSYS_CCORR_BASE + 0x008)
#define DISP_REG_CCORR_INTSTA                                (DISPSYS_CCORR_BASE + 0x00c)
#define DISP_REG_CCORR_STATUS                                (DISPSYS_CCORR_BASE + 0x010)
#define DISP_REG_CCORR_CFG                                   (DISPSYS_CCORR_BASE + 0x020)
#define DISP_REG_CCORR_IN_CNT                                (DISPSYS_CCORR_BASE + 0x024)
#define DISP_REG_CCORR_OUT_CNT                               (DISPSYS_CCORR_BASE + 0x028)
#define DISP_REG_CCORR_CHKSUM                                (DISPSYS_CCORR_BASE + 0x02c)
#define DISP_REG_CCORR_SIZE                                  (DISPSYS_CCORR_BASE + 0x030)
#define DISP_REG_CCORR_COEF_0                                (DISPSYS_CCORR_BASE + 0x080)
#define DISP_REG_CCORR_DUMMY_REG                             (DISPSYS_CCORR_BASE + 0x0c0)

#define CCORR_SIZE_FLD_HSIZE                                  REG_FLD(13, 16)
#define CCORR_SIZE_FLD_VSIZE                                  REG_FLD(13, 0)
#define CCORR_CFG_FLD_CHKSUM_SEL                              REG_FLD(3, 29)
#define CCORR_CFG_FLD_CHKSUM_EN                               REG_FLD(1, 28)
#define CCORR_CFG_FLD_CCORR_GAMMA_OFF                         REG_FLD(1, 2)
#define CCORR_CFG_FLD_CCORR_ENGINE_EN                         REG_FLD(1, 1)

// -------------------------------------------------------------
// GAMMA
#define DISP_REG_GAMMA_EN                               (DISPSYS_GAMMA_BASE + 0x000)
#define DISP_REG_GAMMA_RESET                            (DISPSYS_GAMMA_BASE + 0x004)
#define DISP_REG_GAMMA_INTEN                            (DISPSYS_GAMMA_BASE + 0x008)
#define DISP_REG_GAMMA_INTSTA                           (DISPSYS_GAMMA_BASE + 0x00c)
#define DISP_REG_GAMMA_STATUS                           (DISPSYS_GAMMA_BASE + 0x010)
#define DISP_REG_GAMMA_CFG                              (DISPSYS_GAMMA_BASE + 0x020)
#define DISP_REG_GAMMA_INPUT_COUNT                      (DISPSYS_GAMMA_BASE + 0x024)
#define DISP_REG_GAMMA_OUTPUT_COUNT                     (DISPSYS_GAMMA_BASE + 0x028)
#define DISP_REG_GAMMA_CHKSUM                           (DISPSYS_GAMMA_BASE + 0x02c)
#define DISP_REG_GAMMA_SIZE                             (DISPSYS_GAMMA_BASE + 0x030)
#define DISP_REG_GAMMA_DUMMY_REG                        (DISPSYS_GAMMA_BASE + 0x0c0)
#define DISP_REG_GAMMA_LUT                              (DISPSYS_GAMMA_BASE + 0x700)

#define EN_FLD_GAMMA_EN                         REG_FLD(1, 0)
#define RESET_FLD_GAMMA_RESET                   REG_FLD(1, 0)
#define INTEN_FLD_OF_END_INT_EN                 REG_FLD(1, 1)
#define INTEN_FLD_IF_END_INT_EN                 REG_FLD(1, 0)
#define INTSTA_FLD_OF_END_INT                   REG_FLD(1, 1)
#define INTSTA_FLD_IF_END_INT                   REG_FLD(1, 0)
#define STATUS_FLD_IN_VALID                     REG_FLD(1, 7)
#define STATUS_FLD_IN_READY                     REG_FLD(1, 6)
#define STATUS_FLD_OUT_VALID                    REG_FLD(1, 5)
#define STATUS_FLD_OUT_READY                    REG_FLD(1, 4)
#define STATUS_FLD_OF_UNFINISH                  REG_FLD(1, 1)
#define STATUS_FLD_IF_UNFINISH                  REG_FLD(1, 0)
#define CFG_FLD_CHKSUM_SEL                      REG_FLD(2, 29)
#define CFG_FLD_CHKSUM_EN                       REG_FLD(1, 28)
#define CFG_FLD_CCORR_GAMMA_OFF                 REG_FLD(1, 5)
#define CFG_FLD_CCORR_EN                        REG_FLD(1, 4)
#define CFG_FLD_DITHER_EN                       REG_FLD(1, 2)
#define CFG_FLD_GAMMA_LUT_EN                    REG_FLD(1, 1)
#define CFG_FLD_RELAY_MODE                      REG_FLD(1, 0)
#define INPUT_COUNT_FLD_INP_LINE_CNT            REG_FLD(13, 16)
#define INPUT_COUNT_FLD_INP_PIX_CNT             REG_FLD(13, 0)
#define OUTPUT_COUNT_FLD_OUTP_LINE_CNT          REG_FLD(13, 16)
#define OUTPUT_COUNT_FLD_OUTP_PIX_CNT           REG_FLD(13, 0)
#define CHKSUM_FLD_CHKSUM                       REG_FLD(30, 0)
#define SIZE_FLD_HSIZE                          REG_FLD(13, 16)
#define SIZE_FLD_VSIZE                          REG_FLD(13, 0)
#define CCORR_0_FLD_CCORR_C00                   REG_FLD(12, 16)
#define CCORR_0_FLD_CCORR_C01                   REG_FLD(12, 0)
#define CCORR_1_FLD_CCORR_C02                   REG_FLD(12, 16)
#define CCORR_1_FLD_CCORR_C10                   REG_FLD(12, 0)
#define CCORR_2_FLD_CCORR_C11                   REG_FLD(12, 16)
#define CCORR_2_FLD_CCORR_C12                   REG_FLD(12, 0)
#define CCORR_3_FLD_CCORR_C20                   REG_FLD(12, 16)
#define CCORR_3_FLD_CCORR_C21                   REG_FLD(12, 0)
#define CCORR_4_FLD_CCORR_C22                   REG_FLD(12, 16)
#define DUMMY_REG_FLD_DUMMY_REG                 REG_FLD(32, 0)
#define DITHER_0_FLD_CRC_CLR                    REG_FLD(1, 24)
#define DITHER_0_FLD_CRC_START                  REG_FLD(1, 20)
#define DITHER_0_FLD_CRC_CEN                    REG_FLD(1, 16)
#define DITHER_0_FLD_FRAME_DONE_DEL             REG_FLD(8, 8)
#define DITHER_0_FLD_OUT_SEL                    REG_FLD(1, 4)
#define DITHER_5_FLD_W_DEMO                     REG_FLD(16, 0)
#define DITHER_6_FLD_WRAP_MODE                  REG_FLD(1, 16)
#define DITHER_6_FLD_LEFT_EN                    REG_FLD(2, 14)
#define DITHER_6_FLD_FPHASE_R                   REG_FLD(1, 13)
#define DITHER_6_FLD_FPHASE_EN                  REG_FLD(1, 12)
#define DITHER_6_FLD_FPHASE                     REG_FLD(6, 4)
#define DITHER_6_FLD_ROUND_EN                   REG_FLD(1, 3)
#define DITHER_6_FLD_RDITHER_EN                 REG_FLD(1, 2)
#define DITHER_6_FLD_LFSR_EN                    REG_FLD(1, 1)
#define DITHER_6_FLD_EDITHER_EN                 REG_FLD(1, 0)
#define DITHER_7_FLD_DRMOD_B                    REG_FLD(2, 8)
#define DITHER_7_FLD_DRMOD_G                    REG_FLD(2, 4)
#define DITHER_7_FLD_DRMOD_R                    REG_FLD(2, 0)
#define GAMMA_DITHER_8_FLD_INK_DATA_R           REG_FLD(12, 16)
#define DITHER_8_FLD_INK                        REG_FLD(1, 0)
#define GAMMA_DITHER_9_FLD_INK_DATA_B           REG_FLD(12, 16)
#define GAMMA_DITHER_9_FLD_INK_DATA_G           REG_FLD(12, 0)
#define DITHER_10_FLD_FPHASE_BIT                REG_FLD(3, 8)
#define DITHER_10_FLD_FPHASE_SEL                REG_FLD(2, 4)
#define DITHER_10_FLD_FPHASE_CTRL               REG_FLD(2, 0)
#define DITHER_11_FLD_SUB_B                     REG_FLD(2, 12)
#define DITHER_11_FLD_SUB_G                     REG_FLD(2, 8)
#define DITHER_11_FLD_SUB_R                     REG_FLD(2, 4)
#define DITHER_11_FLD_SUBPIX_EN                 REG_FLD(1, 0)
#define DITHER_12_FLD_H_ACTIVE                  REG_FLD(16, 16)
#define DITHER_12_FLD_TABLE_EN                  REG_FLD(2, 4)
#define DITHER_12_FLD_LSB_OFF                   REG_FLD(1, 0)
#define DITHER_13_FLD_RSHIFT_B                  REG_FLD(3, 8)
#define DITHER_13_FLD_RSHIFT_G                  REG_FLD(3, 4)
#define DITHER_13_FLD_RSHIFT_R                  REG_FLD(3, 0)
#define DITHER_14_FLD_DEBUG_MODE                REG_FLD(2, 8)
#define DITHER_14_FLD_DIFF_SHIFT                REG_FLD(3, 4)
#define DITHER_14_FLD_TESTPIN_EN                REG_FLD(1, 0)
#define DITHER_15_FLD_LSB_ERR_SHIFT_R           REG_FLD(3, 28)
#define DITHER_15_FLD_LSB_OVFLW_BIT_R           REG_FLD(3, 24)
#define DITHER_15_FLD_LSB_ADD_LSHIFT_R          REG_FLD(3, 20)
#define DITHER_15_FLD_LSB_INPUT_RSHIFT_R        REG_FLD(3, 16)
#define DITHER_15_FLD_LSB_NEW_BIT_MODE          REG_FLD(1, 0)
#define DITHER_16_FLD_LSB_ERR_SHIFT_B           REG_FLD(3, 28)
#define DITHER_16_FLD_OVFLW_BIT_B               REG_FLD(3, 24)
#define DITHER_16_FLD_ADD_LSHIFT_B              REG_FLD(3, 20)
#define DITHER_16_FLD_INPUT_RSHIFT_B            REG_FLD(3, 16)
#define DITHER_16_FLD_LSB_ERR_SHIFT_G           REG_FLD(3, 12)
#define DITHER_16_FLD_OVFLW_BIT_G               REG_FLD(3, 8)
#define DITHER_16_FLD_ADD_LSHIFT_G              REG_FLD(3, 4)
#define DITHER_16_FLD_INPUT_RSHIFT_G            REG_FLD(3, 0)
#define DITHER_17_FLD_CRC_RDY                   REG_FLD(1, 16)
#define DITHER_17_FLD_CRC_OUT                   REG_FLD(16, 0)
#define LUT_FLD_GAMMA_LUT_R                     REG_FLD(10, 20)
#define LUT_FLD_GAMMA_LUT_G                     REG_FLD(10, 10)
#define LUT_FLD_GAMMA_LUT_B                     REG_FLD(10, 0)

// -------------------------------------------------------------
// Dither
#define DISP_REG_DITHER_EN                                        (DISPSYS_DITHER_BASE + 0x000)
#define DISP_REG_DITHER_RESET                                     (DISPSYS_DITHER_BASE + 0x004)
#define DISP_REG_DITHER_INTEN                                     (DISPSYS_DITHER_BASE + 0x008)
#define DISP_REG_DITHER_INTSTA                                    (DISPSYS_DITHER_BASE + 0x00c)
#define DISP_REG_DITHER_STATUS                                    (DISPSYS_DITHER_BASE + 0x010)
#define DISP_REG_DITHER_CFG                                       (DISPSYS_DITHER_BASE + 0x020)
#define DISP_REG_DITHER_IN_CNT                                    (DISPSYS_DITHER_BASE + 0x024)
#define DISP_REG_DITHER_OUT_CNT                                   (DISPSYS_DITHER_BASE + 0x028)
#define DISP_REG_DITHER_CHKSUM                                    (DISPSYS_DITHER_BASE + 0x02c)
#define DISP_REG_DITHER_SIZE                                      (DISPSYS_DITHER_BASE + 0x030)
#define DISP_REG_DITHER_DUMMY_REG                                 (DISPSYS_DITHER_BASE + 0x0c0)
#define DISP_REG_DITHER_0                                         (DISPSYS_DITHER_BASE + 0x100)
#define DISP_REG_DITHER_5                                         (DISPSYS_DITHER_BASE + 0x114)
#define DISP_REG_DITHER_6                                         (DISPSYS_DITHER_BASE + 0x118)
#define DISP_REG_DITHER_7                                         (DISPSYS_DITHER_BASE + 0x11c)
#define DISP_REG_DITHER_8                                         (DISPSYS_DITHER_BASE + 0x120)
#define DISP_REG_DITHER_9                                         (DISPSYS_DITHER_BASE + 0x124)
#define DISP_REG_DITHER_10                                        (DISPSYS_DITHER_BASE + 0x128)
#define DISP_REG_DITHER_11                                        (DISPSYS_DITHER_BASE + 0x12c)
#define DISP_REG_DITHER_12                                        (DISPSYS_DITHER_BASE + 0x130)
#define DISP_REG_DITHER_13                                        (DISPSYS_DITHER_BASE + 0x134)
#define DISP_REG_DITHER_14                                        (DISPSYS_DITHER_BASE + 0x138)
#define DISP_REG_DITHER_15                                        (DISPSYS_DITHER_BASE + 0x13c)
#define DISP_REG_DITHER_16                                        (DISPSYS_DITHER_BASE + 0x140)
#define DISP_REG_DITHER_17                                        (DISPSYS_DITHER_BASE + 0x144)

#define DITHER_CFG_FLD_CHKSUM_SEL                           REG_FLD(2, 29)
#define DITHER_CFG_FLD_CHKSUM_EN                            REG_FLD(1, 28)
#define DITHER_CFG_FLD_DITHER_ENGINE_EN                     REG_FLD(1,1)
#define DITHER_CFG_FLD_RELAY_MODE                           REG_FLD(1,0)
#define DITHER_SIZE_FLD_HSIZE                               REG_FLD(13, 16)
#define DITHER_SIZE_FLD_VSIZE                               REG_FLD(13, 0)

// -------------------------------------------------------------
// MUTEX
#define DISP_OVL_SEPARATE_MUTEX_ID (DISP_MUTEX_DDP_LAST+1)	/* other disp will not see mutex 4 */
#define DISP_REG_CONFIG_MUTEX_INTEN					(DISPSYS_MUTEX_BASE + 0x000)
#define DISP_REG_CONFIG_MUTEX_INTSTA				(DISPSYS_MUTEX_BASE + 0x004)
#define DISP_REG_CONFIG_MUTEX_CFG					(DISPSYS_MUTEX_BASE + 0x008)
	#define CFG_FLD_HW_CG								REG_FLD(1, 0)

#define DISP_REG_CONFIG_MUTEX_REG_UPD_TIMEOUT		(DISPSYS_MUTEX_BASE + 0x00C)
	#define UPD_FLD_UPD_TIMEOUT							REG_FLD(8, 0)

#define DISP_REG_CONFIG_MUTEX_REG_COMMIT0			(DISPSYS_MUTEX_BASE + 0x010)
#define DISP_REG_CONFIG_MUTEX_REG_COMMIT1			(DISPSYS_MUTEX_BASE + 0x014)
#define DISP_REG_CONFIG_MUTEX_INTEN_1				(DISPSYS_MUTEX_BASE + 0x018)
#define DISP_REG_CONFIG_MUTEX_INTSTA_1				(DISPSYS_MUTEX_BASE + 0x01C)

/* mutex0 */
#define DISP_REG_CONFIG_MUTEX0_EN					(DISPSYS_MUTEX_BASE + 0x020)
	#define EN_FLD_MUTEX0_EN							REG_FLD(1, 0)

#define DISP_REG_CONFIG_MUTEX0_GET					(DISPSYS_MUTEX_BASE + 0x024)
	#define GET_FLD_MUTEX0_GET							REG_FLD(1, 0)
	#define GET_FLD_INT_MUTEX0_EN						REG_FLD(1, 1)

#define DISP_REG_CONFIG_MUTEX0_RST					(DISPSYS_MUTEX_BASE + 0x028)
	#define RST_FLD_MUTEX0_RST							REG_FLD(1, 0)

#define DISP_REG_CONFIG_MUTEX0_SOF					(DISPSYS_MUTEX_BASE + 0x02C)
	#define SOF_FLD_MUTEX0_SOF							REG_FLD(3, 0)
	#define SOF_FLD_MUTEX0_SOF_TIMING					REG_FLD(2, 3)
	#define SOF_FLD_MUTEX0_SOF_WAIT						REG_FLD(1, 5)
	#define SOF_FLD_MUTEX0_EOF							REG_FLD(3, 6)
	#define SOF_FLD_MUTEX0_FOF_TIMING					REG_FLD(2, 9)
	#define SOF_FLD_MUTEX0_EOF_WAIT						REG_FLD(1, 11)

#define SOF_VAL_MUTEX0_SOF_SINGLE_MODE					(0)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI0					(1)
#define SOF_VAL_MUTEX0_SOF_FROM_DSI1					(2)
#define SOF_VAL_MUTEX0_SOF_FROM_DPI						(3)
#define SOF_VAL_MUTEX0_EOF_SINGLE_MODE					(0)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI0					(1)
#define SOF_VAL_MUTEX0_EOF_FROM_DSI1					(2)
#define SOF_VAL_MUTEX0_EOF_FROM_DPI						(3)

#define DISP_REG_CONFIG_MUTEX0_MOD0					(DISPSYS_MUTEX_BASE + 0x030)
#define DISP_REG_CONFIG_MUTEX0_MOD1					(DISPSYS_MUTEX_BASE + 0x034)

#define DISP_REG_CONFIG_MUTEX1_EN					(DISPSYS_MUTEX_BASE + 0x040)
#define DISP_REG_CONFIG_MUTEX1_GET					(DISPSYS_MUTEX_BASE + 0x044)
#define DISP_REG_CONFIG_MUTEX1_RST					(DISPSYS_MUTEX_BASE + 0x048)
#define DISP_REG_CONFIG_MUTEX1_SOF					(DISPSYS_MUTEX_BASE + 0x04C)
#define DISP_REG_CONFIG_MUTEX1_MOD0					(DISPSYS_MUTEX_BASE + 0x050)
#define DISP_REG_CONFIG_MUTEX1_MOD1					(DISPSYS_MUTEX_BASE + 0x054)
#define DISP_REG_CONFIG_MUTEX2_EN					(DISPSYS_MUTEX_BASE + 0x060)
#define DISP_REG_CONFIG_MUTEX2_GET					(DISPSYS_MUTEX_BASE + 0x064)
#define DISP_REG_CONFIG_MUTEX2_RST					(DISPSYS_MUTEX_BASE + 0x068)
#define DISP_REG_CONFIG_MUTEX2_SOF					(DISPSYS_MUTEX_BASE + 0x06C)
#define DISP_REG_CONFIG_MUTEX2_MOD0					(DISPSYS_MUTEX_BASE + 0x070)
#define DISP_REG_CONFIG_MUTEX2_MOD1					(DISPSYS_MUTEX_BASE + 0x074)
#define DISP_REG_CONFIG_MUTEX3_EN					(DISPSYS_MUTEX_BASE + 0x080)
#define DISP_REG_CONFIG_MUTEX3_GET					(DISPSYS_MUTEX_BASE + 0x084)
#define DISP_REG_CONFIG_MUTEX3_RST					(DISPSYS_MUTEX_BASE + 0x088)
#define DISP_REG_CONFIG_MUTEX3_SOF					(DISPSYS_MUTEX_BASE + 0x08C)
#define DISP_REG_CONFIG_MUTEX3_MOD0					(DISPSYS_MUTEX_BASE + 0x090)
#define DISP_REG_CONFIG_MUTEX3_MOD1					(DISPSYS_MUTEX_BASE + 0x094)
#define DISP_REG_CONFIG_MUTEX4_EN					(DISPSYS_MUTEX_BASE + 0x0A0)
#define DISP_REG_CONFIG_MUTEX4_GET					(DISPSYS_MUTEX_BASE + 0x0A4)
#define DISP_REG_CONFIG_MUTEX4_RST					(DISPSYS_MUTEX_BASE + 0x0A8)
#define DISP_REG_CONFIG_MUTEX4_SOF					(DISPSYS_MUTEX_BASE + 0x0AC)
#define DISP_REG_CONFIG_MUTEX4_MOD0					(DISPSYS_MUTEX_BASE + 0x0B0)
#define DISP_REG_CONFIG_MUTEX4_MOD1					(DISPSYS_MUTEX_BASE + 0x0B4)
#define DISP_REG_CONFIG_MUTEX5_EN					(DISPSYS_MUTEX_BASE + 0x0C0)
#define DISP_REG_CONFIG_MUTEX5_GET					(DISPSYS_MUTEX_BASE + 0x0C4)
#define DISP_REG_CONFIG_MUTEX5_RST					(DISPSYS_MUTEX_BASE + 0x0C8)
#define DISP_REG_CONFIG_MUTEX5_SOF					(DISPSYS_MUTEX_BASE + 0x0CC)
#define DISP_REG_CONFIG_MUTEX5_MOD0					(DISPSYS_MUTEX_BASE + 0x0D0)
#define DISP_REG_CONFIG_MUTEX5_MOD1					(DISPSYS_MUTEX_BASE + 0x0D4)

#define DISP_REG_CONFIG_MUTEX_DUMMY0				(DISPSYS_MUTEX_BASE + 0x300)
#define DISP_REG_CONFIG_MUTEX_DUMMY1				(DISPSYS_MUTEX_BASE + 0x304)
#define DISP_REG_CONFIG_DEBUG_OUT_SEL				(DISPSYS_MUTEX_BASE + 0x30C)
	#define DEBUG_OUT_SEL_FLD_DEBUG_OUT_SEL					REG_FLD(2, 0)

#define DISP_REG_CONFIG_MUTEX_EN(n)					(DISP_REG_CONFIG_MUTEX0_EN + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_GET(n)					(DISP_REG_CONFIG_MUTEX0_GET + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_RST(n)					(DISP_REG_CONFIG_MUTEX0_RST + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD0(n)				(DISP_REG_CONFIG_MUTEX0_MOD0 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_MOD1(n)				(DISP_REG_CONFIG_MUTEX0_MOD1 + (0x20 * (n)))
#define DISP_REG_CONFIG_MUTEX_SOF(n)					(DISP_REG_CONFIG_MUTEX0_SOF + (0x20 * (n)))

// -------------------------------------------------------------
// OD
#define DISP_REG_OD_EN           (DISPSYS_OD_BASE+0x000)
#define DISP_REG_OD_RESET        (DISPSYS_OD_BASE+0x004)
#define DISP_REG_OD_INTEN        (DISPSYS_OD_BASE+0x008)
#define DISP_REG_OD_INTSTA       (DISPSYS_OD_BASE+0x00C)
#define DISP_REG_OD_STATUS       (DISPSYS_OD_BASE+0x010)
#define DISP_REG_OD_CFG          (DISPSYS_OD_BASE+0x020)
#define DISP_REG_OD_INPUT_COUNT  (DISPSYS_OD_BASE+0x024)
#define DISP_REG_OD_OUTPUT_COUNT (DISPSYS_OD_BASE+0x028)
#define DISP_REG_OD_CHKSUM       (DISPSYS_OD_BASE+0x02C)
#define DISP_REG_OD_SIZE         (DISPSYS_OD_BASE+0x030)
#define DISP_REG_OD_HSYNC_WIDTH  (DISPSYS_OD_BASE+0x040)
#define DISP_REG_OD_VSYNC_WIDTH  (DISPSYS_OD_BASE+0x044)
#define DISP_REG_OD_MISC         (DISPSYS_OD_BASE+0x048)
#define DISP_REG_OD_DUMMY_REG    (DISPSYS_OD_BASE+0x0C0)
#define DISP_REG_OD_DITHER_0       (DISPSYS_OD_BASE+0x100)
#define DISP_REG_OD_DITHER_5     (DISPSYS_OD_BASE+0x114)
#define DISP_REG_OD_DITHER_6     (DISPSYS_OD_BASE+0x118)
#define DISP_REG_OD_DITHER_7       (DISPSYS_OD_BASE+0x11C)
#define DISP_REG_OD_DITHER_8       (DISPSYS_OD_BASE+0x120)
#define DISP_REG_OD_DITHER_9       (DISPSYS_OD_BASE+0x124)
#define DISP_REG_OD_DITHER_10      (DISPSYS_OD_BASE+0x128)
#define DISP_REG_OD_DITHER_11      (DISPSYS_OD_BASE+0x12C)
#define DISP_REG_OD_DITHER_12      (DISPSYS_OD_BASE+0x130)
#define DISP_REG_OD_DITHER_13      (DISPSYS_OD_BASE+0x134)
#define DISP_REG_OD_DITHER_14      (DISPSYS_OD_BASE+0x138)
#define DISP_REG_OD_DITHER_15      (DISPSYS_OD_BASE+0x13C)
#define DISP_REG_OD_DITHER_16      (DISPSYS_OD_BASE+0x140)
#define DISP_REG_OD_DITHER_17    (DISPSYS_OD_BASE+0x144)

/* ------------------------------------------------------------- */
/* RDMA */
#define DISP_REG_RDMA_INT_ENABLE				(DISPSYS_RDMA0_BASE+0x000)
#define DISP_REG_RDMA_INT_STATUS				(DISPSYS_RDMA0_BASE+0x004)
#define DISP_REG_RDMA_GLOBAL_CON				(DISPSYS_RDMA0_BASE+0x010)
#define DISP_REG_RDMA_SIZE_CON_0                (DISPSYS_RDMA0_BASE+0x014)
#define DISP_REG_RDMA_SIZE_CON_1                (DISPSYS_RDMA0_BASE+0x018)
#define DISP_REG_RDMA_TARGET_LINE             	(DISPSYS_RDMA0_BASE+0x01C)
#define DISP_REG_RDMA_MEM_CON                   (DISPSYS_RDMA0_BASE+0x024)
#define DISP_REG_RDMA_MEM_SRC_PITCH				(DISPSYS_RDMA0_BASE+0x02C)
#define DISP_REG_RDMA_MEM_GMC_SETTING_0			(DISPSYS_RDMA0_BASE+0x030)
#define DISP_REG_RDMA_MEM_GMC_SETTING_1			(DISPSYS_RDMA0_BASE+0x034)
#define DISP_REG_RDMA_MEM_SLOW_CON				(DISPSYS_RDMA0_BASE+0x038)
#define DISP_REG_RDMA_MEM_GMC_SETTING_2			(DISPSYS_RDMA0_BASE+0x03c)
#define DISP_REG_RDMA_FIFO_CON                  (DISPSYS_RDMA0_BASE+0x040)
#define DISP_REG_RDMA_FIFO_LOG                  (DISPSYS_RDMA0_BASE+0x044)
#define DISP_REG_RDMA_C00                       (DISPSYS_RDMA0_BASE+0x054)
#define DISP_REG_RDMA_C01                       (DISPSYS_RDMA0_BASE+0x058)
#define DISP_REG_RDMA_C02                       (DISPSYS_RDMA0_BASE+0x05C)
#define DISP_REG_RDMA_C10                       (DISPSYS_RDMA0_BASE+0x060)
#define DISP_REG_RDMA_C11                       (DISPSYS_RDMA0_BASE+0x064)
#define DISP_REG_RDMA_C12                       (DISPSYS_RDMA0_BASE+0x068)
#define DISP_REG_RDMA_C20                       (DISPSYS_RDMA0_BASE+0x06C)
#define DISP_REG_RDMA_C21                       (DISPSYS_RDMA0_BASE+0x070)
#define DISP_REG_RDMA_C22                       (DISPSYS_RDMA0_BASE+0x074)
#define DISP_REG_RDMA_PRE_ADD_0					(DISPSYS_RDMA0_BASE+0x078)
#define DISP_REG_RDMA_PRE_ADD_1                 (DISPSYS_RDMA0_BASE+0x07C)
#define DISP_REG_RDMA_PRE_ADD_2                 (DISPSYS_RDMA0_BASE+0x080)
#define DISP_REG_RDMA_POST_ADD_0				(DISPSYS_RDMA0_BASE+0x084)
#define DISP_REG_RDMA_POST_ADD_1                (DISPSYS_RDMA0_BASE+0x088)
#define DISP_REG_RDMA_POST_ADD_2                (DISPSYS_RDMA0_BASE+0x08C)
#define DISP_REG_RDMA_DUMMY                     (DISPSYS_RDMA0_BASE+0x090)
#define DISP_REG_RDMA_DEBUG_OUT_SEL             (DISPSYS_RDMA0_BASE+0x094)
#define DISP_REG_RDMA_MEM_START_ADDR			(DISPSYS_RDMA0_BASE+0xf00)
#define DISP_REG_RDMA_BG_CON_0                  (DISPSYS_RDMA0_BASE+0x0a0)
#define DISP_REG_RDMA_BG_CON_1                  (DISPSYS_RDMA0_BASE+0x0a4)
#define DISP_REG_RDMA_THRESHOLD_FOR_SODI        (DISPSYS_RDMA0_BASE+0x0a8)
#define DISP_REG_RDMA_THRESHOLD_FOR_DVFS		(DISPSYS_RDMA0_BASE+0x0ac)
#define DISP_REG_RDMA_SRAM_SEL					(DISPSYS_RDMA0_BASE+0x0b0)
#define DISP_REG_RDMA_STALL_CG_CON				(DISPSYS_RDMA0_BASE+0x0b4)
#define DISP_REG_RDMA_SHADOW_UPDATE				(DISPSYS_RDMA0_BASE+0x0b8)
#define DISP_REG_RDMA_DRAM_CON					(DISPSYS_RDMA0_BASE+0x0c0)
#define DISP_REG_RDMA_DVFS_SETTING_PRE			(DISPSYS_RDMA0_BASE+0x0d0)
#define DISP_REG_RDMA_DVFS_SETTING_ULTRA		(DISPSYS_RDMA0_BASE+0x0d4)

#define DISP_REG_RDMA_IN_P_CNT                  (DISPSYS_RDMA0_BASE+0x0f0)
#define DISP_REG_RDMA_IN_LINE_CNT               (DISPSYS_RDMA0_BASE+0x0f4)
#define DISP_REG_RDMA_OUT_P_CNT                 (DISPSYS_RDMA0_BASE+0x0f8)
#define DISP_REG_RDMA_OUT_LINE_CNT              (DISPSYS_RDMA0_BASE+0x0fc)

#define DISP_REG_RDMA_DBG_OUT					(DISPSYS_RDMA0_BASE+0x100)
#define DISP_REG_RDMA_DBG_OUT1					(DISPSYS_RDMA0_BASE+0x10c)

#define INT_ENABLE_FLD_TARGET_LINE_INT_EN		    REG_FLD(1, 5)
#define INT_ENABLE_FLD_FIFO_UNDERFLOW_INT_EN		        REG_FLD(1, 4)
#define INT_ENABLE_FLD_EOF_ABNORMAL_INT_EN		    REG_FLD(1, 3)
#define INT_ENABLE_FLD_FRAME_END_INT_EN				    REG_FLD(1, 2)
#define INT_ENABLE_FLD_FRAME_START_INT_EN			    REG_FLD(1, 1)
#define INT_ENABLE_FLD_REG_UPDATE_INT_EN			    REG_FLD(1, 0)
#define INT_STATUS_FLD_FIFO_EMPTY_INT_FLAG		        REG_FLD(1, 6)
#define INT_STATUS_FLD_TARGET_LINE_INT_FLAG		        REG_FLD(1, 5)
#define INT_STATUS_FLD_FIFO_UNDERFLOW_INT_FLAG		            REG_FLD(1, 4)
#define INT_STATUS_FLD_EOF_ABNORMAL_INT_FLAG		        REG_FLD(1, 3)
#define INT_STATUS_FLD_FRAME_END_INT_FLAG			    REG_FLD(1, 2)
#define INT_STATUS_FLD_FRAME_START_INT_FLAG		        REG_FLD(1, 1)
#define INT_STATUS_FLD_REG_UPDATE_INT_FLAG		    REG_FLD(1, 0)
#define GLOBAL_CON_FLD_SMI_BUSY					REG_FLD(1, 12)
#define GLOBAL_CON_FLD_RESET_STATE				REG_FLD(3, 8)
#define GLOBAL_CON_FLD_SOFT_RESET				REG_FLD(1, 4)
#define GLOBAL_CON_FLD_MODE_SEL					REG_FLD(1, 1)
#define GLOBAL_CON_FLD_ENGINE_EN				REG_FLD(1, 0)
#define SIZE_CON_0_FLD_MATRIX_INT_MTX_SEL			    REG_FLD(4, 20)
#define SIZE_CON_0_FLD_MATRIX_WIDE_GAMUT_EN		        REG_FLD(1, 18)
#define SIZE_CON_0_FLD_MATRIX_ENABLE				REG_FLD(1, 17)
#define SIZE_CON_0_FLD_MATRIX_EXT_MTX_EN		    REG_FLD(1, 16)
#define SIZE_CON_0_FLD_OUTPUT_FRAME_WIDTH		        REG_FLD(13, 0)
#define SIZE_CON_1_FLD_OUTPUT_FRAME_HEIGHT		        REG_FLD(20, 0)
#define TARGET_LINE_FLD_TARGET_LINE				REG_FLD(20, 0)
#define MEM_CON_FLD_MEM_MODE_HORI_BLOCK_NUM		            REG_FLD(8, 24)
#define MEM_CON_FLD_MEM_MODE_INPUT_COSITE		            REG_FLD(1, 13)
#define MEM_CON_FLD_MEM_MODE_INPUT_UPSAMPLE		        REG_FLD(1, 12)
#define MEM_CON_FLD_MEM_MODE_INPUT_SWAP				        REG_FLD(1, 8)
#define MEM_CON_FLD_MEM_MODE_INPUT_FORMAT		            REG_FLD(4, 4)
#define MEM_CON_FLD_MEM_MODE_TILE_INTERLACE		        REG_FLD(1, 1)
#define MEM_CON_FLD_MEM_MODE_TILE_EN                            REG_FLD(1, 0)
#define MEM_SRC_PITCH_FLD_MEM_MODE_SRC_PITCH		        REG_FLD(16, 0)
#define MEM_GMC_SETTING_0_FLD_PRE_ULTRA_THRESHOLD_HIGH_OFS	    REG_FLD(8, 24)
#define MEM_GMC_SETTING_0_FLD_ULTRA_THRESHOLD_HIGH_OFS		REG_FLD(8, 16)
#define MEM_GMC_SETTING_0_FLD_PRE_ULTRA_THRESHOLD_LOW_OFS	    REG_FLD(8, 8)
#define MEM_GMC_SETTING_0_FLD_ULTRA_THRESHOLD_LOW               REG_FLD(8, 0)
#define MEM_SLOW_CON_FLD_MEM_MODE_SLOW_COUNT			REG_FLD(16, 16)
#define MEM_SLOW_CON_FLD_MEM_MODE_SLOW_EN			REG_FLD(1, 0)
#define MEM_GMC_SETTING_1_FLD_ISSUE_REQ_THRESHOLD		REG_FLD(8, 0)
#define FIFO_CON_FLD_FIFO_UNDERFLOW_EN				REG_FLD(1, 31)
#define FIFO_CON_FLD_FIFO_PSEUDO_SIZE				REG_FLD(12, 16)
#define FIFO_CON_FLD_OUTPUT_VALID_FIFO_THRESHOLD		REG_FLD(12, 0)
#define FIFO_LOG_FLD_RDMA_FIFO_LOG					REG_FLD(10, 0)
#define C00_FLD_DISP_RDMA_C00					REG_FLD(13, 0)
#define C01_FLD_DISP_RDMA_C01                                   REG_FLD(13, 0)
#define C02_FLD_DISP_RDMA_C02					REG_FLD(13, 0)
#define C10_FLD_DISP_RDMA_C10					REG_FLD(13, 0)
#define C11_FLD_DISP_RDMA_C11					REG_FLD(13, 0)
#define C12_FLD_DISP_RDMA_C12					REG_FLD(13, 0)
#define C20_FLD_DISP_RDMA_C20					REG_FLD(13, 0)
#define C21_FLD_DISP_RDMA_C21					REG_FLD(13, 0)
#define C22_FLD_DISP_RDMA_C22					REG_FLD(13, 0)
#define PRE_ADD_0_FLD_DISP_RDMA_PRE_ADD_0				    REG_FLD(9, 0)
#define PRE_ADD_1_FLD_DISP_RDMA_PRE_ADD_1				    REG_FLD(9, 0)
#define PRE_ADD_2_FLD_DISP_RDMA_PRE_ADD_2				    REG_FLD(9, 0)
#define POST_ADD_0_FLD_DISP_RDMA_POST_ADD_0				    REG_FLD(9, 0)
#define POST_ADD_1_FLD_DISP_RDMA_POST_ADD_1			        REG_FLD(9, 0)
#define POST_ADD_2_FLD_DISP_RDMA_POST_ADD_2			        REG_FLD(9, 0)
#define DUMMY_FLD_DISP_RDMA_DUMMY				REG_FLD(32, 0)
#define DEBUG_OUT_SEL_FLD_DISP_RDMA_DEBUG_OUT_SEL	        REG_FLD(4, 0)
#define MEM_START_ADDR_FLD_MEM_MODE_START_ADDR		        REG_FLD(32, 0)
#define RDMA_BG_CON_0_LEFT					REG_FLD(13, 0)
#define RDMA_BG_CON_0_RIGHT					REG_FLD(13, 16)
#define RDMA_BG_CON_1_TOP					REG_FLD(13, 0)
#define RDMA_BG_CON_1_BOTTOM				REG_FLD(13, 16)

/* ------------------------------------------------------------- */
/* SPLIT */
#define DISP_REG_SPLIT_ENABLE				(DISPSYS_SPLIT0_BASE+0x00)
#define DISP_REG_SPLIT_SW_RESET				(DISPSYS_SPLIT0_BASE+0x04)
#define DISP_REG_SPLIT_CFG				(DISPSYS_SPLIT0_BASE+0x20)
#define DISP_REG_SPLIT_DEBUG				(DISPSYS_SPLIT0_BASE+0x60)

#define ENABLE_FLD_SPLIT_EN				REG_FLD(1, 0)
#define W_RESET_FLD_SPLIT_SW_RST			REG_FLD(1, 0)
#define DEBUG_FLD_SPLIT_FSM				REG_FLD(2, 28)
#define DEBUG_FLD_IN_PIXEL_CNT				REG_FLD(24, 0)

/* ------------------------------------------------------------- */
/* UFO */
#define DISP_REG_UFO_START				(DISPSYS_UFOE_BASE+0x000)

#define DISP_REG_UFO_INTEN				(DISPSYS_UFOE_BASE+0x004)
#define DISP_REG_UFO_INTSTA				(DISPSYS_UFOE_BASE+0x008)
#define DISP_REG_UFO_DBUF				(DISPSYS_UFOE_BASE+0x00C)
#define DISP_REG_UFO_CRC				(DISPSYS_UFOE_BASE+0x014)
#define DISP_REG_UFO_SW_SCRATCH				(DISPSYS_UFOE_BASE+0x018)
#define DISP_REG_UFO_CR0P6_PAD				(DISPSYS_UFOE_BASE+0x020)
#define DISP_REG_UFO_LR_OVERLAP				(DISPSYS_UFOE_BASE+0x024)
#define DISP_REG_UFO_CK_ON				(DISPSYS_UFOE_BASE+0x028)
#define DISP_REG_UFO_STALL_CG				(DISPSYS_UFOE_BASE+0x030)
#define DISP_REG_UFO_FRAME_WIDTH			(DISPSYS_UFOE_BASE+0x050)
#define DISP_REG_UFO_FRAME_HEIGHT			(DISPSYS_UFOE_BASE+0x054)
#define DISP_REG_UFO_OUTEN				(DISPSYS_UFOE_BASE+0x058)

#define DISP_REG_UFO_R0_CRC				(DISPSYS_UFOE_BASE+0x0F0)

#define DISP_REG_UFO_CFG_0B				(DISPSYS_UFOE_BASE+0x100)
#define DISP_REG_UFO_CFG_1B				(DISPSYS_UFOE_BASE+0x104)
#define DISP_REG_UFO_CFG_2B				(DISPSYS_UFOE_BASE+0x108)
#define DISP_REG_UFO_CFG_3B				(DISPSYS_UFOE_BASE+0x10C)
#define DISP_REG_UFO_CFG_4B				(DISPSYS_UFOE_BASE+0x110)

#define DISP_REG_UFO_RO_0B				(DISPSYS_UFOE_BASE+0x120)
#define DISP_REG_UFO_RO_1B				(DISPSYS_UFOE_BASE+0x124)
#define DISP_REG_UFO_RO_2B				(DISPSYS_UFOE_BASE+0x128)
#define DISP_REG_UFO_RO_3B				(DISPSYS_UFOE_BASE+0x12C)
#define DISP_REG_UFO_RO_4B				(DISPSYS_UFOE_BASE+0x130)

#define START_FLD_DISP_UFO_START			REG_FLD(1, 0)
#define START_FLD_DISP_UFO_OUT_SEL			REG_FLD(1, 1)
#define START_FLD_DISP_UFO_BYPASS			REG_FLD(1, 2)
#define START_FLD_DISP_UFO_LR_EN			REG_FLD(1, 3)
#define START_FLD_DISP_UFO_SW_RST_ENGINE		REG_FLD(1, 8)
#define START_FLD_DISP_UFO_DBG_SEL			REG_FLD(8, 16)

#define CFG_0B_FLD_DISP_UFO_CFG_VLC_EN			REG_FLD(1, 0)
#define CFG_0B_FLD_DISP_UFO_CFG_COM_RATIO		REG_FLD(2, 1)

#define CFG_1B_FLD_DISP_UFO_CFG_1B			REG_FLD(32, 0)
#define CFG_2B_FLD_DISP_UFO_CFG_2B			REG_FLD(32, 0)
#define CFG_3B_FLD_DISP_UFO_CFG_3B			REG_FLD(32, 0)
#define CFG_4B_FLD_DISP_UFO_CFG_4B			REG_FLD(32, 0)

#define RO_0B_FLD_DISP_UFO_RO_0B			REG_FLD(32, 0)
#define RO_1B_FLD_DISP_UFO_RO_1B			REG_FLD(32, 0)
#define RO_2B_FLD_DISP_UFO_RO_2B			REG_FLD(32, 0)
#define RO_3B_FLD_DISP_UFO_RO_3B			REG_FLD(32, 0)
#define RO_4B_FLD_DISP_UFO_RO_4B			REG_FLD(32, 0)

#define CR0P6_PAD_FLD_DISP_UFO_STR_PAD_NUM		REG_FLD(3, 0)

#define INTEN_FLD_DISP_UFO_INTEN_FR_UNDERRUN		REG_FLD(1, 2)
#define INTEN_FLD_DISP_UFO_INTEN_FR_DONE		REG_FLD(1, 1)
#define INTEN_FLD_DISP_UFO_INTEN_FR_COMPLETE		REG_FLD(1, 0)
#define INTSTA_FLD_DISP_UFO_INTSTA_FR_UNDERRUN		REG_FLD(1, 2)
#define INTSTA_FLD_DISP_UFO_INTSTA_FR_DONE		REG_FLD(1, 1)
#define INTSTA_FLD_DISP_UFO_INTSTA_FR_COMPLETE		REG_FLD(1, 0)

#define DBUF_FLD_DISP_UFO_DBUF_DIS			REG_FLD(5, 0)
#define CRC_FLD_DISP_UFO_CRC_CLR			REG_FLD(1, 2)
#define CRC_FLD_DISP_UFO_CRC_START			REG_FLD(1, 1)
#define CRC_FLD_DISP_UFO_CRC_CEN			REG_FLD(1, 0)

#define SW_SCRATCH_FLD_DISP_UFO_SW_SCRATCH		REG_FLD(32, 0)
#define CK_ON_FLD_DISP_UFO_CK_ON			REG_FLD(1, 0)
#define FRAME_WIDTH_FLD_DISP_UFO_FRAME_WIDTH		REG_FLD(12, 0)
#define FRAME_HEIGHT_FLD_DISP_UFO_FRAME_HEIGHT		REG_FLD(13, 0)
#define R0_CRC_FLD_DISP_UFO_ENGINE_END			REG_FLD(1, 17)
#define R0_CRC_FLD_DISP_UFO_CRC_RDY_0			REG_FLD(1, 16)
#define R0_CRC_FLD_DISP_UFO_CRC_OUT_0			REG_FLD(16, 0)

/* ------------------------------------------------------------- */
/* WDMA */
#define DISP_REG_WDMA_INTEN							(DISPSYS_WDMA0_BASE+0x000)
#define DISP_REG_WDMA_INTSTA							(DISPSYS_WDMA0_BASE+0x004)
#define DISP_REG_WDMA_EN							(DISPSYS_WDMA0_BASE+0x008)
#define DISP_REG_WDMA_RST							(DISPSYS_WDMA0_BASE+0x00C)
#define DISP_REG_WDMA_SMI_CON							(DISPSYS_WDMA0_BASE+0x010)
#define DISP_REG_WDMA_CFG							(DISPSYS_WDMA0_BASE+0x014)
#define DISP_REG_WDMA_SRC_SIZE							(DISPSYS_WDMA0_BASE+0x018)
#define DISP_REG_WDMA_CLIP_SIZE							(DISPSYS_WDMA0_BASE+0x01C)
#define DISP_REG_WDMA_CLIP_COORD					    (DISPSYS_WDMA0_BASE+0x020)
#define DISP_REG_WDMA_DST_W_IN_BYTE					    (DISPSYS_WDMA0_BASE+0x028)
#define DISP_REG_WDMA_ALPHA							(DISPSYS_WDMA0_BASE+0x02C)
#define DISP_REG_WDMA_BUF_CON1							(DISPSYS_WDMA0_BASE+0x038)
#define DISP_REG_WDMA_BUF_CON2							(DISPSYS_WDMA0_BASE+0x03C)
#define DISP_REG_WDMA_C00							(DISPSYS_WDMA0_BASE+0x040)
#define DISP_REG_WDMA_C02							(DISPSYS_WDMA0_BASE+0x044)
#define DISP_REG_WDMA_C10							(DISPSYS_WDMA0_BASE+0x048)
#define DISP_REG_WDMA_C12							(DISPSYS_WDMA0_BASE+0x04C)
#define DISP_REG_WDMA_C20							(DISPSYS_WDMA0_BASE+0x050)
#define DISP_REG_WDMA_C22							(DISPSYS_WDMA0_BASE+0x054)
#define DISP_REG_WDMA_PRE_ADD0							(DISPSYS_WDMA0_BASE+0x058)
#define DISP_REG_WDMA_PRE_ADD2							(DISPSYS_WDMA0_BASE+0x05C)
#define DISP_REG_WDMA_POST_ADD0						    (DISPSYS_WDMA0_BASE+0x060)
#define DISP_REG_WDMA_POST_ADD2						    (DISPSYS_WDMA0_BASE+0x064)
#define DISP_REG_WDMA_DST_UV_PITCH					    (DISPSYS_WDMA0_BASE+0x078)
#define DISP_REG_WDMA_DST_ADDR_OFFSET0				        (DISPSYS_WDMA0_BASE+0x080)
#define DISP_REG_WDMA_DST_ADDR_OFFSET1				        (DISPSYS_WDMA0_BASE+0x084)
#define DISP_REG_WDMA_DST_ADDR_OFFSET2				        (DISPSYS_WDMA0_BASE+0x088)
#define DISP_REG_WDMA_PROC_TRACK_CON_0				        (DISPSYS_WDMA0_BASE+0x090)
#define DISP_REG_WDMA_PROC_TRACK_CON_1				        (DISPSYS_WDMA0_BASE+0x094)
#define DISP_REG_WDMA_PROC_TRACK_CON_2				        (DISPSYS_WDMA0_BASE+0x098)
#define DISP_REG_WDMA_FLOW_CTRL_DBG					    (DISPSYS_WDMA0_BASE+0x0A0)
#define DISP_REG_WDMA_EXEC_DBG							(DISPSYS_WDMA0_BASE+0x0A4)
#define DISP_REG_WDMA_CT_DBG							(DISPSYS_WDMA0_BASE+0x0A8)
#define DISP_REG_WDMA_SMI_TRAFFIC_DBG                                   (DISPSYS_WDMA0_BASE+0x0AC)
#define DISP_REG_WDMA_PROC_TRACK_DBG_0                                  (DISPSYS_WDMA0_BASE+0x0B0)
#define DISP_REG_WDMA_PROC_TRACK_DBG_1                                  (DISPSYS_WDMA0_BASE+0x0B4)
#define DISP_REG_WDMA_DEBUG							(DISPSYS_WDMA0_BASE+0x0B8)
#define DISP_REG_WDMA_DUMMY							(DISPSYS_WDMA0_BASE+0x100)
#define DISP_REG_WDMA_DITHER_0							(DISPSYS_WDMA0_BASE+0xE00)
#define DISP_REG_WDMA_DITHER_5							(DISPSYS_WDMA0_BASE+0xE14)
#define DISP_REG_WDMA_DITHER_6							(DISPSYS_WDMA0_BASE+0xE18)
#define DISP_REG_WDMA_DITHER_7							(DISPSYS_WDMA0_BASE+0xE1C)
#define DISP_REG_WDMA_DITHER_8							(DISPSYS_WDMA0_BASE+0xE20)
#define DISP_REG_WDMA_DITHER_9							(DISPSYS_WDMA0_BASE+0xE24)
#define DISP_REG_WDMA_DITHER_10						    (DISPSYS_WDMA0_BASE+0xE28)
#define DISP_REG_WDMA_DITHER_11						    (DISPSYS_WDMA0_BASE+0xE2C)
#define DISP_REG_WDMA_DITHER_12						(DISPSYS_WDMA0_BASE+0xE30)
#define DISP_REG_WDMA_DITHER_13						(DISPSYS_WDMA0_BASE+0xE34)
#define DISP_REG_WDMA_DITHER_14						(DISPSYS_WDMA0_BASE+0xE38)
#define DISP_REG_WDMA_DITHER_15						(DISPSYS_WDMA0_BASE+0xE3C)
#define DISP_REG_WDMA_DITHER_16						(DISPSYS_WDMA0_BASE+0xE40)
#define DISP_REG_WDMA_DITHER_17						(DISPSYS_WDMA0_BASE+0xE44)
#define DISP_REG_WDMA_DST_ADDR0						(DISPSYS_WDMA0_BASE+0xF00)
#define DISP_REG_WDMA_DST_ADDR1						(DISPSYS_WDMA0_BASE+0xF04)
#define DISP_REG_WDMA_DST_ADDR2						(DISPSYS_WDMA0_BASE+0xF08)

#define INTEN_FLD_FRAME_UNDERRUN						REG_FLD(1, 1)
#define INTEN_FLD_FRAME_COMPLETE						REG_FLD(1, 0)
#define INTSTA_FLD_FRAME_UNDERRUN						REG_FLD(1, 1)
#define INTSTA_FLD_FRAME_COMPLETE						REG_FLD(1, 0)
#define EN_FLD_ENABLE										REG_FLD(1, 0)
#define RST_FLD_SOFT_RESET								REG_FLD(1, 0)
#define SMI_CON_FLD_SMI_V_REPEAT_NUM					REG_FLD(4, 24)
#define SMI_CON_FLD_SMI_U_REPEAT_NUM					REG_FLD(4, 20)
#define SMI_CON_FLD_SMI_Y_REPEAT_NUM					REG_FLD(4, 16)
#define SMI_CON_FLD_SLOW_COUNT							REG_FLD(8, 8)
#define SMI_CON_FLD_SLOW_LEVEL							REG_FLD(3, 5)
#define SMI_CON_FLD_SLOW_ENABLE							REG_FLD(1, 4)
#define SMI_CON_FLD_THRESHOLD							REG_FLD(4, 0)
#define CFG_FLD_DEBUG_SEL								REG_FLD(4, 28)
#define CFG_FLD_INT_MTX_SEL								REG_FLD(4, 24)
#define CFG_FLD_SWAP									REG_FLD(1, 16)
#define CFG_FLD_DNSP_SEL								REG_FLD(1, 15)
#define CFG_FLD_EXT_MTX_EN								REG_FLD(1, 13)
#define CFG_FLD_VERTICAL_AVG								REG_FLD(1, 12)
#define CFG_FLD_CT_EN										REG_FLD(1, 11)
#define CFG_FLD_OUT_FORMAT								REG_FLD(4, 4)
#define SRC_SIZE_FLD_HEIGHT								REG_FLD(14, 16)
#define SRC_SIZE_FLD_WIDTH								REG_FLD(14, 0)
#define CLIP_SIZE_FLD_HEIGHT								REG_FLD(14, 16)
#define CLIP_SIZE_FLD_WIDTH								REG_FLD(14, 0)
#define CLIP_COORD_FLD_Y_COORD							REG_FLD(14, 16)
#define CLIP_COORD_FLD_X_COORD							REG_FLD(14, 0)
#define DST_W_IN_BYTE_FLD_DST_W_IN_BYTE					REG_FLD(16, 0)
#define ALPHA_FLD_A_SEL									REG_FLD(1, 31)
#define ALPHA_FLD_A_VALUE								REG_FLD(8, 0)
#define BUF_CON1_FLD_ULTRA_ENABLE						REG_FLD(1, 31)
#define BUF_CON1_FLD_PRE_ULTRA_ENABLE						REG_FLD(1, 30)
#define BUF_CON1_FLD_FRAME_END_ULTRA					REG_FLD(1, 28)
#define BUF_CON1_FLD_ISSUE_REQ_TH						REG_FLD(9, 16)
#define BUF_CON1_FLD_FIFO_PSEUDO_SIZE					REG_FLD(9, 0)
#define BUF_CON2_FLD_ULTRA_TH_HIGH_OFS					REG_FLD(8, 24)
#define BUF_CON2_FLD_PRE_ULTRA_TH_HIGH_OFS				REG_FLD(8, 16)
#define BUF_CON2_FLD_ULTRA_TH_LOW_OFS					REG_FLD(8, 8)
#define BUF_CON2_FLD_PRE_ULTRA_TH_LOW					REG_FLD(8, 0)
#define C00_FLD_C01										REG_FLD(13, 16)
#define C00_FLD_C00										REG_FLD(13, 0)
#define C02_FLD_C02										REG_FLD(13, 0)
#define C10_FLD_C11										REG_FLD(13, 16)
#define C10_FLD_C10										REG_FLD(13, 0)
#define C12_FLD_C12										REG_FLD(13, 0)
#define C20_FLD_C21										REG_FLD(13, 16)
#define C20_FLD_C20										REG_FLD(13, 0)
#define C22_FLD_C22										REG_FLD(13, 0)
#define PRE_ADD0_FLD_PRE_ADD_1							REG_FLD(9, 16)
#define PRE_ADD0_FLD_PRE_ADD_0							REG_FLD(9, 0)
#define PRE_ADD2_FLD_PRE_ADD_2							REG_FLD(9, 0)
#define POST_ADD0_FLD_POST_ADD_1						REG_FLD(9, 16)
#define POST_ADD0_FLD_POST_ADD_0						REG_FLD(9, 0)
#define POST_ADD2_FLD_POST_ADD_2						REG_FLD(9, 0)
#define DST_UV_PITCH_FLD_UV_DST_W_IN_BYTE			REG_FLD(16, 0)
#define DST_ADDR_OFFSET0_FLD_WDMA_DESTINATION_ADDRESS_OFFSET0	REG_FLD(28, 0)
#define DST_ADDR_OFFSET1_FLD_WDMA_DESTINATION_ADDRESS_OFFSET1	REG_FLD(28, 0)
#define DST_ADDR_OFFSET2_FLD_WDMA_DESTINATION_ADDRESS_OFFSET2	REG_FLD(28, 0)

#define FLOW_CTRL_DBG_FLD_WDMA_STA_FLOW_CTRL						REG_FLD(10, 0)
#define EXEC_DBG_FLD_WDMA_IN_REQ						REG_FLD(1, 15)
#define EXEC_DBG_FLD_WDMA_IN_ACK						REG_FLD(1, 14)

#define EXEC_DBG_FLD_WDMA_STA_EXEC				REG_FLD(32, 0)
#define CT_DBG_FLD_WDMA_STA_CT					REG_FLD(32, 0)
#define DEBUG_FLD_WDMA_STA_DEBUG				REG_FLD(32, 0)
#define DUMMY_FLD_WDMA_DUMMY					REG_FLD(32, 0)
#define DITHER_0_FLD_CRC_CLR						REG_FLD(1, 24)
#define DITHER_0_FLD_CRC_START					REG_FLD(1, 20)
#define DITHER_0_FLD_CRC_CEN						REG_FLD(1, 16)
#define DITHER_0_FLD_FRAME_DONE_DEL				REG_FLD(8, 8)
#define DITHER_0_FLD_OUT_SEL						REG_FLD(1, 4)
#define DITHER_0_FLD_START						REG_FLD(1, 0)
#define DITHER_5_FLD_W_DEMO						REG_FLD(16, 0)
#define DITHER_6_FLD_WRAP_MODE					REG_FLD(1, 16)
#define DITHER_6_FLD_LEFT_EN						REG_FLD(2, 14)
#define DITHER_6_FLD_FPHASE_R						REG_FLD(1, 13)
#define DITHER_6_FLD_FPHASE_EN					REG_FLD(1, 12)
#define DITHER_6_FLD_FPHASE						REG_FLD(6, 4)
#define DITHER_6_FLD_ROUND_EN					REG_FLD(1, 3)
#define DITHER_6_FLD_RDITHER_EN					REG_FLD(1, 2)
#define DITHER_6_FLD_LFSR_EN						REG_FLD(1, 1)
#define DITHER_6_FLD_EDITHER_EN					REG_FLD(1, 0)
#define DITHER_7_FLD_DRMOD_B						REG_FLD(2, 8)
#define DITHER_7_FLD_DRMOD_G						REG_FLD(2, 4)
#define DITHER_7_FLD_DRMOD_R						REG_FLD(2, 0)
#define DITHER_8_FLD_INK_DATA_R					REG_FLD(10, 16)
#define DITHER_8_FLD_INK						REG_FLD(1, 0)
#define DITHER_9_FLD_INK_DATA_B					 REG_FLD(10, 16)
#define DITHER_9_FLD_INK_DATA_G					REG_FLD(10, 0)
#define DITHER_10_FLD_FPHASE_BIT				REG_FLD(3, 8)
#define DITHER_10_FLD_FPHASE_SEL				REG_FLD(2, 4)
#define DITHER_10_FLD_FPHASE_CTRL				REG_FLD(2, 0)
#define DITHER_11_FLD_SUB_B						REG_FLD(2, 12)
#define DITHER_11_FLD_SUB_G						REG_FLD(2, 8)
#define DITHER_11_FLD_SUB_R						REG_FLD(2, 4)
#define DITHER_11_FLD_SUBPIX_EN					REG_FLD(1, 0)
#define DITHER_12_FLD_H_ACTIVE					REG_FLD(16, 16)
#define DITHER_12_FLD_TABLE_EN					REG_FLD(2, 4)
#define DITHER_12_FLD_LSB_OFF						REG_FLD(1, 0)
#define DITHER_13_FLD_RSHIFT_B						REG_FLD(3, 8)
#define DITHER_13_FLD_RSHIFT_G					REG_FLD(3, 4)
#define DITHER_13_FLD_RSHIFT_R						REG_FLD(3, 0)
#define DITHER_14_FLD_DEBUG_MODE				REG_FLD(2, 8)
#define DITHER_14_FLD_DIFF_SHIFT				REG_FLD(3, 4)
#define DITHER_14_FLD_TESTPIN_EN				REG_FLD(1, 0)
#define DITHER_15_FLD_LSB_ERR_SHIFT_R				REG_FLD(3, 28)
#define DITHER_15_FLD_OVFLW_BIT_R				REG_FLD(3, 24)
#define DITHER_15_FLD_ADD_lSHIFT_R				REG_FLD(3, 20)
#define DITHER_15_FLD_INPUT_RSHIFT_R				REG_FLD(3, 16)
#define DITHER_15_FLD_NEW_BIT_MODE				REG_FLD(1, 0)
#define DITHER_16_FLD_LSB_ERR_SHIFT_B				REG_FLD(3, 28)
#define DITHER_16_FLD_OVFLW_BIT_B				REG_FLD(3, 24)
#define DITHER_16_FLD_ADD_lSHIFT_B				REG_FLD(3, 20)
#define DITHER_16_FLD_INPUT_RSHIFT_B				REG_FLD(3, 16)
#define DITHER_16_FLD_lSB_ERR_SHIFT_G				REG_FLD(3, 12)
#define DITHER_16_FLD_OVFLW_BIT_G				REG_FLD(3, 8)
#define DITHER_16_FLD_ADD_lSHIFT_G				REG_FLD(3, 4)
#define DITHER_16_FLD_INPUT_RSHIFT_G				REG_FLD(3, 0)
#define DITHER_17_FLD_CRC_RDY						REG_FLD(1, 16)
#define DITHER_17_FLD_CRC_OUT						REG_FLD(16, 0)
#define DST_ADDR0_FLD_ADDRESS0					REG_FLD(32, 0)
#define DST_ADDR1_FLD_ADDRESS1					REG_FLD(32, 0)
#define DST_ADDR2_FLD_ADDRESS2					REG_FLD(32, 0)

#include <platform/ddp_reg_mmsys.h>
#include <platform/ddp_reg_ovl.h>

#endif
