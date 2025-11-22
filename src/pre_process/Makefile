# Nom de l'exécutable
TARGET = test

CC = gcc
SANFLAGS = -fsanitize=address
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

SRCS = $(wildcard ../pre_process/*.c ../core/*.c ../pre_process/utils/*.c ../pre_process/grid/*.c)

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

# Lien des fichiers objets
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.bmp
	rm -f ../../resources/pre_process/output/grid/*/*/*.bmp
	rm -f ../../resources/pre_process/output/grid/*.bmp
.PHONY: all clean
