SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
PROGS = $(basename $(OBJS))

all: $(PROGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGS)
