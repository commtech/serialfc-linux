obj-m 	:= serialfc.o
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
IGNORE	:=
serialfc-objs := src/main.o src/card.o src/port.o src/utils.o src/sysfs.o

ifeq ($(DEBUG),1)
	EXTRA_CFLAGS += -DDEBUG
endif

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

install:
	cp serialfc.rules /etc/udev/rules.d/
	cp serialfc.ko /lib/modules/`uname -r`/kernel/drivers/char/
	echo 'serialfc' >> /etc/modules
	cp lib/raw/serialfc.h /usr/local/include/
	depmod

uninstall:
	rm /etc/udev/rules.d/serialfc.rules
	rm /lib/modules/`uname -r`/kernel/drivers/char/serialfc.ko
	sed --in-place '/serialfc/d' /etc/modules
	rm /usr/local/include/serialfc.h
	depmod

clean:
	@find . $(IGNORE) \
	\( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.markers' -o -name '*.symvers' -o -name '*.order' \) \
		-type f -print | xargs rm -f
	rm -rf .tmp_versions

help:
	@echo
	@echo 'Build targets:'
	@echo '  make - Build driver module'
	@echo '  make clean - Remove most generated files'
	@echo '  make install - Installs FSCC driver'
	@echo '  make uninstall - Clean up installation'
	@echo
