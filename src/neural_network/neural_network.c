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
     if (size_of_arr < 2) {
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

void softmax_cross_entropy_backward(struct neural_network *net,char expected,double *dZ)
{
    for (int i = 0; i < 26; i++) {
        double is_true = (expected == i + 'A');
        dZ[i] = (*(net->outputs))[i] - is_true;
        //printf("%f",dZ[i]);
    }
}


void linear_backward_output(struct neural_network* nn,double *dZ2, double **dW2, double *db2, double *dA1)
{
    for (int i = 0; i < 26; i++) 
    {
        db2[i] = dZ2[i];
        for (int j = 0; j < 128; j++) 
        {
            dW2[i][j] = dZ2[i] * nn->layers[0]->outputs[j];
        }
    }

    for (int j = 0; j < 128; j++) 
    {
        dA1[j] = 0.0;
        for (int i = 0; i < 26; i++) 
        {
            dA1[j] += nn->layers[1]->weights[i][j] * dZ2[i];
        }
    }
}

void relu_backward(double *dA,double *Z,double *dZ,int size)
{
    for (int i = 0; i < size; i++) 
    {
        if (Z[i] > 0)
            dZ[i] = dA[i];
        else
            dZ[i] = 0.0;
    }
}

void linear_backward_hidden(struct neural_network *nn,double *dZ1,double **dW1,double *db1,double *dA0)
{
    for (int i = 0; i < 128; i++) {
        db1[i] = dZ1[i];
        for (int j = 0; j < 784; j++) {
            dW1[i][j] = dZ1[i] * nn->layers[0]->inputs[0][j];
        }
    }

    for (int j = 0; j < 784; j++) {
        dA0[j] = 0.0;
        for (int i = 0; i < 128; i++) {
            dA0[j] += nn->layers[0]->weights[i][j] * dZ1[i];
        }
    }
}

void update_layer(
    double **W,
    double *b,
    double **dW,
    double *db,
    int out_size,
    int in_size,
    double lr
)
{
    for (int i = 0; i < out_size; i++) 
    {
        b[i] -= lr * db[i];
        for (int j = 0; j < in_size; j++) 
        {
            W[i][j] -= lr * dW[i][j];
        }
    }
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

int b_p()
{
    return 0;
}


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
