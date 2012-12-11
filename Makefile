obj-m 	:= serialfc.o
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
IGNORE	:=
serialfc-objs := main.o

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	cp serialfc.ko /lib/modules/`uname -r`/kernel/drivers/tty/serial/8250/
	depmod

uninstall:
	rm /lib/modules/`uname -r`/kernel/drivers/tty/serial/8250/serialfc.ko
	depmod

clean:
	@find . $(IGNORE) \
	\( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.markers' -o -name '*.symvers' -o -name '*.order' \) \
		-type f -print | xargs rm -f
	rm -rf .tmp_versions

