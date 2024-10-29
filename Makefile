CC = gcc
CFLAGS = -O3 -std=c99 -Wall -Wshadow -Wvla -pedantic
EXEC = a6
SRCS = main.c tree_utils.c
OBJS = $(SRCS:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean