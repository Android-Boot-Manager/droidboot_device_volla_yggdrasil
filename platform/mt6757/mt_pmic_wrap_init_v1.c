/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
/******************************************************************************
 * MTK PMIC Wrapper Driver
 *
 * Copyright 2016 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     This file provides API for other drivers to access PMIC registers
 *
 ******************************************************************************/

#include <mt_pmic_wrap_init.h>
#if (PMIC_WRAP_PRELOADER)
#elif (PMIC_WRAP_LK)
#elif (PMIC_WRAP_KERNEL)
#elif (PMIC_WRAP_CTP)
#include <gpio.h>
#include <upmu_hw.h>
#else
### Compile error, check SW ENV define
#endif


/************* marco    ******************************************************/
#if (PMIC_WRAP_PRELOADER)
#elif (PMIC_WRAP_LK)
#elif (PMIC_WRAP_KERNEL)
#elif (PMIC_WRAP_SCP)
#elif (PMIC_WRAP_CTP)
#else
### Compile error, check SW ENV define
#endif

#ifdef PMIC_WRAP_NO_PMIC
#if !(PMIC_WRAP_KERNEL)
signed int pwrap_wacs2(unsigned int  write, unsigned int  adr, unsigned int  wdata, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_read(unsigned int adr, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_write(unsigned int adr, unsigned int wdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}
#endif
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

/* Provide PMIC write API */
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_read_nochk(unsigned int adr, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_write_nochk(unsigned int adr, unsigned int wdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

/*
 *pmic_wrap init,init wrap interface
 *
 */
signed int pwrap_init(void)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_init_preloader(void)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_init_lk(void)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}
#else /* #ifdef PMIC_WRAP_NO_PMIC */
/*********************start ---internal API***********************************/
static int _pwrap_timeout_ns(unsigned long long start_time_ns, unsigned long long timeout_time_ns);
static unsigned long long _pwrap_get_current_time(void);
static unsigned long long _pwrap_time2ns(unsigned long long time_us);
static signed int _pwrap_reset_spislv(void);
static signed int _pwrap_init_dio(unsigned int dio_en);
static signed int _pwrap_init_cipher(void);
static signed int _pwrap_init_reg_clock(unsigned int regck_sel);
static void _pwrap_enable(void);
static void _pwrap_starve_set(void);
static signed int _pwrap_wacs2_nochk(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata);
static signed int pwrap_wacs2_hal(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata);
/*********************test API************************************************/
static inline void pwrap_dump_ap_register(void);
static unsigned int pwrap_write_test(void);
static unsigned int pwrap_read_test(void);
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata);
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata);
/************* end--internal API**********************************************/
/*********************** external API for pmic_wrap user ************************/
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata)
{
	pwrap_wacs2(0, adr, 0, rdata);
	return 0;
}

/* Provide PMIC write API */
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata)
{
#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
	unsigned int flag;

	flag = WRITE_CMD | (1 << WRITE_PMIC);
	pwrap_wacs2_ipi(adr, wdata, flag);
#else
	pwrap_wacs2(1, adr, wdata, 0);
#endif
	return 0;
}

signed int pwrap_read(unsigned int adr, unsigned int *rdata)
{
	return pwrap_wacs2(0,adr,0,rdata);
}

signed int pwrap_write(unsigned int adr, unsigned int  wdata)
{
	return pwrap_wacs2(1,adr,wdata,0);
}
/******************************************************************************
 wrapper timeout
******************************************************************************/
/*use the same API name with kernel driver
 *however,the timeout API in uboot use tick instead of ns */
#ifdef PWRAP_TIMEOUT
static unsigned long long _pwrap_get_current_time(void)
{
	return gpt4_get_current_tick();
}

static int _pwrap_timeout_ns(unsigned long long start_time_ns, unsigned long long timeout_time_ns)
{
	return gpt4_timeout_tick(start_time_ns, timeout_time_ns);
}

static unsigned long long _pwrap_time2ns(unsigned long long time_us)
{
	return gpt4_time2tick_us(time_us);
}

#else
static unsigned long long _pwrap_get_current_time(void)
{
	return 0;
}
static int _pwrap_timeout_ns(unsigned long long start_time_ns, unsigned long long elapse_time)
{
	return 0;
}

static unsigned long long _pwrap_time2ns(unsigned long long time_us)
{
	return 0;
}

#endif

/* ##################################################################### */
/* define macro and inline function (for do while loop) */
/* ##################################################################### */
typedef unsigned int(*loop_condition_fp) (unsigned int);    /* define a function pointer */

static inline unsigned int wait_for_fsm_idle(unsigned int x)
{
	return GET_WACS2_FSM(x) != WACS_FSM_IDLE;
}

static inline unsigned int wait_for_fsm_vldclr(unsigned int x)
{
	return GET_WACS2_FSM(x) != WACS_FSM_WFVLDCLR;
}

static inline unsigned int wait_for_sync(unsigned int x)
{
	return GET_SYNC_IDLE2(x) != WACS_SYNC_IDLE;
}

static inline unsigned int wait_for_idle_and_sync(unsigned int x)
{
	return (GET_WACS2_FSM(x) != WACS_FSM_IDLE) || (GET_SYNC_IDLE2(x) != WACS_SYNC_IDLE);
}

static inline unsigned int wait_for_wrap_idle(unsigned int x)
{
	return (GET_WRAP_FSM(x) != 0x0) || (GET_WRAP_CH_DLE_RESTCNT(x) != 0x0);
}

static inline unsigned int wait_for_wrap_state_idle(unsigned int x)
{
	return GET_WRAP_AG_DLE_RESTCNT(x) != 0;
}

static inline unsigned int wait_for_man_idle_and_noreq(unsigned int x)
{
	return (GET_MAN_REQ(x) != MAN_FSM_NO_REQ) || (GET_MAN_FSM(x) != MAN_FSM_IDLE);
}

static inline unsigned int wait_for_man_vldclr(unsigned int x)
{
	return GET_MAN_FSM(x) != MAN_FSM_WFVLDCLR;
}

static inline unsigned int wait_for_cipher_ready(unsigned int x)
{
	return x != 3;
}

static inline unsigned int wait_for_stdupd_idle(unsigned int x)
{
	return GET_STAUPD_FSM(x) != 0x0;
}

/**************used at _pwrap_wacs2_nochk*************************************/
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_ready_init(loop_condition_fp fp, unsigned int timeout_us,
        void *wacs_register, unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_ready_init(loop_condition_fp fp, unsigned int timeout_us,
        unsigned int wacs_register, unsigned int *read_reg)
#endif
{
	unsigned long long start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata = 0x0;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("wait_for_state_ready_init timeout when waiting for idle\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
	} while (fp(reg_rdata));

	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_idle(loop_condition_fp fp, unsigned int timeout_us, void *wacs_register,
        void *wacs_vldclr_register, unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_idle(loop_condition_fp fp, unsigned int timeout_us,
        unsigned int wacs_register, unsigned int wacs_vldclr_register, unsigned int *read_reg)
#endif
{
	unsigned long long start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("wait_for_state_idle timeout when waiting for idle\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
		if (GET_INIT_DONE2(reg_rdata) != WACS_INIT_DONE) {
			PWRAPERR("initialization isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
		switch (GET_WACS2_FSM(reg_rdata)) {
			case WACS_FSM_WFVLDCLR:
				WRAP_WR32(wacs_vldclr_register, 1);
				PWRAPERR("WACS_FSM = PMIC_WRAP_WACS_VLDCLR\n");
				break;
			case WACS_FSM_WFDLE:
				PWRAPERR("WACS_FSM = WACS_FSM_WFDLE\n");
				break;
			case WACS_FSM_REQ:
				PWRAPERR("WACS_FSM = WACS_FSM_REQ\n");
				break;
			default:
				break;
		}
	} while (fp(reg_rdata));
	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

/**************used at pwrap_wacs2********************************************/
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_ready(loop_condition_fp fp, unsigned int timeout_us, void *wacs_register,
        unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_ready(loop_condition_fp fp, unsigned int timeout_us,
        unsigned int wacs_register, unsigned int *read_reg)
#endif
{
	unsigned long long start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("timeout when waiting for idle\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
		if (GET_INIT_DONE2(reg_rdata) != WACS_INIT_DONE) {
			PWRAPERR("initialization isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
	} while (fp(reg_rdata));
	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

/******************************************************
 * Function : pwrap_wacs2()
 * Description :
 * Parameter :
 * Return :
 ******************************************************/
signed int pwrap_wacs2(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata)
{
	unsigned int reg_rdata = 0;
	unsigned int wacs_write = 0;
	unsigned int wacs_adr = 0;
	unsigned int wacs_cmd = 0;
	unsigned int return_value = 0;

	/* Check argument validation */
	if ((write & ~(0x1)) != 0)
		return E_PWR_INVALID_RW;
	if ((adr & ~(0xffff)) != 0)
		return E_PWR_INVALID_ADDR;
	if ((wdata & ~(0xffff)) != 0)
		return E_PWR_INVALID_WDAT;

	/* Check IDLE & INIT_DONE in advance */
	return_value =
	    wait_for_state_idle(wait_for_fsm_idle, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA,
	                        PMIC_WRAP_WACS2_VLDCLR, 0);
	if (return_value != 0) {
		PWRAPERR("wait_for_fsm_idle fail,return_value=%d\n", return_value);
		goto FAIL;
	}
	wacs_write = write << 31;
	wacs_adr = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;

	WRAP_WR32(PMIC_WRAP_WACS2_CMD, wacs_cmd);
	if (write == 0) {
		if (rdata == NULL) {
			PWRAPERR("rdata is a NULL pointer\n");
			return_value = E_PWR_INVALID_ARG;
			goto FAIL;
		}
		return_value =
		    wait_for_state_ready(wait_for_fsm_vldclr, TIMEOUT_READ, PMIC_WRAP_WACS2_RDATA,
		                         &reg_rdata);
		if (return_value != 0) {
			PWRAPERR("wait_for_fsm_vldclr fail,return_value=%d\n", return_value);
			return_value += 1;
			goto FAIL;
		}
		*rdata = GET_WACS2_RDATA(reg_rdata);
		WRAP_WR32(PMIC_WRAP_WACS2_VLDCLR, 1);
	}

FAIL:
	if (return_value != 0) {
		PWRAPERR("pwrap_wacs2_hal fail,return_value=%d\n", return_value);
		PWRAPERR("timeout:BUG_ON here\n");
	}

	return return_value;
}


/*********************internal API for pwrap_init***************************/

/**********************************
 * Function : _pwrap_wacs2_nochk()
 * Description :
 * Parameter :
 * Return :
 ***********************************/
signed int pwrap_read_nochk(unsigned int adr, unsigned int *rdata)
{
	return _pwrap_wacs2_nochk(0, adr, 0, rdata);
}

signed int pwrap_write_nochk(unsigned int adr, unsigned int wdata)
{
	return _pwrap_wacs2_nochk(1, adr, wdata, 0);
}

static signed int _pwrap_wacs2_nochk(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata)
{
	unsigned int reg_rdata = 0x0;
	unsigned int wacs_write = 0x0;
	unsigned int wacs_adr = 0x0;
	unsigned int wacs_cmd = 0x0;
	unsigned int return_value = 0x0;

	/* Check argument validation */
	if ((write & ~(0x1)) != 0)
		return E_PWR_INVALID_RW;
	if ((adr & ~(0xffff)) != 0)
		return E_PWR_INVALID_ADDR;
	if ((wdata & ~(0xffff)) != 0)
		return E_PWR_INVALID_WDAT;

	/* Check IDLE */
	return_value =
	    wait_for_state_ready_init(wait_for_fsm_idle, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA, 0);
	if (return_value != 0) {
		PWRAPERR("_pwrap_wacs2_nochk write command fail,return_value=%x\n", return_value);
		return return_value;
	}

	wacs_write = write << 31;
	wacs_adr = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;
	WRAP_WR32(PMIC_WRAP_WACS2_CMD, wacs_cmd);

	if (write == 0) {
		if (rdata == NULL) {
			PWRAPERR("rdata is a NULL pointer\n");
			return_value = E_PWR_INVALID_ARG;
			return return_value;
		}
		return_value =
		    wait_for_state_ready_init(wait_for_fsm_vldclr, TIMEOUT_READ,
		                              PMIC_WRAP_WACS2_RDATA, &reg_rdata);
		if (return_value != 0) {
			PWRAPERR("wait_for_fsm_vldclr fail,return_value=%d\n", return_value);
			return_value += 1;
			return return_value;
		}
		*rdata = GET_WACS2_RDATA(reg_rdata);
		WRAP_WR32(PMIC_WRAP_WACS2_VLDCLR, 1);
	}

	return 0;
}
static void __pwrap_soft_reset(void)
{
	PWRAPLOG("start reset wrapper\n");
	WRAP_WR32(INFRA_GLOBALCON_RST2_SET, 0x1);
	WRAP_WR32(INFRA_GLOBALCON_RST2_CLR, 0x1);
}

static void __pwrap_spi_clk_set(void)
{
	/* toggle PMIC_WRAP and pwrap_spictl reset */
	__pwrap_soft_reset();
	PWRAPLOG("pwrap_spictl reset ok\n");
	WRAP_WR32(CLK_CFG_5_CLR, CLK_SPI_CK_26M);
	/*sys_ck cg enable, turn off clock*/
	WRAP_WR32(MODULE_SW_CG_0_SET, 0x0000000F);
	/* turn on clock*/
	WRAP_WR32(MODULE_SW_CG_0_CLR, 0x0000000F);
	PWRAPLOG("spi clk set ....\n");
}

/************************************************
 * Function : _pwrap_init_dio()
 * Description :call it in pwrap_init,mustn't check init done
 * Parameter :
 * Return :
 ************************************************/
static signed int _pwrap_init_dio(unsigned int dio_en)
{
	unsigned int rdata = 0x0;

	pwrap_write_nochk(PMIC_DEW_DIO_EN_ADDR, dio_en);
#ifdef DUAL_PMICS
	pwrap_write_nochk(EXT_DEW_DIO_EN, dio_en);
#endif

	do {
		rdata = WRAP_RD32(PMIC_WRAP_WACS2_RDATA);
	} while ((GET_WACS2_FSM(rdata) != WACS_FSM_IDLE) || (GET_SYNC_IDLE2(rdata) != WACS_SYNC_IDLE));

#ifndef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_DIO_EN, 0x1);
#else
	WRAP_WR32(PMIC_WRAP_DIO_EN, 0x3);
#endif

	return 0;
}

/***************************************************
 * Function : _pwrap_init_cipher()
 * Description :
 * Parameter :
 * Return :
 ****************************************************/
static signed int _pwrap_init_cipher(void)
{
	unsigned int rdata = 0;
	unsigned int return_value = 0;
	unsigned int start_time_ns = 0, timeout_ns = 0;

	WRAP_WR32(PMIC_WRAP_CIPHER_SWRST, 1);
	WRAP_WR32(PMIC_WRAP_CIPHER_SWRST, 0);
	WRAP_WR32(PMIC_WRAP_CIPHER_KEY_SEL, 1);
	WRAP_WR32(PMIC_WRAP_CIPHER_IV_SEL, 2);
	WRAP_WR32(PMIC_WRAP_CIPHER_EN, 1);
	/* Config CIPHER @ PMIC */
	pwrap_write_nochk(PMIC_DEW_CIPHER_SWRST_ADDR, 0x1);
	pwrap_write_nochk(PMIC_DEW_CIPHER_SWRST_ADDR, 0x0);
	pwrap_write_nochk(PMIC_DEW_CIPHER_KEY_SEL_ADDR, 0x1);
	pwrap_write_nochk(PMIC_DEW_CIPHER_IV_SEL_ADDR,  0x2);
	pwrap_write_nochk(PMIC_DEW_CIPHER_EN_ADDR,  0x1);
	PWRAPLOG("[_pwrap_init_cipher]Config CIPHER of PMIC 0 ok\n");

#ifdef DUAL_PMICS
	/* Config CIPHER of PMIC 1 */
	pwrap_write_nochk(EXT_DEW_CIPHER_SWRST, 0x1);
	pwrap_write_nochk(EXT_DEW_CIPHER_SWRST, 0x0);
	pwrap_write_nochk(EXT_DEW_CIPHER_KEY_SEL, 0x1);
	pwrap_write_nochk(EXT_DEW_CIPHER_IV_SEL,  0x2);
	pwrap_write_nochk(EXT_DEW_CIPHER_EN,  0x1);
	PWRAPLOG("[_pwrap_init_cipher]Config CIPHER of PMIC 1 ok\n");
#endif
	/*wait for cipher data ready@AP */
	return_value = wait_for_state_ready_init(wait_for_cipher_ready, TIMEOUT_WAIT_IDLE, PMIC_WRAP_CIPHER_RDY, 0);
	if (return_value != 0) {
		PWRAPERR("wait for cipher data ready@AP fail,return_value=%x\n", return_value);
		return return_value;
	}
	PWRAPLOG("wait for cipher 0 and 1 to be ready ok\n");

	/* wait for cipher 0 data ready@PMIC */
	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(0xFFFFFF);
	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns))
			PWRAPERR("wait for cipher 0 data ready@PMIC\n");

		pwrap_read_nochk(PMIC_DEW_CIPHER_RDY_ADDR, &rdata);
	} while (rdata != 0x1); /* cipher_ready */

	return_value = pwrap_write_nochk(PMIC_DEW_CIPHER_MODE_ADDR, 0x1);
	if (return_value != 0) {
		PWRAPERR("write DEW_CIPHER_MODE fail,return_value=%x\n", return_value);
		return return_value;
	}
	PWRAPLOG("wait for cipher0 data ready ok\n");

#ifdef DUAL_PMICS
	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(0xFFFFFF);
	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns))
			PWRAPERR("wait for cipher 1 data ready@PMIC\n");
		pwrap_read_nochk(EXT_DEW_CIPHER_RDY, &rdata);
	} while (rdata != 0x1); /* cipher_ready */

	return_value = pwrap_write_nochk(EXT_DEW_CIPHER_MODE, 0x1);
	if (return_value != 0) {
		PWRAPERR("write EXT_DEW_CIPHER_MODE fail,return_value=%x\n", return_value);
		return return_value;
	}
#endif
	PWRAPLOG("wait for cipher 1 data ready@PMIC ok\n");

	/* wait for cipher mode idle */
	return_value = wait_for_state_ready_init(wait_for_idle_and_sync, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA, 0);
	if (return_value != 0) {
		PWRAPERR("wait for cipher mode idle fail,return_value=%x\n", return_value);
		return return_value;
	}
	WRAP_WR32(PMIC_WRAP_CIPHER_MODE, 1);

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("_pwrap_init_cipher,read test error,error code=%x, rdata=%x\n", 1, rdata);
		return E_PWR_READ_TEST_FAIL;
	}

#ifdef DUAL_PMICS
	pwrap_read_nochk(EXT_DEW_READ_TEST, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("_pwrap_init_cipher,read test error,error code=%x, rdata=%x\n", 1, rdata);
		return E_PWR_READ_TEST_FAIL;
	}
#endif

	return 0;
}

static void _pwrap_InitStaUpd(void)
{

#ifndef DUAL_PMICS
#if (MTK_PLATFORM_MT6757_OE2)
	WRAP_WR32(PMIC_WRAP_STAUPD_GRPEN, 0x54);
#else
	WRAP_WR32(PMIC_WRAP_STAUPD_GRPEN, 0xf5);
#endif
#else
	WRAP_WR32(PMIC_WRAP_STAUPD_GRPEN, 0x19f);
#endif
#if 0
	/* CRC */
#ifdef DUAL_PMICS
	pwrap_write_nochk(PMIC_DEW_CRC_EN_ADDR, 0x1);
	WRAP_WR32(PMIC_WRAP_CRC_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, DEW_CRC_VAL);
#else
	pwrap_write_nochk(PMIC_DEW_CRC_EN_ADDR, 0x1);
	pwrap_write_nochk(EXT_DEW_CRC_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_CRC_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, (EXT_DEW_CRC_VAL << 16 | DEW_CRC_VAL));
#endif
#endif
	/* Signature */
#ifndef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_SIG_ADR, PMIC_DEW_CRC_VAL_ADDR);
#else
	WRAP_WR32(PMIC_WRAP_SIG_MODE, 0x3);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, (EXT_DEW_CRC_VAL << 16) | DEW_CRC_VAL);
	WRAP_WR32(PMIC_WRAP_SIG_VALUE, (0x83 << 16) | 0x83);
#endif

	WRAP_WR32(PMIC_WRAP_EINT_STA0_ADR, PMIC_CPU_INT_STA_ADDR);
#ifdef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_EINT_STA1_ADR, EXT_INT_STA);
#endif

	WRAP_WR32(PMIC_WRAP_ADC_CMD_ADDR, PMIC_AUXADC_RQST1_SET_ADDR);
	WRAP_WR32(PMIC_WRAP_PWRAP_ADC_CMD, 0x0100);
	WRAP_WR32(PMIC_WRAP_ADC_RDATA_ADDR, PMIC_AUXADC_ADC_OUT_CH7_BY_GPS_ADDR);

	    WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR_LATEST, PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR_WP, PMIC_AUXADC_MDBG_R_PTR_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR0, PMIC_AUXADC_BUF_OUT_00_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR1, PMIC_AUXADC_BUF_OUT_01_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR2, PMIC_AUXADC_BUF_OUT_02_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR3, PMIC_AUXADC_BUF_OUT_03_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR4, PMIC_AUXADC_BUF_OUT_04_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR5, PMIC_AUXADC_BUF_OUT_05_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR6, PMIC_AUXADC_BUF_OUT_06_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR7, PMIC_AUXADC_BUF_OUT_07_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR8, PMIC_AUXADC_BUF_OUT_08_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR9, PMIC_AUXADC_BUF_OUT_09_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR10, PMIC_AUXADC_BUF_OUT_10_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR11, PMIC_AUXADC_BUF_OUT_11_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR12, PMIC_AUXADC_BUF_OUT_12_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR13, PMIC_AUXADC_BUF_OUT_13_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR14, PMIC_AUXADC_BUF_OUT_14_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR15, PMIC_AUXADC_BUF_OUT_15_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR16, PMIC_AUXADC_BUF_OUT_16_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR17, PMIC_AUXADC_BUF_OUT_17_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR18, PMIC_AUXADC_BUF_OUT_18_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR19, PMIC_AUXADC_BUF_OUT_19_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR20, PMIC_AUXADC_BUF_OUT_20_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR21, PMIC_AUXADC_BUF_OUT_21_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR22, PMIC_AUXADC_BUF_OUT_22_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR23, PMIC_AUXADC_BUF_OUT_23_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR24, PMIC_AUXADC_BUF_OUT_24_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR25, PMIC_AUXADC_BUF_OUT_25_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR26, PMIC_AUXADC_BUF_OUT_26_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR27, PMIC_AUXADC_BUF_OUT_27_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR28, PMIC_AUXADC_BUF_OUT_28_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR29, PMIC_AUXADC_BUF_OUT_29_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR30, PMIC_AUXADC_BUF_OUT_30_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR31, PMIC_AUXADC_BUF_OUT_31_ADDR);

	/* MD ADC Interface */
	WRAP_WR32(PMIC_WRAP_ADC_CMD_ADDR_1, PMIC_AUXADC_RQST1_SET_ADDR);
	WRAP_WR32(PMIC_WRAP_PWRAP_ADC_CMD_1, 0x0400);
	WRAP_WR32(PMIC_WRAP_ADC_RDATA_ADDR_1, PMIC_AUXADC_ADC_OUT_DCXO_BY_GPS_ADDR);

	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR_LATEST_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR_WP_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR0_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR1_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR2_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR3_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR4_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR5_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR6_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR7_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR8_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR9_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR10_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR11_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR12_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR13_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR14_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR15_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR16_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR17_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR18_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR19_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR20_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR21_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR22_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR23_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR24_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR25_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR26_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR27_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR28_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR29_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR30_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_ADC_RDATA_ADDR31_1, PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR);

}

static void _pwrap_starve_set(void)
{
	WRAP_WR32(PMIC_WRAP_HARB_HPRIO, 0x0007);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_0, 0x0402);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_1, 0x0402);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_2, 0x0403);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_3, 0x0414);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_4, 0x0420);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_5, 0x0420);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_6, 0x0420);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_7, 0x0428);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_8, 0x0428);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_9, 0x0417);
#if (MTK_PLATFORM_MT6757_OE2)
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_10, 0x059F);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_11, 0x047C);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_12, 0x0740);
#else
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_10, 0x0740);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_11, 0x0537);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_12, 0x047c);
#endif
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_13, 0x047c);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_14, 0x0740);

}

static void _pwrap_enable(void)
{

#if (MTK_PLATFORM_MT6757_OE2)
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN, 0x1fff);
#else
	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN, 0x7fff);
#endif
	WRAP_WR32(PMIC_WRAP_WACS0_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_WACS1_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_WACS2_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_WACS3_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_STAUPD_PRD, 0x5); /* 100us */
	WRAP_WR32(PMIC_WRAP_WDT_UNIT, 0xf);
	WRAP_WR32(PMIC_WRAP_WDT_SRC_EN, 0xffffffff);
	WRAP_WR32(PMIC_WRAP_TIMER_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_INT0_EN, 0xffffffff);
	WRAP_WR32(PMIC_WRAP_INT1_EN, 0xffffffff);
}

/************************************************
 * Function : _pwrap_init_sistrobe()
 * scription : Initialize SI_CK_CON and SIDLY
 * Parameter :
 * Return :
 ************************************************/
static signed int _pwrap_init_sistrobe(int dual_si_sample_settings)
{
	unsigned int rdata;
	int si_en_sel, si_ck_sel, si_dly, si_sample_ctrl, reserved = 0;
	char result_faulty = 0;
	char found;
	int test_data[30] = {0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669,
	                     0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A,
	                     0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27
	                    };
	int i;
	int error = 0;

	/* TINFO = "[DrvPWRAP_InitSiStrobe] SI Strobe Calibration For PMIC 0............" */
	/* TINFO = "[DrvPWRAP_InitSiStrobe] Scan For The First Valid Sampling Clock Edge......" */
	found = 0;
	for (si_en_sel = 0; si_en_sel < 8; si_en_sel++) {
		for (si_ck_sel = 0; si_ck_sel < 2; si_ck_sel++) {
			si_sample_ctrl = (si_en_sel << 1) | (si_ck_sel);
			WRAP_WR32(PMIC_WRAP_SI_CK_CON, si_sample_ctrl);

			pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
			if (rdata == 0x5aa5) {
				PWRAPLOG("[DrvPWRAP_InitSiStrobe]The First Valid Sampling Clock Edge Is Found !!!\n");
				PWRAPLOG("si_en_sel = %x, si_ck_sel = %x, si_sample_ctrl = %x, rdata = %x\n",
				         si_en_sel, si_ck_sel, si_sample_ctrl, rdata);
				found = 1;
				break;
			}
			PWRAPERR("si_en_sel = %x, si_ck_sel = %x, *PMIC_WRAP_SI_SAMPLE_CTRL = %x, rdata = %x\n",
			         si_en_sel, si_ck_sel, si_sample_ctrl, rdata);
		}
		if (found == 1)
			break;
	}
	if (found == 0) {
		result_faulty |= 0x1;
		PWRAPERR("result_faulty = %d\n", result_faulty);
	}
	if (si_en_sel == 8)
		result_faulty |= 0x2;
	/* TINFO = "[DrvPWRAP_InitSiStrobe] Search For The Data Boundary......" */
	for (si_dly = 0; si_dly < 10; si_dly++) {
		pwrap_write_nochk(PMIC_RG_SPI_DLY_SEL_ADDR, si_dly);

		error = 0;
#ifndef SPEED_UP_PWRAP_INIT
		for (i = 0; i < 30; i++)
#else
		for (i = 0; i < 1; i++)
#endif
		{
			pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, test_data[i]);
			pwrap_read_nochk(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
			if ((rdata & 0x7fff) != (test_data[i] & 0x7fff)) {
				PWRAPERR("[DrvPWRAP_InitSiStrobe] (%x, %x, %x) Data Boundary Is Found !!\n",
				         si_dly, reserved, rdata);
				PWRAPERR("[DrvPWRAP_InitSiStrobe] (%x, %x, %x) Data Boundary Is Found !!\n",
				         si_dly, si_dly, rdata);
				error = 1;
				break;
			}
		}
		if (error == 1)
			break;
		PWRAPLOG("si_dly = %x, *PMIC_WRAP_RESERVED = %x, rdata = %x\n",
		         si_dly, reserved, rdata);
		PWRAPLOG("si_dly = %x, *RG_SPI_CON2 = %x, rdata = %x\n",
		         si_dly, si_dly, rdata);
	}

	/* TINFO = "[DrvPWRAP_InitSiStrobe] Change The Sampling Clock Edge To The Next One." */
	/*elbrus si_sample_ctrl = (((si_en_sel << 1) | si_ck_sel) + 1) << 2;*/
	si_sample_ctrl = si_sample_ctrl + 0x1;
	WRAP_WR32(PMIC_WRAP_SI_CK_CON, si_sample_ctrl);
	if (si_dly == 10) {
		PWRAPLOG("SI Strobe Calibration For PMIC 0 Done, (%x, si_dly%x)\n", si_sample_ctrl, si_dly);
		si_dly--;
	}
	PWRAPLOG("SI Strobe Calibration For PMIC 0 Done, (%x, %x)\n", si_sample_ctrl, reserved);
	PWRAPLOG("SI Strobe Calibration For PMIC 0 Done, (%x, %x)\n", si_sample_ctrl, si_dly);


	if (result_faulty != 0)
		return result_faulty;

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("[_pwrap_init_dio] Read Test Failed, DEW_READ_TEST rdata = %x, exp = 0x5aa5\n",
		         rdata);
		return 0x10;
	}

	return 0;
}

static int __pwrap_InitSPISLV(void)
{
	pwrap_write_nochk(MT6355_FILTER_CON0, 0xf); /* turn on IO filter function */
	pwrap_write_nochk(PMIC_TOP_CKHWEN_CON0_SET_ADDR, 0x80); /* turn on SPI slave DCM */
	pwrap_write_nochk(MT6355_SMT_CON1, 0xf); /* turn on IO SMT function to improve noise immunity */
	pwrap_write_nochk(PMIC_GPIO_PULLEN0_CLR_ADDR, 0x3c0); /* turn off IO pull function for power saving */
	pwrap_write_nochk(MT6355_RG_SPI_CON0, 0x1); /* turn off IO pull function for power saving */
#ifdef DUAL_PMICS
	pwrap_write_nochk(EXT_FILTER_CON0, 0xf); /* turn on IO filter function */
	pwrap_write_nochk(EXT_TOP_CKHWEN_CON0_SET, 0x80); /* turn on SPI slave DCM */
	pwrap_write_nochk(EXT_SMT_CON1, 0xf); /* turn on IO SMT function to improve noise immunity */
	pwrap_write_nochk(EXT_RG_SPI_CON, 0x1); /* turn off IO pull function for power saving */
#endif

	return 0;
}

/******************************************************
 * Function : _pwrap_reset_spislv()
 * Description :
 * Parameter :
 * Return :
 ******************************************************/
static signed int _pwrap_reset_spislv(void)
{
	unsigned int ret = 0;
	unsigned int return_value = 0;

	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN, DISABLE_ALL);
	WRAP_WR32(PMIC_WRAP_WRAP_EN, DISABLE);
	WRAP_WR32(PMIC_WRAP_MUX_SEL, MANUAL_MODE);
	WRAP_WR32(PMIC_WRAP_MAN_EN, ENABLE);
	WRAP_WR32(PMIC_WRAP_DIO_EN, DISABLE);

	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_CSL << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_CSH << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	return_value =
	    wait_for_state_ready_init(wait_for_sync, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA, 0);

	if (return_value != 0) {
		PWRAPERR("_pwrap_reset_spislv fail,return_value=%x\n", return_value);
		ret = E_PWR_TIMEOUT;
		goto timeout;
	}

	WRAP_WR32(PMIC_WRAP_MAN_EN, DISABLE);
	WRAP_WR32(PMIC_WRAP_MUX_SEL, WRAPPER_MODE);

timeout:
	WRAP_WR32(PMIC_WRAP_MAN_EN, DISABLE);
	WRAP_WR32(PMIC_WRAP_MUX_SEL, WRAPPER_MODE);
	return ret;
}

static signed int _pwrap_init_reg_clock(unsigned int regck_sel)
{
	unsigned int rdata;

#ifndef SLV_CLK_1M
#ifndef DUAL_PMICS
	/* Set Read Dummy Cycle Number (Slave Clock is 18MHz) */
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x8, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x8);
	PWRAPLOG("NO_SLV_CLK_1M Set Read Dummy Cycle\n");
#else
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x8, &rdata);
	_pwrap_wacs2_nochk(1, EXT_DEW_RDDMY_NO, 0x8, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x0808);
	PWRAPLOG("NO_SLV_CLK_1M Set Read Dummy Cycle dual_pmics\n");
#endif
#else
#ifndef DUAL_PMICS
	/* Set Read Dummy Cycle Number (Slave Clock is 1MHz) */
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x68, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x68);
	PWRAPLOG("SLV_CLK_1M Set Read Dummy Cycle\n");
#else
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x68, &rdata);
	_pwrap_wacs2_nochk(1, EXT_DEW_RDDMY_NO, 0x68, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x6868);
	PWRAPLOG("SLV_CLK_1M Set Read Dummy Cycle dual_pmics\n");
#endif
#endif

	/* Config SPI Waveform according to reg clk */
	if (regck_sel == 1) { /* Slave Clock is 18MHz */
		/* wait data written into register => 4T_PMIC:
		 * CSHEXT_WRITE_START+EXT_CK+CSHEXT_WRITE_END+CSLEXT_START
		 */
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE, 0x22);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ, 0x0);
		WRAP_WR32(PMIC_WRAP_CSLEXT_START, 0x0);
		WRAP_WR32(PMIC_WRAP_CSLEXT_END, 0x2);
	} else { /*Safe Mode*/
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE, 0xff);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ, 0xff);
		WRAP_WR32(PMIC_WRAP_CSLEXT_START, 0xf);
		WRAP_WR32(PMIC_WRAP_CSLEXT_END, 0xf);
	}

	return 0;
}

static int _pwrap_wacs2_write_test(int pmic_no)
{
	unsigned int rdata;

	if (pmic_no == 0) {
		pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0xa55a);
		pwrap_read_nochk(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
		if (rdata != 0xa55a) {
			PWRAPERR("Error: Write Test Failed. DEW_WRITE_TEST rdata = %x, exp = 0xa55a\n", rdata);
			return E_PWR_WRITE_TEST_FAIL;
		}
	}

#ifdef DUAL_PMICS
	if (pmic_no == 1) {
		pwrap_write_nochk(EXT_DEW_WRITE_TEST, 0xa55a);
		pwrap_read_nochk(EXT_DEW_WRITE_TEST, &rdata);
		if (rdata != 0xa55a) {
			PWRAPERR("Error: Write Test Failed. EXT_DEW_WRITE_TEST rdata = %x, exp = 0xa55a\n", rdata);
			return E_PWR_WRITE_TEST_FAIL;
		}
	}
#endif

	return 0;
}

static unsigned int pwrap_read_test(void)
{
	unsigned int rdata = 0;
	unsigned int return_value = 0;
	/* Read Test */
	return_value = pwrap_wacs2_read(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("Read Test fail,rdata=0x%x, exp=0x5aa5,return_value=0x%x\n", rdata, return_value);
		return E_PWR_READ_TEST_FAIL;
	}
	PWRAPLOG("Read Test pass,return_value=%d\n", return_value);

	return 0;
}
static unsigned int pwrap_write_test(void)
{
	unsigned int rdata = 0;
	unsigned int sub_return = 0;
	unsigned int sub_return1 = 0;

	/* Write test using WACS2 */
	PWRAPLOG("start pwrap_write\n");
	sub_return = pwrap_wacs2_write(PMIC_DEW_WRITE_TEST_ADDR, DEFAULT_VALUE_READ_TEST);
	PWRAPLOG("after pwrap_write\n");
	sub_return1 = pwrap_wacs2_read(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
	if ((rdata != DEFAULT_VALUE_READ_TEST) || (sub_return != 0) || (sub_return1 != 0)) {
		PWRAPERR("write test error,rdata=0x%x,exp=0xa55a,sub_return=0x%x,sub_return1=0x%x\n", rdata, sub_return,
		         sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}
	PWRAPLOG("write Test pass\n");

	return 0;
}
static void pwrap_ut(unsigned int ut_test)
{
	switch (ut_test) {
		case 1:
			pwrap_write_test();
			break;
		case 2:
			pwrap_read_test();
			break;
		case 3:
#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
			pwrap_wacs2_ipi(0x10010000 + 0xD8, 0xffffffff, (WRITE_CMD | WRITE_PMIC_WRAP));
			break;
#endif
		default:
			PWRAPLOG("default test.\n");
			break;
	}
}
static void _pwrap_set_DCM(void)
{
	signed int sub_return = 0;
#ifndef PWRAP_DCM_ALL_ON
	WRAP_WR32(PMIC_WRAP_DCM_EN, 0x3);
#else
	WRAP_WR32(PMIC_WRAP_DCM_EN, 0x3fff);
#endif
	/* no debounce */
	WRAP_WR32(PMIC_WRAP_DCM_DBC_PRD, DISABLE);
#if 0
	/* had config at DCM_GROUP, default is disable */

	/* 0x10001074 bit[22] = 1 b0 to disable pmic clock DCM */
	sub_return = WRAP_RD32(PMIC_CLOCK_DCM);
	PWRAPLOG("PMIC_CLOCK_DCM=0x%x\n", sub_return);
	WRAP_WR32(PMIC_CLOCK_DCM, (sub_return & 0xFFBFFFFF));
#endif
	/* 0x10001F0C bit[25] = 1 b1 to disable apb clock gating mechanism. */
	sub_return = WRAP_RD32(APB_CLOCK_GATING);
	PWRAPLOG("APB_CLOCK_GATING=0x%x\n", sub_return);
	WRAP_WR32(APB_CLOCK_GATING, ((sub_return & 0xFDFFFFFF) | 0x02000000));
	PWRAPLOG("PMIC_CLOCK_DCM=0x%x APB_CLOCK_GATING=0x%x ,enable DCM ok\n",
		WRAP_RD32(PMIC_CLOCK_DCM), WRAP_RD32(APB_CLOCK_GATING));
}


signed int pwrap_init(void)
{
	signed int sub_return = 0;
	unsigned int rdata = 0x0;

	PWRAPLOG("pwrap_init start!!!!!!!!!!!!!\n");

	__pwrap_spi_clk_set();

	PWRAPLOG("__pwrap_spi_clk_set ok\n");

	/* Enable DCM */
	_pwrap_set_DCM();
	PWRAPLOG("Enable DCM ok\n");

	/* Reset SPISLV */
	sub_return = _pwrap_reset_spislv();
	if (sub_return != 0) {
		PWRAPERR("error,_pwrap_reset_spislv fail,sub_return=%x\n", sub_return);
		return E_PWR_INIT_RESET_SPI;
	}
	PWRAPLOG("Reset SPISLV ok\n");

	/* Enable WRAP */
	WRAP_WR32(PMIC_WRAP_WRAP_EN, ENABLE);
	PWRAPLOG("Enable WRAP  ok\n");

	WRAP_WR32(PMIC_WRAP_HIPRIO_ARB_EN, WACS2); /* ONLY WACS2 */
	/* Enable WACS2 */
	WRAP_WR32(PMIC_WRAP_WACS2_EN, ENABLE);

	PWRAPLOG("Enable WACSW2  ok\n");

	/* SPI Slave Configuration */
	sub_return = __pwrap_InitSPISLV();
	if (sub_return != 0) {
		PWRAPERR("Error: DrvPWRAP_InitSPISLV Failed, sub_return = %x", sub_return);
		return -1;
	}

	/* SPI Waveform Configuration. 0:safe mode, 1:18MHz */
	sub_return = _pwrap_init_reg_clock(1);
	if (sub_return != 0) {
		PWRAPERR("error,_pwrap_init_reg_clock fail,sub_return=%x\n", sub_return);
		return E_PWR_INIT_REG_CLOCK;
	}
	PWRAPLOG("_pwrap_init_reg_clock ok\n");

	/* Enable DIO mode */
	sub_return = _pwrap_init_dio(1);
	if (sub_return != 0) {
		PWRAPERR("_pwrap_init_dio test error,error code=%x, sub_return=%x\n", 0x11, sub_return);
		return E_PWR_INIT_DIO;
	}
	PWRAPLOG("_pwrap_init_dio ok\n");

	/* Input data calibration flow; */
	sub_return = _pwrap_init_sistrobe(0);
	if (sub_return != 0) {
		PWRAPERR("error,DrvPWRAP_InitSiStrobe fail,sub_return=%x\n", sub_return);
		return E_PWR_INIT_SIDLY;
	}
	PWRAPLOG("_pwrap_init_sistrobe ok\n");

	/* Enable Encryption */
	sub_return = _pwrap_init_cipher();
	if (sub_return != 0) {
		PWRAPERR("Enable Encryption fail, return=%x\n", sub_return);
		return E_PWR_INIT_CIPHER;
	}
	PWRAPLOG("_pwrap_init_cipher ok\n");

	/*  Write test using WACS2.  check Wtiet test default value */
	sub_return = _pwrap_wacs2_write_test(0);
	if (rdata != 0) {
		PWRAPERR("[_pwrap_wacs2_write_test(0)] test fail\n");
		return E_PWR_INIT_WRITE_TEST;
	}
#ifdef DUAL_PMICS
	sub_return = _pwrap_wacs2_write_test(1);
	if (rdata != 0) {
		PWRAPERR("[_pwrap_wacs2_write_test(1)] test fail\n");
		return E_PWR_INIT_WRITE_TEST;
	}
#endif
	PWRAPLOG("_pwrap_wacs2_write_test ok\n");

	/* Status update function initialization
	* 1. Signature Checking using CRC (CRC 0 only)
	* 2. EINT update
	* 3. Read back Auxadc thermal data for GPS
	*/
	_pwrap_InitStaUpd();
	PWRAPLOG("_pwrap_InitStaUpd ok\n");

#if (MTK_PLATFORM_MT6757_OE2)
	/* PMIC WRAP priority adjust */
	WRAP_WR32(PMIC_WRAP_PRIORITY_USER_SEL_0, 0x6543C210);
	WRAP_WR32(PMIC_WRAP_PRIORITY_USER_SEL_1, 0xFEDBA987);
	WRAP_WR32(PMIC_WRAP_ARBITER_OUT_SEL_0, 0x87654210);
	WRAP_WR32(PMIC_WRAP_ARBITER_OUT_SEL_1, 0xFED3CBA9);
#else
	/* PMIC WRAP priority adjust */
	WRAP_WR32(PMIC_WRAP_PRIORITY_USER_SEL_0, 0x6543C210);
	WRAP_WR32(PMIC_WRAP_PRIORITY_USER_SEL_1, 0xDBFA9E87);
	WRAP_WR32(PMIC_WRAP_ARBITER_OUT_SEL_0, 0x87654210);
	WRAP_WR32(PMIC_WRAP_ARBITER_OUT_SEL_1, 0xDAF3ECB9);
#endif
	/* PMIC_WRAP starvation setting */
	_pwrap_starve_set();
	PWRAPLOG("_pwrap_starve_set ok\n");

	/* PMIC_WRAP enables */
	_pwrap_enable();
	PWRAPLOG("_pwrap_enable ok\n");

	/* Backward Compatible Settings */
	/* WRAP_WR32(PMIC_WRAP_BWC_OPTIONS,  WRAP_RD32(PMIC_WRAP_BWC_OPTIONS) | 0x8); */

	/* Initialization Done */
	WRAP_WR32(PMIC_WRAP_INIT_DONE0, 0x1);
	WRAP_WR32(PMIC_WRAP_INIT_DONE1, 0x1);
	WRAP_WR32(PMIC_WRAP_INIT_DONE2, 0x1);
	WRAP_WR32(PMIC_WRAP_INIT_DONE3, 0x1);

	PWRAPLOG("pwrap_init Done!!!!!!!!!\n");

	/* WACS2 UT */
	pwrap_ut(1);
	pwrap_ut(2);
	PWRAPLOG("channel pass \n\r ");
#if (PMIC_WRAP_PRELOADER) && (MTK_PLATFORM_MT6799)
	unsigned int rgdata=0;
	pwrap_read(PPCCTL0, &rgdata);
	rgdata |= 0x1;
	pwrap_write(PPCCTL0, rgdata);
	pwrap_read(PPCCTL0, &rgdata);
	PWRAPLOG("PMIC_RG_PWRHOLD=%x(should be 1)\n", rgdata);
#endif
	return 0;
}

/*-------------------pwrap debug---------------------*/
static inline void pwrap_dump_ap_register(void)
{
	unsigned int i = 0;
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
	unsigned int *reg_addr;
#else
	unsigned int reg_addr;
#endif
	unsigned int reg_value = 0;

	PWRAPERR("dump pwrap register\n");
	for (i = 0; i <= PMIC_WRAP_REG_RANGE; i++) {
		reg_addr = (PMIC_WRAP_BASE + i * 4);
#if (PMIC_WRAP_KERNEL)
		reg_value = WRAP_RD32(((unsigned int *) (PMIC_WRAP_BASE + i * 4)));
#else
		reg_value = WRAP_RD32(reg_addr);
#endif
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		PWRAPERR("reg_addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		PWRAPERR("reg_addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}
}

void pwrap_dump_all_register(void)
{
	pwrap_dump_ap_register();
}

static int is_pwrap_init_done(void)
{
	int ret = 0;

	ret = WRAP_RD32(PMIC_WRAP_INIT_DONE2);
	PWRAPLOG("is_pwrap_init_done %d\n", ret);
	if ((ret & 0x1) == 1)
		return 0;
	return -1;
}

signed int pwrap_init_lk(void)
{
	unsigned int pwrap_ret = 0, i = 0;

	PWRAPFUC();
	if (0 == is_pwrap_init_done()) {
		PWRAPLOG("[PMIC_WRAP]wrap_init already init, do nothing\n");
		return 0;
	}
	for (i = 0; i < 3; i++) {
		pwrap_ret = pwrap_init();
		if (pwrap_ret != 0) {
			PWRAPERR("[PMIC_WRAP]wrap_init fail,the return value=%x.\n",pwrap_ret);
			if (i >= 2)
				ASSERT(0);
		} else {
			PWRAPLOG("[PMIC_WRAP]wrap_init pass,the return value=%x.\n",pwrap_ret);
			break;
		}
	}

	return 0;
}

#endif /*endif PMIC_WRAP_NO_PMIC */
