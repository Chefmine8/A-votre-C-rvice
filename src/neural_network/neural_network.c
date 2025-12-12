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

     neural_network->inputs = malloc(sizeof(float) * arr[0]);
     neural_network->input_size = arr[0];

     neural_network->output_size = arr[size_of_arr - 1];
     neural_network->outputs = malloc(sizeof(float *));

     neural_network->layers = malloc(sizeof(struct layer *) * size_of_arr);

     neural_network->layers[0] = create_layer(arr[0], arr[1]);
     link_layer_input(neural_network->layers[0], neural_network->input_size, &neural_network->inputs);

     for(int i = 2; i < size_of_arr; i++)
     {
          neural_network->layers[i-1] = create_layer(arr[i-1], arr[i]);

          link_layers(&neural_network->layers[i - 2], &neural_network->layers[i-1]);
     }

     link_layer_output(neural_network->layers[size_of_arr - 2], neural_network);
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
float calculate_loss(const struct neural_network *neural_network, char expected_output) {

    return -logf((*(neural_network->outputs))[expected_output - 'A'] + 1e-15f);
    /*float res = 0.0;
    for(int i = 0; i < neural_network->output_size; i++) {
        float y_true = (i == (expected_output - 'A')) ? 1.0 : 0.0;
        float y_pred = (*(neural_network->outputs))[i];
        res += (y_true - y_pred) * (y_true - y_pred);
    }

    return  1.0/((float)neural_network->output_size) * res;*/
}




char get_neural_network_output(const struct neural_network *neural_network) {
     float max = (*neural_network->outputs)[0];
    float max_index = 0;
     for (int i = 1; i < neural_network->output_size; ++i) {
          if (max < (*neural_network->outputs)[i]) {
               max = (*neural_network->outputs)[i];
               max_index = i;
          }
     }
    return 'A' + max_index;
}

int backprop_update_4(struct neural_network *neural_network, char expected_output, float learning_rate)
{
    float **dE_dz = m
    // https://doug919.github.io/notes-on-backpropagation-with-cross-entropy/
}

int backprop_update_3(struct neural_network *neural_network, char expected_output, float learning_rate)
{
    neural_network_calculate_output(neural_network);
    float **sum_h = malloc(sizeof(float*) * neural_network->number_of_layers);
    float **h = malloc(sizeof(float*) * neural_network->number_of_layers);

    for (int i = 0; i < neural_network->number_of_layers; i++) {
        sum_h[i] = calloc(neural_network->layers[i]->layer_size, sizeof(float));
        h[i] = calloc(neural_network->layers[i]->layer_size, sizeof(float));
        for (int j = 0; j < neural_network->layers[i]->layer_size; j++) {
            //printf("neural_network->layers[i]->weights[j]=%p\n*(neural_network->layers[i]->inputs)=%p\nneural_network->layers[i]->prev_layer_size=%d\nneural_network->layers[i]->biases[j]=%f\n\n", neural_network->layers[i]->weights[j], *(neural_network->layers[i]->inputs), neural_network->layers[i]->prev_layer_size, neural_network->layers[i]->biases[j]);
            sum_h[i][j] = dot_product(neural_network->layers[i]->weights[j], *(neural_network->layers[i]->inputs), neural_network->layers[i]->prev_layer_size) + neural_network->layers[i]->biases[j];
            //printf("dot_product = %f + biases=%f = sum_h[%d][%d]=%f\n", dot_product(neural_network->layers[i]->weights[j], *(neural_network->layers[i]->inputs), neural_network->layers[i]->prev_layer_size), neural_network->layers[i]->biases[j], i, j, sum_h[i][j]);
            //printf("sigmoid(%f) -> %f\n", sum_h[i][j], 1.0 / (1.0 + exp(-sum_h[i][j])));
            h[i][j] = 1.0 / (1.0 + expf(-sum_h[i][j]));
            //printf("sigmoid(%f) -> %f\n", sum_h[i][j], h[i][j]);
            //if(h[i][j] < -1e5 || h[i][j] > 1e5 || isnanf(h[i][j]) || isinff(h[i][j]) ) {
            //    printf("h[%d][%d] = %f\t dot_product(neural_network->layers[i]->weights[j]=%p, *(neural_network->layers[i]->inputs)=%p, neural_network->layers[i]->prev_layer_size=%d) + neural_network->layers[i]->biases[j]=%f\n", i, j, h[i][j], neural_network->layers[i]->weights[j], *(neural_network->layers[i]->inputs), neural_network->layers[i]->prev_layer_size, neural_network->layers[i]->biases[j]);
            //    return -1;
            //}
        }
    }

    float y_pred = (*(neural_network->outputs))[expected_output - 'A'];

    // printf("y_pred = %f -> %f\n", y_pred, -2.0/(1 - y_pred));
    float dL_dypred = -2.0/(float)(neural_network->output_size) * (1 - y_pred);


    float ***dypred_dw = malloc(sizeof(float**) * neural_network->number_of_layers);
    float **dypred_db = malloc(sizeof(float*) * neural_network->number_of_layers);

    float **dypred_dh = malloc(sizeof(float*) * neural_network->number_of_layers);
    for(int i = 0; i < neural_network->number_of_layers; i++) {
        dypred_dw[i] = malloc(sizeof(float*) * neural_network->layers[i]->layer_size);
        dypred_db[i] = malloc(sizeof(float) * neural_network->layers[i]->layer_size);
        dypred_dh[i] = malloc(sizeof(float) * neural_network->layers[i]->layer_size);
        for(int j = 0; j < neural_network->layers[i]->layer_size; j++) {
            dypred_dw[i][j] = malloc(sizeof(float) * neural_network->layers[i]->prev_layer_size);
            for(int k = 0; k < neural_network->layers[i]->prev_layer_size; k++) {
                dypred_dw[i][j][k] = (i == 0 ? neural_network->inputs[k] : h[i-1][k]) * deriv_sigmoid(sum_h[i][j]);
                if(dypred_dw[i][j][k] < -1e5 || dypred_dw[i][j][k] > 1e5 || isnanf(dypred_dw[i][j][k]) || isinff(dypred_dw[i][j][k]) ) {

                    printf("dypred_dw[%d][%d][%d] = %f\t (i == 0 =%d ? neural_network->inputs[k]=%f : h[i-1][k]=%f) * deriv_sigmoid(sum_h[i][j]%f)\n", i, j, k, dypred_dw[i][j][k], i == 0, neural_network->inputs[k], h[i-1][k], deriv_sigmoid(sum_h[i][j]));
                    return -1;
                }
            }
            dypred_db[i][j] = deriv_sigmoid(sum_h[i][j]);

            dypred_dh[i][j] = 1;
            if(j + 1 != neural_network->layers[i]->layer_size) {
                dypred_dh[i][j] = 0;
                for(int k = 0; k < neural_network->layers[i]->layer_size; k++) {
                    dypred_dh[i][j] += neural_network->layers[i]->weights[j + 1][k] * deriv_sigmoid(sum_h[i][j]);
                }
            }
        }
    }
    for(int i = 0; i < neural_network->number_of_layers; i++) {
        for(int j = 0; j < neural_network->layers[i]->layer_size; j++) {
            for(int k = 0; k < neural_network->layers[i]->prev_layer_size; k++) {
                /*printf("Updating weight[%d][%d] of layer %d: old value=%f \t newvalue = %f \t learning_rate=%f \t dL_dypred=%f \t dypred_dh[i][j]=%f \t dypred_dw[i][j][k]=%f\n", j, k, i,
                        neural_network->layers[i]->weights[j][k],
                         neural_network->layers[i]->weights[j][k] - learning_rate * dL_dypred * dypred_dh[i][j] * dypred_dw[i][j][k],
                         learning_rate, dL_dypred, dypred_dh[i][j], dypred_dw[i][j][k]);*/
                neural_network->layers[i]->weights[j][k] -= learning_rate * dL_dypred * dypred_dh[i][j] * dypred_dw[i][j][k];
                if(neural_network->layers[i]->weights[j][k] < -1e2 || neural_network->layers[i]->weights[j][k] > 1e2 || isnanf(neural_network->layers[i]->weights[j][k]) || isinff(neural_network->layers[i]->weights[j][k]) ) {
                    printf("Weight became nan : learning_rate=%f dL_dypred=%f dypred_dh=%f dypred_dw=%f\n", learning_rate, dL_dypred, dypred_dh[i][j], dypred_dw[i][j][k]);
                    return -1;
                }
            }
            neural_network->layers[i]->biases[j] -= learning_rate * dL_dypred * dypred_dh[i][j] * dypred_db[i][j];
            if(neural_network->layers[i]->biases[j] < -1e2 || neural_network->layers[i]->biases[j] > 1e2 || isnanf(neural_network->layers[i]->biases[j]) || isinff(neural_network->layers[i]->biases[j]) ) {
                printf("Bias became nan : learning_rate=%f dL_dypred=%f dypred_dh=%f dypred_db=%f\n", learning_rate, dL_dypred, dypred_dh[i][j], dypred_db[i][j]);
                return -1;
            }
        }
    }
    for (int i = 0; i < neural_network->number_of_layers; i++) {
        free(sum_h[i]);
        free(h[i]);
        for(int j = 0; j < neural_network->layers[i]->layer_size; j++) {
            free(dypred_dw[i][j]);
        }
        free(dypred_dw[i]);
        free(dypred_db[i]);
        free(dypred_dh[i]);
    }
    free(sum_h);
    free(h);
    free(dypred_dw);
    free(dypred_db);
    free(dypred_dh);
    return 0;
}

#include <stdlib.h>
#include <string.h>

/* Replace minimise_loss or add this new function and call it instead.
 * Assumes layer->outputs, layer->weights, layer->biases, layer->inputs, layer->prev_layer_size are valid.
 */

int backprop_update_2(struct neural_network *neural_network, char expected_output, float learning_rate)
{
    int L = neural_network->number_of_layers;
    if (L <= 0) return -1;

    /* forward pass */
    neural_network_calculate_output(neural_network);

    /* allocate deltas */
    float **deltas = malloc(sizeof(float*) * L);
    if (!deltas) return -1;
    for (int i = 0; i < L; ++i) {
        int sz = neural_network->layers[i]->layer_size;
        deltas[i] = calloc(sz, sizeof(float));
        if (!deltas[i]) {
            for (int k = 0; k < i; ++k) free(deltas[k]);
            free(deltas);
            return -1;
        }
    }

    /* output layer delta: use MSE gradient and sigmoid derivative */
    int out_idx = expected_output - 'A';
    struct layer *out_layer = neural_network->layers[L - 1];
    int out_size = out_layer->layer_size;
    float loss_factor = 2.0f / (float)out_size; /* dL/dy_pred factor for MSE */

    for (int i = 0; i < out_size; ++i) {
        float y_pred = out_layer->outputs[i];
        if (isnanf(y_pred)) {
            for (int k = 0; k < L; ++k) free(deltas[k]);
            free(deltas);
            printf("Output prediction became NaN: y_pred=%f\n", y_pred);
            return -1;
        }
        float y_true = (i == out_idx) ? 1.0f : 0.0f;
        float dL_dypred = loss_factor * (y_pred - y_true); /* dL/dy_pred */
        float sigmoid_deriv = y_pred * (1.0f - y_pred);     /* sigmoid'(z) using output */
        deltas[L - 1][i] = dL_dypred * sigmoid_deriv;
        if (isnanf(deltas[L - 1][i])) {
            for (int k = 0; k < L; ++k) free(deltas[k]);
            free(deltas);
            printf("Output delta became NaN: dL_dypred=%f sigmoid_deriv=%f\n", dL_dypred, sigmoid_deriv);
            return -1;
        }
    }

    /* backpropagate through hidden layers (all assumed sigmoid) */
    for (int li = L - 2; li >= 0; --li) {
        struct layer *cur = neural_network->layers[li];
        struct layer *next = neural_network->layers[li + 1];
        for (int i = 0; i < cur->layer_size; ++i) {
            float sum = 0.0f;
            for (int k = 0; k < next->layer_size; ++k) {
                sum += next->weights[k][i] * deltas[li + 1][k];
                if (isnanf(sum)) {
                    for (int t = 0; t < L; ++t) free(deltas[t]);
                    free(deltas);
                    printf("Sum became NaN during backprop\n");
                    return -1;
                }
            }
            float out = cur->outputs[i];
            float deriv = out * (1.0f - out); /* sigmoid'(z) */
            deltas[li][i] = sum * deriv;
            if (isnanf(deltas[li][i])) {
                for (int t = 0; t < L; ++t) free(deltas[t]);
                free(deltas);
                printf("Hidden delta became NaN: sum=%f deriv=%f\n", sum, deriv);
                return -1;
            }
        }
    }

    /* update weights and biases */
    for (int li = 0; li < L; ++li) {
        struct layer *layer = neural_network->layers[li];
        float *input_arr = *(layer->inputs); /* pointer to inputs for this layer */
        for (int i = 0; i < layer->layer_size; ++i) {
            float delta = deltas[li][i];
            /* update bias */
            layer->biases[i] -= learning_rate * delta;
            if (isnanf(layer->biases[i])) {
                for (int t = 0; t < L; ++t) free(deltas[t]);
                free(deltas);
                printf("Bias became NaN during update\n");
                return -1;
            }
            /* update weights */
            for (int j = 0; j < layer->prev_layer_size; ++j) {
                float grad = delta * input_arr[j];
                layer->weights[i][j] -= learning_rate * grad;
                if (isnanf(layer->weights[i][j])) {
                    for (int t = 0; t < L; ++t) free(deltas[t]);
                    free(deltas);
                    printf("Weight became NaN during update\n");
                    return -1;
                }
            }
        }
    }

    /* free deltas */
    for (int i = 0; i < L; ++i) free(deltas[i]);
    free(deltas);

    return 0;
}
/*
int backprop_update(struct neural_network *neural_network, char expected_output, float learning_rate)
{
    float epsilon = 1e-15;
     for (int i = 0; i < neural_network->number_of_layers; ++i)
     {
          struct layer *layer = neural_network->layers[i];


          for (int j = 0; j < layer->layer_size; ++j)
          {
               float original_bias = layer->biases[j];
               float original_loss = calculate_loss(neural_network, expected_output);
               float *tweaked_losses = malloc(sizeof(float) * layer->prev_layer_size);



               for (int k = 0; k < layer->prev_layer_size; ++k)
               {
                    layer->weights[j][k] += epsilon;
                    neural_network_calculate_output(neural_network);
                    tweaked_losses[k] = calculate_loss(neural_network, expected_output);
                    layer->weights[j][k] -= epsilon;

                    if (isnanf(tweaked_losses[k]))
                    {
			            free(tweaked_losses);
			            printf("Loss is nan\n");
                        return -1;
                    }
               }



              layer->biases[j] += epsilon;
              neural_network_calculate_output(neural_network);
              float tweaked_loss = calculate_loss(neural_network, expected_output);

              layer->biases[j] = (layer->biases[j] - epsilon) - learning_rate * ( tweaked_loss + (layer->biases[j] - epsilon) )/epsilon;

              for (int k = 0; k < layer->prev_layer_size; k++)
              {
                   layer->weights[j][k] = layer->weights[j][k] - learning_rate * ( tweaked_losses[k] + original_loss )/epsilon;
              }

              free(tweaked_losses);
          }
     }
     return 0;
}
*/
void export_neural_network(struct neural_network *neural_network, float learning_rate, int batch_size, int nb_sessions, int success)
{
    FILE *file = fopen("exported_neural_network.nn", "w");

    fprintf(file, "Learning Rate: %f\t| Batch Size: %d\t| Number of sessions : %d\t| Number of Success: %d\n##############################\n", learning_rate, batch_size, nb_sessions, success);

    fprintf(file, "%d ", neural_network->input_size);

    for (int i = 0; i < neural_network->number_of_layers; i++)
    {
        fprintf(file, "%d ", neural_network->layers[i]->layer_size);
    }
    fprintf(file, "\n##############################\n");

    for (int i = 0; i < neural_network->number_of_layers ; ++i)
    {
         fprintf(file, "Bias:\n");
         for (int j = 0; j < neural_network->layers[i]->layer_size; ++j)
         {
            fprintf(file, "\t%f", neural_network->layers[i]->biases[j]);
         }

        fprintf(file, "\nWeights:\n");
        for (int j = 0; j < neural_network->layers[i]->prev_layer_size; ++j)
        {
            for (int k = 0; k < neural_network->layers[i]->layer_size; ++k)
            {
                fprintf(file, "\t%f", neural_network->layers[i]->weights[k][j]);
            }
             fprintf(file, "\n");
        }
         fprintf(file, "\n");
    }

    fclose(file);
}
