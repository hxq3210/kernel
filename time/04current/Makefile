
obj-m := demo1.o demo.o

KERNEL	:= /home/xiaochaomi/linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
