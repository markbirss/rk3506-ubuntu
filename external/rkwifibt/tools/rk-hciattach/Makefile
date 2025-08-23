CFLAGS := -Wall -g
#CC := $(CROSS_COMPLE)gcc
all: rk_hciattach
OBJS := hciattach.o hciattach_rk.o

rtk_hciattach: $(OBJS)
	$(CC) -o rk_hciattach $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)  rk_hciattach

tags: FORCE
	ctags -R
	find ./ -name "*.h" -o -name "*.c" -o -name "*.cc" -o -name "*.cpp" > cscope.files
	cscope -bkq -i cscope.files
PHONY += FORCE
FORCE:
