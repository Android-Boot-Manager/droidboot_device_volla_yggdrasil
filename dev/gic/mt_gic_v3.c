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
#include <reg.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_irq.h>
#include <mt_gic.h>

#include <debug.h>

#define GICD_CTLR_RWP       (1 << 31)
#define GICD_CTLR_ARE       (1 << 4)
#define GICD_CTLR_ENGRP1S   (1 << 2)
#define GICD_CTLR_ENGRP1NS  (1 << 1)
#define GICR_WAKER_ProcessorSleep   (1 << 1)
#define GICR_WAKER_ChildrenAsleep   (1 << 2)

extern void dsb(void);
extern void isb(void);
extern uint32_t mt_interrupt_needed_for_secure(void);
extern uint64_t mt_irq_get_affinity(void);

static void mt_gic_icc_primask_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c4, c6, 0" :: "r" (reg));
}

static uint32_t mt_gic_icc_primask_read(void)
{
	uint32_t reg;

	__asm__ volatile("MRC p15, 0, %0, c4, c6, 0" : "=r" (reg));

	return reg;
}

static void mt_gic_icc_igrpen1_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c12, c12, 7" :: "r" (reg));
}

static uint32_t mt_gic_icc_igrpen1_read(void)
{
	uint32_t reg;

	__asm__ volatile("MRC p15, 0, %0, c12, c12, 7" : "=r" (reg));

	return reg;
}

static uint32_t mt_gic_icc_iar1_read(void)
{
	uint32_t reg;

	__asm__ volatile("MRC p15, 0, %0, c12, c12, 0" : "=r" (reg));

	return reg;
}

static void mt_gic_icc_msre_write(void)
{
	uint32_t reg;

#define MON_MODE    "#22"
#define SVC_MODE    "#19"

	/*
	 * switch to monitor mode and mark ICC_MSRE.
	 */
	__asm__ volatile("CPS " MON_MODE "\n"
	                 "MRC p15, 6, %0, c12, c12, 5\n"
	                 "ORR %0, %0, #9\n"
	                 "MCR p15, 6, %0, c12, c12, 5\n"
	                 "CPS " SVC_MODE "\n" : "=r" (reg));

	dsb();
}

static void mt_gic_icc_sre_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c12, c12, 5" :: "r" (reg));
	dsb();
}

static uint32_t mt_gic_icc_sre_read(void)
{
	uint32_t reg;

	__asm__ volatile("MRC p15, 0, %0, c12, c12, 5" : "=r" (reg));

	return reg;
}

static void mt_gic_icc_eoir1_write(uint32_t reg)
{
	__asm__ volatile("MCR p15, 0, %0, c12, c12, 1" :: "r" (reg));
}

uint32_t mt_mpidr_read(void)
{
	uint32_t reg;

	__asm__ volatile("MRC p15, 0, %0, c0, c0, 5" : "=r" (reg));

	return reg;
}

#ifdef GIC600
/* GIC600-specific accessor functions */
static void gicr_write_pwrr(uintptr_t base, unsigned int val)
{
	DRV_WriteReg32(base + GICR_PWRR, val);
}

static uint32_t gicr_read_pwrr(uintptr_t base)
{
	return DRV_Reg32(base + GICR_PWRR);
}

static void gic600_rdistif_init(void)
{
	unsigned int rdist_base = GIC_REDIS_BASE;
	unsigned int ret;

	do {
		/* Check group not transitioning (polling for PWRR_RDGPO == PWRR_RDGPD) */
		ret = gicr_read_pwrr(rdist_base);
		while (((ret & PWRR_RDGPD) >> PWRR_RDGPD_SHIFT)
				!= ((ret & PWRR_RDGPO) >> PWRR_RDGPO_SHIFT))
			ret = gicr_read_pwrr(rdist_base);

		/* Power on redistributor */
		gicr_write_pwrr(rdist_base, PWRR_ON);

		/* Keep retrying until the power on state is reflected (PWRR_RDGPO == 0)*/
	} while (gicr_read_pwrr(rdist_base) & PWRR_RDGPO);
}
#endif

static void mt_gic_cpu_init(void)
{
	mt_gic_icc_sre_write(0x01);
	mt_gic_icc_primask_write(0xF0);
	mt_gic_icc_igrpen1_write(0x01);
	dsb();
}

static void mt_gic_redist_init(void)
{
	unsigned int value;

#ifdef GIC600
	gic600_rdistif_init();
#endif

	/* Wake up this CPU redistributor */
	value = DRV_Reg32(GIC_REDIS_BASE + GIC_REDIS_WAKER);
	value &= ~GICR_WAKER_ProcessorSleep;
	DRV_WriteReg32(GIC_REDIS_BASE + GIC_REDIS_WAKER, value);

	while (DRV_Reg32(GIC_REDIS_BASE + GIC_REDIS_WAKER) & GICR_WAKER_ChildrenAsleep);
}

static void mt_git_dist_rwp(void)
{
	/*
	 * check GICD_CTLR.RWP for done check
	 */
	while (DRV_Reg32(GIC_DIST_BASE + GIC_DIST_CTRL) & GICD_CTLR_RWP) {

	}
}

static void mt_gic_dist_init(void)
{
	unsigned int i;
	uint64_t affinity;

	affinity = mt_irq_get_affinity();

	DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_CTRL, GICD_CTLR_ARE);

	mt_git_dist_rwp();

	/*
	 * Set all global interrupts to be level triggered, active low.
	 */
	for (i = 32; i < (MT_NR_SPI + 32); i += 16) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_CONFIG + i * 4 / 16, 0);
	}

	/*
	 * Set all global interrupts to this CPU only.
	 */
	for (i = 0; i < MT_NR_SPI; i++) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ROUTE + i * 8, (affinity & 0xFFFFFFFF));
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ROUTE + i * 8 + 4, (affinity >> 32));
	}

	/*
	 * Set all interrupts to G1S.  Leave the PPI and SGIs alone
	 * as they are set by redistributor registers.
	 */
	for (i = 0; i < NR_IRQ_LINE; i += 32)
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_IGRPMODR + i / 8, 0xFFFFFFFF);

	/*
	 * Set priority on all interrupts.
	 */
	for (i = 0; i < NR_IRQ_LINE; i += 4) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_PRI + i * 4 / 4, 0xA0A0A0A0);
	}

	/*
	 * Disable all interrupts.
	 */
	for (i = 0; i < NR_IRQ_LINE; i += 32) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + i * 4 / 32, 0xFFFFFFFF);
	}

	/*
	 * Clear all active status
	 */
	for (i = 0; i < NR_IRQ_LINE; i += 32) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ACTIVE_CLEAR + i * 4 / 32, 0xFFFFFFFF);
	}

	/*
	 * Clear all pending status
	 */
	for (i = 0; i < NR_IRQ_LINE; i += 32) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_PENDING_CLEAR + i * 4 / 32, 0xFFFFFFFF);
	}


	dsb();
	mt_git_dist_rwp();
	DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_CTRL, GICD_CTLR_ARE | GICD_CTLR_ENGRP1S | GICD_CTLR_ENGRP1NS);
	mt_git_dist_rwp();
}

void platform_init_interrupts(void)
{
	uint32_t sec;

	sec = mt_interrupt_needed_for_secure();

	if (sec)
		mt_gic_icc_msre_write();

	mt_gic_dist_init();

	if (sec)
		mt_gic_redist_init();

	mt_gic_cpu_init();
}

void platform_deinit_interrupts(void)
{
	unsigned int irq;

	for (irq = 0; irq < NR_IRQ_LINE; irq += 32) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + irq * 4 / 32, 0xFFFFFFFF);
	}

	dsb();

	while ((irq = mt_gic_icc_iar1_read()) != 1023 ) {
		mt_gic_icc_eoir1_write(irq);
	}
}

uint32_t mt_irq_get(void)
{
	return mt_gic_icc_iar1_read();
}

void mt_irq_set_polarity(unsigned int irq, unsigned int polarity)
{
	unsigned int offset;
	unsigned int reg_index;
	unsigned int value;

	// peripheral device's IRQ line is using GIC's SPI, and line ID >= GIC_PRIVATE_SIGNALS
	if (irq < GIC_PRIVATE_SIGNALS) {
		return;
	}

	offset = (irq - GIC_PRIVATE_SIGNALS) & 0x1F;
	reg_index = (irq - GIC_PRIVATE_SIGNALS) >> 5;
	if (polarity == 0) {
		value = DRV_Reg32(INT_POL_CTL0 + (reg_index * 4));
		value |= (1 << offset); // always invert the incoming IRQ's polarity
		DRV_WriteReg32((INT_POL_CTL0 + (reg_index * 4)), value);
	} else {
		value = DRV_Reg32(INT_POL_CTL0 + (reg_index * 4));
		value &= ~(0x1 << offset);
		DRV_WriteReg32(INT_POL_CTL0 + (reg_index * 4), value);
	}
}

void mt_irq_set_sens(unsigned int irq, unsigned int sens)
{
	unsigned int config;

	if (sens == MT65xx_EDGE_SENSITIVE) {
		config = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4);
		config |= (0x2 << (irq % 16) * 2);
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4, config);
	} else {
		config = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4);
		config &= ~(0x2 << (irq % 16) * 2);
		DRV_WriteReg32( GIC_DIST_BASE + GIC_DIST_CONFIG + (irq / 16) * 4, config);
	}
	dsb();
}

/*
 * mt_irq_mask: mask one IRQ
 * @irq: IRQ line of the IRQ to mask
 */
void mt_irq_mask(unsigned int irq)
{
	unsigned int mask = 1 << (irq % 32);

	DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + irq / 32 * 4, mask);
	dsb();
}

/*
 * mt_irq_unmask: unmask one IRQ
 * @irq: IRQ line of the IRQ to unmask
 */
void mt_irq_unmask(unsigned int irq)
{
	unsigned int mask = 1 << (irq % 32);

	DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ENABLE_SET + irq / 32 * 4, mask);
	dsb();
}

/*
 * mt_irq_ack: ack IRQ
 * @irq: IRQ line of the IRQ to mask
 */
void mt_irq_ack(unsigned int irq)
{
	mt_gic_icc_eoir1_write(irq);
	dsb();
}

/*
 * mt_irq_mask_all: mask all IRQ lines. (This is ONLY used for the sleep driver)
 * @mask: pointer to struct mtk_irq_mask for storing the original mask value.
 * Return 0 for success; return negative values for failure.
 */
int mt_irq_mask_all(struct mtk_irq_mask *mask)
{
	unsigned int i;

	if (mask) {
		for (i = 0; i < IRQ_REGS; i++) {
			mask->mask[i] = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_ENABLE_SET + i * 4);
			DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ENABLE_CLEAR + i * 4, 0xFFFFFFFF);
		}

		dsb();

		mask->header = IRQ_MASK_HEADER;
		mask->footer = IRQ_MASK_FOOTER;

		return 0;
	} else {
		return -1;
	}
}

/*
 * mt_irq_mask_restore: restore all IRQ lines' masks. (This is ONLY used for the sleep driver)
 * @mask: pointer to struct mtk_irq_mask for storing the original mask value.
 * Return 0 for success; return negative values for failure.
 */
int mt_irq_mask_restore(struct mtk_irq_mask *mask)
{
	unsigned int i;

	if (!mask) {
		return -1;
	}
	if (mask->header != IRQ_MASK_HEADER) {
		return -1;
	}
	if (mask->footer != IRQ_MASK_FOOTER) {
		return -1;
	}

	for (i = 0; i < IRQ_REGS; i++) {
		DRV_WriteReg32(GIC_DIST_BASE + GIC_DIST_ENABLE_SET + i * 4, mask->mask[i]);
	}

	dsb();


	return 0;
}

void mt_irq_register_dump(void)
{
	int i;
	uint32_t reg, reg2;

	dprintf(CRITICAL, "%s(): do irq register dump\n", __func__);

	reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_CTRL);
	dprintf(CRITICAL, "GICD_CTLR: 0x%08x\n", reg);

	for (i = 0; i < MT_NR_SPI; i++) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_ROUTE + i * 8);
		reg2 = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_ROUTE + i * 8 + 4);
		dprintf(CRITICAL, "GICD_IROUTER[%d]: 0x%08x, 0x%08x\n", i, reg, reg2);
	}

	for (i = 0; i < NR_IRQ_LINE; i += 32) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_IGRPMODR + i / 8);
		dprintf(CRITICAL, "GICD_IGRPMODR[%d]: 0x%08x\n", i >> 5, reg);
	}

	for (i = 0; i < NR_IRQ_LINE; i += 4) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_PRI + i * 4 / 4);
		dprintf(CRITICAL, "GICD_IPRIORITYR[%d]: 0x%08x\n", i >> 2, reg);
	}

	for (i = 32; i < (MT_NR_SPI + 32); i += 16) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_CONFIG + i * 4 / 16);
		dprintf(CRITICAL, "DIST_ICFGR[%d]: 0x%08x\n", (i >> 4) - 2, reg);
	}

	for (i = 0; i < IRQ_REGS; i++) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_ENABLE_SET + i * 4);
		dprintf(CRITICAL, "GICD_ISENABLER[%d]: 0x%08x\n", i, reg);
	}

	for (i = 0; i < IRQ_REGS; i++) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_PENDING_SET + i * 4);
		dprintf(CRITICAL, "GICD_ISPENDR[%d]: 0x%08x\n", i, reg);
	}

	for (i = 0; i < IRQ_REGS; i++) {
		reg = DRV_Reg32(GIC_DIST_BASE + GIC_DIST_ACTIVE_SET + i * 4);
		dprintf(CRITICAL, "GICD_ISACTIVER[%d]: 0x%08x\n", i, reg);
	}

	reg = mt_gic_icc_sre_read();
	dprintf(CRITICAL, "ICC_SRE: 0x%08x\n", reg);

	reg = mt_gic_icc_primask_read();
	dprintf(CRITICAL, "ICC_PMR: 0x%08x\n", reg);

	reg = mt_gic_icc_igrpen1_read();
	dprintf(CRITICAL, "ICC_IGRPEN1: 0x%08x\n", reg);

	reg = mt_gic_icc_iar1_read();
	dprintf(CRITICAL, "ICC_IAR1: 0x%08x\n", reg);

	reg = mt_mpidr_read();
	dprintf(CRITICAL, "MPIDR: 0x%08x\n", reg);
}
