PWD := $(shell pwd)
obj-m += flash.o
KERNEL_DIR = /lib/modules/`uname -r`/build
all default: modules

install: modules_install

modules modules_install help clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) $@

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
	
	
