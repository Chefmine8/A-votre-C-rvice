CC = gcc
CFLAGS = -Wall -Wextra
SANIT = -g -fsanitize=address

CFLAGS += $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image

SOURCES = main.c link_cells_to_nn.c
CORE = ../core/image.c 
PRE_PROCESS = ../pre_process/scale.c
TARGET = link_cells

all: simple

simple:
	$(CC) -o $(TARGET) $(CFLAGS) $(SOURCES) $(CORE) $(PRE_PROCESS) $(LDFLAGS) 

fsanit:
	$(CC) -o $(TARGET) $(CFLAGS) $(SANIT) $(SOURCES) $(CORE) $(PRE_PROCESS) $(LDFLAGS)


.PHONY: all clean simple fsanit

clean:
	rm -f $(TARGET)

