SRCC=$(wildcard *.c)
SRCASM=$(wildcard *.S)
OBJS=$(SRCC:.c=.o) $(SRCASM:.S=.o) 

all: $(OBJS)
	
$(OBJS):

%.o: %.c
	@$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@

%.o: %.S
	@$(AS) -I$(INCLUDE) $(ASFLAGS)  -c $< -o $@
