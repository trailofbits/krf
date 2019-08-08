MOD := krfx

# Ignore this insanity: we need to do some pathname rewriting
# thanks to the subprocess make that actually does the building.
KRF_SYSCALL_SRCS_FAKE := $(notdir $(wildcard $M/syscalls/*.c))
KRF_SYSCALL_OBJS_FAKE := $(KRF_SYSCALL_SRCS_FAKE:.c=.o)
KRF_SYSCALL_OBJS = $(foreach obj,$(KRF_SYSCALL_OBJS_FAKE),syscalls/$(obj))
KRF_SYSCALL_YMLS = $(wildcard ../codegen/linux/*.yml)

ccflags-y := -DKRF_CODEGEN=1 -DLINUX

obj-m += $(MOD).o
$(MOD)-objs := krf.o syscalls.o netlink.o ../krf.o ../config.o $(KRF_SYSCALL_OBJS)

.PHONY: all
all: module

.PHONY: module
module: ../codegen/.linux.mk
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

.PHONY: codegen
codegen: ../codegen/.linux.mk

../codegen/.linux.mk: ../codegen/codegen $(KRF_SYSCALL_YMLS)
	ruby ../codegen/codegen linux $(FAULTS)
	@touch ../codegen/.linux.mk

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -f ../*.o *.ur-safe ../*.ur-safe # some garbage not cleaned by the kernel's clean target
	rm -f *.gen.x *.gen.h */*.gen.h */*.gen.c ../codegen/.linux.mk # codegen files

.PHONY: install
install: $(MOD).ko
	sudo $(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules_install
	sudo depmod -a

.PHONY: insmod
insmod: $(MOD).ko
	sudo modprobe $(MOD) --first-time

.PHONY: rmmod
rmmod:
	sudo modprobe -r $(MOD) --first-time
