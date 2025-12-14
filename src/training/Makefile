CC = gcc -g
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

SRC = importTrainingData.c \
      ../core/image.c \
      ../neural_network/layer.c \
      ../neural_network/neural_network.c \
      ../pre_process/scale.c \
      ../pre_process/grayscale.c

OBJ = $(SRC:.c=.o)

TARGET = training

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean run
