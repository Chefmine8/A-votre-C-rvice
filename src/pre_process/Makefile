# Nom de l'exécutable
TARGET = test

CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image

SRCS = $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

# Lien des fichiers objets
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.bmp

.PHONY: all clean
