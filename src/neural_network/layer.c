//
// Created by Evan on 30/09/2025.
#include "layer.h"
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include "neural_network.h"

/**
 *
 * @param prev_layer_size size of the previous layer
 * @param layer_size size of this layer
 * @return the layer struct
 */
struct layer *create_layer(const int prev_layer_size, const int layer_size)
{
    struct layer *res = malloc(sizeof(struct layer));


    res->prev_layer = NULL;

    res->outputs = malloc(sizeof(long double * ));

    res->prev_layer_size = prev_layer_size;
    res->layer_size = layer_size;

    res->inputs = malloc(prev_layer_size * sizeof(long double));

    res->weights = malloc(layer_size * sizeof(long double *));

    for (int i = 0; i < layer_size; i++)
    {
        res->weights[i] = malloc(prev_layer_size * sizeof(long double));
        for (int j = 0; j < prev_layer_size; j++)
        {
            res->weights[i][j] = (long double)rand() / (long double)RAND_MAX * 10 - 5;
        }
    }

    res->biases = malloc(layer_size * sizeof(long double));

    for (int i = 0; i < layer_size; i++)
    {
        res->biases[i] =  (long double)rand() / (long double)RAND_MAX * 10 - 5;
    }

    return res;
}

/**
 * Link two internals layers together
 * @param back_layer the layer closest to the input
 * @param front_layer the layer closest to the output
 */
void link_layers(struct layer **back_layer, struct layer **front_layer)
{
    if ((*back_layer)->layer_size != (*front_layer)->prev_layer_size) {
        errx(EXIT_FAILURE, "Trying to link two incompatible layers ! back_layer->layer_size != front_layer->prev_layer_size");
    }
    free((*front_layer)->prev_layer);
    (*front_layer)->prev_layer = *back_layer;
    *(*front_layer)->inputs = (*back_layer)->outputs;
}

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param output_size the size of outputs (for security)
 * @param outputs the array of size output_size that will contain the output of the neural network
 */
void link_layer_output(struct layer *layer, struct neural_network *neural_network)
{
    if (neural_network->output_size != layer->layer_size) {
        errx(EXIT_FAILURE, "layer not the same size as the outputs");
    }


    if(layer->outputs != NULL){

        free(layer->outputs);

    }

    *neural_network->outputs = layer->outputs;

}

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param input_size the size of inputs (for security)
 * @param inputs the array of size input_size that will contain the input of the neural network
 */
void link_layer_input(struct layer *layer, int input_size, long double **inputs)
{
    if (input_size != layer->layer_size) {
        errx(EXIT_FAILURE, "layer not the same size as the inputs");
    }

    free(*(layer->inputs));
    layer->inputs = inputs;
}

/**
 *
 * @param layer The layer to calculate the output.
 * @return The output.
 */
long double activation_function(long double x) {
    return x > 0 ? x : 0;
}

void update_outputs(const struct layer *layer)
{
    if (layer->prev_layer != NULL) {
        update_outputs(layer->prev_layer);
    }
    printf("\n ---------- layer nb : %d ---------- \n", layer->layer_size);
    for (int i = 0; i < layer->layer_size; i++)
    {
        printf("    i = %d\n", i);
        int output = 0;
        // printf("    uo 1\n");
        for (int j = 0; j < layer->prev_layer_size; j++)
        {
            printf("        j = %d\n", j);
            // printf("        %ld\n", layer->weights[i][j] );
            // printf("        %ld\n", (*layer->inputs)[j] );

            output += (*layer->inputs)[j] * layer->weights[i][j];
            // printf("        uo 2\n");
        }
        // printf("    uo 3\n");
        layer->outputs[i] = activation_function( output );
        // printf("    uo 4\n");
    }
    // printf("uo 5\n");
}

void free_layers(struct layer *layer) {
    if((layer->inputs) != NULL)
        free(*(layer->inputs));
    free((layer->biases));
    if (layer->prev_layer != NULL) {
        free_layers(layer->prev_layer);
    }
    free(layer);
}


void check_layer(const struct layer *layer) {
    printf("\n ---------- layer nb: %d ----------\n", layer->layer_size);

    if (layer->prev_layer != NULL) {
        printf("input connected: %d\n",  *(layer->inputs) == (layer->prev_layer->outputs) );
    }
    else {
        printf("input is input: %d\n", *layer->inputs[0] == 69 );
    }
    printf("last layer output connected: %d\n", (layer->outputs)[0] == 69  );

    if (layer->prev_layer != NULL) {
        check_layer(layer->prev_layer);

    }

}