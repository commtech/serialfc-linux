obj-m 	:= fc-async.o
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
IGNORE	:=
fc-async-objs := main.o
	
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	@find . $(IGNORE) \
	\( -name '*.[oas]' -o -name '*.ko' -o -name '.*.cmd' \
		-o -name '.*.d' -o -name '.*.tmp' -o -name '*.mod.c' \
		-o -name '*.markers' -o -name '*.symvers' -o -name '*.order' \) \
		-type f -print | xargs rm -f
	rm -rf .tmp_versions

