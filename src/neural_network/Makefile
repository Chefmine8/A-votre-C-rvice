CC = gcc
CFLAGS = -Wall -Wextra -g
SOURCES = main.c neural_network.c layer.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = reseau

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean all

clean:
	rm -f $(OBJECTS) $(TARGET)

