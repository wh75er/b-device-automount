mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))
userspace-monitoring := $(mkfile_dir)user-space/monitoring/monitoring.out
userspace-mount := $(mkfile_dir)user-space/mount/mount.out
EXTRA_CFLAGS += -DUSERSPACE_MONITORING='"$(userspace-monitoring)"'
EXTRA_CFLAGS += -DUSERSPACE_MOUNT='"$(userspace-mount)"'
obj-m += automount.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
