/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <arch/arm.h>
#include <kernel/thread.h>
#include <ram_console_common.h>
// #include <platform/mtk_wdt.h>       // for mtk_wdt_restart();
void mtk_wdt_restart(void);


void dump_code_section(void)
{
#ifdef DEBUG
#if (DEBUG >= SPEW)
	// Dump 1 MB of the code section.
	const char *code = (char*)MEMBASE;
	const int one_eigth_MB = 131072;

	dprintf(CRITICAL, "Dump code from 0x%p:\n", code);
	for (int i = 0; i < 8; i++) {
		mtk_wdt_restart();
		hexdump((void*)code, one_eigth_MB);
		code += one_eigth_MB;
	}
#endif  // (DEBUG >= SPEW)
#endif  // DEBUG
}

static inline uint32_t arch_read_dfsr(void)
{
	uint32_t reg;
	__asm__ volatile("mrc p15, 0, %0, c5, c0, 0" : "=r" (reg));
	return reg;
}

static inline uint32_t arch_read_dfar(void)
{
	uint32_t reg;
	__asm__ volatile("mrc p15, 0, %0, c6, c0, 0" : "=r" (reg));
	return reg;
}

static void dump_fault_frame(struct arm_fault_frame *frame)
{
	dprintf(CRITICAL, "r0  0x%08x r1  0x%08x r2  0x%08x r3  0x%08x\n", frame->r[0], frame->r[1], frame->r[2], frame->r[3]);
	dprintf(CRITICAL, "r4  0x%08x r5  0x%08x r6  0x%08x r7  0x%08x\n", frame->r[4], frame->r[5], frame->r[6], frame->r[7]);
	dprintf(CRITICAL, "r8  0x%08x r9  0x%08x r10 0x%08x r11 0x%08x\n", frame->r[8], frame->r[9], frame->r[10], frame->r[11]);
	dprintf(CRITICAL, "r12 0x%08x usp 0x%08x ulr 0x%08x pc  0x%08x\n", frame->r[12], frame->usp, frame->ulr, frame->pc);
	dprintf(CRITICAL, "spsr 0x%08x\n", frame->spsr);
	dprintf(CRITICAL, "spsr 0x%08x dfsr 0x%08x dfar 0x%08x\n", frame->spsr, arch_read_dfsr(), arch_read_dfar());
	struct arm_mode_regs regs;
	arm_save_mode_regs(&regs);

	dprintf(CRITICAL, "%c%s r13 0x%08x r14 0x%08x\n", ((frame->spsr & MODE_MASK) == MODE_FIQ) ? '*' : ' ', "fiq", regs.fiq_r13, regs.fiq_r14);
	dprintf(CRITICAL, "%c%s r13 0x%08x r14 0x%08x\n", ((frame->spsr & MODE_MASK) == MODE_IRQ) ? '*' : ' ', "irq", regs.irq_r13, regs.irq_r14);
	dprintf(CRITICAL, "%c%s r13 0x%08x r14 0x%08x\n", ((frame->spsr & MODE_MASK) == MODE_SVC) ? '*' : ' ', "svc", regs.svc_r13, regs.svc_r14);
	dprintf(CRITICAL, "%c%s r13 0x%08x r14 0x%08x\n", ((frame->spsr & MODE_MASK) == MODE_ABT) ? '*' : ' ', "abt", regs.abt_r13, regs.abt_r14);
	dprintf(CRITICAL, "%c%s r13 0x%08x r14 0x%08x\n", ((frame->spsr & MODE_MASK) == MODE_UND) ? '*' : ' ', "und", regs.und_r13, regs.und_r14);
	dprintf(CRITICAL, "%c%s r13 0x%08x r14 0x%08x\n", ((frame->spsr & MODE_MASK) == MODE_SYS) ? '*' : ' ', "sys", regs.sys_r13, regs.sys_r14);

	// dump the bottom of the current stack
	addr_t stack;
	switch (frame->spsr & MODE_MASK) {
		case MODE_FIQ:
			stack = regs.fiq_r13;
			break;
		case MODE_IRQ:
			stack = regs.irq_r13;
			break;
		case MODE_SVC:
			stack = regs.svc_r13;
			break;
		case MODE_UND:
			stack = regs.und_r13;
			break;
		case MODE_SYS:
			stack = regs.sys_r13;
			break;
		default:
			stack = 0;
	}
	dprintf(CRITICAL, "Code: %04x %04x %04x %04x <%04x> %04x %04x %04x %04x\n",
		*(uint16_t *)(frame->pc-8),*(uint16_t *)(frame->pc-6),*(uint16_t *)(frame->pc-4), *(uint16_t *)(frame->pc-2), *(uint16_t *)(frame->pc),
		*(uint16_t *)(frame->pc+2), *(uint16_t *)(frame->pc+4), *(uint16_t *)(frame->pc+6), *(uint16_t *)(frame->pc+8));

	if (current_thread)
	{
		char *stack_limit = current_thread->stack;
		char *stack_bottom = stack_limit + current_thread->stack_size;
		dprintf(CRITICAL, "thread: %s, bottom(stack) = %p, limit(stack) = %p\n",
			current_thread->name, stack_bottom, stack_limit);

		if (stack != 0) {
			unsigned long length, len_ahead = 64;

			if (current_thread->stack_size > 0) {
				length = (unsigned long)(current_thread->stack + current_thread->stack_size)- (unsigned long)stack + len_ahead;
				if ((length <= len_ahead) || (length > DEFAULT_STACK_SIZE))
					length = 4096; //4K
			}
			else //i.e.: idle thread
				length = 384;

			dprintf(CRITICAL, "top of stack at 0x%08x:\n", (unsigned int)stack);
			hexdump((void*)((char*)stack - len_ahead), length);
		}
	}
#ifdef DUMP_CODE_SECTION
	dump_code_section();
#endif
}

static void exception_die(struct arm_fault_frame *frame, int pc_off, const char *msg)
{
	ram_console_set_exp_type(AEE_EXP_TYPE_LK_CRASH);

	inc_critical_section();
	frame->pc += pc_off;
	dprintf(CRITICAL, msg);
	dump_fault_frame(frame);

	halt();
	for(;;);
}

void arm_syscall_handler(struct arm_fault_frame *frame)
{
	exception_die(frame, -4, "unhandled syscall, halting\n");
}

void arm_undefined_handler(struct arm_fault_frame *frame)
{
	exception_die(frame, -4, "undefined abort, halting\n");
}

void arm_data_abort_handler(struct arm_fault_frame *frame)
{
	exception_die(frame, -8, "data abort, halting\n");
}

void arm_prefetch_abort_handler(struct arm_fault_frame *frame)
{
	exception_die(frame, -4, "prefetch abort, halting\n");
}
