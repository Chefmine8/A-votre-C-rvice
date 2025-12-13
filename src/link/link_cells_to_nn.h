
#ifndef LINK_CELLS_TO_NN
#define LINK_CELLS_TO_NN

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <dirent.h>
#include <string.h>
#include <stddef.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>

#include "../neural_network/neural_network.h"
#include "../core/image.h"
#include "../pre_process/scale.h"

char* get_dir_data(char* identifiant);

void create_grid(char* id,struct neural_network* neural_net);

int get_row(char* str);

int get_column(char* str);

char* get_word_letter(int n_word,int n_letter);

void create_word_list(char* id,struct neural_network* neural_net);

#endif

