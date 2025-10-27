//
// Created by Evan on 01/10/2025.
//

#include "neural_network.h"
#include <stdlib.h>
#include <err.h>
#include "layer.h"
#include <stdio.h>
#include <math.h>

struct neural_network *create_neural_network(int size_of_arr, int arr[size_of_arr])
{

     if (size_of_arr <= 2) {
          errx(EXIT_FAILURE, "Do you have a brain of that size ?");
     }
     struct neural_network *neural_network = malloc(sizeof(struct neural_network));

     neural_network->number_of_layers = size_of_arr;

     neural_network->inputs = malloc(sizeof(long double) * arr[0]);
     neural_network->input_size = arr[0];

     neural_network->outputs = malloc(sizeof(long double *));
     neural_network->output_size = arr[size_of_arr - 1];

     neural_network->layers = malloc(sizeof(struct layer *) * size_of_arr);

     neural_network->layers[0] = create_layer(arr[0], arr[0]);

     link_layer_input(neural_network->layers[0], neural_network->input_size, &neural_network->inputs);

     for(int i = 1; i < size_of_arr; i++)
     {

          neural_network->layers[i] = create_layer(arr[i - 1], arr[i]);

          link_layers(&neural_network->layers[i - 1], &neural_network->layers[i]);

     }

     link_layer_output(neural_network->layers[size_of_arr - 1], neural_network);

     return neural_network;
}


void free_neural_network(struct neural_network *neural_network) {
     free_layers(neural_network->layers[neural_network->number_of_layers - 1]);
     free(neural_network->outputs);
     free(neural_network->inputs);
     free(neural_network);
}


void check_neural_network(const struct neural_network *neural_network) {

     neural_network->inputs[0] = 69;

     ((*(neural_network->outputs))[0]) = 69;

     check_layer(neural_network->layers[neural_network->number_of_layers - 1]);

}


void neural_network_calculate_output(const struct neural_network *neural_network) {
     layer_calculate_output( neural_network->layers[neural_network->number_of_layers -1] );
}

/**
 * Calculate the loss of the neural network using the categorical cross-entropy function
 * @param neural_network The neural Network
 * @param expected_output The expected output
 * @return The calculated loss
 */
long double categorical_cross_entropy(const struct neural_network *neural_network, char expected_output) {
     printf("1.1 %c\n", expected_output);
     return - log((*(neural_network->outputs))[expected_output - 'A']);
}
char get_neural_network_output(const struct neural_network *neural_network) {
     long double max = (*neural_network->outputs)[0];
     for (int i = 1; i < neural_network->output_size; ++i) {
          if (max < (*neural_network->outputs)[i]) {
               max = (*neural_network->outputs)[i];
          }
     }
     for (int i = 0; i < neural_network->output_size; ++i) {
          if (max == (*neural_network->outputs)[i]) {
               return i + 'A';
          }
     }
     errx(EXIT_FAILURE, "wtf");
}
void minimise_loss(const struct neural_network *neural_network, char expected_output, long double shift) {

     for (int i = 0; i < neural_network->number_of_layers; ++i) {
          struct layer *layer = neural_network->layers[i];

          for (int j = 0; j < layer->layer_size; ++j) {

               for (int k = 0; k < layer->prev_layer_size; ++k) {
                    long double original_weight = layer->weights[j][k];
                    neural_network_calculate_output(neural_network);
                    long double current_loss = categorical_cross_entropy(neural_network, expected_output);
                    if (current_loss <= 1.0e-10) {
                         break;
                    }
                    layer->weights[j][k] += shift;
                    neural_network_calculate_output(neural_network);
                    long double plus_shift_loss = categorical_cross_entropy(neural_network, expected_output);

                    layer->weights[j][k] -= shift*2;
                    neural_network_calculate_output(neural_network);
                    long double minus_shift_loss = categorical_cross_entropy(neural_network, expected_output);

                    if (current_loss < minus_shift_loss && current_loss < plus_shift_loss) {
                         layer->weights[j][k] = original_weight;
                    }
                    else if (plus_shift_loss < current_loss && plus_shift_loss < minus_shift_loss) {
                         layer->weights[j][k] += shift*2;
                    }

                    if (isnanf(categorical_cross_entropy(neural_network, expected_output))) {
                         errx(EXIT_FAILURE, "Loss is nan");
                    }
                    if (current_loss != categorical_cross_entropy(neural_network, expected_output)) {
                         printf("Loss: %Lg -> %Lg\n", current_loss, categorical_cross_entropy(neural_network, expected_output));
                         printf("Excepted: %c, Got: %c\n", expected_output, get_neural_network_output(neural_network));
                    }
               }

               long double original_bias = layer->biases[j];
               neural_network_calculate_output(neural_network);
               long double current_loss = categorical_cross_entropy(neural_network, expected_output);
               if (current_loss <= 1.0e-10) {
                    break;
               }
               layer->biases[j] += shift;
               neural_network_calculate_output(neural_network);
               long double plus_shift_loss = categorical_cross_entropy(neural_network, expected_output);

               layer->biases[j] -= shift*2;
               neural_network_calculate_output(neural_network);
               long double minus_shift_loss = categorical_cross_entropy(neural_network, expected_output);

               if (current_loss < minus_shift_loss && current_loss < plus_shift_loss) {
                    layer->biases[j] = original_bias;
               }
               else if (plus_shift_loss < current_loss && plus_shift_loss < minus_shift_loss) {
                    layer->biases[j] += shift*2;
               }
          }
     }
}