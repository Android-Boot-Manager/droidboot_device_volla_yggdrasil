LOCAL_DIR := $(GET_LOCAL_DIR)

INCLUDES += \
	    -I$(LK_TOP_DIR)/app/mt_boot/ \
	    -I$(LK_TOP_DIR)/platform/$(PLATFORM)/include \
	    -I$(LK_TOP_DIR)/platform/common/include

OBJS += \
	$(LOCAL_DIR)/spm_common.o
