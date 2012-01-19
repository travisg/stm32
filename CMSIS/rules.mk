LOCAL_DIR := $(GET_LOCAL_DIR)

COMPILEFLAGS += -I$(LOCAL_DIR)/CM3/CoreSupport

OBJS += \
	$(LOCAL_DIR)/CM3/CoreSupport/core_cm3.o
