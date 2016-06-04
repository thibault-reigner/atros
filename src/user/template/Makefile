ARCH := x86
SUBARCH := i686
TARGET := $(ARCH)

TARGET_BIN := test.bin

#répertoire courant
PWD:=$(shell pwd)

export CC=gcc
export AS=gcc
export LD=ld



#gcc paranoia mode, more information (in French): http://www.bien-programmer.fr/codage.php#cfg_compilo
export CFLAGS= \
-DDEBUG \
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


export ASFLAGS = $(CFLAGS)

LDFLAGS =  -nodefaultlibs -no-stack-protector 
LDSCRIPT = user.lds

INCLUDE = 
SRCS_C = main.c
SRCS_ASM = crt0.S

OBJS = $(SRCS_C:.c=.o) $(SRCS_ASM:.S=.o)

all: $(TARGET_BIN)


$(TARGET_BIN): $(OBJS) 
	@$(LD) $(LDFLAGS) -T $(LDSCRIPT) -S -X  --oformat binary -o $(TARGET_BIN) --start-group $(OBJS) --end-group

$(OBJS): 

.c.o:
	@$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@
.S.o:
	@$(AS) -I$(INCLUDE) $(ASFLAGS) -c $< -o $@

clean:
	@rm -rf *.o
