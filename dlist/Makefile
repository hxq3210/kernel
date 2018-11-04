
obj-m := demo.o demo2.o

#KERNEL	:= /home/xiaochaomi/linux-3.5/
KERNEL	:= /lib/modules/`uname -r`/build/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
