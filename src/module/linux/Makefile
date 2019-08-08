MOD := krfx

# Ignore this insanity: we need to do some pathname rewriting
# thanks to the subprocess make that actually does the building.
KRF_SYSCALL_SRCS_FAKE := $(notdir $(wildcard $M/syscalls/*.c))
KRF_SYSCALL_OBJS_FAKE := $(KRF_SYSCALL_SRCS_FAKE:.c=.o)
KRF_SYSCALL_OBJS = $(foreach obj,$(KRF_SYSCALL_OBJS_FAKE),syscalls/$(obj))
KRF_SYSCALL_YMLS = $(wildcard ../codegen/$(PLATFORM)/*.yml)

ccflags-y := -DKRF_CODEGEN=1 -DLINUX

obj-m += $(MOD).o
$(MOD)-objs := krf.o syscalls.o ../krf.o ../config.o $(KRF_SYSCALL_OBJS)

.PHONY: module
module: all

.PHONY: all
all: ../codegen/.$(PLATFORM).mk
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

.PHONY: codegen
codegen: ../codegen/.$(PLATFORM).mk

../codegen/.$(PLATFORM).mk: ../codegen/codegen $(KRF_SYSCALL_YMLS)
	ruby ../codegen/codegen linux $(FAULTS)
	@touch ../codegen/.$(PLATFORM).mk

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -f ../*.o *.ur-safe ../*.ur-safe # some garbage not cleaned by the kernel's clean target
	rm -f *.gen.x *.gen.h */*.gen.h */*.gen.c ../codegen/.$(PLATFORM).mk # codegen files

insmod: krfx.ko
	sudo insmod $(MOD).ko

rmmod:
	sudo ../../krfctl/krfctl -c
	sudo rmmod $(MOD)
