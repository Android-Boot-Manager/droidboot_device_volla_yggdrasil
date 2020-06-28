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

#include "cust_msdc.h"
#include "msdc.h"


struct msdc_cust msdc_cap[MSDC_MAX_NUM] = {
	{
		MSDC50_CLKSRC_DEFAULT, /* host clock source          */
		MSDC_SMPL_RISING,   /* command latch edge            */
		MSDC_SMPL_RISING,   /* data latch edge               */
		MSDC_DRVN_GEAR3,    /* clock pad driving             */
		MSDC_DRVN_GEAR3,    /* command pad driving           */
		MSDC_DRVN_GEAR3,    /* data pad driving              */
		MSDC_DRVN_GEAR3,    /* rst pad driving               */
		MSDC_DRVN_GEAR1,    /* ds pad driving                */
		MSDC_DRVN_DONT_CARE,/* clock pad driving on 1.8V     */
		MSDC_DRVN_DONT_CARE,/* command pad driving on 1.8V   */
		MSDC_DRVN_DONT_CARE,/* data pad driving on 1.8V      */
		8,                  /* data pins                     */
		0,                  /* data address offset           */
#if defined(MMC_MSDC_DRV_CTP)
		MSDC_HIGHSPEED | MSDC_HS200 | MSDC_RST_PIN_EN | MSDC_DDR | MSDC_HS400
#else
		MSDC_HIGHSPEED | MSDC_DDR
#endif
	}

#if (defined(MMC_MSDC_DRV_CTP) && !defined(FPGA_PLATFORM))
	,
	{
		MSDC30_CLKSRC_DEFAULT, /* host clock source          */
		MSDC_SMPL_RISING,   /* command latch edge            */
		MSDC_SMPL_RISING,   /* data latch edge               */
		MSDC_DRVN_GEAR4,    /* clock pad driving             */
		MSDC_DRVN_GEAR3,    /* command pad driving           */
		MSDC_DRVN_GEAR3,    /* data pad driving              */
		MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
		MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
		MSDC_DRVN_GEAR3,    /* clock pad driving on 1.8V     */
		MSDC_DRVN_GEAR3,    /* command pad driving on 1.8V   */
		MSDC_DRVN_GEAR3,    /* data pad driving on 1.8V      */
		4,                  /* data pins                     */
		0,                  /* data address offset           */

		/* hardware capability flags     */
		MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR //|MSDC_UHS1|MSDC_SDIO_IRQ|MSDC_DDR
	}
	,
	{
		MSDC30_CLKSRC_DEFAULT, /* host clock source          */
		MSDC_SMPL_RISING,   /* command latch edge            */
		MSDC_SMPL_RISING,   /* data latch edge               */
		MSDC_DRVN_GEAR1,    /* clock pad driving             */
		MSDC_DRVN_GEAR1,    /* command pad driving           */
		MSDC_DRVN_GEAR1,    /* data pad driving              */
		MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
		MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
		MSDC_DRVN_GEAR1,    /* clock pad driving on 1.8V     */
		MSDC_DRVN_GEAR1,    /* command pad driving on 1.8V   */
		MSDC_DRVN_GEAR1 ,   /* data pad driving on 1.8V      */
		4,                  /* data pins                     */
		0,                  /* data address offset           */

		/* hardware capability flags     */
		MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR //|MSDC_UHS1|MSDC_SDIO_IRQ|MSDC_DDR
	}
#endif
};



