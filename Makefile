CFLAGS := -std=gnu99 -Wall -Werror -pedantic
ALL_SRCS := `find . -type f \( -name '*.c' -o -name '*.h' \)`
PLATFORM := `uname -s | tr '[:upper:]' '[:lower:]'`

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
