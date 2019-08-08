PROG := krfctl
SRCS := $(PROG).c table.gen.c profiles.gen.c $(wildcard ./$(PLATFORM)/*.c)
OBJS := $(SRCS:.c=.o)

.PHONY: all
all: $(PROG)

table.gen.c: gentable
	ruby gentable

profiles.gen.c: genprofiles
	ruby genprofiles

$(OBJS): $(SRCS)

$(PROG): $(OBJS)

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
	rm -f *.gen.c # gentable/genprofiles files
