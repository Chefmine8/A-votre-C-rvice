//
// Created by Evan on 30/09/2025.
#include "layer.h"
#include <stdlib.h>
#include <err.h>
#include <stdio.h>

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

    res->outputs = malloc(sizeof(long double) * layer_size);

    res->prev_layer_size = prev_layer_size;
    res->layer_size = layer_size;


    res->inputs = malloc(sizeof(long double *));

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
    printf("\nlayer nb: %d\n", (*front_layer)->layer_size);
    if ((*back_layer)->layer_size != (*front_layer)->prev_layer_size) {
        errx(EXIT_FAILURE, "Trying to link two incompatible layers ! back_layer->layer_size != front_layer->prev_layer_size");
    }
    printf("ll 1\n");
    free((*front_layer)->prev_layer);
    printf("ll 2\n");
    (*front_layer)->prev_layer = *back_layer;
    printf("ll 3\n");
    free(*(*front_layer)->inputs);
    printf("ll 4\n");
    (*front_layer)->inputs = (*back_layer)->outputs;
    printf("ll 5\n\n");
}

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param output_size the size of outputs (for security)
 * @param outputs the array of size output_size that will contain the output of the neural network
 */
void link_layer_output(struct layer *layer, int output_size, long double **outputs)
{
    printf("\nlayer nb: %d\n", layer->layer_size);
    if (output_size != layer->layer_size) {
        errx(EXIT_FAILURE, "layer not the same size as the outputs");
    }
    printf("llo 1\n");
    if(*layer->outputs != NULL){
        printf("llo 1.5\n");
        printf("%p\n", *layer->outputs);
        free((*layer->outputs));
        printf("llo 1.9\n");
    }
    printf("llo 2\n");
    *layer->outputs = *outputs;
    printf("llo 3\n\n");
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
    if(*layer->inputs != NULL)
        free(*(layer->inputs));
    *layer->inputs = *inputs;
}

/**
 *
 * @param layer The layer to calculate the output.
 * @return The output.
 */
void update_outputs(const struct layer *layer)
{

    for (int i = 0; i < layer->layer_size; i++)
    {
        int output = 0;
        for (int j = 0; j < layer->prev_layer_size; j++)
        {
            output += (*layer->inputs)[j] * layer->weights[i][j];
        }

        *layer->outputs[i] = output;
    }
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
    printf("-----------------------------\nlayer number: %d\n", layer->layer_size);
    if (layer->prev_layer != NULL) {
        printf("cl 1\n");
        printf("%p\n", layer->inputs);
        printf("%p\n", layer->prev_layer->outputs);
        printf("input connected: %d\n", (layer->inputs) == (layer->prev_layer->outputs));
        printf("cl 2\n");
    }
    else {
        printf("cl 1\n");
        printf("input is input: %d\n", *layer->inputs[0] == 69 );
        printf("cl 2\n");
    }
    printf("cl 3\n");
    printf("%p\n", *layer->outputs);
    printf("output connected (if not last layer 0 is normal tkt frère): %d\n", (*layer->outputs)[0] == 69  );
    printf("cl 4\n");

    if (layer->prev_layer != NULL) {
        printf("cl 5\n");
        check_layer(layer->prev_layer);
        printf("cl 6\n");
    }

}