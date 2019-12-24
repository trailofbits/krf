# NOTE(ww): The targets in this file intentionally use `make`
# instead of `$(MAKE)`, since we expect `$(MAKE)` to be GNU Make
# and not the BSD `make` that the FreeBSD module build requires.
KRF_SYSCALL_YMLS = $(wildcard ../codegen/freebsd/*.yml)

.PHONY: all
all: module

.PHONY: module
module: ../codegen/freebsd/.freebsd.mk
	make -f Makefile.module all

.PHONY: codegen
codegen: ../codegen/freebsd/.freebsd.mk

../codegen/freebsd/.freebsd.mk: ../codegen/freebsd/codegen $(KRF_SYSCALL_YMLS)
	ruby ../codegen/freebsd/codegen $(FAULTS)
	@touch ../codegen/freebsd/.freebsd.mk

.PHONY: insmod
insmod:
	sudo make -f Makefile.module load

.PHONY: rmmod
rmmod:
	sudo make -f Makefile.module unload

.PHONY: install
install:
	sudo make -f Makefile.module install

.PHONY: clean
clean:
	make -f Makefile.module clean
	rm -rf *.gen.c *.gen.x *.gen.h syscalls/*.gen.c syscalls/*.gen.h ../codegen/freebsd/.freebsd.mk
