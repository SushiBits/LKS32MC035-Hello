#!/usr/bin/make -F
TARGET := minispin.elf
TARGET_HEX := $(TARGET:.elf=.ihx)
TARGET_BIN := $(TARGET:.elf=.bin)
TARGETS := $(TARGET) $(TARGET_HEX) $(TARGET_BIN)

ARCH := arm-none-eabi-
CC := $(ARCH)gcc
AS := $(ARCH)gcc -x assembler-with-cpp
CCLD := $(ARCH)gcc
OBJCOPY := $(ARCH)objcopy

SVDCONV_PATH := C:\Keil_v5\UV4
SVDCONV := $(SVDCONV_PATH)/SVDConv.exe

GENHDRS := platform/include/LKS32MC03x.h platform/include/LKS32MC03x_handlers.h

C_FILES := $(wildcard src/*.c) $(wildcard platform/src/*.c) # there is no cmsis/src
S_FILES := $(wildcard src/*.S) $(wildcard platform/src/*.S) # there is no cmsis/src
OBJS := $(C_FILES:.c=.c.o) $(S_FILES:.S=.S.o)
DEPS := $(OBJS:.o=.d)

CPUFLAGS := -mcpu=cortex-m0 -mthumb
CPPFLAGS := -Iinclude -Iplatform/include -Icmsis/include
CFLAGS := $(CPUFLAGS) -MMD -MP 
ASFLAGS := $(CPUFLAGS) -MMD -MP
LDFLAGS := -Llib -Lplatform/lib -Tminispin.ld -Wl,--gc-sections # Platform code include a linker script fragment.

all: $(TARGET)
hex: $(TARGET_HEX)
bin: $(TARGET_BIN)
headers: $(GENHDRS)

platform/include/LKS32MC03x.h: platform/share/LKS32MC03x.svd
	-$(SVDCONV) $< -o $(dir $@) --generate=header

platform/include/LKS32MC03x_handlers.h: platform/include/LKS32MC03x.h
	bash platform/share/irqn.sh "\"LKS32MC03x_it.h\"" < $< > $@

%.c.o: %.c $(GENHDRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

%.S.o: %.S $(GENHDRS)
	$(AS) $(CPPFLAGS) $(ASFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CCLD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

%.ihx: %.elf
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	-rm -rf $(TARGETS) $(DEPS) $(OBJS)

distclean: clean
	-rm -rf $(GENHDRS)

-include $(DEPS)
