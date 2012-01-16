
TARGET := stm32
BUILDDIR := build-$(TARGET)

# compiler flags, default libs to link against
COMPILEFLAGS := -g -Os -mcpu=cortex-m3 -mthumb -Wall -W
CFLAGS :=
CPPFLAGS :=
ASMFLAGS :=
LDFLAGS := --script=system-twosegment.ld
LDLIBS :=

UNAME := $(shell uname -s)
ARCH := $(shell uname -m)

# switch any platform specific stuff here
# ifeq ($(findstring CYGWIN,$(UNAME)),CYGWIN)
# ...
# endif
# ifeq ($(UNAME),Darwin)
# ...
# endif

CFLAGS += $(COMPILEFLAGS)
CPPFLAGS += $(COMPILEFLAGS)
ASMFLAGS += $(COMPILEFLAGS)

TOOLCHAIN_PREFIX ?= arm-elf-
CC := $(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PREFIX)ld
CPPFILT := $(TOOLCHAIN_PREFIX)c++filt
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
AS := $(TOOLCHAIN_PREFIX)as
NM := $(TOOLCHAIN_PREFIX)nm

OBJS := \
	start.o \
	main.o

OBJS := $(addprefix $(BUILDDIR)/,$(OBJS))

DEPS := $(OBJS:.o=.d)

.PHONY: all

all: $(BUILDDIR)/$(TARGET).bin $(BUILDDIR)/$(TARGET).lst $(BUILDDIR)/$(TARGET).sym

$(BUILDDIR)/$(TARGET):  $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(BUILDDIR)/$(TARGET).bin: $(BUILDDIR)/$(TARGET)
	$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/$(TARGET).lst: $(BUILDDIR)/$(TARGET)
	$(OBJDUMP) -d $< > $@

$(BUILDDIR)/$(TARGET).sym: $(BUILDDIR)/$(TARGET)
	$(NM) -S -n $< | $(CPPFILT) > $@

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

spotless:
	rm -rf build-*

# makes sure the target dir exists
MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi

$(BUILDDIR)/%.o: %.c
	@$(MKDIR)
	@echo compiling $<
	@$(CC) $(CFLAGS) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

$(BUILDDIR)/%.o: %.cpp
	@$(MKDIR)
	@echo compiling $<
	@$(CC) $(CPPFLAGS) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

$(BUILDDIR)/%.o: %.S
	@$(MKDIR)
	@echo compiling $<
	@$(CC) $(ASMFLAGS) -c $< -MD -MT $@ -MF $(@:%o=%d) -o $@

ifeq ($(filter $(MAKECMDGOALS), clean), )
-include $(DEPS)
endif
