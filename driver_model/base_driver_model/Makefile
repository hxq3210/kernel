
obj-m := driver1.o driver2.o device1.o device2.o device3.o bus.o

KERNEL	:= /home/xiaochaomi/linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
