SRCC=$(wildcard *.c)
SRCASM=$(wildcard *.S)
OBJS=$(SRCC:.c=.o) $(SRCASM:.S=.o)

all:$(OBJS) 

$(OBJS):

.c.o:
	@$(CC) -I$(INCLUDE) $(CFLAGS) -c $< -o $@
.S.o:
	@$(AS) -I$(INCLUDE) $(ASFLAGS) -c $< -o $@

clean:
	@rm -rf *.o

