export CFLAGS := -std=gnu99 -Wall -Werror -pedantic

ALL_SRCS := $(shell find . -type f \( -name '*.c' -o -name '*.h' \) )

all: module krfexec krfctl example

.PHONY: module
module:
	$(MAKE) -C src/module

.PHONY: krfexec
krfexec:
	$(MAKE) -C src/krfexec

.PHONY: krfctl
krfctl:
	$(MAKE) -C src/krfctl

.PHONY: insmod
insmod:
	$(MAKE) -C src/module insmod

.PHONY: rmmod
rmmod:
	$(MAKE) -C src/module rmmod

.PHONY: example
example:
	$(MAKE) -C example

.PHONY: clean
clean:
	$(MAKE) -C src/module clean
	$(MAKE) -C src/krfexec clean
	$(MAKE) -C src/krfctl clean
	$(MAKE) -C example clean

.PHONY: fmt
fmt:
	clang-format -i -style=file $(ALL_SRCS)
