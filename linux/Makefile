obj-m += rootkit.o kthread.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules

log:
	dmesg

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
