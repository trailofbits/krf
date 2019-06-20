.PHONY: all
all: module

.PHONY: module
module: codegen
	$(MAKE) -f Makefile.module all

.PHONY: codegen
codegen:
	ruby ../codegen/codegen freebsd

.PHONY: insmod
insmod:
	sudo $(MAKE) -f Makefile.module load

.PHONY: rmmod
rmmod:
	sudo $(MAKE) -f Makefile.module unload

.PHONY: clean
clean:
	$(MAKE) -f Makefile.module clean
	rm -rf *.gen.c *.gen.x *.gen.h syscalls/*.gen.c syscalls/*.gen.h
