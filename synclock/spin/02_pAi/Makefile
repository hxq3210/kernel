
obj-m := demok.o
demok-objs := demo.o key.o

KERNEL	:= /home/xiaochaomi/linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
