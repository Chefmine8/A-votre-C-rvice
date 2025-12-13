CC = gcc -g
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm
SANIT = -fsanitize=address
SRC = importTrainingData.c \
      ../core/image.c \
      ../neural_network/layer.c \
      ../neural_network/neural_network.c \
      ../pre_process/scale.c

OBJ = $(SRC:.c=.o)

TARGET = training

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

sanit:$(OBJ)
	$(CC) $(CFLAGS) $(SANIT) $(OBJ) -o san $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) san

.PHONY: all clean run
