export CFLAGS := -std=gnu99 -Wall -Werror -pedantic

ALL_SRCS := $(shell find . -type f \( -name '*.c' -o -name '*.h' \) )

# OS detection - manually can be controlled by `make PLATFORM=FreeBSD` or whatever
PLATFORM := $(shell uname -s)

all: module krfexec krfctl example

.PHONY: module
module:
ifeq ($(PLATFORM),Linux)
	$(MAKE) -C src/module/linux
endif
ifeq ($(PLATFORM),FreeBSD)
	$(MAKE) -C src/module/freebsd
endif

.PHONY: krfexec
krfexec:
	$(MAKE) -C src/krfexec

.PHONY: krfctl
krfctl:
	$(MAKE) -C src/krfctl

.PHONY: insmod
insmod:
	$(MAKE) -C src/module/linux insmod

.PHONY: rmmod
rmmod:
	$(MAKE) -C src/module/linux rmmod

.PHONY: example
example:
	$(MAKE) -C example

.PHONY: clean
clean:
ifeq ($(PLATFORM),Linux)
	$(MAKE) -C src/module/linux clean
endif
ifeq ($(PLATFORM),FreeBSD)
	$(MAKE) -C src/module/freebsd clean
endif
	$(MAKE) -C src/krfexec clean
	$(MAKE) -C src/krfctl clean
	$(MAKE) -C example clean

.PHONY: fmt
fmt:
	clang-format -i -style=file $(ALL_SRCS)
