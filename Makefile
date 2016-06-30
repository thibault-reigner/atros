#This Makefile is adapted from http://wiki.osdev.org/Makefile

ARCH := x86
SUBARCH := i686
TARGET := $(ARCH)

TARGET_BIN := atros.img


BASE_DIR:=$(shell pwd)
SRC_DIR = $(BASE_DIR)/src
ISO_DIR = $(BASE_DIR)/iso

export CC = gcc
export AS = gcc
export LD = ld


export INCLUDE=$(SRC_DIR)/include

#gcc paranoia mode, more information (in French): http://www.bien-programmer.fr/codage.php#cfg_compilo
export CFLAGS= \
-std=gnu99 \
-DDEBUG \
-D__ARCH_$(ARCH)__ \
-D__SUBARCH_$(SUBARCH)__ \
-m32 \
-nodefaultlibs \
-ffreestanding \
-nostartfiles \
-fno-stack-protector \
-fno-builtin \
-nostdlib \
-nostdinc \
-mno-mmx \
-mno-sse \
-mno-sse2 \
-mno-sse3 \
-mno-3dnow \
-O1 \
-Wall \
-Wextra \
-Wwrite-strings \
-Wstrict-prototypes \
-Wuninitialized \
-Wunreachable-code \
-Wno-missing-braces \
-Wno-missing-field-initializers \
-Wchar-subscripts \
-Wno-main \
-Wcomment \
-Wformat=2 \
-Wimplicit-int \
-Werror-implicit-function-declaration \
-Wparentheses \
-Wsequence-point \
-Wreturn-type \
-Wswitch \
-Wtrigraphs \
-Wunused \
-Wunknown-pragmas \
-Wfloat-equal \
-Wundef \
-Wshadow \
-Wpointer-arith \
-Wbad-function-cast \
-Wconversion \
-Wsign-compare \
-Waggregate-return \
-Wstrict-prototypes \
-Wmissing-prototypes \
-Wmissing-declarations \
-Wformat \
-Wmissing-format-attribute \
-Wno-deprecated-declarations \
-Wpacked \
-Wredundant-decls \
-Wnested-externs \
-Wlong-long



export ASFLAGS = $(CFLAGS)

LDFLAGS = -m32 -melf_i386 -nodefaultlibs -no-stack-protector 
LDSCRIPT = $(SRC_DIR)/atros.lds


#Folders which contain source files
KERNEL_SRC_DIRS= arch/$(ARCH) \
		arch/$(ARCH)/boot \
		kernel \
		kernel/mm \
		libc

KERNEL_SRC_DIRS_LIST = $(addprefix $(SRC_DIR)/, $(KERNEL_SRC_DIRS))

KERNEL_C_SRC_FILES = $(shell find $(KERNEL_SRC_DIRS_LIST) -maxdepth 1 -type f -name "*.c")
KERNEL_ASM_SRC_FILES = $(shell find $(KERNEL_SRC_DIRS_LIST) -maxdepth 1 -type f -name "*.S")

KERNEL_C_OBJS = $(patsubst %.c, %.o, $(KERNEL_C_SRC_FILES))
KERNEL_ASM_OBJS = $(patsubst %.S, %.o, $(KERNEL_ASM_SRC_FILES))

KERNEL_DEP_FILES = $(patsubst %.c, %.d, $(KERNEL_C_SRC_FILES)) $(patsubst %.S, %.d, $(KERNEL_ASM_SRC_FILES))

-include $(KERNEL_DEP_FILES)

.PHONY: all qemu clean

all: $(TARGET_BIN)


todolist:
	-@for file in $(KERNEL_C_SRC_FILES:Makefile=); do fgrep -H -e TODO -e FIXME $$file; done; true

qemu: $(TARGET_BIN)
	@cp $(TARGET_BIN) $(ISO_DIR)/boot
	grub-mkrescue -o $(BASE_DIR)/atros.iso $(ISO_DIR)
	qemu-system-i386 -boot d -cdrom $(BASE_DIR)/atros.iso -m 16 -monitor stdio 

$(TARGET_BIN): $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS) linker.lds
	@$(LD) $(LDFLAGS) -T $(SRC_DIR)/linker.lds -S -X -Map $(SRC_DIR)/atros.map -o $(TARGET_BIN) --start-group $(KERNEL_C_OBJS) $(KERNEL_ASM_OBJS) --end-group

#Trick to preprocess the linker script as a C-file. Usefull to use constants or symbols defined in C-headers

%.o: %.c Makefile
	@$(CC) -I$(INCLUDE) $(CFLAGS) -MMD -MP -c $< -o $@
%.o: %.S Makefile
	@$(AS) -I$(INCLUDE) $(ASFLAGS) -MMD -MP -c $< -o $@

linker.lds:
	@$(CC) -I$(INCLUDE) -D__ARCH_$(ARCH)__ -D__SUBARCH_$(SUBARCH)__ -x c -P -E $(LDSCRIPT) -o $(SRC_DIR)/linker.lds

clean:
	@rm $(KERNEL_DEP_FILES)
	@rm $(KERNEL_C_OBJS)
	@rm $(KERNEL_ASM_OBJS)
	@rm $(SRC_DIR)/linker.lds
	@rm $(TARGET_BIN)
