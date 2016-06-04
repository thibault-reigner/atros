ARCH := x86
SUBARCH := i686
TARGET := $(ARCH)

TARGET_BIN := atros.img


PWD:=$(shell pwd)

export CC=gcc
export AS=gcc
export LD=ld


export INCLUDE=$(PWD)/include

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
LDSCRIPT = atros.lds


#Folders which contain source files
KERNEL_SRC_DIR= arch/$(ARCH) \
		arch/$(ARCH)/boot \
		kernel \
		kernel/mm \
		libc \

KERNEL_SRCS_C = libc/stdarg.c \
		libc/string.c \
		kernel/main.c \
		kernel/kprintf.c \
		kernel/panic.c \
		kernel/mm/slab.c \
		kernel/mm/virtual_pages.c \
		kernel/mm/physical_pages.c \
		arch/x86/boot/bootloader_info.c \
		arch/x86/gdt.c \
		arch/x86/paging.c \
		arch/x86/mmu.c \
		arch/x86/tss.c \
		arch/x86/idt.c \
		arch/x86/interrupts.c \
		arch/x86/8259a.c \
		arch/x86/pit.c \
		arch/x86/cpucheck.c \
		arch/x86/cpu_context.c

KERNEL_SRCS_ASM = arch/x86/boot/head.S \
		  arch/x86/gdt_flush.S \
		  arch/x86/isr_entry.S \
		  arch/x86/cpu_context_switch.S

KERNEL_OBJS = $(KERNEL_SRCS_C:.c=.o) $(KERNEL_SRCS_ASM:.S=.o)

all: $(TARGET_BIN)

qemu: $(TARGET_BIN)
	@cp $(TARGET_BIN) ../iso/boot
	grub-mkrescue -o ../atros.iso ../iso/
	qemu-system-i386 -boot d -cdrom ../atros.iso -m 16 -monitor stdio 

$(TARGET_BIN): $(KERNEL_OBJS) linker.lds
	@$(LD) $(LDFLAGS) -T linker.lds -S -X -Map atros.map -o $(TARGET_BIN) --start-group $(KERNEL_OBJS) --end-group

$(KERNEL_OBJS): 
	-@for file in $(KERNEL_SRC_DIR); do (cd ./$$file && $(MAKE));  \
	 done; 

#Trick to preprocess the linker script as a C-file. Usefull to use constants or symbols defined in C-headers
linker.lds:
	@$(CC) -I$(INCLUDE) -D__ARCH_$(ARCH)__ -D__SUBARCH_$(SUBARCH)__ -x c -P -E $(LDSCRIPT) -o linker.lds

clean:									
	-@for file in $(KERNEL_SRC_DIR); do (cd ./$$file && rm -f *.o && rm -f *.*~);  \
	 done; 
	@rm linker.lds
