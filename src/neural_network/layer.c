//
// Created by Evan on 30/09/2025.
//

#include "layer.h"

#include <stdlib.h>

struct layer
{
    /**
     * the layer_size of the previous layer (or input)
     */
    int prev_layer_size;

    /**
     * number of neuron on this layer
     */
    int layer_size;


    /**
     * inputs[prev_layer_size]
     */
    int *inputs;

    /**
     * weights[layer_size][prev_layer_size]
     */
    int **weights;


    /**
     * biases[layer_size]
     */
    int *biases;
};

struct layer create_layer(const int prev_layer_size, const int layer_size, int *input, int **weight, int *biases)
{
    struct layer res;

    res.prev_layer_size = prev_layer_size;
    res.layer_size = layer_size;
    res.inputs = input;
    res.weights = weight;
    res.biases = biases;

    return res;
}

/**
 *
 * @param layer the layer to calculate the output
 * @return the output /!\ DON'T FORGET TO FREE THE OUTPUT ONCE DONE
 */
int *calculate_outputs(const struct layer layer)
{
    int *res = malloc(sizeof(int) * layer.layer_size);

    for (int i = 0; i < layer.layer_size; i++)
    {
        int output = 0;
        for (int j = 0; j < layer.prev_layer_size; j++)
        {
            output += layer.inputs[j] * layer.weights[i][j];
        }

        res[i] = output;
    }

    return res;
}