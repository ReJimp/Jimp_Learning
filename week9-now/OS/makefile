BUILD_DIR = ./build
BOOT_DIR = ./boot
ENTRY_PORT = 0xc0001500
AS = nasm
CC = gcc
LD = ld 
LIB = -I lib/ -I lib/kernel/ -I lib/user/ -I kernel/ -I device/
ASFLAGS = -f elf -m32
BOOTFLAGS = -m32
BOOTLIB = -I $(BOOT_DIR)/include/
CFLAGS = -Wall $(LIB) -c -fno-builtin -W -Wstrict-prototypes \
		 -Wmissing-prototypes -fno-stack-protector -m32
LDFLAGS = -m elf_i386 -Ttext $(ENTRY_PORT) -e main -Map $(BUILD_DIR)/kernel.map
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o $(BUILD_DIR)/interrupt.o \
	   $(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/print.o \
	   $(BUILD_DIR)/debug.o

# C代码编译
$(BUILD_DIR)/main.o: kernel/main.c lib/kernel/print.h \
	lib/stdint.h kernel/init.h kernel/debug.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/init.o: kernel/init.c kernel/init.h lib/kernel/print.h \
	lib/stdint.h kernel/interrupt.h device/timer.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/interrupt.o: kernel/interrupt.c kernel/interrupt.h \
	lib/stdint.h kernel/global.h lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/timer.o: device/timer.c device/timer.h lib/stdint.h \
	lib/kernel/io.h lib/kernel/print.h
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/debug.o: kernel/debug.c kernel/debug.h \
	lib/kernel/print.h lib/stdint.h kernel/interrupt.h
	$(CC) $(CFLAGS) $< -o $@

# 汇编代码编译
$(BUILD_DIR)/mbr.bin: boot/mbr.S
	$(AS) $(BOOTLIB) $(BOOTFLAGS) $< -o $@
$(BUILD_DIR)/loader.bin: boot/loader.S 
	$(AS) $(BOOTLIB) $(BOOTFLAGS) $< -o $@
$(BUILD_DIR)/kernel.o: kernel/kernel.S
	$(AS) $(ASFLAGS) $< -o $@
$(BUILD_DIR)/print.o: kernel/print.S
	$(AS) $(ASFLAGS) $< -o $@

# 链接目标文件
$(BUILD_DIR)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

.PHONY : mkdir hd clean all

mkdir:
	if [[ ! -d $(BUILD_DIR) ]]; then mkdir $(BUILD_DIR); fi

hd:
	dd if=$(BUILD_DIR)/mbr.bin of=~/localdebug/bochs/bin/hd60M.img \
	bs=512 count=1 conv=notrunc

	dd if=$(BUILD_DIR)/loader.bin of=~/localdebug/bochs/bin/hd60M.img \
	bs=512 count=2 seek=1 conv=notrunc

	dd if=$(BUILD_DIR)/kernel.bin of=~/localdebug/bochs/bin/hd60M.img \
	bs=512 count=200 seek=9 conv=notrunc

clean:
	cd $(BUILD_DIR) && rm -f ./*

build: $(BUILD_DIR)/kernel.bin

all: mkdir build hd