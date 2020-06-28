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
#include "lcm_drv.h"

#ifndef BUILD_LK
static struct regulator *lcm_vgp;
static unsigned int GPIO_LCD_PWR;

void lcm_request_gpio_control(struct device *dev)
{
	/* LCM_BIAS_EN GPIO50*/
	GPIO_LCD_PWR = of_get_named_gpio(dev->of_node, "gpio_lcd_pwr", 0);

	gpio_request(GPIO_LCD_PWR, "GPIO_LCD_PWR");
	pr_debug("[KE/LCM] GPIO_LCD_PWR = 0x%x\n", GPIO_LCD_PWR);
}

/* get LDO supply */
static int lcm_get_vgp_supply(struct device *dev)
{
	int ret;
	struct regulator *lcm_vgp_ldo;

	pr_debug("LCM: lcm_get_vgp_supply is going\n");

	lcm_vgp_ldo = devm_regulator_get(dev, "reg-lcm");
	if (IS_ERR(lcm_vgp_ldo)) {
		ret = PTR_ERR(lcm_vgp_ldo);
		dev_err(dev, "failed to get reg-lcm LDO, %d\n", ret);
		return ret;
	}

	pr_debug("LCM: lcm get supply ok.\n");

	ret = regulator_enable(lcm_vgp_ldo);
	/* get current voltage settings */
	ret = regulator_get_voltage(lcm_vgp_ldo);
	pr_debug("lcm LDO voltage = %d in LK stage\n", ret);

	lcm_vgp = lcm_vgp_ldo;

	return ret;
}

int lcm_vgp_supply_enable(void)
{
	int ret;
	unsigned int volt;

	pr_debug("LCM: lcm_vgp_supply_enable\n");

	if (lcm_vgp == NULL)
		return 0;

	pr_debug("LCM: set regulator voltage lcm_vgp voltage to 3.3V\n");
	/* set voltage to 3.3V */
	ret = regulator_set_voltage(lcm_vgp, 3300000, 3300000);
	if (ret != 0) {
		pr_err("LCM: lcm failed to set lcm_vgp voltage: %d\n", ret);
		return ret;
	}

	/* get voltage settings again */
	volt = regulator_get_voltage(lcm_vgp);
	if (volt == 3300000)
		pr_err("LCM: check regulator voltage=3300000 pass!\n");
	else
		pr_err("LCM: check regulator voltage=3300000 fail! (voltage: %d)\n", volt);

	ret = regulator_enable(lcm_vgp);
	if (ret != 0) {
		pr_err("LCM: Failed to enable lcm_vgp: %d\n", ret);
		return ret;
	}

	return ret;
}

int lcm_vgp_supply_disable(void)
{
	int ret = 0;
	unsigned int isenable;

	if (lcm_vgp == NULL)
		return 0;

	/* disable regulator */
	isenable = regulator_is_enabled(lcm_vgp);

	pr_debug("LCM: lcm query regulator enable status[0x%x]\n", isenable);

	if (isenable) {
		ret = regulator_disable(lcm_vgp);
		if (ret != 0) {
			pr_err("LCM: lcm failed to disable lcm_vgp: %d\n", ret);
			return ret;
		}
		/* verify */
		isenable = regulator_is_enabled(lcm_vgp);
		if (!isenable)
			pr_err("LCM: lcm regulator disable pass\n");
	}

	return ret;
}

static int lcm_probe(struct device *dev)
{
	lcm_get_vgp_supply(dev);
	lcm_request_gpio_control(dev);
	lcm_vgp_supply_enable();

	return 0;
}

static const struct of_device_id lcm_of_ids[] = {
	{.compatible = "mediatek,lcm",},
	{}
};

static struct platform_driver lcm_driver = {
	.driver = {
		   .name = "mtk_lcm",
		   .owner = THIS_MODULE,
		   .probe = lcm_probe,
#ifdef CONFIG_OF
		   .of_match_table = lcm_of_ids,
#endif
		   },
};

static int __init lcm_init(void)
{
	pr_notice("LCM: Register lcm driver\n");
	if (platform_driver_register(&lcm_driver)) {
		pr_err("LCM: failed to register disp driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit lcm_exit(void)
{
	platform_driver_unregister(&lcm_driver);
	pr_notice("LCM: Unregister lcm driver done\n");
}
late_initcall(lcm_init);
module_exit(lcm_exit);
#endif
/* --------------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------------- */
#define FRAME_WIDTH  (1024)
#define FRAME_HEIGHT (600)

#define GPIO_OUT_ONE  1
#define GPIO_OUT_ZERO 0

#ifdef GPIO_LCM_PWR
#define GPIO_LCD_PWR      GPIO_LCM_PWR
#endif

/* GPIO54       DISP_PWM0 for backlight panel */
#ifdef GPIO_LCM_BL_EN
#define GPIO_LCD_BL_EN		GPIO_LCM_BL_EN
#else
#define GPIO_LCD_BL_EN		GPIO54
#endif

/* GPIO66       LCM_RST panel */
#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST		GPIO_LCM_RST
#else
#define GPIO_LCD_RST		GPIO66
#endif

/* --------------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------------- */
static LCM_UTIL_FUNCS lcm_util = { 0 };

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
#ifdef BUILD_LK
	mt_set_gpio_mode(GPIO, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO, output);
#else
	gpio_set_value(GPIO, output);
#endif
}

static void lcm_init_power(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init_power() enter\n");

	mt6392_upmu_set_rg_vgp2_vosel(7);
	mt6392_upmu_set_rg_vgp2_en(0x1);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_PWR, GPIO_OUT_ONE);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(20);

	/* lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE); */
#else
	pr_err("[Kernel/LCM] lcm_init_power() enter\n");
#endif
}

static void lcm_suspend_power(void)
{
#ifndef BUILD_LK
	pr_err("[Kernel/LCM] lcm_suspend_power() enter\n");

	SET_RESET_PIN(0);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_PWR, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_vgp_supply_disable();
	MDELAY(20);
#endif
}

static void lcm_resume_power(void)
{
#ifndef BUILD_LK
	pr_err("[Kernel/LCM] lcm_resume_power() enter\n");

	lcm_set_gpio_output(GPIO_LCD_PWR, GPIO_OUT_ONE);
	MDELAY(20);

	lcm_vgp_supply_enable();
	MDELAY(20);

	SET_RESET_PIN(1);
	MDELAY(20);
#endif
}

/* --------------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------------- */
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DPI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dpi.format = LCM_DPI_FORMAT_RGB666;
	params->dpi.rgb_order = LCM_COLOR_ORDER_RGB;

	params->dpi.clk_pol = LCM_POLARITY_RISING;
	params->dpi.de_pol = LCM_POLARITY_RISING;
	params->dpi.vsync_pol = LCM_POLARITY_RISING;
	params->dpi.hsync_pol = LCM_POLARITY_RISING;

	params->dpi.hsync_pulse_width = 48;
	params->dpi.hsync_back_porch = 112;
	params->dpi.hsync_front_porch = 160;

	params->dpi.vsync_pulse_width = 10;
	params->dpi.vsync_back_porch = 13;
	params->dpi.vsync_front_porch = 12;

	params->dpi.width = FRAME_WIDTH;
	params->dpi.height = FRAME_HEIGHT;

	params->dpi.PLL_CLOCK = 51;
	params->dpi.ssc_disable = 1;

	params->dpi.lvds_tx_en = 0;

	params->dpi.io_driving_current = LCM_DRIVING_CURRENT_8MA;
}

static void lcm_init_lcm(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init() enter\n");
#else
	pr_err("[Kernel/LCM] lcm_init() enter\n");
#endif
}

void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");
#else
	pr_err("[Kernel/LCM] lcm_suspend() enter\n");
#endif
}

void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_resume() enter\n");
#else
	pr_err("[Kernel/LCM] lcm_resume() enter\n");
#endif
}

LCM_DRIVER kd070d5450nha6_rgb_dpi_lcm_drv = {
	.name = "kd070d5450nha6_rgb_dpi",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init_lcm,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.init_power = lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,
};
