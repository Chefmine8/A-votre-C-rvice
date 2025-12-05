//
// Created by Evan on 01/10/2025.
//

#include "neural_network.h"
#include <stdlib.h>
#include <err.h>
#include "layer.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

struct neural_network *create_neural_network(int size_of_arr, int arr[size_of_arr])
{

     if (size_of_arr <= 2) {
          errx(EXIT_FAILURE, "Do you have a brain of that size ?");
     }
     struct neural_network *neural_network = malloc(sizeof(struct neural_network));

     neural_network->number_of_layers = size_of_arr - 1
             ;

     neural_network->inputs = malloc(sizeof(long double) * arr[0]);
     neural_network->input_size = arr[0];

     neural_network->output_size = arr[size_of_arr - 1];
     neural_network->outputs = malloc(sizeof(long double *));

     neural_network->layers = malloc(sizeof(struct layer *) * size_of_arr);

     neural_network->layers[0] = create_layer(arr[0], arr[1]);
     link_layer_input(neural_network->layers[0], neural_network->input_size, &neural_network->inputs);


     for(int i = 2; i < size_of_arr; i++)
     {
          printf("link layer i=%d\n", i);
          neural_network->layers[i-1] = create_layer(arr[i-1], arr[i]);

          link_layers(&neural_network->layers[i - 2], &neural_network->layers[i-1]);
          printf("link layer i: %d end \n", i);
     }
     printf("lli\n");

     printf("llo\n");
     link_layer_output(neural_network->layers[size_of_arr - 2], neural_network);
     printf("llo end \n");
     return neural_network;
}


void free_neural_network(struct neural_network *neural_network) {
     free_layers(neural_network->layers[neural_network->number_of_layers - 1]);
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
     return - log((*neural_network->outputs)[expected_output - 'A']);
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


void minimise_loss(const struct neural_network *neural_network, char expected_output, long double learning_rate, long double epsilon) {

     for (int i = 0; i < neural_network->number_of_layers; ++i) {
          struct layer *layer = neural_network->layers[i];


         printf("\t\tTrain Layer %d:\n", i);
         time_t tt = time(NULL);
          for (int j = 0; j < layer->layer_size; ++j) {

               //printf("\t\t\tTrain neuron :%d\n", j);
               time_t t = time(NULL);
               for (int k = 0; k < layer->prev_layer_size; ++k) {


                    long double original_weight = layer->weights[j][k];
                    neural_network_calculate_output(neural_network);
                    long double original_loss = categorical_cross_entropy(neural_network, expected_output);
                    // printf("Current loss:     %Lg, %Lg\n", current_loss, (*neural_network->outputs)[0]);

                    layer->weights[j][k] += epsilon;
                   neural_network_calculate_output(neural_network);
                   long double tweaked_loss = categorical_cross_entropy(neural_network, expected_output);

                   layer->weights[j][k] = original_weight - learning_rate * ( tweaked_loss + original_loss )/epsilon;


                    if (isnanf(categorical_cross_entropy(neural_network, expected_output))) {
                        errx(EXIT_FAILURE, "Loss is nan");
                    }
               }
               long double original_bias = layer->biases[j];
               neural_network_calculate_output(neural_network);
               long double original_loss = categorical_cross_entropy(neural_network, expected_output);

              layer->biases[j] += epsilon;
              neural_network_calculate_output(neural_network);
              long double tweaked_loss = categorical_cross_entropy(neural_network, expected_output);

              layer->biases[j] = original_bias - learning_rate * ( tweaked_loss + original_loss )/epsilon;

              //printf("\t\t\t%ld sec\n", time(NULL) - t);
          }
          printf("\t\t%ld\n", time(NULL) - tt);
     }
}

void export_neural_network(struct neural_network *neural_network)
{
    FILE *file = fopen("exported_neural_network.nn", "w");

    fprintf(file, "%d ", neural_network->input_size);

    for (int i = 0; i < neural_network->number_of_layers; i++)
    {
        fprintf(file, "%d ", neural_network->layers[i]->layer_size);
    }
    fprintf(file, "\n##########\n");

    for (int i = 0; i < neural_network->number_of_layers ; ++i)
    {
         fprintf(file, "Bias:\n");
         for (int j = 0; j < neural_network->layers[i]->layer_size; ++j) {
            fprintf(file, "\t%Lf", neural_network->layers[i]->biases[j]);
         }

        fprintf(file, "\nWeights:\n");
        for (int j = 0; j < neural_network->layers[i]->layer_size; ++j) {
            for (int k = 0; k < neural_network->layers[i]->prev_layer_size; ++k) {
                fprintf(file, "\t%Lf", neural_network->layers[i]->weights[j][k]);
            }
             fprintf(file, "\n");
        }
         fprintf(file, "\n");
    }

    fclose(file);
}