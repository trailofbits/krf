PROG := krfctl

all: gentable $(PROG)

.PHONY: gentable
gentable:
	ruby gentable

table.gen.c: gentable

$(PROG): $(PROG).o table.gen.o

.PHONY: clean
clean:
	rm -f $(PROG) *.o
	rm -f *.gen.c # gentable files
