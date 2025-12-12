CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -O2 -g \
         `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`

LDFLAGS = `pkg-config --libs gtk+-3.0 sdl2 SDL2_image` -lm -fsanitize=address

SRC = ui.c \
      ../core/image.c \
      ../pre_process/grayscale.c \
      ../pre_process/grid/grid_detection.c \
      ../pre_process/rotation.c \
      ../pre_process/scale.c \
	  ../pre_process/utils/shapes.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: a.out

a.out: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) a.out

