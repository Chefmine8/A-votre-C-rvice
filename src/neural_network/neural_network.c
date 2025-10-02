//
// Created by Evan on 01/10/2025.
//

#include "neural_network.h"

#include <stdlib.h>
#include <err.h>
#include "layer.h"
#include <stdio.h>


struct neural_network *create_network(int size_of_arr, int arr[size_of_arr])
{
     if (size_of_arr < 2) {
          errx(EXIT_FAILURE, "Too small for a brain");
     }
     struct neural_network *neural_network = malloc(sizeof(struct neural_network));

     neural_network->number_of_layers = size_of_arr;

     neural_network->inputs = malloc(sizeof(long double) * arr[0]);
     neural_network->input_size = arr[0];

     neural_network->outputs = malloc(sizeof(long double) * arr[size_of_arr - 1]);
     neural_network->output_size = arr[size_of_arr - 1];

     neural_network->layers = malloc(sizeof(struct layer *) * size_of_arr);

     neural_network->layers[0] = create_layer(arr[0], arr[0]);
     link_layer_input(neural_network->layers[0], neural_network->input_size, &neural_network->inputs);
     for(int i = 1; i < size_of_arr; i++)
     {
          neural_network->layers[i] = create_layer(arr[i - 1], arr[i]);
          link_layers(neural_network->layers[i - 1], neural_network->layers[i]);
     }

     link_layer_output(neural_network->layers[size_of_arr - 1], neural_network->output_size, &neural_network->outputs);
     return neural_network;
}

void free_neural_network(struct neural_network *neural_network) {
     free_layers(neural_network->layers[neural_network->number_of_layers - 1]);
     free(neural_network->outputs);
     free(neural_network);
}
