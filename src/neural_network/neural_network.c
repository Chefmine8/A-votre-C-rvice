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

     neural_network->number_of_layers = size_of_arr - 1;

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
          // printf("%Lf\n", (*neural_network->outputs)[i]);
          if (max == (*neural_network->outputs)[i]) {
               return i + 'A';
          }
     }
     errx(EXIT_FAILURE, "wtf");
}


// language: c
// Add to `src/neural_network/neural_network.c`

#include <stdlib.h>
#include <string.h>

/* Replace minimise_loss or add this new function and call it instead.
 * Assumes layer->outputs, layer->weights, layer->biases, layer->inputs, layer->prev_layer_size are valid.
 */
void backprop_update(struct neural_network *neural_network, char expected_output, long double learning_rate)
{
    int L = neural_network->number_of_layers;
    if (L <= 0) return;

    // Forward pass: ensure outputs are computed
    neural_network_calculate_output(neural_network);

    // Allocate deltas per layer
    long double **deltas = malloc(sizeof(long double*) * L);
    for (int i = 0; i < L; ++i) {
        int sz = neural_network->layers[i]->layer_size;
        deltas[i] = calloc(sz, sizeof(long double));
    }

    // Output layer delta: softmax + cross-entropy => delta = y_pred - y_true
    int out_idx = expected_output - 'A';
    struct layer *out_layer = neural_network->layers[L - 1];
    for (int i = 0; i < out_layer->layer_size; ++i) {
        long double y_pred = out_layer->outputs[i];
        if(isnanf(y_pred)) {
            errx(EXIT_FAILURE, "Output prediction became nan : y_pred=%Lf\n", y_pred); // errx(EXIT_FAILURE,
        }
        long double y_true = (i == out_idx) ? 1.0L : 0.0L;
        deltas[L - 1][i] = y_pred - y_true;
        if(isnanf(deltas[L - 1][i])) {
            errx(EXIT_FAILURE, "Output delta became nan : y_pred=%Lf y_true=%Lf", y_pred, y_true);
        }
    }

    // Backpropagate deltas through hidden layers (from L-2 down to 0)
    for (int li = L - 2; li >= 0; --li) {
        struct layer *cur = neural_network->layers[li];
        struct layer *next = neural_network->layers[li + 1];
        for (int i = 0; i < cur->layer_size; ++i) {
            long double sum = 0.0L;
            // sum over next layer neurons: next.weights[k][i] * delta_next[k]
            for (int k = 0; k < next->layer_size; ++k) {
                sum += next->weights[k][i] * deltas[li + 1][k];
                if(isnanf(sum)) {
                    errx(EXIT_FAILURE, "Sum became nan: next->weights[%d][%d]=%Lf deltas[%d][%d]=%Lf", k, i, next->weights[k][i], li + 1, k, deltas[li + 1][k]);
                }
            }
            // ReLU derivative: output > 0 => 1 else 0
            long double deriv = cur->outputs[i] > 0.0L ? 1.0L : 0.0L;
            deltas[li][i] = sum * deriv;
            if(isnanf(deltas[li][i])) {
                errx(EXIT_FAILURE, "Delta became nan : sum=%Lf deriv=%Lf", sum, deriv);
            }
        }
    }

    // Update weights and biases using gradients: grad_w = delta_i * input_j
    for (int li = 0; li < L; ++li) {
        struct layer *layer = neural_network->layers[li];
        long double *input_arr = NULL;
        if (layer->prev_layer != NULL) {
            input_arr = layer->prev_layer->outputs;
        } else {
            // layer->inputs is long double** pointing to neural_network->inputs
            input_arr = *(layer->inputs);
        }

        for (int i = 0; i < layer->layer_size; ++i) {
            long double delta = deltas[li][i];
            // bias update
            layer->biases[i] -= learning_rate * delta;
            if (isnanf(layer->biases[i])) {
                errx(EXIT_FAILURE, "Bias became nan : learning_rate=%Lf delta=%Lf", learning_rate, delta);
            }
            // weight updates
            for (int j = 0; j < layer->prev_layer_size; ++j) {
                long double grad = delta * input_arr[j];
                layer->weights[i][j] -= learning_rate * grad;
                if (isnanf(layer->weights[i][j])) {
                    errx(EXIT_FAILURE, "Weight became nan : learning_rate=%Lf grad=%Lf", learning_rate, grad);
                }
            }
        }
    }

    // free deltas
    for (int i = 0; i < L; ++i) free(deltas[i]);
    free(deltas);
}

void minimise_loss(const struct neural_network *neural_network, char expected_output, long double learning_rate, long double epsilon)
{

     for (int i = 0; i < neural_network->number_of_layers; ++i)
     {
          struct layer *layer = neural_network->layers[i];


         printf("\t\tTrain Layer %d:\n", i);
         time_t tt = time(NULL);
          for (int j = 0; j < layer->layer_size; ++j)
          {
               long double original_bias = layer->biases[j];
               long double original_loss = categorical_cross_entropy(neural_network, expected_output);
               long double *tweaked_losses = malloc(sizeof(long double) * layer->prev_layer_size);



               for (int k = 0; k < layer->prev_layer_size; ++k)
               {
                    layer->weights[j][k] += epsilon;
                    neural_network_calculate_output(neural_network);
                    tweaked_losses[k] = categorical_cross_entropy(neural_network, expected_output);
                    layer->weights[j][k] -= epsilon;

                    if (isnanf(tweaked_losses[k]))
                    {
                        errx(EXIT_FAILURE, "Loss is nan");
                    }
               }



              layer->biases[j] += epsilon;
              neural_network_calculate_output(neural_network);
              long double tweaked_loss = categorical_cross_entropy(neural_network, expected_output);

              layer->biases[j] = (layer->biases[j] - epsilon) - learning_rate * ( tweaked_loss + (layer->biases[j] - epsilon) )/epsilon;

              for (int k = 0; k < layer->prev_layer_size; k++)
              {
                   layer->weights[j][k] = layer->weights[j][k] - learning_rate * ( tweaked_losses[k] + original_loss )/epsilon;
              }

              free(tweaked_losses);
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
         for (int j = 0; j < neural_network->layers[i]->layer_size; ++j)
         {
            fprintf(file, "\t%Lf", neural_network->layers[i]->biases[j]);
         }

        fprintf(file, "\nWeights:\n");
        for (int j = 0; j < neural_network->layers[i]->prev_layer_size; ++j)
        {
            for (int k = 0; k < neural_network->layers[i]->layer_size; ++k)
            {
                fprintf(file, "\t%Lf", neural_network->layers[i]->weights[j][k]);
            }
             fprintf(file, "\n");
        }
         fprintf(file, "\n");
    }

    fclose(file);
}