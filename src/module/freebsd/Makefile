SYSCALL_C_FILES=$(wildcard *.gen.c)
SRCS=krf.c syscalls.c ../config.c ../krf.c $(SYSCALL_C_FILES)
KMOD=krf

.PHONY: insmod
insmod:
	$(MAKE) load

.PHONY: rmmod
rmmod:
	$(MAKE) unload

$(SYSCALL_C_FILES): codegen

.PHONY: codegen
codegen:
	ruby ../codegen/codegen freebsd

.include <bsd.kmod.mk>

.PHONY: module
module: codegen all
