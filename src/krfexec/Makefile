PROG := krfexec
SRCS := $(PROG).c $(wildcard $(PLATFORM)/*.c)
OBJS := $(SRCS:.c=.o)

all: $(PROG)

$(PROG): $(OBJS)

.PHONY: clean
clean:
	rm -f $(PROG) $(OBJS)
