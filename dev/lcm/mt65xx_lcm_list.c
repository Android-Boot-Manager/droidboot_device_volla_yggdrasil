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
#include <lcm_drv.h>
#ifdef BUILD_LK
#include <platform/disp_drv_platform.h>
#else
#include <linux/delay.h>
#include <mach/mt_gpio.h>
#endif

/* used to identify float ID PIN status */
#define LCD_HW_ID_STATUS_LOW      0
#define LCD_HW_ID_STATUS_HIGH     1
#define LCD_HW_ID_STATUS_FLOAT 0x02
#define LCD_HW_ID_STATUS_ERROR  0x03

#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  dprintf(CRITICAL, fmt)
#else
#define LCD_DEBUG(fmt)  printk(fmt)
#endif
extern LCM_DRIVER ili9881c_hdplus_dsi_vdo_cmi_jt_lcm_drv;
extern LCM_DRIVER ili9881c_hdplus_dsi_vdo_cmi_bx_lcm_drv;
extern LCM_DRIVER ili9881c_hdplus_dsi_vdo_cmi_ykl_lcm_drv;
extern LCM_DRIVER otm1282a_hd720_dsi_vdo_60hz_lcm_drv;
extern LCM_DRIVER otm1282a_hd720_dsi_vdo_lcm_drv;
extern LCM_DRIVER vvx10f008b00_wuxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER r63319_wqhd_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER nt35598_wqhd_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER lp079x01_lcm_drv;
extern LCM_DRIVER hx8369_lcm_drv;
extern LCM_DRIVER hx8369_6575_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_hvga_lcm_drv;
extern LCM_DRIVER hx8363_6575_dsi_qvga_lcm_drv;
extern LCM_DRIVER hx8363b_wvga_dsi_cmd_drv;
extern LCM_DRIVER bm8578_lcm_drv;
extern LCM_DRIVER nt35582_mcu_lcm_drv;
extern LCM_DRIVER nt35582_mcu_6575_lcm_drv;
extern LCM_DRIVER nt35582_rgb_6575_lcm_drv;
extern LCM_DRIVER hx8357b_lcm_drv;
extern LCM_DRIVER hx8357c_hvga_dsi_cmd_drv;
extern LCM_DRIVER hx8369_dsi_lcm_drv;
extern LCM_DRIVER hx8369_dsi_6575_lcm_drv;
extern LCM_DRIVER hx8369_dsi_6575_hvga_lcm_drv;
extern LCM_DRIVER hx8369_dsi_6575_qvga_lcm_drv;
extern LCM_DRIVER hx8369_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8369b_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8369b_wvga_dsi_vdo_drv;
extern LCM_DRIVER hx8389b_qhd_dsi_vdo_drv;
extern LCM_DRIVER hx8369_hvga_lcm_drv;
extern LCM_DRIVER ili9481_lcm_drv;
extern LCM_DRIVER nt35582_lcm_drv;
extern LCM_DRIVER s6d0170_lcm_drv;
extern LCM_DRIVER spfd5461a_lcm_drv;
extern LCM_DRIVER ta7601_lcm_drv;
extern LCM_DRIVER tft1p3037_lcm_drv;
extern LCM_DRIVER ha5266_lcm_drv;
extern LCM_DRIVER hsd070idw1_lcm_drv;
extern LCM_DRIVER lg4571_lcm_drv;
extern LCM_DRIVER lg4573b_wvga_dsi_vdo_lh430mv1_drv;
extern LCM_DRIVER lvds_wsvga_lcm_drv;
extern LCM_DRIVER lvds_wsvga_ti_lcm_drv;
extern LCM_DRIVER lvds_wsvga_ti_n_lcm_drv;
extern LCM_DRIVER nt35565_3d_lcm_drv;
extern LCM_DRIVER tm070ddh03_lcm_drv;
extern LCM_DRIVER r61408_lcm_drv;
extern LCM_DRIVER r61408_wvga_dsi_cmd_drv;
extern LCM_DRIVER nt35510_lcm_drv;
extern LCM_DRIVER nt35510_dpi_lcm_drv;
extern LCM_DRIVER nt35510_hvga_lcm_drv;
extern LCM_DRIVER nt35510_qvga_lcm_drv;
extern LCM_DRIVER nt35510_wvga_dsi_cmd_drv;
extern LCM_DRIVER nt35510_6517_lcm_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6572_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6572_hvga_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6572_fwvga_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6572_qvga_drv;
extern LCM_DRIVER nt35510_dsi_vdo_6572_drv;
extern LCM_DRIVER nt35510_dpi_6572_lcm_drv;
extern LCM_DRIVER nt35510_mcu_6572_lcm_drv;
extern LCM_DRIVER nt51012_hd720_dsi_vdo_lcm_drv;
extern LCM_DRIVER r63303_idisplay_lcm_drv;
extern LCM_DRIVER hj080ia_lcm_drv;
extern LCM_DRIVER hj101na02a_lcm_drv;
extern LCM_DRIVER hj101na02a_8135_lcm_drv;
extern LCM_DRIVER hsd070pfw3_lcm_drv;
extern LCM_DRIVER hsd070pfw3_8135_lcm_drv;
extern LCM_DRIVER cm_n070ice_dsi_vdo_lcm_drv;
extern LCM_DRIVER ej101ia_lcm_drv;
extern LCM_DRIVER scf0700m48ggu02_lcm_drv;
extern LCM_DRIVER nt35510_fwvga_lcm_drv;
#if defined(GN_SSD2825_SMD_S6E8AA)
extern LCM_DRIVER gn_ssd2825_smd_s6e8aa;
#endif
extern LCM_DRIVER nt35517_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8369_dsi_bld_lcm_drv;
extern LCM_DRIVER hx8369_dsi_tm_lcm_drv;
extern LCM_DRIVER otm1280a_hd720_dsi_cmd_drv;
extern LCM_DRIVER otm8018b_dsi_vdo_lcm_drv;
extern LCM_DRIVER otm8018b_dsi_vdo_txd_fwvga_lcm_drv;
extern LCM_DRIVER nt35512_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt35512_wvga_dsi_vdo_boe_drv;
extern LCM_DRIVER hx8369_rgb_6585_fpga_lcm_drv;
extern LCM_DRIVER hx8369_rgb_6572_fpga_lcm_drv;
extern LCM_DRIVER hx8369_mcu_6572_lcm_drv;
extern LCM_DRIVER hx8369a_wvga_dsi_cmd_drv;
extern LCM_DRIVER hx8369a_wvga_dsi_vdo_drv;
extern LCM_DRIVER hx8389c_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_cmd_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_cmd_3lane_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_cmd_wvga_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_cmd_fwvga_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_cmd_fwvga_plus_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_cmd_qhd_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_vdo_2lane_lcm_drv;
extern LCM_DRIVER hx8392a_dsi_vdo_3lane_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER ssd2075_hd720_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_auo_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_auo_fwvga_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_auo_wvga_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_auo_qhd_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_cmi_lcm_drv;
extern LCM_DRIVER nt35516_qhd_dsi_cmd_ipsboe_lcm_drv;
extern LCM_DRIVER nt35516_qhd_dsi_cmd_ipsboe_wvga_lcm_drv;
extern LCM_DRIVER nt35516_qhd_dsi_cmd_ipsboe_fwvga_lcm_drv;
extern LCM_DRIVER nt35516_qhd_dsi_cmd_ips9k1431_drv;
extern LCM_DRIVER nt35516_qhd_dsi_cmd_tft9k1342_drv;
extern LCM_DRIVER bp070ws1_lcm_drv;
extern LCM_DRIVER bp101wx1_lcm_drv;
extern LCM_DRIVER bp101wx1_n_lcm_drv;
extern LCM_DRIVER nt35516_qhd_rav4_lcm_drv;
extern LCM_DRIVER r63311_fhd_dsi_vdo_sharp_lcm_drv;
extern LCM_DRIVER r81592_hvga_dsi_cmd_drv;
extern LCM_DRIVER rm68190_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt35596_fhd_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_tps65132_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_vdo_truly_tps65132_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_vdo_truly_nt50358_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_extern_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_2th_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_720p_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_qhd_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_fwvga_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_wvga_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_tps65132_720p_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_6735_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_nt50358_6735_720p_lcm_drv;
extern LCM_DRIVER nt35596_fhd_dsi_vdo_yassy_lcm_drv;
extern LCM_DRIVER nt35596_hd720_dsi_vdo_truly_tps65132_lcm_drv;
extern LCM_DRIVER nt35590_hd720_dsi_cmd_truly2_lcm_drv;
extern LCM_DRIVER otm9608_wvga_dsi_cmd_drv;
extern LCM_DRIVER otm9608_fwvga_dsi_cmd_drv;
extern LCM_DRIVER otm9608_qhd_dsi_cmd_drv;
extern LCM_DRIVER otm9608_qhd_dsi_vdo_drv;
extern LCM_DRIVER nt35510_dbi_18bit_gionee_lcm_drv;
extern LCM_DRIVER otm8009a_fwvga_dsi_cmd_tianma_lcm_drv;
extern LCM_DRIVER otm8009a_fwvga_dsi_vdo_tianma_lcm_drv;
extern LCM_DRIVER hx8389b_qhd_dsi_vdo_tianma_lcm_drv;
extern LCM_DRIVER cm_otc3108bhv161_dsi_vdo_lcm_drv;
extern LCM_DRIVER auo_b079xat02_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8389b_qhd_dsi_vdo_tianma055xdhp_lcm_drv;
extern LCM_DRIVER cpt_claa101fp01_dsi_vdo_lcm_drv;
extern LCM_DRIVER cpt_claa101fp01_dsi_vdo_8163_lcm_drv;
extern LCM_DRIVER h070d_18dm_lcm_drv;
extern LCM_DRIVER hx8394a_hd720_dsi_vdo_tianma_lcm_drv;
extern LCM_DRIVER hx8394d_hd720_dsi_vdo_tianma_lcm_drv;
extern LCM_DRIVER cpt_clap070wp03xg_sn65dsi83_lcm_drv;
extern LCM_DRIVER nt35520_hd720_tm_lcm_drv;
extern LCM_DRIVER nt35520_hd720_boe_lcm_drv;
extern LCM_DRIVER nt35521_hd720_dsi_vdo_boe_lcm_drv;
extern LCM_DRIVER nt35521_hd720_tm_lcm_drv;
extern LCM_DRIVER r69429_wuxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER r69429_wuxga_dsi_cmd_lcm_drv;
extern LCM_DRIVER rm68210_hd720_dsi_ufoe_cmd_lcm_drv;
extern LCM_DRIVER r63311_fhd_dsi_vedio_lcm_drv;
extern LCM_DRIVER otm1283a_6589_hd_dsi;
extern LCM_DRIVER hx8394a_hd720_dsi_vdo_tianma_v2_lcm_drv;
extern LCM_DRIVER clap070wp03xg_lvds_8163_lcm_drv;
extern LCM_DRIVER cpt_clap070wp03xg_lvds_lcm_drv;
extern LCM_DRIVER otm8018b_dsi_vdo_lcsh72_lcm_drv;
extern LCM_DRIVER hx8369_dsi_cmd_6571_lcm_drv;
extern LCM_DRIVER hx8369_dsi_vdo_6571_lcm_drv;
extern LCM_DRIVER hx8369_dbi_6571_lcm_drv;
extern LCM_DRIVER hx8369_dpi_6571_lcm_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6571_lcm_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6571_hvga_lcm_drv;
extern LCM_DRIVER nt35510_dsi_cmd_6571_qvga_lcm_drv;
extern LCM_DRIVER nt35510_dsi_vdo_6571_lcm_drv;
extern LCM_DRIVER nt35510_dbi_6571_lcm_drv;
extern LCM_DRIVER nt35510_dpi_6571_lcm_drv;
extern LCM_DRIVER nt35590_dsi_cmd_6571_fwvga_lcm_drv;
extern LCM_DRIVER nt35590_dsi_cmd_6571_qhd_lcm_drv;
extern LCM_DRIVER it6151_edp_dsi_video_sharp_lcm_drv;
extern LCM_DRIVER nt35517_qhd_dsi_vdo_lcm_drv;
extern LCM_DRIVER otm1283a_hd720_dsi_vdo_tm_lcm_drv;
extern LCM_DRIVER otm1284a_hd720_dsi_vdo_tm_lcm_drv;
extern LCM_DRIVER otm1285a_hd720_dsi_vdo_tm_lcm_drv;
extern LCM_DRIVER hx8389b_qhd_dsi_vdo_lgd_lcm_drv;
extern LCM_DRIVER it6151_fhd_edp_dsi_video_auo_lcm_drv;
extern LCM_DRIVER tf070mc_rgb_v18_mt6571_lcm_drv;
extern LCM_DRIVER zs070ih5015b3h6_mt6571_lcm_drv;
extern LCM_DRIVER a080ean01_dsi_vdo_lcm_drv;
extern LCM_DRIVER it6121_g156xw01v1_lvds_vdo_lcm_drv;
extern LCM_DRIVER cpt_clap070wp03xg_lvds_lcm_drv;
extern LCM_DRIVER r63315_fhd_dsi_vdo_truly_lcm_drv;
extern LCM_DRIVER it6151_lp079qx1_edp_dsi_video_lcm_drv;
extern LCM_DRIVER RX_498HX_615B_lcm_drv;
extern LCM_DRIVER RX_498HX_615B_82_lcm_drv;
extern LCM_DRIVER ili9806c_dsi_vdo_djn_fwvga_lcm_drv;
extern LCM_DRIVER hx8389b_hd720_dsi_vdo_drv;
extern LCM_DRIVER r69338_hd720_dsi_vdo_jdi_drv;
extern LCM_DRIVER r69338_hd720_5in_dsi_vdo_jdi_dw8768_drv;
extern LCM_DRIVER db7436_dsi_vdo_fwvga_drv;
extern LCM_DRIVER r63417_fhd_dsi_cmd_truly_nt50358_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_cmd_truly_nt50358_hdplus_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_cmd_truly_nt50358_720p_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_cmd_truly_nt50358_qhd_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_cmd_truly_nt50358_fwvga_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_vdo_truly_nt50358_lcm_drv;
extern LCM_DRIVER cm_n070ice_dsi_vdo_mt8173_lcm_drv;
extern LCM_DRIVER r63419_wqhd_truly_phantom_cmd_lcm_drv;
extern LCM_DRIVER r63419_wqhd_truly_phantom_vdo_lcm_drv;
extern LCM_DRIVER r63419_fhd_truly_phantom_lcm_drv;
extern LCM_DRIVER r63423_wqhd_truly_phantom_lcm_drv;
extern LCM_DRIVER kd070d5450nha6_rgb_dpi_lcm_drv;
extern LCM_DRIVER kr101ia2s_dsi_vdo_lcm_drv;
extern LCM_DRIVER kr070ia4t_dsi_vdo_lcm_drv;
extern LCM_DRIVER r69338_hd720_dsi_vdo_jdi_dw8755a_drv;
extern LCM_DRIVER otm9605a_qhd_dsi_vdo_drv;
extern LCM_DRIVER ili9806e_dsi_vdo_fwvga_drv;
extern LCM_DRIVER ili9881c_hd_dsi_vdo_ilitek_nt50358_lcm_drv;
extern LCM_DRIVER otm1906a_fhd_dsi_cmd_auto_lcm_drv;
extern LCM_DRIVER clap070wp03xg_lvds_lcm_drv;
extern LCM_DRIVER otm1906b_fhd_dsi_cmd_jdi_tps65132_lcm_drv;
extern LCM_DRIVER otm1906b_fhd_dsi_cmd_jdi_tps65132_mt6797_lcm_drv;
extern LCM_DRIVER otm1906b_fhd_dsi_vdo_jdi_tps65132_mt6797_lcm_drv;
extern LCM_DRIVER nt35523_wxga_dsi_vdo_boe_lcm_drv;
extern LCM_DRIVER nt35523_wxga_dsi_vdo_8163_lcm_drv;
extern LCM_DRIVER nt35523_wsvga_dsi_vdo_boe_lcm_drv;
extern LCM_DRIVER s6e3fa2_fhd1080_dsi_vdo_lcm_drv;
extern LCM_DRIVER ek79023_dsi_wsvga_vdo_lcm_drv;
extern LCM_DRIVER nt35532_fhd_dsi_vdo_sharp_lcm_drv;
extern LCM_DRIVER s6d7aa0_wxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER hx8394c_wxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER it6151_lp079qx1_edp_dsi_video_8163evb_lcm_drv;
extern LCM_DRIVER sy20810800210132_wuxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt35521_hd_dsi_vdo_truly_nt50358_lcm_drv;
extern LCM_DRIVER nt35521_hd_dsi_vdo_truly_nt50358_fwvga_lcm_drv;
extern LCM_DRIVER nt35521_hd_dsi_vdo_truly_nt50358_qhd_lcm_drv;
extern LCM_DRIVER nt35521_hd_dsi_vdo_truly_rt5081_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_truly_nt50358_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_nt50358_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_truly_nt50358_laneswap_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_nt50358_laneswap_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_truly_nt50358_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_truly_rt5081_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_rt5081_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_rt5081_hdp_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_rt5081_hdp_19_9_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_rt5081_hdp_1560_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_rt5081_720p_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_rt5081_qhd_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_truly_rt5081_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_truly_rt5081_720p_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_truly_rt5081_qhd_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_rt5081_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_rt5081_720p_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_rt5081_qhd_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_rt5081_hdp_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_rt5081_hdp_19_9_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_rt5081_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_rt5081_720p_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_rt5081_qhd_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_rt5081_hdp_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_rt5081_hdp_19_9_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_rt5081_hdp_1560_lcm_drv;
extern LCM_DRIVER nt36672_fhdp_dsi_vdo_auo_lcm_drv;
extern LCM_DRIVER nt36672_fhdp_dsi_vdo_auo_laneswap_lcm_drv;
extern LCM_DRIVER nt35510_dsi_cmd_lcm_drv;
extern LCM_DRIVER rm69032_dsi_cmd_lcm_drv;
extern LCM_DRIVER st7789h2_dbi_lcm_drv;
extern LCM_DRIVER ek79007_wsvgalnl_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt71397_wuxga_dsi_vdo_nt65902_lcm_drv;
extern LCM_DRIVER nt35521_wxga_innolux_lcm_drv;
extern LCM_DRIVER hx8394d_wxga_kd_lcm_drv;
extern LCM_DRIVER hx8394f_dsi_vdo_t20_lcm_drv;
extern LCM_DRIVER hx8394f_dsi_vdo_t20_ata_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_truly_nt50358_720p_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_nt50358_720p_extern_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_truly_nt50358_720p_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_truly_nt50358_qhd_lcm_drv;
extern LCM_DRIVER r69429_wqxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt35595_truly_fhd_dsi_vdo_lcm_drv;
extern LCM_DRIVER cpt_clap070wp03xg_wxga_lvds_lcm_drv;
extern LCM_DRIVER b080uan01_2_wuxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER nt36850_wqhd_dsi_2k_cmd_lcm_drv;
extern LCM_DRIVER nt35595_fhd_dsi_cmd_truly_8163_lcm_drv;
extern LCM_DRIVER s6e3ha3_wqhd_2k_cmd_lcm_drv;
extern LCM_DRIVER s6e3fa3_fhd_cmd_lcm_drv;
extern LCM_DRIVER db7400_hd720_dsi_vdo_dongbu_m1v_drv;
extern LCM_DRIVER r69338_fhd_dsi_vdo_jdi_drv;
extern LCM_DRIVER ft8707_fhd_dsi_vdo_lgd_drv;
extern LCM_DRIVER db7400_hd720_dsi_vdo_dongbu_m4_drv;
extern LCM_DRIVER lh570_hd720_dsi_vdo_lgd_phase3_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_nt50358_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_vdo_auo_nt50358_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_nt50358_720p_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_auo_nt50358_laneswap_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_auo_nt50358_laneswap_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_cmd_auo_nt50358_laneswap_mt6799_lcm_drv;
extern LCM_DRIVER nt35695_fhd_dsi_vdo_auo_nt50358_laneswap_mt6799_lcm_drv;
extern LCM_DRIVER auo_wuxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER claa101fp01_dsi_vdo_lcm_drv;
extern LCM_DRIVER r61322_fhd_dsi_vdo_sharp_lfr_lcm_drv;
extern LCM_DRIVER s6e3ha3_wqhd_2k_cmd_laneswap_drv;
extern LCM_DRIVER nt36380_wqhd_vdo_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_nt50358_qhd_lcm_drv;
extern LCM_DRIVER otm1287_wxga_dsi_vdo_auo_guoxian_lcm_drv;
extern LCM_DRIVER jd9365_wxga_dsi_vdo_hsd_pingbo_lcm_drv;
extern LCM_DRIVER ili9881c_hdp_dsi_vdo_ilitek_rt5081_lcm_drv;
extern LCM_DRIVER oppo_tianma_td4310_fhdp_dsi_vdo_rt5081_lcm_drv;
extern LCM_DRIVER nt35695B_fhd_dsi_cmd_auo_nt50358_hdp_lcm_drv;
extern LCM_DRIVER nt36672_fhdp_dsi_vdo_tianma_nt50358_lcm_drv;
extern LCM_DRIVER oppo_tianma_td4310_fhdp_dsi_vdo_nt50358_lcm_drv;
extern LCM_DRIVER ili9881c_hd_dsi_vdo_ilitek_nt50358_3lane_lcm_drv;
extern LCM_DRIVER jd9365_hd720_dsi_lcm_drv;
extern LCM_DRIVER otm1901a_fhd_dsi_vdo_tpv_lcm_drv;
extern LCM_DRIVER es6311_anx6585_zigzag_wxga_lcm_drv;
extern LCM_DRIVER ili9881h_hdp_dsi_vdo_ilitek_rt5081_19_9_lcm_drv;
extern LCM_DRIVER hx83112b_fhdp_dsi_cmd_auo_rt5081_lcm_drv;
/*prize-add lcm -pengguangyi-20190119-start*/
extern LCM_DRIVER hx83112_fhdp_dsi_vdo_auo_drv;
/*prize-add lcm -pengguangyi-20190119-end*/
/*prize-add lcm-tangcong-20190612-start*/
extern LCM_DRIVER xm92160a_hd720_dsi_ivo_ykl_lcm_drv;
/*prize-add lcm-tangcong-20190612-end*/
//prize-add-pengzhipeng-20190411-start
extern LCM_DRIVER hct_ft8716f_fhdliuhai_dsi_vdo_auo_618_drv;
//prize-add-pengzhipeng-20190411-end
/* prize added by chenjiaxi, add lcm, 20190111-start */
extern LCM_DRIVER ili9881c_hd_dsi_vdo_cmi_ata_lcm_drv;
/*prize-add lcm ili9881p-tangcong-20191009-start*/
extern LCM_DRIVER ili9881p_hd_dsi_vdo_cmi_ata_lcm_drv;
/*prize-add lcm ili9881p-tangcong-20191009-end*/
extern LCM_DRIVER td4100_hd720_dsi_vdo_jdi_incell_lcm_drv;
extern LCM_DRIVER r63417_fhd_dsi_cmd_sharp55_lcm_drv;
/* prize added by chenjiaxi, add lcm, 20190111-end */
/* prize added by mahuiyin, lcm, 20190321-start */
extern LCM_DRIVER ft8719_fhdpluse2340_dsi_vdo_tm_lcm_drv;
extern LCM_DRIVER ili9881d_hdplus1560_dsi_vdo_hsd_lcm_drv;
/* prize added by tangcong, add lcm, 20191210-start */
extern LCM_DRIVER ili9881d_hdplus1560_dsi_ivo_dc_lcm_drv;
/* prize added by tangcong, add lcm, 20191210-end */
extern LCM_DRIVER jd9365z_hdplus1560_dsi_vdo_hsd_lcm_drv;
/* prize added by mahuiyin, lcm, 20190321-end */
/* prize added by tangcong, add lcm, 20190810-start */
extern LCM_DRIVER pri_ili9881_hd1512_dsi_vdo_hsd_58_by;
/* prize added by tangcong, add lcm, 20190810-end */
/* prize added by mahuiyin, lcm, 20190823-start */
extern LCM_DRIVER ft8006p_hdp_dsi_vdo_boe_drip_incell_lcm_drv;
/* prize added by mahuiyin, lcm, 20190823-end */
/* prize added by tangcong, add lcm, 20190905-start */
extern LCM_DRIVER st7703_hdplus1520_dsi_vdo_hsd_lcm_drv;
/* prize added by tangcong, add lcm, 20190905-end */

//prize add by lipengpeng 20191203 start
extern LCM_DRIVER ft8719_fhdpluse2340_dsi_vdo_auo_lcm_drv;
//prize add by lipengpeng 20191203 end 

LCM_DRIVER *lcm_driver_list[] = {

#if defined(FT8719_FHDPLUSE2340_DSI_VDO_AUO)
	&ft8719_fhdpluse2340_dsi_vdo_auo_lcm_drv,
#endif
/* prize added by mahuiyin, lcm, 20190321-start */
#if defined(FT8719_FHDPLUSE2340_DSI_VDO_TM)
	&ft8719_fhdpluse2340_dsi_vdo_tm_lcm_drv,
#endif
#if defined(ILI9881D_HDPLUS1560_DSI_VDO_HSD)
	&ili9881d_hdplus1560_dsi_vdo_hsd_lcm_drv,
#endif
/* prize added by tangcong, add lcm, 20191210-start */
#if defined(ILI9881D_HDPLUS1560_DSI_IVO_DC)
	&ili9881d_hdplus1560_dsi_ivo_dc_lcm_drv,
#endif
/* prize added by tangcong, add lcm, 20191210-end */
#if defined(JD9365Z_HDPLUS1560_DSI_VDO_HSD)
	&jd9365z_hdplus1560_dsi_vdo_hsd_lcm_drv,
#endif
/* prize added by mahuiyin, lcm, 20190321-end */
/* prize added by chenjiaxi, add lcm, 20190111-start */
#if defined(ILI9881C_HD_DSI_VDO_CMI_ATA)
	&ili9881c_hd_dsi_vdo_cmi_ata_lcm_drv,
#endif
/*prize-add lcm ili9881p-tangcong-20191009-start*/
#if defined(ILI9881P_HD_DSI_VDO_CMI_ATA)
	&ili9881p_hd_dsi_vdo_cmi_ata_lcm_drv,
#endif
/*prize-add lcm ili9881p-tangcong-20191009-end*/
/*prize-add lcm -lishuwen-20190119-start*/
#if defined(ILI9881C_HDPLUS_DSI_VDO_CMI_BX)
	&ili9881c_hdplus_dsi_vdo_cmi_bx_lcm_drv,
#endif
/* prize added by tangcong, add lcm, 20190810-start */
#if defined(ILI9881C_HDPLUS_DSI_VDO_CMI_YKL)
	&ili9881c_hdplus_dsi_vdo_cmi_ykl_lcm_drv,
#endif
#if defined(PRI_ILI9881_HD1512_DSI_VDO_HSD_58_BY)
	&pri_ili9881_hd1512_dsi_vdo_hsd_58_by,
#endif
/* prize added by tangcong, add lcm, 20190810-end */
/* prize added by tangcong, add lcm, 20190905-start */
#if defined(ST7703_HDPLUS1520_DSI_VDO_HSD)
	&st7703_hdplus1520_dsi_vdo_hsd_lcm_drv,
#endif
/* prize added by tangcong, add lcm, 20190905-end */
#if defined(ILI9881C_HDPLUS_DSI_VDO_CMI_JT)
	&ili9881c_hdplus_dsi_vdo_cmi_jt_lcm_drv,
#endif
/*prize-add lcm -lishuwen-20190119-end*/	
#if defined(TD4100_HD720_DSI_VDO_JDI_INCELL)
	&td4100_hd720_dsi_vdo_jdi_incell_lcm_drv,
#endif
#if defined(R63417_FHD_DSI_CMD_SHARP55)
	&r63417_fhd_dsi_cmd_sharp55_lcm_drv,
#endif
/* prize added by chenjiaxi, add lcm, 20190111-end */
/*prize-add lcm -pengguangyi-20190119-start*/
#if defined(HX83112_FHDP_DSI_VDO_AUO)
	&hx83112_fhdp_dsi_vdo_auo_drv,
#endif
#if defined(XM92160A_HD720_DSI_IVO_YKL)
	&xm92160a_hd720_dsi_ivo_ykl_lcm_drv,
#endif	
/*prize-add lcm-tangcong-20190612-end*/
/*prize-add lcm -pengguangyi-20190119-end*/
//prize-add-pengzhipeng-20190411-start
#if defined(HCT_FT8716F_FHDLIUHAI_DSI_VDO_AUO_618)
	&hct_ft8716f_fhdliuhai_dsi_vdo_auo_618_drv,
#endif
//prize-add-pengzhipeng-20190411-end
#if defined(ILI9881C_HD_DSI_VDO_ILITEK_NT50358_3LANE)
	&ili9881c_hd_dsi_vdo_ilitek_nt50358_3lane_lcm_drv,
#endif
#if defined(NT35595_FHD_DSI_CMD_TRULY_8163)
	&nt35595_fhd_dsi_cmd_truly_8163_lcm_drv,
#endif

#if defined(OTM1284A_HD720_DSI_VDO_TM)
	&otm1284a_hd720_dsi_vdo_tm_lcm_drv,
#endif
#if defined(R69338_FHD_DSI_VDO_JDI)
	&r69338_fhd_dsi_vdo_jdi_drv,
#endif
#if defined(FT8707_FHD_DSI_VDO_LGD)
	&ft8707_fhd_dsi_vdo_lgd_drv,
#endif

#if defined(OTM1285A_HD720_DSI_VDO_TM)
	&otm1285a_hd720_dsi_vdo_tm_lcm_drv,
#endif

#if defined(EK79007_WSVGALNL_DSI_VDO)
	&ek79007_wsvgalnl_dsi_vdo_lcm_drv,
#endif

#if defined(NT71397_WUXGA_DSI_VDO_NT65902)
	&nt71397_wuxga_dsi_vdo_nt65902_lcm_drv,
#endif

#if defined(S6E3FA2_FHD1080_DSI_VDO)
	&s6e3fa2_fhd1080_dsi_vdo_lcm_drv,
#endif

#if defined(OTM1283A_HD720_DSI_VDO_TM)
	&otm1283a_hd720_dsi_vdo_tm_lcm_drv,
#endif
#if defined(IT6151_LP079QX1_EDP_DSI_VIDEO)
	&it6151_lp079qx1_edp_dsi_video_lcm_drv,
#endif

#if defined(VVX10F008B00_WUXGA_DSI_VDO)
	&vvx10f008b00_wuxga_dsi_vdo_lcm_drv,
#endif

#if defined(KD070D5450NHA6_RGB_DPI)
	&kd070d5450nha6_rgb_dpi_lcm_drv,
#endif

#if defined(KR101IA2S_DSI_VDO)
	&kr101ia2s_dsi_vdo_lcm_drv,
#endif

#if defined(KR070IA4T_DSI_VDO)
	&kr070ia4t_dsi_vdo_lcm_drv,
#endif

#if defined(HX8394A_HD720_DSI_VDO_TIANMA_V2)
	&hx8394a_hd720_dsi_vdo_tianma_v2_lcm_drv,
#endif

#if defined(HX8394F_DSI_VDO_T20)
	&hx8394f_dsi_vdo_t20_lcm_drv,
#endif
#if defined(HX8394F_DSI_VDO_T20_ATA)
	&hx8394f_dsi_vdo_t20_ata_lcm_drv,
#endif
#if defined(OTM1283A)
	&otm1283a_6589_hd_dsi,
#endif
#if defined(OTM1282A_HD720_DSI_VDO_60HZ)
	&otm1282a_hd720_dsi_vdo_60hz_lcm_drv,
#endif
#if defined(OTM8018B_DSI_VDO_TXD_FWVGA)
	&otm8018b_dsi_vdo_txd_fwvga_lcm_drv,
#endif

#if defined(TF070MC_RGB_V18_MT6571)
	&tf070mc_rgb_v18_mt6571_lcm_drv,
#endif

#if defined(ZS070IH5015B3H6_RGB_MT6571)
	&zs070ih5015b3h6_mt6571_lcm_drv,
#endif

#if defined(OTM1282A_HD720_DSI_VDO)
	&otm1282a_hd720_dsi_vdo_lcm_drv,
#endif

#if defined(NT36672_FHDP_DSI_VDO_AUO)
	&nt36672_fhdp_dsi_vdo_auo_lcm_drv,
#endif

#if defined(NT36672_FHDP_DSI_VDO_AUO_LANESWAP)
	&nt36672_fhdp_dsi_vdo_auo_laneswap_lcm_drv,
#endif

#if defined(R63311_FHD_DSI_VDO)
	&r63311_fhd_dsi_vedio_lcm_drv,
#endif

#if defined(R63315_FHD_DSI_VDO_TRULY)
	&r63315_fhd_dsi_vdo_truly_lcm_drv,
#endif

#if defined(NT35517_QHD_DSI_VDO)
	&nt35517_dsi_vdo_lcm_drv,
#endif

#if defined(ILI9881C_HD_DSI_VDO_ILITEK_NT50358)
	&ili9881c_hd_dsi_vdo_ilitek_nt50358_lcm_drv,
#endif

#if defined(ILI9806E_DSI_VDO_FWVGA)
	&ili9806e_dsi_vdo_fwvga_drv,
#endif

#if defined(LP079X01)
	&lp079x01_lcm_drv,
#endif

#if defined(HX8369)
	&hx8369_lcm_drv,
#endif

#if defined(HX8369_6575)
	&hx8369_6575_lcm_drv,
#endif

#if defined(HX8394D_WXGA_DSI_VIDEO_KD)
	&hx8394d_wxga_kd_lcm_drv,
#endif

#if defined(BM8578)
	&bm8578_lcm_drv,
#endif

#if defined(NT35582_MCU)
	&nt35582_mcu_lcm_drv,
#endif

#if defined(NT35582_MCU_6575)
	&nt35582_mcu_6575_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD_TRULY2)
	&nt35590_hd720_dsi_cmd_truly2_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_VDO_TRULY)
	&nt35590_hd720_dsi_vdo_truly_lcm_drv,
#endif

#if defined(SSD2075_HD720_DSI_VDO_TRULY)
	&ssd2075_hd720_dsi_vdo_truly_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD)
	&nt35590_hd720_dsi_cmd_drv,
#endif

#if defined(NT35521_HD_DSI_VDO_TRULY_NT50358)
	&nt35521_hd_dsi_vdo_truly_nt50358_lcm_drv,
#endif

#if defined(NT35521_HD_DSI_VDO_TRULY_NT50358_FWVGA)
	&nt35521_hd_dsi_vdo_truly_nt50358_fwvga_lcm_drv,
#endif

#if defined(NT35521_HD_DSI_VDO_TRULY_NT50358_QHD)
	&nt35521_hd_dsi_vdo_truly_nt50358_qhd_lcm_drv,
#endif

#if defined(NT35521_HD_DSI_VDO_TRULY_RT5081)
	&nt35521_hd_dsi_vdo_truly_rt5081_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD_AUO)
	&nt35590_hd720_dsi_cmd_auo_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD_AUO_WVGA)
	&nt35590_hd720_dsi_cmd_auo_wvga_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD_AUO_QHD)
	&nt35590_hd720_dsi_cmd_auo_qhd_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD_AUO_FWVGA)
	&nt35590_hd720_dsi_cmd_auo_fwvga_lcm_drv,
#endif

#if defined(NT35590_HD720_DSI_CMD_CMI)
	&nt35590_hd720_dsi_cmd_cmi_lcm_drv,
#endif

#if defined(NT35582_RGB_6575)
	&nt35582_rgb_6575_lcm_drv,
#endif

#if  defined(NT51012_HD720_DSI_VDO)
	&nt51012_hd720_dsi_vdo_lcm_drv,
#endif

#if defined(HX8369_RGB_6585_FPGA)
	&hx8369_rgb_6585_fpga_lcm_drv,
#endif

#if defined(HX8369_RGB_6572_FPGA)
	&hx8369_rgb_6572_fpga_lcm_drv,
#endif

#if defined(HX8369_MCU_6572)
	&hx8369_mcu_6572_lcm_drv,
#endif

#if defined(HX8369A_WVGA_DSI_CMD)
	&hx8369a_wvga_dsi_cmd_drv,
#endif

#if defined(HX8369A_WVGA_DSI_VDO)
	&hx8369a_wvga_dsi_vdo_drv,
#endif

#if defined(HX8357B)
	&hx8357b_lcm_drv,
#endif

#if defined(HX8357C_HVGA_DSI_CMD)
	&hx8357c_hvga_dsi_cmd_drv,
#endif

#if defined(R61408)
	&r61408_lcm_drv,
#endif

#if defined(R61408_WVGA_DSI_CMD)
	&r61408_wvga_dsi_cmd_drv,
#endif

#if defined(HX8369_DSI_VDO)
	&hx8369_dsi_vdo_lcm_drv,
#endif

#if defined(HX8369_DSI)
	&hx8369_dsi_lcm_drv,
#endif

#if defined(HX8369_6575_DSI)
	&hx8369_dsi_6575_lcm_drv,
#endif

#if defined(HX8369_6575_DSI_NFC_ZTE)
	&hx8369_dsi_6575_lcm_drv,
#endif

#if defined(HX8369_6575_DSI_HVGA)
	&hx8369_dsi_6575_hvga_lcm_drv,
#endif

#if defined(HX8369_6575_DSI_QVGA)
	&hx8369_dsi_6575_qvga_lcm_drv,
#endif

#if defined(HX8369_HVGA)
	&hx8369_hvga_lcm_drv,
#endif

#if defined(NT35510)
	&nt35510_lcm_drv,
#endif

#if defined(NT35510_RGB_6575)
	&nt35510_dpi_lcm_drv,
#endif

#if defined(NT35510_HVGA)
	&nt35510_hvga_lcm_drv,
#endif

#if defined(NT35510_QVGA)
	&nt35510_qvga_lcm_drv,
#endif

#if defined(NT35510_WVGA_DSI_CMD)
	&nt35510_wvga_dsi_cmd_drv,
#endif

#if defined(NT35510_6517)
	&nt35510_6517_lcm_drv,
#endif

#if defined(NT35510_DSI_CMD_6572)
	&nt35510_dsi_cmd_6572_drv,
#endif

#if defined(NT35510_DSI_CMD_6572_HVGA)
	&nt35510_dsi_cmd_6572_hvga_drv,
#endif

#if defined(NT35510_DSI_CMD_6572_FWVGA)
	&nt35510_dsi_cmd_6572_fwvga_drv,
#endif

#if defined(NT35510_DSI_CMD_6572_QVGA)
	&nt35510_dsi_cmd_6572_qvga_drv,
#endif

#if defined(NT35510_DSI_VDO_6572)
	&nt35510_dsi_vdo_6572_drv,
#endif

#if defined(NT35510_DPI_6572)
	&nt35510_dpi_6572_lcm_drv,
#endif

#if defined(NT35510_MCU_6572)
	&nt35510_mcu_6572_lcm_drv,
#endif

#if defined(ILI9481)
	&ili9481_lcm_drv,
#endif

#if defined(NT35582)
	&nt35582_lcm_drv,
#endif

#if defined(S6D0170)
	&s6d0170_lcm_drv,
#endif

#if defined(SPFD5461A)
	&spfd5461a_lcm_drv,
#endif

#if defined(TA7601)
	&ta7601_lcm_drv,
#endif

#if defined(TFT1P3037)
	&tft1p3037_lcm_drv,
#endif

#if defined(HA5266)
	&ha5266_lcm_drv,
#endif

#if defined(HSD070IDW1)
	&hsd070idw1_lcm_drv,
#endif

#if defined(HX8363_6575_DSI)
	&hx8363_6575_dsi_lcm_drv,
#endif

#if defined(HX8363_6575_DSI_HVGA)
	&hx8363_6575_dsi_hvga_lcm_drv,
#endif

#if defined(HX8363B_WVGA_DSI_CMD)
	&hx8363b_wvga_dsi_cmd_drv,
#endif

#if defined(LG4571)
	&lg4571_lcm_drv,
#endif

#if defined(LG4573B_WVGA_DSI_VDO_LH430MV1)
	&lg4573b_wvga_dsi_vdo_lh430mv1_drv,
#endif

#if defined(LVDS_WSVGA)
	&lvds_wsvga_lcm_drv,
#endif

#if defined(LVDS_WSVGA_TI)
	&lvds_wsvga_ti_lcm_drv,
#endif

#if defined(LVDS_WSVGA_TI_N)
	&lvds_wsvga_ti_n_lcm_drv,
#endif

#if defined(NT35565_3D)
	&nt35565_3d_lcm_drv,
#endif

#if defined(TM070DDH03)
	&tm070ddh03_lcm_drv,
#endif
#if defined(R63303_IDISPLAY)
	&r63303_idisplay_lcm_drv,
#endif

#if defined(HX8369B_DSI_VDO)
	&hx8369b_dsi_vdo_lcm_drv,
#endif

#if defined(HX8369B_WVGA_DSI_VDO)
	&hx8369b_wvga_dsi_vdo_drv,
#endif

#if defined(HX8369B_QHD_DSI_VDO)
	&hx8389b_qhd_dsi_vdo_drv,
#endif

#if defined(HX8389B_HD720_DSI_VDO)
	&hx8389b_hd720_dsi_vdo_drv,
#endif

#if defined(GN_SSD2825_SMD_S6E8AA)
	&gn_ssd2825_smd_s6e8aa,
#endif
#if defined(HX8369_TM_DSI)
	&hx8369_dsi_tm_lcm_drv,
#endif

#if defined(HX8369_BLD_DSI)
	&hx8369_dsi_bld_lcm_drv,
#endif

#if defined(HJ080IA)
	&hj080ia_lcm_drv,
#endif

#if defined(HJ101NA02A)
	&hj101na02a_lcm_drv,
#endif

#if defined(HJ101NA02A_8135)
	&hj101na02a_8135_lcm_drv,
#endif

#if defined(HSD070PFW3)
	&hsd070pfw3_lcm_drv,
#endif

#if defined(HSD070PFW3_8135)
	&hsd070pfw3_8135_lcm_drv,
#endif

#if defined(EJ101IA)
	&ej101ia_lcm_drv,
#endif

#if defined(SCF0700M48GGU02)
	&scf0700m48ggu02_lcm_drv,
#endif

#if defined(OTM1280A_HD720_DSI_CMD)
	&otm1280a_hd720_dsi_cmd_drv,
#endif

#if defined(OTM8018B_DSI_VDO)
	&otm8018b_dsi_vdo_lcm_drv,
#endif

#if defined(NT35512_DSI_VDO)
	&nt35512_dsi_vdo_lcm_drv,
#endif

#if defined(NT35512_WVGA_DSI_VDO_BOE)
	&nt35512_wvga_dsi_vdo_boe_drv,
#endif

#if defined(HX8389C_DSI_VDO)
	&hx8389c_dsi_vdo_lcm_drv,
#endif

#if defined(HX8392A_DSI_CMD)
	&hx8392a_dsi_cmd_lcm_drv,
#endif

#if defined(HX8392A_DSI_CMD_3LANE)
	&hx8392a_dsi_cmd_3lane_lcm_drv,
#endif

#if defined(HX8392A_DSI_CMD_WVGA)
	&hx8392a_dsi_cmd_wvga_lcm_drv,
#endif

#if defined(HX8392A_DSI_CMD_FWVGA)
	&hx8392a_dsi_cmd_fwvga_lcm_drv,
#endif

#if defined(HX8392A_DSI_CMD_FWVGA_PLUS)
	&hx8392a_dsi_cmd_fwvga_plus_lcm_drv,
#endif
#if defined(HX8392A_DSI_CMD_QHD)
	&hx8392a_dsi_cmd_qhd_lcm_drv,
#endif

#if defined(HX8392A_DSI_VDO)
	&hx8392a_dsi_vdo_lcm_drv,
#endif

#if defined(HX8392A_DSI_VDO_2LANE)
	&hx8392a_dsi_vdo_2lane_lcm_drv,
#endif

#if defined(HX8392A_DSI_VDO_3LANE)
	&hx8392a_dsi_vdo_3lane_lcm_drv,
#endif

#if defined(NT35516_QHD_DSI_CMD_IPSBOE)
	&nt35516_qhd_dsi_cmd_ipsboe_lcm_drv,
#endif

#if defined(NT35516_QHD_DSI_CMD_IPSBOE_WVGA)
	&nt35516_qhd_dsi_cmd_ipsboe_wvga_lcm_drv,
#endif

#if defined(NT35516_QHD_DSI_CMD_IPSBOE_FWVGA)
	&nt35516_qhd_dsi_cmd_ipsboe_fwvga_lcm_drv,
#endif

#if defined(NT35516_QHD_DSI_CMD_IPS9K1431)
	&nt35516_qhd_dsi_cmd_ips9k1431_drv,
#endif

#if defined(NT35516_QHD_DSI_CMD_TFT9K1342)
	&nt35516_qhd_dsi_cmd_tft9k1342_drv,
#endif

#if defined(NT35516_QHD_DSI_VEDIO)
	&nt35516_qhd_rav4_lcm_drv,
#endif

#if defined(BP070WS1)
	&bp070ws1_lcm_drv,
#endif

#if defined(BP101WX1)
	&bp101wx1_lcm_drv,
#endif

#if defined(BP101WX1_N)
	&bp101wx1_n_lcm_drv,
#endif

#if defined(CM_N070ICE_DSI_VDO)
	&cm_n070ice_dsi_vdo_lcm_drv,
#endif

#if defined(CM_N070ICE_DSI_VDO_MT8135)
	&cm_n070ice_dsi_vdo_mt8135_lcm_drv,
#endif

#if defined(CM_OTC3108BH161_DSI_VDO)
	&cm_otc3108bhv161_dsi_vdo_lcm_drv,
#endif
#if defined(NT35510_FWVGA)
	&nt35510_fwvga_lcm_drv,
#endif

#if defined(R63311_FHD_DSI_VDO_SHARP)
	&r63311_fhd_dsi_vdo_sharp_lcm_drv,
#endif

#if defined(R81592_HVGA_DSI_CMD)
	&r81592_hvga_dsi_cmd_drv,
#endif

#if defined(RM68190_QHD_DSI_VDO)
	&rm68190_dsi_vdo_lcm_drv,
#endif

#if defined(NT35596_FHD_DSI_VDO_TRULY)
	&nt35596_fhd_dsi_vdo_truly_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_VDO_TRULY)
	&nt35595_fhd_dsi_vdo_truly_lcm_drv,
#endif

#if defined(R63319_WQHD_DSI_VDO_TRULY)
	&r63319_wqhd_dsi_vdo_truly_lcm_drv,
#endif


#if defined(NT35598_WQHD_DSI_VDO_TRULY)
	&nt35598_wqhd_dsi_vdo_truly_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_TPS65132)
	&nt35595_fhd_dsi_cmd_truly_tps65132_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_VDO_TRULY_TPS65132)
	&nt35595_fhd_dsi_vdo_truly_tps65132_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_TPS65132_720P)
	&nt35595_fhd_dsi_cmd_truly_tps65132_720p_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY)
	&nt35595_fhd_dsi_cmd_truly_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358)
	&nt35595_fhd_dsi_cmd_truly_nt50358_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_VDO_TRULY_NT50358)
	&nt35595_fhd_dsi_vdo_truly_nt50358_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_720P)
	&nt35595_fhd_dsi_cmd_truly_nt50358_720p_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_QHD)
	&nt35595_fhd_dsi_cmd_truly_nt50358_qhd_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_FWVGA)
	&nt35595_fhd_dsi_cmd_truly_nt50358_fwvga_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_WVGA)
	&nt35595_fhd_dsi_cmd_truly_nt50358_wvga_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_6735)
	&nt35595_fhd_dsi_cmd_truly_nt50358_6735_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_6735_720P)
	&nt35595_fhd_dsi_cmd_truly_nt50358_6735_720p_lcm_drv,
#endif

#if defined(NT35596_FHD_DSI_VDO_YASSY)
	&nt35596_fhd_dsi_vdo_yassy_lcm_drv,
#endif

#if defined(NT35596_HD720_DSI_VDO_TRULY_TPS65132)
	&nt35596_hd720_dsi_vdo_truly_tps65132_lcm_drv,
#endif

#if defined(AUO_B079XAT02_DSI_VDO)
	&auo_b079xat02_dsi_vdo_lcm_drv,
#endif

#if defined(OTM9608_WVGA_DSI_CMD)
	&otm9608_wvga_dsi_cmd_drv,
#endif

#if defined(OTM9608_FWVGA_DSI_CMD)
	&otm9608_fwvga_dsi_cmd_drv,
#endif

#if defined(OTM9608_QHD_DSI_CMD)
	&otm9608_qhd_dsi_cmd_drv,
#endif

#if defined(OTM9608_QHD_DSI_VDO)
	&otm9608_qhd_dsi_vdo_drv,
#endif

#if defined(NT35510_DBI_18BIT_GIONEE)
	&nt35510_dbi_18bit_gionee_lcm_drv,
#endif

#if defined(OTM8009A_FWVGA_DSI_CMD_TIANMA)
	&otm8009a_fwvga_dsi_cmd_tianma_lcm_drv,
#endif

#if defined(OTM8009A_FWVGA_DSI_VDO_TIANMA)
	&otm8009a_fwvga_dsi_vdo_tianma_lcm_drv,
#endif

#if defined(HX8389B_QHD_DSI_VDO_TIANMA)
	&hx8389b_qhd_dsi_vdo_tianma_lcm_drv,
#endif
#if defined(HX8389B_QHD_DSI_VDO_TIANMA055XDHP)
	&hx8389b_qhd_dsi_vdo_tianma055xdhp_lcm_drv,
#endif

#if defined(CPT_CLAA101FP01_DSI_VDO)
	&cpt_claa101fp01_dsi_vdo_lcm_drv,
#endif

#if defined(CPT_CLAA101FP01_DSI_VDO_8163)
	&cpt_claa101fp01_dsi_vdo_8163_lcm_drv,
#endif

#if defined(IT6151_EDP_DSI_VIDEO_SHARP)
	&it6151_edp_dsi_video_sharp_lcm_drv,
#endif

#if defined(CPT_CLAP070WP03XG_SN65DSI83)
	&cpt_clap070wp03xg_sn65dsi83_lcm_drv,
#endif
#if defined(NT35520_HD720_DSI_CMD_TM)
	&nt35520_hd720_tm_lcm_drv,
#endif
#if defined(NT35520_HD720_DSI_CMD_BOE)
	&nt35520_hd720_boe_lcm_drv,
#endif
#if defined(NT35521_HD720_DSI_VDO_BOE)
	&nt35521_hd720_dsi_vdo_boe_lcm_drv,
#endif
#if defined(NT35521_HD720_DSI_VIDEO_TM)
	&nt35521_hd720_tm_lcm_drv,
#endif
#if defined(NT35521_WXGA_DSI_VIDEO_INNOLUX)
	&nt35521_wxga_innolux_lcm_drv,
#endif
#if defined(R69338_HD720_DSI_VDO_JDI_DW8755A)
	&r69338_hd720_dsi_vdo_jdi_dw8755a_drv,
#endif
#if defined(H070D_18DM)
	&h070d_18dm_lcm_drv,
#endif
#if defined(R69429_WUXGA_DSI_VDO)
	&r69429_wuxga_dsi_vdo_lcm_drv,
#endif

#if defined(HX8394D_HD720_DSI_VDO_TIANMA)
	&hx8394d_hd720_dsi_vdo_tianma_lcm_drv,
#endif

#if defined(HX8394A_HD720_DSI_VDO_TIANMA)
	&hx8394a_hd720_dsi_vdo_tianma_lcm_drv,
#endif

#if defined(R69429_WUXGA_DSI_CMD)
	&r69429_wuxga_dsi_cmd_lcm_drv,
#endif

#if defined(RM68210_HD720_DSI_UFOE_CMD)
	&rm68210_hd720_dsi_ufoe_cmd_lcm_drv,
#endif

#if defined(CLAP070WP03XG_LVDS_8163)
	&clap070wp03xg_lvds_8163_lcm_drv,
#endif

#if defined(CPT_CLAP070WP03XG_LVDS)
	&cpt_clap070wp03xg_lvds_lcm_drv,
#endif

#if defined(OTM8018B_DSI_VDO_LCSH72)
	&otm8018b_dsi_vdo_lcsh72_lcm_drv,
#endif

#if defined(HX8369_DSI_CMD_6571)
	&hx8369_dsi_cmd_6571_lcm_drv,
#endif

#if defined(HX8369_DSI_VDO_6571)
	&hx8369_dsi_vdo_6571_lcm_drv,
#endif

#if defined(RX_498HX_615B_82)
	&RX_498HX_615B_82_lcm_drv,
#endif

#if defined(HX8369_DBI_6571)
	&hx8369_dbi_6571_lcm_drv,
#endif
#if defined(RX_498HX_615B)
	&RX_498HX_615B_lcm_drv,
#endif

#if defined(HX8369_DPI_6571)
	&hx8369_dpi_6571_lcm_drv,
#endif

#if defined(HX8389B_QHD_DSI_VDO_LGD)
	&hx8389b_qhd_dsi_vdo_lgd_lcm_drv,
#endif

#if defined(NT35510_DSI_CMD_6571)
	&nt35510_dsi_cmd_6571_lcm_drv,
#endif

#if defined(NT35510_DSI_CMD_6571_HVGA)
	&nt35510_dsi_cmd_6571_hvga_lcm_drv,
#endif

#if defined(NT35510_DSI_CMD_6571_QVGA)
	&nt35510_dsi_cmd_6571_qvga_lcm_drv,
#endif

#if defined(NT35510_DSI_VDO_6571)
	&nt35510_dsi_vdo_6571_lcm_drv,
#endif

#if defined(NT35510_DBI_6571)
	&nt35510_dbi_6571_lcm_drv,
#endif

#if defined(NT35510_DPI_6571)
	&nt35510_dpi_6571_lcm_drv,
#endif

#if defined(NT35590_DSI_CMD_6571_FWVGA)
	&nt35590_dsi_cmd_6571_fwvga_lcm_drv,
#endif

#if defined(NT35590_DSI_CMD_6571_QHD)
	&nt35590_dsi_cmd_6571_qhd_lcm_drv,
#endif

#if defined(NT35517_QHD_DSI_VIDEO)
	&nt35517_qhd_dsi_vdo_lcm_drv,
#endif

#if defined(IT6151_FHD_EDP_DSI_VIDEO_AUO)
	&it6151_fhd_edp_dsi_video_auo_lcm_drv,
#endif

#if defined(A080EAN01_DSI_VDO)
	&a080ean01_dsi_vdo_lcm_drv,
#endif

#if defined(IT6121_G156XW01V1_LVDS_VDO)
	&it6121_g156xw01v1_lvds_vdo_lcm_drv,
#endif

#if defined(ILI9806C_DSI_VDO_DJN_FWVGA)
	&ili9806c_dsi_vdo_djn_fwvga_lcm_drv,
#endif

#if defined(R69338_HD720_DSI_VDO_JDI)
	&r69338_hd720_dsi_vdo_jdi_drv,
#endif

#if defined(R69338_HD720_5IN_DSI_VDO_JDI_DW8768)
	&r69338_hd720_5in_dsi_vdo_jdi_dw8768_drv,
#endif

#if defined(DB7436_DSI_VDO_FWVGA)
	&db7436_dsi_vdo_fwvga_drv,
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358)
	&r63417_fhd_dsi_cmd_truly_nt50358_lcm_drv,
#endif

#if defined(CM_N070ICE_DSI_VDO_MT8173)
	&cm_n070ice_dsi_vdo_mt8173_lcm_drv,
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_HDPLUS)
	&r63417_fhd_dsi_cmd_truly_nt50358_hdplus_lcm_drv,
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_720P)
	&r63417_fhd_dsi_cmd_truly_nt50358_720p_lcm_drv,
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_QHD)
	&r63417_fhd_dsi_cmd_truly_nt50358_qhd_lcm_drv,
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_FWVGA)
	&r63417_fhd_dsi_cmd_truly_nt50358_fwvga_lcm_drv,
#endif

#if defined(R63417_FHD_DSI_VDO_TRULY_NT50358)
	&r63417_fhd_dsi_vdo_truly_nt50358_lcm_drv,
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_CMD_OK)
	&r63419_wqhd_truly_phantom_cmd_lcm_drv,
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_CMD_OK_WS3142)
	&r63419_wqhd_truly_phantom_cmd_lcm_drv,
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_VDO_OK)
	&r63419_wqhd_truly_phantom_vdo_lcm_drv,
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_VDO_OK_WS3142)
	&r63419_wqhd_truly_phantom_vdo_lcm_drv,
#endif

#if defined(R63419_FHD_TRULY_PHANTOM_2K_CMD_OK)
	&r63419_fhd_truly_phantom_lcm_drv,
#endif

#if defined(R63419_FHD_TRULY_PHANTOM_2K_CMD_OK_WS3142)
	&r63419_fhd_truly_phantom_lcm_drv,
#endif

#if defined(R63423_WQHD_TRULY_PHANTOM_2K_CMD_OK)
	&r63423_wqhd_truly_phantom_lcm_drv,
#endif

#if defined(NT35523_WXGA_DSI_VDO_BOE)
	&nt35523_wxga_dsi_vdo_boe_lcm_drv,
#endif

#if defined(NT35523_WXGA_DSI_VDO_8163)
        &nt35523_wxga_dsi_vdo_8163_lcm_drv,
#endif

#if defined(NT35523_WSVGA_DSI_VDO_BOE)
	&nt35523_wsvga_dsi_vdo_boe_lcm_drv,
#endif

#if defined(EK79023_DSI_WSVGA_VDO)
	&ek79023_dsi_wsvga_vdo_lcm_drv,
#endif

#if defined(NT35532_FHD_DSI_VDO_SHARP)
	&nt35532_fhd_dsi_vdo_sharp_lcm_drv,
#endif

#if defined(OTM9605A_QHD_DSI_VDO)
	&otm9605a_qhd_dsi_vdo_drv,
#endif

#if defined(OTM1906A_FHD_DSI_CMD_AUTO)
	&otm1906a_fhd_dsi_cmd_auto_lcm_drv,
#endif

#if defined(CLAP070WP03XG_LVDS)
	&clap070wp03xg_lvds_lcm_drv,
#endif

#if defined(S6D7AA0_WXGA_DSI_VDO)
	&s6d7aa0_wxga_dsi_vdo_lcm_drv,
#endif

#if defined(SY20810800210132_WUXGA_DSI_VDO)
	&sy20810800210132_wuxga_dsi_vdo_lcm_drv,
#endif

#if defined(OTM1906B_FHD_DSI_CMD_JDI_TPS65132)
	&otm1906b_fhd_dsi_cmd_jdi_tps65132_lcm_drv,
#endif

#if defined(OTM1906B_FHD_DSI_CMD_JDI_TPS65132_MT6797)
	&otm1906b_fhd_dsi_cmd_jdi_tps65132_mt6797_lcm_drv,
#endif

#if defined(OTM1906B_FHD_DSI_VDO_JDI_TPS65132_MT6797)
        &otm1906b_fhd_dsi_vdo_jdi_tps65132_mt6797_lcm_drv,
#endif

#if defined(HX8394C_WXGA_DSI_VDO)
	&hx8394c_wxga_dsi_vdo_lcm_drv,
#endif

#if defined(IT6151_LP079QX1_EDP_DSI_VIDEO_8163EVB)
	&it6151_lp079qx1_edp_dsi_video_8163evb_lcm_drv,
#endif

#if defined(NT35510_DSI_CMD)
	&nt35510_dsi_cmd_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_TRULY_NT50358)
	&nt35695B_fhd_dsi_cmd_truly_nt50358_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_NT50358)
	&nt35695_fhd_dsi_cmd_truly_nt50358_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_NT50358)
	&nt35695_fhd_dsi_vdo_truly_nt50358_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_NT50358_LANESWAP)
	&nt35695_fhd_dsi_cmd_truly_nt50358_laneswap_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_NT50358_LANESWAP)
	&nt35695_fhd_dsi_vdo_truly_nt50358_laneswap_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_NT50358_720P)
	&nt35695_fhd_dsi_cmd_truly_nt50358_720p_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_NT50358_720P_EXTERN)
	&nt35695B_fhd_dsi_cmd_auo_nt50358_720p_extern_lcm_drv,
#endif


#if defined(NT35695_FHD_DSI_VDO_TRULY_NT50358_720P)
	&nt35695_fhd_dsi_vdo_truly_nt50358_720p_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_NT50358_QHD)
	&nt35695_fhd_dsi_cmd_truly_nt50358_qhd_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_RT5081)
	&nt35695_fhd_dsi_cmd_truly_rt5081_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_RT5081)
	&nt35695_fhd_dsi_vdo_truly_rt5081_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_RT5081_HDP)
	&nt35695_fhd_dsi_vdo_truly_rt5081_hdp_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_RT5081_HDP_19_9)
	&nt35695_fhd_dsi_vdo_truly_rt5081_hdp_19_9_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_RT5081_HDP_1560)
	&nt35695_fhd_dsi_vdo_truly_rt5081_hdp_1560_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_RT5081_720P)
	&nt35695_fhd_dsi_vdo_truly_rt5081_720p_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_TRULY_RT5081_QHD)
	&nt35695_fhd_dsi_vdo_truly_rt5081_qhd_lcm_drv,
#endif
#if defined(NT35695B_FHD_DSI_CMD_TRULY_RT5081)
	&nt35695B_fhd_dsi_cmd_truly_rt5081_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_TRULY_RT5081_720P)
	&nt35695B_fhd_dsi_cmd_truly_rt5081_720p_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_TRULY_RT5081_QHD)
	&nt35695B_fhd_dsi_cmd_truly_rt5081_qhd_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_RT5081)
	&nt35695B_fhd_dsi_cmd_auo_rt5081_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_RT5081_720P)
	&nt35695B_fhd_dsi_cmd_auo_rt5081_720p_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_RT5081_QHD)
	&nt35695B_fhd_dsi_cmd_auo_rt5081_qhd_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_RT5081_HDP)
	&nt35695B_fhd_dsi_cmd_auo_rt5081_hdp_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_RT5081_HDP_19_9)
	&nt35695B_fhd_dsi_cmd_auo_rt5081_hdp_19_9_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_RT5081)
	&nt35695B_fhd_dsi_vdo_auo_rt5081_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_RT5081_720P)
	&nt35695B_fhd_dsi_vdo_auo_rt5081_720p_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_RT5081_QHD)
	&nt35695B_fhd_dsi_vdo_auo_rt5081_qhd_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_RT5081_HDP)
	&nt35695B_fhd_dsi_vdo_auo_rt5081_hdp_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_RT5081_HDP_19_9)
	&nt35695B_fhd_dsi_vdo_auo_rt5081_hdp_19_9_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_RT5081_HDP_1560)
        &nt35695B_fhd_dsi_vdo_auo_rt5081_hdp_1560_lcm_drv,
#endif

#if defined(RM69032_DSI_CMD)
	&rm69032_dsi_cmd_lcm_drv,
#endif

#if defined(ST7789H2_DBI)
	&st7789h2_dbi_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_EXTERN)
	&nt35595_fhd_dsi_cmd_truly_nt50358_extern_lcm_drv,
#endif

#if defined(NT35595_FHD_DSI_CMD_TRULY_NT50358_2TH)
	&nt35595_fhd_dsi_cmd_truly_nt50358_2th_lcm_drv,
#endif

#if  defined(R69429_WQXGA_DSI_VDO)
	&r69429_wqxga_dsi_vdo_lcm_drv,
#endif

#if defined(NT35595_TRULY_FHD_DSI_VDO)
	&nt35595_truly_fhd_dsi_vdo_lcm_drv,
#endif

#if defined(CPT_CLAP070WP03XG_WXGA_LVDS)
	&cpt_clap070wp03xg_wxga_lvds_lcm_drv,
#endif

#if defined(B080UAN01_2_WUXGA_DSI_VDO)
	&b080uan01_2_wuxga_dsi_vdo_lcm_drv,
#endif

#if defined(NT36850_WQHD_DSI_2K_CMD)
    &nt36850_wqhd_dsi_2k_cmd_lcm_drv,
#endif

#if defined(S6E3HA3_WQHD_2K_CMD)
    &s6e3ha3_wqhd_2k_cmd_lcm_drv,
#endif

#if defined(S6E3FA3_FHD_CMD)
    &s6e3fa3_fhd_cmd_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_AUO_NT50358_LANESWAP)
    &nt35695_fhd_dsi_cmd_auo_nt50358_laneswap_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_AUO_NT50358_LANESWAP)
    &nt35695_fhd_dsi_vdo_auo_nt50358_laneswap_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_CMD_AUO_NT50358_LANESWAP_MT6799)
    &nt35695_fhd_dsi_cmd_auo_nt50358_laneswap_mt6799_lcm_drv,
#endif

#if defined(NT35695_FHD_DSI_VDO_AUO_NT50358_LANESWAP_MT6799)
    &nt35695_fhd_dsi_vdo_auo_nt50358_laneswap_mt6799_lcm_drv,
#endif

#if defined(AUO_WUXGA_DSI_VDO)
	&auo_wuxga_dsi_vdo_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_NT50358)
	&nt35695B_fhd_dsi_cmd_auo_nt50358_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_VDO_AUO_NT50358)
	&nt35695B_fhd_dsi_vdo_auo_nt50358_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_NT50358_720P)
	&nt35695B_fhd_dsi_cmd_auo_nt50358_720p_lcm_drv,
#endif

#if defined(CLAA101FP01_DSI_VDO)
	&claa101fp01_dsi_vdo_lcm_drv,
#endif

#if defined(R61322_FHD_DSI_VDO_SHARP_LFR)
	&r61322_fhd_dsi_vdo_sharp_lfr_lcm_drv,
#endif

#if defined(S6E3HA3_WQHD_2K_CMD_LANESWAP)
	&s6e3ha3_wqhd_2k_cmd_laneswap_drv,
#endif

#if defined(NT36380_WQHD_VDO_OK)
	&nt36380_wqhd_vdo_lcm_drv,
#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_NT50358_HDP)
	&nt35695B_fhd_dsi_cmd_auo_nt50358_hdp_lcm_drv,

#endif

#if defined(NT35695B_FHD_DSI_CMD_AUO_NT50358_QHD)
	&nt35695B_fhd_dsi_cmd_auo_nt50358_qhd_lcm_drv,
#endif

#if defined(OTM1287_WXGA_DSI_VDO_AUO_GUOXIAN)
	&otm1287_wxga_dsi_vdo_auo_guoxian_lcm_drv,
#endif

#if defined(JD9365_WXGA_DSI_VDO_HSD_PINGBO)
	&jd9365_wxga_dsi_vdo_hsd_pingbo_lcm_drv,
#endif

#if defined(ILI9881C_HDP_DSI_VDO_ILITEK_RT5081)
	&ili9881c_hdp_dsi_vdo_ilitek_rt5081_lcm_drv,
#endif

#if defined(OPPO_TIANMA_TD4310_FHDP_DSI_VDO_RT5081)
	&oppo_tianma_td4310_fhdp_dsi_vdo_rt5081_lcm_drv,
#endif

#if defined(NT36672_FHDP_DSI_VDO_TIANMA_NT50358)
    &nt36672_fhdp_dsi_vdo_tianma_nt50358_lcm_drv,
#endif

#if defined(OPPO_TIANMA_TD4310_FHDP_DSI_VDO_NT50358)
	&oppo_tianma_td4310_fhdp_dsi_vdo_nt50358_lcm_drv,
#endif

#if defined(JD9365_HD720_DSI)
	&jd9365_hd720_dsi_lcm_drv,
#endif

#if defined(OTM1901A_FHD_DSI_VDO_TPV)
	&otm1901a_fhd_dsi_vdo_tpv_lcm_drv,
#endif

#if defined(ES6311_ANX6585_ZIGZAG_WXGA)
	&es6311_anx6585_zigzag_wxga_lcm_drv,
#endif

#if defined(ILI9881H_HDP_DSI_VDO_ILITEK_RT5081_19_9)
	&ili9881h_hdp_dsi_vdo_ilitek_rt5081_19_9_lcm_drv,
#endif

#if defined(HX83112B_FHDP_DSI_CMD_AUO_RT5081)
	&hx83112b_fhdp_dsi_cmd_auo_rt5081_lcm_drv,
#endif
/* prize added by mahuiyin, lcm, 20190823-start */
#if defined(FT8006P_HDP_DSI_VDO_BOE_DRIP_INCELL)
	&ft8006p_hdp_dsi_vdo_boe_drip_incell_lcm_drv,
#endif
/* prize added by mahuiyin, lcm, 20190823-end */
};

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
unsigned char lcm_name_list[][128] = {
#if defined(HX8392A_DSI_CMD)
	"hx8392a_dsi_cmd",
#endif

#if defined(HX8392A_DSI_VDO)
	"hx8392a_vdo_cmd",
#endif

#if defined(OTM9608_QHD_DSI_CMD)
	"otm9608a_qhd_dsi_cmd",
#endif

#if defined(OTM9608_QHD_DSI_VDO)
	"otm9608a_qhd_dsi_vdo",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358)
	"r63417_fhd_dsi_cmd_truly_nt50358_drv",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_HDPLUS)
	"r63417_fhd_dsi_cmd_truly_nt50358_hdplus_drv",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_FWVGA)
	"r63417_fhd_dsi_cmd_truly_nt50358_fwvga_drv",
#endif

#if defined(R63417_FHD_DSI_VDO_TRULY_NT50358)
	"r63417_fhd_dsi_vdo_truly_nt50358_drv",
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_CMD_OK_WS3142)
	"r63419_wqhd_truly_phantom_2k_cmd_ok",
#endif

#if defined(NT35695_FHD_DSI_CMD_TRULY_NT50358)
	"nt35695_fhd_dsi_cmd_truly_nt50358_drv",
#endif

#if defined(NT35521_HD_DSI_VDO_TRULY_RT5081)
	"nt35521_hd_dsi_vdo_truly_rt5081_drv",
#endif

};
#endif

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition)?1 :  -1]

unsigned int lcm_count = sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *));
#if defined(NT35520_HD720_DSI_CMD_TM) | defined(NT35520_HD720_DSI_CMD_BOE) | defined(NT35521_HD720_DSI_VDO_BOE) | defined(NT35521_HD720_DSI_VIDEO_TM)
#ifdef BUILD_LK
extern void mdelay(unsigned long msec);
#endif
static unsigned char lcd_id_pins_value = 0xFF;


/******************************************************************************
Function:       which_lcd_module_triple
  Description:    read LCD ID PIN status,could identify three status:highlowfloat
  Input:           none
  Output:         none
  Return:         LCD ID1|ID0 value
  Others:
******************************************************************************/
unsigned char which_lcd_module_triple(void)
{
	unsigned char high_read0 = 0;
	unsigned char low_read0 = 0;
	unsigned char high_read1 = 0;
	unsigned char low_read1 = 0;
	unsigned char lcd_id0 = 0;
	unsigned char lcd_id1 = 0;
	unsigned char lcd_id = 0;
	/* Solve Coverity scan warning : check return value */
	unsigned int ret = 0;
	/* only recognise once */
	if (0xFF != lcd_id_pins_value) {
		return lcd_id_pins_value;
	}
	/* Solve Coverity scan warning : check return value */
	ret = mt_set_gpio_mode(GPIO_DISP_ID0_PIN, GPIO_MODE_00);
	if (0 != ret) {
		LCD_DEBUG("ID0 mt_set_gpio_mode fail\n");
	}
	ret = mt_set_gpio_dir(GPIO_DISP_ID0_PIN, GPIO_DIR_IN);
	if (0 != ret) {
		LCD_DEBUG("ID0 mt_set_gpio_dir fail\n");
	}
	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID0_PIN, GPIO_PULL_ENABLE);
	if (0 != ret) {
		LCD_DEBUG("ID0 mt_set_gpio_pull_enable fail\n");
	}
	ret = mt_set_gpio_mode(GPIO_DISP_ID1_PIN, GPIO_MODE_00);
	if (0 != ret) {
		LCD_DEBUG("ID1 mt_set_gpio_mode fail\n");
	}
	ret = mt_set_gpio_dir(GPIO_DISP_ID1_PIN, GPIO_DIR_IN);
	if (0 != ret) {
		LCD_DEBUG("ID1 mt_set_gpio_dir fail\n");
	}
	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID1_PIN, GPIO_PULL_ENABLE);
	if (0 != ret) {
		LCD_DEBUG("ID1 mt_set_gpio_pull_enable fail\n");
	}
	/* pull down ID0 ID1 PIN */
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
	if (0 != ret) {
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");
	}
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
	if (0 != ret) {
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");
	}
	/* delay 100ms , for discharging capacitance */
	mdelay(100);
	/* get ID0 ID1 status */
	low_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	low_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);
	/* pull up ID0 ID1 PIN */
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
	if (0 != ret) {
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");
	}
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
	if (0 != ret) {
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");
	}
	/* delay 100ms , for charging capacitance */
	mdelay(100);
	/* get ID0 ID1 status */
	high_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	high_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);

	if (low_read0 != high_read0) {
		/*float status , pull down ID0 ,to prevent electric leakage */
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret) {
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");
		}
		lcd_id0 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read0) && (LCD_HW_ID_STATUS_LOW == high_read0)) {
		/*low status , pull down ID0 ,to prevent electric leakage */
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret) {
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");
		}
		lcd_id0 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read0) && (LCD_HW_ID_STATUS_HIGH == high_read0)) {
		/*high status , pull up ID0 ,to prevent electric leakage */
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
		if (0 != ret) {
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");
		}
		lcd_id0 = LCD_HW_ID_STATUS_HIGH;
	} else {
		LCD_DEBUG(" Read LCD_id0 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DISABLE);
		if (0 != ret) {
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Disbale fail\n");
		}
		lcd_id0 = LCD_HW_ID_STATUS_ERROR;
	}


	if (low_read1 != high_read1) {
		/*float status , pull down ID1 ,to prevent electric leakage */
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret) {
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");
		}
		lcd_id1 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read1) && (LCD_HW_ID_STATUS_LOW == high_read1)) {
		/*low status , pull down ID1 ,to prevent electric leakage */
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret) {
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");
		}
		lcd_id1 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read1) && (LCD_HW_ID_STATUS_HIGH == high_read1)) {
		/*high status , pull up ID1 ,to prevent electric leakage */
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
		if (0 != ret) {
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");
		}
		lcd_id1 = LCD_HW_ID_STATUS_HIGH;
	} else {

		LCD_DEBUG(" Read LCD_id1 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DISABLE);
		if (0 != ret) {
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Disable fail\n");
		}
		lcd_id1 = LCD_HW_ID_STATUS_ERROR;
	}
#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#else
	printk("which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	printk("which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#endif
	lcd_id = lcd_id0 | (lcd_id1 << 2);

#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#else
	printk("which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#endif

	lcd_id_pins_value = lcd_id;
	return lcd_id;
}
#endif
