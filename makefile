CC = gcc



CFLAGS = -Wall -I$(SRC_DIR) -Wextra -g `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
LDFLAGS =  `pkg-config --libs gtk+-3.0 sdl2 SDL2_image` -lm

SRC = src/core/*.c \
	  src/link/link_cells_to_nn.c \
	  src/neural_network/layer.c \
	  src/neural_network/neural_network.c \
	  src/pre_process/grayscale.c \
	  src/pre_process/scale.c \
	  src/pre_process/grid/*.c \
	  src/pre_process/rotation/*.c \
	  src/pre_process/utils/*.c \
	  src/solver/*.c \
	  src/ui/*.c

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: a.out

a.out: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) a.out
	rm -rf ./output

