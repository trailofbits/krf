MOD := krfx

# Ignore this insanity: we need to do some pathname rewriting
# thanks to the subprocess make that actually does the building.
KRF_SYSCALL_SRCS_FAKE := $(notdir $(wildcard $M/syscalls/*.c))
KRF_SYSCALL_OBJS_FAKE := $(KRF_SYSCALL_SRCS_FAKE:.c=.o)
KRF_SYSCALL_OBJS = $(foreach obj,$(KRF_SYSCALL_OBJS_FAKE),syscalls/$(obj))

ccflags-y := -DKRF_CODEGEN=1

obj-m += $(MOD).o
$(MOD)-objs := krf.o config.o syscalls.o $(KRF_SYSCALL_OBJS)

all: codegen
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

.PHONY: codegen
codegen:
	ruby codegen/codegen

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -f *.ur-safe # some garbage not cleaned by the kernel's clean target
	rm -f *.gen.x *.gen.h */*.gen.h */*.gen.c # codegen files

insmod: all
	sudo insmod $(MOD).ko

rmmod:
	sudo rmmod $(MOD)
