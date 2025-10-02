//
// Created by Evan on 30/09/2025.
#include "layer.h"
#include <stdlib.h>
#include <err.h>


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
    res->outputs = NULL;

    res->prev_layer_size = prev_layer_size;
    res->layer_size = layer_size;

    res->inputs = malloc(prev_layer_size * sizeof(long double));

    res->weights = malloc(prev_layer_size * layer_size * sizeof(long double));

    for (int i = 0; i < layer_size; i++)
    {
        for (int j = 0; j < prev_layer_size; j++)
        {
            res->weights[i][j] = (long double)rand() / (long double)RAND_MAX * 10 - 5;
        }
    }

    res->biases = malloc(layer_size * sizeof(int));

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
void link_layers(struct layer *back_layer, struct layer *front_layer)
{
    if (back_layer->layer_size != front_layer->prev_layer_size) {
        errx(EXIT_FAILURE, "Trying to link two incompatible layers ! back_layer->layer_size != front_layer->prev_layer_size");
    }

    front_layer->prev_layer = back_layer;
    free(*back_layer->outputs);
    back_layer->outputs = front_layer->inputs;
}

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param output_size the size of outputs (for security)
 * @param outputs the array of size output_size that will contain the output of the neural network
 */
void link_layer_output(struct layer *layer, int output_size, long double **outputs)
{
    if (output_size != layer->layer_size) {
        errx(EXIT_FAILURE, "layer not the same size as the outputs");
    }
    free(*layer->outputs);
    layer->outputs = outputs;
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
    free(*layer->inputs);
    layer->inputs = inputs;
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

    if (layer->prev_layer != NULL) {
        free_layers(layer->prev_layer);
    }

    free(layer->inputs);
    free(layer->outputs);
    free(layer->prev_layer);
    free(layer->biases);
    free(layer);
}