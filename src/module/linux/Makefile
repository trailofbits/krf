MOD := krfx

# Ignore this insanity: we need to do some pathname rewriting
# thanks to the subprocess make that actually does the building.
KRF_SYSCALL_SRCS_FAKE := $(notdir $(wildcard $M/syscalls/*.c))
KRF_SYSCALL_OBJS_FAKE := $(KRF_SYSCALL_SRCS_FAKE:.c=.o)
KRF_SYSCALL_OBJS = $(foreach obj,$(KRF_SYSCALL_OBJS_FAKE),syscalls/$(obj))
KRF_SYSCALL_YMLS = $(wildcard ../codegen/linux/*.yml)

ccflags-y := -DKRF_CODEGEN=1 -DLINUX -std=gnu99 -Wno-declaration-after-statement

obj-m += $(MOD).o
$(MOD)-objs := krf.o syscalls.o netlink.o ../krf.o ../config.o $(KRF_SYSCALL_OBJS)

.PHONY: all
all: module

.PHONY: module
module: ../codegen/linux/.linux.mk
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

.PHONY: codegen
codegen: ../codegen/linux/.linux.mk

../codegen/linux/.linux.mk: ../codegen/linux/codegen $(KRF_SYSCALL_YMLS)
	ruby ../codegen/linux/codegen $(FAULTS)
	@touch ../codegen/linux/.linux.mk

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -f ../*.o *.ur-safe ../*.ur-safe # some garbage not cleaned by the kernel's clean target
	rm -f *.gen.x *.gen.h */*.gen.h */*.gen.c ../codegen/linux/.linux.mk # codegen files

.PHONY: install
install: $(MOD).ko
	sudo $(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules_install
	sudo depmod -a

.PHONY: insmod
insmod: $(MOD).ko
	sudo insmod $(MOD).ko

.PHONY: rmmod
rmmod:
	sudo rmmod $(MOD)
