export CFLAGS := -std=gnu99 -Wall -Werror -pedantic
export PLATFORM := $(shell uname -s | tr '[:upper:]' '[:lower:]')

CLANG_FORMAT := clang-format
ALL_SRCS := $(shell find . -type f \( -name '*.c' -o -name '*.h' \))
PREFIX = /usr/local

all: module krfexec krfctl krfmesg example

.PHONY: module
module:
	$(MAKE) -C src/module/$(PLATFORM) module

.PHONY: krfexec
krfexec:
	$(MAKE) -C src/krfexec

.PHONY: krfctl
krfctl:
	$(MAKE) -C src/krfctl

.PHONY: krfmesg
krfmesg:
	$(MAKE) -C src/krfmesg

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
	$(CLANG_FORMAT) -i -style=file $(ALL_SRCS)

.PHONY: install-module
install-module: module
	$(MAKE) -C src/module/$(PLATFORM) install

.PHONY: install-utils
install-utils: krfexec krfctl krfmesg
	install -d $(DESTDIR)$(PREFIX)/bin
	install src/krfexec/krfexec $(DESTDIR)$(PREFIX)/bin
	install src/krfctl/krfctl $(DESTDIR)$(PREFIX)/bin
	install src/krfmesg/krfmesg $(DESTDIR)$(PREFIX)/bin

.PHONY: install
install: install-module install-utils
