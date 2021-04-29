LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += -I$(LOCAL_DIR)
INCLUDES += -I$(LOCAL_DIR)/../include
INCLUDES += -I$(LOCAL_DIR)/../../$(PLATFORM)/include

OBJS += \
	$(LOCAL_DIR)/mtk_battery.o  \
	$(LOCAL_DIR)/mtk_charger.o  \
	$(LOCAL_DIR)/mtk_charger_intf.o \
	$(LOCAL_DIR)/../../$(PLATFORM)/mt_gauge.o

ifeq ($(MTK_CHARGER_NEW_ARCH),yes)
	DEFINES += MTK_CHARGER_NEW_ARCH
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

ifeq ($(MTK_MT6360_PMU_CHARGER_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/mt6360_pmu_charger.o
	DEFINES += MTK_MT6360_PMU_CHARGER_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_RT9467_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/rt9467.o
	DEFINES += MTK_RT9467_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_RT9471_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/rt9471.o
	DEFINES += MTK_RT9471_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(MTK_BQ25601_SUPPORT),yes)
	OBJS +=$(LOCAL_DIR)/bq25601.o
	DEFINES += MTK_BQ25601_SUPPORT
	DEFINES += SWCHR_POWER_PATH
endif

ifeq ($(TYPEC_MT6370),yes)
	OBJS += $(LOCAL_DIR)/tcpc_subpmic.o
	DEFINES += TYPEC_MT6370
endif

ifeq ($(TYPEC_MT6360),yes)
	OBJS += $(LOCAL_DIR)/tcpc_subpmic.o
	DEFINES += TYPEC_MT6360
endif

#prize-add-sunshuai-20191128-start
ifeq ($(MTK_CW2015_SUPPORT),yes)
OBJS +=$(LOCAL_DIR)/cw2015_fuel_gauge.o
DEFINES += MTK_CW2015_SUPPORT
endif

ifeq ($(CONFIG_MTK_CW2015_SUPPORT_OF),yes)
DEFINES += CONFIG_MTK_CW2015_SUPPORT_OF
endif
#prize-add-sunshuai-20191128-end

# prize-add-sunshuai-2015 Multi-Battery Solution-20200222-start
ifeq ($(CONFIG_MTK_CW2015_BATTERY_ID_AUXADC),yes)
DEFINES += CONFIG_MTK_CW2015_BATTERY_ID_AUXADC
endif
# prize-add-sunshuai-2015 Multi-Battery Solution-20200222-end

#//prize added by lipengpeng, hl7005 support, 20191212-start
ifeq ($(HL7005ALL_CHARGER_SUPPORT),yes)
	OBJS += $(LOCAL_DIR)/hl7005all_charger.o
	DEFINES += HL7005ALL_CHARGER_SUPPORT
endif
#//prize added by lipengpeng, hl7005 support, 20191212-end
