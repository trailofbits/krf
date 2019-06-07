MOD := krfx

# Ignore this insanity: we need to do some pathname rewriting
# thanks to the subprocess make that actually does the building.
KRF_SYSCALL_SRCS_FAKE := $(notdir $(wildcard $M/syscalls/*.c))
KRF_SYSCALL_OBJS_FAKE := $(KRF_SYSCALL_SRCS_FAKE:.c=.o)
KRF_SYSCALL_OBJS = $(foreach obj,$(KRF_SYSCALL_OBJS_FAKE),syscalls/$(obj))
KRF_SYSCALL_YMLS := $(wildcard codegen/syscalls/*.yml)
KRF_HEADER_FILES := $(notdir $(wildcard $M/*.h))
ccflags-y := -DKRF_CODEGEN=1

obj-m += $(MOD).o
$(MOD)-objs := krf.o config.o syscalls.o $(KRF_SYSCALL_OBJS)

all: ruby_codegen $(KRF_HEADER_FILES)
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

# .PHONY: codegen # Cannot be phony because phony are always run
ruby_codegen: $(KRF_SYSCALL_YMLS) codegen/codegen
	ruby codegen/codegen
	@touch ruby_codegen # Stops it from always being run

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	rm -f *.ur-safe # some garbage not cleaned by the kernel's clean target
	rm -f *.gen.x *.gen.h */*.gen.h */*.gen.c ruby_codegen # codegen files

insmod: krfx.ko
	sudo insmod $(MOD).ko

rmmod:
	sudo rmmod $(MOD)
