PROG := krfctl
SRCS := $(PROG).c table.gen.c profiles.gen.c $(wildcard ./$(PLATFORM)/*.c)
OBJS := $(SRCS:.c=.o)

all: gentable $(PROG)

.PHONY: gentable
gentable:
	ruby gentable

.PHONY: genprofiles
genprofiles:
	ruby genprofiles

table.gen.c: gentable
profiles.gen.c: genprofiles

$(OBJS): $(SRCS)

$(PROG): $(OBJS)

.PHONY: clean
clean:
	rm -f $(PROG) *.o
	rm -f *.gen.c # gentable/genprofiles files
