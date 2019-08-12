export CFLAGS := -std=gnu99 -Wall -Werror -pedantic
export PLATFORM := $(shell uname -s | tr '[:upper:]' '[:lower:]')
ALL_SRCS := $(shell find . -type f \( -name '*.c' -o -name '*.h' \))
PREFIX = /usr/local

all: module krfexec krfctl example

.PHONY: module
module:
	$(MAKE) -C src/module/$(PLATFORM) module

.PHONY: krfexec
krfexec:
	$(MAKE) -C src/krfexec

.PHONY: krfctl
krfctl:
	$(MAKE) -C src/krfctl

.PHONY: insmod
insmod:
	$(MAKE) -C src/module/$(PLATFORM) insmod

.PHONY: rmmod
rmmod:
	$(MAKE) -C src/module/$(PLATFORM) rmmod

.PHONY: example
example:
	$(MAKE) -C example

.PHONY: clean
clean:
	$(MAKE) -C src/module/$(PLATFORM) clean
	$(MAKE) -C src/krfexec clean
	$(MAKE) -C src/krfctl clean
	$(MAKE) -C example clean

.PHONY: fmt
fmt:
	clang-format -i -style=file $(ALL_SRCS)

.PHONY: install
install:
	install -d $(DESTDIR)$(PREFIX)/bin
	install src/krfexec/krfexec $(DESTDIR)$(PREFIX)/bin
	install src/krfctl/krfctl $(DESTDIR)$(PREFIX)/bin
