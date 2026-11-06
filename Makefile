#!/usr/bin/make -F
TARGET := minispin.elf
TARGET_HEX := $(TARGET:.elf=.ihx)
TARGET_BIN := $(TARGET:.elf=.bin)
TARGETS := $(TARGET) $(TARGET_HEX) $(TARGET_BIN)

BUILD_DIR := .build

ARCH := arm-none-eabi-
CC := $(ARCH)gcc
CXX := $(ARCH)g++
AS := $(ARCH)as
CCAS := $(CC) -x assembler-with-cpp
CCLD := $(CC)
CXXLD := $(CXX)
OBJCOPY := $(ARCH)objcopy

SVDCONV_PATH := C:\Keil_v5\UV4
SVDCONV := $(SVDCONV_PATH)/SVDConv.exe

GENHDRS := platform/include/LKS32MC03x.h platform/include/LKS32MC03x_handlers.h

SRC_PATHS := src platform/src
HEADER_PATHS := include platform/include cmsis/include
LIB_PATHS := lib platform/lib
LIBS += -lm

C_FILES := $(wildcard $(addsuffix /*.c, $(SRC_PATHS)))
CXX_FILES := $(wildcard $(addsuffix /*.cc, $(SRC_PATHS)) $(addsuffix /*.cpp, $(SRC_PATHS)))
S_FILES := $(wildcard $(addsuffix /*.S, $(SRC_PATHS)) $(addsuffix /*.s, $(SRC_PATHS)))
LD_FILES := $(wildcard $(addsuffix /*.ld, $(LIB_PATHS)))
SRC_FILES := $(C_FILES) $(CXX_FILES) $(S_FILES)
OBJS := $(addprefix $(BUILD_DIR)/,$(addsuffix .o, $(SRC_FILES)))
DEPS := $(OBJS:.o=.d)

CPUFLAGS := -mcpu=cortex-m0 -mthumb
CPPFLAGS := $(addprefix -I, $(HEADER_PATHS))
OPTFLAGS := -Og -g
WARNFLAGS := -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
GENFLAGS := -ffunction-sections -fdata-sections
DEPFLAGS := -MMD -MP

CFLAGS := $(CPUFLAGS) -std=gnu17 $(DEPFLAGS) $(GENFLAGS) $(OPTFLAGS) $(WARNFLAGS)
CXXFLAGS := $(CPUFLAGS) -std=gnu++17 $(DEPFLAGS) $(GENFLAGS) $(OPTFLAGS) $(WARNFLAGS)
ASFLAGS := $(CPUFLAGS) $(DEPFLAGS) $(GENFLAGS) $(OPTFLAGS) $(WARNFLAGS)
LDFLAGS := $(CPUFLAGS) $(addprefix -L, $(LIB_PATHS)) -Wl,-Tminispin.ld -Wl,--gc-sections # Platform code include a linker script fragment.

all: headers $(TARGET)
hex: $(TARGET_HEX)
bin: $(TARGET_BIN)
headers: $(GENHDRS)
.PHONY: all hex bin headers clean distclean

platform/include/LKS32MC03x.h: platform/share/LKS32MC03x.svd
	-$(SVDCONV) $< -o $(dir $@) --generate=header --fields=macro --fields=enum

platform/include/LKS32MC03x_handlers.h: platform/include/LKS32MC03x.h
	bash platform/share/irqn.sh "\"LKS32MC03x_it.h\"" < $< > $@

$(BUILD_DIR)/%.c.o: %.c $(GENHDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc $(GENHDRS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp $(GENHDRS)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.S.o: %.S $(GENHDRS)
	@mkdir -p $(dir $@)
	$(CCAS) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.s.o: %.s $(GENHDRS)
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

$(TARGET): $(OBJS) $(LD_FILES)
	$(if $(CXX_FILES),$(CXXLD),$(CCLD)) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

%.ihx: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	-rm -rf $(TARGETS) $(BUILD_DIR)

distclean: clean
	-rm -rf $(GENHDRS)

-include $(DEPS)
