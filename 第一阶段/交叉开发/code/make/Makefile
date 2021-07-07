TARGET := main

CC := gcc

LIBS += -L./  -lxxx

INCS += -I./

SRCS += $(wildcard *.c) 

OBJS += $(patsubst %.c,%.o,$(SRCS))


$(TARGET):$(OBJS)
	$(CC) $^ -o $@  $(INCS) $(LIBS)
	cp $@ /home/gec/tftp
	
%.o:%.c
	$(CC) -c $^ -o $@
	
clean:
	rm -rf $(TARGET) $(OBJS)

