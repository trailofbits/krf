# NOTE(ww): The targets in this file intentionally use `make`
# instead of `$(MAKE)`, since we expect `$(MAKE)` to be GNU Make
# and not the BSD `make` that the FreeBSD module build requires.
KRF_SYSCALL_YMLS = $(wildcard ../codegen/$(PLATFORM)/*.yml)

.PHONY: all
all: module

.PHONY: module
module: ../codegen/.$(PLATFORM).mk
	make -f Makefile.module all

.PHONY: codegen
codegen: ../codegen/.$(PLATFORM).mk

../codegen/.$(PLATFORM).mk: ../codegen/codegen $(KRF_SYSCALL_YMLS)
	ruby ../codegen/codegen freebsd $(FAULTS)
	@touch ../codegen/.$(PLATFORM).mk

.PHONY: insmod
insmod:
	sudo make -f Makefile.module load

.PHONY: rmmod
rmmod:
	sudo make -f Makefile.module unload

.PHONY: clean
clean:
	make -f Makefile.module clean
	rm -rf *.gen.c *.gen.x *.gen.h syscalls/*.gen.c syscalls/*.gen.h ../codegen/.$(PLATFORM).mk
