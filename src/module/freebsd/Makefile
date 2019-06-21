# NOTE(ww): The targets in this file intentionally use `make`
# instead of `$(MAKE)`, since we expect `$(MAKE)` to be GNU Make
# and not the BSD `make` that the FreeBSD module build requires.

.PHONY: all
all: module

.PHONY: module
module: codegen
	make -f Makefile.module all

.PHONY: codegen
codegen:
	ruby ../codegen/codegen freebsd

.PHONY: insmod
insmod:
	sudo make -f Makefile.module load

.PHONY: rmmod
rmmod:
	sudo make -f Makefile.module unload

.PHONY: clean
clean:
	make -f Makefile.module clean
	rm -rf *.gen.c *.gen.x *.gen.h syscalls/*.gen.c syscalls/*.gen.h
