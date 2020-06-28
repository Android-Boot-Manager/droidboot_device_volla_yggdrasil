/*
 * Copyright (c) 2012 MediaTek Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in
 *  the documentation and/or other materials provided with the
 *  distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


/*=======================================================================*/
/* HEADER FILES                                                          */
/*=======================================================================*/
#include <debug.h>
#include <env.h>
#include <platform/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <libfdt.h>
#include <upmu_hw.h>

void usb2jtag_hw_init(void)
{
	dprintf(CRITICAL,"[USB2JTAG] usb2jtag_hw_init\n");
	/* set ap_usb2jtag_en: 0x1000_1F00 bit[14] = 1 */
	*(volatile unsigned int *) 0x10001F00 = *(volatile unsigned int *) 0x10001F00 | (0x1 << 14);

	*(volatile unsigned int *) 0x11290818 = 0x00100488;
	*(volatile unsigned int *) 0x11290800 = 0x0000006f;
	*(volatile unsigned int *) 0x11290808 = 0x00000000;

	*(volatile unsigned int *) 0x11210800 = *(volatile unsigned int *) 0x11210800 | 0x21;
	*(volatile unsigned int *) 0x11210818 = *(volatile unsigned int *) 0x11210818 & ~(1 << 23);
	*(volatile unsigned int *) 0x11210820 = *(volatile unsigned int *) 0x11210820 | (1 << 9);

	*(volatile unsigned int *) 0x1129086C = 0x00050000;
	udelay(100);
	*(volatile unsigned int *) 0x11290820 = 0x0000ff1a;

}


unsigned int get_usb2jtag(void)
{
	unsigned int ap_jtag_status;

	ap_jtag_status = (get_env("usb2jtag") == NULL) ? 0 : atoi(get_env("usb2jtag"));
	dprintf(CRITICAL,"[USB2JTAG] current setting is %d.\n", ap_jtag_status);
	return ap_jtag_status;
}

unsigned int set_usb2jtag(unsigned int en)
{
	char *USB2JTAG[2] = {"0","1"};

	dprintf(CRITICAL,"[USB2JTAG] current setting is %d.\n",(get_env("usb2jtag") == NULL) ? 0 : atoi(get_env("usb2jtag")));
	if (set_env("usb2jtag", USB2JTAG[en]) == 0) {
		dprintf(CRITICAL,"[USB2JTAG]set USB2JTAG %s success.\n",USB2JTAG[en]);
		return 0;
	} else {
		dprintf(CRITICAL,"[USB2JTAG]set USB2JTAG %s fail.\n",USB2JTAG[en]);
		return 1;
	}
}

void usb2jtag_init(void)
{
	if (get_usb2jtag() == 1)
		usb2jtag_hw_init();
}
