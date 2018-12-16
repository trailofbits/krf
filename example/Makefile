SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
PROGS = $(basename $(OBJS))

all: $(PROGS)

$(PROGS): %: %.c
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJS) $(PROGS)
