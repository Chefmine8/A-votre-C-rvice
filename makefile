CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -O2 -g

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: a.out

a.out: $(OBJ)
	$(CC) $(CFLAGS) -o $* $^

%.c:
	$(CC) $(CFLAGS) -o $* $<

%: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o a.out $(basename $(SRC))
