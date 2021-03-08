
obj-m := rootkit.o init_kthread.o hrtimer.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules

log:
	dmesg -w

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
