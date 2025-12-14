CC = gcc



CFLAGS = -Wall -I$(SRC_DIR) -Wextra -g `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
LDFLAGS =  `pkg-config --libs gtk+-3.0 sdl2 SDL2_image` -lm

SRC = ui.c \
      ../core/image.c \
      ../pre_process/grayscale.c \
      ../pre_process/grid/grid_detection.c \
      ../pre_process/rotation/rotation.c \
      ../pre_process/scale.c \
  ../pre_process/utils/shapes.c \
  ../link/link_cells_to_nn.c \
  ../neural_network/neural_network.c \
  ../neural_network/layer.c

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

