CC = gcc
CFLAGS = -Wall -Wextra

SOURCES = neural_network.c main.c layer.c
TARGET = neural_net

all:simple
LDFLAGS =  -lm
simple:
	$(CC) -o $(TARGET) $(CFLAGS) $(SOURCES) $(LDFLAGS)


clean:
	rm -f $(TARGET)
