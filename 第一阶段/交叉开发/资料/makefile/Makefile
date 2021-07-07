TARGET := main

CC := gcc

CARCS += $(wildcard *.c)
OBJS += $(patsubst  %.c,%.o,$(CARCS))

LIBS += -L./ -lxxx
INCL += -I./
%.o:%.c
	$(CC) -c $^ -o $@ $(INCL)
	
$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(LIBS)
	cp $@ ~/tftp
	
clean:
	rm $(TARGET) $(OBJS)

