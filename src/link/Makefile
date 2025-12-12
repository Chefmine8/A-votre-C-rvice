CC = gcc
CFLAGS = -Wall -Wextra
SANIT = -fsanitize=address
GDB = -g

CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lm

SOURCES = link_cells_to_nn.c main.c
CORE = ../core/image.c 
PRE_PROCESS = ../pre_process/scale.c
NN = ../neural_network/neural_network.c ../neural_network/layer.c
TARGET = link_cells

all: simple

simple:
	$(CC) -o $(TARGET) $(CFLAGS) $(SOURCES) $(CORE) $(PRE_PROCESS) $(NN) $(LDFLAGS) 

fsanit:
	$(CC) -o $(TARGET) $(CFLAGS) $(SANIT) $(SOURCES) $(CORE) $(PRE_PROCESS) $(NN) $(LDFLAGS)

gdb:
	$(CC) -o $(TARGET) $(CFLAGS) $(GDB) $(SOURCES) $(CORE) $(PRE_PROCESS) $(LDFLAGS) 



.PHONY: all clean simple fsanit

clean:
	rm -f $(TARGET) img

