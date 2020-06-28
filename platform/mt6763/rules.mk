LOCAL_DIR := $(GET_LOCAL_DIR)

ARCH    := arm
ARM_CPU := cortex-a53
CPU     := generic

MMC_SLOT         := 1

PLATFORM_SECURITY_VERSION:=21
DEFINES += PLATFORM_SECURITY_VERSION=$(PLATFORM_SECURITY_VERSION)
MTK_AVB20_SUPPORT:=no
AVB_SHA256_CRYPTO_HW_SUPPORT:=no

# mtk gpt version
GPT_VER := 4
DEFINES += GPT_VER=$(GPT_VER)

# choose one of following value -> 1: disabled/ 2: permissive /3: enforcing
SELINUX_STATUS := 3

# set kernel decompress size limit (200MB)
KERNEL_DECOMPRESS_SIZE := 0x0c800000
DEFINES += KERNEL_DECOMPRESS_SIZE=$(KERNEL_DECOMPRESS_SIZE)

# overwrite SELINUX_STATUS value with PRJ_SELINUX_STATUS, if defined. it's by project variable.
ifdef PRJ_SELINUX_STATUS
	SELINUX_STATUS := $(PRJ_SELINUX_STATUS)
endif

ifeq (yes,$(strip $(MTK_BUILD_ROOT)))
SELINUX_STATUS := 2
DEFINES += MTK_BUILD_ROOT
endif

USE_DTB_NO_DWS = yes

DEVELOP_STAGE ?= SB
# it may be set in ${PROJECT}.mk
ifeq ($(DEVELOP_STAGE), FPGA)
DEFINES += MACH_FPGA=y
DEFINES += MACH_FPGA_NO_DISPLAY=y
DEFINES += NO_MD_POWER_DOWN
#DEFINES += ENABLE_IO_THREAD
endif
ifeq ($(DEVELOP_STAGE), SB)
#DEFINES += NO_PMIC
#DEFINES += NO_BOOT_MODE_SEL
#DEFINES += NO_PLL_TURN_OFF
#DEFINES += NO_BAT_INIT
#DEFINES += NO_MD_POWER_DOWN
#DEFINES += NO_DCM
#DEFINES += MACH_FPGA_NO_DISPLAY=y
DEFINES += ENABLE_IO_THREAD
endif

DEFINES += PLATFORM=\"$(PLATFORM)\"

DEFINES += SELINUX_STATUS=$(SELINUX_STATUS)

DEFINES += PERIPH_BLK_BLSP=1
DEFINES += WITH_CPU_EARLY_INIT=0 WITH_CPU_WARM_BOOT=0 \
	   MMC_SLOT=$(MMC_SLOT)

DEFINES += MTK_CAN_FROM_CLUSTER1=1

# provide PMIC full (cold) reset functionality
MTK_PMIC_FULL_RESET := yes
ifeq ($(MTK_PMIC_FULL_RESET), yes)
	DEFINES += MTK_PMIC_FULL_RESET
endif

CFG_DTB_EARLY_LOADER_SUPPORT := yes
ifeq ($(CFG_DTB_EARLY_LOADER_SUPPORT), yes)
	DEFINES += CFG_DTB_EARLY_LOADER_SUPPORT=1
endif

MTK_3LEVEL_PAGETABLE := yes
ifeq ($(MTK_3LEVEL_PAGETABLE), yes)
	DEFINES += MTK_3LEVEL_PAGETABLE
endif

MBLOCK_LIB_SUPPORT := yes
ifeq ($(MBLOCK_LIB_SUPPORT), yes)
	DEFINES += MBLOCK_LIB_SUPPORT=2
	MBLOCK_LIB_SUPPORT := 2
	MODULES += lib/mblock
	INCLUDES += -I$(LOCAL_DIR)/../../lib/mblock
endif

PLAT_DBG_INFO_LIB_SUPPORT := yes
ifeq ($(PLAT_DBG_INFO_LIB_SUPPORT), yes)
	DEFINES += PLAT_DBG_INFO_LIB_SUPPORT=1
	PLAT_DBG_INFO_LIB_SUPPORT := 1
	MODULES += lib/plat_dbg_info
endif

# Android Test Mode Feature
MTK_ATM_SUPPORT := yes

ifeq ($(MTK_SECURITY_SW_SUPPORT), yes)
	DEFINES += MTK_SECURITY_SW_SUPPORT
endif

MTK_OTP_FRAMEWORK_V2 := no

ifeq ($(MTK_SECURITY_ANTI_ROLLBACK), yes)
	DEFINES += MTK_SECURITY_ANTI_ROLLBACK

ifneq ($(MTK_VER_SIMPLE_TEST),)
	DEFINES += MTK_VER_SIMPLE_TEST=$(MTK_VER_SIMPLE_TEST)
endif

ifeq ($(MTK_OTP_FRAMEWORK_V2), yes)
	DEFINES += MTK_OTP_FRAMEWORK_V2
endif
endif

MTK_SECURITY_YELLOW_STATE_SUPPORT := no
ifeq ($(MTK_SECURITY_YELLOW_STATE_SUPPORT), yes)
	DEFINES += MTK_SECURITY_YELLOW_STATE_SUPPORT
endif

ifeq ($(MD_RMA_SUPPORT), yes)
	DEFINES += MD_RMA_SUPPORT
endif

ifeq ($(MTK_GOOGLE_TRUSTY_SUPPORT), yes)
DEFINES += MTK_LM_2LEVEL_PAGETABLE_MODE
endif

# choose one of following value -> 0: disabled/1: enable

MTK_POWER_ON_WRITE_PROTECT := 1
ifdef PRJ_MTK_POWER_ON_WRITE_PROTECT
	MTK_POWER_ON_WRITE_PROTECT := $(PRJ_MTK_POWER_ON_WRITE_PROTECT)
endif

ifeq ($(MTK_POWER_ON_WRITE_PROTECT), 1)
DEFINES += MTK_POWER_ON_WRITE_PROTECT=$(MTK_POWER_ON_WRITE_PROTECT)
ifeq ($(MTK_SIM_LOCK_POWER_ON_WRITE_PROTECT), yes)
	DEFINES += MTK_SIM_LOCK_POWER_ON_WRITE_PROTECT
endif
endif

MTK_PERSIST_PARTITION_SUPPORT := 0
ifdef PRJ_MTK_PERSIST_PARTITION_SUPPORT
	MTK_PERSIST_PARTITION_SUPPORT := $(PRJ_MTK_PERSIST_PARTITION_SUPPORT)
endif
ifeq ($(MTK_PERSIST_PARTITION_SUPPORT), 1)
DEFINES += MTK_PERSIST_PARTITION_SUPPORT=$(MTK_PERSIST_PARTITION_SUPPORT)
endif

ifeq ($(MTK_SEC_FASTBOOT_UNLOCK_SUPPORT), yes)
	DEFINES += MTK_SEC_FASTBOOT_UNLOCK_SUPPORT
ifeq ($(MTK_SEC_FASTBOOT_UNLOCK_KEY_SUPPORT), yes)
	DEFINES += MTK_SEC_FASTBOOT_UNLOCK_KEY_SUPPORT
endif
endif

ifeq ($(MTK_KERNEL_POWER_OFF_CHARGING),yes)
#Fastboot support off-mode-charge 0/1
#1: charging mode, 0:skip charging mode
DEFINES += MTK_OFF_MODE_CHARGE_SUPPORT
endif

DEFINES += MTK_SMC_K32_SUPPORT

KEDUMP_MINI := yes

ARCH_HAVE_MT_RAMDUMP := yes

# Use it Carefully! Make sure that you know what you are doing!
# This config will force MRDUMP enabled.
ifeq ($(MTK_MRDUMP_ENABLE),)
#MTK_MRDUMP_ENABLE := yes
endif

# Use SRAM to preserve control block
ifneq ($(wildcard $(LOCAL_DIR)/include/platform/mtk_mrdump.h),)
	DEFINES += MTK_MRDUMP_SRAM_CB
	MRDUMP_USB_DUMP := yes
endif

# This config will enable USB dump for MRDUMP
ifeq ($(MRDUMP_USB_DUMP),yes)
	DEFINES += MTK_MRDUMP_USB_DUMP
endif

DEFINES += $(shell echo $(BOOT_LOGO) | tr a-z A-Z)

MTK_POWER_ON_WP := yes
ifeq ($(MTK_EMMC_SUPPORT),yes)
ifeq ($(MTK_POWER_ON_WP),yes)
        DEFINES += MTK_POWER_ON_WP
endif
endif
ifeq ($(MTK_UFS_SUPPORT),yes)
ifeq ($(MTK_POWER_ON_WP),yes)
        DEFINES += MTK_POWER_ON_WP
endif
endif

MTK_PARTITION_COMMON := yes
ifeq ($(MTK_PARTITION_COMMON), yes)
DEFINES += MTK_PARTITION_COMMON
endif

$(info libshowlogo new path ------- $(LOCAL_DIR)/../../lib/libshowlogo)
INCLUDES += -I$(LOCAL_DIR)/include \
            -I$(LOCAL_DIR)/include/platform \
            -I$(LOCAL_DIR)/../../lib/libshowlogo \
            -I$(LK_TOP_DIR)/app/mt_boot/ \
            -Icustom/$(FULL_PROJECT)/lk/include/target \
            -Icustom/$(FULL_PROJECT)/lk/lcm/inc \
            -Icustom/$(FULL_PROJECT)/lk/inc \
            -Icustom/$(FULL_PROJECT)/common \
            -Icustom/$(FULL_PROJECT)/kernel/dct/ \
            -I$(BUILDDIR)/include/dfo \
            -I$(LOCAL_DIR)/../../dev/lcm/inc

INCLUDES += -I$(DRVGEN_OUT)/inc

OBJS += \
	$(LOCAL_DIR)/bitops.o \
	$(LOCAL_DIR)/mt_gpio.o \
	$(LOCAL_DIR)/mt_disp_drv.o \
	$(LOCAL_DIR)/mt_gpio_init.o \
	$(LOCAL_DIR)/mt_i2c.o \
	$(LOCAL_DIR)/platform.o \
	$(LOCAL_DIR)/uart.o \
	$(LOCAL_DIR)/interrupts.o \
	$(LOCAL_DIR)/timer.o \
	$(LOCAL_DIR)/debug.o \
	$(LOCAL_DIR)/boot_mode.o \
	$(LOCAL_DIR)/load_image.o \
	$(LOCAL_DIR)/atags.o \
	$(LOCAL_DIR)/partition_setting.o \
	$(LOCAL_DIR)/mt_get_dl_info.o \
	$(LOCAL_DIR)/addr_trans.o \
	$(LOCAL_DIR)/factory.o \
	$(LOCAL_DIR)/mtk_key.o \
	$(LOCAL_DIR)/mt_logo.o\
	$(LOCAL_DIR)/boot_mode_menu.o\
	$(LOCAL_DIR)/mt_pmic_wrap_init.o\
	$(LOCAL_DIR)/mmc_common_inter.o \
	$(LOCAL_DIR)/mmc_core.o\
	$(LOCAL_DIR)/mmc_test.o\
	$(LOCAL_DIR)/msdc.o\
	$(LOCAL_DIR)/msdc_io.o\
	$(LOCAL_DIR)/mmc_rpmb.o\
	$(LOCAL_DIR)/msdc_dma.o\
	$(LOCAL_DIR)/msdc_utils.o\
	$(LOCAL_DIR)/msdc_irq.o\
	$(LOCAL_DIR)/upmu_common.o\
	$(LOCAL_DIR)/mt_latch.o\
	$(LOCAL_DIR)/mt_pmic.o\
	$(LOCAL_DIR)/mtk_wdt.o\
	$(LOCAL_DIR)/mt_rtc.o\
	$(LOCAL_DIR)/mt_usb.o\
	$(LOCAL_DIR)/mt_usb_qmu.o\
	$(LOCAL_DIR)/ddp_manager.o\
	$(LOCAL_DIR)/ddp_path.o\
	$(LOCAL_DIR)/ddp_ovl.o\
	$(LOCAL_DIR)/ddp_rdma.o\
	$(LOCAL_DIR)/ddp_misc.o\
	$(LOCAL_DIR)/ddp_info.o\
	$(LOCAL_DIR)/ddp_dither.o\
	$(LOCAL_DIR)/ddp_dump.o\
	$(LOCAL_DIR)/ddp_dsi.o\
	$(LOCAL_DIR)/primary_display.o\
	$(LOCAL_DIR)/disp_lcm.o\
	$(LOCAL_DIR)/ddp_pwm.o\
	$(LOCAL_DIR)/pwm.o \
	$(LOCAL_DIR)/sec_efuse.o\
	$(LOCAL_DIR)/sec_policy.o\
	$(LOCAL_DIR)/mtk_auxadc.o\
	$(LOCAL_DIR)/mt_efuse.o\
	$(LOCAL_DIR)/aee_platform_debug.o\
	$(LOCAL_DIR)/mt_dummy_read.o\
	$(LOCAL_DIR)/../common/emi/emi_mpu_v1.o\
	$(LOCAL_DIR)/../common/emi/emi_info_v1.o\
	$(LOCAL_DIR)/pll.o \
	$(LOCAL_DIR)/spm_md_mtcmos.o \
	$(LOCAL_DIR)/md1_off.o \
	$(LOCAL_DIR)/mtk_dcm.o \
	$(LOCAL_DIR)/mtk_dcm_autogen.o \
	$(LOCAL_DIR)/mtk_mcdi.o \
	$(LOCAL_DIR)/crypto_hw.o \
	$(LOCAL_DIR)/oaep.o \
	$(LOCAL_DIR)/rsa_oaep.o \
	$(LOCAL_DIR)/anti_rollback_cust.o

ifeq ($(MTK_AB_OTA_UPDATER),yes)
OBJS += $(LOCAL_DIR)/write_protect_ab.o
else
OBJS += $(LOCAL_DIR)/write_protect.o
endif

ifeq ($(MTK_SMI_SUPPORT),yes)
OBJS += $(LOCAL_DIR)/mtk_smi.o
DEFINES += MTK_SMI_SUPPORT
endif

ifeq ($(MTK_UFS_SUPPORT),yes)
OBJS += \
	$(LOCAL_DIR)/ufs_aio_platform.o
endif

ifeq ($(MTK_TINYSYS_SCP_SUPPORT), yes)
OBJS += $(LOCAL_DIR)/mt_scp.o
DEFINES += MTK_TINYSYS_SCP_SUPPORT
endif

ifeq ($(MTK_TINYSYS_SSPM_SUPPORT), yes)
OBJS += $(LOCAL_DIR)/mt_sspm.o
DEFINES += MTK_TINYSYS_SSPM_SUPPORT

ifeq ($(MTK_TINYSYS_SSPM_GPIO_SUPPORT), yes)
DEFINES += MTK_TINYSYS_SSPM_GPIO_SUPPORT
endif
endif

ifeq ($(SPM_FW_USE_PARTITION), yes)
DEFINES += SPM_FW_USE_PARTITION
endif

ifeq ($(MTK_SECURITY_SW_SUPPORT), yes)
OBJS +=	$(LOCAL_DIR)/img_auth_stor.o
endif

ifneq ($(MACH_FPGA_LED_SUPPORT), yes)
OBJS += $(LOCAL_DIR)/mt_leds.o
else
DEFINES += MACH_FPGA_LED_SUPPORT
endif

OBJS += $(LOCAL_DIR)/mt_battery.o

ifneq ($(MTK_EMMC_SUPPORT),yes)
#	OBJS +=$(LOCAL_DIR)/mtk_nand.o
#	OBJS +=$(LOCAL_DIR)/bmt.o
endif

ifeq ($(MTK_USB2JTAG_SUPPORT),yes)
OBJS += $(LOCAL_DIR)/mt_usb2jtag.o
DEFINES += MTK_USB2JTAG_SUPPORT
endif

ifeq ($(MTK_MT8193_SUPPORT),yes)
#OBJS +=$(LOCAL_DIR)/mt8193_init.o
#OBJS +=$(LOCAL_DIR)/mt8193_ckgen.o
#OBJS +=$(LOCAL_DIR)/mt8193_i2c.o
endif

# begin, prize-lifenfen-20181207, add fuel gauge cw2015
#cw2015 support for capacity.voltage
ifeq ($(MTK_CW2015_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/cw2015_fuel_gauge.o
DEFINES += MTK_CW2015_SUPPORT

ifneq ($(PRIZE_CW2015_I2C_EXCHANGE_SUPPORT),)
DEFINES += PRIZE_CW2015_I2C_EXCHANGE_SUPPORT_$(PRIZE_CW2015_I2C_EXCHANGE_SUPPORT)
endif
$(info ---$(PRIZE_CW2015_I2C_EXCHANGE_SUPPORT) --- $(DEFINES) --- MTK_CW2015_SUPPORT test)
endif
# end, prize-lifenfen-20181207, add fuel gauge cw2015
ifeq ($(MTK_KERNEL_POWER_OFF_CHARGING),yes)
OBJS +=$(LOCAL_DIR)/mt_kernel_power_off_charging.o
DEFINES += MTK_KERNEL_POWER_OFF_CHARGING
endif

ifeq ($(MTK_PUMP_EXPRESS_PLUS_SUPPORT),yes)
DEFINES += MTK_PUMP_EXPRESS_PLUS_SUPPORT
endif

ifeq ($(MTK_BQ24261_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/bq24261.o
else
  ifeq ($(MTK_BQ24196_SUPPORT),yes)
    OBJS +=$(LOCAL_DIR)/bq24196.o
  else
    ifeq ($(MTK_BQ25896_SUPPORT),yes)
      OBJS +=$(LOCAL_DIR)/bq25890.o
      DEFINES += MTK_BQ25896_SUPPORT
      DEFINES += SWCHR_POWER_PATH
    endif
  endif
endif


#prize-add by sunshuai for 5715 wireless charge start
ifeq ($(PRIZE_WIRELESS_RECEIVER_MAXIC_MT5715),yes)
    DEFINES += PRIZE_WIRELESS_RECEIVER_MAXIC_MT5715
endif
#prize-add by sunshuai for 5715 wireless charge end

#//prize added by huarui, hl7005 support, 20190111-start
ifeq ($(HL7005ALL_CHARGER_SUPPORT),yes)
    OBJS += $(LOCAL_DIR)/hl7005all.o
    DEFINES += HL7005ALL_CHARGER_SUPPORT
endif
#//prize added by huarui, hl7005 support, 20190111-end

ifeq ($(MTK_RT9466_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/rt9466.o
	DEFINES += MTK_RT9466_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_RT9468_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/rt9468.o
	DEFINES += MTK_RT9468_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_MT6370_PMU_CHARGER_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/mt6370_pmu_charger.o
	DEFINES += MTK_MT6370_PMU_CHARGER_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_MT6370_PMU_BLED_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/mt6370_pmu_bled.o
	DEFINES += MTK_MT6370_PMU_BLED_SUPPORT
endif

ifeq ($(MTK_CHARGER_INTERFACE),yes)
	OBJS += $(LOCAL_DIR)/mtk_charger_intf.o
	DEFINES += MTK_CHARGER_INTERFACE
endif

ifeq ($(MTK_NCP1854_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/ncp1854.o
endif

#DUMMY_AP :=yes
ifeq ($(DUMMY_AP),yes)
OBJS +=$(LOCAL_DIR)/dummy_ap.o
DEFINES += DUMMY_AP_MODE
endif

LK_LD_MD_SUPPORT := yes
ifeq ($(LK_LD_MD_SUPPORT), yes)
	INCLUDES += -I$(LOCAL_DIR)/../common/md
	OBJS +=$(LOCAL_DIR)/ccci_lk_load_img_plat.o
	DEFINES += LK_OPT_TO_KERNEL_CCCI
	DEFINES += LK_LD_MD_SUPPORT
	DEFINES += ENABLE_PARSING_LK_ENV
ifneq (,$(strip $(MTK_PROTOCOL1_RAT_CONFIG)))
	DEFINES += MTK_PROTOCOL1_RAT_CONFIG="\"$(strip $(MTK_PROTOCOL1_RAT_CONFIG))\""
endif
endif

ifeq ($(MTK_EFUSE_WRITER_SUPPORT), yes)
    DEFINES += MTK_EFUSE_WRITER_SUPPORT
endif

ifeq ($(TRUSTONIC_TEE_SUPPORT),yes)
DEFINES += TRUSTONIC_TEE_SUPPORT
endif

ifeq ($(RDA_STATUS_SUPPORT),yes)
DEFINES += RDA_STATUS_SUPPORT
endif

ifeq ($(MTK_GOOGLE_TRUSTY_SUPPORT),yes)
DEFINES += MTK_GOOGLE_TRUSTY_SUPPORT
endif

ifeq ($(MTK_SECURITY_SW_SUPPORT), yes)
ifeq ($(CUSTOM_SEC_AUTH_SUPPORT), yes)
LIBSEC := -L$(LOCAL_DIR)/lib --start-group -lsec
else
LIBSEC := -L$(LOCAL_DIR)/lib --start-group -lsec -lcrypto
endif
LIBSEC_PLAT := -lsecplat -ldevinfo --end-group
else
LIBSEC := -L$(LOCAL_DIR)/lib
LIBSEC_PLAT := -lsecplat -ldevinfo
endif

MTK_GIC_VERSION := v3

MTK_LK_REGISTER_WDT := yes
ifeq ($(MTK_LK_REGISTER_WDT),yes)
	DEFINES += MTK_LK_REGISTER_WDT
	OBJS += $(LOCAL_DIR)/mt_debug_dump.o
endif

ifeq ($(MTK_NO_BIGCORES),yes)
	DEFINES += MTK_NO_BIGCORES
endif

LINKER_SCRIPT += $(BUILDDIR)/system-onesegment.ld

MTK_AEE_PLATFORM_DEBUG_SUPPORT := yes
DEFINES += MTK_AEE_PLATFORM_DEBUG_SUPPORT

ifeq ($(MTK_USERIMAGES_USE_F2FS), yes)
DEFINES += MTK_USERIMAGES_USE_F2FS
endif

ifeq ($(MTK_DM_VERITY_OFF),yes)
DEFINES += MTK_DM_VERITY_OFF
endif

OBJS += $(LOCAL_DIR)/dm_verity_status.o

MTK_RC_TO_VENDOR ?= yes

ifeq ($(AVB_SHA256_CRYPTO_HW_SUPPORT),yes)
OBJS += $(LOCAL_DIR)/avb_crypto_hw.o
endif

CFG_LOG_STORE_SUPPORT := yes
#prize-wyq, enum USB port quickly for autotest, 20190531-start
ifeq ($(PRIZE_AUTOTEST_SUPPORT),yes)
DEFINES += PRIZE_AUTOTEST_SUPPORT
endif
#prize-wyq, enum USB port quickly for autotest, 20190531--end
