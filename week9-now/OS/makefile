BUILD_DIR = ./build
ENTRY_PORT = 0xc0001500
AS = nasm
CC = gcc
LD = ld 
LIB = -I lib/ -I lib/kernel/ -I lib/user/ -I kernel/ -I device/
ASFLAGS = -f elf
CFLAGS = -Wall $(LIB) -c -fno-builtin -W -Wstrict-prototypes \
		 -Wmissing-prototypes -m32