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

#ifndef __MT_I2C_H__
#define __MT_I2C_H__
#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <debug.h>
#include <platform/sync_write.h>
#include <sys/types.h>

#define I2C_NR      8 /* Number of I2C controllers */

#define I2CTAG                "[I2C-LK] "
#define I2CLOG(fmt, arg...)   dprintf(INFO,I2CTAG fmt, ##arg)
#define I2CMSG(fmt, arg...)   dprintf(SPEW,I2CTAG fmt, ##arg)
#define I2CERR(fmt, arg...)   dprintf(ALWAYS,I2CTAG "%d: "fmt, __LINE__, ##arg)
#define I2CFUC()   dprintf(ALWAYS,I2CTAG "%s\n", __FUNCTION__)

#ifdef MACH_FPGA
#define CONFIG_MT_I2C_FPGA_ENABLE
#endif

//#define I2C_DRIVER_IN_KERNEL
#ifdef I2C_DRIVER_IN_KERNEL
#define I2C_MB() mb()
//#define I2C_DEBUG
#ifdef I2C_DEBUG
#define I2C_BUG_ON(a) BUG_ON(a)
#else
#define I2C_BUG_ON(a)
#endif

#else
#ifdef CONFIG_MT_I2C_FPGA_ENABLE
#define FPGA_CLOCK      12000 /* FPGA crystal frequency (KHz) */
#define I2C_CLK_DIV     (5) /* frequency divisor */
#define I2C_CLK_RATE        (FPGA_CLOCK / I2C_CLK_DIV) /* I2C base clock (KHz) */
#define SCP_I2C_CLK     (FPGA_CLOCK / 16)
#else
#define I2C_CLK_RATE    13650 /* TODO: Calculate from bus clock */
#define I2C_CLK_DIV     (5) /* frequency divisor */
#define SCP_I2C_CLK     (26000 / 2) /* TODO: Check the correct frequency */
#endif

#define I2C_MB()
#define I2C_BUG_ON(a)
#define I2C_M_RD       0x0001
#endif

#ifndef I2C0_BASE
#define I2C0_BASE (0x11007000)
#endif
#ifndef I2C1_BASE
#define I2C1_BASE (0x11014000)
#endif
#ifndef I2C2_BASE
#define I2C2_BASE (0x11015000)
#endif
#ifndef I2C3_BASE
#define I2C3_BASE (0x1100F000)
#endif
#ifndef I2C4_BASE
#define I2C4_BASE (0x11011000)
#endif
#ifndef I2C5_BASE
#define I2C5_BASE (0x11017000)
#endif
#ifndef I2C6_BASE
#define I2C6_BASE (0x11005000)
#endif
#ifndef I2C7_BASE
#define I2C7_BASE (0x11008000)
#endif
#ifndef I2C8_BASE
#define I2C8_BASE (0x11009000)
#endif
#ifndef I2C9_BASE
#define I2C9_BASE (0x11016000)
#endif

#define I2C0_APDMA_BASE (0x11000100)
#define I2C1_APDMA_BASE (0x11000380)
#define I2C2_APDMA_BASE (0x11000400)
#define I2C3_APDMA_BASE (0x11000280)
#define I2C4_APDMA_BASE (0x11000300)
#define I2C5_APDMA_BASE (0x11000500)
#define I2C6_APDMA_BASE (0x11000580)
#define I2C7_APDMA_BASE (0x11000180)
#define I2C8_APDMA_BASE (0x11000200)
#define I2C9_APDMA_BASE (0x11000480)

#define I2C_OK                              0x0000
#define EAGAIN_I2C                          11  /* Try again */
#define EINVAL_I2C                          22  /* Invalid argument */
#define EOPNOTSUPP_I2C                      95  /* Operation not supported on transport endpoint */
#define ETIMEDOUT_I2C                       110 /* Connection timed out */
#define EREMOTEIO_I2C                       121 /* Remote I/O error */
#define ENOTSUPP_I2C                        524 /* Remote I/O error */
#define I2C_WRITE_FAIL_HS_NACKERR           0xA013
#define I2C_WRITE_FAIL_ACKERR               0xA014
#define I2C_WRITE_FAIL_TIMEOUT              0xA015

//#define I2C_CLK_WRAPPER_RATE    36000   /* kHz for wrapper I2C work frequency */

/******************************************register operation***********************************/
enum I2C_REGS_OFFSET {
	OFFSET_DATA_PORT      = 0x0,    //0x0
	OFFSET_SLAVE_ADDR     = 0x04,   //0x04
	OFFSET_INTR_MASK      = 0x08,   //0x08
	OFFSET_INTR_STAT      = 0x0C,   //0x0C
	OFFSET_CONTROL        = 0x10,   //0X10
	OFFSET_TRANSFER_LEN   = 0x14,   //0X14
	OFFSET_TRANSAC_LEN    = 0x18,   //0X18
	OFFSET_DELAY_LEN      = 0x1C,   //0X1C
	OFFSET_TIMING         = 0x20,   //0X20
	OFFSET_START          = 0x24,   //0X24
	OFFSET_EXT_CONF       = 0x28,
	OFFSET_LTIMING        = 0x2C,   //0X2C
	OFFSET_FIFO_STAT      = 0x30,   //0X30
	OFFSET_FIFO_THRESH    = 0x34,   //0X34
	OFFSET_FIFO_ADDR_CLR  = 0x38,   //0X38
	OFFSET_IO_CONFIG      = 0x40,   //0X40
	OFFSET_RSV_DEBUG      = 0x44,   //0X44
	OFFSET_HS             = 0x48,   //0X48
	OFFSET_SOFTRESET      = 0x50,   //0X50
	OFFSET_DCM_EN         = 0x54,   //0X54
	OFFSET_DEBUGSTAT      = 0x64,   //0X64
	OFFSET_DEBUGCTRL      = 0x68,   //0x68
	OFFSET_TRANSFER_LEN_AUX      = 0x6C,   //0x6C
	OFFSET_CLOCK_DIV      = 0x70
};

#define I2C_HS_NACKERR            (1 << 2)
#define I2C_ACKERR                (1 << 1)
#define I2C_TRANSAC_COMP          (1 << 0)

#define I2C_FIFO_SIZE             8

#define MAX_ST_MODE_SPEED         100  /* khz */
#define MAX_FS_MODE_SPEED         400  /* khz */
#define MAX_HS_MODE_SPEED         3400 /* khz */

#define MAX_DMA_TRANS_SIZE        65532 /* Max(65535) aligned to 4 bytes = 65532 */
#define MAX_DMA_TRANS_NUM         256

#define MAX_SAMPLE_CNT_DIV        8
#define MAX_STEP_CNT_DIV          64
#define MAX_HS_STEP_CNT_DIV       8

#define DUTY_CYCLE                45

#define DMA_ADDRESS_HIGH          (0xC0000000)

enum DMA_REGS_OFFSET {
	OFFSET_INT_FLAG       = 0x0,
	OFFSET_INT_EN         = 0x04,
	OFFSET_EN             = 0x08,
	OFFSET_RST            = 0x0C,
	OFFSET_CON            = 0x18,
	OFFSET_TX_MEM_ADDR    = 0x1C,
	OFFSET_RX_MEM_ADDR    = 0x20,
	OFFSET_TX_LEN         = 0x24,
	OFFSET_RX_LEN         = 0x28
};

enum i2c_trans_st_rs {
	I2C_TRANS_STOP = 0,
	I2C_TRANS_REPEATED_START
};

typedef enum {
	ST_MODE,
	FS_MODE,
	HS_MODE
} I2C_SPEED_MODE;

enum mt_trans_op {
	I2C_MASTER_NONE = 0,
	I2C_MASTER_WR = 1,
	I2C_MASTER_RD,
	I2C_MASTER_WRRD
};

//CONTROL
#define I2C_CONTROL_RS          (0x1 << 1)
#define I2C_CONTROL_DMA_EN      (0x1 << 2)
#define I2C_CONTROL_CLK_EXT_EN      (0x1 << 3)
#define I2C_CONTROL_DIR_CHANGE      (0x1 << 4)
#define I2C_CONTROL_ACKERR_DET_EN   (0x1 << 5)
#define I2C_CONTROL_TRANSFER_LEN_CHANGE (0x1 << 6)
#define I2C_CONTROL_WRAPPER          (0x1 << 0)
/***********************************end of register operation****************************************/
/***********************************I2C Param********************************************************/
struct mt_trans_data {
	U16 trans_num;
	U16 data_size;
	U16 trans_len;
	U16 trans_auxlen;
};

typedef struct mt_i2c_t {
#ifdef I2C_DRIVER_IN_KERNEL
	//==========only used in kernel================//
	struct i2c_adapter  adap;   /* i2c host adapter */
	struct device   *dev;   /* the device object of i2c host adapter */
	atomic_t          trans_err;  /* i2c transfer error */
	atomic_t          trans_comp; /* i2c transfer completion */
	atomic_t          trans_stop; /* i2c transfer stop */
	spinlock_t        lock;   /* for mt_i2c struct protection */
	wait_queue_head_t wait;   /* i2c transfer wait queue */
#endif
	//==========set in i2c probe============//
	U32      base;    /* i2c base addr */
	U16      id;
	U16      irqnr;    /* i2c interrupt number */
	U16      irq_stat; /* i2c interrupt status */
	U32      clk;     /* host clock speed in khz */
	U32      pdn;     /*clock number*/
	//==========common data define============//
	enum     i2c_trans_st_rs st_rs;
	enum     mt_trans_op op;
	U32      pdmabase;
	U32      speed;   //The speed (khz)
	U16      delay_len;    //number of half pulse between transfers in a trasaction
	U32      msg_len;    //number of half pulse between transfers in a trasaction
	U8       *msg_buf;    /* pointer to msg data      */
	U8       addr;      //The address of the slave device, 7bit,the value include read/write bit.
	U8       master_code;/* master code in HS mode */
	U8       mode;    /* ST/FS/HS mode */
	//==========reserved funtion============//
	U8       is_push_pull_enable; //IO push-pull or open-drain
	U8       is_clk_ext_disable;   //clk entend default enable
	U8       is_dma_enabled;   //Transaction via DMA instead of 8-byte FIFO
	U8       read_flag;//read,write,read_write
	BOOL     dma_en;
	BOOL     poll_en;
	BOOL     pushpull;//open drain
	BOOL     filter_msg;//filter msg error log
	BOOL     i2c_3dcamera_flag;//flag for 3dcamera

	//==========define reg============//
	U16      timing_reg;
	U16      ltiming_reg;
	U16      high_speed_reg;
	U16      control_reg;
	struct   mt_trans_data trans_data;
} mt_i2c;

//===========================i2c old driver===================================================//
#define I2C_APPM I2C5
enum {
	I2C0 = 0,
	I2C1 = 1,
	I2C2 = 2,
	I2C3 = 3,
	I2C4 = 4,
	I2C5 = 5,
	I2C6 = 6,
	I2C7 = 7,
	I2C8 = 8,
	I2C9 = 9
};

//==============================================================================
// I2C Exported Function
//==============================================================================
extern S32 i2c_read(mt_i2c *i2c,U8 *buffer, U32 len);
extern S32 i2c_write(mt_i2c *i2c,U8  *buffer, U32 len);
extern S32 i2c_write_read(mt_i2c *i2c,U8 *buffer, U32 write_len, U32 read_len);
extern S32 i2c_set_speed(mt_i2c *i2c);
extern int i2c_hw_init(void);

//#define I2C_EARLY_PORTING
#ifdef I2C_EARLY_PORTING
int mt_i2c_test(void);
#endif
#endif /* __MT_I2C_H__ */
