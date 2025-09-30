//
// Created by Evan on 30/09/2025.
//
#include "layer.h"
#include <stdlib.h>

struct layer
{
    /**
     * The layer_size of the previous layer (or input of the neural network).
     */
    int prev_layer_size;

    /**
     * Number of neuron on this layer.
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

/**
 * /!\ Use only when you know every value of each inputs of the layer
 * @param prev_layer_size Size of the previous layer (or input of the neural network).
 * @param layer_size Size of this layer.
 * @param inputs Array of prev_layer_size int (representing the output of the previous layer).
 * @param weights Array of layer_size * previous_layer_size int representing the weights of each inputs for each neuron of the layer.
 * @param biases Array of layer_size int representing the bias of each neuron of the layer.
 * @return the layer structure with good values
 */
struct layer create_layer(const int prev_layer_size, const int layer_size, int *inputs, int **weights, int *biases)
{
    struct layer res;

    res.prev_layer_size = prev_layer_size;
    res.layer_size = layer_size;
    res.inputs = inputs;
    res.weights = weights;
    res.biases = biases;

    return res;
}

/**
 *
 * @param layer The layer to calculate the output.
 * @return The output. \n /!\ DON'T FORGET TO FREE THE OUTPUT ONCE DONE
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