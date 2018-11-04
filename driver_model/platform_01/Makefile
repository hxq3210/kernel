
obj-m := drv.o dev.o

KERNEL	:= /home/xiaochaomi/linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
