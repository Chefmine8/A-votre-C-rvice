CC = gcc
CFLAGS = -Wall -Wextra
SANIT = -g -fsanitize=address
SOURCES = main.c link_cells_to_nn.c
TARGET = link_cells

all: simple

simple:
	$(CC) -o $(TARGET) $(CFLAGS) $(SOURCES)

fsanit:
	$(CC) -o $(TARGET) $(CFLAGS) $(SANIT) $(SOURCES)

.PHONY: all clean simple fsanit

clean:
	rm -f $(TARGET)

